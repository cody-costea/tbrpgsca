/*
Copyright (C) AD 2013-2019 Claudiu-Stefan Costea

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "actor.h"
#include "ability.h"
#include "costume.h"
#include "state.h"
#include "scene.h"
#include "role.h"

#include <QStringBuilder>

using namespace tbrpgsca;

Actor& Actor::setAgility(const int agi)
{
    this->agi = agi;
    return *this;
}

Actor& Actor::setAgility(const int agi, Scene& scene)
{
    Actor& actor = this->setAgility(agi);
    scene.agiCalc();
    return actor;
}

Actor& Actor::setCurrentHp(const int hp, QString& ret, Scene& scene)
{
    Actor& actor = this->setCurrentHp(hp);
    if (hp < 1)
    {
        scene.checkStatus(ret);
    }
    return actor;
}

Actor& Actor::setCurrentHp(const int hp)
{
    int mHp = this->mHp;
    this->hp = hp > mHp ? mHp : (hp < 0 ? 0 : hp);
    return *this;
}

Actor& Actor::setCurrentMp(const int mp)
{
    int mMp = this->mMp;
    this->mp = mp > mMp ? mMp : (mp < 0 ? 0 : mp);
    return *this;
}

Actor& Actor::setCurrentRp(const int sp)
{
    int mSp = this->mSp;
    this->sp = sp > mSp ? mSp : (sp < 0 ? 0 : sp);
    return *this;
}

Actor& Actor::setMaximumHp(const int mHp)
{
    this->mHp = mHp;
    return *this;
}

Actor& Actor::setMaximumMp(const int mMp)
{
    this->mMp = mMp;
    return *this;
}

Actor& Actor::setMaximumRp(const int mMp)
{
    this->mSp = mMp;
    return *this;
}

Actor& Actor::setName(QString& value)
{
    this->name = value;
    return *this;
}

Actor& Actor::setSprite(QString& value)
{
    Actor& actor = *this;
    QString* sprite = actor.sprite;
    if (sprite != nullptr)
    {
        delete sprite;
    }
    actor.sprite = new QString(value);
    return *this;
}

Actor& Actor::setRanged(const bool range)
{
    int flags = this->flags;
    if (range != ((flags & FLAG_RANGE) == FLAG_RANGE))
    {
        this->flags = flags ^ FLAG_RANGE;
    }
    return *this;
}

Actor& Actor::setReviving(const bool revive)
{
    int flags = this->flags;
    if (revive != ((flags & FLAG_REVIVE) == FLAG_REVIVE))
    {
        this->flags = flags ^ FLAG_REVIVE;
    }
    return *this;
}

Actor& Actor::updateSkills(const bool remove, const bool counters, QVector<Ability*>& skills)
{
    Actor& actor = *this;
    QVector<Ability*>* aSkills = counters ? actor.counters : actor.aSkills;
    if (remove)
    {
        if (aSkills != nullptr)
        {
            for (Ability* const ability : skills)
            {
                aSkills->removeOne(ability);
                if (ability->rQty > 0)
                {
                    QMap<Ability*, int>* regTurn = actor.skillsRgTurn;
                    if (regTurn != nullptr)
                    {
                        regTurn->remove(ability);
                    }
                }
                if (ability->mQty > 0)
                {
                    QMap<Ability*, int>* crQty = actor.skillsCrQty;
                    if (crQty != nullptr)
                    {
                        crQty->remove(ability);
                    }
                }
            }
        }
    }
    else
    {
        if (aSkills == nullptr)
        {
            aSkills = new QVector<Ability*>();
            if (counters)
            {
                this->counters = aSkills;
            }
            else
            {
                this->aSkills = aSkills;
            }
        }
        for (Ability* const ability : skills)
        {
            if (!aSkills->contains(ability))
            {
                aSkills->append(ability);
                /*int const mQty = ability->mQty;
                if (mQty > 0)
                {
                    QMap<Ability*, int>* crQty = actor.skillsCrQty;
                    if (crQty == nullptr)
                    {
                        crQty = new QMap<Ability*, int>();
                        actor.skillsCrQty = crQty;
                    }
                    crQty->operator[](ability) = mQty;
                }*/
            }
        }
    }
    return actor;
}

Actor& Actor::refreshCostume(Costume& costume)
{
    Actor& actor = *this;
    actor.dmgType |= costume.dmgType;
    actor.flags |= costume.flags;
    QVector<Ability*>* skills = costume.aSkills;
    if (skills != nullptr)
    {
        actor.updateSkills(false, false, *skills);
    }
    skills = costume.counters;
    if (skills != nullptr)
    {
        actor.updateSkills(false, true, *skills);
    }
    return actor;
}

Actor& Actor::refreshCostumes()
{
    Actor& actor = *this;
    for (Costume* const costume : actor.equipment.values())
    {
        actor.refreshCostume(*costume);
    }
    QMap<State*, int>* stateDur = actor.stateDur;
    if (stateDur != nullptr)
    {
        for (State* const state : stateDur->keys())
        {
            if (stateDur->value(state, -3) > -3)
            {
                actor.refreshCostume(*state);
            }
        }
    }
    return actor;
}

Actor::~Actor()
{
    auto stRes = this->stRes;
    if (stRes != nullptr)
    {
        this->stRes = nullptr;
        delete stRes;
    }
    auto res = this->res;
    if (res != nullptr)
    {
        this->res = nullptr;
        delete res;
    }
    auto states = this->stateDur;
    if (states != nullptr)
    {
        this->stateDur = nullptr;
        delete states;
    }
    auto skills = this->aSkills;
    if (skills != nullptr)
    {
        this->aSkills = nullptr;
        delete skills;
    }
    auto dmgRoles = this->dmgRoles;
    if (dmgRoles != nullptr)
    {
        this->dmgRoles = nullptr;
        delete dmgRoles;
    }
}
