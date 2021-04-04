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
        PROP_FIELD(Ailment, Duration, duration, int, inline, public, public, _dur)
        PROP_FIELD(Ailment, Resistance, resistance, int, inline, public, public, stateData()._s_res)
        //PROP_FIELD(Ailment, CurrentDuration, currentDuration, int, inline, public, public, _crt_dur)
    public:
        inline static constexpr int EndDur = -3;

        int removedSkillsIdsSize() const;
        bool hasRemovedSkillId(const int skill) const;
        int removedSkillId(int const n) const;

        void blockSkills(Actor& actor, bool const remove) const;
        void alter(QString& ret, Actor& actor, bool const consume);
        bool disable(Actor& actor, int const dur, bool const remove);

        Q_INVOKABLE void abandon(QString& ret, tbrpgsca::Actor& actor) const override;
        Q_INVOKABLE void apply(QString& ret, tbrpgsca::Actor& actor) const override;

        Q_INVOKABLE void inflict(QString& ret, tbrpgsca::Actor* user, tbrpgsca::Actor& target, int const dur, bool const always);

        Ailment(int const id, QString& name, QString& sprite, bool const shapeShift, int const dur, int const sRes, int const mActions, int const element, int const blockedSkillTypes,
                int const hpDmg, int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
                bool const stun, bool const range, bool const automate, bool const confuse, bool const convert, bool const reflect, bool const ko, bool const invincible, bool const revive,
                QList<Ability>* const aSkills, QList<Ability>* const counters, QVector<int>* const rSkills, QList<Ailment>* const states, QMap<int, int>* const stRes, QMap<int, int>* const res,
                QObject* const parent = NIL);

        Ailment(int const id, QString&& name, QString&& sprite, bool const shapeShift, int const dur, int const sRes, int const mActions, int const element, int const blockedSkillTypes,
                int const hpDmg, int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
                bool const stun, bool const range, bool const automate, bool const confuse, bool const convert, bool const reflect, bool const ko, bool const invincible, bool const revive,
                QList<Ability>* const aSkills, QList<Ability>* const counters, QVector<int>* const rSkills, QList<Ailment>* const states, QMap<int, int>* const stRes, QMap<int, int>* const res,
                QObject* const parent = NIL);

        explicit Ailment(QObject* const parent = NIL);

        Ailment(const Ailment&& state);

        Ailment(const Ailment& state);

        virtual ~Ailment();
    protected:
        class AilmentSheet : public CostumeSheet
        {
        public:
            virtual ~AilmentSheet();

        protected:
            int _s_res;
            QVector<int>* _r_skills;

            AilmentSheet(int const id, QString& name, QString& sprite, bool const shapeShift, int const sRes, int const mActions, int const element, int const blockedSkillTypes,
                         int const hpDmg, int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis,
                         int const agi, bool const stun, bool const range, bool const automate, bool const confuse, bool const convert, bool const reflect, bool const ko,
                         bool const invincible, bool const revive, QList<Ability>* const aSkills, QList<Ability>* const counters, QVector<int>* const rSkills,
                         QList<Ailment>* const states, QMap<int, int>* const stRes, QMap<int, int>* const res);

            friend class Actor;
            friend class Ability;
            friend class Costume;
            friend class Ailment;
            friend class Scene;
        };

        int _dur;

        inline AilmentSheet& stateMutData()
        {
            return (*static_cast<AilmentSheet*>(this->_play_data.data()));
        }

        inline const AilmentSheet& stateData() const
        {
            return (*static_cast<const AilmentSheet*>(this->_play_data.data()));
        }

        void remove(QString* const ret, Actor& actor) const;
        void alter(QString* const ret, Actor& actor, bool const consume);
        bool disable(QString* const ret, Actor& actor, int dur, bool const remove);

        void inflict(QString* const ret, Actor* user, Actor& target, int dur, bool const always);

        explicit Ailment(QObject* const parent, AilmentSheet* const dataPtr);

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
