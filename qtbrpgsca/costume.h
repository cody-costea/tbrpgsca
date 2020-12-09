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
        QML_ELEMENT
        PROP_FLAG(Costume, Stunned, stunned, Attribute::STUN, inline, public, public)
        PROP_FLAG(Costume, Enraged, enraged, Attribute::ENRAGED, inline, public, public)
        PROP_FLAG(Costume, KnockedOut, knockedOut, Attribute::KO, inline, public, public)
        PROP_FLAG(Costume, Covering, covers, Attribute::COVERING, inline, public, public)
        PROP_FLAG(Costume, Confused, confused, Attribute::CONFUSE, inline, public, public)
        PROP_FLAG(Costume, Converted, converted, Attribute::CONVERT, inline, public, public)
        PROP_FLAG(Costume, Reflecting, reflects, Attribute::REFLECT, inline, public, public)
        PROP_FLAG(Costume, Invincible, invincible, Attribute::INVINCIBLE, inline, public, public)
        PROP_FLAG(Costume, ShapeShifted, shapeShifted, Attribute::SHAPE_SHIFT, inline, public, public)
        PROP_FIELD(Costume, MaximumActions, maximumActions, int, virtual inline, public, public, _costume_data->_m_actions)
        PROP_FIELD(Costume, ReflectType, reflectType, int, inline, public, public, _costume_data->_rfl_type)
        PROP_FIELD(Costume, CoverType, coverType, int, inline, public, public, _costume_data->_cvr_type)
        PROP_FIELD(Costume, Agility, agility, int, inline, public, public, _costume_data->_agi)
        PROP_FIELD(Costume, Offense, offense, int, inline, public, public, _costume_data->_atk)
        PROP_FIELD(Costume, Defense, defense, int, inline, public, public, _costume_data->_def)
        PROP_FIELD(Costume, Wisdom, wisdom, int, inline, public, public, _costume_data->_wis)
        PROP_FIELD(Costume, Spirit, spirit, int, inline, public, public, _costume_data->_spi)
    public:
        enum Attribute {
            ENRAGED = 4,
            CONFUSE = 8,
            CONVERT = 16,
            REFLECT = 32,
            SHAPE_SHIFT = 64,
            INVINCIBLE = 128,
            COVERING = 1024,
            STUN = 256,
            KO = 512
        };
        Q_DECLARE_FLAGS(Attributes, Attribute)
        Q_FLAG(Attributes)

        static QString CausesTxt;

        bool isCountering() const;

        int elementResistance(int const element) const;
        int stateResistance(const State* const state) const;

        Ability& addedSkill(int const n) const;
        bool hasAddedSkill(Ability& skill) const;
        int addedSkillsSize() const;

        Ability& counterSkill(int const n) const;
        bool hasCounterSkill(Ability& skill) const;
        int counterSkillsSize() const;

        void adopt(QString& ret, Actor& actor) const;
        void abandon(QString& ret, Actor& actor) const;
        void apply(QString& ret, Actor& actor) const;

        Costume(int const id, QString& name, QString& sprite, bool const shapeShift, int const mActions, int const element, int const hpDmg, int const mpDmg, int const spDmg,
                int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const range, bool const automate,
                bool const confuse, bool const reflect, bool const invincible, bool const revive, QList<Ability*>* const skills, QList<Ability*>* const counters,
                QMap<State*, int>* const states, QMap<const State*, int>* const stRes, QMap<int, int>* const res, QObject* const parent = NIL);

        Costume(int const id, QString&& name, QString&& sprite, bool const shapeShift, int const mActions, int const element, int const hpDmg, int const mpDmg, int const spDmg,
                int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const range, bool const automate,
                bool const confuse, bool const reflect, bool const invincible, bool const revive, QList<Ability*>* const skills, QList<Ability*>* const counters,
                QMap<State*, int>* const states, QMap<const State*, int>* const stRes, QMap<int, int>* const res, QObject* const parent = NIL);

        explicit Costume(QObject* const parent = NIL);

        Costume(const Costume& costume);

        ~Costume();
    protected:
        class CostumeData : public QSharedData
        {
        public:
            ~CostumeData();

        protected:
            int _atk, _def, _spi, _wis, _agi, _m_actions, _cvr_type, _rfl_type;
            QList<Ability*>* _a_skills,* _counters;
            QMap<const State*, int>* _st_res;
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

        void refresh(QString* const ret, Actor& actor, bool const updStates, bool const remove) const;
        void adopt(QString* const ret, Actor& actor, bool const upeStates, bool const rmeove) const;

        Costume(int const id, QString& name, QString& sprite, bool const shapeShift, int const mActions, int const element, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
                int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun, bool const range, bool const automate, bool const confuse,
                bool const reflect, bool const ko, bool const invincible, bool const revive, QList<Ability*>* const skills, QList<Ability*>* const counters, QMap<State*, int>* const states,
                QMap<const State*, int>* const stRes, QMap<int, int>* const res, QObject* const parent = NIL);

        friend class Actor;
        friend class Ability;
        friend class DemoLib;
        friend class State;
        friend class Scene;
    };
}

Q_DECLARE_METATYPE(tbrpgsca::Costume)
Q_DECLARE_METATYPE(tbrpgsca::Costume*)
Q_DECLARE_INTERFACE(tbrpgsca::Costume, "com.codycostea.tbrpgsca.Costume")

Q_DECLARE_OPERATORS_FOR_FLAGS(tbrpgsca::Costume::Attributes)

#endif // COSTUME_H
