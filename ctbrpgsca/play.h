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

#define PROP_FIELD_GET_CUSTOM(Name, Type, GetLevel, Field) \
    GetLevel: inline Type Name() const \
    { \
        return this->Field; \
    }

#define PROP_FIELD_GET_NEW(Name, Type, GetLevel, Field, FieldLevel) \
    FieldLevel: Type Field; \
    PROP_FIELD_GET_CUSTOM(Name, Type, GetLevel, Field)

#define PROP_FIELD_GET(Name, Type, Level) \
    PROP_FIELD_GET_NEW(Name, Type, Level, _##Name, protected)

#define PROP_REF_GET(Name, Type, Level, Field) \
    Level: inline Type& Name() const \
    { \
        return *this->Field; \
    }

#define PROP_REF_GET_NEW(Name, Type, Level, Field, FieldLevel) \
    FieldLevel: Type* Field; \
    PROP_REF_GET(Name, Type, GetLevel, Field)

#define PROP_FIELD_SET(Name, Type, Level, Field) \
    Level: inline void Name(Type const value) \
    { \
        this->Field = value; \
    }

#define PROP_FIELD_SETGET(SetGetName, SetName, Type, Level, GetName) \
    Level: inline Type SetGetName(Type const value) \
    { \
        Type old = this->GetName(); \
        this->SetName(value); \
        return old; \
    }

#define PROP_FIELD_WITH(Class, WithName, Type, Level, SetName) \
    Level: inline Class& WithName(Type const value) \
    { \
        this->SetName(value); \
        return *this; \
    }

#define PROP_FIELD_WITH_SETGET(Class, SetName, SetGetName, WithName, Type, Level, GetName) \
    PROP_FIELD_WITH(Class, WithName, Type, Level, SetName) \
    PROP_FIELD_SETGET(SetGetName, SetName, Type, Level, GetName)

#define PROP_FIELD_SET_ALL(Class, SetName, SetGetName, WithName, Type, Level, GetName, Field) \
    PROP_FIELD_SET(SetName, Type, Level, Field) \
    PROP_FIELD_WITH_SETGET(Class, SetName, SetGetName, WithName, Type, Level, GetName)

#define PROP_CUSTOM_FIELD(Class, GetName, SetName, WithName, Type, GetLevel, SetLevel, Field) \
    PROP_FIELD_GET_NEW(GetName, Type, GetLevel, Field, protected) \
    PROP_FIELD_SET_ALL(Class, SetName, GetName, WithName, Type, SetLevel, GetName, Field)

#define PROP_FIELD(Class, PropName, GetName, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD(Class, GetName, set##PropName, with##PropName, Type, GetLevel, SetLevel, _##GetName) \

#if (defined(__clang__) || defined(_MSC_VER))
#define PROP_DECL_FIELD(Class, PropName, GetName, Type, GetLevel, SetLevel) \
    PROP_FIELD(Class, PropName, GetName, Type, GetLevel, SetLevel) \
    __declspec(property(put = set##PropName, get = GetName)) Type PropName;
#endif

#define PROP_CAMEL_FIELD(Class, Name, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD(Class, get##Name, set##Name, with##Name, Type, GetLevel, SetLevel, _##Name)

#define PROP_SNAKE_FIELD(Class, Name, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD(Class, Name, set_##Name, with_##Name, Type, GetLevel, SetLevel, _##Name)

#define PROP_PASCAL_FIELD(Class, Name, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD(Class, Get##Name, Set##Name, With##Name, Type, GetLevel, SetLevel, _##Name)

#define PROP_CUSTOM_REF(Class, GetName, SetName, WithName, Type, GetLevel, SetLevel, Field) \
    PROP_REF_GET_NEW(GetName, Type, GetLevel, Field, protected) \
    PROP_FIELD_SET_ALL(Class, SetName, WithName, Type*, SetLevel, GetName, Field)

#define PROP_REF(Class, GetName, SetName, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_REF(Class, GetName, set##SetName, with##SetName, Type, GetLevel, SetLevel, _##GetName)

#define PROP_FLAG_SET(Name, Flag, Level) \
    Level: inline void set##Name(bool const value) const \
    { \
        this->setFlag(flag, value); \
    }

#define PROP_FLAG_SET_ALL(Class, Name, Flag, Level) \
    PROP_FLAG_SET(Name, Flag, Level) \
    PROP_FIELD_WITH_SETGET(Class, is##Name, Name, bool, Level, is##Name)

#define PROP_FLAG_GET(Name, Flag, Level) \
    Level: inline bool Name() const \
    { \
        return this->hasFlag(Flag); \
    }

#define PROP_FLAG(Class, Name, Flag, Level) \
    PROP_FLAG_GET(is##Name, Flag, Level) \
    PROP_FLAG_SET_ALL(Class, Name, Flag, Level)

    class Play
    {
        PROP_FIELD(Play, Flags, flags, int, public, protected)
    public:
        inline bool hasFlag(int const flag) const
        {
            return (this->_flags & flag) == flag;
        }
    protected:
        inline void setFlag(int const flag, bool const value)
        {
            int const flags = this->_flags;
            if (value != ((flags & flag) == flag))
            {
                this->_flags = flags ^ flag;
            }
        }

        inline Play& withFlag(int const flag, bool const value)
        {
            this->setFlag(flag, value);
            return *this;
        }

        Play();

        Play(int const flags);

        ~Play();
    };

}

#endif // PLAY_H
