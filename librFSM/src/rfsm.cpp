#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <sstream>
#include <rfsmUtils.h>
#include <rfsm.h>

using namespace std;
using namespace rfsm;

#define CHECK_RFSM_LOADED(L) if(!L) { yError()<<"StateMachine has not been initialized. call StateMachine::load()"<<ENDL; return false; }

#ifdef WITH_EMBEDDED_RFSM
extern "C" const char gen_rfsm_res[];
extern "C" const char gen_rfsm_utils_res[];
#endif

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
        if(Utils::dostring(L, command.c_str(), "command") == !LUA_OK)
            yWarning()<<"Could not set lua package path from"<<luaPackagePath<<ENDL;
    }

    // loading rfsm package
#ifdef WITH_EMBEDDED_RFSM
    if(Utils::dostring(L, gen_rfsm_utils_res, "gen_rfsm_utils_res") != LUA_OK)
        return false;
    if(Utils::dostring(L, gen_rfsm_res, "gen_rfsm_res") != LUA_OK)
        return false;
#else
    if (Utils::dolibrary(L, "rfsm") != LUA_OK) {
        close();
        return false;
    }
#endif

    // registering some utility fuctions in lua
    lua_pushlightuserdata(L, this);
    lua_setglobal(L, "RFSM_Owner");
    if(!registerAuxiliaryFunctions())
        return false;

    // loading rfsm state machine
    string cmd = "fsm_model = rfsm.load('"+filename+"')";
    if(Utils::dostring(L, cmd.c_str(), "fsm_model") != LUA_OK) {
        close();
        return false;
    }

    // setting verbosity mode
    if(!verbose) {
        //doString("function rfsm_null_func() return end");
        doString("fsm_model.warn = rfsm_null_func");
        doString("fsm_model.info = rfsm_null_func");
        //doString("fsm_model.err = rfsm_null_func");
    }

    // initializing rfsm state machine
    if(Utils::dostring(L, "fsm = rfsm.init(fsm_model)", "fsm") != LUA_OK) {
        close();
        return false;
    }

    // getting all availabe events and state graph
    if(!getAllEvents())
        yWarning()<<"Cannot retrieve all events"<<ENDL;
    if(!getAllStateGraph())
        yWarning()<<"Cannot retrieve state graph"<<ENDL;

    return true;
}

bool StateMachine::registerAuxiliaryFunctions() {
    registerCFunction("entryCallback", StateMachine::entryCallback);
    registerCFunction("dooCallback", StateMachine::dooCallback);
    registerCFunction("exitCallback", StateMachine::exitCallback);
    registerCFunction("preStepCallback", StateMachine::preStepCallback);
    registerCFunction("postStepCallback", StateMachine::postStepCallback);
    if(Utils::dostring(L, RFSM_NULL_FUNCTION_CHUNK, "RFSM_NULL_FUNCTION_CHANK") != LUA_OK)
        return false;
    if(Utils::dostring(L, EVENT_RETREIVE_CHUNK, "EVENT_RETREIVE_CHUNK") != LUA_OK)
        return false;
    if(Utils::dostring(L, SET_STATE_CALLBACKS_CHUNK, "SET_STATE_CALLBACKS_CHUNK") != LUA_OK)
        return false;
    if(Utils::dostring(L, GET_CURRENT_STATE_CHUNK, "GET_CURRENT_STATE_CHUNK") != LUA_OK)
        return false;
    if(Utils::dostring(L, GET_ALL_STATES_CHUNK, "GET_ALL_STATES_CHUNK") != LUA_OK)
        return false;
    if(Utils::dostring(L, GET_ALL_TRANSITIONS_CHUNK, "GET_ALL_TRANSITIONS_CHUNK") != LUA_OK)
        return false;
    if(Utils::dostring(L, GET_EVET_QUEUE_CHUNK, "GET_EVET_QUEUE_CHUNK") != LUA_OK)
        return false;
    if(Utils::dostring(L, PRE_STEP_HOOK_CHUNK, "PRE_STEP_HOOK_CHUNK") != LUA_OK)
        return false;
    if(Utils::dostring(L, POST_STEP_HOOK_CHUNK, "POST_STEP_HOOK_CHUNK") != LUA_OK)
        return false;
    return true;
}

bool StateMachine::run() {
    CHECK_RFSM_LOADED(L);
    return (Utils::dostring(L, "rfsm.run(fsm)", "run") == LUA_OK);
}

bool StateMachine::step(unsigned int n) {
    CHECK_RFSM_LOADED(L);
    char command[128];
    snprintf(command, 128, "rfsm.step(fsm, %d)", n);
    return (Utils::dostring(L, command, "step") == LUA_OK);
}

bool StateMachine::sendEvent(const std::string& event) {
    CHECK_RFSM_LOADED(L);
    if(std::find(events.begin(), events.end(), event) == events.end())
        yWarning()<<"Sending the undefined event"<<event<<ENDL;
    string command = "rfsm.send_events(fsm, '"+event+"')";
    return (Utils::dostring(L, command.c_str(), "sendEvent") == LUA_OK);
}

