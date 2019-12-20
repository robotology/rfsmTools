

[![Build Status (Linux) (master branch)](https://img.shields.io/travis/com/robotology/rfsmTools/master.svg?logo=travis&label=[master]%20build%20(Linux))](https://travis-ci.com/robotology/rfsmTools)
[![Build Status (macOs) (master branch)](https://img.shields.io/travis/com/robotology/rfsmTools/master.svg?logo=travis&label=[master]%20build%20(macOs))](https://travis-ci.com/robotology/rfsmTools)


# rFSM Tools

This repository contains a C++ library (*librFSM*) to load and execute rFSM LUA-based state machines and a graphical tools (*rfsmGui*) to create, debug and execute the state machine.

![scenario2](/doc/rfsmGui.png)

Features
--------
:tada: Compatible with Lua 5.1 and 5.2  
:tada: Gui built with Qt  
:tada: Multiplatform compatibilty(Linux, Windows, macOS)  
:tada: User friendly, easy to install, easy to use  
:tada: Few dependencies  
:tada: Includes debugger for rFSM-based state machines  
:tada: Includes builder for editing an existing state machine or build one from scratch  

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
* If you do not want to use the built-in rfsm then follow the installation of 
rFSM on https://github.com/kmarkus/rFSM.

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

Installation on macOS
---------------------
* Update brew  
```
$ brew update
```

* Install Lua
```
$ brew install lua
```

* Install Qt5
```
$ brew install qt
$ brew link --force qt
```

* Add the following lines to your .bash_profile

```
$ export Qt5_DIR=/usr/local/opt/qt5/lib/cmake
$ export PATH=/usr/local/opt/qt5/bin:$PATH
```

* Install Graphviz
```
$ brew install graphviz
```
now you can choose to compile using GNU Makefiles or Xcode:

* GNU Makefiles
```
$ cd rfsmTools
$ mkdir build; cd build
$ cmake ../; make
```

* Xcode
```
$ ccmake .. -G Xcode
```
This will generate .xcodeproj file which can be opened with Xcode. Build it by clicking **Product -> Build** or **cmd+B** shortcut. One can also compile the *Release* biaries by clicking **Product -> Archive**.

Alternatively, it is possible to directly compile the project on the command line by
```
$ xcodebuild [-configuration Debug|Release|Other Configs] [-target ALL_BUILD|install|Other Targets]
```

Using YARP
----------
Toggle on the cmake flag `USE_YARP` to enable yarp features.


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
* **Building:** To build a new state machine simply press `New rFSM`, select the filename and then start creating your rFSM using the build toolbar on the left. Once you finish just press `Save` and your Lua code will be generated. 
* **Editing**: You can edit a pre-existing state machine in two ways, directly from the embedded source editor and graphically through the rFSMGui. In the latter case it is possible to add/remove/rename states and transitions and pressing `Save` the code will be generated in one file preserving pre-definded entry, doo, exit functions and all the code defined before `return rfsm.state{` statement. For now the "graphical editor" is enabled **ONLY** for those state machines that are defined in only one file. Later we will remove this limitation. 

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

