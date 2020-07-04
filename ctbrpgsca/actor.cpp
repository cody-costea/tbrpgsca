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

int Actor::getLevel() const
{
    return this->_lv;
}

int Actor::getExperience() const
{
    return this->_xp;
}

int Actor::getMaximumLevel() const
{
    return this->_max_lv;
}

int Actor::getMaximumExperience() const
{
    return this->_maxp;
}

int Actor::getCurrentActions() const
{
    return this->_actions;
}

int Actor::getInitiative() const
{
    return this->_init;
}

int Actor::getRemainingSkillUses(Ability& skill) const
{
    QMap<Ability*, int>* crQty = this->_skills_cr_qty;
    return crQty == nullptr ? 0 : crQty->value(&skill, 0);
}

int Actor::getRegeneratingSkillTurn(Ability& skill) const
{
    QMap<Ability*, int>* regTurn = this->_skills_rg_turn;
    return regTurn == nullptr ? 0 : regTurn->value(&skill, 0);
}

QMap<Ability*, int> Actor::getItems() const
{
    QMap<Ability*, int>* items = this->_items;
    return items == nullptr? QMap<Ability*, int>() : *items;
}

Costume& Actor::getRace() const
{
    return *(this->_equipment[CHAR_RACE]);
}

Costume& Actor::getJob() const
{
    return *(this->_equipment[CHAR_JOB]);
}

void* Actor::getExtra() const
{
    return this->_extra;
}

bool Actor::hasNewItems() const
{
    return this->hasFlag(FLAG_NEW_ITEMS);
}

bool Actor::isAiPlayer() const
{
    return this->hasFlag(FLAG_AI_PLAYER);
}

bool Actor::isRandomAi() const
{
    return this->hasFlag(FLAG_RANDOM_AI);
}

Costume* Actor::equipItem(const char pos, Costume* const item)
{
    assert(pos != CHAR_NONE && pos != CHAR_RACE && pos != CHAR_JOB);
    return this->equipItem(nullptr, pos, item);
}

char Actor::unequipItem(Costume& item)
{
    return this->unequipItem(nullptr, item);
}

Costume* Actor::unequipPos(const char pos)
{
    assert(pos != CHAR_NONE && pos != CHAR_RACE && pos != CHAR_JOB);
    return this->equipItem(nullptr, pos, nullptr);
}

Costume* Actor::unequipPos(Scene* const scene, const char pos)
{
    return this->equipItem(scene, pos, nullptr);
}

char Actor::unequipItem(Scene* const scene, Costume& item)
{
    QMap<char, Costume*>& equipment = this->_equipment;
    char const old = equipment.key(&item, CHAR_NONE);
    this->equipItem(scene, old, nullptr);
    return old;
}

Costume* Actor::equipItem(Scene* const scene, const char pos, Costume* const item)
{
    QMap<char, Costume*>& equipment = this->_equipment;
    Costume* old = equipment.value(pos, nullptr);
    switchCostume(nullptr, scene, old, item);
    equipment[pos] = item;
    return old;
}

Actor& Actor::removeStates(QString* const ret, Scene* const scene, bool const remove)
{
    Actor& actor = *this;
    QMap<State*, int>* const stateDur = actor._state_dur;
    if (stateDur != nullptr)
    {
        actor.updateStates(true, ret, scene, *stateDur, true);
        if (remove && stateDur->size() == 0)
        {
            actor._state_dur = nullptr;
            delete stateDur;
        }
    }
    return actor;
}

inline Actor& Actor::setExtra(void* const extra)
{
    this->_extra = extra;
    return *this;
}

inline Actor& Actor::setRace(Costume& race)
{
    return this->setRace(nullptr, race);
}

inline Actor& Actor::setJob(Costume& job)
{
    return this->setJob(nullptr, job);
}

inline Actor& Actor::setRace(Scene* const scene, Costume& race)
{
    Actor& actor = *this;
    actor.equipItem(scene, CHAR_RACE, &race);
    return actor;
}

Actor& Actor::setJob(Scene* const scene, Costume& job)
{
    Actor& actor = *this;
    actor.equipItem(scene, CHAR_JOB, &job);
    if (!actor.isShapeShifted())
    {
        QString* spr = job._sprite;
        if (spr != nullptr)
        {
            QString* actorSpr = actor._sprite;
            if (actorSpr == nullptr)
            {
                actor._sprite = new QString(*spr);
            }
            else
            {
                (*actorSpr) = *spr;
            }
        }
    }
    return actor;
}

