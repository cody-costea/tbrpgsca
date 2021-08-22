/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "actor.hpp"
#include "costume.hpp"
#include "ability.hpp"
#include "state.hpp"
#include "scene.hpp"
#include "role.hpp"
#include "arenawidget.hpp"

#include <QStringBuilder>
#include <QString>

using namespace tbrpgsca;

const CmpsVct<const Ability, uint32_t, 2U> Actor::_def_skills
{
    Ability(1, TR_TXT_SKILL_ATTACK, "", "", false, false, false, true, 0, 0,0,0, DMG_TYPE_ATK, 10,0,-3, FLAG_TRG_ONE,0, 0,0, false, false, nullptr, &(Play::StateMasks()[10])),
    Ability(2, TR_TXT_SKILL_DEFEND, "", "", false, false, false, false, 0, 0,0,0, DMG_TYPE_DEF, 0,-1,-2, FLAG_TRG_SELF,0, 0,0, false, false, nullptr, nullptr)
};

int Actor::remainingSkillUses(const Ability& skill) const
{
    QMap<const Ability*, int>* crQty = this->_skills_cr_qty;
    return crQty ? crQty->value(&skill, 0) : 0;
}

int Actor::regeneratingSkillTurn(const Ability& skill) const
{
    QMap<const Ability*, int>* regTurn = this->_skills_rg_turn;
    return regTurn ? regTurn->value(&skill, 0) : 0;
}

QMap<const Ability*, int> Actor::items() const
{
    QMap<const Ability*, int>* items = this->_items;
    return items ? *items : QMap<const Ability*, int>();
}

const Costume& Actor::race() const
{
    return *(this->_equipment[CHAR_RACE]);
}

const Costume& Actor::job() const
{
    return *(this->_equipment[CHAR_JOB]);
}

uint32_t Actor::skillsCount() const
{
    uint32_t len = _def_skills.size();
    auto equipment = this->_equipment;
    for (uint32_t i = 0U; i < Actor::EquipPos::COUNT; i += 1U)
    {
        auto skills = equipment[i].ptr()->_a_skills;
        auto size = skills.size();
        for (uint32_t j = 0U; j < size; j += 1U)
        {
            len += skills[j].size();
        }
    }
    return len;
}

const Ability* Actor::skill(uint32_t idx) const
{
    uint32_t len = _def_skills.size();
    if (idx < len)
    {
        return &_def_skills[idx];
    }
    auto equipment = this->_equipment;
    for (uint32_t i = 0U; i < Actor::EquipPos::COUNT; i += 1U)
    {
        auto skillVct = equipment[i].ptr()->_a_skills;
        if (skillVct)
        {
            uint32_t vctSize = skillVct.size();
            for (uint32_t j = 0U; j < vctSize; j += 1U)
            {
                auto skills = skillVct[j];
                uint32_t cnt = len + skills.size();
                if (idx < cnt)
                {
                    return &skills.at(idx - len);
                }
                else
                {
                    len = cnt;
                }
            }
        }
    }
    return nullptr;
}

Actor::SkillSearch Actor::aiSkill(uint32_t const defSkill, bool const restore) const
{
    /*const Ability* s = skills[defSkill];
    int ret = defSkill, sSize = skills.size();
    for (int i = defSkill + 1; i < sSize; i += 1)
    {
        const Ability* a = skills.at(i);
        if (a->canPerform(user) && ((defSkill > 0 && (a->_hp < s->_hp)
            && ((restore && a->isReviving()) || !(restore || a->targetsOnlyKO())))
            || (defSkill == 0 && a->_hp > s->_hp)))
        {
            ret = i;
            s = a;
        }
    }
    return ret;*/
    return this->skill<false>(defSkill, [restore, defSkill, this](const Ability& a, const Ability* s) -> bool
    {
        return s == nullptr || (a.canPerform(*this) && ((defSkill > 0 && (a._hp < s->_hp)
            && ((restore && a.isReviving()) || !(restore || a.targetsOnlyKO())))
            || (defSkill == 0 && a._hp > s->_hp)));
    });
}

