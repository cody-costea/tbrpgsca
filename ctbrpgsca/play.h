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

#define PROP_FIELD_SWAP(SwapName, SetName, GetType, SetType, Level, GetName) \
    Level: inline GetType SwapName(SetType value) \
    { \
        GetType old = this->GetName(); \
        this->SetName(value); \
        return old; \
    }

#define PROP_FIELD_WITH(Class, WithName, Type, Level, SetName) \
    Level: inline Class& WithName(Type value) \
    { \
        this->SetName(value); \
        return *this; \
    }

#define PROP_FIELD_WITH_SWAP(Class, SetName, SwapName, WithName, Type, Level, GetName) \
    PROP_FIELD_WITH(Class, WithName, Type, Level, SetName) \
    PROP_FIELD_SWAP(SwapName, SetName, Type, Type, Level, GetName)

#define PROP_FIELD_SET_ALL(Class, SetName, SwapName, WithName, Type, Level, GetName, Field) \
    PROP_FIELD_SET(SetName, Type, Level, Field) \
    PROP_FIELD_WITH_SWAP(Class, SetName, SwapName, WithName, Type, Level, GetName)

#define PROP_CUSTOM_FIELD(Class, GetName, SetName, SwapName, WithName, Type, GetLevel, SetLevel, Field) \
    PROP_FIELD_GET_NEW(GetName, Type, GetLevel, Field, protected) \
    PROP_FIELD_SET_ALL(Class, SetName, SwapName, WithName, Type, SetLevel, GetName, Field)

#define PROP_FIELD(Class, PropName, GetName, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD(Class, GetName, set##PropName, swap##PropName, with##PropName, Type, GetLevel, SetLevel, _##GetName) \

#if (defined(__clang__) || defined(_MSC_VER))
#define PROP_DECL_FIELD(Class, PropName, GetName, Type, GetLevel, SetLevel) \
    PROP_FIELD(Class, PropName, GetName, Type, GetLevel, SetLevel) \
    __declspec(property(put = set##PropName, get = GetName)) Type PropName;
#endif

#define PROP_CAMEL_FIELD(Class, Name, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD(Class, get##Name, set##Name swap##Name, with##Name, Type, GetLevel, SetLevel, _##Name)

#define PROP_SNAKE_FIELD(Class, Name, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD(Class, Name, set_##Name swap_##Name, with_##Name, Type, GetLevel, SetLevel, _##Name)

#define PROP_PASCAL_FIELD(Class, Name, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD(Class, Get##Name, Set##Name, Swap##Name, With##Name, Type, GetLevel, SetLevel, _##Name)

#define PROP_CUSTOM_REF(Class, GetName, SetName, SwapName, WithName, Type, GetLevel, SetLevel, Field) \
    PROP_REF_GET_NEW(GetName, Type, GetLevel, Field, protected) \
    PROP_FIELD_SET_ALL(Class, SetName, SwapName, WithName, Type*, SetLevel, GetName, Field)

#define PROP_REF(Class, GetName, SetName, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_REF(Class, GetName, set##SetName, swap##SetName, with##SetName, Type, GetLevel, SetLevel, _##GetName)

#define PROP_FLAG_SET(Name, Flag, Level) \
    Level: inline void Name(bool const value) \
    { \
        this->setFlag(Flag, value); \
    }

#define PROP_FLAG_SET_ALL(Class, PropName, Flag, Level, GetName) \
    PROP_FLAG_SET(set##PropName, Flag, Level) \
    PROP_FIELD_WITH_SWAP(Class, set##PropName, swap##PropName, with##PropName, bool, Level, GetName)

#define PROP_FLAG_GET(Name, Flag, Level) \
    Level: inline bool Name() const \
    { \
        return this->hasAllFlags(Flag); \
    }

#define PROP_FLAG(Class, Name, Flag, GetLevel, SetLevel) \
    PROP_FLAG_GET(is##Name, Flag, GetLevel) \
    PROP_FLAG_SET_ALL(Class, Name, Flag, SetLevel, is##Name)

    class Play
    {
        PROP_FIELD(Play, Flags, flags, int, public, protected)
    public:
        inline bool hasAllFlags(int const flag) const
        {
            return (this->_flags & flag) == flag;
        }
        inline bool hasOneFlag(int const flag) const
        {
            return (this->_flags & flag) != 0;
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
