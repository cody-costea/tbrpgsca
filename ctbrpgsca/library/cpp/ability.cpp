/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "ability.hpp"
#include "costume.hpp"
#include "actor.hpp"
#include "state.hpp"
#include "scene.hpp"
#include "role.hpp"
#include "arenawidget.hpp"

#include <QStringBuilder>

using namespace tbrpgsca;

QString Ability::MissesTxt = ", but misses %1";
QString Ability::ReflectTxt = ", reflected by %1";
QString Ability::StolenTxt = ", obtaining %1 from %2";

int Ability::removedStateDuration(const State& state) const
{
    QMap<const State*, int>* aStates = this->_r_states;
    return aStates == nullptr ? 0 : aStates->value(&state, 0);
}

QList<const State*> Ability::removedStatesList() const
{
    QMap<const State*, int>* aStates = this->_r_states;
    return aStates == nullptr ? QList<const State*>() : aStates->keys();
}

bool Ability::removedState(const State& state) const
{
    QMap<const State*, int>* aStates = this->_r_states;
    return aStates && aStates->contains(&state);
}

int Ability::removedStatesSize() const
{
    QMap<const State*, int>* aStates = this->_r_states;
    return aStates == nullptr ? 0 : aStates->size();
}

void Ability::replenish(Actor& user) const
{
    const Ability& ability = *this;
    int const mQty = ability._m_qty;
    if (mQty > 0)
    {
        QMap<const Ability*, int>* usrSkills = user._skills_cr_qty;
        if (usrSkills == nullptr)
        {
            usrSkills = new QMap<const Ability*, int>();
            user._skills_cr_qty = usrSkills;
        }
        usrSkills->operator[](this) = mQty;
    }

}

bool Ability::canPerform(const Actor& actor) const
{
    int const blockedSkills = actor._b_skill_types;
    QMap<const Ability*, int>* skillsQty = actor._skills_cr_qty;
    return (this->_dmg_type & blockedSkills) == 0 && this->_m_mp <= actor._mp && this->_m_hp < actor._hp
            && this->_m_sp <= actor._sp && actor._lv >= this->_lv_rq && (skillsQty == nullptr || skillsQty->value(this, 1) > 0);
}

void Ability::execute(QString& ret, Actor& user, Actor& target, bool applyCosts) const
{
    return this->execute(ret, nullptr, static_cast<Scene::SpriteAct*>(nullptr), user, &target, applyCosts);
}

