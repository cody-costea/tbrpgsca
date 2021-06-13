/*
Copyright (C) AD 2013-2021 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef PLAY_HPP
#define PLAY_HPP

#include <QMap>
#include <QCoreApplication>

#include "library/translations.h"

#include <mutex>

namespace tbrpgsca
{
#if Q_PROCESSOR_WORDSIZE > 4
/*
If the COMPRESS_POINTERS macro is set to a non-zero value, 64bit pointers will be compressed into 32bit integers, according to the following options:
    +3 can compress addresses up to 16GB, at the expense of the 3 lower tag bits, which can no longer be used for other purporses
    +2 can compress addresses up to 8GB, at the expense of the 2 lower tag bits, which can no longer be used for other purporses
    +1 can compress addresses up to 4GB, at the expense of the lower tag bit, which can no longer be used for other purporses
Attempting to compress an address higher than the mentioned limits, will lead however to increased usage of both CPU and RAM;
The following negative values can also be used, but they are not safe and will lead to crashes, when the memory limits are exceeded:
    -4 can compress addresses up to 32GB, at the expense of the 3 lower tag bits, which can no longer be used for other purporses
    -3 can compress addresses up to 16GB, at the expense of the 2 lower tag bits, which can no longer be used for other purporses
    -2 can compress addresses up to 8GB, at the expense of the lower tag bit, which can no longer be used for other purporses
    -1 can compress addresses up to 4GB, leaving the 3 lower tag bits to be used for other purporses
*/
    #define COMPRESS_POINTERS 3
#else
    #define COMPRESS_POINTERS 0
#endif

#define USE_BIT_FIELDS 1

#define CONVERT_DELEGATE(Type, Attribute, Field) \
    Attribute operator Type() const { return Field; } \
    Attribute operator Type*() const { return &(Field); } \
    Attribute operator Type&() const { return Field; }

#define FORWARD_DELEGATE(Type, Attribute, Field) \
    Attribute Type& operator*() const { return Field; } \
    Attribute Type* operator->() const { return &(Field); } \
    CONVERT_DELEGATE(Type, Attribute, Field)

#define FORWARD_DELEGATE_PTR(Type, Attribute, Field) \
    FORWARD_DELEGATE(Type, Attribute, *Field)

#define CONVERT_DELEGATE_PTR(Type, Attribute, Field) \
    CONVERT_DELEGATE(Type, Attribute, *Field)

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

#define PROP_CUSTOM_FIELD_NEW(Class, GetName, SetName, SwapName, WithName, Type, GetLevel, SetLevel, Field) \
    PROP_FIELD_GET_NEW(GetName, Type, GetLevel, Field, protected) \
    PROP_FIELD_SET_ALL(Class, SetName, SwapName, WithName, Type, SetLevel, GetName, Field)

#define PROP_CUSTOM_FIELD(Class, GetName, SetName, SwapName, WithName, Type, GetLevel, SetLevel, Field) \
    PROP_FIELD_GET_CUSTOM(GetName, Type, GetLevel, Field) \
    PROP_FIELD_SET_ALL(Class, SetName, SwapName, WithName, Type, SetLevel, GetName, Field)

#define PROP_FIELD(Class, PropName, GetName, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD_NEW(Class, GetName, set##PropName, swap##PropName, with##PropName, Type, GetLevel, SetLevel, _##GetName) \

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