inline Actor& Actor::setAttack(const int atk)
{
    this->_atk = atk;
    return *this;
}

inline Actor& Actor::setDefense(const int def)
{
    this->_def = def;
    return *this;
}

inline Actor& Actor::setSpirit(const int spi)
{
    this->_spi = spi;
    return *this;
}

inline Actor& Actor::setWisdom(const int wis)
{
    this->_wis = wis;
    return *this;
}

inline Actor& Actor::setAgility(const int agi)
{
    this->_agi = agi;
    return *this;
}

Actor& Actor::setAgility(const int agi, Scene& scene)
{
    Actor& actor = this->setAgility(agi);
    scene.agiCalc();
    return actor;
}

Actor& Actor::setCurrentHp(const int hp, QString& ret, const bool survive)
{
    return this->setCurrentHp(hp, &ret, nullptr, survive);
}

Actor& Actor::setCurrentHp(const int hp, QString* const ret, Scene* const scene, bool const survive)
{
    Actor& actor = *this;
    if (hp < 1)
    {
        if (actor._hp != 0)
        {
            if (survive || actor.isInvincible())
            {
                actor._hp = 1;
            }
            else
            {
                actor._sp = 0;
                if (ret != nullptr)
                {
                    *ret = *ret % Actor::KoTxt.arg(actor._name);
                }
                if (actor._init > 0)
                {
                    actor._init = 0;
                }
                bool const revives = actor.isReviving();
                if (revives)
                {
                    if (ret != nullptr)
                    {
                        *ret = *ret % Actor::RiseTxt;
                    }
                    actor._hp = actor._m_hp;
                    if (scene != nullptr && actor.isShapeShifted())
                    {
                        Scene::SpriteRun* const actorEvent = scene->_actor_run;
                        if (actorEvent != nullptr)
                        {
                            ((*actorEvent)(*scene, &actor, nullptr, true, nullptr, nullptr));
                        }
                    }
                    actor.removeStates(ret, scene, false);
                }
                else
                {
                    actor._hp = 0;
                    actor.removeStates(ret, scene, false);
                    actor.setStunned(true);
                    actor.setKnockedOut(true);
                    if (scene != nullptr && ret != nullptr)
                    {
                        scene->checkStatus(*ret);
                    }
                }
            }
        }
    }
    else
    {
        int const oHp = actor._hp, mHp = actor._m_hp;
        actor._hp = hp > mHp ? mHp : hp;
        if (oHp < 1)
        {
            actor.setStunned(false);
            actor.setKnockedOut(false);
            actor.refreshCostumes(ret, scene);
            //actor.applyStates(ret, scene, false);
            if (scene != nullptr)
            {
                scene->resetTurn(actor);
            }
        }
    }
    return actor;
}

Actor& Actor::setCurrentHp(const int hp)
{
    int mHp = this->_m_hp;
    this->_hp = hp > mHp ? mHp : (hp < 1 ? 0 : hp);
    return *this;
}

Actor& Actor::setCurrentMp(const int mp)
{
    int mMp = this->_m_mp;
    this->_mp = mp > mMp ? mMp : (mp < 1 ? 0 : mp);
    return *this;
}

Actor& Actor::setCurrentRp(const int sp)
{
    int mSp = this->_m_sp;
    this->_sp = sp > mSp ? mSp : (sp < 1 ? 0 : sp);
    return *this;
}

Actor& Actor::setMaximumActions(const int mActions)
{
    Actor& actor = *this;
    actor._m_actions = mActions;
    if (mActions < actor._actions)
    {
        actor._actions = mActions;
    }
    return actor;
}

Actor& Actor::setMaximumHp(const int mHp)
{
    Actor& actor = *this;
    actor._m_hp = mHp;
    if (mHp < actor._hp)
    {
        actor._hp = mHp;
    }
    return actor;
}

Actor& Actor::setMaximumMp(const int mMp)
{
    Actor& actor = *this;
    actor._m_mp = mMp;
    if (mMp < actor._mp)
    {
        actor._mp = mMp;
    }
    return actor;
}

Actor& Actor::setMaximumRp(const int mRp)
{
    Actor& actor = *this;
    actor._m_sp = mRp;
    if (mRp < actor._sp)
    {
        actor._sp = mRp;
    }
    return actor;
}

