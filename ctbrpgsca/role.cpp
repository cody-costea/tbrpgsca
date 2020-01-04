/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "role.h"
#include "actor.h"
#include "state.h"

#include <QStringBuilder>

using namespace tbrpgsca;

QString Role::HpTxt = "HP";
QString Role::MpTxt = "MP";
QString Role::RpTxt = "RP";

inline int Role::getId() const
{
    return this->id;
}

inline QString Role::getName() const
{
    return this->name;
}

inline QString Role::getSprite() const
{
    QString* spr = this->sprite;
    return spr == nullptr ? QString() : *(this->sprite);
}

inline int Role::getDamageElement() const
{
    return this->dmgType;
}

inline int Role::getMaximumHp() const
{
    return this->mHp;
}

inline int Role::getMaximumMp() const
{
    return this->mMp;
}

inline int Role::getMaximumRp() const
{
    return this->mSp;
}

inline int Role::getCurrentHp() const
{
    return this->hp;
}

inline int Role::getCurrentMp() const
{
    return this->mp;
}

inline int Role::getCurrentRp() const
{
    return this->sp;
}

inline bool Role::isRanged() const
{
    return (this->flags & FLAG_RANGE) == FLAG_RANGE;
}

inline bool Role::isReviving() const
{
    return (this->flags & FLAG_REVIVE) == FLAG_REVIVE;
}

inline int Role::getStateDuration(State& state) const
{
    QMap<State*, int>* aStates = this->stateDur;
    return aStates == nullptr ? 0 : aStates->value(&state, 0);
}

inline QList<State*> Role::getStatesList() const
{
    QMap<State*, int>* aStates = this->stateDur;
    return aStates == nullptr ? QList<State*>() : aStates->keys();
}

inline bool Role::hasState(State& state) const
{
    QMap<State*, int>* aStates = this->stateDur;
    return aStates != nullptr && aStates->contains(&state);
}

inline int Role::getStatesSize() const
{
    QMap<State*, int>* aStates = this->stateDur;
    return aStates == nullptr ? 0 : aStates->size();
}

inline bool Role::operator==(Role& role) const
{
    return this->id == role.id;
}

inline Role& Role::damage(QString& ret, Actor* const absorber, Actor& target, int const dmg, bool const percent)
{
    return this->damage(ret, nullptr, absorber, target, dmg, percent);
}

Role& Role::damage(QString& ret, Scene* scene, Actor* const absorber, Actor& actor, int dmg, bool const percent)
{
    Role& role = *this;
    if (!actor.isInvincible())
    {
        int dmgHp, dmgMp, dmgSp;
        if (percent)
        {
            dmgHp = (actor.mHp + dmg) * role.hp / 100;
            dmgMp = (actor.mMp + dmg) * role.mp / 100;
            dmgSp = (actor.mSp + dmg) * role.sp / 100;
        }
        else
        {
            dmgHp = role.hp;
            dmgMp = role.mp;
            dmgSp = role.sp;
        }
        bool c = false;
        if (dmgSp != 0)
        {
            c = true;
            ret = ret % Ability::SuffersTxt.arg(actor.name) % " ";
            if (dmgSp < 0)
            {
                dmg =  -1 * dmg + dmgSp;
                ret = ret % "+";
            }
            else
            {
                dmg += dmgSp;
            }
            actor.setCurrentRp(actor.sp - dmgSp);
            ret = ret % (QString("%d %s").arg(QString(-dmgSp), Role::RpTxt));
        }
        if (dmgMp != 0)
        {
            if (c)
            {
                ret = ret % ", ";
            }
            else
            {
                ret = ret % Ability::SuffersTxt.arg(actor.name) % " ";
                c = true;
            }
            if (dmgMp < 0)
            {
                dmg =  -1 * dmg + dmgMp;
                ret = ret % "+";
            }
            else
            {
                dmg += dmgMp;
            }
            actor.setCurrentMp(actor.mp - dmgMp);
            ret = ret % (QString("%d %s").arg(QString(-dmgMp), Role::MpTxt));
        }
        if (dmgHp != 0)
        {
            if (c)
            {
                ret = ret % ", ";
            }
            else
            {
                ret = ret % Ability::SuffersTxt.arg(actor.name) % " ";
                c = true;
            }
            if (dmgHp < 0)
            {
                dmg =  -1 * dmg + dmgSp;
                ret = ret % "+";
            }
            else
            {
                dmg += dmgHp;
            }
            actor.setCurrentHp(actor.hp - dmgHp, ret, scene, percent);
            ret = ret % (QString("%d %s").arg(QString(-dmgHp), Role::HpTxt));
        }
        if (c && absorber != nullptr)
        {
            absorber->setCurrentRp(absorber->sp + dmgSp / 2);
            absorber->setCurrentMp(absorber->mp + dmgMp / 2);
            absorber->setCurrentHp(absorber->hp + dmgHp / 2, ret, scene, true);
        }
    }
    return role;
}

Role::Role(int const id, QString& name, QString* sprite, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
           int const mMp, int const mSp, int const element, bool const range, bool const revive, QMap<State*, int>* aStates)
{
    this->id = id;
    this->name = name;
    if (sprite != nullptr && sprite->length() > 0)
    {
        this->sprite = new QString(*sprite);
    }
    this->mHp = mHp;
    this->mHp = mMp;
    this->mSp = mSp;
    this->hp = hpDmg;
    this->mp = mpDmg;
    this->sp = spDmg;
    this->dmgType = element;
    int flags = revive ? FLAG_REVIVE : 0;
    if (range)
    {
        flags |= FLAG_RANGE;
    }
    this->stateDur = aStates;
    this->flags = flags;
}

Role::Role(Role& role)
{
    this->id = role.id;
    this->name = role.name;
    this->sprite = role.sprite;
    this->flags = role.flags;
    this->stateDur = role.stateDur;
    this->dmgType = role.dmgType;
    this->mHp = role.mHp;
    this->mMp = role.mMp;
    this->mSp = role.mSp;
    this->hp = role.hp;
    this->mp = role.mp;
    this->sp = role.sp;
}

Role::~Role()
{
    QString* sprite = this->sprite;
    if (sprite != nullptr)
    {
        this->sprite = nullptr;
        delete sprite;
    }
}
