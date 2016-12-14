#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <yarp/os/LogStream.h>
#include <rFSM.h>

#ifndef LUA_OK
    #define LUA_OK      0
#endif

#ifndef yError
    #include <iostream>
    #include <assert.h>
    #define yError()    std::cerr<<"[ERROR]"
    #define yWarning()      std::cout<<"[WARNING]"
    #define yInfo()     std::cout<<"[INFO]"
    #define yAssert(x)  assert(x)
    #define ENDL        std::endl
#else
    #define ENDL        ""
#endif


using namespace std;
using namespace rfsm;


#define EVENT_RETREIVE_CHUNK \
"function get_all_events()\n"\
"    local known_events = { e_init_fsm=true, }\n"\
"    rfsm.mapfsm(function(t)\n"\
"           local events = t.events or {}\n"\
"           for _,e in ipairs(events) do\n"\
"              known_events[e] = true\n"\
"           end\n"\
"            end, fsm, rfsm.is_trans)\n"\
"    local a = {}\n"\
"    for k, v in pairs(known_events) do\n"\
"       if string.find(k, 'e_done@') == nil then table.insert(a, k) end\n"\
"    end\n"\
"    table.sort(a)\n"\
"    return a\n"\
"end"

#define SET_STATE_CALLBACKS_CHUNK \
"function set_state_callbacks(name)\n"\
"    local found = false\n"\
"    local function proc_node(state)\n"\
"       if state._fqn == ('root.' .. name) then\n"\
"           state.entry = function() RFSM.entryCallback(name) end\n"\
"           state.doo = function() RFSM.dooCallback(name) end\n"\
"           state.exit = function() RFSM.exitCallback(name) end\n"\
"           found = true\n"\
"        end\n"\
"    end\n"\
"    rfsm.mapfsm(function (s)\n"\
"          if rfsm.is_root(s) then return end\n"\
"          if found == true then return end\n"\
"          proc_node(s)\n"\
"          end, fsm, rfsm.is_node)\n"\
"    return found\n"\
"end"



static int report (lua_State *L, int status) {
  if (status && !lua_isnil(L, -1)) {
    const char *msg = lua_tostring(L, -1);
    if (msg == NULL)
        msg = "(error object is not a string)";
    else
        yError()<<msg<<ENDL;
    lua_pop(L, 1);
  }
  return status;
}

static int traceback (lua_State *L) {
#if LUA_VERSION_NUM > 501
  const char *msg = lua_tostring(L, 1);
  if (msg)
    luaL_traceback(L, L, msg, 1);
  else if (!lua_isnoneornil(L, 1)) {  /* is there an error object? */
    if (!luaL_callmeta(L, 1, "__tostring"))  /* try its 'tostring' metamethod */
      lua_pushliteral(L, "(no error message)");
  }
  return 1;
#else
    lua_getfield(L, LUA_GLOBALSINDEX, "debug");
    if (!lua_istable(L, -1)) {
      lua_pop(L, 1);
      return 1;
    }
    lua_getfield(L, -1, "traceback");
    if (!lua_isfunction(L, -1)) {
      lua_pop(L, 2);
      return 1;
    }
    lua_pushvalue(L, 1);  /* pass error message */
    lua_pushinteger(L, 2);  /* skip this function and traceback */
    lua_call(L, 2, 1);  /* call debug.traceback */
    return 1;
#endif
}

static int docall(lua_State *L, int narg, int clear) {
  int status;
  int base = lua_gettop(L) - narg;  /* function index */
  lua_pushcfunction(L, traceback);  /* push traceback function */
  lua_insert(L, base);  /* put it under chunk and args */
  status = lua_pcall(L, narg, (clear ? 0 : LUA_MULTRET), base);
  lua_remove(L, base);  /* remove traceback function */
  /* force a complete garbage collection in case of errors */
  if (status != 0) lua_gc(L, LUA_GCCOLLECT, 0);
  return status;
}


static int dofile(lua_State *L, const char *name) {
  int status = luaL_loadfile(L, name) || docall(L, 0, 1);
  return report(L, status);
}


static int dostring (lua_State *L, const char *s, const char *name) {
  int status = luaL_loadbuffer(L, s, strlen(s), name) || docall(L, 0, 1);
  return report(L, status);
}


static int dolibrary (lua_State *L, const char *name) {
  lua_getglobal(L, "require");
  lua_pushstring(L, name);
  return report(L, lua_pcall(L, 1, 0, 0));
}


StateMachine::StateMachine(bool verbose) : L(NULL) {
    StateMachine::verbose = verbose;
}

StateMachine::~StateMachine() {
    close();
}

void StateMachine::close() {
    if(L){
        lua_close(L);
        L = NULL;
    }
}

const std::string StateMachine::getFileName() {
    return fileName;
}

