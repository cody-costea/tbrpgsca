/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "suit.hpp"

using namespace tbrpgsca;


/*Suit::Suit(int const id, QString& name, QString& sprite, bool const shapeShift, int const element, int blockedSkills, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
           int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun, bool const range, bool const counters,
           bool const automate, bool const confuse, bool const reflect, bool const ko, bool const invincible, bool const revive, CmpsPtr<QMap<CmpsPtr<const State>, int>> const states)*/
Suit::Suit(int const id, QString& name, QString& sprite, bool const shapeShift, int const element, int blockedSkills, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
           int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun, bool const range, bool const counters, bool const automate,
           bool const confuse, bool const reflect, bool const ko, bool const invincible, bool const revive, CmpsPtr<QMap<CmpsPtr<const State>, int>> const states)
    : Role(id, name, sprite, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, element, range, revive, states)
{
    this->_atk = atk;
    this->_def = def;
    this->_spi = spi;
    this->_wis = wis;
    this->_agi = agi;
    //this->_res = res;
    //this->_st_res = stRes;
    //this->_a_skills = skills;
    this->_b_skill_types = blockedSkills;
    int flags = this->_play_flags;
    if (stun)
    {
        flags |= FLAG_STUN;
    }
    if (counters)
    {
        flags |= FLAG_COUNTER;
    }
    if (shapeShift)
    {
        QString* spr = this->_sprite;
        if (spr && spr->length() > 0)
        {
            flags |= FLAG_SHAPE_SHIFT;
        }
    }
    if (automate)
    {
        flags |= FLAG_ENRAGED;
    }
    if (confuse)
    {
        flags |= FLAG_CONFUSE;
    }
    if (reflect)
    {
        flags |= FLAG_REFLECT;
    }
    if (invincible)
    {
        flags |= FLAG_INVINCIBLE;
    }
    if (ko)
    {
        flags |= FLAG_KO;
    }
    this->_play_flags = flags;
}

Suit::Suit(const Suit& costume) : Role(costume)
{
    this->_b_skill_types = costume._b_skill_types;
    this->_atk = costume._atk;
    this->_def = costume._def;
    this->_spi = costume._spi;
    this->_wis = costume._wis;
    this->_agi = costume._agi;
}
