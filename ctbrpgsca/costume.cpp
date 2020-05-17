/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "costume.h"
#include "ability.h"
#include "actor.h"
#include "state.h"
#include "scene.h"
#include "role.h"

#include <QStringBuilder>

using namespace tbrpgsca;

QString Costume::CausesTxt = ", %1 is affected by %2";

int Costume::getAttack() const
{
    return this->atk;
}

int Costume::getDefense() const
{
    return this->def;
}

int Costume::getSpirit() const
{
    return this->spi;
}

int Costume::getWisdom() const
{
    return this->wis;
}

int Costume::getAgility() const
{
    return this->agi;
}

int Costume::getMaximumActions() const
{
    return this->mActions;
}

Ability& Costume::getAddedSkill(int const n) const
{
    return *(this->aSkills->at(n));
}

bool Costume::hasAddedSkill(Ability& skill) const
{
    QVector<Ability*>* aSkills = this->aSkills;
    return aSkills != nullptr && aSkills->contains(&skill);
}

int Costume::getAddedSkillsSize() const
{
    QVector<Ability*>* aSkills = this->aSkills;
    return aSkills == nullptr ? 0 : aSkills->size();
}

Ability& Costume::getCounterSkill(int const n) const
{
    return *(this->counters->at(n));
}

bool Costume::hasCounterSkill(Ability& skill) const
{
    QVector<Ability*>* counters = this->counters;
    return counters != nullptr && counters->contains(&skill);
}

int Costume::getCounterSkillsSize() const
{
    QVector<Ability*>* counters = this->counters;
    return counters == nullptr ? 0 : counters->size();
}

int Costume::getElementResistance(const int element) const
{
    QMap<int, int>* res = this->res;
    return res == nullptr ? 0 : res->value(element, 0);
}

int Costume::getStateResistance(State* const state) const
{
    QMap<State*, int>* stRes = this->stRes;
    return stRes == nullptr ? 0 : stRes->value(state, 0);
}

bool Costume::isReflecting() const
{
    return (this->flags & FLAG_REFLECT) == FLAG_REFLECT;
}

bool Costume::isCountering() const
{
    QVector<Ability*>* counters = this->counters;
    return counters != nullptr && counters->size() > 0;
}

bool Costume::isShapeShifted() const
{
    return (this->flags & FLAG_SHAPE_SHIFT) == FLAG_SHAPE_SHIFT;
}

bool Costume::isEnraged() const
{
    return (this->flags & FLAG_ENRAGED) == FLAG_ENRAGED;
}

bool Costume::isConfused() const
{
    return (this->flags & FLAG_CONFUSE) == FLAG_CONFUSE;
}

bool Costume::isConverted() const
{
    return (this->flags & FLAG_CONVERT) == FLAG_CONVERT;
}

bool Costume::isInvincible() const
{
    return (this->flags & FLAG_INVINCIBLE) == FLAG_INVINCIBLE;
}

bool Costume::isStunned() const
{
    return (this->flags & FLAG_STUN) == FLAG_STUN;
}

bool Costume::isKnockedOut() const
{
    return (this->flags & FLAG_KO) == FLAG_KO;
}

Costume& Costume::adopt(QString& ret, Actor& actor)
{
    return this->adopt(&ret, nullptr, actor, true, false);
}

Costume& Costume::abandon(QString& ret, Actor& actor)
{
    return this->adopt(&ret, nullptr, actor, true, true);
}

Costume& Costume::apply(QString& ret, Actor& actor)
{
    return this->apply(ret, nullptr, actor);
}

Costume& Costume::adopt(QString* const ret, Scene* const scene, Actor& actor, bool const updStates, bool const remove)
{
    Costume& costume = *this;
    actor.updateAttributes(remove, scene, costume);
    actor.updateResistance(remove, costume.res, costume.stRes);
    if (remove)
    {
        int const roleFlags = costume.flags;
        int const actorFlags = actor.flags;
        if ((actorFlags & roleFlags) == roleFlags)
        {
            actor.flags = actorFlags ^ roleFlags;
        }
        if (costume.isShapeShifted() && costume.sprite != nullptr)
        {
            (*actor.sprite) = *(actor.getJob().sprite);
        }
        if (costume.hp == 0 && costume.mp == 0 && costume.sp == 0)
        {
            int const roleElm = costume.dmgType;
            int const actorElm = actor.dmgType;
            if ((actorElm & roleElm) == roleElm)
            {
                actor.dmgType = actorElm ^ roleElm;
            }
        }
        else
        {
            QVector<Costume*>* const dmgRoles = actor.dmgRoles;
            if (dmgRoles != nullptr)
            {
                dmgRoles->removeOne(&costume);
            }
        }
        costume.refresh(ret, scene, actor, updStates, true);
        actor.refreshCostumes(ret, scene);
    }
    else
    {
        if (updStates)
        {
            QMap<State*, int>* const cStates = costume.stateDur;
            if (cStates != nullptr)
            {
                actor.updateStates(false, ret, scene, *cStates, true);
            }
        }
        if (costume.hp != 0 || costume.mp != 0 || costume.sp != 0)
        {
            QVector<Costume*>* dmgRoles = actor.dmgRoles;
            if (dmgRoles == nullptr)
            {
                dmgRoles = new QVector<Costume*>();
                actor.dmgRoles = dmgRoles;
            }
            dmgRoles->append(&costume);
        }
        costume.refresh(ret, scene, actor, false, false);
    }
    return costume;
}

