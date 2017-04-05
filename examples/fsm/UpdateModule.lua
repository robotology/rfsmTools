return rfsm.state{
        entry = function()
            print("entry() of UpdateModule (hello from Lua)")
        end,
        doo  = function()
            print("doo() of UpdateModule (hello from Lua)")
        end,
        exit = function()
            print("exit() of UpdateModule (hello from Lua)")
        end,
    } --end of UpdateModule
