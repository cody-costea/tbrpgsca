/*
Copyright (C) AD 2013-2021 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
module tbrpgsca.actor;

import tbrpgsca.costume;
import tbrpgsca.ailment;
import tbrpgsca.ability;
import tbrpgsca.role;
import tbrpgsca.play;

import std.container.array : Array;
import std.bitmanip;

struct Actor
{
    mixin RoleSheet;
    mixin CostumeSheet;
    package Array!AilmentRes _st_res = void;
    package Array!ElementRes _elm_res = void;
    package Array!Ailment* _states = void;
    package Array!Ability* _skills = void;
}
