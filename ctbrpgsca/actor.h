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

    class Scene;

    class Actor : public Costume
    {
        #define FLAG_GUARDS 64
        #define FLAG_AI_PLAYER 128
        #define FLAG_SHAPE_SHIFT 256
    public:
        static QString KoTxt;
        static QString RiseTxt;

        inline Costume& getJob() const;
        inline Costume& getRace() const;

        inline bool isGuarding() const;
        inline bool isShapeShifted() const;
        inline bool isAiPlayer() const;

        inline int getLevel() const;
        inline int getMaximumLevel() const;
        inline int getMaximumExperience() const;
        inline int getCurrentActions() const;
        inline int getExperience() const;
        inline int getInitiative() const;
        inline int getPartySide() const;
        inline void* getExtra() const;

        int getRemainingSkillUses(const Ability& skill) const;
        int getRegeneratingSkillTurn(const Ability& skill) const;

        Costume* unequipPos(int const pos);
        Costume* equipItem(char const pos, Costume* const item);
        char unequipItem(const Costume& item);

        Actor& recover();
        Actor& checkStatus(const QString& ret);
        Actor& setExtra(void* const extra);
        Actor& setJob(const Costume& job);
        Actor& setRace(const Costume& race);
        Actor& setSprite(const QString& value);
        Actor& setName(const QString& value);
        Actor& setLevel(int const level);
        Actor& setExperience(int const xp);
        Actor& setMaximumLevel(int const maxLv);
        Actor& setMaximumHp(int const mHp);
        Actor& setMaximumMp(int const mMp);
        Actor& setMaximumRp(int const mSp);
        Actor& setCurrentHp(int const hp);
        Actor& setCurrentMp(int const mp);
        Actor& setCurrentRp(int const sp);
        Actor& setInitiative(int const init);
        Actor& setAttack(int const atk);
        Actor& setDefense(int const def);
        Actor& setSpirit(int const spi);
        Actor& setWisdom(int const wis);
        Actor& setAgility(int const agi);
        Actor& setRanged(bool const range);
        Actor& setGuarding(bool const guards);
        Actor& setCountering(bool const counters);
        Actor& setReflecting(bool const reflects);
        Actor& setShapeShifet(bool const shapeshift);
        Actor& setAutomated(bool const automated);
        Actor& setConfused(bool const confused);
        Actor& setAiPlayer(bool const aiPlayer);
        Actor& setReviving(bool const revive);
        Actor& setElementResistance(int const element, int const res);
        Actor& setStateResistance(const State* state, int const res);
        Actor& switchCostume(Costume* const oldCostume, Costume* const newCostume);
        Actor& updateSkills(bool const remove, const QVector<Ability*>& skills);
        Actor& updateResistance(bool const remove, const QMap<int, int>& elmRes, QMap<State*, int>& stRes);
        Actor& updateStates(bool const remove, const QVector<State*>& states);
        Actor& updateAttributes(bool const remove, const Costume& costume);
        Actor& applyStates(const QString& ret, bool const consume);
        Actor& checkRegSkill(const Ability& skill);
        Actor& applyRoles(const QString& ret);

        Actor(int const id, const QString& name, const Costume& race, const Costume& job, int const level, int const maxLv, int const mHp,
              int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const range,
              QVector<State*>* const states, QMap<int, int>* const res, QMap<State*, int>* const stRes, QMap<Ability*, int>* const items);

        Actor(Actor& actor);

        ~Actor();
    private:
        Costume* race,* job;
        int lv, maxLv, xp, maxp, init, side, actions;
        QMap<Ability*, int>* skillsCrQty,* skillsRgTurn,* items;
        QMap<char, Costume*>* equipment;
        QMap<State*, int>* stateDur;
        void* extra;

        Actor& applyRoles(const QString& ret, Scene* scene);
        Actor& applyStates(const QString& ret, Scene* scene, bool const consume);
        Actor& setCurrentHp(int const hp, QString& ret, Scene& scene);
        Actor& setAgility(int const agi, Scene& scene);

        friend class Scene;
        friend class Ability;
        friend class State;
        friend class Role;
    };

}

#endif // ACTOR_H
