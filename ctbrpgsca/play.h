/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef PLAY_H
#define PLAY_H

namespace tbrpgsca
{

#define PROP_FIELD_GET(Name, Type, Field, Level) \
    Level: Type Name() const \
    { \
        return this->Field; \
    }

#define PROP_FIELD_SET(Name, Type, Field, Level) \
    inline void set##Name(Type const value) \
    { \
        this->Field = value; \
    } \

#define PROP_FIELD_SETGET(Name, Type, Field, Level) \
    Level: Type Field(Type const value) \
    { \
        Type old = this->Field(); \
        this->set##Name(value); \
        return old; \
    } \

#define PROP_FIELD_WITH(Class, Name, Type, Level) \
    Level: Class& with##Name(Type const value) \
    { \
        this->set##Name(value); \
        return *this; \
    }

#define PROP_FIELD_WITH_SETGET(Class, Name, Type, Field, Level) \
    PROP_FIELD_SETGET(Name, Type, Field, Level) \
    PROP_FIELD_WITH(Class, Name, Type, Level)

#define PROP_FIELD_SET_ALL(Class, Name, Type, Field, Level) \
    PROP_FIELD_SET(Name, Type, _##Field, Level) \
    PROP_FIELD_WITH_SETGET(Class, Name, Type, Field, Level)

#define PROP_FIELD_NEW(Class, Name, Type, GetLevel, SetLevel, Field, FieldLevel) \
    FieldLevel: Type _##Field; \
    PROP_FIELD(Class, Name, Type, GetLevel, SetLevel, Field)

#define PROP_FIELD(Class, Name, Type, GetLevel, SetLevel, Field) \
    PROP_FIELD_GET(Field, Type, _##Field, GetLevel) \
    PROP_FIELD_SET_ALL(Class, Name, Type, Field, SetLevel)

#define PROP_FLAG_SET(Name, Flag, Level, Field) \
    Level: void set##Name(bool const value) const \
    { \
        this->setFlag(flag, value); \
    }

#define PROP_FLAG_GET(Name, Flag, Level, Field) \
    Level: bool is##Name() const \
    { \
        return this->hasFlag(Flag); \
    }

#define PROP_FLAG(Class, Name, Flag, Level, Field) \
    PROP_FLAG_GET(Name, Flag, Level, Field) \
    PROP_FLAG_SET(Name, Flag, Level, Field) \
    PROP_FIELD_WITH_SETGET(Class, Name, bool, is##Name, Level)

    class Play
    {
PROP_FIELD_NEW(Play, Flags, int, public, protected, flags, protected);
    public:
        //int flags() const;
        bool hasFlag(int const flag) const;
    protected:
        /*int _flags;

        int flags(int const flags);
        void setFlags(int const flags);
        Play& withFlags(int const flags);
        int flags(int const flags, bool const returnOld);*/

        Play& withFlag(int const flag, bool const value);
        void setFlag(int const flag, bool const value);

        Play(int const flags);

        ~Play();
    };

}

#endif // PLAY_H
