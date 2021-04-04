/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "actor.h"
#include "costume.h"
#include "ability.h"
#include "ailment.h"
#include "scene.h"
#include "role.h"

#include <QStringBuilder>
#include <QString>

using namespace tbrpgsca;

QString Actor::KoTxt = ", %1 falls unconscious";
QString Actor::RiseTxt = ", but rises again";

QMap<Ability*, int> Actor::items() const
{
    const auto items = this->actorData()._items;
    return items == NIL? QMap<Ability*, int>() : *items;
}

Costume& Actor::race() const
{
    return *(this->actorData()._equipment[CHAR_RACE]);
}

Costume& Actor::job() const
{
    return *(this->actorData()._equipment[CHAR_JOB]);
}

const Costume* Actor::unequipPos(const char pos)
{
    return this->equipItem(pos, NIL);
}

char Actor::unequipItem(Costume& item)
{
    const QMap<char, Costume*>& equipment = this->actorData()._equipment;
    char const old = equipment.key(&item, CHAR_NONE);
    this->equipItem(old, NIL);
    return old;
}

const Costume* Actor::equipItem(const char pos, Costume* const item)
{
    QMap<char, Costume*>& equipment = this->actorMutData()._equipment;
    Costume* old = equipment.value(pos, NIL);
    switchCostume(NIL, old, item);
    equipment[pos] = item;
    return old;
}

void Actor::removeStates(QString* const ret, bool const remove)
{
    Actor& actor = *this;
    QList<Ailment>* const stateDur = actor.roleMutData()._a_states;
    if (stateDur)
    {
        actor.updateStates(true, ret, *stateDur, true);
        if (remove && stateDur->size() == 0)
        {
            actor.roleMutData()._a_states = NIL;
            delete stateDur;
        }
    }
}

void Actor::applyStates(QString& ret, const bool consume)
{
    this->applyStates(&ret, consume);
}

void Actor::setRace(Costume& race)
{
    Actor& actor = *this;
    actor.equipItem(CHAR_RACE, &race);
}

void Actor::setJob(Costume& job)
{
    Actor& actor = *this;
    actor.equipItem(CHAR_JOB, &job);
    if (!actor.Costume::isShapeShifted())
    {
        QString* spr = job.roleMutData()._sprite;
        if (spr)
        {
            QString* actorSpr = actor.roleMutData()._sprite;
            if (actorSpr)
            {
                (*actorSpr) = *spr;
            }
            else
            {
                actor.roleMutData()._sprite = new QString(*spr);
            }
        }
    }
}

/*inline void Actor::setOffense(const int atk)
{
    this->costumeData()._atk = atk;
}

inline void Actor::setDefense(const int def)
{
    this->costumeData()._def = def;
}

inline void Actor::setSpirit(const int spi)
{
    this->costumeData()._spi = spi;
}

inline void Actor::setWisdom(const int wis)
{
    this->costumeData()._wis = wis;
}

inline void Actor::setAgility(const int agi)
{
    this->costumeData()._agi = agi;
}

void Actor::setAgility(const int agi, Scene& scene)
{
    this->Costume::setAgility(agi);
    //scene.agiCalc();
}*/


void Actor::setCurrentHp(const int hp)
{
    const int mHp = this->maximumHp();
    this->Costume::setCurrentHp(hp > mHp ? mHp : (hp < 1 ? 0 : hp));
}

void Actor::setCurrentMp(const int mp)
{
    const int mMp = this->maximumMp();
    this->Costume::setCurrentMp(mp > mMp ? mMp : (mp < 1 ? 0 : mp));
}

void Actor::setCurrentRp(const int sp)
{
    const int mSp = this->maximumRp();
    this->Costume::setCurrentRp(sp > mSp ? mSp : (sp < 1 ? 0 : sp));
}

void Actor::setMaximumActions(const int mActions)
{
    Actor& actor = *this;
    actor.Costume::setMaximumActions(mActions);
    if (mActions < actor.currentActions())
    {
        actor.setCurrentActions(mActions);
    }
}

void Actor::setMaximumHp(const int mHp)
{
    Actor& actor = *this;
    //QSharedDataPointer<RoleData>& roleData = actor._role_data;
    this->Costume::setMaximumHp(mHp);
    if (mHp < actor.currentHp())
    {
        this->Costume::setCurrentHp(mHp);
    }
}

