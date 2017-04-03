--
-- Copyright (C) 2015 iCub Facility
-- Authors: Nicolò Genesio
-- CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
--


return rfsm.state {


    --States

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


    initial = rfsm.conn{ },

    --Configure
    Configure = rfsm.state{
        entry = function()
            print("entry() of Configure (hello from Lua)")
        end,
        exit  = function()
            print("exit() of Configure (hello from Lua)")
        end,

        initial = rfsm.conn{ },

        --Hardware
        Hardware = rfsm.state{
        }, --end of Hardware


        --Software
        Software = rfsm.state{

            initial = rfsm.conn{ },

            --Run
            Run = rfsm.state{
            }, --end of Run


            --Initialize
            Initialize = rfsm.state{
            }, --end of Initialize

        }, --end of Software

    }, --end of Configure


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



    --Transitions
    rfsm.trans{ src = 'initial', tgt = 'Configure.initial' },
    rfsm.trans{ src = 'Configure', tgt = 'UpdateModule', events = {"e_true"}, pn = 10 },
    rfsm.trans{ src = 'Configure', tgt = 'Phase.initial', events = {"e_true"} },
    rfsm.trans{ src = 'Configure.initial', tgt = 'Configure.Software.initial' },
    rfsm.trans{ src = 'Configure.Software', tgt = 'Configure.Hardware', events = {"e_software_done"} },
    rfsm.trans{ src = 'Configure.Software.initial', tgt = 'Configure.Software.Initialize' },
    rfsm.trans{ src = 'Configure.Software.Initialize', tgt = 'Configure.Software.Run' },
    rfsm.trans{ src = 'Phase.initial', tgt = 'Phase.Phase1' },
    rfsm.trans{ src = 'Phase.Phase1', tgt = 'Phase.Phase2', pn =5
 },
}
