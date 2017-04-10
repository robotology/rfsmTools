# rFSM Tools

This repository contains a C++ library (*librFSM*) to load and execute rFSM LUA-based state machines and a graphical tools (*rfsmGui*) to run/simulate the state machine.


Dependencies 
------------
* [Lua](https://www.lua.org/about.html) library (5.x)
* [Qt 5](https://www.qt.io/download/)
* [Graphviz](http://www.graphviz.org/) development library for *rfsmGui*
* [rFSM](https://people.mech.kuleuven.be/~bruyninc/rFSM/doc/README.html) (optional)

*librFSM* can be built with the built-in rfsm lua library (default is ON).


Installation on Linux
---------------------
* Install one of the lua developemnt library (e.g., 5.1, 5.2, ...) and the graphviz development package 
* If you do not want to use the built-in rfsm then follwo the installation of 
rFSM on https://people.mech.kuleuven.be/~bruyninc/rFSM/doc/README.html. 

```
$ sudo apt-get install lua5.X-dev libgraphviz-dev \
       qtbase5-dev qtdeclarative5-dev qtdeclarative5-qtquick2-plugin qtdeclarative5-window-plugin \
       qtdeclarative5-controls-plugin qtdeclarative5-dialogs-plugin
```

* Compile and build
```
$ cd rfsmTools
$ mkdir build; cd build
$ cmake ../; make
```
Notice: if you do not want to to use the built-in rfsm you can disable cmake `EMBED_RFSM` flag:

```
$ cmake -DEMBED_RFSM=OFF ../; make
```

Installation on Windows
---------------------
* Install [lua for windows](https://github.com/rjpcomputing/luaforwindows/releases/download/v5.1.5-51/LuaForWindows_v5.1.5-51.exe) or download and build one of the lua library (e.g., 5.1, 5.2, ...) 
* Download and Install Qt5 (https://www.qt.io/download/). Follow the the installation instruction to set the environemnt variables (`Qt5_DIR` and `PATH`). 
* Install [graphviz for windows](http://www.graphviz.org/pub/graphviz/stable/windows/graphviz-2.38.msi) and set/update the corresponding enviornment variables (`GRAPHVIZ_ROOT` and `PATH`): 
```
 C:\> setx.exe Qt5_DIR <path to the qt installed directory>\qt5\5.X\msvc20XX\lib\cmake\Qt5
 C:\> setx.exe PATH "%PATH%;<path to the qt installed directory>\qt5\5.X\msvc2010\bin"
 C:\> setx.exe GRAPHVIZ_ROOT <path to the graphviz installed directory>
 C:\> setx.exe PATH "%PATH%;<path to the graphviz installed directory>\bin"
```

* The compilation is straightforward and uses the CMake build system. Get [CMake for windows](https://cmake.org/download/) if you have not yet installed. Then simply run the Cmake and, set the project (rfsmTools) root folder and the desired build folder. Configure and generate project solution for your favorite IDE (e.g. Visual Studio 11). Then open the solution from your IDE and build the project.   


Testing the Library
--------------------
```
$ ./examples/rfsmTest ../examples/fsm/rfmodule_fsm.lua
```


Testing the rfsmGui 
-------------------
Launch the `rfsmGui`. Open the example rFSM state machine from `rfsmTools/examples/fsm/rfmodule_fsm.lua`. 

* **Running**: To execute the state machine, just press the `Start` from the `Run` menu. you can interfere the execution of the state machine by `Pausing` it or sending arbitrary events from Gui. 

* **Debuging**: In the debug mode, you can step the state machine, send arbitrary events and etc. Debugging state machine using GUI also offer the `dry-run` mode. When an state machine executed in dry-run mode, none of the `entry`, `doo` or `exit` callbacks of a state are called! To debug the state machine, just press the `Step` or `Run` from the 'Debug' menu. Stepping the state machine, consumes an event from the event queue and performs the corresponding transition. Running it, will continue stepping until there is no event left in the event queue. 


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
* [*Silvio Traversaro*](https://github.com/traversaro)
* [*Nicolò Genesio*](https://github.com/Nicogene)

