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

#include <atomic>
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
    #define COMPRESS_POINTERS 4//3
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
    Attribute Type* operator->() const { return &(Field); }

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

#define CMPS_METHODS(Type) \
inline T* swapPtr(T* const ptr) \
{ \
    auto _ptr = this->ptr(); \
    this->setAddr(ptr); \
    return _ptr; \
} \
inline T* takePtr() \
{ \
    return this->swapPtr(nullptr); \
} \
inline Type& withPtr(T* const ptr) \
{ \
    this->setPtr(ptr); \
    return *this; \
} \
inline void resetPtr(T* const ptr) \
{ \
    auto _ptr = this->ptr(); \
    if (_ptr) \
    { \
        delete _ptr; \
    } \
    this->setAddr(ptr); \
} \
inline operator bool() const \
{ \
    return this->_ptr; \
} \
inline Type& operator=(std::nullptr_t) \
{ \
    this->setPtr(static_cast<std::nullptr_t>(nullptr)); \
    return *this; \
} \
inline Type& operator=(T* const ptr) \
{ \
    this->setPtr(ptr); \
    return *this; \
} \
inline bool operator<(const T* const ptr) const \
{ \
    return this->ptr() < ptr; \
} \
inline bool operator<(const Type& cloned) const \
{ \
    return this->_ptr < cloned._ptr; \
} \
inline bool operator>(const T* const ptr) const \
{ \
    return this->ptr() > ptr; \
} \
inline bool operator>(const Type& cloned) const \
{ \
    return this->_ptr > cloned._ptr; \
} \
inline bool operator<=(const T* const ptr) const \
{ \
    return this->ptr() <= ptr; \
} \
inline bool operator<=(const Type& cloned) const \
{ \
    return this->_ptr <= cloned._ptr; \
} \
inline bool operator>=(const T* const ptr) const \
{ \
    return this->ptr() >= ptr; \
} \
inline bool operator>=(const Type& cloned) const \
{ \
    return this->_ptr >= cloned._ptr; \
} \
inline bool operator==(const T* const ptr) const \
{ \
    return this->ptr() == ptr; \
} \
inline bool operator==(const Type& cloned) const \
{ \
    return this->_ptr == cloned._ptr; \
} \
inline bool operator!=(const T* const ptr) const \
{ \
    return this->ptr() != ptr; \
} \
inline bool operator!=(const Type& cloned) \
{ \
    return this->_ptr != cloned._ptr; \
} \
inline Type& operator=(Type&& cloned) \
{ \
    if (this != &cloned) \
    { \
        this->move(cloned); \
    } \
    return *this; \
} \
inline Type& operator=(const Type& cloned) \
{ \
    if (this != &cloned) \
    { \
        this->copy(cloned); \
    } \
    return *this; \
} \
inline Type& operator=(T& cloned) \
{ \
    this->setPtr(&cloned); \
    return *this; \
} \
inline void setPtr(std::nullptr_t) \
{ \
    this->setAddr(static_cast<std::nullptr_t>(nullptr)); \
} \
inline Type(const Type& cloned) \
{ \
    this->copy(cloned); \
} \
inline void setRef(T& cloned) \
{ \
    this->setPtr(&cloned); \
} \
inline Type(Type&& cloned) \
{ \
    this->move(cloned); \
} \
inline Type(T& ptr) \
{ \
    this->setAddr(&ptr); \
} \
inline Type(T* const ptr) \
{ \
    this->setAddr(ptr); \
} \
inline T& ref() const \
{ \
    return *this->ptr(); \
}

#define COMMA ,

