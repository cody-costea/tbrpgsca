/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef ACTOR_HPP
#define ACTOR_HPP

#include "costume.hpp"

namespace tbrpgsca
{

    class Scene;

    class Actor : public Suit
    {
#define USE_DMG_ROLES 0

        #define FLAG_ACTIVE 32768
        #define FLAG_AI_PLAYER 8192
        #define FLAG_RANDOM_AI 16384
        #define FLAG_NEW_ITEMS 4096
        #define FLAG_DMG_ROLE 65536
        #define CHAR_NONE 0
        #define CHAR_RACE 1
        #define CHAR_JOB 2

        PROP_FLAG_GET(hasNewItems, FLAG_NEW_ITEMS, public)
        PROP_FLAG(Actor, Active, FLAG_ACTIVE, public, public)
        PROP_FLAG(Actor, AiPlayer, FLAG_AI_PLAYER, public, public)
        PROP_FLAG(Actor, RandomAi, FLAG_RANDOM_AI, public, public)
        PROP_FLAG_SET_ALL(Actor, Ranged, FLAG_RANGE, public, Role::isRanged)
        PROP_FLAG_SET_ALL(Actor, NewItems, FLAG_NEW_ITEMS, public, hasNewItems)
        PROP_FLAG_SET_ALL(Actor, Stunned, FLAG_STUN, public, Costume::isStunned)
        PROP_FLAG_SET_ALL(Actor, Reviving, FLAG_REVIVE, public, Role::isReviving)
        PROP_FLAG_SET_ALL(Actor, Enraged, FLAG_ENRAGED, public, Costume::isEnraged)
        PROP_FLAG_SET_ALL(Actor, KnockedOut, FLAG_KO, public, Costume::isKnockedOut)
        PROP_FLAG_SET_ALL(Actor, Confused, FLAG_CONFUSE, public, Costume::isConfused)
        PROP_FLAG_SET_ALL(Actor, Covering, FLAG_COVERING, public, Costume::isCovering)
        PROP_FLAG_SET_ALL(Actor, Reflecting, FLAG_REFLECT, public, Costume::isReflecting)
        PROP_FLAG_SET_ALL(Actor, Invincible, FLAG_INVINCIBLE, public, Costume::isInvincible)
        PROP_FLAG_SET_ALL(Actor, ShapeShifted, FLAG_SHAPE_SHIFT, public, Costume::isShapeShifted)
        PROP_CUSTOM_FIELD_NEW(Actor, partySide, setPartySide, swapPartySide, withPartySide, int, public, public, _side)
        PROP_CUSTOM_FIELD_NEW(Actor, initiative, setInitiative, swapInitiative, withInitive, int, public, public, _init)
        PROP_FIELD_WITH_SWAP(Actor, setCurrentExperience, swapCurrentExperience, withCurrentExperience, int, public, currentExperience)
        PROP_FIELD_WITH_SWAP(Actor, setMaximumHp, swapMaximumHp, withMaximumHp, int, public, Role::maximumHp)
        PROP_FIELD_WITH_SWAP(Actor, setMaximumMp, swapMaximumMp, withMaximumMp, int, public, Role::maximumMp)
        PROP_FIELD_WITH_SWAP(Actor, setMaximumRp, swapMaximumRp, withMaximumRp, int, public, Role::maximumRp)
        //PROP_FIELD_WITH_SWAP(Actor, setMaxActions, swapMaxActions, withMaxActions, int, public, Costume::maxActions)
        PROP_FIELD_WITH_SWAP(Actor, setCurrentHp, swapCurrentHp, withCurrentHp, int, public, Role::currentHp)
        PROP_FIELD_WITH_SWAP(Actor, setCurrentMp, swapCurrentMp, withCurrentMp, int, public, Role::currentMp)
        PROP_FIELD_WITH_SWAP(Actor, setCurrentRp, swapCurrentRp, withCurrentRp, int, public, Role::currentRp)
        PROP_FIELD_WITH_SWAP(Actor, setOffense, swapOffense, withOffense, int, public, Costume::offense)
        PROP_FIELD_WITH_SWAP(Actor, setDefense, swapDefense, withDefense, int, public, Costume::defense)
        PROP_FIELD_WITH_SWAP(Actor, setAgility, swapAgility, withAgility, int, public, Costume::agility)
        PROP_FIELD_WITH_SWAP(Actor, setWisdom, swapWisdom, withWisdom, int, public, Costume::wisdom)
        PROP_FIELD_WITH_SWAP(Actor, setSpirit, swapSpirit, withSpirit, int, public, Costume::spirit)
        PROP_FIELD_WITH_SWAP(Actor, setMaxLevel, swapMaxLevel, withMaxLevel, int, public, maximumLevel)
        PROP_FIELD_WITH_SWAP(Actor, setCurrentLevel, swapCurrentLevel, withCurrentLevel, int, public, currentLevel)
        //PROP_FIELD_WITH_SWAP(Actor, setItems, items, withItems, QMap<Ability*, int>*, public, items)
        PROP_FIELD_SET_ALL(Actor, setName, sawpName, withName, QString, public, Role::name, _name)
        PROP_FIELD_WITH_SWAP(Actor, setRace, swapRace, withRace, const Costume&, public, race)
        PROP_FIELD_WITH_SWAP(Actor, setJob, swapJob, withJob, const Costume&, public, job)
        PROP_FIELD_SWAP(swapSprite, setSprite, QString, QString&, public, Role::sprite)
        PROP_FIELD_WITH(Actor, withSprite, QString&, public, setSprite)
        PROP_FIELD(Actor, Extra, extra, void*, public, protected)
        PROP_FIELD_GET_CUSTOM(maximumExperience, int, public, _maxp)
        //PROP_FIELD_GET_CUSTOM(initiative, int, public, _init)
        //PROP_FIELD_GET_CUSTOM(actions, int, public, _actions)
        PROP_FIELD_GET_CUSTOM(maximumLevel, int, public, _max_lv)
        PROP_FIELD_GET_CUSTOM(currentExperience, int, public, _xp)
        PROP_FIELD_GET_CUSTOM(currentLevel, int, public, _lv)
#if !USE_DMG_ROLES
        PROP_FLAG_GET(hasDmgRole, FLAG_DMG_ROLE, public)
        PROP_FLAG_SET_ALL(Actor, DmgRole, FLAG_DMG_ROLE, public, hasDmgRole)
#endif
    public:
        enum EquipPos : uint8_t
        {
            JOB = 0U,
            RACE = 1U,
            HEAD = 2U,
            BODY = 3U,
            WEAPON = 4U,
            SHIELD = 5U,
            EXTRA1 = 6U,
            EXTRA2 = 7U,
            COUNT = 8U
        };

