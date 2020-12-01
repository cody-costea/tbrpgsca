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
        #define FLAG_ACTIVE 32768
        #define FLAG_AI_PLAYER 8192
        #define FLAG_RANDOM_AI 16384
        #define FLAG_NEW_ITEMS 4096
        #define CHAR_NONE 0
        #define CHAR_RACE 1
        #define CHAR_JOB 2

        PROP_FLAG_GET(hasNewItems, FLAG_NEW_ITEMS, public)
        PROP_FLAG(Actor, Active, FLAG_ACTIVE, public, public)
        PROP_FLAG(Actor, AiPlayer, FLAG_AI_PLAYER, public, public)
        PROP_FLAG(Actor, RandomAi, FLAG_RANDOM_AI, public, public)
        PROP_FLAG_SET_ALL(Actor, Ranged, FLAG_RANGE, public, Role::isRanged)
        PROP_FLAG_SET_ALL(Actor, NewItems, FLAG_NEW_ITEMS, public, hasNewItems)
        PROP_FLAG_SET_ALL(Actor, Stunned, FLAG_STUN, public, Costume::isStunned)
        PROP_FLAG_SET_ALL(Actor, Reviving, FLAG_REVIVE, public, Role::isReviving)
        PROP_FLAG_SET_ALL(Actor, Enraged, FLAG_ENRAGED, public, Costume::isEnraged)
        PROP_FLAG_SET_ALL(Actor, KnockedOut, FLAG_KO, public, Costume::isKnockedOut)
        PROP_FLAG_SET_ALL(Actor, Confused, FLAG_CONFUSE, public, Costume::isConfused)
        PROP_FLAG_SET_ALL(Actor, Covering, FLAG_COVERING, public, Costume::isCovering)
        PROP_FLAG_SET_ALL(Actor, Reflecting, FLAG_REFLECT, public, Costume::isReflecting)
        PROP_FLAG_SET_ALL(Actor, Invincible, FLAG_INVINCIBLE, public, Costume::isInvincible)
        PROP_FLAG_SET_ALL(Actor, ShapeShifted, FLAG_SHAPE_SHIFT, public, Costume::isShapeShifted)
        PROP_CUSTOM_FIELD_NEW(Actor, initiative, setInitiative, swapInitiative, withInitive, int, public, public, _init)
        PROP_CUSTOM_FIELD_NEW(Actor, partySide, setPartySide, swapPartySide, withPartySide, int, public, public, _side)
        PROP_FIELD_WITH_SWAP(Actor, setExperience, swapExperience, withExperience, int, public, experience)
        PROP_FIELD_WITH_SWAP(Actor, setMaximumHp, swapMaximumHp, withMaximumHp, int, public, Role::maximumHp)
        PROP_FIELD_WITH_SWAP(Actor, setMaximumMp, swapMaximumMp, withMaximumMp, int, public, Role::maximumMp)
        PROP_FIELD_WITH_SWAP(Actor, setMaximumRp, swapMaximumRp, withMaximumRp, int, public, Role::maximumRp)
        //PROP_FIELD_WITH_SWAP(Actor, setMaxActions, swapMaxActions, withMaxActions, int, public, Costume::maxActions)
        PROP_FIELD_WITH_SWAP(Actor, setCurrentHp, swapCurrentHp, withCurrentHp, int, public, Role::currentHp)
        PROP_FIELD_WITH_SWAP(Actor, setCurrentMp, swapCurrentMp, withCurrentMp, int, public, Role::currentMp)
        PROP_FIELD_WITH_SWAP(Actor, setCurrentRp, swapCurrentRp, withCurrentRp, int, public, Role::currentRp)
        PROP_FIELD_WITH_SWAP(Actor, setOffense, swapOffense, withOffense, int, public, Costume::offense)
        PROP_FIELD_WITH_SWAP(Actor, setDefense, swapDefense, withDefense, int, public, Costume::defense)
        PROP_FIELD_WITH_SWAP(Actor, setAgility, swapAgility, withAgility, int, public, Costume::agility)
        PROP_FIELD_WITH_SWAP(Actor, setWisdom, swapWisdom, withWisdom, int, public, Costume::wisdom)
        PROP_FIELD_WITH_SWAP(Actor, setSpirit, swapSpirit, withSpirit, int, public, Costume::spirit)
        PROP_FIELD_WITH_SWAP(Actor, setMaxLevel, swapMaxLevel, withMaxLevel, int, public, maxLevel)
        PROP_FIELD_SET_ALL(Actor, setName, sawpName, withName, QString, public, Role::name, _name)
        PROP_FIELD_WITH_SWAP(Actor, setRace, swapRace, withRace, const Costume&, public, race)
        PROP_FIELD_WITH_SWAP(Actor, setLevel, swapLevel, withLevel, int, public, level)
        //PROP_FIELD_WITH_SWAP(Actor, setItems, items, withItems, QMap<Ability*, int>*, public, items)
        PROP_FIELD_SWAP(swapSprite, setSprite, QString, QString&, public, Role::sprite)
        PROP_FIELD_WITH_SWAP(Actor, setJob, swapJob, withJob, const Costume&, public, job)
        PROP_FIELD_WITH(Actor, withSprite, QString&, public, setSprite)
        PROP_FIELD(Actor, Extra, extra, void*, public, protected)
        PROP_FIELD_GET_CUSTOM(maxExperience, int, public, _maxp)
        //PROP_FIELD_GET_CUSTOM(initiative, int, public, _init)
        //PROP_FIELD_GET_CUSTOM(actions, int, public, _actions)
        PROP_FIELD_GET_CUSTOM(maxLevel, int, public, _max_lv)
        PROP_FIELD_GET_CUSTOM(experience, int, public, _xp)
        PROP_FIELD_GET_CUSTOM(level, int, public, _lv)
    public:
        static QString KoTxt;
        static QString RiseTxt;

        QMap<const Ability*, int> items() const;
        //int getPartySide() const;
        const Costume& race() const;
        const Costume& job() const;

        int remainingSkillUses(Ability& skill) const;
        int regeneratingSkillTurn(Ability& skill) const;

        const Costume* unequipPos(char const pos);
        const Costume* equipItem(char const pos, const Costume* const item);
        char unequipItem(const Costume& item);

        void recover(QString& ret);
        //Actor& applyRoles(QString& ret);
        void applyStates(QString& ret, bool const consume);
        void setElementResistance(int const element, int const res);
        void setStateResistance(const State* const state, int const res);
        void setItems(QMap<const Ability*, int>* items);
        void setLevel(int const level);
        void setExperience(int const xp);
        void setSprite(QString& value);
        void setJob(const Costume& job);
        void setRace(const Costume& race);
        //void setMaxActions(int const mActions);
        void setMaxLevel(int const maxLv);
        void setAgility(int const agi);
        void setOffense(int const atk);
        void setDefense(int const def);
        void setWisdom(int const wis);
        void setSpirit(int const spi);
        void setMaximumHp(int const mHp);
        void setMaximumMp(int const mMp);
        void setMaximumRp(int const mSp);
        void setCurrentHp(int const hp, QString& ret, bool const survive);
        void setCurrentHp(int const hp);
        void setCurrentMp(int const mp);
        void setCurrentRp(int const sp);

        Actor(int const id, QString name, QString sprite, const Costume& race, const Costume& job, int const level, int const maxLv, int const mActions,
              int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
              QMap<int, int>* const res, QMap<const State*, int>* const stRes, QMap<const Ability*, int>* const items);

        Actor(const Actor& actor);

        ~Actor();
    protected:
        signed int _lv: 8, _max_lv: 8, _xp: 32, _maxp: 32;
        QMap<const Ability*, int>* _skills_cr_qty,* _skills_rg_turn,* _items;
        QMap<char, const Costume*> _equipment;
        QVector<const Costume*>* _dmg_roles;

        void levelUp(Scene* const scene);
        inline void checkRegSkill(const Ability& skill);
        void recover(QString* const ret, Scene* const scene);
        void removeStates(QString* const ret, Scene* const scene, bool const remove);
        void updateStates(bool const remove, QString* const ret, Scene* const scene,
                            QMap<const State*, int>& states, bool const includeWithDur);
        void updateAttributes(bool const remove, Scene* const scene, const Costume& costume);
        void updateSkills(bool const remove, bool const counters, QVector<const Ability*>& skills);
        void updateResistance(bool const remove, QMap<int, int>* const elmRes, QMap<const State*, int>* const stRes);
        void switchCostume(QString* const ret, Scene* const scene, const Costume* const oldCostume, const Costume* const newCostume);
        void setExperience(Scene* const scene, int const xp);
        void setLevel(Scene* const scene, int const level);
        void setRace(Scene* const scene, const Costume& race);
        void setAgility(int const agi, Scene& scene);
        void setJob(Scene* const scene, const Costume& job);
        char unequipItem(Scene* const scene, const Costume& item);
        const Costume* unequipPos(Scene* const scene, char const pos);
        const Costume* equipItem(Scene* const scene, char const pos, const Costume* const item);
        void refreshCostumes(QString* const ret, Scene* const scene);

        template <typename SpriteRun>
        void applyDmgRoles(QString& ret, Scene* const scene, SpriteRun* const actorEvent);

        template <typename SpriteRun>
        void applyStates(QString* const ret, Scene* const scene, SpriteRun* const spriteRun, bool const consume);

        template <typename SpriteRun>
        void setCurrentHp(QString* const ret, Scene* const scene, SpriteRun* const actorEvent, int const hp, bool const survive);

    private:

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
