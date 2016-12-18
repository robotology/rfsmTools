# rFSMSimulator
A C++ library to load an execute rFSM LUA-based state machines

Example
-------
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

    rfsm.sendEvent("e_true");
    rfsm.step(1);    
    std::cout<<rfsm.getCurrentState();
    rfsm.sendEvents(2, "e_true", "e_ok");
    rfsm.run()
    //...    
    return 0;
}
```

Dependencies 
------------
* Lua (5.x)
* [rFSM](https://people.mech.kuleuven.be/~bruyninc/rFSM/doc/README.html)


Installation on Linux
---------------------
Follwo the installation of rFSM on https://people.mech.kuleuven.be/~bruyninc/rFSM/doc/README.html. 

Install one of the lua developemnt library (e.g., 5.1, 5.2, ...)

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

Testing
-------
```
$ ./examples/rfsmTest ../examples/fsm/rfmodule_fsm.lua
```

Contributors
-------------
* [*Ali Paikan*](https://github.com/apaikan)
* [*Nicolò Genesio*](https://github.com/Nicogene)

