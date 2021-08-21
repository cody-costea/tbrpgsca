/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef COSTUME_HPP
#define COSTUME_HPP

#include "ability.hpp"
#include "suit.hpp"

#include <QMap>

namespace tbrpgsca
{
    class Costume : public Suit
    {
    public:
        inline static const QString CausesTxt = TR_TXT_SCENE_CAUSES;

        int elementResistance(int const element) const;
        int stateResistance(State* const state) const;

        const Ability& addedSkill(int const n) const;
        bool hasAddedSkill(const Ability& skill) const;
        int addedSkillsSize() const;

        void adopt(QString& ret, Actor& actor) const;
        void abandon(QString& ret, Actor& actor) const;
        void apply(QString& ret, Actor& actor) const;

        Costume(int const id, QString name, QString sprite, bool const shapeShift, int const element, int const blockedSkills, int const hpDmg, int const mpDmg, int const spDmg,
                int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const range, bool const automate,
                bool const confuse, bool const reflect, bool const invincible, bool const revive, CmpsVct<CmpsVct<const Ability>, uint32_t, 2U> const skills, bool const counters,
                CmpsPtr<QMap<CmpsPtr<const State>, int>> const states, CmpsPtr<QMap<CmpsPtr<const State>, int>> const stRes, QMap<int, int>* const res);

        Costume(const Costume& costume);

        ~Costume();
    protected:
        QMap<int, int>* _res;
        CmpsPtr<QMap<CmpsPtr<const State>, int>> _st_res;
        CmpsVct<CmpsVct<const Ability>, uint32_t, 2U> _a_skills;
        CmpsPtr<QMap<CmpsPtr<const State>, int>> _state_dur;

        void apply(QString& ret, CmpsPtr<Scene> scene, Actor& actor) const;
        void refresh(QString* const ret, CmpsPtr<Scene> scene, Actor& actor, bool const updStates, bool const remove) const;
        void adopt(QString* const ret, CmpsPtr<Scene> scene, Actor& actor, bool const upeStates, bool const rmeove) const;

        Costume(int const id, QString& name, QString& sprite, bool const shapeShift, int const element, int blockedSkills, int const hpDmg, int const mpDmg, int const spDmg,
                int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun, bool const range,
                bool const automate, bool const confuse, bool const reflect, bool const ko, bool const invincible, bool const revive, CmpsVct<CmpsVct<const Ability>, uint32_t, 2U> const skills,
                bool const counters, CmpsPtr<QMap<CmpsPtr<const State>, int>> const states, CmpsPtr<QMap<CmpsPtr<const State>, int>> const stRes, QMap<int, int>* const res);

        inline Costume() : Suit() {}

        friend class Actor;
        friend class Ability;
        friend class SkillsModel;
        friend class ItemsModel;
        friend class DemoLib;
        friend class State;
        friend class Scene;

        template <typename, class, const int> friend class BasePtr;
        template <typename, const int, const bool, const int, typename, const int> friend class BaseCnt;
        template<typename, typename, typename L, const L, const bool> friend class BaseVct;
    };
}

#endif // COSTUME_HPP
