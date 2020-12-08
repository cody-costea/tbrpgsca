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
    return *(this->_costume_data->_a_skills->at(n));
}

bool Costume::hasAddedSkill(Ability& skill) const
{
    auto& aSkills = this->_costume_data->_a_skills;
    return aSkills && aSkills->contains(&skill);
}

int Costume::addedSkillsSize() const
{
    auto& aSkills = this->_costume_data->_a_skills;
    return aSkills == NIL ? 0 : aSkills->size();
}

Ability& Costume::counterSkill(int const n) const
{
    return *(this->_costume_data->_counters->at(n));
}

bool Costume::hasCounterSkill(Ability& skill) const
{
    auto& counters = this->_costume_data->_counters;
    return counters && counters->contains(&skill);
}

int Costume::counterSkillsSize() const
{
    auto& counters = this->_costume_data->_counters;
    return counters == NIL ? 0 : counters->size();
}

int Costume::elementResistance(const int element) const
{
    QMap<int, int>* res = this->_costume_data->_res;
    return res == NIL ? 0 : res->value(element, 0);
}

int Costume::stateResistance(const State* const state) const
{
    QMap<const State*, int>* stRes = this->_costume_data->_st_res;
    return stRes == NIL ? 0 : stRes->value(state, 0);
}

bool Costume::isCountering() const
{
    auto& counters = this->_costume_data->_counters;
    return counters && counters->size() > 0;
}

void Costume::adopt(QString& ret, Actor& actor) const
{
    this->adopt(&ret, actor, true, false);
}

void Costume::abandon(QString& ret, Actor& actor) const
{
    this->adopt(&ret, actor, true, true);
}

void Costume::adopt(QString* const ret, Actor& actor, bool const updStates, bool const remove) const
{
    const Costume& costume = *this;
    auto& actorData = actor._actor_data;
    auto& costumeData = costume._costume_data;
    actor.updateAttributes(remove, costume);
    actor.updateResistance(remove, costumeData->_res, costumeData->_st_res);
    if (remove)
    {
        int const roleFlags = costume.playFlags();
        int const actorFlags = actor.playFlags();
        if ((actorFlags & roleFlags) == roleFlags)
        {
            actor.setPlayFlags(actorFlags ^ roleFlags);
        }
        if (costume.isShapeShifted() && costume._role_data->_sprite)
        {
            (*actor._role_data->_sprite) = *(actor.job()._role_data->_sprite);
        }
        if (costume.currentHp() == 0 && costume.currentMp() == 0 && costume.currentRp() == 0)
        {
            int const roleElm = costume.damageType();
            int const actorElm = actor.damageType();
            if ((actorElm & roleElm) == roleElm)
            {
                actor.setDamageType(actorElm ^ roleElm);
            }
        }
        else
        {
            QVector<const Costume*>* const dmgRoles = actorData->_dmg_roles;
            if (dmgRoles)
            {
                dmgRoles->removeOne(&costume);
            }
        }
        costume.refresh(ret, actor, updStates, true);
        actor.refreshCostumes(ret);
    }
    else
    {
        if (updStates)
        {
            QMap<State*, int>* const cStates = costume._role_data->_state_dur;
            if (cStates)
            {
                actor.updateStates(false, ret, *cStates, true);
            }
        }
        if (costume.currentHp() != 0 || costume.currentMp() != 0 || costume.currentRp() != 0)
        {
            QVector<const Costume*>* dmgRoles = actorData->_dmg_roles;
            if (dmgRoles == NIL)
            {
                dmgRoles = new QVector<const Costume*>();
                actorData->_dmg_roles = dmgRoles;
            }
            dmgRoles->append(&costume);
        }
        costume.refresh(ret, actor, false, false);
    }
}

void Costume::apply(QString& ret, Actor& actor) const
{
    const Costume& role = *this;
    ret += QString(Costume::CausesTxt).arg(actor.name(), role.name());
    role.damage(ret, NIL, actor, 0, true);
}

