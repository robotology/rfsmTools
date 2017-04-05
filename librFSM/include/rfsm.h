// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Ali Paikan <ali.paikan@iit.it>, Nicolo' Genesio <nicolo.genesio@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef RFSM_H
#define RFSM_H


#include <string>
#include <vector>
#include <map>

namespace rfsm {
    class StateMachine;
    class StateCallback;
    class StateGraph;
    class LuaTraceCallback;
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
 * @brief The LuaTraceCallback class
 */
class rfsm::LuaTraceCallback {
public:
    /**
     * @brief onTrace is called on lua traceback
     * @param message the trace message
     */
    virtual void onTrace(const std::string& message ) { }
};


/**
 * @brief The StateGraph class represents the rFSM state graph
 * in term of states and the transitions among them
 */
class rfsm::StateGraph {
public:
    struct LuaFuncCode {
        int startLine;
        int endLine;
        std::string fileName;
    };


    struct State {
        std::string name;
        std::string type;
        LuaFuncCode entry;
        LuaFuncCode doo;
        LuaFuncCode exit;
        bool operator==(const State& s) const {
            return (s.name == name);
        }
    };

    struct Transition {
        std::string source;
        std::string target;
        std::vector<std::string> events;
        int priority;
        bool operator==(const Transition& s) const {
            //return (s.source == source) && (s.target == target) && (s.events == events);
            return (s.source == source) && (s.target == target);
        }
    };

    typedef std::vector<Transition>::iterator TransitionItr;
    typedef std::vector<State>::iterator StateItr;

    void clear();

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
class rfsm::StateMachine : public rfsm::LuaTraceCallback{
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
     *
     * \note This should be called before running/stepping the state machine
     */
    bool enablePreStepHook();

    /**
     * @brief enablePostStepHook enables the post-step hook function
     * of the rFSM. If it is enabled, then onPostStep() callback will be called
     * after stepping the state machine.
     * @return true on success
     *
     * \note This should be called before running/stepping the state machine
     */
    bool enablePostStepHook();

    /**
     * @brief catchPrintOutput redirect the output of the lua 'print()' function
     * to rfsm::StateMachine::onInfo()
     * @return true on success
     *
     * \note This should be called before running/stepping the state machine
     */
    bool catchPrintOutput();

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

    /**
     * @brief this is called on every warning message generated from rFSM
     *        in verbose mode (i.e. StateMachine(bool verbose=true) )
     * @param message the warning message
     */
    virtual void onWarning(const std::string message);

    /**
     * @brief this is called on every error message generated from rFSM
     * @param message the error message
     */
    virtual void onError(const std::string message);

    /**
     * @brief this is called on every info message generated from rFSM
     *        in verbose mode (i.e. StateMachine(bool verbose=true) )
     * @param message the info message
     */
    virtual void onInfo(const std::string message);

private:
    /**
     * @brief onTrace is called on lua traceback
     * @param message the trace message
     */
    virtual void onTrace(const std::string& message );

private:
	class Private;
    Private * const mPriv;
    bool verbose;
};


#endif // RFSM_H
