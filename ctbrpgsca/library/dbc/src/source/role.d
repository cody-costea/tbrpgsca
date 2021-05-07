/*
Copyright (C) AD 2013-2021 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
module tbrpgsca.role;

import std.bitmanip;
//import std.meta : AliasSeq;
import std.container.array : Array;
import std.typecons : BitFlags;

import tbrpgsca.ailment;
import tbrpgsca.play;

/*alias RoleBitVars = AliasSeq!(
    Nr, "c_hp", 12,
    Nr, "c_mp", 10,
    Nr, "c_sp", 10,
    Nr, "m_hp", 12,
    Nr, "m_mp", 10,
    Nr, "m_sp", 10
);*/

struct ElementRes
{
    static if (USE_BITFIELDS)
    {
        package mixin(bitfields!(
            Element, "type", 16,
            Nr, "value", 16
        ));
    }
    else
    {
        package Element type = void;
        package Nr value = void;
    }
    
    this(const Element type, const int value)
    {
        this.value = value;
        this.type = type;
    }

    @disable this();
}

struct AilmentRes
{
    package Ailment* ailment = void;
    package Nr value = void;

    this(ref Ailment ailment, const int value)
    {
        this.ailment = &ailment;
        this.value = value;
    }

    @disable this();
}

mixin template RoleSheet()
{
    static if (USE_BITFIELDS)
    {
        package mixin(bitfields!(
            Nr, "_c_hp", 12,
            Nr, "_c_mp", 10,
            Nr, "_c_sp", 10
        ));
        package mixin(bitfields!(
            Nr, "_m_hp", 12,
            Nr, "_m_mp", 10,
            Nr, "_m_sp", 10
        ));
        package mixin(bitfields!(
            Element, "_dmg_type", 12,
            Nr, "_flags", 20
        ));
    }
    else
    {
        package Nr _c_hp = void;
        package Nr _c_mp = void;
        package Nr _c_sp = void;
        package Nr _m_hp = void;
        package Nr _m_mp = void;
        package Nr _m_sp = void;
        package Nr _dmg_type = void;
        package Nr _flags = void;
    }
    
    package Txt _sprite = void;
    package Txt _name = void;

    pragma(inline, true):
    @property public Nr cHp()
    {
        return this._c_hp;
    }

    pragma(inline, true):
    public Nr cMp()
    {
        return this._c_mp;
    }

    pragma(inline, true):
    public Nr cRp()
    {
        return this._c_sp;
    }

    pragma(inline, true):
    public Nr mHp()
    {
        return this._m_hp;
    }

    pragma(inline, true):
    public Nr mMp()
    {
        return this._m_mp;
    }

    pragma(inline, true):
    public Nr mRp()
    {
        return this._m_sp;
    }

    pragma(inline, true):
    public Nr dmgType()
    {
        return this._dmg_type;
    }    

    pragma(inline, true):
    public Nr flags()
    {
        return this._flags;
    }
}

mixin template ResistanceSheet()
{
    package ElementRes[] _elm_res = void;
    package AilmentRes[] _st_res = void;
    
    pragma(inline, true):
    public ElementRes[] elmRes()
    {
        return this._elm_res;
    }

    pragma(inline, true):
    public AilmentRes[] stRes()
    {
        return this._st_res;
    }
}
