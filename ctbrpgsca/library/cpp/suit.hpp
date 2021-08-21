/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef SUIT_HPP
#define SUIT_HPP

#include "role.hpp"

namespace tbrpgsca
{

    class Suit : public Role
    {
        #define FLAG_ENRAGED 4
        #define FLAG_CONFUSE 8
        #define FLAG_CONVERT 16
        #define FLAG_COVERING 1024
        #define FLAG_SHAPE_SHIFT 64
        #define FLAG_INVINCIBLE 128
        #define FLAG_COUNTER 2048
        #define FLAG_REFLECT 32
        #define FLAG_STUN 256
        #define FLAG_KO 512

        PROP_FLAG_GET(isStunned, FLAG_STUN, public)
        PROP_FLAG_GET(isKnockedOut, FLAG_KO, public)
        PROP_FLAG_GET(isEnraged, FLAG_ENRAGED, public)
        PROP_FLAG_GET(isConfused, FLAG_CONFUSE, public)
        PROP_FLAG_GET(isCovering, FLAG_COVERING, public)
        PROP_FLAG_GET(isConverted, FLAG_CONVERT, public)
        PROP_FLAG_GET(isCountering, FLAG_COUNTER, public)
        PROP_FLAG_GET(isReflecting, FLAG_REFLECT, public)
        PROP_FLAG_GET(isInvincible, FLAG_INVINCIBLE, public)
        PROP_FLAG_GET(isShapeShifted, FLAG_SHAPE_SHIFT, public)
        //PROP_FIELD_GET_CUSTOM(maxActions, int, public, _m_actions)
        PROP_FIELD_GET_CUSTOM(offense, int, public, _atk)
        PROP_FIELD_GET_CUSTOM(defense, int, public, _def)
        PROP_FIELD_GET_CUSTOM(spirit, int, public, _spi)
        PROP_FIELD_GET_CUSTOM(wisdom, int, public, _wis)
        PROP_FIELD_GET_CUSTOM(agility, int, public, _agi)

    protected:
    #if USE_BIT_FIELDS
        signed int _atk: 16, _def: 16, _spi: 16, _wis: 16, _agi: 16, _b_skill_types: 16;
    #else
        signed int _atk, _def, _spi, _wis, _agi, _b_skill_types;
    #endif
        /*QMap<int, int>* _res;
        CmpsPtr<QMap<CmpsPtr<const State>, int>> _st_res;
        QVector<const Ability*>* _a_skills;
        CmpsPtr<QMap<CmpsPtr<const State>, int>> _state_dur;*/

        Suit(int const id, QString& name, QString& sprite, bool const shapeShift, int const element, int blockedSkills, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
             int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun, bool const range, bool const counters,
             bool const automate, bool const confuse, bool const reflect, bool const ko, bool const invincible, bool const revive, CmpsPtr<QMap<CmpsPtr<const State>, int>> const states);

        Suit(const Suit& suit);

        inline Suit() : Role() {}

        template <typename, class, const int> friend class cmpsptr::BasePtr;
        template <typename, const int, const bool, const int, typename, const int> friend class cmpsptr::BaseCnt;
        template<typename, typename, typename L, const L, const bool> friend class cmpsptr::BaseVct;
    };

}

#endif // SUIT_H
