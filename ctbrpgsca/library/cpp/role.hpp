/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef ROLE_HPP
#define ROLE_HPP

#include "play.hpp"

#include <QString>
#include <QVector>

namespace tbrpgsca
{

    class Actor;
    class State;
    class Scene;

    class Role : public Play
    {
        #define FLAG_REVIVE 1
        #define FLAG_RANGE 2
        #define DMG_TYPE_ATK 1
        #define DMG_TYPE_DEF 2
        #define DMG_TYPE_SPI 4
        #define DMG_TYPE_WIS 8
        #define DMG_TYPE_AGI 16
        #define ELEMENT_FIRE 32
        #define ELEMENT_WATER 64
        #define ELEMENT_THUNDER 128
        #define ELEMENT_EARTH 256
        #define ELEMENT_PSYCHIC 512
        #define ELEMENT_LIGHT 1024

        PROP_FLAG_GET(isRanged, FLAG_RANGE, public)
        PROP_FLAG_GET(isReviving, FLAG_REVIVE, public)
        PROP_FIELD_GET_CUSTOM(currentHp, int, public, _hp)
        PROP_FIELD_GET_CUSTOM(currentMp, int, public, _mp)
        PROP_FIELD_GET_CUSTOM(currentRp, int, public, _sp)
        PROP_FIELD_GET_CUSTOM(maximumHp, int, public, _m_hp)
        PROP_FIELD_GET_CUSTOM(maximumMp, int, public, _m_mp)
        PROP_FIELD_GET_CUSTOM(maximumRp, int, public, _m_sp)
        PROP_FIELD_GET_CUSTOM(dmgElement, int, public, _dmg_type)
        PROP_FIELD_GET_CUSTOM(name, QString, public, _name)
        //PROP_FIELD_GET_CUSTOM(id, int, public, _id)
    public:
        inline static const QString SuffersTxt = TR_TXT_SCENE_SUFFERS;
        inline static const QString ResistTxt = TR_TXT_SCENE_RESISTS;

        QString sprite() const;
        QList<const State*> statesList() const;
        int stateDuration(const State& state) const;
        bool hasState(const State& state) const;
        int statesSize() const;

        void damage(QString& ret, Actor* const user, Actor& target, int const dmg, bool const percent) const;
    protected:
        QString _name,* _sprite;
#if USE_BIT_FIELDS
        signed int _dmg_type: 16, _hp: 24, _mp: 16, _sp: 16, _m_hp: 24, _m_mp: 16, _m_sp: 16;
#else
        signed int _dmg_type, _hp, _mp, _sp, _m_hp, _m_mp, _m_sp;
#endif
        QMap<const State*, int>* _state_dur;

        template <typename SpriteRun>
        void damage(QString& ret, Scene* const scene, const SpriteRun* const spriteRun, Actor* const absorber, Actor& target, int dmg, bool const percent) const;

        Role(int const id, QString& name, QString& sprite, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
             int const mMp, int const mSp, int const element, bool const range, bool const revive, QMap<const State*, int>* const states);

        Role(const Role& role);

        ~Role();

        friend class Actor;
        friend class Ability;
        friend class Costume;
        friend class State;
        friend class Scene;
    };

}
#endif // ROLE_HPP
