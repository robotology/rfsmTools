# rFSM Tools

The reository contains a C++ library to load and execute rFSM LUA-based state machines and a graphical tools to run/simulate the state machine.


![scenario2](/doc/rfsmGui.png)


Dependencies 
------------
* Lua (5.x)
* [rFSM](https://people.mech.kuleuven.be/~bruyninc/rFSM/doc/README.html) (optional)
* Graphviz development library for `rfsmGui` 

`librFSM` can be built with the built-in rfsm lua library (default is off). Please see [Compile and build](#Compile and build) 


Installation on Linux
---------------------
* If you do not want to use the built-in rfsm then follwo the installation of 
rFSM on https://people.mech.kuleuven.be/~bruyninc/rFSM/doc/README.html. 

* Install one of the lua developemnt library (e.g., 5.1, 5.2, ...)

```
$ sudo apt-get install lua5.X-dev
```

Compile and build
-----------------
```
$ cd librFSM
$ mkdir build; cd build
$ cmake ../; make
```
Notice: if you do not have rfsm installed and configure on your system, you can 
still use the librFSM with the the rfsm built-in library. To do that: 

```
$ cmake -DEMBED_RFSM=ON ../; make
```


Testing
-------
```
$ ./examples/rfsmTest ../examples/fsm/rfmodule_fsm.lua
```

Example of using rfsm from C++
------------------------------
```c++
class MyStateCallback : public rfsm::StateCallback {
public:
    virtual void entry() {
        std::cout<<"entry() of MyState (hello from C++)"<<std::endl;
    }
} myStateCallback;

int main(int argc, char** argv) {
    rfsm::StateMachine rfsm;   
    rfsm.load("my-statemachine.lua")

    // setting some callbacks
    rfsm.setStateCallback("MyState", myStateCallback);    

    rfsm.sendEvent("event1");
    rfsm.step(1);    
    std::cout<<rfsm.getCurrentState();
    rfsm.sendEvents(2, "event1", "event2");
    rfsm.run()
    //...    
    return 0;
}
```


Contributors
-------------
* [*Ali Paikan*](https://github.com/apaikan)
* [*Nicolò Genesio*](https://github.com/Nicogene)

