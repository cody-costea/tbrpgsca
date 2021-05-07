/*
Copyright (C) AD 2013-2021 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
module tbrpgsca.ability;

import tbrpgsca.ailment;
import tbrpgsca.role;
import tbrpgsca.play;

import std.bitmanip;

mixin template AbilitySheet()
{
    static if (USE_BITFIELDS)
    {
        package mixin(bitfields!(
            Nr, "_lv_rq", 8,
            Nr, "_m_qty", 12,
            Nr, "_r_qty", 12
        ));
    }
    else
    {
        package Nr _lv_rq = void;
        package Nr _m_qty = void;
        package Nr _r_qty = void;
    }
}

struct Ability
{
    mixin AbilitySheet;
    package Ailment[] _a_states = void;
    package Ailment[] _r_states = void;
    package Txt _sound = void;
}

struct Skill
{
    static if (USE_BITFIELDS)
    {
        package mixin(bitfields!(
            Nr, "_c_qty", 16,
            Nr, "_r_trn", 16
        ));
    }
    else
    {
        package Nr _c_qty = void;
        package Nr _r_trn = void;
    }
    
    package Ptr!Ability _ability = void;
    
    public ref Ability ability()
    {
        return *(this._ability);
    }

    alias ability this;
}
