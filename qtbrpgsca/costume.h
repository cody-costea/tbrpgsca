/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef COSTUME_H
#define COSTUME_H

#include "ability.h"
#include "role.h"

#include <QMap>

namespace tbrpgsca
{
    class Costume : public Role
    {
        Q_OBJECT
        PROP_FLAG_GET(isStunned, Attribute::Stun, public)
        PROP_FLAG_GET(isKnockedOut, Attribute::Ko, public)
        PROP_FLAG_GET(isEnraged, Attribute::Enraged, public)
        PROP_FLAG_GET(isConfused, Attribute::Confuse, public)
        PROP_FLAG_GET(isCovering, Attribute::Covering, public)
        PROP_FLAG_GET(isConverted, Attribute::Convert, public)
        PROP_FLAG_GET(isReflecting, Attribute::Reflect, public)
        PROP_FLAG_GET(isInvincible, Attribute::Invincible, public)
        PROP_FLAG_GET(isShapeShifted, Attribute::ShapeShift, public)
        PROP_FIELD_GET_CUSTOM(maxActions, int, public, _costume_data->_m_actions)
        PROP_FIELD_GET_CUSTOM(coverDmgType, int, public, _costume_data->_cvr_type)
        PROP_FIELD_GET_CUSTOM(reflectDmgType, int, public, _costume_data->_rfl_type)
        PROP_FIELD_GET_CUSTOM(offense, int, public, _costume_data->_atk)
        PROP_FIELD_GET_CUSTOM(defense, int, public, _costume_data->_def)
        PROP_FIELD_GET_CUSTOM(spirit, int, public, _costume_data->_spi)
        PROP_FIELD_GET_CUSTOM(wisdom, int, public, _costume_data->_wis)
        PROP_FIELD_GET_CUSTOM(agility, int, public, _costume_data->_agi)
    public:
        enum Attribute {
            Enraged = 4,
            Confuse = 8,
            Convert = 16,
            Reflect = 32,
            ShapeShift = 64,
            Invincible = 128,
            Covering = 1024,
            Stun = 256,
            Ko = 512
        };
        Q_DECLARE_FLAGS(Attributes, Attribute)
        Q_FLAG(Attributes)

        static QString CausesTxt;

        bool isCountering() const;

        int elementResistance(int const element) const;
        int stateResistance(State* const state) const;

        Ability& addedSkill(int const n) const;
        bool hasAddedSkill(Ability& skill) const;
        int addedSkillsSize() const;

        Ability& counterSkill(int const n) const;
        bool hasCounterSkill(Ability& skill) const;
        int counterSkillsSize() const;

        void adopt(QString& ret, Actor& actor);
        void abandon(QString& ret, Actor& actor);
        void apply(QString& ret, Actor& actor);

        Costume(int const id, QString name, QString sprite, bool const shapeShift, int const mActions, int const element, int const hpDmg, int const mpDmg, int const spDmg,
                int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const range, bool const automate,
                bool const confuse, bool const reflect, bool const invincible, bool const revive, QVector<Ability*>* const skills, QVector<Ability*>* const counters,
                QMap<State*, int>* const states, QMap<State*, int>* const stRes, QMap<int, int>* const res);

        Costume(Costume& costume);

        ~Costume();
    protected:
        class CostumeData : public QSharedData
        {
        public:
            ~CostumeData();

        protected:
            int _atk, _def, _spi, _wis, _agi, _m_actions, _cvr_type, _rfl_type;
            QVector<Ability*>* _a_skills,* _counters;
            QMap<State*, int>* _st_res;
            QMap<int, int>* _res;

            friend class Costume;
            friend class Ability;
            friend class SkillsModel;
            friend class DemoLib;
            friend class Actor;
            friend class State;
            friend class Scene;
            friend class Role;
        };

        QSharedDataPointer<CostumeData> _costume_data;

        void apply(QString& ret, Scene* const scene, Actor& actor);
        void refresh(QString* const ret, Scene* const scene, Actor& actor, bool const updStates, bool const remove);
        void adopt(QString* const ret, Scene* const scene, Actor& actor, bool const upeStates, bool const rmeove);

        Costume(int const id, QString& name, QString& sprite, bool const shapeShift, int const mActions, int const element, int const hpDmg, int const mpDmg, int const spDmg,
                int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun, bool const range,
                bool const automate, bool const confuse, bool const reflect, bool const ko, bool const invincible, bool const revive, QVector<Ability*>* const skills,
                QVector<Ability*>* const counters, QMap<State*, int>* const states, QMap<State*, int>* const stRes, QMap<int, int>* const res);

        friend class Actor;
        friend class Ability;
        friend class DemoLib;
        friend class State;
        friend class Scene;
    };
}

Q_DECLARE_OPERATORS_FOR_FLAGS(tbrpgsca::Costume::Attributes)

#endif // COSTUME_H
