#include <iostream>
#include <yarp/os/LogStream.h>
#include <rfsm.h>

using namespace yarp::os;

class ConfigureCallback : public rfsm::StateCallback {
public:
    virtual void entry() {
        std::cout<<"entry() of Configure (hello from C++)"<<std::endl;
    }

    virtual void doo() {
        std::cout<<"doo()   of Configure (hello from C++)"<<std::endl;
    }

    virtual void exit() {
        std::cout<<"exit()  of Configure (hello from C++)"<<std::endl;
    }
} configureCallback;


void printStateGraph(const rfsm::StateGraph& graph) {
    yInfo()<<"------------------------------------------";
    yInfo()<<"States:";
    for(int i=0; i<graph.states.size(); i++)
        yInfo()<<"\t"<<graph.states[i].name<<"("<<graph.states[i].type<<")";
    yInfo()<<"Transitions:";
    for(int i=0; i<graph.transitions.size(); i++) {
        std::string events;
        for(int e=0; e<graph.transitions[i].events.size();e++)
            events = events +  ((events.size()) ?  ", " + graph.transitions[i].events[e] : graph.transitions[i].events[e]);
        yInfo()<<"\t"<<graph.transitions[i].source<<"->"<<graph.transitions[i].target<<"("<<events<<")";
    }
    yInfo()<<"------------------------------------------";
}

void printEventQueue(std::vector<std::string>& equeue) {
    std::string events;
    for(int i=0; i<equeue.size(); i++)
        events = events +  ((events.size()) ?  ", " + equeue[i] : equeue[i]);
    yInfo()<<"Event queue:"<<events;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        yInfo()<<"Usage:"<<argv[0]<<"myfsm.lua";
        return 0;
    }

    // enable rfsm verbosity: true
    rfsm::StateMachine rfsm(true);
    // set the path to the rFSM if it is not set in LUA_PATH environemnt variable
    //rfsm.addLuaPackagePath("/path/to/rFSM/?.lua");

    if(!rfsm.load(argv[1])) {
        yError()<<"Cannot load"<<argv[1];
        return 0;
    }

    // enable pre and post step hooks for debuging
    //rfsm.enablePreStepHook();
    //rfsm.enablePostStepHook();

    // printing the state graph
    //printStateGraph(rfsm.getStateGraph());

    // setting some callbacks
    rfsm.setStateCallback("Configure", configureCallback);    
    // ...

    rfsm.run();    
    yDebug()<<"Sending event 'e_true'";
    rfsm.sendEvent("e_true");

    // printing the current event queue
    //std::vector<std::string> equeue;
    //rfsm.getEventQueue(equeue);
    //printEventQueue(equeue);

    rfsm.run();
    yDebug()<<"Sending event 'e_true'";
    rfsm.sendEvent("e_true");

    rfsm.run();
    yDebug()<<"Sending event 'e_interrupt'";
    rfsm.sendEvent("e_interrupt");
    rfsm.run();

    return 0;
}


