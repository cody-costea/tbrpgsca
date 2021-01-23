/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef AILMENT_H
#define AILMENT_H

#include "costume.h"


namespace tbrpgsca
{
    class Scene;

    class Ailment : public Costume
    {
        Q_OBJECT
        QML_ELEMENT
        PROP_FIELD(Ailment, MaximumDuration, maximumDuration, int, inline, public, public, _max_dur)
        PROP_FIELD(Ailment, Resistance, resistance, int, inline, public, public, _state_data->_s_res)
        //PROP_FIELD(Ailment, CurrentDuration, currentDuration, int, inline, public, public, _crt_dur)
    public:
        inline static constexpr int EndDur = -3;

        int removedSkillsIdsSize() const;
        bool hasRemovedSkillId(const int skill) const;
        int removedSkillId(int const n) const;

        void blockSkills(Actor& actor, bool const remove) const;
        Q_INVOKABLE void inflict(QString& ret, Actor* user, Actor& target, int const dur, bool const always);

        Ailment(int const id, QString& name, QString& sprite, bool const shapeShift, int const dur, int const sRes, int const mActions, int const element, int const hpDmg,
              int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
              bool const stun, bool const range, bool const automate, bool const confuse, bool const convert, bool const reflect, bool const ko, bool const invincible,
              bool const revive, QList<Ability>* const aSkills, QList<Ability>* const counters, QVector<int>* const rSkills, QList<Ailment>* const states,
              QMap<int, int>* const stRes, QMap<int, int>* const res, QObject* const parent = NIL);

        Ailment(int const id, QString&& name, QString&& sprite, bool const shapeShift, int const dur, int const sRes, int const mActions, int const element, int const hpDmg,
              int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
              bool const stun, bool const range, bool const automate, bool const confuse, bool const convert, bool const reflect, bool const ko, bool const invincible,
              bool const revive, QList<Ability>* const aSkills, QList<Ability>* const counters, QVector<int>* const rSkills, QList<Ailment>* const states,
              QMap<int, int>* const stRes, QMap<int, int>* const res, QObject* const parent = NIL);

        explicit Ailment(QObject* const parent = NIL);

        Ailment(const Ailment& state);

        virtual ~Ailment();
    protected:
        class AilmentData : public QSharedData
        {
        public:
            ~AilmentData();

        protected:
            int _s_res;
            QVector<int>* _r_skills;

            friend class Actor;
            friend class Ability;
            friend class Costume;
            friend class Ailment;
            friend class Scene;
        };

        int _max_dur;
        QSharedDataPointer<AilmentData> _state_data;

        void inflict(QString* const ret, Actor* user, Actor& target, int dur, bool const always);

        friend class Actor;
        friend class Ability;
        friend class Costume;
        friend class Scene;
    };

}

Q_DECLARE_METATYPE(tbrpgsca::Ailment)
Q_DECLARE_METATYPE(tbrpgsca::Ailment*)
Q_DECLARE_INTERFACE(tbrpgsca::Ailment, "com.codycostea.tbrpgsca.Ailment")

#endif // AILMENT_H