        struct SkillSearch
        {
            const uint32_t index;
            const Ability* const ability;

            inline operator const int() const
            {
                return this->index;
            }

            inline operator const uint32_t() const
            {
                return this->index;
            }

            inline operator const Ability*() const
            {
                return this->ability;
            }

            inline operator const Ability&() const
            {
                return *this->ability;
            }

            inline SkillSearch(const Ability* const ability, const uint32_t index) : index(index), ability(ability) {}
        };

        inline static const QString KoTxt = TR_TXT_SCENE_FALLS;

        QMap<const Ability*, int> items() const;
        //int getPartySide() const;
        const Costume& race() const;
        const Costume& job() const;

        uint32_t skillsCount() const;
        //const Ability* skill(uint32_t index) const;
        const CmpsVct<CmpsPtr<Ability>>& skills() const;
        int remainingSkillUses(const Ability& skill) const;
        int regeneratingSkillTurn(const Ability& skill) const;

        const Ability* skill(uint32_t idx) const;
        template<const bool first, typename F>
        const Ability* skill(F compFun) const
        {
            const Ability* ability = nullptr;
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
                        uint32_t cnt = skills.size();
                        for (uint32_t k = 0U; k < cnt; k += 1U)
                        {
                            auto skill = &skills.at(k);
                            if constexpr(first)
                            {
                                if (compFun(*skill))
                                {
                                    return skill;
                                }
                            }
                            else
                            {
                                if (compFun(*skill, ability))
                                {
                                    ability = skill;
                                }
                            }
                        }
                    }
                }
            }
            return ability;
        }

        template<const bool first, typename F>
        SkillSearch skill(uint32_t idx, F compFun) const
        {
            uint32_t len = 0;
            uint32_t ret = idx;
            const Ability* ability = nullptr;
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
                        uint32_t size = skills.size();
                        uint32_t cnt = len + size;
                        //if (ret < (len += size))
                        if (ret < cnt)
                        {
                            if (ability)
                            {
                                idx = 0U;
                            }
                            else
                            {
                                idx = (ret - len);
                                ability = &skills.at(idx);
                                idx += 1;
                            }
                            for (; idx < size; idx += 1U)
                            {
                                auto skill = &skills.at(idx);
                                if (compFun(*skill, *ability))
                                {
                                    if constexpr(first)
                                    {
                                        return SkillSearch(skill, idx + len);
                                    }
                                    else
                                    {
                                        ability = skill;
                                        ret = idx + len;
                                    }
                                }
                            }
                        }
                        len = cnt;
                    }
                }
            }
            return SkillSearch(ability, idx);
        }

        template<const bool first, typename F>
        uint32_t skillIndex(F compFun) const
        {
            uint32_t len = 0U, ret = 0U;
            const Ability* ability = nullptr;
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
                        uint32_t cnt = skills.size();
                        for (uint32_t k = 0U; k < cnt; k += 1U)
                        {
                            auto skill = &skills.at(k);
                            if constexpr(first)
                            {
                                if (compFun(*skill))
                                {
                                    return k + len;
                                }
                            }
                            else
                            {
                                if (compFun(*skill, ability))
                                {
                                    ret = k + len;
                                }
                            }
                        }
                        len += cnt;
                    }
                }
            }
            return ret;
        }

        SkillSearch aiSkill(uint32_t const defSkill, bool const restore) const;

        CmpsPtr<const Costume> unequipPos(const EquipPos pos);
        CmpsPtr<const Costume> equipItem(const EquipPos pos, const CmpsPtr<const Costume> item);
        EquipPos unequipItem(const Costume& item);

        void recover(QString& ret);
        //Actor& applyRoles(QString& ret);
        void applyStates(QString& ret, bool const consume);
        void setElementResistance(int const element, int const res);
        void setStateResistance(const State* const state, int const res);
        void setItems(QMap<const Ability*, int>* items);
        void setCurrentExperience(int const xp);
        void setCurrentLevel(int const level);
        void setSprite(QString& value);
        void setJob(const Costume& job);
        void setRace(const Costume& race);
        //void setMaxActions(int const mActions);
        void setMaxLevel(int const maxLv);
        void setAgility(int const agi);
        void setOffense(int const atk);
        void setDefense(int const def);
        void setWisdom(int const wis);
        void setSpirit(int const spi);
        void setMaximumHp(int const mHp);
        void setMaximumMp(int const mMp);
        void setMaximumRp(int const mSp);
        void setCurrentHp(int const hp, QString& ret, bool const survive);
        void setCurrentHp(int const hp);
        void setCurrentMp(int const mp);
        void setCurrentRp(int const sp);

        Actor(int const id, QString name, QString sprite, const Costume& race, const Costume& job, int const level, int const maxLv,
              int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
              QMap<int, int>* const res, CmpsPtr<QMap<CmpsPtr<const State>, int>> const stRes, QMap<const Ability*, int>* const items);

        Actor(const Actor& actor);

        ~Actor();
    protected:
