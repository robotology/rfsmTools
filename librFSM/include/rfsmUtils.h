// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2016 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef RFSM_UTILS_H
#define RFSM_UTILS_H

#include <lua.hpp>

//#include <yarp/os/LogStream.h>

namespace rfsm {
    class Utils;
    class LuaTraceCallback;
}

#ifndef LUA_OK
    #define LUA_OK      0
#endif

#ifndef yError
    #include <iostream>
    #include <assert.h>
    #define yError()    std::cerr<<"[ERROR]"
    #define yWarning()  std::cout<<"[WARNING]"
    #define yInfo()     std::cout<<"[INFO]"
	#define yDebug()    std::cout<<"[DEBUG]"
    #define yAssert(x)  assert(x)
    #define ENDL        std::endl
#else
    #define ENDL        ""
#endif

#define EVENT_RETREIVE_CHUNK \
"function rfsm_get_all_events()\n"\
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
"function rfsm_set_state_callbacks(name)\n"\
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
"function rfsm_get_current_state()\n"\
"    function get_active_state(node)\n"\
"      if node._actchild._actchild == nil then return node._actchild._fqn end\n"\
"      return get_active_state(node._actchild)\n"\
"    end\n"\
"    if fsm == nil then return '<none>' end\n"\
"    if not fsm._actchild then return '<none>' end\n"\
"    return get_active_state(fsm)\n"\
"end"


#define GET_ALL_STATES_CHUNK \
"function rfsm_get_all_states()\n"\
"    local nodes = {}\n"\
"    local function getFunctionInfo(func)\n"\
"         if func == nil then\n"\
"            return -1, -1, ''\n"\
"         end\n"\
"         local dbg = debug.getinfo(func)\n"\
"         assert(dbg)\n"\
"         return dbg.linedefined, dbg.lastlinedefined, dbg.source:sub(2)\n"\
"    end\n"\
"    local function proc_node(node)\n"\
"       local node_type = ''\n"\
"       if rfsm.is_composite(node) then node_type='composit'\n"\
"       elseif rfsm.is_leaf(node)  then node_type='single'\n"\
"       elseif rfsm.is_conn(node)  then node_type='connector'\n"\
"       else node_type='unknown' end\n"\
"        local el1, el2, efn = getFunctionInfo(node.entry)\n"\
"        local dl1, dl2, dfn = getFunctionInfo(node.doo)\n"\
"        local xl1, xl2, xfn = getFunctionInfo(node.exit)\n"\
"       table.insert(nodes, {sname=node._fqn, stype=node_type, sentry_l1=el1, sentry_l2=el2, sentry_filename=efn, sdoo_l1=dl1, sdoo_l2=dl2, sdoo_filename=dfn, sexit_l1=xl1, sexit_l2=xl2, sexit_filename=xfn })\n"\
"    end\n"\
"   rfsm.mapfsm(function (s)\n"\
"		  if rfsm.is_root(s) then return end\n"\
"		  proc_node(s)\n"\
"	       end, fsm, rfsm.is_node)\n"\
"   return nodes\n"\
"end"

#define GET_ALL_TRANSITIONS_CHUNK \
"function rfsm_get_all_transitions()\n"\
"    local trans = {}\n"\
"    local function proc_trans(t, parent)\n"\
"       if t.tgt == 'internal' then return true\n"\
"       else\n"\
"          local str_events = ''\n"\
"          if t.events then str_events = table.concat(t.events, ',') end\n"\
"          table.insert(trans, {source=t.src._fqn, target=t.tgt._fqn, events=str_events})\n"\
"       end\n"\
"    end\n"\
"    rfsm.mapfsm(function (t, p) proc_trans(t, p) end, fsm, rfsm.is_trans)\n"\
"   return trans\n"\
"end"

#define GET_EVET_QUEUE_CHUNK \
"function rfsm_get_event_queue()\n"\
"   rfsm.check_events(fsm)\n"\
"   return fsm._intq\n"\
"end"\

#define RFSM_NULL_FUNCTION_CHUNK \
"function rfsm_null_func() return end\n"

#define PRE_STEP_HOOK_CHUNK \
"function rfsm_pre_step_hook() RFSM.preStepCallback() end\n"

#define POST_STEP_HOOK_CHUNK \
"function rfsm_post_step_hook() RFSM.postStepCallback() end\n"

#if LUA_VERSION_NUM > 501
#define RFSM_PACK_ARGS_CHUNK \
"function rfsm_pack_args(...)\n"\
"  res = ''\n"\
"  local arguments=arg \n"\
"  if arguments==nil then arguments = table.pack(...) end\n"\
"  for i,v in ipairs(arguments) do\n"\
"    res = res .. tostring(v) .. ' '\n"\
"  end\n"\
"  return res\n"\
"end"
#else
#define RFSM_PACK_ARGS_CHUNK \
"function rfsm_pack_args(...)\n"\
"  res = ''\n"\
"  for i,v in ipairs(arg) do\n"\
"    res = res .. tostring(v) .. ' '\n"\
"  end\n"\
"  return res\n"\
"end"

#endif

#define RFSM_WARNING_CHUNK \
"function rfsm_warning(...)\n"\
"  RFSM.warningCallback(rfsm_pack_args(...))\n"\
"end"

#define RFSM_ERROR_CHUNK \
"function rfsm_error(...)\n"\
"  RFSM.errorCallback(rfsm_pack_args(...))\n"\
"end"

#define RFSM_INFO_CHUNK \
"function rfsm_info(...)\n"\
"  RFSM.infoCallback(rfsm_pack_args(...))\n"\
"end"


class rfsm::Utils {
public:
    static int report (lua_State *L, int status);
    static int traceback (lua_State *L);
    static int docall(lua_State *L, int narg, int clear);
    static int dofile(lua_State *L, const char *name);
    static int dostring (lua_State *L, const char *s, const char *name);
    static int dolibrary (lua_State *L, const char *name);
    static int getTableNumberField(lua_State *L, const char *key);
    static std::string getTableStringField(lua_State *L, const char *key);    
    static bool isNilTableField(lua_State *L, const char *key);    
    static void setLuaTraceCallback(LuaTraceCallback* callback);

private:
    static LuaTraceCallback* traceCallback;
};


#endif // RFSM_UTILS_H