inline Actor& Actor::setName(QString value)
{
    this->_name = value;
    return *this;
}

inline Actor& Actor::setSprite(QString& value)
{
    Actor& actor = *this;
    QString* sprite = actor._sprite;
    if (sprite != nullptr)
    {
        delete sprite;
    }
    actor._sprite = new QString(value);
    return *this;
}

Actor& Actor::setRanged(const bool range)
{
    this->setFlag(FLAG_RANGE, range);
    return *this;
}

Actor& Actor::setReviving(const bool revive)
{
    this->setFlag(FLAG_REVIVE, revive);
    return *this;
}

Actor& Actor::setEnraged(const bool automate)
{
    this->setFlag(FLAG_ENRAGED, automate);
    return *this;
}

Actor& Actor::setConfused(const bool confuse)
{
    this->setFlag(FLAG_CONFUSE, confuse);
    return *this;
}

Actor& Actor::setAiPlayer(const bool aiPlayer)
{
    this->setFlag(FLAG_AI_PLAYER, aiPlayer);
    return *this;
}

Actor& Actor::setRandomAi(const bool randomAi)
{
    this->setFlag(FLAG_RANDOM_AI, randomAi);
    return *this;
}

Actor& Actor::setNewItems(const bool newItems)
{
    this->setFlag(FLAG_NEW_ITEMS, newItems);
    return *this;
}

Actor& Actor::setReflecting(const bool reflect)
{
    this->setFlag(FLAG_REFLECT, reflect);
    return *this;
}

Actor& Actor::setShapeShifted(const bool shapeshift)
{
    this->setFlag(FLAG_SHAPE_SHIFT, shapeshift);
    return *this;
}

Actor& Actor::setKnockedOut(const bool ko)
{
    this->setFlag(FLAG_KO, ko);
    return *this;
}

Actor& Actor::setInvincible(const bool invincible)
{
    this->setFlag(FLAG_INVINCIBLE, invincible);
    return *this;
}

Actor& Actor::setStunned(const bool stun)
{
    int const flags = this->_flags;
    if (stun != ((flags & FLAG_STUN) == FLAG_STUN))
    {
        this->_flags = flags ^ FLAG_STUN;
    }
    return *this;
}

Actor& Actor::setItems(QMap<Ability*, int>* const items)
{
    if (this->hasNewItems())
    {
        this->setNewItems(false);
        QMap<Ability*, int>* oldItems = this->_items;
        if (oldItems != nullptr)
        {
            delete oldItems;
        }
    }
    this->_items = items;
    return *this;
}

inline Actor& Actor::setMaximumLevel(const int maxLv)
{
    this->_max_lv = maxLv;
    return *this;
}

inline Actor& Actor::setLevel(const int level)
{
    return this->setLevel(nullptr, level);
}

Actor& Actor::setLevel(Scene* const scene, const int level)
{
    Actor& actor = *this;
    while (level > actor._lv)
    {
        actor._xp = actor._maxp;
        actor.levelUp(scene);
    }
    actor._lv = level;
    return actor;
}

inline Actor& Actor::setExperience(const int xp)
{
    return this->setExperience(nullptr, xp);
}

Actor& Actor::setExperience(Scene* const scene, const int xp)
{
    Actor& actor = *this;
    actor._xp = xp;
    if (xp >= actor._maxp)
    {
        actor.levelUp(scene);
    }
    return actor;
}

Actor& Actor::setElementResistance(const int element, const int res)
{
    Actor& actor = *this;
    QMap<int, int>* rMap = actor._res;
    if (rMap == nullptr)
    {
        rMap = new QMap<int, int>();
        actor._res = rMap;
    }
    rMap->operator[](element) = res;
    return actor;
}

Actor& Actor::setStateResistance(State* const state, const int res)
{
    Actor& actor = *this;
    QMap<State*, int>* stRes = actor._st_res;
    if (stRes == nullptr)
    {
        stRes = new QMap<State*, int>();
        actor._st_res = stRes;
    }
    stRes->operator[](state) = res;
    return actor;
}

