--
-- Authors: ngenesio
-- Version: 1.0.0
-- Created using rFSMGui
--
return rfsm.state {


    --States

    --Configure
    Configure = rfsm.state{

        initial = rfsm.conn{ },

        --Hardware
        Hardware = rfsm.state{

            initial = rfsm.conn{ },

            --Phase1
            Phase1 = rfsm.state{
                doo = function()   end,
            }, --end of Phase1

        }, --end of Hardware


        --Software
        Software = rfsm.state{

            initial = rfsm.conn{ },

            --Phase2
            Phase2 = rfsm.state{
                doo = function()   end,
            }, --end of Phase2


            --Phase1
            Phase1 = rfsm.state{
                doo = function()   end,
            }, --end of Phase1

        }, --end of Software

    }, --end of Configure


    --Fatal_Error
    Fatal_Error = rfsm.state{
        doo = function()   end,
    }, --end of Fatal_Error


    --Run
    Run = rfsm.state{

        --Pausing
        Pausing = rfsm.state{
            doo = function()   end,
        }, --end of Pausing


        initial = rfsm.conn{ },

        --Moving
        Moving = rfsm.state{
            doo = function()   end,
        }, --end of Moving

    }, --end of Run


    initial = rfsm.conn{ },


    --Transitions
    rfsm.trans{ src = 'initial', tgt = 'Configure.initial', pn = 0 },
    rfsm.trans{ src = 'Configure.initial', tgt = 'Configure.Software.initial', pn = 1, events = {"e_init_sw"} },
    rfsm.trans{ src = 'Configure.Software.initial', tgt = 'Configure.Software.Phase1', pn = 0 },
    rfsm.trans{ src = 'Configure.Software.Phase1', tgt = 'Configure.Software.Phase2', pn = 0 },
    rfsm.trans{ src = 'Configure.Hardware.initial', tgt = 'Configure.Hardware.Phase1', pn = 0 },
    rfsm.trans{ src = 'Configure.initial', tgt = 'Configure.Hardware.initial', pn = 2, events = {"e_init_hw"} },
    rfsm.trans{ src = 'Configure', tgt = 'Fatal_Error', pn = 0, events = {"e_error"} },
    rfsm.trans{ src = 'Run.Pausing', tgt = 'Run.Moving', pn = 0, events = {"e_move"} },
    rfsm.trans{ src = 'Run.Moving', tgt = 'Run.Pausing', pn = 0, events = {"e_pause"} },
    rfsm.trans{ src = 'Run.initial', tgt = 'Run.Pausing', pn = 0 },
    rfsm.trans{ src = 'Configure', tgt = 'Run.initial', pn = 0, events = {"e_ok"} },
    rfsm.trans{ src = 'Run', tgt = 'Fatal_Error', pn = 0, events = {"e_error"} },
}