CmpsPtr<const Costume> Actor::equipItem(const EquipPos pos, const CmpsPtr<const Costume> item)
{
    //assert(pos != CHAR_NONE && pos != CHAR_RACE && pos != CHAR_JOB);
    return this->equipItem(nullptr, pos, item);
}

Actor::EquipPos Actor::unequipItem(const Costume& item)
{
    return this->unequipItem(nullptr, item);
}

CmpsPtr<const Costume> Actor::unequipPos(const Actor::EquipPos pos)
{
    //assert(pos != CHAR_NONE && pos != CHAR_RACE && pos != CHAR_JOB);
    return this->equipItem(nullptr, pos, nullptr);
}

CmpsPtr<const Costume> Actor::unequipPos(CmpsPtr<Scene> scene, const EquipPos pos)
{
    return this->equipItem(scene, pos, nullptr);
}

Actor::EquipPos Actor::unequipItem(CmpsPtr<Scene> scene, const Costume& item)
{
    auto equipment = this->_equipment;
    for (int i = 0; i < EquipPos::COUNT; i += i)
    {
        if (item == *(equipment[i]))
        {
            this->equipItem(scene, static_cast<EquipPos>(i), nullptr);
            return static_cast<EquipPos>(i);
        }
    }
    return EquipPos::COUNT;
}

CmpsPtr<const Costume> Actor::equipItem(CmpsPtr<Scene> scene, const EquipPos pos, const CmpsPtr<const Costume> item)
{
    auto equipment = this->_equipment;
    const auto old = equipment[pos];
    switchCostume(nullptr, scene, old, item);
    equipment[pos] = item;
    return old;
}

void Actor::removeStates(QString* const ret, CmpsPtr<Scene> scene, bool const remove)
{
    Actor& actor = *this;
    CmpsPtr<QMap<CmpsPtr<const State>, int>> const stateDur = actor._state_dur;
    if (stateDur)
    {
        actor.updateStates(true, ret, scene, *stateDur, true);
        if (remove && stateDur->size() == 0)
        {
            actor._state_dur = nullptr;
            delete stateDur.ptr();
        }
    }
}

inline void Actor::setRace(const Costume& race)
{
    return this->setRace(nullptr, race);
}

inline void Actor::setJob(const Costume& job)
{
    return this->setJob(nullptr, job);
}

inline void Actor::setRace(CmpsPtr<Scene> scene, const Costume& race)
{
    Actor& actor = *this;
    actor.equipItem(scene, EquipPos::RACE, &race);
}

void Actor::setJob(CmpsPtr<Scene> scene, const Costume& job)
{
    Actor& actor = *this;
    actor.equipItem(scene, EquipPos::JOB, &job);
    if (!actor.Suit::isShapeShifted())
    {
        QString* spr = job._sprite;
        if (spr)
        {
            QString* actorSpr = actor._sprite;
            if (actorSpr)
            {
                (*actorSpr) = *spr;
            }
            else
            {
                actor._sprite = new QString(*spr);
            }
        }
    }
}

inline void Actor::setOffense(const int atk)
{
    this->_atk = atk;
}

inline void Actor::setDefense(const int def)
{
    this->_def = def;
}

inline void Actor::setSpirit(const int spi)
{
    this->_spi = spi;
}

inline void Actor::setWisdom(const int wis)
{
    this->_wis = wis;
}

inline void Actor::setAgility(const int agi)
{
    this->_agi = agi;
}

void Actor::setAgility(const int agi, Scene& scene)
{
    this->setAgility(agi);
    scene.agiCalc();
}

void Actor::setCurrentHp(const int hp, QString& ret, const bool survive)
{
    const Scene::SpriteAct* const spr = nullptr;
    return this->setCurrentHp(&ret, nullptr, spr, hp, survive);
}

