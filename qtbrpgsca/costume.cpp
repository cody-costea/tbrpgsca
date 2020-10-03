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

Ability& Costume::addedSkill(int const n) const
{
    return *(this->_a_skills->at(n));
}

bool Costume::hasAddedSkill(Ability& skill) const
{
    QVector<Ability*>* aSkills = this->_a_skills;
    return aSkills && aSkills->contains(&skill);
}

int Costume::addedSkillsSize() const
{
    QVector<Ability*>* aSkills = this->_a_skills;
    return aSkills == nullptr ? 0 : aSkills->size();
}

Ability& Costume::counterSkill(int const n) const
{
    return *(this->_counters->at(n));
}

bool Costume::hasCounterSkill(Ability& skill) const
{
    QVector<Ability*>* counters = this->_counters;
    return counters && counters->contains(&skill);
}

int Costume::counterSkillsSize() const
{
    QVector<Ability*>* counters = this->_counters;
    return counters == nullptr ? 0 : counters->size();
}

int Costume::elementResistance(const int element) const
{
    QMap<int, int>* res = this->_res;
    return res == nullptr ? 0 : res->value(element, 0);
}

int Costume::stateResistance(State* const state) const
{
    QMap<State*, int>* stRes = this->_st_res;
    return stRes == nullptr ? 0 : stRes->value(state, 0);
}

bool Costume::isCountering() const
{
    QVector<Ability*>* counters = this->_counters;
    return counters && counters->size() > 0;
}

void Costume::adopt(QString& ret, Actor& actor)
{
    this->adopt(&ret, nullptr, actor, true, false);
}

void Costume::abandon(QString& ret, Actor& actor)
{
    this->adopt(&ret, nullptr, actor, true, true);
}

void Costume::apply(QString& ret, Actor& actor)
{
    this->apply(ret, nullptr, actor);
}

void Costume::adopt(QString* const ret, Scene* const scene, Actor& actor, bool const updStates, bool const remove)
{
    Costume& costume = *this;
    actor.updateAttributes(remove, scene, costume);
    actor.updateResistance(remove, costume._res, costume._st_res);
    if (remove)
    {
        int const roleFlags = costume.flags();
        int const actorFlags = actor.flags();
        if ((actorFlags & roleFlags) == roleFlags)
        {
            actor.setFlags(actorFlags ^ roleFlags);
        }
        if (costume.isShapeShifted() && costume._sprite)
        {
            (*actor._sprite) = *(actor.job()._sprite);
        }
        if (costume._hp == 0 && costume._mp == 0 && costume._sp == 0)
        {
            int const roleElm = costume.dmgType();
            int const actorElm = actor.dmgType();
            if ((actorElm & roleElm) == roleElm)
            {
                actor.setDmgType(actorElm ^ roleElm);
            }
        }
        else
        {
            QVector<Costume*>* const dmgRoles = actor._dmg_roles;
            if (dmgRoles)
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
            QMap<State*, int>* const cStates = costume._state_dur;
            if (cStates)
            {
                actor.updateStates(false, ret, scene, *cStates, true);
            }
        }
        if (costume._hp != 0 || costume._mp != 0 || costume._sp != 0)
        {
            QVector<Costume*>* dmgRoles = actor._dmg_roles;
            if (dmgRoles == nullptr)
            {
                dmgRoles = new QVector<Costume*>();
                actor._dmg_roles = dmgRoles;
            }
            dmgRoles->append(&costume);
        }
        costume.refresh(ret, scene, actor, false, false);
    }
}

void Costume::apply(QString& ret, Scene* scene, Actor& actor)
{
    Costume& role = *this;
    ret += QString(Costume::CausesTxt).arg(actor._name, role._name);
    role.damage(ret, scene, nullptr, actor, 0, true);
}

void Costume::refresh(QString* const ret, Scene* const scene, Actor& actor, bool const updStates, bool const remove)
{
    Costume& costume = *this;
    {
        QVector<Ability*>* skills = costume._a_skills;
        if (skills)
        {
            actor.updateSkills(remove, false, *skills);
        }
        skills = costume._counters;
        if (skills)
        {
            actor.updateSkills(remove, true, *skills);
        }
    }
    if (updStates)
    {
        QMap<State*, int>* cStates = costume._state_dur;
        if (cStates)
        {
            actor.updateStates(remove, ret, scene, *cStates, false);
        }
    }
    if (!remove)
    {
        int const cFlags = costume._flags;
        if (costume._hp == 0 && costume._mp == 0 && costume._sp == 0)
        {
            actor._dmg_type |= costume._dmg_type;
        }
        actor._flags |= cFlags;
        if (scene)
        {
#if ALLOW_COVERING
            if ((cFlags & FLAG_COVERING) == FLAG_COVERING)
            {
                scene->setHasCovers(true);
            }
#endif
            if (ret && (cFlags & FLAG_KO) == FLAG_KO)
            {
                scene->checkStatus(*ret);
            }
        }
        if (costume.isShapeShifted())
        {
            QString* const spr = costume._sprite;
            if (spr)
            {
                (*actor._sprite) = *spr;
            }
        }
    }
}

Costume::Costume(int const id, QString& name, QString& sprite, bool const shapeShift, int const mActions, int const elm, int const hpDmg, int const mpDmg, int const spDmg,
                 int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun, bool const range,
                 bool const automate, bool const confuse, bool const reflect, bool const ko, bool const invincible, bool const revive, QVector<Ability*>* const skills,
                 QVector<Ability*>* const counters, QMap<State*, int>* const states, QMap<State*, int>* const stRes, QMap<int, int>* const res)
    : Role(id, name, sprite, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, elm, range, revive, states)
{
    this->_atk = atk;
    this->_def = def;
    this->_spi = spi;
    this->_wis = wis;
    this->_agi = agi;
    this->_res = res;
    this->_m_actions = mActions;
    this->_st_res = stRes;
    this->_a_skills = skills;
    this->_counters = counters;
    int flags = this->_flags;
    if (stun)
    {
        flags |= FLAG_STUN;
    }
    if (shapeShift)
    {
        QString* spr = this->_sprite;
        if (spr && spr->length() > 0)
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
    this->_flags = flags;
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
    this->_atk = costume._atk;
    this->_def = costume._def;
    this->_spi = costume._spi;
    this->_wis = costume._wis;
    this->_agi = costume._agi;
    this->_res = costume._res;
    this->_a_skills = costume._a_skills;
    this->_m_actions = costume._m_actions;
    this->_st_res = costume._st_res;
}

Costume::~Costume()
{

}