void Costume::refresh(QString* const ret, Actor& actor, bool const updStates, bool const remove) const
{
    const Costume& costume = *this;
    {
        auto& costumeData = costume._costume_data;
        auto& skills = costumeData->_a_skills;
        if (skills)
        {
            actor.updateSkills(remove, false, *skills);
        }
        auto& counters = costumeData->_counters;
        if (counters)
        {
            actor.updateSkills(remove, true, *counters);
        }
    }
    if (updStates)
    {
        QMap<State*, int>* cStates = costume._role_data->_state_dur;
        if (cStates)
        {
            actor.updateStates(remove, ret, *cStates, false);
        }
    }
    if (!remove)
    {
        int const cFlags = costume.playFlags();
        if (costume.currentHp() == 0 && costume.currentMp() == 0 && costume.currentRp() == 0)
        {
            actor.setDamageType(actor.damageType() | costume.damageType());
        }
        actor.setPlayFlags(actor.playFlags() | cFlags);
        /*if (scene) //TODO: adapt without using "scene" pointer here
        {
#if ALLOW_COVERING
            if ((cFlags & Attribute::Covering) == Attribute::Covering)
            {
                scene->setHasCovers(true);
            }
#endif
            if (ret && (cFlags & Attribute::Ko) == Attribute::Ko)
            {
                scene->checkStatus(ret);
            }
        }*/
        if (costume.isShapeShifted())
        {
            QString* const spr = costume._role_data->_sprite;
            if (spr)
            {
                (*actor._role_data->_sprite) = *spr;
            }
        }
    }
}

Costume::CostumeData::~CostumeData() {}

Costume::Costume(int const id, QString& name, QString& sprite, bool const shapeShift, int const mActions, int const elm, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
                 int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun, bool const range, bool const automate, bool const confuse,
                 bool const reflect, bool const ko, bool const invincible, bool const revive, QList<Ability*>* const skills, QList<Ability*>* const counters, QMap<State*, int>* const states,
                 QMap<const State*, int>* const stRes, QMap<int, int>* const res, QObject* const parent)
    : Role(id, name, sprite, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, elm, range, revive, states, parent)
{
    QSharedDataPointer<CostumeData> costumeData(new CostumeData);
    costumeData->_m_actions = mActions;
    costumeData->_counters = counters;
    costumeData->_a_skills = skills;
    costumeData->_atk = atk;
    costumeData->_def = def;
    costumeData->_spi = spi;
    costumeData->_wis = wis;
    costumeData->_agi = agi;
    costumeData->_res = res;
    costumeData->_st_res = stRes;
    int flags = this->playFlags();
    if (stun)
    {
        flags |= Attribute::Stun;
    }
    if (shapeShift)
    {
        QString* spr = this->_role_data->_sprite;
        if (spr && spr->length() > 0)
        {
            flags |= Attribute::ShapeShift;
        }
    }
    if (automate)
    {
        flags |= Attribute::Enraged;
    }
    if (confuse)
    {
        flags |= Attribute::Confuse;
    }
    if (reflect)
    {
        flags |= Attribute::Reflect;
    }
    if (invincible)
    {
        flags |= Attribute::Invincible;
    }
    if (ko)
    {
        flags |= Attribute::Ko;
    }
    this->setPlayFlags(flags);
    this->_costume_data = costumeData;
}

Costume::Costume(int const id, QString& name, QString& sprite, bool const shapeShift, int const mActions, int const elm, int const hpDmg, int const mpDmg, int const spDmg,
                 int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const range, bool const automate,
                 bool const confuse, bool const reflect, bool const invincible, bool const revive, QList<Ability*>* const skills, QList<Ability*>* const counters,
                 QMap<State*, int>* const states, QMap<const State*, int>* const stRes, QMap<int, int>* const res, QObject* const parent)
    : Costume(id, name, sprite, shapeShift, mActions, elm, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis, agi, false, range, automate, confuse, reflect, false, invincible,
              revive, skills, counters, states, stRes, res, parent) {}

Costume::Costume(int const id, QString&& name, QString&& sprite, bool const shapeShift, int const mActions, int const elm, int const hpDmg, int const mpDmg, int const spDmg,
                 int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const range, bool const automate,
                 bool const confuse, bool const reflect, bool const invincible, bool const revive, QList<Ability*>* const skills, QList<Ability*>* const counters,
                 QMap<State*, int>* const states, QMap<const State*, int>* const stRes, QMap<int, int>* const res, QObject* const parent)
    : Costume(id, name, sprite, shapeShift, mActions, elm, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis, agi, range, automate, confuse, reflect, invincible,
              revive, skills, counters, states, stRes, res, parent) {}

Costume::Costume(QObject* const parent) : Costume(0, QString(), QString(), false, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, false, false, false, NIL, NIL, NIL, NIL, NIL, parent) {}

Costume::Costume(const Costume& costume) : Role(costume)
{
    this->_costume_data = costume._costume_data;
}

Costume::~Costume() {}
