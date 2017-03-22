#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <sstream>
#include <rfsmUtils.h>
#include <rfsm.h>

#include <lua.hpp>

using namespace std;
using namespace rfsm;

#define CHECK_LUA_INITIALIZED(L) if(!L) { yError()<<"Lua has not been initialized. call StateMachine::load()"<<ENDL; return false; }

#ifdef WITH_EMBEDDED_RFSM
extern "C" const char gen_rfsm_res[];
extern "C" const char gen_rfsm_utils_res[];
#endif



class StateMachine::Private {
public:
	Private() : L(NULL) { } 
	virtual ~Private() { }

    static int entryCallback(lua_State* L);
    static int dooCallback(lua_State* L);
    static int exitCallback(lua_State* L);
    static int preStepCallback(lua_State* L);
    static int postStepCallback(lua_State* L);
    static int warningCallback(lua_State* L);
    static int infoCallback(lua_State* L);
    static int errorCallback(lua_State* L);

    static bool getLuaFuncStringParam(lua_State* L,
                                      StateMachine* &owner, std::string& strParam);

    bool getAllEvents();
    bool getAllStateGraph();
    bool registerAuxiliaryFunctions();
    bool registerCFunction(const std::string& name, lua_CFunction func);
    void callEntryCallback(const std::string& state);
    void callDooCallback(const std::string& state);
    void callExitCallback(const std::string& state);
    bool isrFSMLoaded();
    //typedef int (rfsm::StateMachine::* LuaRfsmCallback) (lua_State *L);
    //bool registerLuaFunction(const std::string& name, LuaRfsmCallback func);

    void close();

public:
    lua_State *L;
    std::vector<luaL_reg> luaFuncReg;
    std::string fileName;
    std::string luaPackagePath;
    std::vector<std::string> events;
    rfsm::StateGraph graph;
    std::map<std::string, rfsm::StateCallback*> callbacks;
};


StateMachine::StateMachine(bool verbose):  mPriv(new Private()) {	
    StateMachine::verbose = verbose;
    Utils::setLuaTraceCallback((rfsm::LuaTraceCallback*) this);
}

StateMachine::~StateMachine() {	
    close();
	delete mPriv;
}

void StateMachine::close() {
    mPriv->close();
}

const std::string StateMachine::getFileName() {
    return mPriv->fileName;
}

bool StateMachine::load(const std::string& filename) {   
    close();
    mPriv->fileName = filename;
    // initiate lua state
    mPriv->L = luaL_newstate();
    if(mPriv->L==NULL) {
        yError()<<"Cannot initialize lua! (luaL_newstate)"<<ENDL;
        return false;
    }

    luaL_openlibs(mPriv->L);

    // setting user-defined lua package paths
    if(mPriv->luaPackagePath.size()) {
        string command = "package.path=package.path .. '" + mPriv->luaPackagePath + "'";
        if(Utils::dostring(mPriv->L, command.c_str(), "command") != LUA_OK)
            yWarning()<<"Could not set lua package path from"<<mPriv->luaPackagePath<<ENDL;
    }

    // loading rfsm package
#ifdef WITH_EMBEDDED_RFSM
    if(Utils::dostring(mPriv->L, gen_rfsm_utils_res, "gen_rfsm_utils_res") != LUA_OK)
        return false;
    if(Utils::dostring(mPriv->L, gen_rfsm_res, "gen_rfsm_res") != LUA_OK)
        return false;
#else
    if (Utils::dolibrary(mPriv->L, "rfsm") != LUA_OK) {
        close();
        return false;
    }
#endif

    // registering some utility fuctions in lua
    lua_pushlightuserdata(mPriv->L, this);
    lua_setglobal(mPriv->L, "RFSM_Owner");
    if(!mPriv->registerAuxiliaryFunctions())
        return false;

    // loading rfsm state machine
    string cmd = "fsm_model = rfsm.load('"+filename+"')";
    if(Utils::dostring(mPriv->L, cmd.c_str(), "fsm_model") != LUA_OK) {
        close();
        return false;
    }

    // setting verbosity mode
    if(!verbose) {
        doString("fsm_model.warn = rfsm_null_func");
        doString("fsm_model.info = rfsm_null_func");
    }
    else {
        doString("fsm_model.warn = rfsm_warning");
        doString("fsm_model.info = rfsm_info");
    }
    doString("fsm_model.err = rfsm_error");

    // initializing rfsm state machine
    if(Utils::dostring(mPriv->L, "fsm = rfsm.init(fsm_model)", "fsm") != LUA_OK) {
        close();
        return false;
    }

    // getting all availabe events and state graph
    if(!mPriv->getAllEvents())
        yWarning()<<"Cannot retrieve all events"<<ENDL;
    if(!mPriv->getAllStateGraph())
        yWarning()<<"Cannot retrieve state graph"<<ENDL;

    return true;
}

