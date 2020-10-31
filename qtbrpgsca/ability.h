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
        PROP_FLAG_GET(canMiss, Attribute::Missable, public)
        PROP_FLAG_GET(isStealing, Attribute::Steal, public)
        PROP_FLAG_GET(isOnlyMelee, Attribute::Melee, public)
        PROP_FLAG_GET(isAbsorbing, Attribute::Absorb, public)
        PROP_FLAG_GET(targetsAll, Attribute::TargetAll, public)
        PROP_FLAG_GET(targetsSide, Attribute::TargetSide, public)
        PROP_FLAG_GET(targetsSelf, Attribute::TargetSelf, public)
        PROP_FIELD_GET_CUSTOM(requiredLevel, int, public, _lv_rq)
        PROP_FIELD_GET_CUSTOM(attributeIncrement, int, public, _attr_inc)
        PROP_FIELD_GET_CUSTOM(maximumUses, int, public, _m_qty)
        PROP_FIELD_GET_CUSTOM(usesRegen, int, public, _r_qty)
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

        Q_INVOKABLE bool canPerform(Actor* const user);
        void execute(QString& ret, Actor& user, Actor& target, bool const applyCosts);
        void replenish(Actor& user);

        Ability(int const id, QString name, QString sprite, QString sound, bool const steal, bool const range, bool const melee, bool const canMiss, int const lvRq,
                int const hpCost, int const mpCost, int const spCost, int const dmgType, int const attrInc, int const hpDmg, int const mpDmg, int const spDmg, int const trg,
                int const elm, int const mQty, int const rQty, bool const absorb, bool const revive, QMap<State*, int>* const aStates, QMap<State*, int>* const rStates);

        Ability(Ability& ability);

        ~Ability();
    protected:
        int _lv_rq, _attr_inc, _m_qty, _r_qty;
        QMap<State*, int>* _r_states;
        QString* _sound;

        void execute(QString& ret, Scene* const scene, Actor& user, Actor* target, bool const applyCosts);

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
