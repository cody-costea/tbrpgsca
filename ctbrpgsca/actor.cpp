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
    return this->lv;
}

int Actor::getExperience() const
{
    return this->xp;
}

int Actor::getMaximumLevel() const
{
    return this->maxLv;
}

int Actor::getMaximumExperience() const
{
    return this->maxp;
}

int Actor::getCurrentActions() const
{
    return this->actions;
}

int Actor::getInitiative() const
{
    return this->init;
}

int Actor::getRemainingSkillUses(Ability& skill) const
{
    QMap<Ability*, int>* crQty = this->skillsCrQty;
    return crQty == nullptr ? 0 : crQty->value(&skill, 0);
}

int Actor::getRegeneratingSkillTurn(Ability& skill) const
{
    QMap<Ability*, int>* regTurn = this->skillsRgTurn;
    return regTurn == nullptr ? 0 : regTurn->value(&skill, 0);
}

Costume& Actor::getRace() const
{
    return *(this->equipment[CHAR_RACE]);
}

Costume& Actor::getJob() const
{
    return *(this->equipment[CHAR_JOB]);
}

void* Actor::getExtra() const
{
    return this->extra;
}

bool Actor::isAiPlayer() const
{
    return (this->flags & FLAG_AI_PLAYER) == FLAG_AI_PLAYER;
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

Costume* Actor::unequipPos(Scene* scene, const char pos)
{
    return this->equipItem(scene, pos, nullptr);
}

char Actor::unequipItem(Scene* const scene, Costume& item)
{
    QMap<char, Costume*>& equipment = this->equipment;
    char const old = equipment.key(&item, CHAR_NONE);
    this->equipItem(scene, old, nullptr);
    return old;
}

Costume* Actor::equipItem(Scene* const scene, const char pos, Costume* const item)
{
    QMap<char, Costume*>& equipment = this->equipment;
    Costume* old = equipment.value(pos, nullptr);
    switchCostume(nullptr, scene, old, item);
    equipment[pos] = item;
    return old;
}

Actor& Actor::setExtra(void* const extra)
{
    this->extra = extra;
    return *this;
}

Actor& Actor::setRace(Costume& race)
{
    return this->setRace(nullptr, race);
}

Actor& Actor::setJob(Costume& job)
{
    return this->setJob(nullptr, job);
}

Actor& Actor::setRace(Scene* const scene, Costume& race)
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
        QString* spr = job.sprite;
        if (spr != nullptr)
        {
            QString* actorSpr = actor.sprite;
            if (actorSpr == nullptr)
            {
                actor.sprite = new QString(*spr);
            }
            else
            {
                (*actorSpr) = *spr;
            }
        }
    }
    return actor;
}

Actor& Actor::setAttack(const int atk)
{
    this->atk = atk;
    return *this;
}

Actor& Actor::setDefense(const int def)
{
    this->def = def;
    return *this;
}

Actor& Actor::setSpirit(const int spi)
{
    this->spi = spi;
    return *this;
}

Actor& Actor::setWisdom(const int wis)
{
    this->wis = wis;
    return *this;
}

Actor& Actor::setAgility(const int agi)
{
    this->agi = agi;
    return *this;
}

Actor& Actor::setAgility(const int agi, Scene& scene)
{
    Actor& actor = this->setAgility(agi);
    scene.agiCalc();
    return actor;
}

Actor& Actor::setCurrentHp(const int hp, QString &ret, const bool survive)
{
    return this->setCurrentHp(hp, ret, nullptr, survive);
}