bool StateMachine::sendEvents(unsigned int n, ...) {
    CHECK_RFSM_LOADED(L);
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
    return (Utils::dostring(L, command.c_str(), "sendEvents") == LUA_OK);
}

bool StateMachine::getAllEvents() {
    CHECK_RFSM_LOADED(L);
    events.clear();
    if(Utils::dostring(L, "events = rfsm_get_all_events()", "EVENT_RETREIVE_CHUNK") != LUA_OK)
        return false;
    lua_getglobal(L, "events");
    if(!lua_istable(L, -1)) {
        yError()<<"got the wrong value from rfsm_get_all_events()"<<ENDL;
        return false;
    }
    lua_pushnil(L);
    while(lua_next(L, -2) != 0) {
        if(lua_isstring(L, -1))
            events.push_back(lua_tostring(L, -1));
        else
            yWarning()<<"found a wrong type in the result from rfsm_get_all_events()"<<ENDL;
       lua_pop(L, 1);
    }
    return true;
}

const std::vector<std::string>& StateMachine::getEventsList() {
    return events;
}

bool StateMachine::getEventQueue(std::vector<std::string>& equeue) {
    CHECK_RFSM_LOADED(L);
    equeue.clear();
    lua_getglobal(L, "rfsm_get_event_queue");
    if(!lua_isfunction(L, -1)) {
        yError()<<"StateMachine::getEventQueue() could not find rfsm_get_event_queue()"<<ENDL;
        return false;
    }

    if(lua_pcall(L, 0, 1, 0) != 0) {
        yError()<<"StateMachine::getEventQueue()"<<lua_tostring(L, -1)<<ENDL;
        lua_pop(L, 1);
        return false;
    }

    if(!lua_istable(L, -1)) {
        yError()<<"StateMachine::getEventQueue() got wrong result type"<<ENDL;
        lua_pop(L, 1);
        return false;
    }
    lua_pushnil(L);
    while(lua_next(L, -2) != 0) {
        if(lua_isstring(L, -1))
            equeue.push_back(lua_tostring(L, -1));
        else
            yWarning()<<"StateMachine::getEventQueue() found a wrong type in the result from rfsm_get_event_queue()"<<ENDL;
        lua_pop(L, 1);
    }
    lua_pop(L, 1); // pop the result from Lua stack
    return true;
}

bool StateMachine::doString(const std::string& command) {
    CHECK_RFSM_LOADED(L);
    return (Utils::dostring(L, command.c_str(), "command") == LUA_OK);
}

bool StateMachine::doFile(const std::string& filename) {
    CHECK_RFSM_LOADED(L);
    return (Utils::dofile(L, filename.c_str()) == LUA_OK);
}

void StateMachine::addLuaPackagePath(const std::string& path) {
    luaPackagePath += string(";")+path;
}

