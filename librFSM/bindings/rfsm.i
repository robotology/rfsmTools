// Copyright: 2017 iCub Facility, Istituto Italiano di Tecnologia
// Author: Silvio  Traversaro
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

/* File : rfsm.i */
%module rfsm

%include "std_string.i"
%include "std_vector.i"

// Wrap the std::vector<std::string> params
// TODO : create nice SWIG Typemaps to wrap the std::string<std::string> as:
//        * a cell array of strings in matlab
//        * a list of strings in Python
%template(StringVector) std::vector<std::string>;

%{

#include <rfsm.h>

%}

%include <rfsm.h>