Actor& Actor::setCurrentHp(const int hp, QString& ret, Scene* scene, bool const survive)
{
    Actor& actor = *this;
    if (hp < 1)
    {
        if (actor.hp != 0)
        {
            if (survive || actor.isInvincible())
            {
                actor.hp = 1;
            }
            else
            {
                actor.hp = 0;
                bool const revives = actor.isReviving();
                ret = ret % Actor::KoTxt.arg(actor.name);
                //actor.actions = 0;
                /*if (actor.isShapeShifted())
                {
                    actor.setShapeShifted(false);
                    (*actor.sprite) = (*actor.getJob().sprite);
                }*/
                actor.sp = 0;
                {
                    QMap<State*, int>* stateDur = actor.stateDur;
                    if (stateDur != nullptr)
                    {
                        auto const last = stateDur->cend();
                        for (auto it = stateDur->cbegin(); it != last; ++it)
                        {
                            it.key()->disable(ret, scene, actor, STATE_END_DUR + 1, false);
                        }
                    }
                }
                if (revives)
                {
                    ret = ret % Actor::RiseTxt;
                    actor.hp = actor.mHp;
                }
                else
                {
                    actor.setStunned(true);
                    actor.setKnockedOut(true);
                    if (scene != nullptr)
                    {
                        scene->checkStatus(ret);
                    }
                }
            }
        }
    }
    else
    {
        int const oHp = actor.hp, mHp = actor.mHp;
        actor.hp = hp > mHp ? mHp : hp;
        if (oHp < 1)
        {
            actor.setStunned(false);
            actor.setKnockedOut(false);
            actor.refreshCostumes(&ret, scene);
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
    int mHp = this->mHp;
    this->hp = hp > mHp ? mHp : (hp < 0 ? 0 : hp);
    return *this;
}

Actor& Actor::setCurrentMp(const int mp)
{
    int mMp = this->mMp;
    this->mp = mp > mMp ? mMp : (mp < 0 ? 0 : mp);
    return *this;
}

Actor& Actor::setCurrentRp(const int sp)
{
    int mSp = this->mSp;
    this->sp = sp > mSp ? mSp : (sp < 0 ? 0 : sp);
    return *this;
}

Actor& Actor::setMaximumActions(const int mActions)
{
    Actor& actor = *this;
    actor.mActions = mActions;
    if (mActions < actor.actions)
    {
        actor.actions = mActions;
    }
    return actor;
}

Actor& Actor::setMaximumHp(const int mHp)
{
    Actor& actor = *this;
    actor.mHp = mHp;
    if (mHp < actor.hp)
    {
        actor.hp = mHp;
    }
    return actor;
}

Actor& Actor::setMaximumMp(const int mMp)
{
    Actor& actor = *this;
    actor.mMp = mMp;
    if (mMp < actor.mp)
    {
        actor.mp = mMp;
    }
    return actor;
}

Actor& Actor::setMaximumRp(const int mRp)
{
    Actor& actor = *this;
    actor.mSp = mRp;
    if (mRp < actor.sp)
    {
        actor.sp = mRp;
    }
    return actor;
}

Actor& Actor::setName(QString value)
{
    this->name = value;
    return *this;
}

Actor& Actor::setSprite(QString& value)
{
    Actor& actor = *this;
    QString* sprite = actor.sprite;
    if (sprite != nullptr)
    {
        delete sprite;
    }
    actor.sprite = new QString(value);
    return *this;
}

Actor& Actor::setRanged(const bool range)
{
    int const flags = this->flags;
    if (range != ((flags & FLAG_RANGE) == FLAG_RANGE))
    {
        this->flags = flags ^ FLAG_RANGE;
    }
    return *this;
}

Actor& Actor::setReviving(const bool revive)
{
    int const flags = this->flags;
    if (revive != ((flags & FLAG_REVIVE) == FLAG_REVIVE))
    {
        this->flags = flags ^ FLAG_REVIVE;
    }
    return *this;
}

Actor& Actor::setEnraged(const bool automate)
{
    int const flags = this->flags;
    if (automate != ((flags & FLAG_ENRAGED) == FLAG_ENRAGED))
    {
        this->flags = flags ^ FLAG_ENRAGED;
    }
    return *this;
}

Actor& Actor::setConfused(const bool confuse)
{
    int const flags = this->flags;
    if (confuse != ((flags & FLAG_CONFUSE) == FLAG_CONFUSE))
    {
        this->flags = flags ^ FLAG_CONFUSE;
    }
    return *this;
}

Actor& Actor::setAiPlayer(const bool aiPlayer)
{
    int const flags = this->flags;
    if (aiPlayer != ((flags & FLAG_AI_PLAYER) == FLAG_AI_PLAYER))
    {
        this->flags = flags ^ FLAG_AI_PLAYER;
    }
    return *this;
}

Actor& Actor::setReflecting(const bool reflect)
{
    int const flags = this->flags;
    if (reflect != ((flags & FLAG_REFLECT) == FLAG_REFLECT))
    {
        this->flags = flags ^ FLAG_REFLECT;
    }
    return *this;
}

Actor& Actor::setShapeShifted(const bool shapeshift)
{
    int const flags = this->flags;
    if (shapeshift != ((flags & FLAG_SHAPE_SHIFT) == FLAG_SHAPE_SHIFT))
    {
        this->flags = flags ^ FLAG_SHAPE_SHIFT;
    }
    return *this;
}

Actor& Actor::setKnockedOut(const bool ko)
{
    int const flags = this->flags;
    if (ko != ((flags & FLAG_KO) == FLAG_KO))
    {
        this->flags = flags ^ FLAG_KO;
    }
    return *this;
}

Actor& Actor::setInvincible(const bool invincible)
{
    int const flags = this->flags;
    if (invincible != ((flags & FLAG_INVINCIBLE) == FLAG_INVINCIBLE))
    {
        this->flags = flags ^ FLAG_INVINCIBLE;
    }
    return *this;
}

Actor& Actor::setStunned(const bool stun)
{
    int const flags = this->flags;
    if (stun != ((flags & FLAG_STUN) == FLAG_STUN))
    {
        this->flags = flags ^ FLAG_STUN;
    }
    return *this;
}

Actor& Actor::setMaximumLevel(const int maxLv)
{
    this->maxLv = maxLv;
    return *this;
}

Actor& Actor::setLevel(const int level)
{
    return this->setLevel(nullptr, level);
}

Actor& Actor::setLevel(Scene* const scene, const int level)
{
    Actor& actor = *this;
    while (level > actor.lv)
    {
        actor.xp = actor.maxp;
        actor.levelUp(scene);
    }
    actor.lv = level;
    return actor;
}

Actor& Actor::setExperience(const int xp)
{
    return this->setExperience(nullptr, xp);
}

Actor& Actor::setExperience(Scene* const scene, const int xp)
{
    Actor& actor = *this;
    actor.xp = xp;
    actor.levelUp(scene);
    return actor;
}

Actor& Actor::setElementResistance(const int element, const int res)
{
    Actor& actor = *this;
    QMap<int, int>* rMap = actor.res;
    if (rMap == nullptr)
    {
        rMap = new QMap<int, int>();
        actor.res = rMap;
    }
    rMap->operator[](element) = res;
    return actor;
}

Actor& Actor::setStateResistance(State* const state, const int res)
{
    Actor& actor = *this;
    QMap<State*, int>* stRes = actor.stRes;
    if (stRes == nullptr)
    {
        stRes = new QMap<State*, int>();
        actor.stRes = stRes;
    }
    stRes->operator[](state) = res;
    return actor;
}

Actor& Actor::applyDmgRoles(QString& ret, Scene* const scene)
{
    Actor& actor = *this;
    QVector<Costume*>* dmgRoles = actor.dmgRoles;
    if (dmgRoles != nullptr)
    {
        for (Costume* const role : *dmgRoles)
        {
            role->apply(ret, scene, actor);
        }
    }
    return actor;
}

Actor& Actor::applyStates(QString& ret, Scene* const scene, const bool consume)
{
    Actor& actor = *this;
    if (consume)
    {
        actor.applyDmgRoles(ret, scene);
    }
    QMap<State*, int>* stateDur = actor.stateDur;
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

Actor& Actor::checkRegSkill(Ability& skill)
{
    Actor& actor = *this;
    if (skill.rQty > 0)
    {
        QMap<Ability*, int>* regSkills = actor.skillsRgTurn;
        if (regSkills == nullptr)
        {
            regSkills = new QMap<Ability*, int>();
            actor.skillsRgTurn = regSkills;
        }
        regSkills->operator[](&skill) = 0;
    }
    return actor;
}

Actor& Actor::recover(QString& ret)
{
    return this->recover(&ret, nullptr);
}

Actor& Actor::recover(QString* ret, Scene* const scene)
{
    Actor& actor = *this;
    actor.actions = actor.mActions;
    actor.hp = actor.mHp;
    actor.mp = actor.mMp;
    actor.sp = 0;
    if (ret != nullptr)
    {
        QMap<State*, int>* const sDur = actor.stateDur;
        if (sDur != nullptr)
        {
            {
                auto const last = stateDur->cend();
                for (auto it = stateDur->cbegin(); it != last; ++it)
                {
                    it.key()->disable(*ret, scene, actor, STATE_END_DUR + 1, false);
                }
            }
            if (sDur->size() == 0)
            {
                actor.stateDur = nullptr;
                delete sDur;
            }
        }
    }
    //actor.setShapeShifted(false);
    actor.refreshCostumes(ret, scene);
    {
        QMap<int, int>* const res = actor.res;
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
                actor.res = nullptr;
                delete res;
            }
        }
    }
    {
        QMap<State*, int>* const stRes = actor.stRes;
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
                actor.stRes = nullptr;
                delete stRes;
            }
        }
    }
    {
        QMap<Ability*, int>* skillsQty = actor.skillsCrQty;
        if (skillsQty != nullptr)
        {
            auto const last = skillsQty->cend();
            for (auto it = skillsQty->cbegin(); it != last; ++it)
            {
                Ability* const ability = it.key();
                skillsQty->operator[](ability) = ability->mQty;
            }
        }
    }
    return actor;
}