void Actor::setMaximumMp(const int mMp)
{
    Actor& actor = *this;
    //QSharedDataPointer<RoleData>& roleData = actor._role_data;
    this->Costume::setMaximumMp(mMp);
    if (mMp < actor.currentMp())
    {
        this->Costume::setCurrentMp(mMp);
    }
}

void Actor::setMaximumRp(const int mRp)
{
    Actor& actor = *this;
    //QSharedDataPointer<RoleData>& roleData = actor._role_data;
    this->Costume::setMaximumRp(mRp);
    if (mRp < actor.currentRp())
    {
        this->Costume::setCurrentRp(mRp);
    }
}

void Actor::setSprite(QString& value)
{
    Actor& actor = *this;
    auto& roleData = actor.roleMutData();
    QString* sprite = roleData._sprite;
    if (sprite)
    {
        delete sprite;
    }
    roleData._sprite = new QString(value);
}

void Actor::setItems(const QSharedPointer<QMap<Ability*, int>>& items)
{
    this->actorMutData()._items = items;
}

void Actor::setMaximumLevel(const int maxLv)
{
    this->actorMutData()._max_lv = maxLv;
}

void Actor::setCurrentLevel(const int level)
{
    auto& actorData = this->actorMutData();
    while (level > actorData._lv)
    {
        actorData._xp = actorData._maxp;
        this->levelUp();
    }
    actorData._lv = level;

}

void Actor::setCurrentExperience(const int xp)
{
    auto& actorData = this->actorMutData();
    actorData._xp = xp;
    if (xp >= actorData._maxp)
    {
        this->levelUp();
    }
}

void Actor::setElementResistance(const int element, const int res)
{
    auto& actorCostData = this->costumeMutData();
    QMap<int, int>* rMap = actorCostData._res;
    if (rMap == NIL)
    {
        rMap = new QMap<int, int>();
        actorCostData._res = rMap;
    }
    rMap->operator[](element) = res;
}

void Actor::setStateResistance(const int state, const int res)
{
    auto& actorCostData = this->costumeMutData();
    auto stRes = actorCostData._st_res;
    if (stRes == NIL)
    {
        stRes = new QMap<int, int>();
        actorCostData._st_res = stRes;
    }
    stRes->operator[](state) = res;
}

void Actor::applyDmgRoles(QString& ret)
{
    Actor& actor = *this;
    QVector<const Costume*>* const dmgRoles = actor.actorData()._dmg_roles;
    if (dmgRoles && dmgRoles->size() > 0)
    {
        for (const Costume* const role : *dmgRoles)
        {
            role->apply(ret, actor);
        }
        /*if (scene) //TODO: adapt without using "scene" pointer here
        {
            emit scene->spriteAct(scene, &actor, NIL, false, NIL, NIL);
#if USE_TARGET_LIST
            {
                QVector<Actor*>* targets = scene->_scene_data->_targets;
                if (targets == NIL)
                {
                    targets = new QVector<Actor*>(1);
                    scene->_scene_data->_targets = targets;
                }
                else
                {
                    targets->clear();
                }
                targets->append(this);
            }
#endif
        }*/
    }
}

void Actor::applyStates(QString* const ret, const bool consume)
{
    Actor& actor = *this;
    if (consume && ret)
    {
        actor.applyDmgRoles(*ret);
    }
    if (!consume)
    {
        QList<Ailment>* const stateDur = actor.roleMutData()._a_states;
        if (stateDur)
        {
            /*auto const last = stateDur->cend();
            for (auto it = stateDur->cbegin(); it != last; ++it)
            {
                if (it.value() > Ailment::EndDur)
                {
                    it.key()->alter(ret, actor, consume);
                }
            }*/
            const int statesSize = stateDur->size();
            for (int i = 0; i < statesSize; ++i)
            {
                Ailment& aState = static_cast<Ailment&>(stateDur->operator[](i));
                if (aState.duration() > Ailment::EndDur)
                {
                    aState.alter(ret, actor, false);
                }
            }
        }
    }
}

/*inline void Actor::checkRegSkill(const Ability& skill)
{
    Actor& actor = *this;
    auto actorData = actor._actor_data;
    if (skill.usesRegen() > 0)
    {
        QMap<const Ability*, int>* regSkills = actorData->_skills_rg_turn;
        if (regSkills == NIL)
        {
            regSkills = new QMap<const Ability*, int>();
            actorData->_skills_rg_turn = regSkills;
        }
        regSkills->operator[](&skill) = 0;
    }
}*/