bool StateMachine::load(const std::string& filename) {

    close();

    StateMachine::fileName = filename;
    // initiate lua state
    L = luaL_newstate();
    if(L==NULL) {
        yError()<<"Cannot initialize lua! (luaL_newstate)"<<ENDL;
        return false;
    }

    luaL_openlibs(L);

    // setting user-defined lua package paths
    if(luaPackagePath.size()) {
        string command = "package.path=package.path .. '" + luaPackagePath + "'";
        if(dostring(L, command.c_str(), "command") == !LUA_OK)
            yWarning()<<"Could not set lua package path from"<<luaPackagePath<<ENDL;
    }

    if (dolibrary(L, "rfsm") != LUA_OK) {
        close();
        return false;
    }

    // registering some utility fuctions in lua
    lua_pushlightuserdata(L, this);
    lua_setglobal(L, "RFSM_Owner");
    if(dostring(L, EVENT_RETREIVE_CHUNK, "EVENT_RETREIVE_CHUNK") != LUA_OK)
        return false;
    if(dostring(L, SET_STATE_CALLBACKS_CHUNK, "SET_STATE_CALLBACKS_CHUNK") != LUA_OK)
        return false;

    registerLuaFunction("entryCallback", StateMachine::entryCallback);
    registerLuaFunction("dooCallback", StateMachine::dooCallback);
    registerLuaFunction("exitCallback", StateMachine::exitCallback);

    string cmd = "fsm_model = rfsm.load('"+filename+"')";
    if(dostring(L, cmd.c_str(), "fsm_model") != LUA_OK) {
        close();
        return false;
    }

    if(!verbose) {
        doString("function rfsm_null_func() return end");
        doString("fsm_model.warn = rfsm_null_func");
        doString("fsm_model.info = rfsm_null_func");
        //doString("fsm_model.err = rfsm_null_func");
    }

    if(dostring(L, "fsm = rfsm.init(fsm_model)", "fsm") != LUA_OK) {
        close();
        return false;
    }

    // getting all availabe events
    if(!getAllEvents())
        yWarning()<<"Cannot retrieve all events"<<ENDL;

    return true;
}


bool StateMachine::run() {
    return (dostring(L, "rfsm.run(fsm)", "run") == LUA_OK);
}

bool StateMachine::step(unsigned int n) {
    char command[128];
    snprintf(command, 128, "rfsm.step(fsm, %d)", n);
    return (dostring(L, command, "step") == LUA_OK);
}

bool StateMachine::sendEvent(const std::string& event) {
    if(std::find(events.begin(), events.end(), event) == events.end())
        yWarning()<<"Sending the undefined event"<<event<<ENDL;
    string command = "rfsm.send_events(fsm, '"+event+"')";
    return (dostring(L, command.c_str(), "sendEvent") == LUA_OK);
}

bool StateMachine::sendEvents(unsigned int n, ...) {
    register unsigned int i;
    va_list ap;
    va_start(ap, n);
    const char* event = va_arg(ap, char*);
    yAssert(event != NULL);
    string command = string("rfsm.send_events(fsm, '") + event + "'";
    for(i=2; i<= n; i++) {
        event = va_arg(ap, const char*);
        yAssert(event != NULL);
        if(std::find(events.begin(), events.end(), event) == events.end())
            yWarning()<<"Sending the undefined event"<<event<<ENDL;
        command += string(", '") + event + string("'");
    }
    va_end(ap);
    command += ")";
    return (dostring(L, command.c_str(), "sendEvents") == LUA_OK);
}

bool StateMachine::getAllEvents() {
    events.clear();
    if(dostring(L, "events = get_all_events()", "EVENT_RETREIVE_CHUNK") != LUA_OK)
        return false;
    lua_getglobal(L, "events");
    if(!lua_istable(L, -1)) {
        yError()<<"got the wrong value from get_all_events()"<<ENDL;
        return false;
    }
    lua_pushnil(L);
    while(lua_next(L, -2) != 0) {
        if(lua_isstring(L, -1))
            events.push_back(lua_tostring(L, -1));
        else
            yWarning()<<"found a wrong type in the result from get_all_events()"<<ENDL;
       lua_pop(L, 1);
    }
    return true;
}

const std::vector<std::string>& StateMachine::getEventsList() {
    return events;
}

bool StateMachine::doString(const std::string& command) {
    return (dostring(L, command.c_str(), "command") == LUA_OK);
}

bool StateMachine::doFile(const std::string& filename) {
    return (dofile(L, filename.c_str()) == LUA_OK);
}

void StateMachine::addLuaPackagePath(const std::string& path) {
    luaPackagePath += string(";")+path;
}

