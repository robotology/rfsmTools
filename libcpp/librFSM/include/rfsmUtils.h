// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef RFSM_UTILS_H
#define RFSM_UTILS_H

#include <lua.hpp>

#include <yarp/os/LogStream.h>

namespace rfsm {
    class Utils;
}

#ifndef LUA_OK
    #define LUA_OK      0
#endif

#ifndef yError
    #include <iostream>
    #include <assert.h>
    #define yError()    std::cerr<<"[ERROR]"
    #define yWarning()      std::cout<<"[WARNING]"
    #define yInfo()     std::cout<<"[INFO]"
    #define yAssert(x)  assert(x)
    #define ENDL        std::endl
#else
    #define ENDL        ""
#endif

#define EVENT_RETREIVE_CHUNK \
"function get_all_events()\n"\
"    local known_events = { e_init_fsm=true, }\n"\
"    rfsm.mapfsm(function(t)\n"\
"           local events = t.events or {}\n"\
"           for _,e in ipairs(events) do\n"\
"              known_events[e] = true\n"\
"           end\n"\
"            end, fsm, rfsm.is_trans)\n"\
"    local a = {}\n"\
"    for k, v in pairs(known_events) do\n"\
"       if string.find(k, 'e_done@') == nil then table.insert(a, k) end\n"\
"    end\n"\
"    table.sort(a)\n"\
"    return a\n"\
"end"

#define SET_STATE_CALLBACKS_CHUNK \
"function set_state_callbacks(name)\n"\
"    local found = false\n"\
"    local function proc_node(state)\n"\
"       if state._fqn == ('root.' .. name) then\n"\
"           state.entry = function() RFSM.entryCallback(name) end\n"\
"           state.doo = function() RFSM.dooCallback(name) end\n"\
"           state.exit = function() RFSM.exitCallback(name) end\n"\
"           found = true\n"\
"        end\n"\
"    end\n"\
"    rfsm.mapfsm(function (s)\n"\
"          if rfsm.is_root(s) then return end\n"\
"          if found == true then return end\n"\
"          proc_node(s)\n"\
"          end, fsm, rfsm.is_node)\n"\
"    return found\n"\
"end"

#define GET_CURRENT_STATE_CHUNK \
"function get_current_state()\n"\
"   if fsm._actchild then return fsm._actchild._fqn end\n"\
"   return '<none>'\n"\
"end"

#define GET_ALL_STATES_CHUNK \
"function get_all_states()\n"\
"    local nodes = {}\n"\
"    local function proc_node(node)\n"\
"       local node_type = ''\n"\
"       if rfsm.is_composite(node) then node_type='composit'\n"\
"       elseif rfsm.is_leaf(node)  then node_type='single'\n"\
"       elseif rfsm.is_conn(node)  then node_type='connector'\n"\
"       else node_type='unknown' end\n"\
"        table.insert(nodes, {sname=node._fqn, stype=node_type})\n"\
"    end\n"\
"   rfsm.mapfsm(function (s)\n"\
"		  if rfsm.is_root(s) then return end\n"\
"		  proc_node(s)\n"\
"	       end, fsm, rfsm.is_node)\n"\
"   return nodes\n"\
"end"


class rfsm::Utils {
public:
    static int report (lua_State *L, int status);
    static int traceback (lua_State *L);
    static int docall(lua_State *L, int narg, int clear);
    static int dofile(lua_State *L, const char *name);
    static int dostring (lua_State *L, const char *s, const char *name);
    static int dolibrary (lua_State *L, const char *name);
    static std::string getTableField(lua_State *L, const char *key);
};


#endif // RFSM_UTILS_H