void Actor::recover(QString& ret)
{
    this->recover(&ret);
}

void Actor::recover(QString* const ret)
{
    Actor& actor = *this;
    actor.removeStates(ret, true);
    actor.refreshCostumes(ret);
    actor.setCurrentActions(actor.maximumActions());
    auto actorCostData = actor.costumeData();
    auto& roleData = actor.roleMutData();
    roleData._hp = actor.maximumHp();
    roleData._mp = actor.maximumMp();
    roleData._sp = 0;
    {
        QMap<int, int>* const res = actorCostData._res;
        if (res)
        {
            {
                auto const last = res->cend();
                for (auto it = res->cbegin(); it != last; ++it)
                {
                    if (it.value() == 0)
                    {
                        res->remove(it.key());
                    }
                }
            }
            if (res->size() == 0)
            {
                actorCostData._res = NIL;
                delete res;
            }
        }
    }
    {
        QMap<int, int>* const stRes = actorCostData._st_res;
        if (stRes)
        {
            {
                auto const last = stRes->cend();
                for (auto it = stRes->cbegin(); it != last; ++it)
                {
                    if (it.value() == 0)
                    {
                        stRes->remove(it.key());
                    }
                }
            }
            if (stRes->size() == 0)
            {
                actorCostData._st_res = NIL;
                delete stRes;
            }
        }
    }
    {
        /*QMap<const Ability*, int>* skillsQty = actor.actorData()._skills_cr_qty;
        if (skillsQty)
        {
            auto const last = skillsQty->cend();
            for (auto it = skillsQty->cbegin(); it != last; ++it)
            {
                const Ability* const ability = it.key();
                skillsQty->operator[](ability) = ability->maximumUses();
            }
        }*/
        if (actor.hasQtySkills())
        {
            for (auto& ability : *actor.costumeData()._a_skills)
            {
                ability.replenish();
            }
        }
    }
}

void Actor::levelUp()
{
    Actor& actor = *this;
    int lv = actor.currentLevel();
    int const maxp = actor.maximumExperience(), xp = actor.currentExperience(), maxLv = actor.maximumLevel();
    auto& roleData = actor.actorMutData();
    while (maxp <= xp && lv < maxLv)
    {
        roleData._m_hp += 3;
        roleData._m_mp += 2;
        roleData._m_sp += 2;
        actor.setMaximumExperience(maxp * 2);
        actor.setOffense(actor.offense() + 1);
        actor.setDefense(actor.defense() + 1);
        actor.setWisdom(actor.wisdom() + 1);
        actor.setSpirit(actor.spirit() + 1);
        actor.setAgility(actor.agility() + 1);
        /*if (scene == NIL)
        {
            actor.Costume::setAgility(actor.agility() + 1);
        }
        else
        {
            actor.setAgility(actor.agility() + 1, *scene);
        }*/
        lv += 1;
    }
    roleData._lv = lv;
}

void Actor::switchCostume(QString* const ret, const Costume* const oldCost, const Costume* const newCost)
{
    Actor& actor = *this;
    if (oldCost)
    {
        oldCost->adopt(ret, actor, true, true);
    }
    if (newCost)
    {
        newCost->adopt(ret, actor, true, false);
    }
}

void Actor::updateAttributes(const bool remove, const Costume& costume)
{
    Actor& actor = *this;
    int const i = remove ? -1 : 1;
    actor.setMaximumHp(actor.maximumHp() + (i * costume.maximumHp()));
    actor.setMaximumMp(actor.maximumMp() + (i * costume.maximumMp()));
    actor.setMaximumRp(actor.maximumRp() + (i * costume.maximumRp()));
    actor.setMaximumActions(actor.maximumActions() + (i * costume.maximumActions()));
    actor.setOffense(actor.offense() + (i * costume.offense()));
    actor.setDefense(actor.defense() + (i * costume.defense()));
    actor.setSpirit(actor.spirit() + (i * costume.spirit()));
    actor.setWisdom(actor.wisdom() + (i * costume.wisdom()));
    actor.setAgility(actor.agility() + (i * costume.agility()));
    /*if (scene == NIL)
    {
        actor.Costume::setAgility(actor.agility() + (i * costume.agility()));
    }
    else
    {
        actor.setAgility(actor.agility() + (i * costume.agility()), *scene);
    }*/
}