bool StateMachine::registerLuaFunction(const std::string& name, lua_CFunction func) {
    luaL_reg reg;
    if(luaFuncReg.size()) {
        luaFuncReg.back().name = name.c_str();
        luaFuncReg.back().func = func;
    }
    else {
        reg.name = name.c_str();
        reg.func = func;
        luaFuncReg.push_back(reg);
    }
    reg.name = 0;
    reg.func = 0;
    luaFuncReg.push_back(reg);
#if LUA_VERSION_NUM > 501
    lua_newtable(L);
    luaL_setfuncs (L, &luaFuncReg[0], 0);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "RFSM");
#else
    luaL_register(L, "RFSM", &luaFuncReg[0]);
#endif
}

int StateMachine::entryCallback(lua_State* L) {
    if (lua_gettop(L) < 1) {
        yError()<<"StateMachine::entryCallback() expects exactly one argument"<<ENDL;
       return 0;
    }
    const char *cst = luaL_checkstring(L, -1);
    if(cst) {
        lua_getglobal(L, "RFSM_Owner");
        if(!lua_islightuserdata(L, -1)) {
            lua_pop(L, 1);
            yError()<<"StateMachine::entryCallback() cannot access RFSM_Owner"<<ENDL;
            return 0;
        }
        StateMachine* owner = static_cast<StateMachine*>(lua_touserdata(L, -1));
        lua_pop(L, 1);
        yAssert(owner!=NULL);
        owner->callEntryCallback(cst);
    }
    else
        yError()<<"StateMachine::entryCallback() expects a string argument"<<ENDL;
    return 0;
}

int StateMachine::dooCallback(lua_State* L){
    if (lua_gettop(L) < 1) {
        yError()<<"StateMachine::dooCallback() expects exactly one argument"<<ENDL;
       return 0;
    }
    const char *cst = luaL_checkstring(L, -1);
    if(cst) {
        lua_getglobal(L, "RFSM_Owner");
        if(!lua_islightuserdata(L, -1)) {
            lua_pop(L, 1);
            yError()<<"StateMachine::dooCallback() cannot access RFSM_Owner"<<ENDL;
            return 0;
        }
        StateMachine* owner = static_cast<StateMachine*>(lua_touserdata(L, -1));
        lua_pop(L, 1);
        yAssert(owner!=NULL);
        owner->callDooCallback(cst);
    }
    else
        yError()<<"StateMachine::dooCallback() expects a string argument"<<ENDL;
    return 0;
}

int StateMachine::exitCallback(lua_State* L){
    if (lua_gettop(L) < 1) {
        yError()<<"StateMachine::exitCallback() expects exactly one argument"<<ENDL;
       return 0;
    }
    const char *cst = luaL_checkstring(L, -1);
    if(cst) {
        lua_getglobal(L, "RFSM_Owner");
        if(!lua_islightuserdata(L, -1)) {
            lua_pop(L, 1);
            yError()<<"StateMachine::exitCallback() cannot access RFSM_Owner"<<ENDL;
            return 0;
        }
        StateMachine* owner = static_cast<StateMachine*>(lua_touserdata(L, -1));
        lua_pop(L, 1);
        yAssert(owner!=NULL);
        owner->callExitCallback(cst);
    }
    else
        yError()<<"StateMachine::exitCallback() expects a string argument"<<ENDL;
    return 0;
}

void StateMachine::callEntryCallback(const std::string& state) {
    std::map<string,StateCallback*>::iterator it;
    if ((it = callbacks.find(state)) == callbacks.end())
        return;
    it->second->entry();
}

void StateMachine::callDooCallback(const std::string& state) {
    std::map<string,StateCallback*>::iterator it;
    if ((it = callbacks.find(state)) == callbacks.end())
        return;
    it->second->doo();
}

void StateMachine::callExitCallback(const std::string& state) {
    std::map<string,StateCallback*>::iterator it;
    if ((it = callbacks.find(state)) == callbacks.end())
        return;
    it->second->exit();
}

bool StateMachine::setStateCallback(const std::string state, rfsm::StateCallback& callback) {       
    lua_getglobal(L, "set_state_callbacks");
    if(!lua_isfunction(L, -1)) {
        yError()<<"StateMachine::setStateCallback() could not find set_state_callbacks()"<<ENDL;
        return false;
    }

    lua_pushstring(L, state.c_str());
    if(lua_pcall(L, 1, 1, 0) != 0) {
        yError()<<"StateMachine::setStateCallback()"<<lua_tostring(L, -1)<<ENDL;
        lua_pop(L, 1);
        return false;
    }

    // converting the results
    bool result = (bool) lua_toboolean(L, -1);
    lua_pop(L, 1); // pop the result from Lua stack
    lua_pop(L, 1);
    if(result)
        callbacks[state] = &callback;
    else
        yWarning()<<"State"<<state<<"does not exist"<<ENDL;
    return result;
}
