#include<stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <yarp/os/LogStream.h>
#include <rFSM.h>

#ifndef LUA_OK
    #define LUA_OK      0
#endif

using namespace std;
using namespace yarp::os;


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


static int report (lua_State *L, int status) {
  if (status && !lua_isnil(L, -1)) {
    const char *msg = lua_tostring(L, -1);
    if (msg == NULL)
        msg = "(error object is not a string)";
    else
        yError()<<msg;
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
  //signal(SIGINT, laction);
  status = lua_pcall(L, narg, (clear ? 0 : LUA_MULTRET), base);
  //signal(SIGINT, SIG_DFL);
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

int onEntry(lua_State* L) {
    yDebug()<<"onEntry";
    return 0;
}


RFSM::RFSM() : L(NULL) {

}

RFSM::~RFSM() {
    close();
}

void RFSM::close() {
    if(L){
        lua_close(L);
        L = NULL;
    }
}

const std::string RFSM::getFileName() {
    return fileName;
}

bool RFSM::load(const std::string& filename) {

    close();

    RFSM::fileName = filename;
    // initiate lua state
    L = luaL_newstate();
    if(L==NULL) {
        yError()<<"Cannot initialize lua! (luaL_newstate)";
        return false;
    }

    luaL_openlibs(L);

    if (dolibrary(L, "rfsm") != LUA_OK) {
        close();
        return false;
    }

    string cmd = "fsm_model = rfsm.load('"+filename+"')";
    if(dostring(L, cmd.c_str(), "fsm_model") != LUA_OK) {
        close();
        return false;
    }

    if(dostring(L, "fsm = rfsm.init(fsm_model)", "fsm") != LUA_OK) {
        close();
        return false;
    }

    // registering some utility fuctions in lua
    if(dostring(L, EVENT_RETREIVE_CHUNK, "EVENT_RETREIVE_CHUNK") != LUA_OK)
        return false;

    // getting all availabe events
    if(!getAllEvents())
        yWarning()<<"Cannot retrieve all events";

    //doString("function __null_func() return end");
    //doString("fsm.warn = __null_func");
    //doString("fsm.err = __null_func");

    registerLuaFunction("onEntry", onEntry);
    return true;
}


bool RFSM::run() {
    return (dostring(L, "rfsm.run(fsm)", "run") == LUA_OK);
}

bool RFSM::step(unsigned int n) {
    char command[128];
    snprintf(command, 128, "rfsm.step(fsm, %d)", n);
    return (dostring(L, command, "step") == LUA_OK);
}

bool RFSM::sendEvent(const std::string& event) {
    string command = "rfsm.send_events(fsm, '"+event+"')";
    return (dostring(L, command.c_str(), "sendEvent") == LUA_OK);
}

bool RFSM::sendEvents(unsigned int n, ...) {
    register unsigned int i;
    va_list ap;
    va_start(ap, n);
    const char* event = va_arg(ap, char*);
    yAssert(event != NULL);
    string command = string("rfsm.send_events(fsm, '") + event + "'";
    for(i=2; i<= n; i++) {
        event = va_arg(ap, const char*);
        yAssert(event != NULL);
        command += string(", '") + event + string("'");
    }
    va_end(ap);
    command += ")";
    return (dostring(L, command.c_str(), "sendEvents") == LUA_OK);
}

bool RFSM::getAllEvents() {
    events.clear();
    if(dostring(L, "events = get_all_events()", "EVENT_RETREIVE_CHUNK") != LUA_OK)
        return false;
    lua_getglobal(L, "events");
    if(!lua_istable(L, -1)) {
        yError()<<"got the wrong value from get_all_events()";
        return false;
    }
    lua_pushnil(L);
    while(lua_next(L, -2) != 0) {
        if(lua_isstring(L, -1))
            events.push_back(lua_tostring(L, -1));
        else
            yWarning()<<"find a wrong type in the result from get_all_events()";
       lua_pop(L, 1);
    }
    return true;
}

const std::vector<std::string>& RFSM::getEventsList() {
    return events;
}

bool RFSM::doString(const std::string& command) {
    return (dostring(L, command.c_str(), "command") == LUA_OK);
}

bool RFSM::doFile(const std::string& filename) {
    return (dofile(L, filename.c_str()) == LUA_OK);
}

bool RFSM::registerLuaFunction(const std::string& name, lua_CFunction func) {
    static struct luaL_reg luaReg[2];
    luaReg[1].name = name.c_str();
    luaReg[1].func = func;
    luaReg[2].name = 0;
    luaReg[2].func = 0;

#if LUA_VERSION_NUM > 501
    lua_newtable(L);
    luaL_setfuncs (L, luaReg, 0);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "RFSM");
#else
    luaL_register(L, "RFSM", luaReg);
#endif
}