Actor& Actor::levelUp(Scene* const scene)
{
    Actor& actor = *this;
    //int const i = remove ? -1 : 1;
    while (actor.maxp <= actor.xp && actor.lv < actor.maxLv)
    {
        actor.maxp *= 2;
        ++(actor.lv);
        actor.mHp += 3;
        actor.mMp += 2;
        actor.mSp += 2;
        ++(actor.atk);
        ++(actor.def);
        ++(actor.wis);
        ++(actor.spi);
        if (scene == nullptr)
        {
            ++(actor.agi);
        }
        else
        {
            actor.setAgility(actor.agi + 1, *scene);
        }
    }
    return actor;
}

Actor& Actor::switchCostume(QString* ret, Scene* const scene, Costume* const oldCost, Costume* const newCost)
{
    Actor& actor = *this;
    if (oldCost != nullptr)
    {
        oldCost->abandon(ret, scene, actor, true);
    }
    if (newCost != nullptr)
    {
        newCost->adopt(ret, scene, actor, true);
    }
    return actor;
}

Actor& Actor::updateAttributes(const bool remove, Scene* const scene, Costume& costume)
{
    Actor& actor = *this;
    int i = remove ? -1 : 1;
    actor.setMaximumHp(actor.mHp + (i * costume.mHp));
    actor.setMaximumMp(actor.mMp + (i * costume.mMp));
    actor.setMaximumRp(actor.mSp + (i * costume.mSp));
    actor.setMaximumActions(actor.mActions + (i * costume.mActions));
    actor.atk += i * costume.atk;
    actor.def += i * costume.def;
    actor.mSp += i * costume.spi;
    actor.wis += i * costume.wis;
    if (scene == nullptr)
    {
        actor.agi += i * costume.agi;
    }
    else
    {
        actor.setAgility(actor.agi + (i * costume.agi), *scene);
    }
    return actor;
}

