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
    yInfo()<<"States:";
    for(int i=0; i<graph.states.size(); i++)
        yInfo()<<"\t"<<graph.states[i].name<<"("<<graph.states[i].type<<")";
}

int main(int argc, char** argv) {
    if(argc < 2) {
        yInfo()<<"Usage:"<<argv[0]<<"myfsm.lua";
        return 0;
    }

    rfsm::StateMachine rfsm(false);
    // set the path to the rFSM if it is not set in LUA_PATH environemnt variable
    //rfsm.addLuaPackagePath("/path/to/rFSM/?.lua");

    if(!rfsm.load(argv[1])) {
        yError()<<"Cannot load"<<argv[1];
        return 0;
    }

    // printing the state graph
    //printStateGraph(rfsm.getStateGraph());

    // setting some callbacks
    rfsm.setStateCallback("Configure", configureCallback);    
    // ...

    yDebug()<<"Current state:"<<rfsm.getCurrentState();
    rfsm.run();    

    yDebug()<<"Sending event 'e_true'";
    rfsm.sendEvent("e_true");

    rfsm.run();
    yDebug()<<"Current state:"<<rfsm.getCurrentState();

    yDebug()<<"Sending event 'e_true'";
    rfsm.sendEvent("e_true");
    rfsm.run();
    yDebug()<<"Current state:"<<rfsm.getCurrentState();

    yDebug()<<"Sending event 'e_interrupt'";
    rfsm.sendEvent("e_interrupt");
    rfsm.step();
    yDebug()<<"Current state:"<<rfsm.getCurrentState();
    rfsm.step();
    yDebug()<<"Current state:"<<rfsm.getCurrentState();
    return 0;
}


