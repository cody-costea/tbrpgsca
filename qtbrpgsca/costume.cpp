/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "costume.h"
#include "ability.h"
#include "actor.h"
#include "ailment.h"
#include "scene.h"
#include "role.h"

#include <QStringBuilder>

using namespace tbrpgsca;

QString Costume::CausesTxt = ", %1 is affected by %2";

Ability Costume::addedSkill(int const n) const
{
    return (this->costumeData()._a_skills->at(n));
}

bool Costume::hasAddedSkill(const Ability& skill) const
{
    auto aSkills = this->costumeData()._a_skills;
    return aSkills && aSkills->contains(skill);
}

int Costume::addedSkillsSize() const
{
    auto aSkills = this->costumeData()._a_skills;
    return aSkills == NIL ? 0 : aSkills->size();
}

Ability Costume::counterSkill(int const n) const
{
    return (this->costumeData()._counters->at(n));
}

bool Costume::hasCounterSkill(const Ability& skill) const
{
    auto counters = this->costumeData()._counters;
    return counters && counters->contains(skill);
}

int Costume::counterSkillsSize() const
{
    auto counters = this->costumeData()._counters;
    return counters == NIL ? 0 : counters->size();
}

int Costume::elementResistance(const int element) const
{
    QMap<int, int>* res = this->costumeData()._res;
    return res == NIL ? 0 : res->value(element, 0);
}

int Costume::stateResistance(const int state) const
{
    auto stRes = this->costumeData()._st_res;
    return stRes == NIL ? 0 : stRes->value(state, 0);
}

bool Costume::isCountering() const
{
    auto& counters = this->costumeData()._counters;
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
    auto& actorData = actor.actorMutData();
    auto& costumeData = costume.costumeData();
    actor.updateAttributes(remove, costume);
    actor.updateResistance(remove, costumeData._res, costumeData._st_res);
    if (remove)
    {
        int const roleFlags = costume.playFlags();
        int const actorFlags = actor.playFlags();
        if ((actorFlags & roleFlags) == roleFlags)
        {
            actor.setPlayFlags(actorFlags ^ roleFlags);
        }
        if (costume.isShapeShifted() && costume.roleData()._sprite)
        {
            (*actor.roleMutData()._sprite) = *(actor.job().roleMutData()._sprite);
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
            QVector<const Costume*>* const dmgRoles = actorData._dmg_roles;
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
            QList<Ailment>* const cStates = costume.roleData()._a_states;
            if (cStates)
            {
                actor.updateStates(false, ret, *cStates, true);
            }
        }
        if (costume.currentHp() != 0 || costume.currentMp() != 0 || costume.currentRp() != 0)
        {
            QVector<const Costume*>* dmgRoles = actorData._dmg_roles;
            if (dmgRoles == NIL)
            {
                dmgRoles = new QVector<const Costume*>();
                actorData._dmg_roles = dmgRoles;
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
        auto costumeData = costume.costumeData();
        auto skills = costumeData._a_skills;
        if (skills)
        {
            actor.updateSkills(remove, false, *skills);
        }
        auto counters = costumeData._counters;
        if (counters)
        {
            actor.updateSkills(remove, true, *counters);
        }
    }
    if (updStates)
    {
        QList<Ailment>* cStates = costume.roleData()._a_states;
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
            QString* const spr = costume.roleData()._sprite;
            if (spr)
            {
                (*actor.roleData()._sprite) = *spr;
            }
        }
    }
}

Costume::CostumeSheet::~CostumeSheet() {}

Costume::CostumeSheet::CostumeSheet(int const id, QString& name, QString& sprite, bool const shapeShift, int const mActions, int const elm, int const hpDmg,
                                    int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi,
                                    int const wis, int const agi, bool const stun, bool const range, bool const automate, bool const confuse, bool const reflect,
                                    bool const ko, bool const invincible, bool const revive, QList<Ability>* const skills, QList<Ability>* const counters,
                                    QList<Ailment>* const states, QMap<int, int>* const stRes, QMap<int, int>* const res)
    : RoleSheet(id, name, sprite, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, elm, range, revive, states)
{
    this->_m_actions = mActions;
    this->_counters = counters;
    this->_a_skills = skills;
    this->_atk = atk;
    this->_def = def;
    this->_spi = spi;
    this->_wis = wis;
    this->_agi = agi;
    this->_res = res;
    this->_st_res = stRes;
    int flags = this->_play_flags;
    if (stun)
    {
        flags |= Attribute::STUN;
    }
    if (shapeShift)
    {
        QString* spr = this->_sprite;
        if (spr && spr->length() > 0)
        {
            flags |= Attribute::SHAPE_SHIFT;
        }
    }
    if (automate)
    {
        flags |= Attribute::ENRAGED;
    }
    if (confuse)
    {
        flags |= Attribute::CONFUSE;
    }
    if (reflect)
    {
        flags |= Attribute::REFLECT;
    }
    if (invincible)
    {
        flags |= Attribute::INVINCIBLE;
    }
    if (ko)
    {
        flags |= Attribute::KO;
    }
    this->_play_flags = flags;
}

Costume::Costume(int const id, QString& name, QString& sprite, bool const shapeShift, int const mActions, int const elm, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
                 int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun, bool const range, bool const automate, bool const confuse,
                 bool const reflect, bool const ko, bool const invincible, bool const revive, QList<Ability>* const skills, QList<Ability>* const counters, QList<Ailment>* const states,
                 QMap<int, int>* const stRes, QMap<int, int>* const res, QObject* const parent)
    : Costume(parent, new CostumeSheet(id, name, sprite, shapeShift, mActions, elm, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis, agi, stun, range,
                                       automate, confuse, reflect, ko, invincible, revive, skills, counters, states, stRes, res)) {}

Costume::Costume(int const id, QString& name, QString& sprite, bool const shapeShift, int const mActions, int const elm, int const hpDmg, int const mpDmg, int const spDmg,
                 int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const range, bool const automate,
                 bool const confuse, bool const reflect, bool const invincible, bool const revive, QList<Ability>* const skills, QList<Ability>* const counters,
                 QList<Ailment>* const states, QMap<int, int>* const stRes, QMap<int, int>* const res, QObject* const parent)
    : Costume(id, name, sprite, shapeShift, mActions, elm, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis, agi, false, range, automate, confuse, reflect, false, invincible,
              revive, skills, counters, states, stRes, res, parent) {}

Costume::Costume(int const id, QString&& name, QString&& sprite, bool const shapeShift, int const mActions, int const elm, int const hpDmg, int const mpDmg, int const spDmg,
                 int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const range, bool const automate,
                 bool const confuse, bool const reflect, bool const invincible, bool const revive, QList<Ability>* const skills, QList<Ability>* const counters,
                 QList<Ailment>* const states, QMap<int, int>* const stRes, QMap<int, int>* const res, QObject* const parent)
    : Costume(id, name, sprite, shapeShift, mActions, elm, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis, agi, range, automate, confuse, reflect, invincible,
              revive, skills, counters, states, stRes, res, parent) {}

Costume::Costume(QObject* const parent) : Costume(0, QString(), QString(), false, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, false, false, false, NIL, NIL, NIL, NIL, NIL, parent) {}

Costume::Costume(QObject* const parent, CostumeSheet* const costumeDataPtr) : Role(parent, costumeDataPtr) {}

Costume::Costume(const Costume&& costume) : Costume(static_cast<const Costume&>(costume)) {}

Costume::Costume(const Costume& costume) : Role(costume) {}

Costume::~Costume() {}
