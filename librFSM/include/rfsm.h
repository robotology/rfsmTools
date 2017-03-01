// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2016 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef RFSM_H
#define RFSM_H


#include <string>
#include <vector>
#include <map>

#include <lua.hpp>

namespace rfsm {
    class StateMachine;
    class StateCallback;
    class StateGraph;
}

#ifndef luaL_reg
    #define luaL_reg luaL_Reg
#endif

/**
 * @brief The rfsm::StateCallback class can be used to implement
 *  the rFSM state callbacs in c++
 */
class rfsm::StateCallback {
public:
    virtual ~StateCallback() {}
    /**
     * @brief entry is called on rFSM state.entry
     */
    virtual void entry() {}

    /**
     * @brief doo is called on rFSM state.doo
     */
    virtual void doo() {}

    /**
     * @brief exit is called on rFSM state.exit
     */
    virtual void exit() {}
};


/**
 * @brief The StateGraph class represents the rFSM state graph
 * in term of states and the transitions among them
 */
class rfsm::StateGraph {
public:
    struct State {
        std::string name;
        std::string type;
    };

    struct Transition {
        std::string source;
        std::string target;
        std::vector<std::string> events;
    };

public:
    /**
     * @brief states is a list of all states
     */
    std::vector<State> states;
    /**
     * @brief transitions is a list of all transitions
     */
    std::vector<Transition> transitions;
};


/**
 * @brief The rfsm::StateMachine class
 */
class rfsm::StateMachine {
public:

    /**
     * @brief StateMachine load and execute a rFSM state machine written in LUA
     * @param verbose enable verbosity if it is true.
     */
    StateMachine(bool verbose=false);

    /**
     * @brief ~StateMachine
     */
    virtual ~StateMachine();

    /**
     * @brief getFileName
     * @return the loaded rFSM state machine's name
     */
    const std::string getFileName();

    /**
     * @brief loads and initializes a rFSM state machine
     * @param filename rFSM state machine file name
     * @return true on success
     */
    bool load(const std::string& filename);

    /**
     * @brief run calls rfsm.run()
     * @return true on success
     */
    bool run();

    /**
     * @brief step calls rfs.step(n)
     * @param n number the steps to taken (defaule is 1)
     * @return true on success
     */
    bool step(unsigned int n=1);

    /**
     * @brief sendEvent calls rfsm.send_events(event)
     * @param event a single event to be send to the state machine
     * @return true on success
     */
    bool sendEvent(const std::string& event);

    /**
     * @brief sendEvents calls rfsm.send_events(...)
     * @param n number of events to be send
     * @param ...the n events given as separate params (e.g. sendEvents(2, "e1", "e2"))
     * @return true on success
     */
    bool sendEvents(unsigned int n, ...);

    /**
     * @brief doString execute a generic lua command
     * @param command a string containg a valid lua command
     * @return true on success
     */
    bool doString(const std::string& command);

    /**
     * @brief doFile execute a lua file
     * @param filename lua file name to execute
     * @return true on success
     */
    bool doFile(const std::string& filename);

    /**
     * @brief addLuaPackagePath add a new path to lua package.path
     * @param path to a folder containg lua packages
     */
    void addLuaPackagePath(const std::string& path);

    /**
     * @brief closes the state machine if it is already loaded
     */
    void close();

    /**
     * @brief setStateCallback set an StateCallback object for a given state
     * @param state the name of the state (should corespond the state name in rFSM)
     * @param callback an object of StateCallback class
     * @return true on success
     */
    bool setStateCallback(const std::string& state, rfsm::StateCallback& callback);

    /**
     * @brief getCurrentState returns the current activated state
     * @return the name of current active state
     */
    const std::string getCurrentState();

    /**
     * @brief getEventsList retrieves all available events in the state machine
     * @return a list of all available events
     */
    const std::vector<std::string>& getEventsList();

    /**
     * @brief getEventQueue gets the current events in the rFSM event queue
     * @param equeue a vector of string to be filled with the current events
     * @return true on success
     */
    bool getEventQueue(std::vector<std::string>& equeue);

    /**
     * @brief getStateGraph return the rFSM state graph
     * @return rFSM state graph
     */
    const rfsm::StateGraph& getStateGraph();

    /**
     * @brief enablePreStepHook enables the pre-step hook function
     * of the rFSM. If it is enabled, then onPreStep() callback will be called
     * before stepping the state machine.
     * @return true on success
     */
    bool enablePreStepHook();

    /**
     * @brief enablePostStepHook enables the post-step hook function
     * of the rFSM. If it is enabled, then onPostStep() callback will be called
     * after stepping the state machine.
     * @return true on success
     */
    bool enablePostStepHook();

public:
    /**
     * @brief if pre-step hook function of rFSM is enabled
     * this callback is called before stepping the state machine.
     */
    virtual void onPreStep();

    /**
     * @brief if post-step hook function of rFSM is enabled
     * this callback is called after stepping the state machine.
     */
    virtual void onPostStep();

private:
    static int entryCallback(lua_State* L);
    static int dooCallback(lua_State* L);
    static int exitCallback(lua_State* L);
    static int preStepCallback(lua_State* L);
    static int postStepCallback(lua_State* L);

    bool getAllEvents();
    bool getAllStateGraph();
    bool registerAuxiliaryFunctions();
    bool registerCFunction(const std::string& name, lua_CFunction func);
    void callEntryCallback(const std::string& state);
    void callDooCallback(const std::string& state);
    void callExitCallback(const std::string& state);

    //typedef int (rfsm::StateMachine::* LuaRfsmCallback) (lua_State *L);
    //bool registerLuaFunction(const std::string& name, LuaRfsmCallback func);

private:
    lua_State *L;
    std::vector<luaL_reg> luaFuncReg;
    std::string fileName;
    std::string luaPackagePath;
    std::vector<std::string> events;
    rfsm::StateGraph graph;
    std::map<std::string, rfsm::StateCallback*> callbacks;
    bool verbose;
};


#endif // RFSM_H