bool StateMachine::run() {
    CHECK_LUA_INITIALIZED(mPriv->L);
    return (Utils::dostring(mPriv->L, "rfsm.run(fsm)", "run") == LUA_OK);
}

bool StateMachine::step(unsigned int n) {
    CHECK_LUA_INITIALIZED(mPriv->L);
    char command[128];

#ifdef WIN32
    _snprintf(command, 128, "rfsm.step(fsm, %d)", n);
#else
	snprintf(command, 128, "rfsm.step(fsm, %d)", n);
#endif
    return (Utils::dostring(mPriv->L, command, "step") == LUA_OK);
}

bool StateMachine::sendEvent(const std::string& event) {
    if(!mPriv->isrFSMLoaded())
        return false;
    if(std::find(mPriv->events.begin(), mPriv->events.end(), event) == mPriv->events.end())
        yWarning()<<"Sending the undefined event"<<event<<ENDL;
    string command = "rfsm.send_events(fsm, '"+event+"')";
    return (Utils::dostring(mPriv->L, command.c_str(), "sendEvent") == LUA_OK);
}

bool StateMachine::sendEvents(unsigned int n, ...) {
    if(!mPriv->isrFSMLoaded())
        return false;
    register unsigned int i;
    va_list ap;
    va_start(ap, n);
    const char* event = va_arg(ap, char*);
    yAssert(event != NULL);
    string command = string("rfsm.send_events(fsm, '") + event + "'";
    for(i=2; i<= n; i++) {
        event = va_arg(ap, const char*);
        yAssert(event != NULL);
        if(std::find(mPriv->events.begin(), mPriv->events.end(), event) == mPriv->events.end())
            yWarning()<<"Sending the undefined event"<<event<<ENDL;
        command += string(", '") + event + string("'");
    }
    va_end(ap);
    command += ")";
    return (Utils::dostring(mPriv->L, command.c_str(), "sendEvents") == LUA_OK);
}


const std::vector<std::string>& StateMachine::getEventsList() {
    return mPriv->events;
}

bool StateMachine::getEventQueue(std::vector<std::string>& equeue) {
    if(!mPriv->isrFSMLoaded())
        return false;
    equeue.clear();
    lua_getglobal(mPriv->L, "rfsm_get_event_queue");
    if(!lua_isfunction(mPriv->L, -1)) {
        yError()<<"StateMachine::getEventQueue() could not find rfsm_get_event_queue()"<<ENDL;
        return false;
    }

    if(lua_pcall(mPriv->L, 0, 1, 0) != 0) {
        yError()<<"StateMachine::getEventQueue()"<<lua_tostring(mPriv->L, -1)<<ENDL;
        lua_pop(mPriv->L, 1);
        return false;
    }

    if(!lua_istable(mPriv->L, -1)) {
        yError()<<"StateMachine::getEventQueue() got wrong result type"<<ENDL;
        lua_pop(mPriv->L, 1);
        return false;
    }
    lua_pushnil(mPriv->L);
    while(lua_next(mPriv->L, -2) != 0) {
        if(lua_isstring(mPriv->L, -1))
            equeue.push_back(lua_tostring(mPriv->L, -1));
        else
            yWarning()<<"StateMachine::getEventQueue() found a wrong type in the result from rfsm_get_event_queue()"<<ENDL;
        lua_pop(mPriv->L, 1);
    }
    lua_pop(mPriv->L, 1); // pop the result from Lua stack
    return true;
}

