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
#include "role.h"
#include "actor.h"
#include "state.h"

#include <QStringBuilder>

using namespace tbrpgsca;

QString Role::HpTxt = "HP";
QString Role::MpTxt = "MP";
QString Role::RpTxt = "RP";
QString Role::CausesTxt = " %s causes %s";

QString& Role::AddDmgText(QString& ret, int dmgHp, int dmgMp, int dmgSp)
{
    bool c = false;
    if (dmgHp != 0)
    {
        ret = ret % " ";
        if (dmgHp < 0)
        {
            ret = ret % "+";
        }
        ret = ret % (QString("%d %s").arg(QString(-dmgHp), Role::HpTxt));
        c = true;
    }
    if (dmgMp != 0)
    {
        if (c)
        {
            ret = ret % ",";
        }
        ret = ret % " ";
        if (dmgMp < 0)
        {
            ret = ret % "+";
        }
        ret = ret % (QString("%d %s").arg(QString(-dmgMp), Role::MpTxt));
        c = true;
    }
    if (dmgSp != 0)
    {
        if (c)
        {
            ret = ret % ",";
        }
        ret = ret % " ";
        if (dmgSp < 0)
        {
            ret = ret % "+";
        }
        ret = ret % (QString("%d %s").arg(QString(-dmgSp), Role::RpTxt));
        c = true;
    }
    return ret;
}

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
    return this->sprite;
}

inline int Role::getDmgElement() const
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

inline Role& Role::apply(QString& ret, Actor& actor)
{
    return this->apply(ret, nullptr, actor);
}

inline Role& Role::adopt(Actor& actor)
{
    Role& role = *this;
    actor.dmgType |= role.dmgType;
    actor.flags |= role.flags;
    return role;
}

Role& Role::abandon(Actor& actor)
{
    Role& role = *this;
    {
        int const roleElm = role.dmgType;
        int const actorElm = actor.dmgType;
        if ((actorElm & roleElm) == roleElm)
        {
            actor.dmgType = actorElm ^ roleElm;
        }
    }
    {
        int const roleFlags = role.flags;
        int const actorFlags = actor.flags;
        if ((actorFlags & roleFlags) == roleFlags)
        {
            actor.flags = actorFlags ^ roleFlags;
        }
        /*if ((roleFlags & FLAG_RANGE) == FLAG_RANGE && (actorFlags & FLAG_RANGE) == FLAG_RANGE)
        {
            actor.flags = actorFlags ^ FLAG_RANGE;
        }
        if ((roleFlags & FLAG_REVIVE) == FLAG_REVIVE && (actorFlags & FLAG_REVIVE) == FLAG_REVIVE)
        {
            actor.flags = actorFlags ^ FLAG_REVIVE;
        }*/
    }
    return role;
}

Role& Role::apply(QString& ret, Scene* scene, Actor& actor)
{
    Role& role = *this;
    int rnd = std::rand() % 3;
    int dmgHp = (actor.mHp + rnd) * role.hp / 100;
    int dmgMp = (actor.mMp + rnd) * role.mp / 100;
    int dmgSp = (actor.mSp + rnd) * role.sp / 100;
    int actorHp = actor.hp;
    if (scene != nullptr)
    {
        actor.setCurrentHp(actorHp > dmgHp ? actorHp - dmgHp : 1, ret, *scene);
    }
    actor.setCurrentMp(actor.mp - dmgMp);
    actor.setCurrentRp(actor.sp - dmgSp);
    ret = ret % QString(Role::CausesTxt).arg(role.name, actor.name);
    Role::AddDmgText(ret, dmgHp, dmgMp, dmgSp);
    return role;
}

Role::Role(int const id, QString& name, QString& sprite, int const hpDmg, int const mpDmg, int const spDmg,
           int const mHp, int const mMp, int const mSp, int const element, bool const range, bool const revive)
{
    this->id = id;
    this->name = name;
    if (sprite.length() > 0)
    {
        this->sprite = new QString(sprite);
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
    this->flags = flags;
}

Role::Role(Role& role)
{
    this->id = role.id;
    this->name = role.name;
    this->sprite = role.sprite;
    this->flags = role.flags;
    this->mHp = role.mHp;
    this->mMp = role.mMp;
    this->mSp = role.mSp;
    this->dmgType = role.dmgType;
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
