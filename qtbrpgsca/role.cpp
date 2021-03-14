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
#include "ailment.h"
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
    QString* const spr = this->roleData()._sprite;
    return spr ? *spr : QString();
}

/*int Role::stateDuration(Ailment& state) const
{
    QList<Ailment>* const aStates = this->roleData()._a_states;
    return aStates == NIL ? 0 : aStates->value(&state, 0);
}

QList<Ailment*> Role::statesList() const
{
    QList<Ailment>* const aStates = this->roleData()._a_states;
    return aStates == NIL ? QList<Ailment*>() : aStates->keys();
}

bool Role::hasState(Ailment& state) const
{
    QList<Ailment>* const aStates = this->roleData()._a_states;
    return aStates && aStates->contains(&state);
}

int Role::statesSize() const
{
    QList<Ailment>* const aStates = this->roleData()._a_states;
    return aStates == NIL ? 0 : aStates->size();
}*/

void Role::damage(QString& ret, Actor* const absorber, Actor& actor, int const dmg, bool const percent) const
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
            QMap<int, int>* trgResMap = actor.costumeData()._res;
            if (trgResMap)
            {
                int res = Ability::DefaultRes;
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
                dmgHp /= Ability::DefaultRes;
                dmgMp /= Ability::DefaultRes;
                dmgSp /= Ability::DefaultRes;
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
            const int cHp = actor.currentHp();
            if (percent && dmgHp >= cHp)
            {
                dmgHp = cHp - 1;
            }
            ret += (QString("%1 %2").arg(QString::number(-dmgHp), Role::HpTxt));
            actor.setCurrentHp(cHp - dmgHp);
            /*if (actor.hp < 1)
            {
                ret += Actor::KoTxt.arg(actor.name);
            }*/
        }
        if (c && absorber)
        {
            absorber->setCurrentRp(absorber->currentRp() + dmgSp / 2);
            absorber->setCurrentMp(absorber->currentMp() + dmgMp / 2);
            absorber->setCurrentHp(absorber->currentHp() + dmgHp / 2);
        }
    }
}


Role::RoleSheet::RoleSheet(int const id, QString& name, QString& sprite, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
                           int const mMp, int const mSp, int const element, bool const range, bool const revive, QList<Ailment>* const aStates)
    : PlaySheet(((revive ? Attribute::REVIVE : 0) | (range ? Attribute::RANGE : 0)))
{
    this->_sprite = sprite.isNull() ? NIL : new QString(sprite);
    this->_a_states = aStates;
    this->_dmg_type = element;
    this->_name = name;
    this->_m_hp = mHp;
    this->_m_mp = mMp;
    this->_m_sp = mSp;
    this->_hp = hpDmg;
    this->_mp = mpDmg;
    this->_sp = spDmg;
    this->_id = id;
}

Role::RoleSheet::~RoleSheet()
{
    QString* sprite = this->_sprite;
    if (sprite)
    {
        this->_sprite = NIL;
        delete sprite;
    }
}

Role::Role(QObject* const parent, RoleSheet* const roleDataPtr) : Play(parent, roleDataPtr) {}

Role::Role(int const id, QString& name, QString& sprite, int const hpDmg, int const mpDmg, int const spDmg, int const mHp, int const mMp,
           int const mSp, int const element, bool const range, bool const revive, QList<Ailment>* aStates, QObject* const parent)
    : Role(parent, new RoleSheet(id, name, sprite, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, element, range, revive, aStates)) {}

Role::Role(int const id, QString&& name, QString&& sprite, int const hpDmg, int const mpDmg, int const spDmg, int const mHp, int const mMp,
           int const mSp, int const element, bool const range, bool const revive, QList<Ailment>* aStates, QObject* const parent)
    : Role(id, name, sprite, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, element, range, revive, aStates, parent) {}

Role::Role(QObject* const parent) : Role(0, QString(), QString(), 0, 0, 0, 0, 0, 0, 0, false, false, NIL, parent) {}

Role::Role(const Role&& role) : Role(static_cast<const Role&>(role)) {}

Role::Role(const Role& role) : Play(role) {}

Role::~Role() {}