bool StateMachine::doString(const std::string& command) {
    CHECK_LUA_INITIALIZED(mPriv->L);
    return (Utils::dostring(mPriv->L, command.c_str(), "command") == LUA_OK);
}

bool StateMachine::doFile(const std::string& filename) {
    CHECK_LUA_INITIALIZED(mPriv->L);
    return (Utils::dofile(mPriv->L, filename.c_str()) == LUA_OK);
}

void StateMachine::addLuaPackagePath(const std::string& path) {
    mPriv->luaPackagePath += string(";")+path;
}


int StateMachine::Private::entryCallback(lua_State* L) {
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
        owner->mPriv->callEntryCallback(cst);
    }
    else
        yError()<<"StateMachine::entryCallback() expects a string argument"<<ENDL;
    return 0;
}

int StateMachine::Private::dooCallback(lua_State* L){
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
        owner->mPriv->callDooCallback(cst);
    }
    else
        yError()<<"StateMachine::dooCallback() expects a string argument"<<ENDL;
    return 0;
}

int StateMachine::Private::exitCallback(lua_State* L){
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
        owner->mPriv->callExitCallback(cst);
    }
    else
        yError()<<"StateMachine::exitCallback() expects a string argument"<<ENDL;
    return 0;
}

int StateMachine::Private::preStepCallback(lua_State* L) {
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

int StateMachine::Private::postStepCallback(lua_State* L) {
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
	return 0;
}

int StateMachine::Private::warningCallback(lua_State* L) {
    std::string message;
    StateMachine* owner;
    if(getLuaFuncStringParam(L, owner, message))
        owner->onWarning(message);
    else
        yError()<<"StateMachine::warningCallback() failed on getLuaFuncStringParam"<<ENDL;
    return 0;
}

int StateMachine::Private::errorCallback(lua_State* L) {
    std::string message;
    StateMachine* owner;
    if(getLuaFuncStringParam(L, owner, message))
        owner->onError(message);
    else
        yError()<<"StateMachine::errorCallback() failed on getLuaFuncStringParam"<<ENDL;
    return 0;
}

int StateMachine::Private::infoCallback(lua_State* L) {
    std::string message;
    StateMachine* owner;
    if(getLuaFuncStringParam(L, owner, message))
        owner->onInfo(message);
    else
        yError()<<"StateMachine::infoCallback() failed on getLuaFuncStringParam"<<ENDL;
    return 0;
}

bool StateMachine::Private::getLuaFuncStringParam(lua_State* L, StateMachine* &owner , std::string& strParam) {
    if (lua_gettop(L) < 1) {
        yError()<<"StateMachine::getLuaFuncStringParam() expects exactly one argument"<<ENDL;
       return false;
    }

    const char *cst = luaL_checkstring(L, -1);
    if(cst) {
        lua_getglobal(L, "RFSM_Owner");
        if(!lua_islightuserdata(L, -1)) {
            lua_pop(L, 1);
            yError()<<"StateMachine::getLuaFuncStringParam() cannot access RFSM_Owner"<<ENDL;
            return false;
        }
        owner = static_cast<StateMachine*>(lua_touserdata(L, -1));
        lua_pop(L, 1);
        yAssert(owner!=NULL);
        strParam = cst;
        return true;
    }
    else
        yError()<<"StateMachine::getLuaFuncStringParam() expects a string argument"<<ENDL;
    return false;
}

void StateMachine::Private::callEntryCallback(const std::string& state) {
    std::map<string,StateCallback*>::iterator it;
    if ((it = callbacks.find(state)) == callbacks.end())
        return;
    it->second->entry();
}

void StateMachine::Private::callDooCallback(const std::string& state) {
    std::map<string,StateCallback*>::iterator it;
    if ((it = callbacks.find(state)) == callbacks.end())
        return;
    it->second->doo();
}

void StateMachine::Private::callExitCallback(const std::string& state) {
    std::map<string,StateCallback*>::iterator it;
    if ((it = callbacks.find(state)) == callbacks.end())
        return;
    it->second->exit();
}

bool StateMachine::Private::isrFSMLoaded() {
    CHECK_LUA_INITIALIZED(L);
    lua_getglobal(L, "fsm");
    return lua_istable(L, -1);
}

bool StateMachine::setStateCallback(const string &state, rfsm::StateCallback& callback) {
    if(!mPriv->isrFSMLoaded())
        return false;
    lua_getglobal(mPriv->L, "rfsm_set_state_callbacks");
    if(!lua_isfunction(mPriv->L, -1)) {
        yError()<<"StateMachine::setStateCallback() could not find rfsm_set_state_callbacks()"<<ENDL;
        return false;
    }

    lua_pushstring(mPriv->L, state.c_str());
    if(lua_pcall(mPriv->L, 1, 1, 0) != 0) {
        yError()<<"StateMachine::setStateCallback()"<<lua_tostring(mPriv->L, -1)<<ENDL;
        lua_pop(mPriv->L, 1);
        return false;
    }

    // converting the results
    bool result = (lua_toboolean(mPriv->L, -1) == 1);
    lua_pop(mPriv->L, 1); // pop the result from Lua stack    
    if(result)
        mPriv->callbacks[state] = &callback;
    else
        yWarning()<<"State"<<state<<"does not exist"<<ENDL;
    return result;
}

const std::string StateMachine::getCurrentState() {
    if(!mPriv->isrFSMLoaded()) {
        yError()<<"StateMachine has not been initialized. call StateMachine::load()"<<ENDL;
        return "";
    }

    lua_getglobal(mPriv->L, "rfsm_get_current_state");
    if(!lua_isfunction(mPriv->L, -1)) {
        yError()<<"StateMachine::getCurrentState() could not find rfsm_get_current_state()"<<ENDL;
        return "";
    }

    if(lua_pcall(mPriv->L, 0, 1, 0) != 0) {
        yError()<<"StateMachine::getCurrentState()"<<lua_tostring(mPriv->L, -1)<<ENDL;        
        return "";
    }

    if(lua_type(mPriv->L, -1) != LUA_TSTRING) {
        yError()<<"StateMachine::getCurrentState() got wrong result type"<<ENDL;
        lua_pop(mPriv->L, 1); // pop the result from Lua stack        
        return "";
    }
    // converting the results
    string result = lua_tostring(mPriv->L, -1);
    std::size_t pos = result.find("root.");
    if(pos != std::string::npos)
        result.erase(pos, 5);
    lua_pop(mPriv->L, 1); // pop the result from Lua stack
    return result;
}

const rfsm::StateGraph& StateMachine::getStateGraph() {
    return mPriv->graph;
}



bool StateMachine::enablePreStepHook() {
    if(!mPriv->isrFSMLoaded())
        return false;
    return (Utils::dostring(mPriv->L, "rfsm.pre_step_hook_add(fsm, rfsm_pre_step_hook)", "rfsm_pre_step_hook") != LUA_OK);
}


bool StateMachine::enablePostStepHook() {
    if(!mPriv->isrFSMLoaded())
        return false;
    return (Utils::dostring(mPriv->L, "rfsm.post_step_hook_add(fsm, rfsm_post_step_hook)", "rfsm_post_step_hook") != LUA_OK);
}


void StateMachine::onPreStep() {
    if(verbose)
        yDebug()<<"onPreStep(): current state:"<<getCurrentState()<<ENDL;
}

void StateMachine::onPostStep() {
    if(verbose)
        yDebug()<<"onPostStep(): current state:"<<getCurrentState()<<ENDL;
}


void StateMachine::onWarning(const std::string message) {
    if(verbose)
        yWarning()<<" "<<message<<ENDL;
}


void StateMachine::onError(const std::string message) {
    yError()<<" "<<message<<ENDL;
}

void StateMachine::onInfo(const std::string message) {
    if(verbose)
        yInfo()<<" "<<message<<ENDL;
}

void StateMachine::onTrace(const std::string& message ) {
    onError(message);
}


/**********************************************************
* class StateMachine::Private
***********************************************************/
bool StateMachine::Private::registerCFunction(const std::string& name, lua_CFunction func) {
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

bool StateMachine::Private::registerAuxiliaryFunctions() {
    registerCFunction("entryCallback", StateMachine::Private::entryCallback);
    registerCFunction("dooCallback", StateMachine::Private::dooCallback);
    registerCFunction("exitCallback", StateMachine::Private::exitCallback);
    registerCFunction("preStepCallback", StateMachine::Private::preStepCallback);
    registerCFunction("postStepCallback", StateMachine::Private::postStepCallback);    
    registerCFunction("warningCallback", StateMachine::Private::warningCallback);
    registerCFunction("errorCallback", StateMachine::Private::errorCallback);
    registerCFunction("infoCallback", StateMachine::Private::infoCallback);

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
    if(Utils::dostring(L, RFSM_PACK_ARGS_CHUNK, "RFSM_PACK_ARGS_CHUNK") != LUA_OK)
            return false;
    if(Utils::dostring(L, RFSM_WARNING_CHUNK, "RFSM_WARNING_CHUNK") != LUA_OK)
        return false;
    if(Utils::dostring(L, RFSM_ERROR_CHUNK, "RFSM_ERROR_CHUNK") != LUA_OK)
        return false;
    if(Utils::dostring(L, RFSM_INFO_CHUNK, "RFSM_INFO_CHUNK") != LUA_OK)
        return false;
    return true;
}

bool StateMachine::Private::getAllEvents() {
    if(!isrFSMLoaded())
        return false;
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

bool StateMachine::Private::getAllStateGraph() {
    if(!isrFSMLoaded())
        return false;
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
            state.name = Utils::getTableStringField(L, "sname");
            std::size_t pos = state.name.find("root.");
            if(pos != std::string::npos)
                state.name.erase(pos, 5);
            state.type = Utils::getTableStringField(L, "stype");

            state.entry.startLine = Utils::getTableNumberField(L, "sentry_l1");
            state.entry.endLine = Utils::getTableNumberField(L, "sentry_l2");
            state.entry.fileName = Utils::getTableStringField(L, "sentry_filename");

            state.doo.startLine = Utils::getTableNumberField(L, "sdoo_l1");
            state.doo.endLine = Utils::getTableNumberField(L, "sdoo_l2");
            state.doo.fileName = Utils::getTableStringField(L, "sdoo_filename");

            state.exit.startLine = Utils::getTableNumberField(L, "sexit_l1");
            state.exit.endLine = Utils::getTableNumberField(L, "sexit_l2");
            state.exit.fileName = Utils::getTableStringField(L, "sexit_filename");

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
            trans.source = Utils::getTableStringField(L, "source");
            std::size_t pos = trans.source.find("root.");
            if(pos != std::string::npos)
                trans.source.erase(pos, 5);
            trans.target = Utils::getTableStringField(L, "target");
            pos = trans.target.find("root.");
            if(pos != std::string::npos)
                trans.target.erase(pos, 5);
            istringstream ss(Utils::getTableStringField(L, "events"));
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

void StateMachine::Private::close() {
    if(L){
        lua_close(L);
        L = NULL;
    }
    luaFuncReg.clear();
    callbacks.clear();
    graph.clear();
    events.clear();
}
