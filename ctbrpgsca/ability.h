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
        #define FLAG_TRG_ALL 4
        #define FLAG_TRG_SIDE 32
        #define FLAG_TRG_SELF 64
        #define DEFAULT_RES 3
    public:
        static QString MissesTxt;
        static QString SuffersTxt;
        static QString ReflectTxt;
        static QString ResistTxt;
        static QString StolenTxt;

        inline int getRequiredLevel() const;
        inline int getAttributeIncrement() const;
        inline int getMaximumUses() const;
        inline int getUsesRegen() const;

        inline bool isStealing() const;
        inline bool isAbsorbing() const;
        inline bool isOnlyMelee() const;
        inline bool targetsSide() const;
        inline bool targetsSelf() const;
        inline bool targetsAll() const;

        inline QList<State*> getRemovedStatesList() const;
        inline int getRemovedStateDuration(State& state) const;
        inline bool hasRemovedState(State& state) const;
        inline int getRemovedStatesSize() const;

        inline bool canPerform(Actor& user);
        inline Ability& execute(QString& ret, Actor& user, Actor& target, bool const applyCosts);
        Ability& replenish(Actor& user);

        Ability(int const id, QString& name, QString* sprite, bool const steal, bool const range, bool const melee, int const lvRq, int const hpC, int const mpC,
                int const spC, int const dmgType, int const attrInc, int const hpDmg, int const mpDmg, int const spDmg, int const trg, int const elm, int const mQty,
                int const rQty, bool const absorb, bool const revive, QMap<State*, int>* const aStates, QMap<State*, int>* const rStates);

        Ability(Ability& ability);

        ~Ability();
    protected:
        int lvRq, attrInc, mQty, rQty;
        QMap<State*, int>* rStates;

        Ability& execute(QString& ret, Scene* scene, Actor& user, Actor* target, bool const applyCosts);

        friend class Actor;
        friend class Costume;
        friend class State;
        friend class Scene;
    };

}

#endif // ABILITY_H
