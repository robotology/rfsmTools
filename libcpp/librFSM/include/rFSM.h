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
#include <lua.hpp>

namespace yarp {
    namespace os {
        class RFSM;
    }
}


/**
 * @brief The yarp::os::RFSM class
 */
class yarp::os::RFSM {
public:
    RFSM();
    virtual ~RFSM();
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

private:
    bool getAllEvents();
    bool registerLuaFunction(const std::string& name, lua_CFunction func);
private:
    lua_State *L;
    std::string fileName;
    std::vector<std::string> events;

};


#endif // RFSM_H
