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
    QString* const spr = this->_role_data->_sprite;
    return spr ? *spr : QString();
}

int Role::stateDuration(State& state) const
{
    QMap<State*, int>* const aStates = this->_role_data->_state_dur;
    return aStates == NIL ? 0 : aStates->value(&state, 0);
}

QList<State*> Role::statesList() const
{
    QMap<State*, int>* const aStates = this->_role_data->_state_dur;
    return aStates == NIL ? QList<State*>() : aStates->keys();
}

bool Role::hasState(State& state) const
{
    QMap<State*, int>* const aStates = this->_role_data->_state_dur;
    return aStates && aStates->contains(&state);
}

int Role::statesSize() const
{
    QMap<State*, int>* const aStates = this->_role_data->_state_dur;
    return aStates == NIL ? 0 : aStates->size();
}

bool Role::operator==(Role& role) const
{
    return this->databaseId() == role.databaseId();
}

void Role::damage(QString& ret, Actor* const absorber, Actor& target, int const dmg, bool const percent) const
{
    return this->damage(ret, NIL, absorber, target, dmg, percent);
}

void Role::damage(QString& ret, Scene* const scene, Actor* const absorber, Actor& actor, int const dmg, bool const percent) const
{
    const Role& role = *this;
    if (!actor.Costume::isInvincible())
    {
        int dmgHp, dmgMp, dmgSp;
        if (percent)
        {
            //dmgHp = (actor._m_hp + (dmg + (std::rand() % (dmg / 3)))) * role._hp / 100;
            dmgHp = (actor.maximumHp() + (dmg + (std::rand() % DMG_RND))) * role.currentHp() / 100;
            dmgMp = (actor.maximumMp() + (dmg + (std::rand() % DMG_RND))) * role.currentMp() / 100;
            dmgSp = (actor.maximumRp() + (dmg + (std::rand() % DMG_RND))) * role.currentRp() / 100;
        }
        else
        {
            dmgHp = role.currentHp();
            dmgMp = role.currentMp();
            dmgSp = role.currentRp();
            if (dmgHp != 0)
            {
                dmgHp += dmgHp < 0 ? (-1 * (dmg + (std::rand() % DMG_RND))) : (dmg + (std::rand() % DMG_RND));
            }
            if (dmgMp != 0)
            {
                dmgMp += dmgMp < 0 ? (-1 * (dmg + (std::rand() % DMG_RND))) : (dmg + (std::rand() % DMG_RND));
            }
            if (dmgSp != 0)
            {
                dmgSp += dmgSp < 0 ? (-1 * (dmg + (std::rand() % DMG_RND))) : (dmg + (std::rand() % DMG_RND));
            }
        }
        {
            QMap<int, int>* trgResMap = actor._costume_data->_res;
            if (trgResMap)
            {
                int res = DEFAULT_RES;
                {
                    int const dmgType = role.damageType();
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
                        ret += Ability::ResistTxt.arg(actor.name());
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
            ret += Ability::SuffersTxt.arg(actor.name());
            if (dmgSp < 0)
            {
                ret += "+";
            }
            ret += (QString("%1 %2").arg(QString::number(-dmgSp), Role::RpTxt));
            actor.setCurrentRp(actor.currentRp() - dmgSp);
        }
        if (dmgMp != 0)
        {
            if (c)
            {
                ret += ", ";
            }
            else
            {
                ret += Ability::SuffersTxt.arg(actor.name());
                c = true;
            }
            if (dmgMp < 0)
            {
                ret += "+";
            }
            ret += (QString("%1 %2").arg(QString::number(-dmgMp), Role::MpTxt));
            actor.setCurrentMp(actor.currentMp() - dmgMp);
        }
        if (dmgHp != 0)
        {
            if (c)
            {
                ret += ", ";
            }
            else
            {
                ret += Ability::SuffersTxt.arg(actor.name());
                c = true;
            }
            if (dmgHp < 0)
            {
                ret += "+";
            }
            ret += (QString("%1 %2").arg(QString::number(-dmgHp), Role::HpTxt));
            actor.setCurrentHp(actor.currentHp() - dmgHp, &ret, scene, percent);
            /*if (actor.hp < 1)
            {
                ret += Actor::KoTxt.arg(actor.name);
            }*/
        }
        if (c && absorber)
        {
            absorber->setCurrentRp(absorber->currentRp() + dmgSp / 2);
            absorber->setCurrentMp(absorber->currentMp() + dmgMp / 2);
            absorber->setCurrentHp(absorber->currentHp() + dmgHp / 2, &ret, scene, true);
        }
    }
}

Role::RoleData::~RoleData()
{
    QString* sprite = this->_sprite;
    if (sprite)
    {
        this->_sprite = NIL;
        delete sprite;
    }
}

Role::Role(int const id, QString &name, QString& sprite, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
           int const mMp, int const mSp, int const element, bool const range, bool const revive, QMap<State*, int>* aStates)
    : Play(NIL, ((revive ? Attribute::Revive : 0) | (range ? Attribute::Range : 0)))
{
    QSharedDataPointer<RoleData> roleData(new RoleData);
    roleData->_sprite = sprite.length() == 0 ? NIL : new QString(sprite);
    roleData->_state_dur = aStates;
    roleData->_dmg_type = element;
    roleData->_name = name;
    roleData->_m_hp = mHp;
    roleData->_m_mp = mMp;
    roleData->_m_sp = mSp;
    roleData->_hp = hpDmg;
    roleData->_mp = mpDmg;
    roleData->_sp = spDmg;
    roleData->_id = id;
    this->_role_data = roleData;
}

Role::Role(const Role& role) : Play(NIL, role.playFlags())
{
    this->_role_data = role._role_data;
}

Role::~Role()
{

}
