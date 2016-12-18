# rFSMSimulator
A C++ library to load an execute rFSM LUA-based state machines

Dependencies 
------------
* Lua5.1 
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

