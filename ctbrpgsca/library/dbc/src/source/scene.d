/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
module tbrpgsca.scene;

import tbrpgsca.actor;
import tbrpgsca.ability;
import tbrpgsca.costume;
import tbrpgsca.ailment;
import tbrpgsca.role;
import tbrpgsca.play;

import std.bitmanip;

struct Scene
{
    static if (USE_BITFIELDS)
    {
        package mixin(bitfields!(
            Nr, "_current", 8,
            Nr, "_original", 8,
            Nr, "_f_target", 8,
            Nr, "_l_target", 8
        ));
        package mixin(bitfields!(
            bool, "_use_guards", 1,
            bool, "_has_covers", 1,
            Nr, "_surprise", 2,
            Nr, "_m_init", 26,
            Nr, "_status", 2
        ));
        package Ptr!Ability _l_ability = void;
    }
    else
    {
        static if (COMPRESSED_POINTERS == 0)
        {
            package mixin(taggedPointer!(
                Ability*, "_l_ability",
                bool, "_use_guards", 1,
                bool, "_has_covers", 1
            ));
        }
        else
        {
            package mixin(bitfields!(
                bool, "_use_guards", 1,
                bool, "_has_covers", 1
            ));
            package Ptr!Ability _l_ability = void;
        }
        package Nr _current = void;
        package Nr _original = void;
        package Nr _f_target = void;
        package Nr _l_target = void;
        package Nr _surprise = void;
        package Nr _status = void;
        package Nr _m_init = void;
    }
    package Actor*[][] _parties = void;
    package Actor*[] _players = void;
}
