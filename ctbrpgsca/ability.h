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
        #define FLAG_MELEE 4
        #define FLAG_STEAL 8
        #define FLAG_ABSORB 16
        #define FLAG_MISSABLE 32
        #define FLAG_TRG_ALL 4
        #define FLAG_TRG_SIDE 128
        #define FLAG_TRG_SELF 64
        #define FLAG_TRG_ONE 0
        #define DEFAULT_RES 3

        PROP_FLAG_GET(canMiss, FLAG_MISSABLE, public)
        PROP_FLAG_GET(isStealing, FLAG_STEAL, public)
        PROP_FLAG_GET(isOnlyMelee, FLAG_MELEE, public)
        PROP_FLAG_GET(targetsAll, FLAG_TRG_ALL, public)
        PROP_FLAG_GET(isAbsorbing, FLAG_ABSORB, public)
        PROP_FLAG_GET(targetsSide, FLAG_TRG_SIDE, public)
        PROP_FLAG_GET(targetsSelf, FLAG_TRG_SELF, public)
        PROP_FIELD_GET_CUSTOM(requiredLevel, int, public, _lv_rq)
        PROP_FIELD_GET_CUSTOM(attributeIncrement, int, public, _attr_inc)
        PROP_FIELD_GET_CUSTOM(maximumUses, int, public, _m_qty)
        PROP_FIELD_GET_CUSTOM(usesRegen, int, public, _r_qty)
    public:
        static QString MissesTxt;
        static QString ReflectTxt;
        static QString StolenTxt;

        QList<State*> removedStatesList() const;
        int removedStateDuration(State& state) const;
        bool removedState(State& state) const;
        int removedStatesSize() const;

        bool canPerform(Actor& user);
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

#endif // ABILITY_H