void Actor::updateResistance(const bool remove, QMap<int, int>* const elmRes, QMap<int, int>* const stRes)
{
    auto& actorCostData = this->costumeMutData();
    if (elmRes)
    {
        QMap<int, int>* aElmRes = actorCostData._res;
        if (remove)
        {
            if (aElmRes)
            {
                auto const last = elmRes->cend();
                for (auto it = elmRes->cbegin(); it != last; ++it)
                {
                    int const i = it.key(), v = it.value();
                    aElmRes->operator[](i) = aElmRes->value(i, v) - v;
                }
            }
        }
        else
        {
            if (aElmRes == NIL)
            {
                aElmRes = new QMap<int, int>();
                actorCostData._res = aElmRes;
            }
            auto const last = elmRes->cend();
            for (auto it = elmRes->cbegin(); it != last; ++it)
            {
                int const i = it.key(), v = it.value();
                aElmRes->operator[](i) = aElmRes->value(i, 0) + v;
            }
        }
    }
    if (stRes)
    {
        auto aStateRes = actorCostData._st_res;
        if (remove)
        {
            if (aStateRes)
            {
                auto const last = stRes->cend();
                for (auto it = stRes->cbegin(); it != last; ++it)
                {
                    int v = it.value();
                    auto i = it.key();
                    aStateRes->operator[](i) = aStateRes->value(i, v) - v;
                }
            }
        }
        else
        {
            if (aStateRes == NIL)
            {
                aStateRes = new QMap<int, int>();
                actorCostData._st_res = aStateRes;
            }
            auto const last = stRes->cend();
            for (auto it = stRes->cbegin(); it != last; ++it)
            {
                int v = it.value();
                auto i = it.key();
                aStateRes->operator[](i) = aStateRes->value(i, 0) + v;
            }
        }
    }
}

void Actor::updateSkills(const bool remove, const bool counters, QList<Ability>& skills)
{
    Actor& actor = *this;
    auto actorData = actor.actorData();
    auto aSkills = counters ? actorData._counters : actorData._a_skills;
    if (remove)
    {
        if (aSkills)
        {
            for (Ability& ability : skills)
            {
                aSkills->removeOne(ability);
                /*if (ability.usesRegen() > 0)
                {
                    QMap<const Ability*, int>* regTurn = actorData->_skills_rg_turn;
                    if (regTurn)
                    {
                        regTurn->remove(&ability);
                    }
                }
                if (ability.maximumUses() > 0)
                {
                    QMap<const Ability*, int>* crQty = actorData->_skills_cr_qty;
                    if (crQty)
                    {
                        crQty->remove(&ability);
                    }
                }*/
            }
        }
    }
    else
    {
        if (aSkills == NIL)
        {
            aSkills = new QList<Ability>();
            if (counters)
            {
                actorData._counters = aSkills;
            }
            else
            {
                actorData._a_skills = aSkills;
            }
        }
        for (Ability& ability : skills)
        {
            if (!aSkills->contains(ability))
            {
                aSkills->append(ability);
                if (ability.maximumUses() != 0)
                {
                    /*QMap<const Ability*, int>* crQty = actorData->_skills_cr_qty;
                    if (crQty == NIL)
                    {
                        crQty = new QMap<const Ability*, int>();
                        actorData->_skills_cr_qty = crQty;
                    }
                    crQty->operator[](&ability) = mQty;
                    actor.checkRegSkill(ability);*/
                    this->setHasQtySkills(true);
                    ability.replenish();
                }
            }
        }
    }
}

void Actor::updateStates(bool const remove, QString* const ret, QList<Ailment>& states, bool const includeWithDur)
{
    Actor& actor = *this;
    if (remove)
    {
        int statesSize;
        QList<Ailment>* const stateDur = actor.roleMutData()._a_states;
        if (stateDur && (statesSize = stateDur->size()) > 0)
        {
            /*auto const last = states.cend();
            for (auto it = states.cbegin(); it != last; ++it)
            {
                int const rDur = it.value();
                if (includeWithDur || (rDur < 0 && rDur > Ailment::EndDur))
                {
                    it.key()->disable(ret, actor, rDur, includeWithDur);
                }
            }*/
            for (int i = 0; i < statesSize; ++i)
            {
                Ailment& aState = static_cast<Ailment&>(stateDur->operator[](i));
                for (auto& rState : states)
                {
                    if (aState.databaseId() == rState.databaseId())
                    {
                        int const rDur = rState.duration();
                        if (includeWithDur || (rDur < 0 && rDur > Ailment::EndDur))
                        {
                            aState.disable(ret, actor, rDur, includeWithDur);
                        }
                    }
                }
            }
        }
    }
    else
    {
        /*auto const last = states.cend();
        for (auto it = states.cbegin(); it != last; ++it)
        {
            int const rDur = it.value();
            if (includeWithDur || (rDur < 0 && rDur > Ailment::EndDur))
            {
                it.key()->inflict(ret, NIL, actor, rDur, true);
            }
        }*/
        for (auto& aState : states)
        {
            int const rDur = aState.duration();
            if (includeWithDur || (rDur < 0 && rDur > Ailment::EndDur))
            {
                aState.inflict(ret, NIL, actor, rDur, true);
            }
        }
    }
}

