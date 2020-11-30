/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "actor.h"
#include "costume.h"
#include "ability.h"
#include "state.h"
#include "scene.h"
#include "role.h"

#include <QStringBuilder>
#include <QString>

using namespace tbrpgsca;

QString Actor::KoTxt = ", %1 falls unconscious";
QString Actor::RiseTxt = ", but rises again";

int Actor::remainingSkillUses(Ability& skill) const
{
    QMap<Ability*, int>* crQty = this->_actor_data->_skills_cr_qty;
    return crQty == NIL ? 0 : crQty->value(&skill, 0);
}

int Actor::regeneratingSkillTurn(Ability& skill) const
{
    QMap<Ability*, int>* regTurn = this->_actor_data->_skills_rg_turn;
    return regTurn == NIL ? 0 : regTurn->value(&skill, 0);
}

QMap<Ability*, int> Actor::items() const
{
    const auto& items = this->_actor_data->_items;
    return items == NIL? QMap<Ability*, int>() : *items;
}

Costume& Actor::race() const
{
    return *(this->_actor_data->_equipment[CHAR_RACE]);
}

Costume& Actor::job() const
{
    return *(this->_actor_data->_equipment[CHAR_JOB]);
}

Costume* Actor::equipItem(const char pos, Costume* const item)
{
    assert(pos != CHAR_NONE && pos != CHAR_RACE && pos != CHAR_JOB);
    return this->equipItem(NIL, pos, item);
}

char Actor::unequipItem(Costume& item)
{
    return this->unequipItem(NIL, item);
}

Costume* Actor::unequipPos(const char pos)
{
    assert(pos != CHAR_NONE && pos != CHAR_RACE && pos != CHAR_JOB);
    return this->equipItem(NIL, pos, NIL);
}

Costume* Actor::unequipPos(Scene* const scene, const char pos)
{
    return this->equipItem(scene, pos, NIL);
}

char Actor::unequipItem(Scene* const scene, Costume& item)
{
    QMap<char, Costume*>& equipment = this->_actor_data->_equipment;
    char const old = equipment.key(&item, CHAR_NONE);
    this->equipItem(scene, old, NIL);
    return old;
}

Costume* Actor::equipItem(Scene* const scene, const char pos, Costume* const item)
{
    QMap<char, Costume*>& equipment = this->_actor_data->_equipment;
    Costume* old = equipment.value(pos, NIL);
    switchCostume(NIL, scene, old, item);
    equipment[pos] = item;
    return old;
}

void Actor::removeStates(QString* const ret, Scene* const scene, bool const remove)
{
    Actor& actor = *this;
    QMap<State*, int>* const stateDur = actor._role_data->_state_dur;
    if (stateDur)
    {
        actor.updateStates(true, ret, scene, *stateDur, true);
        if (remove && stateDur->size() == 0)
        {
            actor._role_data->_state_dur = NIL;
            delete stateDur;
        }
    }
}

inline void Actor::setRace(Costume& race)
{
    return this->setRace(NIL, race);
}

inline void Actor::setJob(Costume& job)
{
    return this->setJob(NIL, job);
}

inline void Actor::setRace(Scene* const scene, Costume& race)
{
    Actor& actor = *this;
    actor.equipItem(scene, CHAR_RACE, &race);
}

void Actor::setJob(Scene* const scene, Costume& job)
{
    Actor& actor = *this;
    actor.equipItem(scene, CHAR_JOB, &job);
    if (!actor.Costume::isShapeShifted())
    {
        QString* spr = job._role_data->_sprite;
        if (spr)
        {
            QString* actorSpr = actor._role_data->_sprite;
            if (actorSpr)
            {
                (*actorSpr) = *spr;
            }
            else
            {
                actor._role_data->_sprite = new QString(*spr);
            }
        }
    }
}

/*inline void Actor::setOffense(const int atk)
{
    this->_costume_data->_atk = atk;
}

inline void Actor::setDefense(const int def)
{
    this->_costume_data->_def = def;
}

inline void Actor::setSpirit(const int spi)
{
    this->_costume_data->_spi = spi;
}

inline void Actor::setWisdom(const int wis)
{
    this->_costume_data->_wis = wis;
}

inline void Actor::setAgility(const int agi)
{
    this->_costume_data->_agi = agi;
}*/

void Actor::setAgility(const int agi, Scene& scene)
{
    this->Costume::setAgility(agi);
    scene.agiCalc();
}

void Actor::setCurrentHp(const int hp, QString& ret, const bool survive)
{
    return this->setCurrentHp(hp, &ret, NIL, survive);
}

