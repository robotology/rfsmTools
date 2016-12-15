#!/usr/bin/lua

require("rfsm")

RFSM = {}

--[[
function showfqn()
   local actfqn
   if fsm._actchild then
      actfqn = fsm._actchild._fqn .. '(' .. rfsm.get_sta_mode(fsm._actchild) .. ')'
   else
      actfqn = "<none>"
   end
   print("active: " .. actfqn)
end ]]--

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



--[[
local function proc_trans(gh, t, parent)
   if t.tgt == 'internal' then
      return true
   else
      print("new_tr(gh, t.src._fqn, t.tgt._fqn, t.events)")
   end
end
--]]

fsm_model = rfsm.load("rfmodule_fsm.lua")
fsm = rfsm.init(fsm_model)

setStateCallbacks("root.Configure")
setStateCallbacks("root.Configure.Software")

--rfsm.mapfsm(function (t, p) proc_trans(gh, t, p) end, root, rfsm.is_trans)
showfqn()
rfsm.run(fsm)
showfqn()
rfsm.send_events(fsm, 'e_true')
showfqn()
rfsm.run(fsm)
showfqn()