bool StateMachine::registerCFunction(const std::string& name, lua_CFunction func) {
    if(!func)
        return false;
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
    if(luaFuncReg.size() <= 2)
        lua_newtable(L);
    luaL_setfuncs (L, &luaFuncReg[0], 0);
    lua_pushvalue(L, -1);
    if(luaFuncReg.size() <= 2)
        lua_setglobal(L, "RFSM");
#else
    luaL_register(L, "RFSM", &luaFuncReg[0]);
#endif
    return true;
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

int StateMachine::preStepCallback(lua_State* L) {
    lua_getglobal(L, "RFSM_Owner");
    if(!lua_islightuserdata(L, -1)) {
        lua_pop(L, 1);
        yError()<<"StateMachine::preStepCallback() cannot access RFSM_Owner"<<ENDL;
        return 0;
    }
    StateMachine* owner = static_cast<StateMachine*>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    yAssert(owner!=NULL);
    owner->onPreStep();
    return 0;
}

int StateMachine::postStepCallback(lua_State* L) {
    lua_getglobal(L, "RFSM_Owner");
    if(!lua_islightuserdata(L, -1)) {
        lua_pop(L, 1);
        yError()<<"StateMachine::preStepCallback() cannot access RFSM_Owner"<<ENDL;
        return 0;
    }
    StateMachine* owner = static_cast<StateMachine*>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    yAssert(owner!=NULL);
    owner->onPostStep();
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

bool StateMachine::setStateCallback(const string &state, rfsm::StateCallback& callback) {
    CHECK_RFSM_LOADED(L);
    lua_getglobal(L, "rfsm_set_state_callbacks");
    if(!lua_isfunction(L, -1)) {
        yError()<<"StateMachine::setStateCallback() could not find rfsm_set_state_callbacks()"<<ENDL;
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
    if(result)
        callbacks[state] = &callback;
    else
        yWarning()<<"State"<<state<<"does not exist"<<ENDL;
    return result;
}

const std::string StateMachine::getCurrentState() {    
    if(!L) {
        yError()<<"StateMachine has not been initialized. call StateMachine::load()"<<ENDL;
        return "";
    }

    lua_getglobal(L, "rfsm_get_current_state");
    if(!lua_isfunction(L, -1)) {
        yError()<<"StateMachine::getCurrentState() could not find rfsm_get_current_state()"<<ENDL;
        return "";
    }

    if(lua_pcall(L, 0, 1, 0) != 0) {
        yError()<<"StateMachine::getCurrentState()"<<lua_tostring(L, -1)<<ENDL;        
        return "";
    }

    if(lua_type(L, -1) != LUA_TSTRING) {
        yError()<<"StateMachine::getCurrentState() got wrong result type"<<ENDL;
        lua_pop(L, 1); // pop the result from Lua stack        
        return "";
    }
    // converting the results
    string result = lua_tostring(L, -1);
    std::size_t pos = result.find("root.");
    if(pos != std::string::npos)
        result.erase(pos, 5);
    lua_pop(L, 1); // pop the result from Lua stack
    return result;
}

const rfsm::StateGraph& StateMachine::getStateGraph() {
    return graph;
}


bool StateMachine::getAllStateGraph() {
    CHECK_RFSM_LOADED(L);
    graph.states.clear();
    graph.transitions.clear();

    // reterieving all states
    lua_getglobal(L, "rfsm_get_all_states");
    if(!lua_isfunction(L, -1)) {
        yError()<<"StateMachine::getAllStateGraph() could not find rfsm_get_all_states()"<<ENDL;
        return false;
    }

    if(lua_pcall(L, 0, 1, 0) != 0) {
        yError()<<"StateMachine::getAllStateGraph()"<<lua_tostring(L, -1)<<ENDL;
        lua_pop(L, 1);
        return false;
    }

    if(!lua_istable(L, -1)) {
        yError()<<"StateMachine::getAllStateGraph() got wrong result type from rfsm_get_all_states()"<<ENDL;
        lua_pop(L, 1);
        return false;
    }

    lua_pushnil(L);
    while(lua_next(L, -2) != 0) {
        if(lua_istable(L, -1)) {
            StateGraph::State state;
            state.name = Utils::getTableField(L, "sname");
            std::size_t pos = state.name.find("root.");
            if(pos != std::string::npos)
                state.name.erase(pos, 5);
            state.type = Utils::getTableField(L, "stype");
            graph.states.push_back(state);
        }
        else
            yWarning()<<"StateMachine::getAllStateGraph() found a wrong type in the result from rfsm_get_all_states()"<<ENDL;
       lua_pop(L, 1);
    }
    lua_pop(L, 1); // pop the result from Lua stack

    // reterieving all transitions
    lua_getglobal(L, "rfsm_get_all_transitions");
    if(!lua_isfunction(L, -1)) {
        yError()<<"StateMachine::getAllStateGraph() could not find rfsm_get_all_transitions()"<<ENDL;
        return false;
    }

    if(lua_pcall(L, 0, 1, 0) != 0) {
        yError()<<"StateMachine::getAllStateGraph()"<<lua_tostring(L, -1)<<ENDL;
        lua_pop(L, 1);
        return false;
    }

    if(!lua_istable(L, -1)) {
        yError()<<"StateMachine::getAllStateGraph() got wrong result type from rfsm_get_all_transitions()"<<ENDL;
        lua_pop(L, 1);
        return false;
    }

    lua_pushnil(L);
    while(lua_next(L, -2) != 0) {
        if(lua_istable(L, -1)) {
            StateGraph::Transition trans;
            trans.source = Utils::getTableField(L, "source");
            std::size_t pos = trans.source.find("root.");
            if(pos != std::string::npos)
                trans.source.erase(pos, 5);
            trans.target = Utils::getTableField(L, "target");
            pos = trans.target.find("root.");
            if(pos != std::string::npos)
                trans.target.erase(pos, 5);
            istringstream ss(Utils::getTableField(L, "events"));
            string s;
            while (getline(ss, s, ','))
                trans.events.push_back(s);
            graph.transitions.push_back(trans);
        }
        else
            yWarning()<<"StateMachine::getAllStateGraph() found a wrong type in the result from rfsm_get_all_transitions()"<<ENDL;
       lua_pop(L, 1);
    }
    lua_pop(L, 1); // pop the result from Lua stack
    return true;
}

bool StateMachine::enablePreStepHook() {
    CHECK_RFSM_LOADED(L);
    return (Utils::dostring(L, "rfsm.pre_step_hook_add(fsm, rfsm_pre_step_hook)", "rfsm_pre_step_hook") != LUA_OK);
}


bool StateMachine::enablePostStepHook() {
    CHECK_RFSM_LOADED(L);
    return (Utils::dostring(L, "rfsm.post_step_hook_add(fsm, rfsm_post_step_hook)", "rfsm_post_step_hook") != LUA_OK);
}


void StateMachine::onPreStep() {
    if(verbose)
        yDebug()<<"onPreStep(): current state:"<<getCurrentState()<<ENDL;
}

void StateMachine::onPostStep() {
    if(verbose)
        yDebug()<<"onPostStep(): current state:"<<getCurrentState()<<ENDL;
}