void Actor::setCurrentHp(const int hp, QString* const ret, Scene* const scene, bool const survive)
{
    Actor& actor = *this;
    QSharedDataPointer<RoleData>& roleData = actor._role_data;
    if (hp < 1)
    {
        if (actor.currentHp() != 0)
        {
            if (survive || actor.Costume::isInvincible())
            {
                roleData->_hp = 1;
            }
            else
            {
                roleData->_sp = 0;
                if (ret)
                {
                    *ret = *ret % Actor::KoTxt.arg(actor.name());
                }
                if (actor.initiative() > 0)
                {
                    actor.setInitiative(0);
                }
                if (actor.Role::isReviving())
                {
                    if (ret)
                    {
                        *ret = *ret % Actor::RiseTxt;
                    }
                    roleData->_hp = actor.maximumHp();
                    if (scene && actor.Costume::isShapeShifted())
                    {
                        emit scene->spriteAct(scene, &actor, NIL, true, NIL, NIL);
                    }
                    actor.removeStates(ret, scene, false);
                }
                else
                {
                    roleData->_hp = 0;
                    actor.removeStates(ret, scene, false);
                    actor.setStunned(true);
                    actor.setKnockedOut(true);
                    if (scene && ret)
                    {
                        scene->checkStatus(ret);
                    }
                }
            }
        }
    }
    else
    {
        int const oHp = actor.currentHp(), mHp = actor.maximumHp();
        roleData->_hp = hp > mHp ? mHp : hp;
        if (oHp < 1)
        {
            actor.setStunned(false);
            actor.setKnockedOut(false);
            actor.refreshCostumes(ret, scene);
            //actor.applyStates(ret, scene, false);
            if (scene)
            {
                scene->resetTurn(actor);
            }
        }
    }
}

void Actor::setCurrentHp(const int hp)
{
    int mHp = this->maximumHp();
    this->_role_data->_hp = hp > mHp ? mHp : (hp < 1 ? 0 : hp);
}

void Actor::setCurrentMp(const int mp)
{
    int mMp = this->maximumMp();
    this->_role_data->_mp = mp > mMp ? mMp : (mp < 1 ? 0 : mp);
}

void Actor::setCurrentRp(const int sp)
{
    int mSp = this->maximumRp();
    this->_role_data->_sp = sp > mSp ? mSp : (sp < 1 ? 0 : sp);
}

void Actor::setMaximumActions(const int mActions)
{
    Actor& actor = *this;
    actor._costume_data->_m_actions = mActions;
    if (mActions < actor.currentActions())
    {
        actor.setCurrentActions(mActions);
    }
}

void Actor::setMaximumHp(const int mHp)
{
    Actor& actor = *this;
    QSharedDataPointer<RoleData>& roleData = actor._role_data;
    roleData->_m_hp = mHp;
    if (mHp < actor.currentHp())
    {
        roleData->_hp = mHp;
    }
}

void Actor::setMaximumMp(const int mMp)
{
    Actor& actor = *this;
    QSharedDataPointer<RoleData>& roleData = actor._role_data;
    roleData->_m_mp = mMp;
    if (mMp < actor.currentMp())
    {
        roleData->_mp = mMp;
    }
}

void Actor::setMaximumRp(const int mRp)
{
    Actor& actor = *this;
    QSharedDataPointer<RoleData>& roleData = actor._role_data;
    roleData->_m_sp = mRp;
    if (mRp < actor.currentRp())
    {
        roleData->_sp = mRp;
    }
}

inline void Actor::setSprite(QString& value)
{
    Actor& actor = *this;
    QSharedDataPointer<RoleData>& roleData = actor._role_data;
    QString* sprite = roleData->_sprite;
    if (sprite)
    {
        delete sprite;
    }
    roleData->_sprite = new QString(value);
}

void Actor::setItems(const QSharedPointer<QMap<Ability*, int>>& items)
{
    this->_actor_data->_items = items;
}

inline void Actor::setMaximumLevel(const int maxLv)
{
    this->_actor_data->_max_lv = maxLv;
}

inline void Actor::setCurrentLevel(const int level)
{
    return this->setCurrentLevel(NIL, level);
}

void Actor::setCurrentLevel(Scene* const scene, const int level)
{
    auto& actorData = this->_actor_data;
    while (level > actorData->_lv)
    {
        actorData->_xp = actorData->_maxp;
        this->levelUp(scene);
    }
    actorData->_lv = level;

}

inline void Actor::setCurrentExperience(const int xp)
{
    return this->setCurrentExperience(NIL, xp);
}

