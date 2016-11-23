#!/bin/bash

#if [[ "$(uname -m)" == "x86_64" ]]; then ARCH="x64"; else ARCH="x86"; fi
ROOT=`pwd`
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOT/lib/linux/x64/clibs51:$ROOT/lib/linux/x64/clibs52:$ROOT/lib/linux/x64/clibs53
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOT/lib/linux/x86/clibs51:$ROOT/lib/linux/x86/clibs52:$ROOT/lib/linux/x86/clibs53
(cd "./"; lua src/rFSMSimulator.lua rfsmsim "$@") &