#if USE_BIT_FIELDS
        signed int _lv: 8, _max_lv: 8, _xp: 32, _maxp: 32;
#else
        signed int _lv, _max_lv, _xp, _maxp;
#endif
        QMap<int, int>* _res;
        CmpsPtr<QMap<CmpsPtr<const State>, int>> _st_res;
        //CmpsPtr<QMap<CmpsPtr<const State>, int>> _state_dur;
        //CmpsVct<CmpsVct<const Ability>, uint32_t, 2U, CmpsPtr<CmpsVct<const Ability>>> _a_skills;
        QMap<const Ability*, int>* _skills_cr_qty,* _skills_rg_turn,* _items;
        CmpsVct<CmpsPtr<const Costume>, uint32_t, EquipPos::COUNT> _equipment;
        //QMap<char, CmpsPtr<Costume>> _equipment;
#if USE_DMG_ROLES
        QVector<CmpsPtr<Costume>>* _dmg_roles;
#endif
        inline void checkRegSkill(const Ability& skill);
        void recover(QString* const ret, CmpsPtr<Scene> scene);
        EquipPos unequipItem(CmpsPtr<Scene> scene, const Costume& item);
        void refreshCostumes(QString* const ret, CmpsPtr<Scene> scene);
        CmpsPtr<const Costume> unequipPos(CmpsPtr<Scene> scene, EquipPos const pos);
        CmpsPtr<const Costume> equipItem(CmpsPtr<Scene> scene, EquipPos const pos, const CmpsPtr<const Costume> item);
        void switchCostume(QString* const ret, CmpsPtr<Scene> scene, const CmpsPtr<const Costume> oldCostume, const CmpsPtr<const Costume> newCostume);
        void removeStates(QString* const ret, CmpsPtr<Scene> scene, bool const remove);
        void updateStates(bool const remove, QString* const ret, CmpsPtr<Scene> scene,
                          const QMap<CmpsPtr<const State>, int>& states, bool const includeWithDur);
        void updateAttributes(bool const remove, CmpsPtr<Scene> scene, const Costume& costume);
        void updateResistance(bool const remove, QMap<int, int>* const elmRes, CmpsPtr<QMap<CmpsPtr<const State>, int>> const stRes);
        //void updateSkills(bool const remove, CmpsVct<const Ability> skills);
        void setCurrentExperience(CmpsPtr<Scene> scene, int const xp);
        void setCurrentLevel(CmpsPtr<Scene> scene, int const level);
        void setRace(CmpsPtr<Scene> scene, const Costume& race);
        void setJob(CmpsPtr<Scene> scene, const Costume& job);
        void setAgility(int const agi, Scene& scene);
        void levelUp(CmpsPtr<Scene> scene);

        template <typename SpriteRun>
        void applyDmgRoles(QString& ret, CmpsPtr<Scene> scene, const SpriteRun* const actorEvent);

        template <typename SpriteRun>
        void applyStates(QString* const ret, CmpsPtr<Scene> scene, const SpriteRun* const spriteRun, bool const consume);

        template <typename SpriteRun>
        void setCurrentHp(QString* const ret, CmpsPtr<Scene> scene, const SpriteRun* const actorEvent, int const hp, bool const survive);

        inline Actor() : Suit() {}

        friend class Scene;
        friend class Ability;
        friend class ArenaWidget;
        friend class TargetsModel;
        friend class SkillsModel;
        friend class ItemsModel;
        friend class Costume;
        friend class State;
        friend class Role;

        template <typename, class, const int> friend class BasePtr;
        template <typename, const int, const bool, const int, typename, const int> friend class BaseCnt;
        template<typename, typename, typename L, const L, const bool> friend class BaseVct;
    };

}

#endif // ACTOR_HPP
