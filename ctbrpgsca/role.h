/*
Copyright (C) AD 2013-2019 Claudiu-Stefan Costea

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#ifndef ROLE_H
#define ROLE_H

#include <QString>
#include <QVector>

namespace tbrpgsca
{
    class Actor;
    class State;
    class Scene;

    class Role
    {
        #define FLAG_REVIVE 1
        #define FLAG_RANGE 2
    public:
        static QString HpTxt;
        static QString MpTxt;
        static QString RpTxt;
        static QString CausesTxt;

        static QString& AddDmgText(QString& ret, int dmgHp, int dmgMp, int dmgRp);

        inline int getId() const;
        inline QString getName() const;
        inline QString getSprite() const;
        inline bool isReviving() const;
        inline bool isRanged() const;
        inline int getMaximumHp() const;
        inline int getMaximumMp() const;
        inline int getMaximumRp() const;
        inline int getCurrentHp() const;
        inline int getCurrentMp() const;
        inline int getCurrentRp() const;
        inline int getElement() const;

        inline Role& apply(QString& ret, Actor& actor);
    protected:
        QString name, sprite;
        int id, hp, mp, sp, mHp, mMp, mSp, elm, flags;

        inline Role& adopt(Actor& actor);

        inline Role& abandon(Actor& actor);

        Role& apply(QString& ret, Scene* scene, Actor& actor);

        Role(int const id, QString& name, QString& sprite, int const hpDmg, int const mpDmg, int const spDmg,
             int const mHp, int const mMp, int const mSp, int const element, bool const range, bool const revive);

        Role(const Role& role);

        ~Role();

        friend class Ability;
        friend class Costume;
        friend class State;
        friend class Actor;
        friend class Scene;
    };
}
#endif // ROLE_H