Actor& Actor::applyDmgRoles(QString& ret, Scene* const scene)
{
    Actor& actor = *this;
    QVector<Costume*>* const dmgRoles = actor._dmg_roles;
    if (dmgRoles != nullptr && dmgRoles->size() > 0)
    {
        for (Costume* const role : *dmgRoles)
        {
            role->apply(ret, scene, actor);
        }
        if (scene != nullptr)
        {
            Scene::SpriteRun* const actorEvent = scene->_actor_run;
            if (actorEvent == nullptr || ((*actorEvent)(*scene, &actor, nullptr, false, nullptr, nullptr)))
            {
                QVector<Actor*>* targets = scene->_targets;
                if (targets == nullptr)
                {
                    targets = new QVector<Actor*>(1);
                    scene->_targets = targets;
                }
                else
                {
                    targets->clear();
                }
                targets->append(this);
            }
        }
    }
    return actor;
}

Actor& Actor::applyStates(QString* const ret, Scene* const scene, const bool consume)
{
    Actor& actor = *this;
    if (consume && ret != nullptr)
    {
        actor.applyDmgRoles(*ret, scene);
    }
    QMap<State*, int>* const stateDur = actor._state_dur;
    if (stateDur != nullptr)
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
    return actor;
}

inline Actor& Actor::checkRegSkill(Ability& skill)
{
    Actor& actor = *this;
    if (skill._r_qty > 0)
    {
        QMap<Ability*, int>* regSkills = actor._skills_rg_turn;
        if (regSkills == nullptr)
        {
            regSkills = new QMap<Ability*, int>();
            actor._skills_rg_turn = regSkills;
        }
        regSkills->operator[](&skill) = 0;
    }
    return actor;
}

Actor& Actor::recover(QString& ret)
{
    return this->recover(&ret, nullptr);
}

Actor& Actor::recover(QString* const ret, Scene* const scene)
{
    Actor& actor = *this;
    actor.removeStates(ret, scene, true);
    actor.refreshCostumes(ret, scene);
    actor._actions = actor._m_actions;
    actor._hp = actor._m_hp;
    actor._mp = actor._m_mp;
    actor._sp = 0;
    {
        QMap<int, int>* const res = actor._res;
        if (res != nullptr)
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
                actor._res = nullptr;
                delete res;
            }
        }
    }
    {
        QMap<State*, int>* const stRes = actor._st_res;
        if (stRes != nullptr)
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
                actor._st_res = nullptr;
                delete stRes;
            }
        }
    }
    {
        QMap<Ability*, int>* skillsQty = actor._skills_cr_qty;
        if (skillsQty != nullptr)
        {
            auto const last = skillsQty->cend();
            for (auto it = skillsQty->cbegin(); it != last; ++it)
            {
                Ability* const ability = it.key();
                skillsQty->operator[](ability) = ability->_m_qty;
            }
        }
    }
    return actor;
}

Actor& Actor::levelUp(Scene* const scene)
{
    Actor& actor = *this;
    int lv = actor._lv;
    int const maxp = actor._maxp, xp = actor._xp, maxLv = actor._max_lv;
    while (maxp <= xp && lv < maxLv)
    {
        actor._maxp = maxp * 2;
        actor._m_hp += 3;
        actor._m_mp += 2;
        actor._m_sp += 2;
        actor._atk += 1;
        actor._def += 1;
        actor._wis += 1;
        actor._spi += 1;
        if (scene == nullptr)
        {
            actor._agi += 1;
        }
        else
        {
            actor.setAgility(actor._agi + 1, *scene);
        }
        lv += 1;
    }
    actor._lv = lv;
    return actor;
}

Actor& Actor::switchCostume(QString* const ret, Scene* const scene, Costume* const oldCost, Costume* const newCost)
{
    Actor& actor = *this;
    if (oldCost != nullptr)
    {
        oldCost->adopt(ret, scene, actor, true, true);
    }
    if (newCost != nullptr)
    {
        newCost->adopt(ret, scene, actor, true, false);
    }
    return actor;
}

Actor& Actor::updateAttributes(const bool remove, Scene* const scene, Costume& costume)
{
    Actor& actor = *this;
    int const i = remove ? -1 : 1;
    actor.setMaximumHp(actor._m_hp + (i * costume._m_hp));
    actor.setMaximumMp(actor._m_mp + (i * costume._m_mp));
    actor.setMaximumRp(actor._m_sp + (i * costume._m_sp));
    actor.setMaximumActions(actor._m_actions + (i * costume._m_actions));
    actor._atk += i * costume._atk;
    actor._def += i * costume._def;
    actor._spi += i * costume._spi;
    actor._wis += i * costume._wis;
    if (scene == nullptr)
    {
        actor._agi += i * costume._agi;
    }
    else
    {
        actor.setAgility(actor._agi + (i * costume._agi), *scene);
    }
    return actor;
}

