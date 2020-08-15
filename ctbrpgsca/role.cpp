/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "role.h"
#include "ability.h"
#include "costume.h"
#include "actor.h"
#include "state.h"
#include "scene.h"

#include <QStringBuilder>

using namespace tbrpgsca;

QString Role::HpTxt = "HP";
QString Role::MpTxt = "MP";
QString Role::RpTxt = "RP";
QString Role::ResistTxt = ", resisted by %1";
QString Role::SuffersTxt = ", %1 suffers ";

QString Role::sprite() const
{
    QString* const spr = this->_sprite;
    return spr == nullptr ? QString() : *(this->_sprite);
}

int Role::stateDuration(State& state) const
{
    QMap<State*, int>* const aStates = this->_state_dur;
    return aStates == nullptr ? 0 : aStates->value(&state, 0);
}

QList<State*> Role::statesList() const
{
    QMap<State*, int>* const aStates = this->_state_dur;
    return aStates == nullptr ? QList<State*>() : aStates->keys();
}

bool Role::hasState(State& state) const
{
    QMap<State*, int>* const aStates = this->_state_dur;
    return aStates != nullptr && aStates->contains(&state);
}

int Role::statesSize() const
{
    QMap<State*, int>* const aStates = this->_state_dur;
    return aStates == nullptr ? 0 : aStates->size();
}

bool Role::operator==(Role& role) const
{
    return this->_id == role._id;
}

void Role::damage(QString& ret, Actor* const absorber, Actor& target, int const dmg, bool const percent)
{
    return this->damage(ret, nullptr, absorber, target, dmg, percent);
}

void Role::damage(QString& ret, Scene* const scene, Actor* const absorber, Actor& actor, int const dmg, bool const percent)
{
    Role& role = *this;
    if (!actor.Costume::isInvincible())
    {
        int dmgHp, dmgMp, dmgSp;
        if (percent)
        {
            dmgHp = (actor._m_hp + dmg) * role._hp / 100;
            dmgMp = (actor._m_mp + dmg) * role._mp / 100;
            dmgSp = (actor._m_sp + dmg) * role._sp / 100;
        }
        else
        {
            dmgHp = role._hp;
            dmgMp = role._mp;
            dmgSp = role._sp;
            if (dmgHp != 0)
            {
                dmgHp += dmgHp < 0 ? (-1 * dmg) : dmg;
            }
            if (dmgMp != 0)
            {
                dmgMp += dmgMp < 0 ? (-1 * dmg) : dmg;
            }
            if (dmgSp != 0)
            {
                dmgSp += dmgSp < 0 ? (-1 * dmg) : dmg;
            }
        }
        {
            QMap<int, int>* trgResMap = actor._res;
            if (trgResMap != nullptr)
            {
                int res = DEFAULT_RES;
                {
                    int const dmgType = role._dmg_type;
                    auto const last = trgResMap->cend();
                    for (auto it = trgResMap->cbegin(); it != last; ++it)
                    {
                        int const elm = it.key();
                        if ((dmgType & elm) == elm)
                        {
                            res += it.value();
                        }
                    }
                }
                if (res > 0)
                {
                    if (res > 7)
                    {
                        res = -7 + (res - 7);
                        if (res > -1)
                        {
                            res = -1 * (res + 2);
                        }
                    }
                    else if (res == 7)
                    {
                        ret = ret % Ability::ResistTxt.arg(actor._name);
                        return;
                    }
                    else
                    {
                        dmgHp /= res;
                        dmgMp /= res;
                        dmgSp /= res;
                        goto applyChanges;
                    }
                }
                else
                {
                    res = -1 * (res - 2);
                }                
                dmgHp *= res;
                dmgMp *= res;
                dmgSp *= res;
            }
            else
            {
                dmgHp /= DEFAULT_RES;
                dmgMp /= DEFAULT_RES;
                dmgSp /= DEFAULT_RES;
            }
        }
        applyChanges:
        bool c = false;
        if (dmgSp != 0)
        {
            c = true;
            ret = ret % Ability::SuffersTxt.arg(actor._name);
            if (dmgSp < 0)
            {
                ret = ret % "+";
            }
            ret = ret % (QString("%1 %2").arg(QString::number(-dmgSp), Role::RpTxt));
            actor.setCurrentRp(actor._sp - dmgSp);
        }
        if (dmgMp != 0)
        {
            if (c)
            {
                ret = ret % ", ";
            }
            else
            {
                ret = ret % Ability::SuffersTxt.arg(actor._name);
                c = true;
            }
            if (dmgMp < 0)
            {
                ret = ret % "+";
            }
            ret = ret % (QString("%1 %2").arg(QString::number(-dmgMp), Role::MpTxt));
            actor.setCurrentMp(actor._mp - dmgMp);
        }
        if (dmgHp != 0)
        {
            if (c)
            {
                ret = ret % ", ";
            }
            else
            {
                ret = ret % Ability::SuffersTxt.arg(actor._name);
                c = true;
            }
            if (dmgHp < 0)
            {
                ret = ret % "+";
            }
            ret = ret % (QString("%1 %2").arg(QString::number(-dmgHp), Role::HpTxt));
            actor.setCurrentHp(actor._hp - dmgHp, &ret, scene, percent);
            /*if (actor.hp < 1)
            {
                ret = ret % Actor::KoTxt.arg(actor.name);
            }*/
        }
        if (c && absorber != nullptr)
        {
            absorber->setCurrentRp(absorber->_sp + dmgSp / 2);
            absorber->setCurrentMp(absorber->_mp + dmgMp / 2);
            absorber->setCurrentHp(absorber->_hp + dmgHp / 2, &ret, scene, true);
        }
    }
}

Role::Role(int const id, QString &name, QString& sprite, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
           int const mMp, int const mSp, int const element, bool const range, bool const revive, QMap<State*, int>* aStates)
    : Play(((revive ? FLAG_REVIVE : 0) | (range ? FLAG_RANGE : 0)))
{
    this->_id = id;
    this->_name = name;
    this->_sprite = sprite.length() == 0 ? nullptr : new QString(sprite);
    this->_m_hp = mHp;
    this->_m_mp = mMp;
    this->_m_sp = mSp;
    this->_hp = hpDmg;
    this->_mp = mpDmg;
    this->_sp = spDmg;
    this->_dmg_type = element;
    this->_state_dur = aStates;
}

Role::Role(Role& role) : Play(role._flags)
{
    this->_id = role._id;
    this->_name = role._name;
    this->_sprite = role._sprite;
    this->_state_dur = role._state_dur;
    this->_dmg_type = role._dmg_type;
    this->_m_hp = role._m_hp;
    this->_m_mp = role._m_mp;
    this->_m_sp = role._m_sp;
    this->_hp = role._hp;
    this->_mp = role._mp;
    this->_sp = role._sp;
}

Role::~Role()
{
    QString* sprite = this->_sprite;
    if (sprite != nullptr)
    {
        this->_sprite = nullptr;
        delete sprite;
    }
}
