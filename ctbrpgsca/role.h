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

namespace ctbrpgsca
{
    class State;

    class Role
    {
    #define FLAG_REVIVE 1
    #define FLAG_RANGE 2
    public:
        int getId() const;
        Role* setId(const int value);

        QString getName() const;
        Role* setName(const QString& value);

        QString getSprite() const;
        Role* setSprite(const QString& value);

        int getHpMod() const;
        Role* setHpMod(const int hp);

        int getMpMod() const;
        Role* setMpMod(const int mp);

        int getSpMod() const;
        Role* setSpMod(const int sp);

        int getInitMod() const;
        Role* setInitMod(const int init);

        bool isRanged() const;
        Role* setRanged(const bool range);

        bool isReviving() const;
        Role* setReviving(const bool revive);

        State** getAddedStates() const;
        Role* setAddedStates(State** const value);

        Role(const int id, const QString& name, const QString& sprite, const int mHp, const int mMp,
             const int mSp, const int mInit, const bool range, const bool revive, State** const aStates);

        ~Role();
    private:
        QString name, sprite;
        int id, mHp, mMp, mSp, mInit, flags;
        State** aStates;

        friend class Ability;
        friend class Costume;
        friend class State;
        friend class Actor;
        friend class Scene;
    };
}
#endif // ROLE_H
