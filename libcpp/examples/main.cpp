#include <unistd.h>

#include <iostream>
#include <string>
#include <yarp/os/LogStream.h>
#include <rFSM.h>


using namespace std;
using namespace yarp::os;

class ConfigureCallback : public rfsm::StateCallback {
public:
    virtual void entry() {
        yInfo()<<"in the entry of ConfigureCallback (hello from c++)";
    }
} configureCallback;


int main(int argc, char** argv) {
    rfsm::StateMachine rfsm;

    if(argc < 2) {
        yInfo()<<"Usage:"<<argv[0]<<"myfsm.lua";
        return 0;
    }

    if(!rfsm.load(argv[1])) {
        yError()<<"Cannot load"<<argv[1];
        return 0;
    }

    // seting some callbacks
    rfsm.setStateCallback("Configure", configureCallback);
    // ...

    rfsm.run();
    rfsm.sendEvent("e_true");
    rfsm.run();
    rfsm.sendEvent("e_false");
    rfsm.run();
    return 0;
}


