--
-- Copyright (C) 2015 iCub Facility
-- Authors: Nicolò Genesio
-- CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
--


return rfsm.state {  
    Configure = rfsm.sista{
        entry = function()            
            RFSM.entryCallback("Configure")
        end,
    },

    UpdateModule = rfsm.sista{
        entry = function()
            print("UpdateModule")
        end,
    },

    Close = rfsm.sista{
        entry = function()
            print("Close")
        end,
    },

    InterruptModule = rfsm.sista{
        entry = function()
            print("InterruptModule")
        end,
    },


    rfsm.transition { src='initial', tgt='Configure' },
    rfsm.transition { src='Configure', tgt='UpdateModule', events={ 'e_true' } },
    rfsm.transition { src='Configure', tgt='Close', events={ 'e_false' } },
    rfsm.transition { src='Configure', tgt='InterruptModule', events={ 'e_stopModule','e_interrupt' } },
    rfsm.transition { src='UpdateModule', tgt='UpdateModule', events={ 'e_true' } },
    rfsm.transition { src='UpdateModule', tgt='Close', events={ 'e_false' } },
    rfsm.transition { src='UpdateModule', tgt='InterruptModule', events={ 'e_stopModule','e_interrupt' } },
    rfsm.transition { src='InterruptModule', tgt='Close' },
}

