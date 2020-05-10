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
        #define FLAG_AI_PLAYER 1024
        #define FLAG_RANDOM_AI 2048
        #define FLAG_NEW_ITEMS 4096
        #define CHAR_NONE 0
        #define CHAR_RACE 1
        #define CHAR_JOB 2
    public:
        static QString KoTxt;
        static QString RiseTxt;

        int getLevel() const;
        int getMaximumLevel() const;
        int getMaximumExperience() const;
        QMap<Ability*, int> getItems() const;
        int getCurrentActions() const;
        int getExperience() const;
        int getInitiative() const;
        //int getPartySide() const;
        Costume& getRace() const;
        Costume& getJob() const;
        bool hasNewItems() const;
        bool isAiPlayer() const;
        bool isRandomAi() const;
        void* getExtra() const;

        int getRemainingSkillUses(Ability& skill) const;
        int getRegeneratingSkillTurn(Ability& skill) const;

        Costume* unequipPos(char const pos);
        Costume* equipItem(char const pos, Costume* const item);
        char unequipItem(Costume& item);

        Actor& recover(QString& ret);
        //Actor& applyRoles(QString& ret);
        Actor& applyStates(QString& ret, bool const consume);
        Actor& setElementResistance(int const element, int const res);
        Actor& setStateResistance(State* const state, int const res);
        Actor& setItems(QMap<Ability*, int>* items);
        Actor& setExtra(void* const extra);
        Actor& setLevel(int const level);
        Actor& setExperience(int const xp);
        Actor& setSprite(QString& value);
        Actor& setName(QString value);
        Actor& setJob(Costume& job);
        Actor& setRace(Costume& race);
        Actor& setMaximumLevel(int const maxLv);
        Actor& setMaximumActions(int const mActions);
        Actor& setMaximumHp(int const mHp);
        Actor& setMaximumMp(int const mMp);
        Actor& setMaximumRp(int const mSp);
        Actor& setCurrentHp(int const hp, QString& ret, bool const survive);
        Actor& setCurrentHp(int const hp);
        Actor& setCurrentMp(int const mp);
        Actor& setCurrentRp(int const sp);
        Actor& setAttack(int const atk);
        Actor& setDefense(int const def);
        Actor& setSpirit(int const spi);
        Actor& setWisdom(int const wis);
        Actor& setAgility(int const agi);
        Actor& setInitiative(int const init);
        Actor& setRanged(bool const range);
        Actor& setStunned(bool const stun);
        Actor& setReflecting(bool const reflects);
        Actor& setShapeShifted(bool const shapeshift);
        Actor& setKnockedOut(bool const knockOut);
        Actor& setInvincible(bool const invincible);
        Actor& setEnraged(bool const automate);
        Actor& setConfused(bool const confuse);
        Actor& setRandomAi(bool const randomAi);
        Actor& setAiPlayer(bool const aiPlayer);
        Actor& setReviving(bool const revive);

        Actor(int const id, QString name, QString sprite, Costume& race, Costume& job, int const level, int const maxLv, int const mActions,
              int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
              QMap<int, int>* const res, QMap<State*, int>* const stRes, QMap<Ability*, int>* const items);

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
        Actor& recover(QString* const ret, Scene* const scene);
        Actor& applyDmgRoles(QString& ret, Scene* const scene);
        Actor& removeStates(QString* const ret, Scene* const scene, bool const remove);
        Actor& applyStates(QString* const ret, Scene* const scene, bool const consume);
        Actor& updateStates(bool const remove, QString* const ret, Scene* const scene,
                            QMap<State*, int>& states, bool const includeWithDur);
        Actor& updateAttributes(bool const remove, Scene* const scene, Costume& costume);
        Actor& updateSkills(bool const remove, bool const counters, QVector<Ability*>& skills);
        Actor& updateResistance(bool const remove, QMap<int, int>* const elmRes, QMap<State*, int>* const stRes);
        Actor& switchCostume(QString* const ret, Scene* const scene, Costume* const oldCostume, Costume* const newCostume);
        Actor& setCurrentHp(int const hp, QString* const ret, Scene* const scene, bool const survive);
        Actor& setExperience(Scene* const scene, int const xp);
        Actor& setLevel(Scene* const scene, int const level);
        Actor& setRace(Scene* const scene, Costume& race);
        Actor& setAgility(int const agi, Scene& scene);
        Actor& setJob(Scene* const scene, Costume& job);
        char unequipItem(Scene* const scene, Costume& item);
        Costume* unequipPos(Scene* const scene, char const pos);
        Costume* equipItem(Scene* const scene, char const pos, Costume* const item);
        Actor& refreshCostumes(QString* const ret, Scene* const scene);

    private:
        int oldSide;

        Actor& setNewItems(bool const newItems);

        friend class Scene;
        friend class Ability;
        friend class ArenaWidget;
        friend class TargetsModel;
        friend class SkillsModel;
        friend class ItemsModel;
        friend class Costume;
        friend class State;
        friend class Role;
    };

}

#endif // ACTOR_H