#if COMPRESS_POINTERS > 0
    static class BasePtr
    {
    protected:
        inline static std::vector<void*> _ptrList;
        inline static std::mutex _locker;

        inline static bool listed(const uint32_t ptr)
        {
            return (ptr & 1U) == 1U;
        }

        inline BasePtr() {};
    };

    template<typename T, const int own = 0, const int level = COMPRESS_POINTERS - 1> class CmprPtr : protected BasePtr
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
                    (*ptrList)[oldPtr - 1U] = const_cast<void*>(reinterpret_cast<const void*>(ptr));
                    return;
                }
            }
            size_t ptrListLen = ptrList->size();
            for (uint32_t i = 0; i < ptrListLen; i += 1U)
            {
                if (ptrList->at(i) == nullptr)
                {
                    ptrList->operator[](i) = const_cast<void*>(reinterpret_cast<const void*>(ptr));
                    this->_ptr = (i << 1U) | 1U;
                    return;
                }
            }
            ptrList->push_back(const_cast<void*>(reinterpret_cast<const void*>(ptr)));
            this->_ptr = static_cast<uint32_t>(((ptrListLen + 1) << 1) | 1);
            uniqueLocker.unlock();
        }

        inline void setAddr(std::nullptr_t)
        {
            if (clearList(this->_ptr))
            {
                this->_ptr = 0U;
            }
        }

        void setAddr(T* const ptr)
        {
            if (this->ptr() == ptr)
            {
                return;
            }
            else if (ptr == nullptr)
            {
                this->setAddr(static_cast<std::nullptr_t>(nullptr));
                return;
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
    static class BasePtr {};
    template<typename T, const int own = 0, const int level = COMPRESS_POINTERS < -1 ? COMPRESS_POINTERS + 1 : 0> class CmprPtr : protected BasePtr
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
        inline void setAddr(std::nullptr_t)
        {
            this->_ptr = 0U;
        }

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

        inline void copy(const CmprPtr<T, own, level>& cloned)
        {
            static_assert(own < 1, "Attempting to clone unique pointer.");
            this->_ptr = cloned._ptr;
            if constexpr(own < 0)
            {
                const_cast<CmprPtr<T, own, level>&>(cloned)._ptr = 0U;
            }
        }

        inline void move(CmprPtr<T, own, level>&& cloned)
        {
            this->_ptr = cloned._ptr;
            cloned._ptr = 0U;
        }

    public:
        FORWARD_DELEGATE_PTR(T, inline, ptr());
        CONVERT_DELEGATE_PTR(T, inline, ptr());
        CMPS_METHODS(CmprPtr<T COMMA own COMMA level>)

        inline void setPtr(T* const ptr)
        {
            static_assert(!own, "Attempting to change unique pointer.");
            this->setAddr(ptr);
        }
    };

    template <typename T, typename C = std::atomic<uint32_t>, const int level = COMPRESS_POINTERS> class CmprCnt : protected BasePtr
    {
        //static_assert(std::is_integral<C>::value, "Reference counter type must be an integral.");
        CmprPtr<C, 0, 3> _ref_cnt;
        CmprPtr<T, 0, level> _ptr;

    protected:
        void clean()
        {
            auto ptr = this->ptr();
            if (ptr)
            {
                delete ptr;
            }
            auto cnt = this->_ref_cnt.ptr();
            if (cnt)
            {
                delete cnt;
            }
        }

        inline void reset()
        {
            this->_ref_cnt.setPtr(new C(0U));
        }

        inline void increase()
        {
            (*(this->_ref_cnt)) += 1U;
        }

        inline void decrease()
        {
            if (--(*(this->_ref_cnt)) < 1U)
            {
                this->clean();
            }
        }

        void setAddr(T* const ptr, const bool decrease = true)
        {
            if (decrease)
            {
                auto old = this->ptr();
                if (old)
                {
                    this->decrease();
                }
            }
            this->_ptr.setPtr(ptr);
            this->reset();
        }

        inline void copy(const CmprCnt<T, C, level>& cloned)
        {
            cloned.increase();
            this->_ptr = cloned._ptr;
            this->_ref_cnt = cloned._ref_cnt;
        }

        inline void move(CmprCnt<T, C, level>&& cloned)
        {
            this->_ptr = cloned._ptr;
            this->_ref_cnt = cloned._ref_cnt;
            cloned._ref_cnt._ptr = 0U;
            cloned._ptr._ptr = 0U;
        }

    public:
        FORWARD_DELEGATE_PTR(T, inline, ptr())
        CONVERT_DELEGATE_PTR(T, inline, ptr())
        CMPS_METHODS(CmprCnt<T COMMA C COMMA level>)
        inline T* ptr() const
        {
            return this->_ptr.ptr();
        }

        inline void setPtr(T* const ptr)
        {
            this->setAddr(ptr);
        }

        /*inline CmprShr(const CmprPtr<T, false, level>& cloned)
        {
            this->setAddr(cloned.ptr());
        }*/

        inline CmprCnt<T, C, level>()
        {
            this->setAddr(nullptr, false);
        }

        inline ~CmprCnt<T, C, level>()
        {
            this->decrease();
        }
    };

    template<typename T, typename P = CmprPtr<T>, const bool allowDef = true> class CmprOpt : protected P
    {
        static_assert(std::is_base_of<BasePtr, P>::value, "Only references to compressed pointer types can be wrapped.");
    public:
        FORWARD_DELEGATE(T, inline, refOrDef())
        CONVERT_DELEGATE(T, inline explicit, refOrFail())
        using P::operator bool;
        using P::operator==;
        using P::operator!=;
        using P::operator>=;
        using P::operator<=;
        using P::operator>;
        using P::operator<;
        using P::setRef;
        //using P::ref;

        inline void resetRef()
        {
            this->P::setPtr(nullptr);
        }

        inline T& ref() const = delete;

        inline bool hasRef() const
        {
            return this->_ptr;
        }

        inline T& refOrFail() const
        {
            return this->P::ref();
        }

        inline T& refOrElse(T& def) const
        {
            T* ptr = this->P::ptr();
            return ptr ? *ptr : def;
        }

        template<typename... Args>
        inline T& refOrNew(Args&&... args)
        {
            T* ptr = this->P::ptr();
            if (ptr == nullptr)
            {
                ptr = new T(std::forward<Args>(args)...);
                P::setAddr(ptr);
            }
            return ptr;
        }

        inline T& refOrDef() const
        {
            if constexpr(allowDef)
            {
                return const_cast<CmprOpt<T, P, allowDef>*>(this)->refOrNew();
            }
            else
            {
                return this->refOrFail();
            }
        }

        inline T& refOrSet(T& def)
        {
            T* ptr = this->P::ptr();
            if (ptr == nullptr)
            {
                ptr = &def;
                P::setAddr(ptr);
            }
            return ptr;
        }

        template<typename F>
        inline void runIfRef(F callback)
        {
            T* ptr = this->P::ptr();
            if (ptr)
            {
                callback(*ptr);
            }
        }

        template<typename F, typename R>
        inline R callIfRef(F callback, R defValue)
        {
            T* ptr = this->P::ptr();
            if (ptr)
            {
                return callback(*ptr);
            }
            else
            {
                return defValue;
            }
        }

        inline CmprOpt<T, P, allowDef>& operator=(T& cloned)
        {
            return static_cast<CmprOpt<T, P, allowDef>&>(P::operator=(cloned));
        }

        inline CmprOpt<T, P, allowDef>& operator=(T&& cloned)
        {
            return static_cast<CmprOpt<T, P, allowDef>&>(P::operator=(cloned));
        }

        inline CmprOpt<T, P, allowDef>& operator=(CmprOpt<T, P, allowDef>&& cloned)
        {
            return static_cast<CmprOpt<T, P, allowDef>&>(P::operator=(cloned));
        }

        inline CmprOpt<T, P, allowDef>& operator=(const CmprOpt<T, P, allowDef>& cloned)
        {
            return static_cast<CmprOpt<T, P, allowDef>&>(P::operator=(cloned));
        }

        /*inline bool operator<(const T& cloned) const
        {
            return P::operator<(cloned);
        }

        inline bool operator<(const P& cloned) const
        {
            return P::operator<(cloned);
        }

        inline bool operator>(const T& cloned) const
        {
            return P::operator>(cloned);
        }

        inline bool operator>(const P& cloned) const
        {
            return P::operator>(cloned);
        }

        inline bool operator<=(const T& cloned) const
        {
            return P::operator<=(cloned);
        }

        inline bool operator<=(const P& cloned) const
        {
            return P::operator<=(cloned);
        }

        inline bool operator>=(const T& cloned) const
        {
            return P::operator>=(cloned);
        }

        inline bool operator>=(const P& cloned) const
        {
            return P::operator>=(cloned);
        }

        inline bool operator==(const T& cloned) const
        {
            return P::operator==(cloned);
        }

        inline bool operator==(const P& cloned) const
        {
            return P::operator==(cloned);
        }

        inline bool operator!=(const T& cloned) const
        {
            return P::operator!=(cloned);
        }

        inline bool operator!=(const P& cloned) const
        {
            return P::operator!=(cloned);
        }

        inline operator bool() const
        {
            return this->hasRef();
        }*/

        inline CmprOpt<T, P>(const CmprOpt<T, P, allowDef>& cloned) : P(cloned) {}
        inline CmprOpt<T, P>(CmprOpt<T, P, allowDef>&& moved) : P(moved) {}
        inline CmprOpt<T, P>(const T& cloned) : P(cloned) {}
        inline CmprOpt<T, P>(T&& moved) : P(moved) {}
        inline CmprOpt<T, P>() : P() {}
    };

    template<typename T, typename P = CmprPtr<T>, const bool lazy = false, const bool allowDef = false> class CmprRef : public CmprOpt<T, P, allowDef>
    {
    public:
        //CONVERT_DELEGATE(T, inline, P::ref())
        using P::operator T&;
        using P::operator T*;
        using P::operator T;
        using P::operator->;
        using P::operator*;
        using P::ref;

        inline bool hasRef() const
        {
            if constexpr(lazy)
            {
                return CmprOpt<T, P>::hasRef();
            }
            else
            {
                return true;
            }
        }

        inline void resetRef() = delete;

        inline T& refOrFail() const = delete;

        inline T& refOrElse(T& def) const = delete;

        template<typename F>
        inline void runIfRef(F callback) = delete;

        template<typename F, typename R>
        inline R callIfRef(F callback) = delete;

        inline auto refOrDef() -> std::enable_if<lazy && allowDef, T&>
        {
            return this->CmprOpt<T, P, allowDef>::refOrDef();
        }

        template<typename... Args>
        inline auto refOrNew(Args&&... args) -> std::enable_if<lazy, T&>
        {
            return this->CmprOpt<T, P, allowDef>::refOrNew(std::forward<Args>(args)...);
        }

        inline auto refOrSet(T& cloned) -> std::enable_if<lazy, T&>
        {
            return this->CmprOpt<T, P, allowDef>::refOrSet(cloned);
        }

        inline CmprRef<T, P, lazy, allowDef>& operator=(T& cloned)
        {
            return static_cast<CmprRef<T, P, lazy, allowDef>&>(P::operator=(cloned));
        }

        inline CmprRef<T, P, lazy, allowDef>& operator=(T&& cloned)
        {
            return static_cast<CmprRef<T, P, lazy, allowDef>&>(P::operator=(cloned));
        }

        inline CmprRef<T, P, lazy, allowDef>& operator=(CmprRef<T, P, lazy, allowDef>&& cloned)
        {
            return static_cast<CmprRef<T, P, lazy, allowDef>&>(P::operator=(cloned));
        }

        inline CmprRef<T, P, lazy, allowDef>& operator=(const CmprRef<T, P, lazy, allowDef>& cloned)
        {
            return static_cast<CmprRef<T, P, lazy, allowDef>&>(P::operator=(cloned));
        }

        inline CmprOpt<T, P, allowDef>& operator=(CmprOpt<T, P, allowDef>&& cloned) = delete;

        inline CmprOpt<T, P, allowDef>& operator=(const CmprOpt<T, P, allowDef>& cloned) = delete;


        inline CmprRef<T, P>(const CmprRef<T, P, lazy, allowDef>& cloned) : CmprOpt<T, P>(cloned) {}
        inline CmprRef<T, P>(CmprRef<T, P, lazy, allowDef>&& moved) : CmprOpt<T, P>(moved) {}
        inline CmprRef<T, P>(const T& cloned) : CmprOpt<T, P>(cloned) {}
        inline CmprRef<T, P>(T&& moved) : CmprOpt<T, P>(moved) {}
        inline CmprRef<T, P>() : CmprOpt<T, P>()
        {
            static_assert(lazy, "Non-lazy reference must be initialized.");
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
