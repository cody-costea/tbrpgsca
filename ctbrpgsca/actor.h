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
#ifndef ACTOR_H
#define ACTOR_H

#include "costume.h"

namespace tbrpgsca
{

    class Actor : public Costume
    {
        #define FLAG_GUARDS 64
        #define FLAG_AI_PLAYER 128
        #define FLAG_SHAPE_SHIFT 256
    public:
        static QString KoTxt;
        static QString RiseTxt;

        inline Costume* getJob() const;
        inline Costume* getRace() const;

        inline bool isGuarding() const;
        inline bool isShapeShifted() const;
        inline bool isAiPlayer() const;

        inline int getLevel() const;
        inline int getMaximumLevel() const;
        inline int getMaximumExperience() const;
        inline int getExperience() const;
        inline int getInitiative() const;

        int getRemainingSkillUses(Ability* const skill) const;
        int getRegeneratingSkillTurn(Ability* const skill) const;

        Costume* unequipPos(int const pos);
        Costume* equipItem(char const pos, Costume* const item);
        char unequipItem(Costume* const item);

        QString applyStates(bool const consume);
        QString checkStatus();

        Actor* recover();
        Actor* setJob(Costume* const job);
        Actor* setRace(Costume* const race);
        Actor* setName(const QString& value);
        Actor* setSprite(const QString& value);
        Actor* setMaximumHp(const int mHp);
        Actor* setMaximumMp(const int mMp);
        Actor* setMaximumRp(const int mSp);
        Actor* setCurrentHp(const int hp);
        Actor* setCurrentMp(const int mp);
        Actor* setCurrentRp(const int sp);
        Actor* setInitiative(const int init);
        Actor* setRanged(const bool range);
        Actor* setReviving(const bool revive);

        Actor* setElementResistance(int const element, int const res);
        Actor* setStateResistance(State* const state, int const res);

        Actor* switchCostume(Costume* const oldCostume, Costume* const newCostume);
        Actor* updateSkills(bool const remove, const QVector<Ability*>& skills);
        Actor* updateResistance(bool const remove, const QMap<int, int>& elmRes, QMap<State*, int>& stRes);
        Actor* updateStates(bool const remove, const QVector<State*>& states);
        Actor* updateAttributes(bool const remove, const Costume& costume);
        Actor* checkRegSkill(Ability* const skill);

        Actor(int const id, const QString& name, Costume* const race, Costume* const job, int const level, int const maxLv, int const mHp,
              int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const range,
              QVector<State*>* const states, QMap<int, int>* const res, QMap<State*, int> const stRes, QMap<Ability*, int> const items);

        Actor(Actor& actor);

        ~Actor();
    private:
        Costume* race, job;
        int lv, maxLv, xp, maxp, init;
        QMap<Ability*, int>* skillsCrQty, skillsRgTurn, items;
        QMap<char, Costume*>* equipment;
        QMap<State*, int>* stateDur;

        friend class Scene;
        friend class Ability;
        friend class State;
        friend class Role;
    };

}

#endif // ACTOR_H
