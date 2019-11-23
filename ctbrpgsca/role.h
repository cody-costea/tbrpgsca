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

namespace ctbrpgsca
{
    class State;

    class Role
    {
    #define FLAG_REVIVE 1
    #define FLAG_RANGE 2
    public:
        int getId() const;
        QString getName() const;
        State* getAddedState(int n) const;
        int getAddedStatesSize() const;
        QString getSprite() const;
        bool isReviving() const;
        bool isRanged() const;
        //int getInitMod() const;
        int getHpMod() const;
        int getMpMod() const;
        int getSpMod() const;
        int getDmgHp() const;
        int getDmgMp() const;
        int getDmgSp() const;

        Role(const int id, const QString& name, const QString& sprite, const int hpDmg, const int mpDmg, const int spDmg,
             const int mHp, const int mMp, const int mSp, const bool range, const bool revive, QVector<State*>* const aStates);

        ~Role();
    private:
        QString name, sprite;
        int id, hp, mp, sp, mHp, mMp, mSp, flags;
        QVector<State*>* aStates;

        friend class Ability;
        friend class Costume;
        friend class State;
        friend class Actor;
        friend class Scene;
    };
}
#endif // ROLE_H
