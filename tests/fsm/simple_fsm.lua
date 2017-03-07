--
-- Copyright (C) 2017 iCub Facility
-- Authors: Ali Paikan
-- CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
--


return rfsm.state {
    STATE1 = rfsm.state {
		},

    STATE2 = rfsm.csta {
		},

    STATE3 = rfsm.sista {
		},

    rfsm.transition { src='initial', tgt='STATE1' },
    rfsm.transition { src='STATE1', tgt='STATE2', events={ 'e_one', 'e_two'} },
    rfsm.transition { src='STATE1', tgt='STATE3', events={ 'e_three'} },
}
