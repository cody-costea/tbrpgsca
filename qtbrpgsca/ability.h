/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef ABILITY_H
#define ABILITY_H

#include "role.h"

namespace tbrpgsca
{

    class Actor;
    class Scene;

    class Ability : public Role
    {
        #define DEFAULT_RES 3

        Q_OBJECT
        PROP_FLAG(Ability, Stealing, steals, Attribute::Steal, inline, public, public)
        PROP_FLAG(Ability, OnlyMelee, melee, Attribute::Melee, inline, public, public)
        PROP_FLAG(Ability, Missable, misses, Attribute::Missable, inline, public, public)
        PROP_FLAG(Ability, Absorbing, absorbs, Attribute::Absorb, inline, public, public)
        PROP_FLAG(Ability, TargetingAll, targetsAll, Attribute::TargetAll, inline, public, public)
        PROP_FLAG(Ability, TargetingSide, targetsSide, Attribute::TargetSide, inline, public, public)
        PROP_FLAG(Ability, TargetingSelf, targetsSelf, Attribute::TargetSelf, inline, public, public)
        PROP_FIELD(Ability, RequiredLevel, requiredLevel, int, inline, public, public, _ability_data->_lv_rq)
        PROP_FIELD(Ability, AttributeIncrement, attributeIncrement, int, inline, public, public, _ability_data->_attr_inc)
        PROP_FIELD(Ability, MaximumUses, maximumUses, int, inline, public, public, _ability_data->_m_qty)
        PROP_FIELD(Ability, UsesRegen, usesRegen, int, inline, public, public, _ability_data->_r_qty)
    public:
        enum Attribute {
            Melee = 4,
            Steal = 8,
            Absorb = 16,
            Missable = 32,
            TargetAll = 4,
            TargetSide = 128,
            TargetSelf = 64,
            TargetOne = 0
        };
        Q_DECLARE_FLAGS(Attributes, Attribute)
        Q_FLAG(Attributes)

        static QString MissesTxt;
        static QString ReflectTxt;
        static QString StolenTxt;

        QList<State*> removedStatesList() const;
        int removedStateDuration(State& state) const;
        bool removedState(State& state) const;
        int removedStatesSize() const;

        Q_INVOKABLE bool canPerform(Actor* const user) const;
        void execute(QString& ret, Actor& user, Actor& target, bool const applyCosts) const;
        void replenish(Actor& user) const;

        Ability(int const id, QString name, QString sprite, QString sound, bool const steal, bool const range, bool const melee, bool const canMiss, int const lvRq,
                int const hpCost, int const mpCost, int const spCost, int const dmgType, int const attrInc, int const hpDmg, int const mpDmg, int const spDmg, int const trg,
                int const elm, int const mQty, int const rQty, bool const absorb, bool const revive, QMap<State*, int>* const aStates, QMap<State*, int>* const rStates);

        Ability(const Ability& ability);

        ~Ability();
    protected:
        class AbilityData : public QSharedData
        {
        public:
            ~AbilityData();

        protected:
            int _lv_rq, _attr_inc, _m_qty, _r_qty;
            QMap<State*, int>* _r_states;
            QString* _sound;

            friend class Actor;
            friend class Costume;
            friend class Ability;
            friend class ArenaWidget;
            friend class SkillsModel;
            friend class ItemsModel;
            friend class State;
            friend class Scene;
        };

        QSharedDataPointer<AbilityData> _ability_data;

        void execute(QString& ret, Scene* const scene, Actor& user, Actor* target, bool const applyCosts) const;

        friend class Actor;
        friend class Costume;
        friend class ArenaWidget;
        friend class SkillsModel;
        friend class ItemsModel;
        friend class State;
        friend class Scene;
    };

}

Q_DECLARE_OPERATORS_FOR_FLAGS(tbrpgsca::Ability::Attributes)

#endif // ABILITY_H