template <typename SpriteRun>
void Actor::setCurrentHp(QString* const ret, CmpsPtr<Scene> scene, const SpriteRun* const actorEvent, const int hp, bool const survive)
{
    Actor& actor = *this;
    if (hp < 1)
    {
        if (actor._hp != 0)
        {
            if (survive || actor.Suit::isInvincible())
            {
                actor._hp = 1;
            }
            else
            {
                actor._sp = 0;
                if (ret)
                {
                    *ret = *ret % Actor::KoTxt.arg(actor._name);
                }
                if (actor._init > 0)
                {
                    actor._init = 0;
                }
                if (actor.Role::isReviving())
                {
                    if (ret)
                    {
                        *ret = *ret % TR_TXT_SCENE_RISES;
                    }
                    actor._hp = actor._m_hp;
                    if (scene && actor.Suit::isShapeShifted())
                    {
                        //Scene::SpriteRun* const actorEvent = scene->_actor_run;
                        if (actorEvent)
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
                    if (scene && ret)
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
            if (scene)
            {
                scene->resetTurn(actor);
            }
        }
    }
}

void Actor::setCurrentHp(const int hp)
{
    int mHp = this->_m_hp;
    this->_hp = hp > mHp ? mHp : (hp < 1 ? 0 : hp);
}

void Actor::setCurrentMp(const int mp)
{
    int mMp = this->_m_mp;
    this->_mp = mp > mMp ? mMp : (mp < 1 ? 0 : mp);
}

void Actor::setCurrentRp(const int sp)
{
    int mSp = this->_m_sp;
    this->_sp = sp > mSp ? mSp : (sp < 1 ? 0 : sp);
}

void Actor::setMaximumHp(const int mHp)
{
    Actor& actor = *this;
    actor._m_hp = mHp;
    if (mHp < actor._hp)
    {
        actor._hp = mHp;
    }
}

void Actor::setMaximumMp(const int mMp)
{
    Actor& actor = *this;
    actor._m_mp = mMp;
    if (mMp < actor._mp)
    {
        actor._mp = mMp;
    }
}

void Actor::setMaximumRp(const int mRp)
{
    Actor& actor = *this;
    actor._m_sp = mRp;
    if (mRp < actor._sp)
    {
        actor._sp = mRp;
    }
}

inline void Actor::setSprite(QString& value)
{
    Actor& actor = *this;
    QString* sprite = actor._sprite;
    if (sprite)
    {
        delete sprite;
    }
    actor._sprite = new QString(value);
}

void Actor::setItems(QMap<const Ability*, int>* const items)
{
    if (this->hasNewItems())
    {
        this->setNewItems(false);
        QMap<const Ability*, int>* oldItems = this->_items;
        if (oldItems)
        {
            delete oldItems;
        }
    }
    this->_items = items;
}

inline void Actor::setMaxLevel(const int maxLv)
{
    this->_max_lv = maxLv;
}

inline void Actor::setCurrentLevel(const int level)
{
    return this->setCurrentLevel(nullptr, level);
}

void Actor::setCurrentLevel(CmpsPtr<Scene> scene, const int level)
{
    Actor& actor = *this;
    while (level > actor._lv)
    {
        actor._xp = actor._maxp;
        actor.levelUp(scene);
    }
    actor._lv = level;

}

inline void Actor::setCurrentExperience(const int xp)
{
    return this->setCurrentExperience(nullptr, xp);
}

void Actor::setCurrentExperience(CmpsPtr<Scene> scene, const int xp)
{
    Actor& actor = *this;
    actor._xp = xp;
    if (xp >= actor._maxp)
    {
        actor.levelUp(scene);
    }
}

void Actor::setElementResistance(const int element, const int res)
{
    Actor& actor = *this;
    QMap<int, int>* rMap = actor._res;
    if (rMap == nullptr)
    {
        rMap = new QMap<int, int>();
        actor._res = rMap;
    }
    rMap->operator[](element) = res;
}

void Actor::setStateResistance(const State* const state, const int res)
{
    Actor& actor = *this;
    CmpsPtr<QMap<CmpsPtr<const State>, int>> stRes = actor._st_res;
    if (stRes == nullptr)
    {
        stRes = new QMap<CmpsPtr<const State>, int>();
        actor._st_res = stRes;
    }
    stRes->operator[](state) = res;
}

template <typename SpriteRun>
void Actor::applyDmgRoles(QString& ret, CmpsPtr<Scene> scene, const SpriteRun* const actorEvent)
{
    Actor& actor = *this;
#if USE_DMG_ROLES
    auto const dmgRoles = actor._dmg_roles;
    if (dmgRoles && dmgRoles->size() > 0)
    {
        for (auto role : *dmgRoles)
        {
            role->apply(ret, scene, actor);
        }
#else
    if (actor.hasDmgRole())
    {
        //QVector<CmpsPtr<Costume>>* const dmgRoles = actor._actor_data->_dmg_roles;
        {
            auto equipment = actor._equipment;
            //if (dmgRoles && dmgRoles->size() > 0)
            if (equipment.size() > 0)
            {
                /*for (const CmpsPtr<const Costume> role : *dmgRoles)
                {
                    role->apply(ret, actor);
                }*/
                auto const last = equipment.cend();
                for (auto it = equipment.cbegin(); it != last; ++it)
                {
                    auto role = it->ptr();
                    if (role->currentHp() != 0 || role->currentMp() != 0 || role->currentRp() != 0)
                    {
                        role->apply(ret, actor);
                    }
                }
            }
        }
        {
            auto& states = actor._state_dur;
            if (states && states->size() > 0)
            {
                auto const last = states->cend();
                for (auto it = states->cbegin(); it != last; ++it)
                {
                    auto& role = it.key();
                    if (role->currentHp() != 0 || role->currentMp() != 0 || role->currentRp() != 0)
                    {
                        role->apply(ret, actor);
                    }
                }
            }
        }
#endif
        if (scene)
        {
            //Scene::SpriteRun* const actorEvent = scene->_actor_run;
#if USE_TARGET_LIST
            if (actorEvent == nullptr || ((*actorEvent)(*scene, &actor, nullptr, false, nullptr, nullptr)))
            {
                QVector<Actor*>* targets = scene->_targets;
                if (targets)
                {
                    targets->clear();
                }
                else
                {
                    targets = new QVector<Actor*>(1);
                    scene->_targets = targets;
                }
                targets->append(this);
            }
#else
            if (actorEvent)
            {
                (*actorEvent)(*scene, &actor, nullptr, false, nullptr, nullptr);
            }
#endif
        }
    }
}

template <typename SpriteRun>
void Actor::applyStates(QString* const ret, CmpsPtr<Scene> scene, const SpriteRun* const spriteRun, const bool consume)
{
    Actor& actor = *this;
    if (consume && ret)
    {
        actor.applyDmgRoles(*ret, scene, spriteRun);
    }
    CmpsPtr<QMap<CmpsPtr<const State>, int>> const stateDur = actor._state_dur;
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

inline void Actor::checkRegSkill(const Ability& skill)
{
    Actor& actor = *this;
    if (skill._r_qty > 0)
    {
        QMap<const Ability*, int>* regSkills = actor._skills_rg_turn;
        if (regSkills == nullptr)
        {
            regSkills = new QMap<const Ability*, int>();
            actor._skills_rg_turn = regSkills;
        }
        regSkills->operator[](&skill) = 0;
    }
}

void Actor::recover(QString& ret)
{
    this->recover(&ret, nullptr);
}

void Actor::recover(QString* const ret, CmpsPtr<Scene> scene)
{
    Actor& actor = *this;
    actor.removeStates(ret, scene, true);
    actor.refreshCostumes(ret, scene);
    actor.setActive(true);
    actor._hp = actor._m_hp;
    actor._mp = actor._m_mp;
    actor._sp = 0;
    {
        QMap<int, int>* const res = actor._res;
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
                actor._res = nullptr;
                delete res;
            }
        }
    }
    {
        auto stRes = actor._st_res.ptr();
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
                actor._st_res = nullptr;
                delete stRes;
            }
        }
    }
    {
        QMap<const Ability*, int>* skillsQty = actor._skills_cr_qty;
        if (skillsQty)
        {
            auto const last = skillsQty->cend();
            for (auto it = skillsQty->cbegin(); it != last; ++it)
            {
                const Ability* const ability = it.key();
                skillsQty->operator[](ability) = ability->_m_qty;
            }
        }
    }
#if USE_DMG_ROLES
    auto dmgRoles = this->_dmg_roles;
    if (dmgRoles)
    {
        this->_dmg_roles = nullptr;
        delete dmgRoles;
    }
#else
    this->setDmgRole(false);
#endif
}

void Actor::levelUp(CmpsPtr<Scene> scene)
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
        if (scene)
        {
            actor.setAgility(actor._agi + 1, *scene);
        }
        else
        {
            actor._agi += 1;
        }
        lv += 1;
    }
    actor._lv = lv;
}

