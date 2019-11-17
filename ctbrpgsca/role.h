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
    public:
        #define FLAG_RANGE 1
        //static constexpr const int FLAG_RANGE = 1;

        QString* name;
        int id, mHp, mMp, mSp, mInit, flags;
        State** aStates;

        int getId() const;
        Role* setId(const int value);

        QString* getName() const;
        Role* setName(QString* const value);

        int getHpMod() const;
        Role* setHpMod(const int hp);

        int getMpMod() const;
        Role* setMpMod(const int mp);

        int getSpMod() const;
        Role* setSpMod(const int sp);

        int getInitMod() const;
        Role* setInitMod(const int init);

        bool getRange() const;
        Role* setRange(const bool range);

        State** getAddedStates() const;
        Role* setAddedStates(State** const value);

        Role(int id, QString* name, int mHp, int mMp, int mSp, int mInit, bool range, State** aStates);

        ~Role();
    };
}
#endif // ROLE_H