void Actor::setCurrentExperience(Scene* const scene, const int xp)
{
    auto& actorData = this->_actor_data;
    actorData->_xp = xp;
    if (xp >= actorData->_maxp)
    {
        this->levelUp(scene);
    }
}

void Actor::setElementResistance(const int element, const int res)
{
    auto& actorCostData = this->_costume_data;
    QMap<int, int>* rMap = actorCostData->_res;
    if (rMap == NIL)
    {
        rMap = new QMap<int, int>();
        actorCostData->_res = rMap;
    }
    rMap->operator[](element) = res;
}

void Actor::setStateResistance(State* const state, const int res)
{
    auto& actorCostData = this->_costume_data;
    QMap<State*, int>* stRes = actorCostData->_st_res;
    if (stRes == NIL)
    {
        stRes = new QMap<State*, int>();
        actorCostData->_st_res = stRes;
    }
    stRes->operator[](state) = res;
}

void Actor::applyDmgRoles(QString& ret, Scene* const scene)
{
    Actor& actor = *this;
    QVector<Costume*>* const dmgRoles = actor._actor_data->_dmg_roles;
    if (dmgRoles && dmgRoles->size() > 0)
    {
        for (Costume* const role : *dmgRoles)
        {
            role->apply(ret, scene, actor);
        }
        if (scene)
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
        }
    }
}

void Actor::applyStates(QString* const ret, Scene* const scene, const bool consume)
{
    Actor& actor = *this;
    if (consume && ret)
    {
        actor.applyDmgRoles(*ret, scene);
    }
    QMap<State*, int>* const stateDur = actor._role_data->_state_dur;
    if (stateDur)
    {
        auto const last = stateDur->cend();
        for (auto it = stateDur->cbegin(); it != last; ++it)
        {
            if (it.value() > STATE_END_DUR)
            {
                it.key()->alter(ret, scene, actor, consume);
            }
        }
    }
}

inline void Actor::checkRegSkill(Ability& skill)
{
    Actor& actor = *this;
    auto& actorData = actor._actor_data;
    if (skill.usesRegen() > 0)
    {
        QMap<Ability*, int>* regSkills = actorData->_skills_rg_turn;
        if (regSkills == NIL)
        {
            regSkills = new QMap<Ability*, int>();
            actorData->_skills_rg_turn = regSkills;
        }
        regSkills->operator[](&skill) = 0;
    }
}

void Actor::recover(QString& ret)
{
    this->recover(&ret, NIL);
}

void Actor::recover(QString* const ret, Scene* const scene)
{
    Actor& actor = *this;
    actor.removeStates(ret, scene, true);
    actor.refreshCostumes(ret, scene);
    actor.setCurrentActions(actor.maximumActions());
    auto& actorCostData = actor._costume_data;
    QSharedDataPointer<RoleData>& roleData = actor._role_data;
    roleData->_hp = actor.maximumHp();
    roleData->_mp = actor.maximumMp();
    roleData->_sp = 0;
    {
        QMap<int, int>* const res = actorCostData->_res;
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
                actorCostData->_res = NIL;
                delete res;
            }
        }
    }
    {
        QMap<State*, int>* const stRes = actorCostData->_st_res;
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
                actorCostData->_st_res = NIL;
                delete stRes;
            }
        }
    }
    {
        QMap<Ability*, int>* skillsQty = actor._actor_data->_skills_cr_qty;
        if (skillsQty)
        {
            auto const last = skillsQty->cend();
            for (auto it = skillsQty->cbegin(); it != last; ++it)
            {
                Ability* const ability = it.key();
                skillsQty->operator[](ability) = ability->maximumUses();
            }
        }
    }
}

void Actor::levelUp(Scene* const scene)
{
    Actor& actor = *this;
    int lv = actor.currentLevel();
    int const maxp = actor.maximumExperience(), xp = actor.currentExperience(), maxLv = actor.maximumLevel();
    QSharedDataPointer<RoleData>& roleData = actor._role_data;
    while (maxp <= xp && lv < maxLv)
    {
        roleData->_m_hp += 3;
        roleData->_m_mp += 2;
        roleData->_m_sp += 2;
        actor.setMaximumExperience(maxp * 2);
        actor.setOffense(actor.offense() + 1);
        actor.setDefense(actor.defense() + 1);
        actor.setWisdom(actor.wisdom() + 1);
        actor.setSpirit(actor.spirit() + 1);
        if (scene == NIL)
        {
            actor.Costume::setAgility(actor.agility() + 1);
        }
        else
        {
            actor.setAgility(actor.agility() + 1, *scene);
        }
        lv += 1;
    }
    actor._actor_data->_lv = lv;
}