Actor& Actor::updateResistance(const bool remove, QMap<int, int>* const elmRes, QMap<State *, int>* const stRes)
{
    Actor& actor = *this;
    if (elmRes != nullptr)
    {
        QMap<int, int>* aElmRes = actor._res;
        if (remove)
        {
            if (aElmRes != nullptr)
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
            if (aElmRes == nullptr)
            {
                aElmRes = new QMap<int, int>();
                actor._res = aElmRes;
            }
            auto const last = elmRes->cend();
            for (auto it = elmRes->cbegin(); it != last; ++it)
            {
                int const i = it.key(), v = it.value();
                aElmRes->operator[](i) = aElmRes->value(i, 0) + v;
            }
        }
    }
    if (stRes != nullptr)
    {
        QMap<State*, int>* aStateRes = actor._st_res;
        if (remove)
        {
            if (aStateRes != nullptr)
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
            if (aStateRes == nullptr)
            {
                aStateRes = new QMap<State*, int>();
                actor._st_res = aStateRes;
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
    return actor;
}

Actor& Actor::updateSkills(const bool remove, const bool counters, QVector<Ability*>& skills)
{
    Actor& actor = *this;
    QVector<Ability*>* aSkills = counters ? actor._counters : actor._a_skills;
    if (remove)
    {
        if (aSkills != nullptr)
        {
            for (Ability* const ability : skills)
            {
                aSkills->removeOne(ability);
                if (ability->_r_qty > 0)
                {
                    QMap<Ability*, int>* regTurn = actor._skills_rg_turn;
                    if (regTurn != nullptr)
                    {
                        regTurn->remove(ability);
                    }
                }
                if (ability->_m_qty > 0)
                {
                    QMap<Ability*, int>* crQty = actor._skills_cr_qty;
                    if (crQty != nullptr)
                    {
                        crQty->remove(ability);
                    }
                }
            }
        }
    }
    else
    {
        if (aSkills == nullptr)
        {
            aSkills = new QVector<Ability*>();
            if (counters)
            {
                this->_counters = aSkills;
            }
            else
            {
                this->_a_skills = aSkills;
            }
        }
        for (Ability* const ability : skills)
        {
            if (!aSkills->contains(ability))
            {
                aSkills->append(ability);
                int const mQty = ability->_m_qty;
                if (mQty > 0)
                {
                    QMap<Ability*, int>* crQty = actor._skills_cr_qty;
                    if (crQty == nullptr)
                    {
                        crQty = new QMap<Ability*, int>();
                        actor._skills_cr_qty = crQty;
                    }
                    crQty->operator[](ability) = mQty;
                    actor.checkRegSkill(*ability);
                }
            }
        }
    }
    return actor;
}

Actor& Actor::updateStates(bool const remove, QString* const ret, Scene* const scene,
                           QMap<State*, int>& states, bool const includeWithDur)
{
    Actor& actor = *this;
    if (remove)
    {
        QMap<State*, int>* const stateDur = actor._state_dur;
        if (stateDur != nullptr && stateDur->size() > 0)
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
                it.key()->inflict(ret, scene, nullptr, actor, rDur, true);
            }
        }
    }
    return actor;
}

Actor& Actor::refreshCostumes(QString* const ret, Scene* const scene)
{
    Actor& actor = *this;
    {
        QMap<char, Costume*>& equipment = actor._equipment;
        auto const last = equipment.cend();
        for (auto it = equipment.cbegin(); it != last; ++it)
        {
            it.value()->refresh(ret, scene, actor, true, false);
        }
    }
    QMap<State*, int>* const stateDur = actor._state_dur;
    if (stateDur != nullptr)
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
    return actor;
}

Actor::Actor(int const id, QString name, QString sprite, Costume& race, Costume& job, int const level, int const maxLv, int const mActions,
             int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
             QMap<int, int>* const res, QMap<State*, int>* const stRes, QMap<Ability*, int>* const items)
    : Costume(id, name, sprite, false, mActions, 0, mHp, mMp, 0, mHp, mMp, mSp, atk, def, spi, wis, agi, false, false, false, false, false, false, false, false, new QVector<Ability*>(),
              nullptr, nullptr, stRes, res)
{
    this->_lv = 1;
    this->_xp = 0;
    this->_maxp = 15;
    this->_side = 0;
    this->_init = 0;
    this->_side = 0;
    this->_old_side = 0;
    this->_dmg_roles = nullptr;
    this->_skills_rg_turn = nullptr;
    this->_skills_cr_qty = nullptr;
    this->_state_dur = nullptr;
    this->_actions = mActions;
    this->_max_lv = maxLv;
    this->setRace(race);
    this->setJob(job);
    this->_items = items;
    this->setLevel(level);
    this->recover(nullptr, nullptr);
    this->_extra = nullptr;
}

Actor::Actor(Actor& actor) : Costume(actor)
{
    this->_init = 0;//actor.init;
    this->_side = 0;//actor.side;
    this->_old_side = 0;
    /*this->setJob(actor.getJob());
    this->setRace(actor.getRace());
    this->setExperience(actor.xp);*/
    QVector<Ability*>* aSkills = new QVector<Ability*>();
    this->_items = actor.hasNewItems() ? new QMap<Ability*, int>(*(actor._items)) : actor._items;
    *(aSkills) = *(actor._a_skills);
    this->_a_skills = aSkills;
    this->_maxp = actor._maxp;
    this->_xp = actor._xp;
    this->_lv = actor._lv;
    this->_max_lv = actor._lv;
    this->_equipment = actor._equipment;
    {
        QMap<Ability*, int>* crSkillsQty = actor._skills_cr_qty;
        if (crSkillsQty == nullptr)
        {
            this->_skills_cr_qty = nullptr;
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
        if (skillsRgTurn == nullptr)
        {
            this->_skills_rg_turn = nullptr;
        }
        else
        {
            QMap<Ability*, int>* nSkillsRgTurn = new QMap<Ability*, int>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            this->_skills_rg_turn = nSkillsRgTurn;
        }
    }
    {
        QMap<State*, int>* crSkillsQty = actor._state_dur;
        if (crSkillsQty == nullptr)
        {
            this->_state_dur = nullptr;
        }
        else
        {
            QMap<State*, int>* nSkillsQty = new QMap<State*, int>();
            (*nSkillsQty) = (*crSkillsQty);
            this->_state_dur = nSkillsQty;
        }
    }
    {
        QVector<Costume*>* skillsRgTurn = actor._dmg_roles;
        if (skillsRgTurn == nullptr)
        {
            this->_dmg_roles = nullptr;
        }
        else
        {
            QVector<Costume*>* nSkillsRgTurn = new QVector<Costume*>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            this->_dmg_roles = nSkillsRgTurn;
        }
    }
    {
        QVector<Ability*>* skillsRgTurn = actor._a_skills;
        if (skillsRgTurn == nullptr)
        {
            this->_a_skills = nullptr;
        }
        else
        {
            QVector<Ability*>* nSkillsRgTurn = new QVector<Ability*>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            this->_a_skills = nSkillsRgTurn;
        }
    }
    {
        QVector<Ability*>* skillsRgTurn = actor._counters;
        if (skillsRgTurn == nullptr)
        {
            this->_counters = nullptr;
        }
        else
        {
            QVector<Ability*>* nSkillsRgTurn = new QVector<Ability*>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            this->_counters = nSkillsRgTurn;
        }
    }
    this->_extra = actor._extra;
}

Actor::~Actor()
{
    auto stRes = this->_st_res;
    if (stRes != nullptr)
    {
        this->_st_res = nullptr;
        delete stRes;
    }
    auto res = this->_res;
    if (res != nullptr)
    {
        this->_res = nullptr;
        delete res;
    }
    auto states = this->_state_dur;
    if (states != nullptr)
    {
        this->_state_dur = nullptr;
        delete states;
    }
    auto skills = this->_a_skills;
    if (skills != nullptr)
    {
        this->_a_skills = nullptr;
        delete skills;
    }
    auto dmgRoles = this->_dmg_roles;
    if (dmgRoles != nullptr)
    {
        this->_dmg_roles = nullptr;
        delete dmgRoles;
    }
    if (this->hasNewItems())
    {
        //this->setNewItems(false);
        QMap<Ability*, int>* items = this->_items;
        if (items != nullptr)
        {
            this->_items = nullptr;
            delete items;
        }
    }
}