Actor& Actor::updateResistance(const bool remove, QMap<int, int>* const elmRes, QMap<State *, int>* const stRes)
{
    Actor& actor = *this;
    if (elmRes != nullptr)
    {
        QMap<int, int>* aElmRes = actor.res;
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
                actor.res = aElmRes;
            }
            auto const last = elmRes->cend();
            for (auto it = elmRes->cbegin(); it != last; ++it)
            {
                int const i = it.key(), v = it.value();
                aElmRes->operator[](i) = aElmRes->value(i, v) + v;
            }
        }
    }
    if (stRes != nullptr)
    {
        QMap<State*, int>* aStateRes = actor.stRes;
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
                actor.stRes = aStateRes;
            }
            auto const last = stRes->cend();
            for (auto it = stRes->cbegin(); it != last; ++it)
            {
                int v = it.value();
                State* const i = it.key();
                aStateRes->operator[](i) = aStateRes->value(i, v) + v;
            }
        }
    }
    return actor;
}

Actor& Actor::updateSkills(const bool remove, const bool counters, QVector<Ability*>& skills)
{
    Actor& actor = *this;
    QVector<Ability*>* aSkills = counters ? actor.counters : actor.aSkills;
    if (remove)
    {
        if (aSkills != nullptr)
        {
            for (Ability* const ability : skills)
            {
                aSkills->removeOne(ability);
                if (ability->rQty > 0)
                {
                    QMap<Ability*, int>* regTurn = actor.skillsRgTurn;
                    if (regTurn != nullptr)
                    {
                        regTurn->remove(ability);
                    }
                }
                if (ability->mQty > 0)
                {
                    QMap<Ability*, int>* crQty = actor.skillsCrQty;
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
                this->counters = aSkills;
            }
            else
            {
                this->aSkills = aSkills;
            }
        }
        for (Ability* const ability : skills)
        {
            if (!aSkills->contains(ability))
            {
                aSkills->append(ability);
                /*int const mQty = ability->mQty;
                if (mQty > 0)
                {
                    QMap<Ability*, int>* crQty = actor.skillsCrQty;
                    if (crQty == nullptr)
                    {
                        crQty = new QMap<Ability*, int>();
                        actor.skillsCrQty = crQty;
                    }
                    crQty->operator[](ability) = mQty;
                    actor.checkRegSkill(*ability);
                }*/
            }
        }
    }
    return actor;
}

Actor& Actor::updateStates(bool const remove, QString& ret, Scene* const scene, QMap<State*, int>& states)
{
    Actor& actor = *this;
    if (remove)
    {
        QMap<State*, int>* stateDur = actor.stateDur;
        if (stateDur != nullptr)
        {
            auto const last = states.cend();
            for (auto it = states.cbegin(); it != last; ++it)
            {
                //if (it.value() == -1)
                //{
                    it.key()->disable(ret, scene, actor, it.value(), false);
                //}
            }
        }
    }
    else
    {
        auto const last = states.cend();
        for (auto it = states.cbegin(); it != last; ++it)
        {
            int const dur = it.value();
            if (dur != -1)
            {
                it.key()->inflict(ret, scene, nullptr, actor, dur, true);
            }
        }
    }
    return actor;
}

Actor& Actor::refreshCostume(QString* ret, Scene* scene, Costume& costume)
{
    Actor& actor = *this;
    actor.dmgType |= costume.dmgType;
    int const cFlags = costume.flags;
    actor.flags |= cFlags;
    {
        QVector<Ability*>* skills = costume.aSkills;
        if (skills != nullptr)
        {
            actor.updateSkills(false, false, *skills);
        }
        skills = costume.counters;
        if (skills != nullptr)
        {
            actor.updateSkills(false, true, *skills);
        }
    }
    if ((cFlags & FLAG_KO) == FLAG_KO)
    {
        if (ret == nullptr)
        {
            QString tmp = QString();
            scene->checkStatus(tmp);
        }
        else
        {
            scene->checkStatus(*ret);
        }
    }
    if (costume.isShapeShifted())
    {
        QString* spr = costume.sprite;
        if (spr != nullptr)
        {
            (*actor.sprite) = *spr;
        }
    }
    if (ret != nullptr)
    {
        QMap<State*, int>* cStates = costume.stateDur;
        if (cStates != nullptr)
        {
            auto const last = cStates->cend();
            for (auto it = cStates->cbegin(); it != last; ++it)
            {
                int const rDur = it.value();
                if (rDur < 0 && rDur > STATE_END_DUR)
                {
                    it.key()->inflict(*ret, scene, nullptr, actor, rDur, true);
                }
            }
        }
    }
    return actor;
}

Actor& Actor::refreshCostumes(QString* ret, Scene* scene)
{
    Actor& actor = *this;
    {
        QMap<char, Costume*>& equipment = actor.equipment;
        auto const last = equipment.cend();
        for (auto it = equipment.cbegin(); it != last; ++it)
        {
            actor.refreshCostume(ret, scene, *(it.value()));
        }
    }
    QMap<State*, int>* stateDur = actor.stateDur;
    if (stateDur != nullptr)
    {
        auto const last = stateDur->cend();
        for (auto it = stateDur->cbegin(); it != last; ++it)
        {
            if (it.value() > STATE_END_DUR)
            {
                actor.refreshCostume(ret, scene, *(it.key()));
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
    this->lv = 1;
    this->xp = 0;
    this->maxp = 15;
    this->side = 0;
    this->init = 0;
    this->side = 0;
    this->oldSide = 0;
    this->actions = mActions;
    this->maxLv = maxLv;
    this->setRace(race);
    this->setJob(job);
    this->setLevel(level);
    this->items = items;
    this->dmgRoles = nullptr;
    this->skillsRgTurn = nullptr;
    this->skillsCrQty = nullptr;
    this->stateDur = nullptr;
    this->recover(nullptr, nullptr);
}

Actor::Actor(Actor& actor) : Costume(actor)
{
    this->init = 0;//actor.init;
    this->side = 0;//actor.side;
    this->oldSide = 0;
    /*this->setJob(actor.getJob());
    this->setRace(actor.getRace());
    this->setExperience(actor.xp);*/
    QVector<Ability*>* aSkills = new QVector<Ability*>();
    *(aSkills) = *(actor.aSkills);
    this->aSkills = aSkills;
    this->maxp = actor.maxp;
    this->xp = actor.xp;
    this->lv = actor.lv;
    this->maxLv = actor.lv;
    this->items = actor.items;
    this->equipment = actor.equipment;
    {
        QMap<Ability*, int>* crSkillsQty = actor.skillsCrQty;
        if (crSkillsQty == nullptr)
        {
            this->skillsCrQty = nullptr;
        }
        else
        {
            QMap<Ability*, int>* nSkillsQty = new QMap<Ability*, int>();
            (*nSkillsQty) = (*crSkillsQty);
            this->skillsCrQty = nSkillsQty;
        }

    }
    {
        QMap<Ability*, int>* skillsRgTurn = actor.skillsRgTurn;
        if (skillsRgTurn == nullptr)
        {
            this->skillsRgTurn = nullptr;
        }
        else
        {
            QMap<Ability*, int>* nSkillsRgTurn = new QMap<Ability*, int>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            this->skillsRgTurn = nSkillsRgTurn;
        }
    }
    {
        QMap<State*, int>* crSkillsQty = actor.stateDur;
        if (crSkillsQty == nullptr)
        {
            this->stateDur = nullptr;
        }
        else
        {
            QMap<State*, int>* nSkillsQty = new QMap<State*, int>();
            (*nSkillsQty) = (*crSkillsQty);
            this->stateDur = nSkillsQty;
        }
    }
    {
        QVector<Costume*>* skillsRgTurn = actor.dmgRoles;
        if (skillsRgTurn == nullptr)
        {
            this->dmgRoles = nullptr;
        }
        else
        {
            QVector<Costume*>* nSkillsRgTurn = new QVector<Costume*>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            this->dmgRoles = nSkillsRgTurn;
        }
    }
    {
        QVector<Ability*>* skillsRgTurn = actor.aSkills;
        if (skillsRgTurn == nullptr)
        {
            this->aSkills = nullptr;
        }
        else
        {
            QVector<Ability*>* nSkillsRgTurn = new QVector<Ability*>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            this->aSkills = nSkillsRgTurn;
        }
    }
    {
        QVector<Ability*>* skillsRgTurn = actor.counters;
        if (skillsRgTurn == nullptr)
        {
            this->counters = nullptr;
        }
        else
        {
            QVector<Ability*>* nSkillsRgTurn = new QVector<Ability*>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            this->counters = nSkillsRgTurn;
        }
    }
}

Actor::~Actor()
{
    auto stRes = this->stRes;
    if (stRes != nullptr)
    {
        this->stRes = nullptr;
        delete stRes;
    }
    auto res = this->res;
    if (res != nullptr)
    {
        this->res = nullptr;
        delete res;
    }
    auto states = this->stateDur;
    if (states != nullptr)
    {
        this->stateDur = nullptr;
        delete states;
    }
    auto skills = this->aSkills;
    if (skills != nullptr)
    {
        this->aSkills = nullptr;
        delete skills;
    }
    auto dmgRoles = this->dmgRoles;
    if (dmgRoles != nullptr)
    {
        this->dmgRoles = nullptr;
        delete dmgRoles;
    }
}
