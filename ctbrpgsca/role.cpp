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

QString& Role::AddDmgText(QString& ret, int dmgHp, int dmgMp, int dmgSp)
{
    bool c = false;
    if (dmgHp != 0)
    {
        ret = ret % (dmgHp < 0 ? " +" : " ") % (QString("%d %s").arg(QString(-dmgHp), Role::HpTxt));
        c = true;
    }
    if (dmgMp != 0)
    {
        if (c)
        {
            ret = ret % ",";
        }
        else
        {
            c = true;
        }
        ret = ret % (dmgMp < 0 ? " +" : " ") % (QString("%d %s").arg(QString(-dmgMp), Role::MpTxt));
    }
    if (dmgSp != 0)
    {
        if (c)
        {
            ret = ret % ",";
        }
        else
        {
            c = true;
        }
        ret = ret % (dmgSp < 0 ? " +" : " ") % (QString("%d %s").arg(QString(-dmgSp), Role::RpTxt));
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

inline QString* Role::getSprite() const
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
