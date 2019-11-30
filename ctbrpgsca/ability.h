/*
Copyright (C) AD 2013-2019 Claudiu-Stefan Costea

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#ifndef ABILITY_H
#define ABILITY_H

#include "role.h"

namespace tbrpgsca
{

    class Actor;

    class Ability : public Role
    {
        #define FLAG_MELEE 4
        #define FLAG_STEAL 8
        #define FLAG_ABSORB 16
        #define FLAG_TRG_ALL 4
        #define FLAG_TRG_FULL 32
        #define FLAG_TRG_SELF 64
        #define FLAG_DMG_ATK 128
        #define FLAG_DMG_DEF 256
        #define FLAG_DMG_SPI 512
        #define FLAG_DMG_WIS 1024
        #define FLAG_DMG_AGI 2048
    public:
        static QString MissesTxt;
        static QString SuffersTxt;
        static QString ReflectTxt;
        static QString StolenTxt;

        inline int getElementType() const;
        inline int getRequiredLevel() const;
        inline int getMaximumUses() const;
        inline int getUsesRegen() const;
        int getTargetRange();

        inline bool isStealing() const;
        inline bool isAbsorbing() const;
        inline bool isOnlyMelee() const;

        inline State* getRemovedState(int n) const;
        inline bool hasRemovedState(State* state) const;
        inline int getRemovedStatesSize() const;

        bool canPerform(const Actor& user);
        QString execute(const Actor& user, const Actor& target, bool const applyCosts);
        void replenish(const Actor& user);

        Ability(int const id, const QString& name, const QString& sprite, bool const steal, bool const range, bool const melee, int const lvRq,
                int const hpC, int const mpC, int const spC, int const dmgType, int const attrInc, int const hpDmg, int const mpDmg, int const spDmg,
                int const trg, int const elm,int const mQty, int const rQty, bool const absorb, bool const revive, QVector<State*>* const aStates,
                QVector<State*>* const rStates);

        Ability(Ability& ability);

        ~Ability();
    private:
        int lvRq, attrInc, elm, mQty, rQty;
        QVector<State*>* rStates;

        friend class Costume;
        friend class Actor;
        friend class State;
        friend class Scene;
    };

}

#endif // ABILITY_H