Costume& Costume::apply(QString& ret, Scene* scene, Actor& actor)
{
    Costume& role = *this;
    ret = ret % QString(Costume::CausesTxt).arg(actor.name, role.name);
    role.damage(ret, scene, nullptr, actor, std::rand() % 4, true);
    return role;
}

Costume& Costume::refresh(QString* const ret, Scene* const scene, Actor& actor, bool const updStates, bool const remove)
{
    Costume& costume = *this;
    {
        QVector<Ability*>* skills = costume.aSkills;
        if (skills != nullptr)
        {
            actor.updateSkills(remove, false, *skills);
        }
        skills = costume.counters;
        if (skills != nullptr)
        {
            actor.updateSkills(remove, true, *skills);
        }
    }
    if (updStates)
    {
        QMap<State*, int>* cStates = costume.stateDur;
        if (cStates != nullptr)
        {
            actor.updateStates(remove, ret, scene, *cStates, false);
        }
    }
    if (!remove)
    {
        int const cFlags = costume.flags;
        if (costume.hp == 0 && costume.mp == 0 && costume.sp == 0)
        {
            actor.dmgType |= costume.dmgType;
        }
        actor.flags |= cFlags;
        if (scene != nullptr && ret != nullptr && (cFlags & FLAG_KO) == FLAG_KO)
        {
            scene->checkStatus(*ret);
        }
        if (costume.isShapeShifted())
        {
            QString* const spr = costume.sprite;
            if (spr != nullptr)
            {
                (*actor.sprite) = *spr;
            }
        }
    }
    return actor;
}

Costume::Costume(int const id, QString& name, QString& sprite, bool const shapeShift, int const mActions, int const elm, int const hpDmg, int const mpDmg, int const spDmg,
                 int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun, bool const range,
                 bool const automate, bool const confuse, bool const reflect, bool const ko, bool const invincible, bool const revive, QVector<Ability*>* const skills,
                 QVector<Ability*>* const counters, QMap<State*, int>* const states, QMap<State*, int>* const stRes, QMap<int, int>* const res)
    : Role(id, name, sprite, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, elm, range, revive, states)
{
    this->atk = atk;
    this->def = def;
    this->spi = spi;
    this->wis = wis;
    this->agi = agi;
    this->res = res;
    this->mActions = mActions;
    this->stRes = stRes;
    this->aSkills = skills;
    this->counters = counters;
    int flags = this->flags;
    if (stun)
    {
        flags |= FLAG_STUN;
    }
    if (shapeShift)
    {
        QString* spr = this->sprite;
        if (spr != nullptr && spr->length() > 0)
        {
            flags |= FLAG_SHAPE_SHIFT;
        }
    }
    if (automate)
    {
        flags |= FLAG_ENRAGED;
    }
    if (confuse)
    {
        flags |= FLAG_CONFUSE;
    }
    if (reflect)
    {
        flags |= FLAG_REFLECT;
    }
    if (invincible)
    {
        flags |= FLAG_INVINCIBLE;
    }
    if (ko)
    {
        flags |= FLAG_KO;
    }
    this->flags = flags;
}

Costume::Costume(int const id, QString name, QString sprite, bool const shapeShift, int const mActions, int const elm, int const hpDmg, int const mpDmg, int const spDmg,
                 int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const range, bool const automate,
                 bool const confuse, bool const reflect, bool const invincible, bool const revive, QVector<Ability*>* const skills, QVector<Ability*>* const counters,
                 QMap<State*, int>* const states, QMap<State*, int>* const stRes, QMap<int, int>* const res)
    : Costume(id, name, sprite, shapeShift, mActions, elm, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis, agi, false, range, automate, confuse, reflect, false, invincible,
              revive, skills, counters, states, stRes, res)
{

}

Costume::Costume(Costume& costume) : Role(costume)
{
    this->atk = costume.atk;
    this->def = costume.def;
    this->spi = costume.spi;
    this->wis = costume.wis;
    this->agi = costume.agi;
    this->res = costume.res;
    this->aSkills = costume.aSkills;
    this->mActions = costume.mActions;
    this->stRes = costume.stRes;
}

Costume::~Costume()
{

}
