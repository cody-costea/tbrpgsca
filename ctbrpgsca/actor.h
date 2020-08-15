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

        PROP_FLAG_GET(hasNewItems, FLAG_NEW_ITEMS, public)
        //PROP_FLAG_GET(isCovering, FLAG_COVERING, public)
        PROP_FLAG(Actor, Covering, FLAG_COVERING, public, public)
        PROP_FLAG(Actor, AiPlayer, FLAG_AI_PLAYER, public, public)
        PROP_FLAG(Actor, RandomAi, FLAG_RANDOM_AI, public, public)
        PROP_FLAG_SET_ALL(Actor, Ranged, FLAG_RANGE, public, Role::isRanged)
        PROP_FLAG_SET_ALL(Actor, NewItems, FLAG_NEW_ITEMS, public, hasNewItems)
        PROP_FLAG_SET_ALL(Actor, Stunned, FLAG_STUN, public, Costume::isStunned)
        PROP_FLAG_SET_ALL(Actor, Reviving, FLAG_REVIVE, public, Role::isReviving)
        PROP_FLAG_SET_ALL(Actor, Enraged, FLAG_ENRAGED, public, Costume::isEnraged)
        PROP_FLAG_SET_ALL(Actor, KnockedOut, FLAG_KO, public, Costume::isKnockedOut)
        PROP_FLAG_SET_ALL(Actor, Confused, FLAG_CONFUSE, public, Costume::isConfused)
        PROP_FLAG_SET_ALL(Actor, Reflecting, FLAG_REFLECT, public, Costume::isReflecting)
        PROP_FLAG_SET_ALL(Actor, Invincible, FLAG_INVINCIBLE, public, Costume::isInvincible)
        PROP_FLAG_SET_ALL(Actor, ShapeShifted, FLAG_SHAPE_SHIFT, public, Costume::isShapeShifted)
        PROP_CUSTOM_FIELD(Actor, initiative, setInitiative, withInitive, int, public, public, _init)
        PROP_CUSTOM_FIELD(Actor, partySide, setPartySide, withPartySide, int, public, public, _side)
        PROP_FIELD_WITH_SETGET(Actor, setExperience, experience, withExperience, int, public, experience)
        PROP_FIELD_WITH_SETGET(Actor, setMaximumHp, maximumHp, withMaximumHp, int, public, Role::maximumHp)
        PROP_FIELD_WITH_SETGET(Actor, setMaximumMp, maximumMp, withMaximumMp, int, public, Role::maximumMp)
        PROP_FIELD_WITH_SETGET(Actor, setMaximumRp, maximumRp, withMaximumRp, int, public, Role::maximumRp)
        PROP_FIELD_WITH_SETGET(Actor, setMaxActions, maxActions, withMaxActions, int, public, Costume::maxActions)
        PROP_FIELD_WITH_SETGET(Actor, setCurrentHp, currentHp, withCurrentHp, int, public, Role::currentHp)
        PROP_FIELD_WITH_SETGET(Actor, setCurrentMp, currentMp, withCurrentMp, int, public, Role::currentMp)
        PROP_FIELD_WITH_SETGET(Actor, setCurrentRp, currentRp, withCurrentRp, int, public, Role::currentRp)
        PROP_FIELD_WITH_SETGET(Actor, setOffense, offense, withOffense, int, public, Costume::offense)
        PROP_FIELD_WITH_SETGET(Actor, setDefense, defense, withDefense, int, public, Costume::defense)
        PROP_FIELD_WITH_SETGET(Actor, setAgility, agility, withAgility, int, public, Costume::agility)
        PROP_FIELD_WITH_SETGET(Actor, setWisdom, wisdom, withWisdom, int, public, Costume::wisdom)
        PROP_FIELD_WITH_SETGET(Actor, setSpirit, spirit, withSpirit, int, public, Costume::spirit)
        PROP_FIELD_WITH_SETGET(Actor, setMaxLevel, maxLevel, withMaxLevel, int, public, maxLevel)
        PROP_FIELD_SET_ALL(Actor, setName, name, withName, QString, public, Role::name, _name)
        PROP_FIELD_WITH_SETGET(Actor, setRace, race, withRace, Costume&, public, race)
        PROP_FIELD_WITH_SETGET(Actor, setLevel, level, withLevel, int, public, level)
        //PROP_FIELD_WITH_SETGET(Actor, setItems, items, withItems, QMap<Ability*, int>*, public, items)
        PROP_FIELD_SETGET(sprite, setSprite, QString, QString&, public, Role::sprite)
        PROP_FIELD_WITH_SETGET(Actor, setJob, job, withJob, Costume&, public, job)
        PROP_FIELD_WITH(Actor, withSprite, QString&, public, setSprite)
        PROP_FIELD(Actor, Extra, extra, void*, public, protected)
        PROP_FIELD_GET_CUSTOM(maxExperience, int, public, _maxp)
        //PROP_FIELD_GET_CUSTOM(initiative, int, public, _init)
        PROP_FIELD_GET_CUSTOM(actions, int, public, _actions)
        PROP_FIELD_GET_CUSTOM(maxLevel, int, public, _max_lv)
        PROP_FIELD_GET_CUSTOM(experience, int, public, _xp)
        PROP_FIELD_GET_CUSTOM(level, int, public, _lv)
    public:
        static QString KoTxt;
        static QString RiseTxt;

        QMap<Ability*, int> items() const;
        //int getPartySide() const;
        Costume& race() const;
        Costume& job() const;

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
        inline void setLevel(int const level);
        inline void setExperience(int const xp);
        inline void setSprite(QString& value);
        inline void setJob(Costume& job);
        inline void setRace(Costume& race);
        void setMaxActions(int const mActions);
        inline void setMaxLevel(int const maxLv);
        inline void setAgility(int const agi);
        inline void setOffense(int const atk);
        inline void setDefense(int const def);
        inline void setWisdom(int const wis);
        inline void setSpirit(int const spi);
        void setMaximumHp(int const mHp);
        void setMaximumMp(int const mMp);
        void setMaximumRp(int const mSp);
        void setCurrentHp(int const hp, QString& ret, bool const survive);
        void setCurrentHp(int const hp);
        void setCurrentMp(int const mp);
        void setCurrentRp(int const sp);

        Actor(int const id, QString name, QString sprite, Costume& race, Costume& job, int const level, int const maxLv, int const mActions,
              int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
              QMap<int, int>* const res, QMap<State*, int>* const stRes, QMap<Ability*, int>* const items);

        Actor(Actor& actor);

        ~Actor();
    protected:
        int _lv, _max_lv, _xp, _maxp, _actions;
        QMap<Ability*, int>* _skills_cr_qty,* _skills_rg_turn,* _items;
        QMap<char, Costume*> _equipment;
        QVector<Costume*>* _dmg_roles;

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
