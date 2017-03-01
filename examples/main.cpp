#include <iostream>

#include <rfsm.h>

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
    std::cout<<"------------------------------------------"<<std::endl;
    std::cout<<"States:";
    for(int i=0; i<graph.states.size(); i++)
        std::cout<<"\t"<<graph.states[i].name<<"("<<graph.states[i].type<<")"<<std::endl;
    std::cout<<"Transitions:"<<std::endl;
    for(int i=0; i<graph.transitions.size(); i++) {
        std::string events;
        for(int e=0; e<graph.transitions[i].events.size();e++)
            events = events +  ((events.size()) ?  ", " + graph.transitions[i].events[e] : graph.transitions[i].events[e]);
        std::cout<<"\t"<<graph.transitions[i].source<<"->"<<graph.transitions[i].target<<"("<<events<<")"<<std::endl;
    }
    std::cout<<"------------------------------------------"<<std::endl;
}

void printEventQueue(std::vector<std::string>& equeue) {
    std::string events;
    for(int i=0; i<equeue.size(); i++)
        events = events +  ((events.size()) ?  ", " + equeue[i] : equeue[i]);
    std::cout<<"Event queue:"<<events<<std::endl;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout<<"Usage:"<<argv[0]<<"myfsm.lua"<<std::endl;
        return 0;
    }

    // enable rfsm verbosity: true
    rfsm::StateMachine rfsm(true);
    // set the path to the rFSM if it is not set in LUA_PATH environemnt variable
    // or EMBED_RFSM is disbaled
    //rfsm.addLuaPackagePath("/path/to/rfsm/?.lua");

    if(!rfsm.load(argv[1])) {
        std::cerr<<"Cannot load"<<argv[1]<<std::endl;
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
    std::cout<<"Sending event 'e_true'"<<std::endl;
    rfsm.sendEvent("e_true");

    // printing the current event queue
    //std::vector<std::string> equeue;
    //rfsm.getEventQueue(equeue);
    //printEventQueue(equeue);

    rfsm.run();
    std::cout<<"Sending event 'e_true'"<<std::endl;
    rfsm.sendEvent("e_true");

    rfsm.run();
    std::cout<<"Sending event 'e_interrupt'"<<std::endl;
    rfsm.sendEvent("e_interrupt");
    rfsm.run();

    return 0;
}