#if COMPRESS_POINTERS > 0
    static class PtrList
    {
    protected:
        inline static std::vector<void*> _ptrList;
        inline static std::mutex _locker;

        inline static bool listed(const uint32_t ptr)
        {
            return (ptr & 1U) == 1U;
        }

        inline PtrList() {};
    };

    template<typename T, const bool own = false, const int level = COMPRESS_POINTERS - 1> class CmprPtr : PtrList
    {
        static constexpr uint CmpsLengthShift(int cmpsLevel)
        {
            if (cmpsLevel < 0)
            {
                cmpsLevel *= -1;
            }
            return cmpsLevel > 1 ? 2 : cmpsLevel;
        }

        u_int32_t _ptr = 0;

    protected:
        static bool clearList(uint32_t ptr)
        {
            if (ptr == 0U)
            {
                return false;
            }
            if (listed(ptr))
            {
                auto uniqueLocker = std::unique_lock(_locker);
                uniqueLocker.lock();
                auto ptrList = &_ptrList;
                if ((ptr >>= 1) == ptrList->size())
                {
                    size_t ptrListLen;
                    do
                    {
                        ptrList->pop_back();
                    }
                    while ((ptrListLen = ptrList->size()) > 0 && (--ptr) == ptrListLen && ptrList->at(ptr - 1U) == nullptr);
                }
                else
                {
                    (*ptrList)[ptr - 1U] = nullptr;
                }
                uniqueLocker.unlock();
            }
            return true;
        }

        void listPtr(T* const ptr)
        {
            uint32_t oldPtr = this->_ptr;
            auto uniqueLocker = std::unique_lock(_locker);
            uniqueLocker.lock();
            auto ptrList = &_ptrList;
            if (listed(oldPtr))
            {
                oldPtr >>= 1;
                if (oldPtr > 0U)
                {
                    (*ptrList)[oldPtr - 1U] = ptr;
                    return;
                }
            }
            size_t ptrListLen = ptrList->size();
            for (uint32_t i = 0; i < ptrListLen; i += 1U)
            {
                if (ptrList->at(i) == nullptr)
                {
                    ptrList->operator[](i) = ptr;
                    this->_ptr = (i << 1U) | 1U;
                    return;
                }
            }
            ptrList->push_back(ptr);
            this->_ptr = static_cast<uint32_t>(((ptrListLen + 1) << 1) | 1);
            uniqueLocker.unlock();
        }

        void setAddr(T* const ptr)
        {
            if (this->ptr() == ptr)
            {
                return;
            }
            else if (ptr == nullptr)
            {
                if (clearList(this->_ptr))
                {
                    this->_ptr = 0U;
                    return;
                }
            }
            uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
            if (addr < (4294967296UL << SHIFT_LEN))
            {
                if constexpr(own)
                {
                    clearList(this->_ptr);
                }
                this->_ptr = static_cast<uint32_t>(addr >> SHIFT_LEN);
            }
            else
            {
                this->listPtr(ptr);
            }
        }

    public:
        inline bool comrpessed() const
        {
            return !listed(this->_ptr);
        }

        inline T* ptr() const
        {
            const uint32_t ptr = this->_ptr;
            if (ptr == 0)
            {
                return nullptr;
            }
            else if (listed(ptr))
            {
                return static_cast<T*>(_ptrList[(ptr >> 1) - 1U]);
            }
            else
            {
                return reinterpret_cast<T*>(static_cast<uintptr_t>(this->_ptr) << SHIFT_LEN);
            }
        }

        inline CmprPtr() {}

        inline ~CmprPtr()
        {
            if constexpr(own)
            {
                auto ptr = this->ptr();
                if (ptr)
                {
                    clearList(this->_ptr);
                    delete ptr;
                }
            }
        }
#else
    template<typename T, const int own = 0, const int level = COMPRESS_POINTERS < 0 ? COMPRESS_POINTERS + 1 : COMPRESS_POINTERS> class CmprPtr
    {
    #if COMPRESS_POINTERS == 0
        static constexpr uint CmpsLengthShift(const int cmpsLevel)
        {
            return cmpsLevel;
        }

        T* _ptr;

    protected:
        inline void setAddr(T* const ptr)
        {
            this->_ptr = ptr;
        }

    public:
        inline bool comrpessed() const
        {
            return false;
        }

        inline T* ptr() const
        {
            return this->_ptr;
        }

        inline CmprPtr()
        {
         this->setPtr(nullptr);
        }
    #else
        static constexpr uint CmpsLengthShift(int cmpsLevel)
        {
            if (cmpsLevel < 0)
            {
                cmpsLevel *= -1;
            }
            return cmpsLevel > 2 ? 3 : cmpsLevel;
        }

        u_int32_t _ptr;

    protected:
        inline void setAddr(T* const ptr)
        {
            uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
            assert(addr < (4294967296UL << SHIFT_LEN)); //TODO: analyze alternative solutions
            this->_ptr = static_cast<uint32_t>(addr >> SHIFT_LEN);
        }

    public:
        inline bool comrpessed() const
        {
            return true;
        }

        inline T* ptr() const
        {
            return reinterpret_cast<T*>(static_cast<uintptr_t>(this->_ptr) << SHIFT_LEN);
        }

        inline CmprPtr()
        {
            this->_ptr = 0U;
        }
    #endif
    public:
        inline ~CmprPtr()
        {
            if constexpr(own)
            {
                auto ptr = this->ptr();
                if (ptr)
                {
                    delete ptr;
                }
            }
        }
#endif
    protected:
        static constexpr int SHIFT_LEN = CmpsLengthShift(level);
    public:
        //FORWARD_DELEGATE(T, inline, *(ptr()))
        FORWARD_DELEGATE_PTR(T, inline, ptr());
        inline T* swapPtr(T* const ptr)
        {
            auto _ptr = this->ptr();
            this->setAddr(ptr);
            return _ptr;
        }

        inline T* takePtr()
        {
            return this->swapPtr(nullptr);
        }

        inline void setPtr(T* const ptr)
        {
            static_assert(!own, "Attempting to change unique pointer.");
            this->setAddr(ptr);
        }

        inline CmprPtr<T, own, level>& withPtr(T* const ptr)
        {
            this->setPtr(ptr);
            return *this;
        }

        inline void resetPtr(T* const ptr)
        {
            if constexpr(own)
            {
                auto _ptr = this->ptr();
                if (_ptr)
                {
                    delete _ptr;
                }
            }
            this->setAddr(ptr);
        }

        inline operator bool() const
        {
            return this->_ptr;
        }

        inline CmprPtr& operator=(T* const ptr)
        {
            if (this->ptr() != ptr)
            {
                this->setPtr(ptr);
            }
            return *this;
        }

        inline CmprPtr& operator=(const CmprPtr<T, own, level>& cloned)
        {
            if (this != &cloned)
            {
                static_assert(!own, "Attempting to copy unique pointer.");
                this->_ptr = cloned._ptr;
            }
            return *this;
        }

        inline CmprPtr(const CmprPtr<T, own, level>& cloned)
        {
            static_assert(!own, "Attempting to clone unique pointer.");
            this->_ptr = cloned._ptr;
        }

        inline CmprPtr(T* const ptr)
        {
            this->setAddr(ptr);
        }
    };

    class Ability;
    class Costume;
    class Actor;
    class State;

    class Play
    {
        //Q_DECLARE_TR_FUNCTIONS(Play)
        PROP_CUSTOM_FIELD(Play, playFlags, setPlayFlags, swapPlayFlags, withPlayFlags, int, public, protected, _play_flags)
    public:
        static void FreeDemoMemory();
        static QVector<Actor*>& Players();
        static QVector<const State*>& States();
        static QVector<QVector<const Ability*>*>& Abilities();
        static QVector<QMap<const State*, int>*>& StateMasks();
        static QVector<QVector<Actor*>*>& Enemies();
        static QVector<const Ability*>& PartyItems();
        static QVector<const Costume*>& Races();
        static QVector<const Costume*>& Jobs();

        inline bool hasAllFlags(unsigned int const flag) const
        {
            return (this->_play_flags & flag) == flag;
        }
        inline bool hasAnyFlag(unsigned int const flag) const
        {
            return (this->_play_flags & flag) != 0;
        }
    protected:
        inline static QVector<Actor*> _Players;
        inline static QVector<QVector<Actor*>*> _Enemies;
        inline static QVector<QMap<const State*, int>*> _State_Masks;
        inline static QVector<QVector<const Ability*>*> _Abilities;
        inline static QVector<const Costume*> _Races, _Jobs;
        inline static QVector<const Ability*> _PartyItems;
        inline static QVector<const State*> _States;

#if USE_BIT_FIELDS
        unsigned int _play_flags: 16;
#else
        unsigned int _play_flags;
#endif
        inline void setFlag(int const flag, bool const value)
        {
            int const flags = this->_play_flags;
            if (value != ((flags & flag) == flag))
            {
                this->_play_flags = flags ^ flag;
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

#endif // PLAY_HPP