void Actor::refreshCostumes(QString* const ret)
{
    Actor& actor = *this;
    {
        const QMap<char, Costume*>& equipment = actor.actorData()._equipment;
        auto const last = equipment.cend();
        for (auto it = equipment.cbegin(); it != last; ++it)
        {
            it.value()->refresh(ret, actor, true, false);
        }
    }
    QList<Ailment>* const stateDur = actor.roleMutData()._a_states;
    if (stateDur)
    {
        /*auto const last = stateDur->cend();
        for (auto it = stateDur->cbegin(); it != last; ++it)
        {
            if (it.value() > Ailment::EndDur)
            {
                it.key()->refresh(ret, actor, false, false);
            }
        }*/
        const int aStatesSize = stateDur->size();
        for (int i = 0; i < aStatesSize; ++i)
        {
            Ailment& state = static_cast<Ailment&>(stateDur->operator[](i));
            if (state.duration() > Ailment::EndDur)
            {
                state.refresh(ret, actor, false, false);
            }
        }
    }
}

Actor::ActorSheet::ActorSheet(int const id, QString& name, QString& sprite, /*Costume& race, Costume& job, int const level,*/ int const maxLv, int const mActions,
                              int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
                              QMap<int, int>* const res, QMap<int, int>* const stRes, const QSharedPointer<QMap<Ability*, int>>& items)
    : CostumeSheet(id, name, sprite, false, mActions, 0, 0, mHp, mMp, 0, mHp, mMp, mSp, atk, def, spi, wis, agi, false,
                   false, false, false, false, false, false, false, new QList<Ability>(), NIL, NIL, stRes, res)
{
    this->_lv = 1;
    this->_xp = 0;
    this->_maxp = 15;
    this->_side = 0;
    this->_init = 0;
    this->_side = 0;
    this->_old_side = 0;
    this->_dmg_roles = NIL;
    //this->_skills_rg_turn = NIL;
    //this->_skills_cr_qty = NIL;
    this->_a_states = NIL;
    this->_actions = mActions;
    this->_max_lv = maxLv;
    this->_items = items.isNull() ? NIL : items;
    this->_extra = NIL;
    /*this->setRace(race);
    this->setJob(job);
    this->setCurrentLevel(level);
    this->recover(NIL);*/
}

Actor::Actor(int const id, QString&& name, QString&& sprite, Costume&& race, Costume&& job, int const level, int const maxLv, int const mActions, int const mHp, int const mMp,
             int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, QMap<int, int>* const res, QMap<int, int>* const stRes,
             const QSharedPointer<QMap<Ability*, int>>&& items, QObject* const parent)
    : Actor(id, name, sprite, race, job, level, maxLv, mActions, mHp, mMp, mSp, atk, def, spi, wis, agi, res, stRes, items, parent) {}

Actor::Actor(int const id, QString& name, QString& sprite, Costume& race, Costume& job, int const level, int const maxLv, int const mActions, int const mHp, int const mMp,
             int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, QMap<int, int>* const res, QMap<int, int>* const stRes,
             const QSharedPointer<QMap<Ability*, int>>& items, QObject* const parent)
    : Actor(parent, new ActorSheet(id, name, sprite, maxLv, mActions, mHp, mMp, mSp, atk, def, spi, wis, agi, res, stRes, items))
{
    this->setRace(race);
    this->setJob(job);
    this->setCurrentLevel(level);
    this->recover(NIL);
}

Actor::Actor(QObject* const parent) : Actor(0, NIL, NIL, Costume(NIL), Costume(NIL), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NIL, NIL, QSharedPointer<QMap<Ability*, int>>(), parent) {}

