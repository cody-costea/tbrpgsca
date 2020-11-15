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
        #define FLAG_NEW_ITEMS 4096
        #define CHAR_NONE 0
        #define CHAR_RACE 1
        #define CHAR_JOB 2

        Q_OBJECT
        PROP_FLAG_GET(hasNewItems, FLAG_NEW_ITEMS, public)
        PROP_FLAG(Actor, aiPlayer, AiPlayer, Attribute::AiPlayer, public, public)
        PROP_FLAG(Actor, randomAi, RandomAi, Attribute::RandomAi, public, public)
        PROP_FLAG_SET_ALL(Actor, Ranged, Role::Attribute::Range, public, isRanged)
        PROP_FLAG_SET_ALL(Actor, NewItems, FLAG_NEW_ITEMS, public, hasNewItems)
        PROP_FLAG_SET_ALL(Actor, Stunned, Costume::Attribute::Stun, public, isStunned)
        PROP_FLAG_SET_ALL(Actor, Reviving, Role::Attribute::Revive, public, isReviving)
        PROP_FLAG_SET_ALL(Actor, Enraged, Costume::Attribute::Enraged, public, isEnraged)
        PROP_FLAG_SET_ALL(Actor, KnockedOut, Costume::Attribute::Ko, public, isKnockedOut)
        PROP_FLAG_SET_ALL(Actor, Confused, Costume::Attribute::Confuse, public, isConfused)
        PROP_FLAG_SET_ALL(Actor, Covering, Costume::Attribute::Covering, public, isCovering)
        PROP_FLAG_SET_ALL(Actor, Reflecting, Costume::Attribute::Reflect, public, isReflecting)
        PROP_FLAG_SET_ALL(Actor, Invincible, Costume::Attribute::Invincible, public, isInvincible)
        PROP_FLAG_SET_ALL(Actor, ShapeShifted, Costume::Attribute::ShapeShift, public, isShapeShifted)
        PROP_CUSTOM_FIELD(Actor, initiative, setInitiative, swapInitiative, withInitive, int, public, public, _init)
        PROP_CUSTOM_FIELD(Actor, partySide, setPartySide, swapPartySide, withPartySide, int, public, public, _side)
        PROP_FIELD_WITH_SWAP(Actor, setExperience, swapExperience, withExperience, int, public, experience)
        PROP_FIELD_WITH_SWAP(Actor, setMaximumHp, swapMaximumHp, withMaximumHp, int, public, maximumHp)
        PROP_FIELD_WITH_SWAP(Actor, setMaximumMp, swapMaximumMp, withMaximumMp, int, public, maximumMp)
        PROP_FIELD_WITH_SWAP(Actor, setMaximumRp, swapMaximumRp, withMaximumRp, int, public, maximumRp)
        PROP_FIELD_WITH_SWAP(Actor, setMaxActions, swapMaxActions, withMaxActions, int, public, maxActions)
        PROP_FIELD_WITH_SWAP(Actor, setCurrentHp, swapCurrentHp, withCurrentHp, int, public, currentHp)
        PROP_FIELD_WITH_SWAP(Actor, setCurrentMp, swapCurrentMp, withCurrentMp, int, public, currentMp)
        PROP_FIELD_WITH_SWAP(Actor, setCurrentRp, swapCurrentRp, withCurrentRp, int, public, currentRp)
        PROP_FIELD_WITH_SWAP(Actor, setOffense, swapOffense, withOffense, int, public, offense)
        PROP_FIELD_WITH_SWAP(Actor, setDefense, swapDefense, withDefense, int, public, defense)
        PROP_FIELD_WITH_SWAP(Actor, setAgility, swapAgility, withAgility, int, public, agility)
        PROP_FIELD_WITH_SWAP(Actor, setWisdom, swapWisdom, withWisdom, int, public, wisdom)
        PROP_FIELD_WITH_SWAP(Actor, setSpirit, swapSpirit, withSpirit, int, public, spirit)
        PROP_FIELD_WITH_SWAP(Actor, setMaxLevel, swapMaxLevel, withMaxLevel, int, public, maxLevel)
        PROP_FIELD_SET_ALL(Actor, setDmgType, sawpDmgType, withDmgType, int, public, dmgType, _role_data->_dmg_type)
        PROP_FIELD_SET_ALL(Actor, setReflectDmgType, sawpReflectDmgType, withReflectDmgType, int, public, reflectDmgType, _costume_data->_rfl_type)
        PROP_FIELD_SET_ALL(Actor, setCoverDmgType, sawpCoverDmgType, withCoverDmgType, int, public, coverDmgType, _costume_data->_cvr_type)
        PROP_FIELD_SET_ALL(Actor, setName, sawpName, withName, QString, public, name, _role_data->_name)
        PROP_FIELD_WITH_SWAP(Actor, setRace, swapRace, withRace, Costume&, public, race)
        PROP_FIELD_WITH_SWAP(Actor, setLevel, swapLevel, withLevel, int, public, level)
        //PROP_FIELD_WITH_SWAP(Actor, setItems, items, withItems, QMap<Ability*, int>*, public, items)
        PROP_FIELD_SWAP(swapSprite, setSprite, QString, QString&, public, sprite)
        PROP_FIELD_WITH_SWAP(Actor, setJob, swapJob, withJob, Costume&, public, job)
        PROP_FIELD_WITH(Actor, withSprite, QString&, public, setSprite)
        PROP_FIELD(Actor, Actions, actions, int, public, protected)
        PROP_FIELD(Actor, Extra, extra, void*, public, protected)
        PROP_FIELD_GET_CUSTOM(maxExperience, int, public, _maxp)
        //PROP_FIELD_GET_CUSTOM(initiative, int, public, _init)
        //PROP_FIELD_GET_CUSTOM(actions, int, public, _actions)
        PROP_FIELD_GET_CUSTOM(maxLevel, int, public, _max_lv)
        PROP_FIELD_GET_CUSTOM(experience, int, public, _xp)
        PROP_FIELD_GET_CUSTOM(level, int, public, _lv)
    public:
        enum Attribute {
            //HasNewItems = 4096,
            RandomAi = 2048,
            AiPlayer = 8192
        };
        Q_DECLARE_FLAGS(Attributes, Attribute)
        Q_FLAG(Attributes)

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
        void setLevel(int const level);
        void setExperience(int const xp);
        void setSprite(QString& value);
        void setJob(Costume& job);
        void setRace(Costume& race);
        void setMaxActions(int const mActions);
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

        Actor(int const id, QString name, QString sprite, Costume& race, Costume& job, int const level, int const maxLv, int const mActions,
              int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
              QMap<int, int>* const res, QMap<State*, int>* const stRes, QMap<Ability*, int>* const items);

        Actor(Actor& actor);

        ~Actor();
    protected:
        int _lv, _max_lv, _xp, _maxp;
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

Q_DECLARE_OPERATORS_FOR_FLAGS(tbrpgsca::Actor::Attributes)

#endif // ACTOR_H
