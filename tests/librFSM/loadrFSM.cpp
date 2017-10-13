// -*- mode:C++ { } tab-width:4 { } c-basic-offset:4 { } indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Ali Paikan <ali.paikan@iit.it>, Nicolo' Genesio <nicolo.genesio@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <rfsm.h>
#include <rtf/TestAssert.h>
#include <rtf/dll/Plugin.h>

#include <algorithm>

using namespace RTF;
using namespace rfsm;


class LoadrFSM : public RTF::TestCase {

public:
    LoadrFSM() : TestCase("LoadrFSM") {}

    virtual bool setup(int argc, char**argv) {
        RTF_ASSERT_ERROR_IF_FALSE(argc>=2, "Missing lua rfsm file as argument");
        filename = argv[1];
        RTF_TEST_REPORT(Asserter::format("Loading lua rfsm file %s", filename.c_str()));
        RTF_ASSERT_ERROR_IF_FALSE(fsm.load(filename), Asserter::format("Cannot load %s", filename.c_str()));
        return true;
    }

    virtual void run() {

        const rfsm::StateGraph& graph = fsm.getStateGraph();
        RTF_TEST_CHECK(fsm.getFileName() == filename, "Checking getFileName");
        RTF_TEST_CHECK(graph.states.size() == 4, Asserter::format("Checking number of states (got %d)", graph.states.size()));
        RTF_TEST_CHECK(graph.transitions.size() == 3, Asserter::format("Checking number of transitions (got %d)", graph.transitions.size()));

        // checking states
        std::vector<StateGraph::State> states = graph.states;
        StateGraph::State st_init = {"initial", "connector"};
        RTF_TEST_CHECK(find(states.begin(), states.end(), st_init) != states.end(), "Cheking 'initial' state");

        StateGraph::State st_ST1 = {"STATE1", "single"};
        RTF_TEST_CHECK(find(states.begin(), states.end(), st_ST1) != states.end(), "Cheking 'STATE1'");

        StateGraph::State st_ST2 = {"STATE2", "single"};
        RTF_TEST_CHECK(find(states.begin(), states.end(), st_ST2) != states.end(), "Cheking 'STATE2'");

        StateGraph::State st_ST3 = {"STATE3", "single"};
        RTF_TEST_CHECK(find(states.begin(), states.end(), st_ST3) != states.end(), "Cheking 'STATE3'");

        // cheking evnets list
        const std::vector<std::string>& events = fsm.getEventsList();
        RTF_TEST_CHECK(events.size() == 4, Asserter::format("Checking number of events (got %d)", events.size()));
        RTF_TEST_CHECK(find(events.begin(), events.end(), "e_one") != events.end(), "Cheking event 'e_one'");
        RTF_TEST_CHECK(find(events.begin(), events.end(), "e_two") != events.end(), "Cheking event 'e_two'");
        RTF_TEST_CHECK(find(events.begin(), events.end(), "e_three") != events.end(), "Cheking event 'e_three'");
        RTF_TEST_CHECK(find(events.begin(), events.end(), "e_init_fsm") != events.end(), "Cheking event 'e_init_fsm'");

        // checking transitions
        std::vector<StateGraph::Transition> trans = graph.transitions;
        StateGraph::Transition tr;
        tr.source = "initial";
        tr.target = "STATE1";
        RTF_TEST_CHECK(find(trans.begin(), trans.end(), tr) != trans.end(), "Cheking transition 'intitial -> STATE1'");


        tr.source = "STATE1";
        tr.target = "STATE2";
        tr.events.clear();
        tr.events.push_back("e_one");
        tr.events.push_back("e_two");
        RTF_TEST_CHECK(find(trans.begin(), trans.end(), tr) != trans.end(), "Cheking transition 'STATE1 -> STATE2'");

        tr.source = "STATE1";
        tr.target = "STATE3";
        tr.events.clear();
        tr.events.push_back("e_three");
        RTF_TEST_CHECK(find(trans.begin(), trans.end(), tr) != trans.end(), "Cheking transition 'STATE2 -> STATE3'");

    }

private:
    std::string filename;
    rfsm::StateMachine fsm;
};

PREPARE_PLUGIN(LoadrFSM)

