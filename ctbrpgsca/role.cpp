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
#include "arenawidget.h"

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
    return spr ? *spr : QString();
}

int Role::stateDuration(const State& state) const
{
    QMap<const State*, int>* const aStates = this->_state_dur;
    return aStates ? aStates->value(&state, 0) : 0;
}

QList<const State*> Role::statesList() const
{
    QMap<const State*, int>* const aStates = this->_state_dur;
    return aStates ? aStates->keys() : QList<const State*>();
}

bool Role::hasState(const State& state) const
{
    QMap<const State*, int>* const aStates = this->_state_dur;
    return aStates && aStates->contains(&state);
}

int Role::statesSize() const
{
    QMap<const State*, int>* const aStates = this->_state_dur;
    return aStates ? aStates->size() : 0;
}

void Role::damage(QString& ret, Actor* const absorber, Actor& target, int const dmg, bool const percent) const
{
    return this->damage(ret, nullptr, static_cast<Scene::SpriteAct*>(nullptr), absorber, target, dmg, percent);
}

template <typename SpriteRun>
void Role::damage(QString& ret, Scene* const scene, SpriteRun* const spriteRun, Actor* const absorber, Actor& actor, int const dmg, bool const percent) const
{
    const Role& role = *this;
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
            if (trgResMap)
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
                        ret += Ability::ResistTxt.arg(actor._name);
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
            ret += Ability::SuffersTxt.arg(actor._name);
            if (dmgSp < 0)
            {
                ret += "+";
            }
            ret += (QString("%1 %2").arg(QString::number(-dmgSp), Role::RpTxt));
            actor.setCurrentRp(actor._sp - dmgSp);
        }
        if (dmgMp != 0)
        {
            if (c)
            {
                ret += ", ";
            }
            else
            {
                ret += Ability::SuffersTxt.arg(actor._name);
                c = true;
            }
            if (dmgMp < 0)
            {
                ret += "+";
            }
            ret += (QString("%1 %2").arg(QString::number(-dmgMp), Role::MpTxt));
            actor.setCurrentMp(actor._mp - dmgMp);
        }
        if (dmgHp != 0)
        {
            if (c)
            {
                ret += ", ";
            }
            else
            {
                ret += Ability::SuffersTxt.arg(actor._name);
                c = true;
            }
            if (dmgHp < 0)
            {
                ret += "+";
            }
            ret += (QString("%1 %2").arg(QString::number(-dmgHp), Role::HpTxt));
            actor.setCurrentHp(&ret, scene, spriteRun, actor._hp - dmgHp, percent);
            /*if (actor.hp < 1)
            {
                ret += Actor::KoTxt.arg(actor.name);
            }*/
        }
        if (c && absorber)
        {
            absorber->setCurrentRp(absorber->_sp + dmgSp / 2);
            absorber->setCurrentMp(absorber->_mp + dmgMp / 2);
            absorber->setCurrentHp(&ret, scene, spriteRun, absorber->_hp + dmgHp / 2, true);
        }
    }
}

Role::Role(int const id, QString &name, QString& sprite, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
           int const mMp, int const mSp, int const element, bool const range, bool const revive, QMap<const State*, int>* aStates)
    : Play(((revive ? FLAG_REVIVE : 0) | (range ? FLAG_RANGE : 0)))
{
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

Role::Role(const Role& role) : Play(role._play_flags)
{
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
    if (sprite)
    {
        this->_sprite = nullptr;
        delete sprite;
    }
}

#if USE_TEMPLATE
template void Role::damage(QString& ret, Scene* const scene, ArenaWidget* const spriteRun, Actor* const absorber, Actor& target, int dmg, bool const percent) const;
#endif

template void Role::damage(QString& ret, Scene* const scene, Scene::SpriteAct* const spriteRun, Actor* const absorber, Actor& target, int dmg, bool const percent) const;
