/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef STATE_H
#define STATE_H

#include "costume.h"


namespace tbrpgsca
{
    class Scene;

    class State : public Costume
    {
        Q_OBJECT
        QML_ELEMENT
        PROP_FIELD(State, Resistance, resistance, int, inline, public, public, _state_data->_s_res)
        PROP_FIELD(State, Duration, duration, int, inline, public, public, _state_data->_dur)
    public:
        inline static constexpr int EndDur = -3;

        int removedSkillsIdsSize() const;
        bool hasRemovedSkillId(const int skill) const;
        int removedSkillId(int const n) const;

        void alter(QString& ret, Actor& actor, bool const consume);
        void inflict(QString& ret, Actor* user, Actor& target, int const dur, bool const always);
        bool disable(Actor& actor, int const dur, bool const remove);
        void blockSkills(Actor& actor, bool const remove) const;

        State(int const id, QString& name, QString& sprite, bool const shapeShift, int const dur, int const sRes, int const mActions, int const element, int const hpDmg,
              int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
              bool const stun, bool const range, bool const automate, bool const confuse, bool const convert, bool const reflect, bool const ko, bool const invincible,
              bool const revive, QList<Ability>* const aSkills, QList<Ability>* const counters, QVector<int>* const rSkills, QMap<State*, int>* const states,
              QMap<int, int>* const stRes, QMap<int, int>* const res, QObject* const parent = NIL);

        State(int const id, QString&& name, QString&& sprite, bool const shapeShift, int const dur, int const sRes, int const mActions, int const element, int const hpDmg,
              int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
              bool const stun, bool const range, bool const automate, bool const confuse, bool const convert, bool const reflect, bool const ko, bool const invincible,
              bool const revive, QList<Ability>* const aSkills, QList<Ability>* const counters, QVector<int>* const rSkills, QMap<State*, int>* const states,
              QMap<int, int>* const stRes, QMap<int, int>* const res, QObject* const parent = NIL);

        explicit State(QObject* const parent = NIL);

        State(const State& state);

        ~State();
    protected:
        class StateData : public QSharedData
        {
        public:
            ~StateData();

        protected:
            int _dur, _s_res;
            QVector<int>* _r_skills;

            friend class Actor;
            friend class Ability;
            friend class Costume;
            friend class State;
            friend class Scene;
        };

        QSharedDataPointer<StateData> _state_data;

        void alter(QString* const ret, Actor& actor, bool const consume);
        void inflict(QString* const ret, Actor* user, Actor& target, int dur, bool const always);
        bool disable(QString* const ret, Actor& actor, int dur, bool const remove);
        void remove(QString* const ret, Actor& actor) const;

        friend class Actor;
        friend class Ability;
        friend class Costume;
        friend class Scene;
    };

}

Q_DECLARE_METATYPE(tbrpgsca::State)
Q_DECLARE_METATYPE(tbrpgsca::State*)
Q_DECLARE_INTERFACE(tbrpgsca::State, "com.codycostea.tbrpgsca.State")

#endif // STATE_H