void Actor::switchCostume(QString* const ret, CmpsPtr<Scene> scene, const CmpsPtr<const Costume> oldCost, const CmpsPtr<const Costume> newCost)
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

void Actor::updateAttributes(const bool remove, CmpsPtr<Scene> scene, const Costume& costume)
{
    Actor& actor = *this;
    int const i = remove ? -1 : 1;
    actor.setMaximumHp(actor._m_hp + (i * costume._m_hp));
    actor.setMaximumMp(actor._m_mp + (i * costume._m_mp));
    actor.setMaximumRp(actor._m_sp + (i * costume._m_sp));
    //actor.setMaxActions(actor._m_actions + (i * costume._m_actions));
    actor._atk += i * costume._atk;
    actor._def += i * costume._def;
    actor._spi += i * costume._spi;
    actor._wis += i * costume._wis;
    if (scene)
    {
        actor.setAgility(actor._agi + (i * costume._agi), *scene);
    }
    else
    {
        actor._agi += i * costume._agi;
    }
}

void Actor::updateResistance(const bool remove, QMap<int, int>* const elmRes, CmpsPtr<QMap<CmpsPtr<const State>, int>> const stRes)
{
    Actor& actor = *this;
    if (elmRes)
    {
        QMap<int, int>* aElmRes = actor._res;
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
    if (stRes)
    {
        CmpsPtr<QMap<CmpsPtr<const State>, int>> aStateRes = actor._st_res;
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
            if (aStateRes == nullptr)
            {
                aStateRes = new QMap<CmpsPtr<const State>, int>();
                actor._st_res = aStateRes;
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

/*void Actor::updateSkills(const bool remove, CmpsVct<const Ability>& skills)
{
    Actor& actor = *this;
    auto aSkills = actor._a_skills;
    if (remove)
    {
        if (aSkills)
        {
            for (const Ability& ability : skills)
            {
                aSkills->removeOne(ability);
                if (ability->_r_qty > 0)
                {
                    QMap<const Ability*, int>* regTurn = actor._skills_rg_turn;
                    if (regTurn)
                    {
                        regTurn->remove(ability);
                    }
                }
                if (ability->_m_qty > 0)
                {
                    QMap<const Ability*, int>* crQty = actor._skills_cr_qty;
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
        if (aSkills == nullptr)
        {
            aSkills = new QVector<const Ability*>();
            this->_a_skills = aSkills;
        }
        for (const Ability* const ability : skills)
        {
            if (!aSkills->contains(ability))
            {
                aSkills->append(ability);
                int const mQty = ability->_m_qty;
                if (mQty > 0)
                {
                    QMap<const Ability*, int>* crQty = actor._skills_cr_qty;
                    if (crQty == nullptr)
                    {
                        crQty = new QMap<const Ability*, int>();
                        actor._skills_cr_qty = crQty;
                    }
                    crQty->operator[](ability) = mQty;
                    actor.checkRegSkill(*ability);
                }
            }
        }
    }
}*/

void Actor::updateStates(bool const remove, QString* const ret, CmpsPtr<Scene> scene,
                         const QMap<CmpsPtr<const State>, int>& states, bool const includeWithDur)
{
    Actor& actor = *this;
    if (remove)
    {
        CmpsPtr<QMap<CmpsPtr<const State>, int>> const stateDur = actor._state_dur;
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
                it.key()->inflict(ret, scene, nullptr, actor, rDur, true);
            }
        }
    }
}

void Actor::refreshCostumes(QString* const ret, CmpsPtr<Scene> scene)
{
    Actor& actor = *this;
    {
        auto equipment = actor._equipment;
        auto const last = equipment.cend();
        for (auto it = equipment.cbegin(); it != last; ++it)
        {
            auto costume = it->ptr();
            if (costume)
            {
                costume->refresh(ret, scene, actor, true, false);
            }
        }
    }
    CmpsPtr<QMap<CmpsPtr<const State>, int>> const stateDur = actor._state_dur;
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

Actor::Actor(int const id, QString name, QString sprite, const Costume& race, const Costume& job, int const level, int const maxLv, int const mHp,
             int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, QMap<int, int>* const res,
             CmpsPtr<QMap<CmpsPtr<const State>, int>> const stRes, QMap<const Ability*, int>* const items)
    : Suit(id, name, sprite, false, 0, 0, mHp, mMp, 0, mHp, mMp, mSp, atk, def, spi, wis, agi, false, false, false, false, false,
           false, false, false, false, nullptr)
{
    this->_lv = 1;
    this->_xp = 0;
    this->_maxp = 15;
    this->_side = 0;
    this->_init = 0;
    this->_side = 0;
    this->_res = res;
#if USE_DMG_ROLES
    this->_dmg_roles = nullptr;
#endif
    this->_skills_rg_turn = nullptr;
    this->_skills_cr_qty = nullptr;
    this->_state_dur = nullptr;
    this->_st_res = stRes;
    this->setActive(true);
    this->_max_lv = maxLv;
    this->setRace(race);
    this->setJob(job);
    this->_items = items;
    this->setCurrentLevel(level);
    this->recover(nullptr, nullptr);
    this->_extra = nullptr;
}

Actor::Actor(const Actor& actor) : Suit(actor)
{
    this->_init = 0;//actor.init;
    this->_side = 0;//actor.side;
    /*this->setJob(actor.getJob());
    this->setRace(actor.getRace());
    this->setExperience(actor.xp);*/
    //QVector<const Ability*>* aSkills = new QVector<const Ability*>();
    this->_items = actor.hasNewItems() ? new QMap<const Ability*, int>(*(actor._items)) : actor._items;
    //*(aSkills) = *(actor._a_skills);
    //this->_a_skills = aSkills;
    this->_maxp = actor._maxp;
    this->_xp = actor._xp;
    this->_lv = actor._lv;
    this->_max_lv = actor._lv;
    this->_equipment = actor._equipment;
    {
        QMap<const Ability*, int>* crSkillsQty = actor._skills_cr_qty;
        if (crSkillsQty)
        {
            QMap<const Ability*, int>* nSkillsQty = new QMap<const Ability*, int>();
            (*nSkillsQty) = (*crSkillsQty);
            this->_skills_cr_qty = nSkillsQty;
        }
        else
        {
            this->_skills_cr_qty = nullptr;
        }
    }
    {
        QMap<const Ability*, int>* skillsRgTurn = actor._skills_rg_turn;
        if (skillsRgTurn)
        {
            QMap<const Ability*, int>* nSkillsRgTurn = new QMap<const Ability*, int>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            this->_skills_rg_turn = nSkillsRgTurn;
        }
        else
        {
            this->_skills_rg_turn = nullptr;
        }
    }
    {
        CmpsPtr<QMap<CmpsPtr<const State>, int>> crSkillsQty = actor._state_dur;
        if (crSkillsQty)
        {
            CmpsPtr<QMap<CmpsPtr<const State>, int>> nSkillsQty = new QMap<CmpsPtr<const State>, int>();
            (*nSkillsQty) = (*crSkillsQty);
            this->_state_dur = nSkillsQty;
        }
        else
        {
            this->_state_dur = nullptr;
        }
    }
    {
        CmpsPtr<QMap<CmpsPtr<const State>, int>> crSkillsQty = actor._st_res;
        if (crSkillsQty)
        {
            CmpsPtr<QMap<CmpsPtr<const State>, int>> nSkillsQty = new QMap<CmpsPtr<const State>, int>();
            (*nSkillsQty) = (*crSkillsQty);
            this->_st_res = nSkillsQty;
        }
        else
        {
            this->_st_res = nullptr;
        }
    }
    {
        QMap<int, int>* crSkillsQty = actor._res;
        if (crSkillsQty)
        {
            QMap<int, int>* nSkillsQty = new QMap<int, int>();
            (*nSkillsQty) = (*crSkillsQty);
            this->_res = nSkillsQty;
        }
        else
        {
            this->_st_res = nullptr;
        }
    }
#if USE_DMG_ROLES
    {
        auto skillsRgTurn = actor._dmg_roles;
        if (skillsRgTurn)
        {
            QVector<CmpsPtr<Costume>>* nSkillsRgTurn = new QVector<CmpsPtr<Costume>>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            this->_dmg_roles = nSkillsRgTurn;
        }
        else
        {
            this->_dmg_roles = nullptr;
        }
    }
#endif
    /*{
        auto skillsRgTurn = actor._a_skills;
        if (skillsRgTurn)
        {
            QVector<const Ability*>* nSkillsRgTurn = new QVector<const Ability*>();
            (*nSkillsRgTurn) = (*skillsRgTurn);
            this->_a_skills = nSkillsRgTurn;
        }
        else
        {
            this->_a_skills = nullptr;
        }
    }*/
    /*{
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
    }*/
    //this->_extra = actor._extra;
}

Actor::~Actor()
{
    auto stRes = this->_st_res.ptr();
    if (stRes)
    {
        this->_st_res = nullptr;
        delete stRes;
    }
    auto res = this->_res;
    if (res)
    {
        this->_res = nullptr;
        delete res;
    }
    auto states = this->_state_dur.ptr();
    if (states)
    {
        this->_state_dur = nullptr;
        delete states;
    }
    /*auto skills = this->_a_skills;
    if (skills)
    {
        this->_a_skills = nullptr;
        delete skills;
    }*/
#if USE_DMG_ROLES
    auto dmgRoles = this->_dmg_roles;
    if (dmgRoles)
    {
        this->_dmg_roles = nullptr;
        delete dmgRoles;
    }
#endif
    if (this->hasNewItems())
    {
        //this->setNewItems(false);
        QMap<const Ability*, int>* items = this->_items;
        if (items)
        {
            this->_items = nullptr;
            delete items;
        }
    }
}

#if USE_TEMPLATE
template void Actor::applyDmgRoles(QString& ret, CmpsPtr<Scene> scene, const ArenaWidget* const actorEvent);
template void Actor::applyStates(QString* const ret, CmpsPtr<Scene> scene, const ArenaWidget* const spriteRun, bool const consume);
template void Actor::setCurrentHp(QString* const ret, CmpsPtr<Scene> scene, const ArenaWidget* const actorEvent, int const hp, bool const survive);
#endif
//#else
template void Actor::applyDmgRoles(QString& ret, CmpsPtr<Scene> scene, const Scene::SpriteAct* const actorEvent);
template void Actor::applyStates(QString* const ret, CmpsPtr<Scene> scene, const Scene::SpriteAct* const spriteRun, bool const consume);
template void Actor::setCurrentHp(QString* const ret, CmpsPtr<Scene> scene, const Scene::SpriteAct* const actorEvent, int const hp, bool const survive);
//#endif
