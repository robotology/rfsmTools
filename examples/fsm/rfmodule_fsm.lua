--
-- Copyright (C) 2015 iCub Facility
-- Authors: Nicolò Genesio
-- CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
--


return rfsm.state {


    --States

    --Configure
    Configure = rfsm.state{
        entry = function()
            print("entry() of Configure (hello from Lua)")
        end,
        exit  = function()
            print("exit() of Configure (hello from Lua)")
        end,

        initial = rfsm.conn{ },

        --Software
        Software = rfsm.state{

            initial = rfsm.conn{ },

            --Initialize
            Initialize = rfsm.state{
            }, --end of Initialize


            --Run
            Run = rfsm.state{
            }, --end of Run

        }, --end of Software


        --Hardware
        Hardware = rfsm.state{
        }, --end of Hardware

    }, --end of Configure


    initial = rfsm.conn{ },

    --Phase
    Phase = rfsm.state{

        initial = rfsm.conn{ },

        --Phase1
        Phase1 = rfsm.state{
                entry = function()
                    print("entry() of Phase1 (hello from Lua)")
                end,
        }, --end of Phase1


        --Phase2
        Phase2 = rfsm.state{
        }, --end of Phase2

    }, --end of Phase


    --UpdateModule
    UpdateModule = rfsm.state{
        entry = function()
            print("entry() of UpdateModule (hello from Lua)")
        end,
        doo  = function()
            print("doo() of UpdateModule (hello from Lua)")
        end,
        exit = function()
            print("exit() of UpdateModule (hello from Lua)")
        end,
    }, --end of UpdateModule



    --Transitions
    rfsm.trans{ src = 'initial', tgt = 'Configure.initial', pn = 0 },
    rfsm.trans{ src = 'Configure', tgt = 'UpdateModule', pn = 10, events = {"e_true"} },
    rfsm.trans{ src = 'Configure', tgt = 'Phase.initial', pn = 0, events = {"e_true"} },
    rfsm.trans{ src = 'Configure.initial', tgt = 'Configure.Software.initial', pn = 0 },
    rfsm.trans{ src = 'Configure.Software', tgt = 'Configure.Hardware', pn = 0, events = {"e_software_done"} },
    rfsm.trans{ src = 'Configure.Software.initial', tgt = 'Configure.Software.Initialize', pn = 0 },
    rfsm.trans{ src = 'Configure.Software.Initialize', tgt = 'Configure.Software.Run', pn = 0 },
    rfsm.trans{ src = 'Phase.initial', tgt = 'Phase.Phase1', pn = 0 },
    rfsm.trans{ src = 'Phase.Phase1', tgt = 'Phase.Phase2', pn = 0 },
    rfsm.trans{ src = 'Configure.Hardware', tgt = 'Phase.Phase1', pn = 0 },
}
