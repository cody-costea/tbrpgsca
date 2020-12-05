/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef ACTOR_H
#define ACTOR_H

#include "costume.h"

#include <QSharedPointer>

namespace tbrpgsca
{

    class Scene;

    class Actor : public Costume
    {
        #define CHAR_NONE 0
        #define CHAR_RACE 1
        #define CHAR_JOB 2

        Q_OBJECT
        QML_ELEMENT
        PROP_FLAG(Actor, AiPlayer, aiPlayer, Attribute::AiPlayer, inline, public, public)
        PROP_FLAG(Actor, RandomAi, randomAi, Attribute::RandomAi, inline, public, public)
        /*PROP_FLAG_SET_ALL(Actor, Ranged, Role::Attribute::Range, public, isRanged)
        PROP_FLAG_SET_ALL(Actor, Stunned, Costume::Attribute::Stun, public, isStunned)
        PROP_FLAG_SET_ALL(Actor, Reviving, Role::Attribute::Revive, public, isReviving)
        PROP_FLAG_SET_ALL(Actor, Enraged, Costume::Attribute::Enraged, public, isEnraged)
        PROP_FLAG_SET_ALL(Actor, KnockedOut, Costume::Attribute::Ko, public, isKnockedOut)
        PROP_FLAG_SET_ALL(Actor, Confused, Costume::Attribute::Confuse, public, isConfused)
        PROP_FLAG_SET_ALL(Actor, Covering, Costume::Attribute::Covering, public, isCovering)
        PROP_FLAG_SET_ALL(Actor, Reflecting, Costume::Attribute::Reflect, public, isReflecting)
        PROP_FLAG_SET_ALL(Actor, Invincible, Costume::Attribute::Invincible, public, isInvincible)
        PROP_FLAG_SET_ALL(Actor, ShapeShifted, Costume::Attribute::ShapeShift, public, isShapeShifted)
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
        PROP_FIELD_SET_ALL(Actor, setDmgType, sawpDmgType, withDmgType, int, public, dmgType, _role_data->_dmg_type)
        PROP_FIELD_SET_ALL(Actor, setReflectDmgType, sawpReflectDmgType, withReflectDmgType, int, public, reflectDmgType, _costume_data->_rfl_type)
        PROP_FIELD_SET_ALL(Actor, setCoverDmgType, sawpCoverDmgType, withCoverDmgType, int, public, coverDmgType, _costume_data->_cvr_type)
        PROP_FIELD_SET_ALL(Actor, setName, sawpName, withName, QString, public, name, _role_data->_name)*/
        PROP_FIELD_WITH_SWAP(Actor, setRace, swapRace, withRace, Costume&, inline, public, race)
        PROP_FIELD_WITH_SWAP(Actor, setMaximumLevel, swapMaximumLevel, withMaximumLevel, int, inline, public, maximumLevel)
        //PROP_FIELD_SET_ALL(Actor, setExtra, sawpExtra, withExtra, void*, inline, public, extra, _actor_data->_extra)
        PROP_FIELD_WITH_SWAP(Actor, setCurrentLevel, swapCurrentLevel, withCurrentLevel, int, inline, public, currentLevel)
        PROP_FIELD_SET_ALL(Actor, setCurrentActions, sawpCurrentActions, withCurrentActions, int, inline, public, currentActions, _actor_data->_actions)
        PROP_FIELD_SET_ALL(Actor, setPartySide, sawpPartySide, withPartySide, int, inline, public, partySide, _actor_data->_side)
        PROP_FIELD_SET_ALL(Actor, setInitiative, sawpInitiative, withInitiative, int, inline, public, initiative, _actor_data->_init)
        PROP_FIELD_SET_ALL(Actor, setMaximumExperience, sawpMaximumExperience, withMaximumExperience, int, inline, public, maximumExperience, _actor_data->_maxp)
        PROP_FIELD_SWAP(swapSprite, setSprite, QString, QString&, inline, public, sprite)
        PROP_FIELD_WITH_SWAP(Actor, setJob, swapJob, withJob, Costume&, inline, public, job)
        PROP_FIELD_WITH(Actor, withSprite, QString&, inline, public, setSprite)
        PROP_FIELD(Actor, Extra, extra, void*, inline, public, protected, _actor_data->_extra)
        PROP_FIELD_GET_CUSTOM(maximumExperience, int, inline, public, _actor_data->_maxp)
        PROP_FIELD_GET_CUSTOM(currentActions, int, inline, public, _actor_data->_actions)
        PROP_FIELD_GET_CUSTOM(maximumLevel, int, inline, public, _actor_data->_max_lv)
        PROP_FIELD_GET_CUSTOM(initiative, int, inline, public, _actor_data->_init)
        PROP_FIELD_GET_CUSTOM(partySide, int, inline, public, _actor_data->_side)
        //PROP_FIELD_GET_CUSTOM(extra, void*, inline, public, _actor_data->_extra)
        PROP_FIELD_GET_CUSTOM(currentExperience, int, inline, public, _actor_data->_xp)
        PROP_FIELD_GET_CUSTOM(currentLevel, int, inline, public, _actor_data->_lv)
    public:
        enum Attribute {
            RandomAi = 2048,
            AiPlayer = 4096
        };
        Q_DECLARE_FLAGS(Attributes, Attribute)
        Q_FLAG(Attributes)

