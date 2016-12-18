#!/usr/bin/lua

require("rfsm")

RFSM = {}



function get_current_state()
   if fsm._actchild then return fsm._actchild._fqn end
   return "<none>"   
end


function RFSM.entryCallback(name) 
    print(name, "in myEntry")
end

function RFSM.dooCallback(name) 
    print(name, "in myDoo")
end

function RFSM.exitCallback(name) 
    print(name, "in myExit")
end



function setStateCallbacks(name)
    local found = false    
    local function proc_node(state)
       if state._fqn == name then
           state.entry = function() RFSM.entryCallback(name) end
           state.doo = function() RFSM.dooCallback(name) end
           state.exit = function() RFSM.exitCallback(name) end
           found = true
        end
    end
    rfsm.mapfsm(function (s)
	      if rfsm.is_root(s) then return end
	      if found == true then return end
	      proc_node(s)
          end, fsm, rfsm.is_node)
end


function get_states()
    local nodes = {}
    local function proc_node(node)
       local node_type = ""
       if rfsm.is_composite(node) then node_type="composit"
       elseif rfsm.is_leaf(node)  then node_type="single"
       elseif rfsm.is_conn(node)  then node_type="connector"
       else node_type="unknown" end
        table.insert(nodes, {sname=node._fqn, stype=node_type})
    end   
   rfsm.mapfsm(function (s)
		  if rfsm.is_root(s) then return end
		  proc_node(s)
	       end, fsm, rfsm.is_node)
   return nodes
end

function get_transitions()
    local trans = {}
    local function proc_trans(t, parent)
       if t.tgt == 'internal' then return true
       else
          local str_events = ""
          if t.events then str_events = table.concat(t.events, ',') end
          table.insert(trans, {source=t.src._fqn, target=t.tgt._fqn, events=str_events})
       end
    end
    rfsm.mapfsm(function (t, p) proc_trans(t, p) end, fsm, rfsm.is_trans)
   return trans
end

function get_event_queue()
   rfsm.check_events(fsm)
   return fsm._intq
end

function onPreStep()
    print("onPreStep()")
end

function onPostStep()
    print("onPostStep()")
end

fsm_model = rfsm.load("rfmodule_fsm.lua")
fsm = rfsm.init(fsm_model)

rfsm.pre_step_hook_add(fsm, onPreStep)
rfsm.post_step_hook_add(fsm, onPostStep)
rfsm.run(fsm)

local states = get_states()
for key,value in pairs(states) do 
    print(value.sname, value.stype)
end

local trans = get_transitions()
for key,value in pairs(trans) do 
    print(value.source, value.target, value.events)
end

rfsm.send_events(fsm, 'e_true')
q = get_event_queue()
for key,value in pairs(q) do
    print(value)
end

--[[
setStateCallbacks("root.Configure")
setStateCallbacks("root.Configure.Software")
showfqn()
rfsm.run(fsm)
showfqn()
rfsm.send_events(fsm, 'e_true')
showfqn()
rfsm.run(fsm)
showfqn()
--]]


