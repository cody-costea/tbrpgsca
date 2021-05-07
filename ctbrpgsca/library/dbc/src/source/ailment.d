/*
Copyright (C) AD 2013-2021 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
module tbrpgsca.ailment;

import tbrpgsca.costume;
import tbrpgsca.role;
import tbrpgsca.play;

import std.bitmanip;

struct Ailment
{
    static if (USE_BITFIELDS)
    {
        package mixin(bitfields!(
            Nr, "_def_res", 16,
            Nr, "_dur", 16
        ));
    }
    else
    {
        package Nr _def_res = void;
        package Nr _dur = void;
    }

    package Costume* _costume;

    public ref Costume costume()
    {
        return *(this._costume);
    }

    alias costume this;
}

