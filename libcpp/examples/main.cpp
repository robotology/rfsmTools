#include <yarp/os/LogStream.h>
#include <rFSM.h>

using namespace yarp::os;

class ConfigureCallback : public rfsm::StateCallback {
public:
    virtual void entry() {
        yInfo()<<"in the entry() of ConfigureCallback (hello from C++)";
    }

    virtual void doo() {
        yInfo()<<"in the doo()   of ConfigureCallback (hello from C++)";
    }

    virtual void exit() {
        yInfo()<<"in the exit()  of ConfigureCallback (hello from C++)";
    }
} configureCallback;


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

    // setting some callbacks
    rfsm.setStateCallback("Configure", configureCallback);
    //rfsm.setStateCallback("UpdateModule", updatemoduleCallback);
    // ...

    rfsm.run();
    rfsm.sendEvent("e_true");
    rfsm.run();
    rfsm.sendEvent("e_true");
    rfsm.run();
    return 0;
}