void Actor::switchCostume(QString* const ret, Scene* const scene, Costume* const oldCost, Costume* const newCost)
{
    Actor& actor = *this;
    if (oldCost)
    {
        oldCost->adopt(ret, scene, actor, true, true);
    }
    if (newCost)
    {
        newCost->adopt(ret, scene, actor, true, false);
    }
}

void Actor::updateAttributes(const bool remove, Scene* const scene, Costume& costume)
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
    if (scene == NIL)
    {
        actor.Costume::setAgility(actor.agility() + (i * costume.agility()));
    }
    else
    {
        actor.setAgility(actor.agility() + (i * costume.agility()), *scene);
    }
}

void Actor::updateResistance(const bool remove, QMap<int, int>* const elmRes, QMap<State *, int>* const stRes)
{
    auto& actorCostData = this->_costume_data;
    if (elmRes)
    {
        QMap<int, int>* aElmRes = actorCostData->_res;
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
                actorCostData->_res = aElmRes;
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
        QMap<State*, int>* aStateRes = actorCostData->_st_res;
        if (remove)
        {
            if (aStateRes)
            {
                auto const last = stRes->cend();
                for (auto it = stRes->cbegin(); it != last; ++it)
                {
                    int v = it.value();
                    State* const i = it.key();
                    aStateRes->operator[](i) = aStateRes->value(i, v) - v;
                }
            }
        }
        else
        {
            if (aStateRes == NIL)
            {
                aStateRes = new QMap<State*, int>();
                actorCostData->_st_res = aStateRes;
            }
            auto const last = stRes->cend();
            for (auto it = stRes->cbegin(); it != last; ++it)
            {
                int v = it.value();
                State* const i = it.key();
                aStateRes->operator[](i) = aStateRes->value(i, 0) + v;
            }
        }
    }
}

void Actor::updateSkills(const bool remove, const bool counters, QVector<Ability*>& skills)
{
    Actor& actor = *this;
    auto& actorData = actor._actor_data;
    auto& actorCostData = actor._costume_data;
    QVector<Ability*>* aSkills = counters ? actorCostData->_counters : actorCostData->_a_skills;
    if (remove)
    {
        if (aSkills)
        {
            for (Ability* const ability : skills)
            {
                aSkills->removeOne(ability);
                if (ability->usesRegen() > 0)
                {
                    QMap<Ability*, int>* regTurn = actorData->_skills_rg_turn;
                    if (regTurn)
                    {
                        regTurn->remove(ability);
                    }
                }
                if (ability->maximumUses() > 0)
                {
                    QMap<Ability*, int>* crQty = actorData->_skills_cr_qty;
                    if (crQty)
                    {
                        crQty->remove(ability);
                    }
                }
            }
        }
    }
    else
    {
        if (aSkills == NIL)
        {
            aSkills = new QVector<Ability*>();
            if (counters)
            {
                actorCostData->_counters = aSkills;
            }
            else
            {
                actorCostData->_a_skills = aSkills;
            }
        }
        for (Ability* const ability : skills)
        {
            if (!aSkills->contains(ability))
            {
                aSkills->append(ability);
                int const mQty = ability->maximumUses();
                if (mQty > 0)
                {
                    QMap<Ability*, int>* crQty = actorData->_skills_cr_qty;
                    if (crQty == NIL)
                    {
                        crQty = new QMap<Ability*, int>();
                        actorData->_skills_cr_qty = crQty;
                    }
                    crQty->operator[](ability) = mQty;
                    actor.checkRegSkill(*ability);
                }
            }
        }
    }
}

void Actor::updateStates(bool const remove, QString* const ret, Scene* const scene,
                           QMap<State*, int>& states, bool const includeWithDur)
{
    Actor& actor = *this;
    if (remove)
    {
        QMap<State*, int>* const stateDur = actor._role_data->_state_dur;
        if (stateDur && stateDur->size() > 0)
        {
            auto const last = states.cend();
            for (auto it = states.cbegin(); it != last; ++it)
            {
                int const rDur = it.value();
                if (includeWithDur || (rDur < 0 && rDur > STATE_END_DUR))
                {
                    it.key()->disable(ret, scene, actor, rDur, includeWithDur);
                }
            }
        }
    }
    else
    {
        auto const last = states.cend();
        for (auto it = states.cbegin(); it != last; ++it)
        {
            int const rDur = it.value();
            if (includeWithDur || (rDur < 0 && rDur > STATE_END_DUR))
            {
                it.key()->inflict(ret, scene, NIL, actor, rDur, true);
            }
        }
    }
}

