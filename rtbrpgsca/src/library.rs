/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
use crate::actor::*;
use crate::ability::*;
use crate::costume::*;
use crate::state::*;
use crate::role::*;

pub trait Library {
    
    const STATES: &'static [&'static State<'static>] = &[

    ];

    const SKILLS: &'static [&'static Ability<'static>] = &[

    ];

    const ITEMS: &'static [&'static Ability<'static>] = &[

    ];

    const RACES: &'static [&'static Costume<'static>] = &[

    ];
    
    const JOBS: &'static [&'static Costume<'static>] = &[

    ];

    const PLAYERS: &'static [&'static mut Actor<'static>] = &[

    ];

    const ENEMIES: &'static [&'static mut [&'static mut Actor<'static>]] = &[

    ];

}