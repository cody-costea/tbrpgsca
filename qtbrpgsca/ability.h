/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef ABILITY_H
#define ABILITY_H

#include "role.h"

namespace tbrpgsca
{

    class Actor;
    class Scene;

    class Ability : public Role
    {
        Q_OBJECT
        QML_ELEMENT
        PROP_FLAG(Ability, Stealing, steals, Attribute::STEAL, inline, public, public)
        PROP_FLAG(Ability, OnlyMelee, melee, Attribute::MELEE, inline, public, public)
        PROP_FLAG(Ability, Missable, misses, Attribute::MISSABLE, inline, public, public)
        PROP_FLAG(Ability, Absorbing, absorbs, Attribute::ABSORB, inline, public, public)
        PROP_FLAG(Ability, TargetingAll, targetsAll, Attribute::TARGET_ALL, inline, public, public)
        PROP_FLAG(Ability, TargetingSide, targetsSide, Attribute::TARGET_SIDE, inline, public, public)
        PROP_FLAG(Ability, TargetingSelf, targetsSelf, Attribute::TARGET_SELF, inline, public, public)
        PROP_FIELD(Ability, RequiredLevel, requiredLevel, int, inline, public, public, _ability_data->_lv_rq)
        PROP_FIELD(Ability, AttributeIncrement, attributeIncrement, int, inline, public, public, _ability_data->_attr_inc)
        PROP_FIELD(Ability, MaximumUses, maximumUses, int, inline, public, public, _ability_data->_max_qty)
        PROP_FIELD(Ability, UsesRegen, usesRegen, int, inline, public, public, _ability_data->_reg_qty)
        //PROP_FIELD(Ability, CurrentUses, currentUses, int, inline, public, public, _crt_qty)
        //PROP_FIELD(Ability, RegenTurn, regenTurn, int, inline, public, public, _reg_turn)
    public:
        inline static constexpr int DefaultRes = 3;

        enum Attribute {
            MELEE = 4,
            STEAL = 8,
            ABSORB = 16,
            MISSABLE = 32,
            TARGET_ALL = 4,
            TARGET_SIDE = 128,
            TARGET_SELF = 64,
            TARGET_ONE = 0
        };
        Q_DECLARE_FLAGS(Attributes, Attribute)
        Q_FLAG(Attributes)

        static QString MissesTxt;
        static QString ReflectTxt;
        static QString StolenTxt;

        QList<int> removedStatesList() const;
        int removedStateDuration(int state) const;
        bool removedState(int state) const;
        int removedStatesSize() const;

        Q_INVOKABLE void replenish();
        Q_INVOKABLE bool canPerform(Actor* const user) const;
        Q_INVOKABLE void execute(QString& ret, Actor& user, Actor& target, bool const applyCosts);

        Ability(int const id, QString& name, QString& sprite, QString& sound, bool const steal, bool const range, bool const melee, bool const canMiss, int const lvRq, int const hpCost,
                int const mpCost, int const spCost, int const dmgType, int const attrInc, int const hpDmg, int const mpDmg, int const spDmg, int const trg, int const elm, int const mQty,
                int const rQty, bool const absorb, bool const revive, QList<Ailment>* const aStates, QMap<int, int>* const rStates, QObject* const parent = NIL);

        Ability(int const id, QString&& name, QString&& sprite, QString&& sound, bool const steal, bool const range, bool const melee, bool const canMiss, int const lvRq, int const hpCost,
                int const mpCost, int const spCost, int const dmgType, int const attrInc, int const hpDmg, int const mpDmg, int const spDmg, int const trg, int const elm, int const mQty,
                int const rQty, bool const absorb, bool const revive, QList<Ailment>* const aStates, QMap<int, int>* const rStates, QObject* const parent = NIL);

        explicit Ability(QObject* const parent = NIL);

        Ability(const Ability& ability);

        virtual ~Ability();
    protected:
        class AbilityData : public QSharedData
        {
        public:
            ~AbilityData();

        protected:
            int _lv_rq, _attr_inc, _max_qty, _reg_qty;
            QMap<int, int>* _r_states;
            QString* _sound;

            friend class Actor;
            friend class Costume;
            friend class Ability;
            friend class ArenaWidget;
            friend class SkillsModel;
            friend class ItemsModel;
            friend class Ailment;
            friend class Scene;
        };

        //int _crt_qty, _reg_turn;
        QSharedDataPointer<AbilityData> _ability_data;

        void execute(QString& ret, Actor& user, Actor* target, bool const applyCosts);

        friend class Actor;
        friend class Costume;
        friend class ArenaWidget;
        friend class SkillsModel;
        friend class ItemsModel;
        friend class Ailment;
        friend class Scene;
    };

}

Q_DECLARE_METATYPE(tbrpgsca::Ability)
Q_DECLARE_METATYPE(tbrpgsca::Ability*)
Q_DECLARE_INTERFACE(tbrpgsca::Ability, "com.codycostea.tbrpgsca.Ability")

Q_DECLARE_OPERATORS_FOR_FLAGS(tbrpgsca::Ability::Attributes)

#endif // ABILITY_H