void Actor::refreshCostumes(QString* const ret, Scene* const scene)
{
    Actor& actor = *this;
    {
        QMap<char, Costume*>& equipment = actor._actor_data->_equipment;
        auto const last = equipment.cend();
        for (auto it = equipment.cbegin(); it != last; ++it)
        {
            it.value()->refresh(ret, scene, actor, true, false);
        }
    }
    QMap<State*, int>* const stateDur = actor._role_data->_state_dur;
    if (stateDur)
    {
        auto const last = stateDur->cend();
        for (auto it = stateDur->cbegin(); it != last; ++it)
        {
            if (it.value() > STATE_END_DUR)
            {
                it.key()->refresh(ret, scene, actor, false, false);
            }
        }
    }
}

Actor::Actor(int const id, QString name, QString sprite, Costume& race, Costume& job, int const level, int const maxLv, int const mActions,
             int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
             QMap<int, int>* const res, QMap<State*, int>* const stRes, const QSharedPointer<QMap<Ability*, int>>&& items)
    : Actor(id, name, sprite, race, job, level, maxLv, mActions, mHp, mMp, mSp, atk, def, spi, wis, agi, res, stRes, items) {}

Actor::Actor(int const id, QString name, QString sprite, Costume& race, Costume& job, int const level, int const maxLv, int const mActions,
             int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
             QMap<int, int>* const res, QMap<State*, int>* const stRes, const QSharedPointer<QMap<Ability*, int>>& items)
    : Costume(id, name, sprite, false, mActions, 0, mHp, mMp, 0, mHp, mMp, mSp, atk, def, spi, wis, agi, false, false, false, false, false, false, false, false, new QVector<Ability*>(),
              NIL, NIL, stRes, res)
{
    QSharedDataPointer<ActorData> actorData(new ActorData);
    actorData->_lv = 1;
    actorData->_xp = 0;
    actorData->_maxp = 15;
    actorData->_side = 0;
    actorData->_init = 0;
    actorData->_side = 0;
    actorData->_old_side = 0;
    actorData->_dmg_roles = NIL;
    actorData->_skills_rg_turn = NIL;
    actorData->_skills_cr_qty = NIL;
    this->_role_data->_state_dur = NIL;
    actorData->_actions = mActions;
    actorData->_max_lv = maxLv;
    actorData->_items = items;
    actorData->_extra = NIL;
    this->setRace(race);
    this->setJob(job);
    this->setCurrentLevel(level);
    this->recover(NIL, NIL);
}

Actor::Actor(Actor& actor) : Costume(actor)
{
    this->_actor_data = actor._actor_data;
    /*auto& copyCostData = actor._costume_data;
    auto& actorCostData = this->_costume_data;
    QVector<Ability*>* aSkills = new QVector<Ability*>();
    this->_items = actor.hasNewItems() ? new QMap<Ability*, int>(*(actor._items)) : actor._items;
    *(aSkills) = *(copyCostData->_a_skills);
    actorCostData->_a_skills = aSkills;
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
        QMap<State*, int>* crSkillsQty = actor._role_data->_state_dur;
        if (crSkillsQty == NIL)
        {
            this->_role_data->_state_dur = NIL;
        }
        else
        {
            QMap<State*, int>* nSkillsQty = new QMap<State*, int>();
            (*nSkillsQty) = (*crSkillsQty);
            this->_role_data->_state_dur = nSkillsQty;
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
            actorCostData->_a_skills = NIL;
        }
        else
        {
            QVector<Ability*>* nSkillsRgTurn = new QVector<Ability*>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            actorCostData->_a_skills = nSkillsRgTurn;
        }
    }
    {
        QVector<Ability*>* skillsRgTurn = copyCostData->_counters;
        if (skillsRgTurn == NIL)
        {
            actorCostData->_counters = NIL;
        }
        else
        {
            QVector<Ability*>* nSkillsRgTurn = new QVector<Ability*>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            actorCostData->_counters = nSkillsRgTurn;
        }
    }*/
}

Actor::ActorData::~ActorData()
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
    /*auto& actorCostData = this->_costume_data;
    auto stRes = actorCostData->_st_res;
    if (stRes)
    {
        actorCostData->_st_res = NIL;
        delete stRes;
    }
    auto res = actorCostData->_res;
    if (res)
    {
        actorCostData->_res = NIL;
        delete res;
    }
    QSharedDataPointer<RoleData>& roleData = this->_role_data;
    auto states = roleData->_state_dur;
    if (states)
    {
        roleData->_state_dur = NIL;
        delete states;
    }
    auto skills = actorCostData->_a_skills;
    if (skills)
    {
        actorCostData->_a_skills = NIL;
        delete skills;
    }*/
}