        static QString KoTxt;
        static QString RiseTxt;

        QMap<Ability*, int> items() const;
        //int getPartySide() const;
        Costume& race() const;
        Costume& job() const;

        int remainingSkillUses(const Ability& skill) const;
        int regeneratingSkillTurn(const Ability& skill) const;

        const Costume* unequipPos(char const pos);
        const Costume* equipItem(char const pos, Costume* const item);
        char unequipItem(Costume& item);

        void recover(QString& ret);
        //Actor& applyRoles(QString& ret);
        void applyStates(QString& ret, bool const consume);
        void setStateResistance(const State* const state, int const res);
        void setElementResistance(int const element, int const res);
        void setItems(const QSharedPointer<QMap<Ability*, int>>& items);
        void setCurrentExperience(int const xp);
        void setCurrentLevel(int const level);
        void setSprite(QString& value);
        void setJob(Costume& job);
        void setRace(Costume& race);
        void setMaximumActions(int const mActions) override;
        void setMaximumLevel(int const maxLv);
        /*void setAgility(int const agi);
        void setOffense(int const atk);
        void setDefense(int const def);
        void setWisdom(int const wis);
        void setSpirit(int const spi);*/
        void setMaximumHp(int const mHp) override;
        void setMaximumMp(int const mMp) override;
        void setMaximumRp(int const mSp) override;
        void setCurrentHp(int const hp) override;
        void setCurrentMp(int const mp) override;
        void setCurrentRp(int const sp) override;

        Actor(int const id, QString& name, QString& sprite, Costume& race, Costume& job, int const level, int const maxLv, int const mActions, int const mHp, int const mMp,
              int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, QMap<int, int>* const res, QMap<const State*, int>* const stRes,
              const QSharedPointer<QMap<Ability*, int>>& items, QObject* const parent = NIL);

        Actor(int const id, QString&& name, QString&& sprite, Costume&& race, Costume&& job, int const level, int const maxLv, int const mActions, int const mHp, int const mMp,
              int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, QMap<int, int>* const res, QMap<const State*, int>* const stRes,
              const QSharedPointer<QMap<Ability*, int>>&& items, QObject* const parent = NIL);

        explicit Actor(QObject* const parent = NIL);

        Actor(const Actor& actor);

        ~Actor();
    protected:
        class ActorData : public QSharedData
        {
        public:
            ~ActorData();

        protected:
            int _lv, _max_lv, _xp, _maxp, _old_side, _init, _side, _actions;
            QMap<const Ability*, int>* _skills_cr_qty,* _skills_rg_turn;
            QSharedPointer<QMap<Ability*, int>> _items;
            QVector<const Costume*>* _dmg_roles;
            QMap<char, Costume*> _equipment;
            void* _extra;

            friend class Scene;
            friend class Actor;
            friend class Ability;
            friend class ArenaWidget;
            friend class TargetsModel;
            friend class SkillsModel;
            friend class ItemsModel;
            friend class Costume;
            friend class State;
            friend class Role;
        };

        QSharedDataPointer<ActorData> _actor_data;

        void levelUp();
        inline void checkRegSkill(const Ability& skill);
        void recover(QString* const ret);
        void applyDmgRoles(QString& ret);
        void removeStates(QString* const ret, bool const remove);
        void applyStates(QString* const ret, bool const consume);
        void updateStates(bool const remove, QString* const ret,
                            QMap<State*, int>& states, bool const includeWithDur);
        void updateAttributes(bool const remove, const Costume& costume);
        void updateSkills(bool const remove, bool const counters, QVector<Ability*>& skills);
        void updateResistance(bool const remove, QMap<int, int>* const elmRes, QMap<const State*, int>* const stRes);
        void switchCostume(QString* const ret, const Costume* const oldCostume, const Costume* const newCostume);
        void setCurrentHp(int const hp, QString* const ret, bool const survive);
        void refreshCostumes(QString* const ret);

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

Q_DECLARE_METATYPE(tbrpgsca::Actor)
Q_DECLARE_METATYPE(tbrpgsca::Actor*)
Q_DECLARE_INTERFACE(tbrpgsca::Actor, "com.codycostea.tbrpgsca.Actor")

Q_DECLARE_OPERATORS_FOR_FLAGS(tbrpgsca::Actor::Attributes)

#endif // ACTOR_H
