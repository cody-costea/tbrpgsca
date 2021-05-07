/*
Copyright (C) AD 2013-2021 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
module tbrpgsca.costume;

import tbrpgsca.ability;
import tbrpgsca.ailment;
import tbrpgsca.role;
import tbrpgsca.play;

import std.container.array : Array;
import std.bitmanip;

mixin template CostumeSheet()
{
    static if (USE_BITFIELDS)
    {
        package mixin(bitfields!(
            Nr, "_spi", 16,
            Nr, "_wis", 16
        ));

        package mixin(bitfields!(
            Nr, "_def", 16,
            Nr, "_atk", 16
        ));

        package mixin(bitfields!(
            Nr, "_agi", 16,
            Nr, "_b_skills", 16
        ));
    }
    else
    {
        package Nr _spi = void;
        package Nr _wis = void;
        package Nr _agi = void;
        package Nr _atk = void;
        package Nr _def = void;
        package Nr _b_skills = void;
    }
}

struct Costume
{
    mixin RoleSheet;
    mixin CostumeSheet;
    mixin ResistanceSheet;
    package Ailment[] _states = void;
    package Ability*[] _skills = void;
}