template <typename SpriteRun>
void Ability::execute(QString& ret, Scene* const scene, const SpriteRun* const spriteRun, Actor& user, Actor* target, bool const applyCosts) const
{
    //assert(target);
    const Ability& ability = *this;
    int const dmgType = ability._dmg_type | user._dmg_type;
    if (dmgType == DMG_TYPE_WIS && target != &user && target->Costume::isReflecting())
    {
        ret += Ability::ReflectTxt.arg(target->_name);
        target = &user;
    }
    {
        int canMiss = ability.canMiss() ? 4 : 0, def = 0, i = 0, dmg = 0, usrAgi = user._agi,
                trgAgi = target->_agi, trgSpi = target->_spi, usrWis = user._wis;
        if ((dmgType & DMG_TYPE_ATK) == DMG_TYPE_ATK)
        {
            dmg += user._atk;
            def += target->_def;
            ++i;
        }
        if ((dmgType & DMG_TYPE_DEF) == DMG_TYPE_DEF)
        {
            dmg += user._def;
            def += target->_def;
            ++i;
        }
        if ((dmgType & DMG_TYPE_SPI) == DMG_TYPE_SPI)
        {
            dmg += user._spi;
            def += target->_wis;
            ++i;
        }
        if ((dmgType & DMG_TYPE_WIS) == DMG_TYPE_WIS)
        {
            dmg += usrWis;
            def += trgSpi;
            ++i;
        }
        if ((dmgType & DMG_TYPE_AGI) == DMG_TYPE_AGI)
        {
            dmg += usrAgi;
            def += trgAgi;
            if (canMiss > 0)
            {
                canMiss = 3;
            }
            ++i;
        }
        trgAgi = ((trgAgi + trgSpi) / 2) / 3, usrAgi = (usrAgi + usrWis) / 2;
        if (canMiss == 0 || target == &user || ((canMiss = (std::rand() % usrAgi / 2)
                + (usrAgi / canMiss)) > trgAgi - (std::rand() % trgAgi)))
        {
            if (canMiss > (trgAgi * 2) + (std::rand() % trgAgi))
            {
                dmg = (dmg * 2) + (dmg / 2); //TODO: add text for critical
            }
            if (i != 0)
            {
                def += std::rand() % (def / 2);
                dmg += std::rand() % (dmg / 2);
                dmg = (/*ability._attr_inc +*/ (dmg / i)) - ((def / i) / 2);
                if (dmg < 0)
                {
                    dmg = 0;
                }
            }
            ability.damage(ret, scene, spriteRun, (ability.isAbsorbing() ? &user : nullptr), *target, dmg, false);
            {
                QMap<const State*, int>* aStates = ability._state_dur;
                if (aStates)
                {
                    auto const last = aStates->cend();
                    for (auto it = aStates->cbegin(); it != last; ++it)
                    {
                        it.key()->inflict(&ret, scene, &user, *target, it.value(), user._side == target->_side);
                    }
                }
            }
            {
                QMap<const State*, int>* stateDur = target->_state_dur;
                if (stateDur)
                {
                    QMap<const State*, int>* rStates = ability._r_states;
                    if (rStates)
                    {
                        auto const rLast = rStates->cend();
                        for (auto rIt = rStates->cbegin(); rIt != rLast; ++rIt)
                        {
                            int const rDur = rIt.value();
                            if (rDur > STATE_END_DUR)
                            {
                                const State* const rState = rIt.key();
                                auto const last = stateDur->cend();
                                for (auto it = stateDur->cbegin(); it != last; ++it)
                                {
                                    const State* const aState = it.key();
                                    if (aState == rState)
                                    {
                                        if (it.value() > STATE_END_DUR)
                                        {
                                            rState->disable(&ret, scene, *target, rDur, false);
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (ability.isStealing())
            {
                QMap<const Ability*, int>* usrItems = user._items;
                //if (usrItems)
                {
                    int trgItemsSize;
                    QMap<const Ability*, int>* trgItems = target->_items;
                    if (trgItems && trgItems != usrItems && (trgItemsSize = trgItems->size()) > 0
                            && (((std::rand() % 12) + user._agi / 4) > 4 + target->_agi / 3))
                    {
                        int const itemId = std::rand() % trgItemsSize;
                        //if (itemId < trgItemsSize)
                        //{
                            int trgItemQty = 0;
                            const Ability* stolen = nullptr;
                            //Ability* stolen = trgItems->keys().at(itemId);
                            {
                                i = 0;
                                auto const last = trgItems->cend();
                                for (auto it = trgItems->cbegin(); it != last; ++it, ++i)
                                {
                                    if (i == itemId)
                                    {
                                        stolen = it.key();
                                        trgItemQty = it.value();
                                        break;
                                    }
                                }
                            }
                            if (trgItemQty > 0)
                            {
                                if (usrItems == nullptr)
                                {
                                    usrItems = new QMap<const Ability*, int>();
                                    user.setNewItems(true);
                                    user._items = usrItems;
                                }
                                usrItems->operator[](stolen) = usrItems->value(stolen, 0) + 1;
                                if (trgItemQty < 2)
                                {
                                    trgItems->remove(stolen);
                                }
                                else
                                {
                                    trgItems->operator[](stolen) = trgItemQty - 1;
                                }
                                ret += Ability::StolenTxt.arg(stolen->_name, target->_name);
                            }
                        //}
                    }
                }
            }
        }
        else
        {
            ret += Ability::MissesTxt.arg(target->_name);
        }
    }
    if (applyCosts)
    {
        user.setCurrentRp(user._sp - ability._m_sp);
        user.setCurrentMp(user._mp - ability._m_mp);
        user.setCurrentHp(&ret, scene, spriteRun, user._hp - ability._m_hp, false);
        int mQty = ability._m_qty;
        if (mQty > 0)
        {
            QMap<const Ability*, int>* usrSkillsQty = user._skills_cr_qty;
            if (usrSkillsQty == nullptr)
            {
                usrSkillsQty = new QMap<const Ability*, int>();
                user._skills_cr_qty = usrSkillsQty;
            }
            usrSkillsQty->operator[](this) = (usrSkillsQty->value(this, mQty) - 1);
        }
    }

}

Ability::Ability(int const id, QString name, QString sprite, QString sound, bool const steal, bool const range, bool const melee, bool const canMiss, int const lvRq,
                 int const hpC, int const mpC, int const spC, int const dmgType, int const hpDmg, int const mpDmg, int const spDmg, int const trg, int const elm,
                 int const mQty, int const rQty, bool const absorb, bool const revive, QMap<const State*, int>* const aStates, QMap<const State*, int>* const rStates)
    : Role(id, name, sprite, hpDmg, mpDmg, spDmg, hpC, mpC, spC, (elm | dmgType), range, hpDmg < 0 && revive, aStates)
{
    this->_lv_rq = lvRq;
    this->_m_qty = mQty;
    this->_r_qty = rQty;
    //this->_attr_inc = attrInc;
    this->_sound = sound.length() > 0 ? new QString(sound) : nullptr;
    this->_r_states = rStates;
    int flags = this->_play_flags;
    if (canMiss)
    {
        flags |= FLAG_MISSABLE;
    }
    /*if (revive)
    {
        flags |= FLAG_FOR_KO;
    }*/
    if (melee)
    {
        flags |= FLAG_MELEE;
    }
    if (steal)
    {
        flags |= FLAG_STEAL;
    }
    if (absorb)
    {
        flags |= FLAG_ABSORB;
    }
    flags |= trg;
    this->_play_flags = flags;
}

Ability::Ability(const Ability& ability) : Role(ability)
{
    this->_lv_rq = ability._lv_rq;
    this->_m_qty = ability._m_qty;
    this->_r_qty = ability._r_qty;
    //this->_attr_inc = ability._attr_inc;
    this->_state_dur = ability._state_dur;
    this->_r_states = ability._r_states;
}

Ability::~Ability()
{
    QString* const sound = this->_sound;
    if (sound)
    {
        delete sound;
    }
}

#if USE_TEMPLATE
template void Ability::execute(QString& ret, Scene* const scene, const ArenaWidget* const spriteRun, Actor& user, Actor* target, bool const applyCosts) const;
#endif
template void Ability::execute(QString& ret, Scene* const scene, const Scene::SpriteAct* const spriteRun, Actor& user, Actor* target, bool const applyCosts) const;
