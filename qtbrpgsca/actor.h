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
        PROP_FLAG_GET(hasQtySkills, Attribute::QTY_SKILLS, inline, public)
        PROP_FLAG_SET_ALL(Actor, HasQtySkills, Attribute::QTY_SKILLS, inline, protected, hasQtySkills)
        PROP_FLAG(Actor, AiPlayer, aiPlayer, Attribute::AI_PLAYER, inline, public, public)
        PROP_FLAG(Actor, RandomAi, randomAi, Attribute::RANDOM_AI, inline, public, public)
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
        //PROP_FIELD_SET_ALL(Actor, setExtra, sawpExtra, withExtra, void*, inline, public, extra, actorData()._extra)
        PROP_FIELD_WITH_SWAP(Actor, setCurrentLevel, swapCurrentLevel, withCurrentLevel, int, inline, public, currentLevel)
        PROP_FIELD_SET_ALL(Actor, setCurrentActions, sawpCurrentActions, withCurrentActions, int, inline, public, currentActions, actorData()._actions)
        PROP_FIELD_SET_ALL(Actor, setPartySide, sawpPartySide, withPartySide, int, inline, public, partySide, actorData()._side)
        PROP_FIELD_SET_ALL(Actor, setInitiative, sawpInitiative, withInitiative, int, inline, public, initiative, actorData()._init)
        PROP_FIELD_SET_ALL(Actor, setMaximumExperience, sawpMaximumExperience, withMaximumExperience, int, inline, public, maximumExperience, actorData()._maxp)
        PROP_FIELD_SWAP(swapSprite, setSprite, QString, QString&, inline, public, sprite)
        PROP_FIELD_WITH_SWAP(Actor, setJob, swapJob, withJob, Costume&, inline, public, job)
        PROP_FIELD_WITH(Actor, withSprite, QString&, inline, public, setSprite)
        PROP_FIELD(Actor, Extra, extra, void*, inline, public, protected, actorData()._extra)
        PROP_FIELD_GET_CUSTOM(maximumExperience, int, inline, public, actorData()._maxp)
        PROP_FIELD_GET_CUSTOM(currentActions, int, inline, public, actorData()._actions)
        PROP_FIELD_GET_CUSTOM(maximumLevel, int, inline, public, actorData()._max_lv)
        PROP_FIELD_GET_CUSTOM(initiative, int, inline, public, actorData()._init)
        PROP_FIELD_GET_CUSTOM(partySide, int, inline, public, actorData()._side)
        //PROP_FIELD_GET_CUSTOM(extra, void*, inline, public, actorData()._extra)
        PROP_FIELD_GET_CUSTOM(currentExperience, int, inline, public, actorData()._xp)
        PROP_FIELD_GET_CUSTOM(currentLevel, int, inline, public, actorData()._lv)
        PROP_CUSTOM_NEW_SHR_REF(Actor, ItemsTest, itemsTest, Ability, inline, public, public, _itemsTest)
    public:
        enum Attribute {
            RANDOM_AI = 2048,
            AI_PLAYER = 4096,
            QTY_SKILLS = 8192
        };
        Q_DECLARE_FLAGS(Attributes, Attribute)
        Q_FLAG(Attributes)

        static QString KoTxt;
        static QString RiseTxt;

        Q_INVOKABLE QMap<Ability*, int> items() const;
        //int getPartySide() const;
        Q_INVOKABLE Costume& race() const;
        Q_INVOKABLE Costume& job() const;

        //int remainingSkillUses(const Ability& skill) const;
        //int regeneratingSkillTurn(const Ability& skill) const;

        Q_INVOKABLE const Costume* unequipPos(char const pos);
        Q_INVOKABLE const Costume* equipItem(char const pos, Costume* const item);
        Q_INVOKABLE char unequipItem(Costume& item);

        Q_INVOKABLE void recover(QString& ret);
        //Actor& applyRoles(QString& ret);
        Q_INVOKABLE void applyStates(QString& ret, bool const consume);
        Q_INVOKABLE void setStateResistance(const int state, int const res);
        Q_INVOKABLE void setElementResistance(int const element, int const res);
        Q_INVOKABLE void setItems(const QSharedPointer<QMap<Ability*, int>>& items);
        Q_INVOKABLE void setCurrentExperience(int const xp);
        Q_INVOKABLE void setCurrentLevel(int const level);
        Q_INVOKABLE void setSprite(QString& value);
        Q_INVOKABLE void setJob(Costume& job);
        Q_INVOKABLE void setRace(Costume& race);
        Q_INVOKABLE void setMaximumActions(int const mActions) override;
        Q_INVOKABLE void setMaximumLevel(int const maxLv);
        /*void setAgility(int const agi);
        void setOffense(int const atk);
        void setDefense(int const def);
        void setWisdom(int const wis);
        void setSpirit(int const spi);*/
        Q_INVOKABLE void setMaximumHp(int const mHp) override;
        Q_INVOKABLE void setMaximumMp(int const mMp) override;
        Q_INVOKABLE void setMaximumRp(int const mSp) override;
        Q_INVOKABLE void setCurrentHp(int const hp) override;
        Q_INVOKABLE void setCurrentMp(int const mp) override;
        Q_INVOKABLE void setCurrentRp(int const sp) override;

        Actor(int const id, QString& name, QString& sprite, Costume& race, Costume& job, int const level, int const maxLv, int const mActions, int const mHp, int const mMp,
              int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, QMap<int, int>* const res, QMap<int, int>* const stRes,
              const QSharedPointer<QMap<Ability*, int>>& items, QObject* const parent = NIL);

        Actor(int const id, QString&& name, QString&& sprite, Costume&& race, Costume&& job, int const level, int const maxLv, int const mActions, int const mHp, int const mMp,
              int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, QMap<int, int>* const res, QMap<int, int>* const stRes,
              const QSharedPointer<QMap<Ability*, int>>&& items, QObject* const parent = NIL);

        explicit Actor(QObject* const parent = NIL);

        Actor(const Actor&& actor);

        Actor(const Actor& actor);

        virtual ~Actor();
    protected:
        class ActorSheet : public CostumeSheet
        {
        public:
            virtual ~ActorSheet();

        protected:
            int _lv, _max_lv, _xp, _maxp, _old_side, _init, _side, _actions;
            //QMap<const Ability*, int>* _skills_cr_qty,* _skills_rg_turn;
            QSharedPointer<QMap<Ability*, int>> _items;
            QVector<const Costume*>* _dmg_roles;
            QMap<char, Costume*> _equipment;
            void* _extra;

            ActorSheet(int const id, QString& name, QString& sprite, /*Costume& race, Costume& job, int const level,*/ int const maxLv, int const mActions,
                       int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
                       QMap<int, int>* const res, QMap<int, int>* const stRes, const QSharedPointer<QMap<Ability*, int>>& items);

            friend class Scene;
            friend class Actor;
            friend class Ability;
            friend class ArenaWidget;
            friend class TargetsModel;
            friend class SkillsModel;
            friend class ItemsModel;
            friend class Costume;
            friend class Ailment;
            friend class State;
            friend class Role;
        };

        inline ActorSheet& actorMutData()
        {
            return (*static_cast<ActorSheet*>(this->_play_data.data()));
        }

        inline const ActorSheet& actorData() const
        {
            return (*static_cast<const ActorSheet*>(this->_play_data.data()));
        }

        void levelUp();
        //inline void checkRegSkill(const Ability& skill);
        void recover(QString* const ret);
        void applyDmgRoles(QString& ret);
        void removeStates(QString* const ret, bool const remove);
        void applyStates(QString* const ret, bool const consume);
        void updateAttributes(bool const remove, const Costume& costume);
        void updateSkills(bool const remove, bool const counters, QList<Ability>& skills);
        void updateStates(bool const remove, QString* const ret, QList<Ailment>& states, bool const includeWithDur);
        void updateResistance(bool const remove, QMap<int, int>* const elmRes, QMap<int, int>* const stRes);
        void switchCostume(QString* const ret, const Costume* const oldCostume, const Costume* const newCostume);
        void setCurrentHp(int const hp, QString* const ret, bool const survive);
        void refreshCostumes(QString* const ret);

        explicit Actor(QObject* const parent, ActorSheet* const actorDataPtr);

        friend class Scene;
        friend class Ability;
        friend class ArenaWidget;
        friend class TargetsModel;
        friend class SkillsModel;
        friend class ItemsModel;
        friend class Costume;
        friend class Ailment;
        friend class State;
        friend class Role;
    };

}

Q_DECLARE_METATYPE(tbrpgsca::Actor)
Q_DECLARE_METATYPE(tbrpgsca::Actor*)
Q_DECLARE_INTERFACE(tbrpgsca::Actor, "com.codycostea.tbrpgsca.Actor")

Q_DECLARE_OPERATORS_FOR_FLAGS(tbrpgsca::Actor::Attributes)

#endif // ACTOR_H
