#include <unistd.h>

#include <iostream>
#include <string>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <rFSM.h>


using namespace std;
using namespace yarp::os;


int main(int argc, char** argv) {
    yarp::os::Network yarp;
    RFSM rfsm;

    if(argc < 2) {
        yInfo()<<"Usage:"<<argv[0]<<"myfsm.lua";
        return 0;
    }

    if(!rfsm.load(argv[1])) {
        yError()<<"Cannot load ...";
    }

    vector<string> events;
    rfsm.getAllEvents(events);
    yDebug()<<"Available events:";
    for(int i=0;i<events.size(); i++)
        yDebug()<<"\t"<<events[i];

    rfsm.run();
    rfsm.sendEvent("e_true");
    rfsm.run();
    //rfsm.setcallback("confgiure").entry = myfunc;
    //rfsm.setcallback("confgiure").exit = myfunc;
    return 0;
}


