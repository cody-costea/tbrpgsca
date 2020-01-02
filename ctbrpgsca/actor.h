/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef ACTOR_H
#define ACTOR_H

#include "costume.h"

namespace tbrpgsca
{

    class Scene;

    class Actor : public Costume
    {
        #define FLAG_AI_PLAYER 256
        #define CHAR_NONE 0
        #define CHAR_RACE 1
        #define CHAR_JOB 2
    public:
        static QString KoTxt;
        static QString RiseTxt;

        inline Costume& getJob() const;
        inline Costume& getRace() const;
        inline bool isAiPlayer() const;
        inline int getLevel() const;
        inline int getMaximumLevel() const;
        inline int getMaximumExperience() const;
        inline int getCurrentActions() const;
        inline int getExperience() const;
        inline int getInitiative() const;
        //inline int getPartySide() const;
        inline void* getExtra() const;

        int getRemainingSkillUses(Ability& skill) const;
        int getRegeneratingSkillTurn(Ability& skill) const;

        Costume* unequipPos(char const pos);
        Costume* equipItem(char const pos, Costume* const item);
        char unequipItem(Costume& item);

        inline Actor& recover(QString& ret);
        //Actor& applyRoles(QString& ret);
        Actor& applyStates(QString& ret, bool const consume);
        Actor& setElementResistance(int const element, int const res);
        Actor& setStateResistance(State* const state, int const res);
        Actor& setExtra(void* const extra);
        Actor& setLevel(int const level);
        Actor& setExperience(int const xp);
        inline Actor& setSprite(QString& value);
        inline Actor& setName(QString& value);
        inline Actor& setJob(Costume& job);
        inline Actor& setRace(Costume& race);
        inline Actor& setMaximumLevel(int const maxLv);
        inline Actor& setMaximumActions(int const mActions);
        inline Actor& setMaximumHp(int const mHp);
        inline Actor& setMaximumMp(int const mMp);
        inline Actor& setMaximumRp(int const mSp);
        inline Actor& setCurrentHp(int const hp, QString& ret, bool const survive);
        inline Actor& setCurrentHp(int const hp);
        inline Actor& setCurrentMp(int const mp);
        inline Actor& setCurrentRp(int const sp);
        inline Actor& setAttack(int const atk);
        inline Actor& setDefense(int const def);
        inline Actor& setSpirit(int const spi);
        inline Actor& setWisdom(int const wis);
        inline Actor& setAgility(int const agi);
        inline Actor& setInitiative(int const init);
        Actor& setRanged(bool const range);
        Actor& setStunned(bool const stun);
        Actor& setReflecting(bool const reflects);
        Actor& setShapeShifted(bool const shapeshift);
        Actor& setEnraged(bool const automate);
        Actor& setConfused(bool const confuse);
        Actor& setAiPlayer(bool const aiPlayer);
        Actor& setReviving(bool const revive);

        Actor(int const id, QString& name, Costume& race, Costume& job, int const level, int const maxLv, int const mHp, int const mMp, int const mSp, int const atk,
              int const def, int const spi, int const wis, int const agi, QMap<int, int>* const res, QMap<State*, int>* const stRes, QMap<Ability*, int>* const items);

        Actor(Actor& actor);

        ~Actor();
    protected:
        int lv, maxLv, xp, maxp, init, side, actions;
        QMap<Ability*, int>* skillsCrQty,* skillsRgTurn,* items;
        QMap<char, Costume*> equipment;
        QVector<Costume*>* dmgRoles;
        void* extra;

        Actor& levelUp(Scene* const scene);
        Actor& checkRegSkill(Ability& skill);
        Actor& recover(QString* ret, Scene* const scene);
        Actor& applyDmgRoles(QString& ret, Scene* const scene);
        Actor& applyStates(QString& ret, Scene* const scene, bool const consume);
        Actor& updateAttributes(bool const remove, Scene* const scene, Costume& costume);
        Actor& updateSkills(bool const remove, bool const counters, QVector<Ability*>& skills);
        Actor& updateStates(bool const remove, QString& ret, Scene* const scene, QMap<State*, int>& states);
        Actor& updateResistance(bool const remove, QMap<int, int>* const elmRes, QMap<State*, int>* const stRes);
        Actor& switchCostume(QString* ret, Scene* const scene, Costume* const oldCostume, Costume* const newCostume);
        Actor& setCurrentHp(int const hp, QString& ret, Scene* scene, bool const survive);
        Actor& setAgility(int const agi, Scene& scene);
        inline Actor& setJob(Scene* const scene, Costume& job);
        inline Actor& setRace(Scene* const scene, Costume& race);
        Actor& setLevel(Scene* const scene, int const level);
        Actor& setExperience(Scene* const scene, int const xp);
        Costume* equipItem(Scene* const scene, char const pos, Costume* const item);
        Costume* unequipPos(Scene* const scene, char const pos);
        char unequipItem(Scene* const scene, Costume& item);
        Actor& refreshCostume(QString* ret, Scene* scene, Costume& costume);
        Actor& refreshCostumes(QString* ret, Scene* scene);

    private:
        unsigned char oldSide;

        friend class Scene;
        friend class Ability;
        friend class Costume;
        friend class State;
        friend class Role;
    };

}

#endif // ACTOR_H
