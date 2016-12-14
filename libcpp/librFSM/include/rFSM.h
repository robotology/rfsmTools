// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility
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
}

class rfsm::StateCallback {
public:
    virtual ~StateCallback() {}
    virtual void entry() {}
    virtual void doo() {}
    virtual void exit() {}
};


/**
 * @brief The rfsm::StateMachine class
 */
class rfsm::StateMachine {
public:
    /**
     * @brief StateMachine
     */
    StateMachine();

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
     * @brief closes the state machine if it is already loaded
     */
    void close();

    /**
     * @brief getEventsList retrieves all available events in the state machine
     * @return a list of all available events
     */
    const std::vector<std::string>& getEventsList();

    /**
     * @brief setStateCallback set an StateCallback object for a given state
     * @param state the name of the state (should corespond the state name in rFSM)
     * @param callback an object of StateCallback class
     * @return true on success
     */
    bool setStateCallback(const std::string state, rfsm::StateCallback& callback);

private:
    static int entryCallback(lua_State* L);
    static int dooCallback(lua_State* L);
    static int exitCallback(lua_State* L);

    bool getAllEvents();
    bool registerLuaFunction(const std::string& name, lua_CFunction func);
    void callEntryCallback(const std::string& state);
    void callDooCallback(const std::string& state);
    void callExitCallback(const std::string& state);

    //typedef int (rfsm::StateMachine::* LuaRfsmCallback) (lua_State *L);
    //bool registerLuaFunction(const std::string& name, LuaRfsmCallback func);

private:
    lua_State *L;
    std::vector<luaL_reg> luaFuncReg;
    std::string fileName;
    std::vector<std::string> events;
    std::map<std::string, rfsm::StateCallback*> callbacks;
};


#endif // RFSM_H
