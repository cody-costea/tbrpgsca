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
        #define FLAG_COVERING 8192
        #define CHAR_NONE 0
        #define CHAR_RACE 1
        #define CHAR_JOB 2
    public:
        static QString KoTxt;
        static QString RiseTxt;

        int level() const;
        int maximumLevel() const;
        int maximumExperience() const;
        QMap<Ability*, int> items() const;
        int currentActions() const;
        int experience() const;
        int initiative() const;
        //int getPartySide() const;
        Costume& race() const;
        Costume& job() const;
        bool hasNewItems() const;
        bool isAiPlayer() const;
        bool isRandomAi() const;
        bool isCovering() const;
        void* extra() const;

        int remainingSkillUses(Ability& skill) const;
        int regeneratingSkillTurn(Ability& skill) const;

        Costume* unequipPos(char const pos);
        Costume* equipItem(char const pos, Costume* const item);
        char unequipItem(Costume& item);

        void recover(QString& ret);
        //Actor& applyRoles(QString& ret);
        void applyStates(QString& ret, bool const consume);
        void setElementResistance(int const element, int const res);
        void setStateResistance(State* const state, int const res);
        void setItems(QMap<Ability*, int>* items);
        inline void setExtra(void* const extra);
        inline void setLevel(int const level);
        inline void setExperience(int const xp);
        inline void setSprite(QString& value);
        inline void setName(QString value);
        inline void setJob(Costume& job);
        inline void setRace(Costume& race);
        void setMaximumActions(int const mActions);
        inline void setMaximumLevel(int const maxLv);
        void setMaximumHp(int const mHp);
        void setMaximumMp(int const mMp);
        void setMaximumRp(int const mSp);
        void setCurrentHp(int const hp, QString& ret, bool const survive);
        void setCurrentHp(int const hp);
        void setCurrentMp(int const mp);
        void setCurrentRp(int const sp);
        inline void setOffense(int const atk);
        inline void setDefense(int const def);
        inline void setSpirit(int const spi);
        inline void setWisdom(int const wis);
        inline void setAgility(int const agi);
        void setInitiative(int const init);
        void setRanged(bool const range);
        void setStunned(bool const stun);
        void setCovering(bool const covers);
        void setReflecting(bool const reflects);
        void setShapeShifted(bool const shapeshift);
        void setKnockedOut(bool const knockOut);
        void setInvincible(bool const invincible);
        void setEnraged(bool const automate);
        void setConfused(bool const confuse);
        void setRandomAi(bool const randomAi);
        void setAiPlayer(bool const aiPlayer);
        void setReviving(bool const revive);

        Actor(int const id, QString name, QString sprite, Costume& race, Costume& job, int const level, int const maxLv, int const mActions,
              int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
              QMap<int, int>* const res, QMap<State*, int>* const stRes, QMap<Ability*, int>* const items);

        Actor(Actor& actor);

        ~Actor();
    protected:
        int _lv, _max_lv, _xp, _maxp, _init, _side, _actions;
        QMap<Ability*, int>* _skills_cr_qty,* _skills_rg_turn,* _items;
        QMap<char, Costume*> _equipment;
        QVector<Costume*>* _dmg_roles;
        void* _extra;

        void levelUp(Scene* const scene);
        inline void checkRegSkill(Ability& skill);
        void recover(QString* const ret, Scene* const scene);
        void applyDmgRoles(QString& ret, Scene* const scene);
        void removeStates(QString* const ret, Scene* const scene, bool const remove);
        void applyStates(QString* const ret, Scene* const scene, bool const consume);
        void updateStates(bool const remove, QString* const ret, Scene* const scene,
                            QMap<State*, int>& states, bool const includeWithDur);
        void updateAttributes(bool const remove, Scene* const scene, Costume& costume);
        void updateSkills(bool const remove, bool const counters, QVector<Ability*>& skills);
        void updateResistance(bool const remove, QMap<int, int>* const elmRes, QMap<State*, int>* const stRes);
        void switchCostume(QString* const ret, Scene* const scene, Costume* const oldCostume, Costume* const newCostume);
        void setCurrentHp(int const hp, QString* const ret, Scene* const scene, bool const survive);
        void setExperience(Scene* const scene, int const xp);
        void setLevel(Scene* const scene, int const level);
        void setRace(Scene* const scene, Costume& race);
        void setAgility(int const agi, Scene& scene);
        void setJob(Scene* const scene, Costume& job);
        char unequipItem(Scene* const scene, Costume& item);
        Costume* unequipPos(Scene* const scene, char const pos);
        Costume* equipItem(Scene* const scene, char const pos, Costume* const item);
        void refreshCostumes(QString* const ret, Scene* const scene);

    private:
        int _old_side;

        void setNewItems(bool const newItems);

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