Actor::Actor(QObject* const parent, ActorSheet* const actorDataPtr) : Costume(parent, actorDataPtr) {}

Actor::Actor(const Actor&& actor) : Actor(static_cast<const Actor&>(actor)) {}

Actor::Actor(const Actor& actor) : Costume(actor)
{
    /*this->_actor_data = actor._actor_data;
    auto& copyCostData = actor.costumeData();
    auto& actorCostData = this->costumeData();
    QVector<Ability*>* aSkills = new QVector<Ability*>();
    this->_items = actor.hasNewItems() ? new QMap<Ability*, int>(*(actor._items)) : actor._items;
    *(aSkills) = *(copyCostData->_a_skills);
    actorCostData._a_skills = aSkills;
    this->_equipment = actor._equipment;
    {
        QMap<Ability*, int>* crSkillsQty = actor._skills_cr_qty;
        if (crSkillsQty == NIL)
        {
            this->_skills_cr_qty = NIL;
        }
        else
        {
            QMap<Ability*, int>* nSkillsQty = new QMap<Ability*, int>();
            (*nSkillsQty) = (*crSkillsQty);
            this->_skills_cr_qty = nSkillsQty;
        }

    }
    {
        QMap<Ability*, int>* skillsRgTurn = actor._skills_rg_turn;
        if (skillsRgTurn == NIL)
        {
            this->_skills_rg_turn = NIL;
        }
        else
        {
            QMap<Ability*, int>* nSkillsRgTurn = new QMap<Ability*, int>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            this->_skills_rg_turn = nSkillsRgTurn;
        }
    }
    {
        QMap<State*, int>* crSkillsQty = actor.roleMutData()._state_dur;
        if (crSkillsQty == NIL)
        {
            this->roleMutData()._state_dur = NIL;
        }
        else
        {
            QMap<State*, int>* nSkillsQty = new QMap<State*, int>();
            (*nSkillsQty) = (*crSkillsQty);
            this->roleMutData()._state_dur = nSkillsQty;
        }
    }
    {
        QVector<Costume*>* skillsRgTurn = actor._dmg_roles;
        if (skillsRgTurn == NIL)
        {
            this->_dmg_roles = NIL;
        }
        else
        {
            QVector<Costume*>* nSkillsRgTurn = new QVector<Costume*>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            this->_dmg_roles = nSkillsRgTurn;
        }
    }
    {
        QVector<Ability*>* skillsRgTurn = copyCostData->_a_skills;
        if (skillsRgTurn == NIL)
        {
            actorCostData._a_skills = NIL;
        }
        else
        {
            QVector<Ability*>* nSkillsRgTurn = new QVector<Ability*>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            actorCostData._a_skills = nSkillsRgTurn;
        }
    }
    {
        QVector<Ability*>* skillsRgTurn = copyCostData->_counters;
        if (skillsRgTurn == NIL)
        {
            actorCostData._counters = NIL;
        }
        else
        {
            QVector<Ability*>* nSkillsRgTurn = new QVector<Ability*>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            actorCostData._counters = nSkillsRgTurn;
        }
    }*/
}

Actor::ActorSheet::~ActorSheet()
{
    auto dmgRoles = this->_dmg_roles;
    if (dmgRoles)
    {
        this->_dmg_roles = NIL;
        delete dmgRoles;
    }
    //TODO: use shared pointer;
    /*if (this->hasNewItems())
    {
        //this->setNewItems(false);
        QMap<Ability*, int>* items = this->_items;
        if (items)
        {
            this->_items = NIL;
            delete items;
        }
    }*/
}

Actor::~Actor()
{
    //TODO: analyze moving to ActorData destructor
    /*auto& actorCostData = this->costumeData();
    auto stRes = actorCostData._st_res;
    if (stRes)
    {
        actorCostData._st_res = NIL;
        delete stRes;
    }
    auto res = actorCostData._res;
    if (res)
    {
        actorCostData._res = NIL;
        delete res;
    }
    QSharedDataPointer<RoleData>& roleData = this->_role_data;
    auto states = roleData->_state_dur;
    if (states)
    {
        roleData->_state_dur = NIL;
        delete states;
    }
    auto skills = actorCostData._a_skills;
    if (skills)
    {
        actorCostData._a_skills = NIL;
        delete skills;
    }*/
}
