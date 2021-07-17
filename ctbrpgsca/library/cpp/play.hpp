/*
Copyright (C) AD 2013-2021 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef PLAY_HPP
#define PLAY_HPP

#include <atomic>
#include <mutex>

#include <QMap>
#include <QDebug>
#include <QMutex>
#include <QCoreApplication>

#include "library/translations.h"

#if Q_PROCESSOR_WORDSIZE > 4
/*
If the COMPRESS_POINTERS macro is set to a non-zero value, 64bit pointers will be compressed into 32bit integers, according to the following options:
    +4 can compress addresses up to 16GB, at the expense of the 3 lower tag bits, which can no longer be used for other purporses
    +3 can compress addresses up to 8GB, at the expense of the 2 lower tag bits, which can no longer be used for other purporses
    +2 can compress addresses up to 4GB, at the expense of the lower tag bit, which can no longer be used for other purporses
    +1 always stores the pointer in a thread-safe vector, returning its index, thus preserving its full form (including higher bits)
Attempting to compress an address higher than the mentioned limits, will result in the pointer being stored in a thread-safe vector;
The following negative values can also be used, but they are not safe and will lead to crashes, when the memory limits are exceeded:
    -4 can compress addresses up to 32GB, at the expense of the 3 lower tag bits, which can no longer be used for other purporses
    -3 can compress addresses up to 16GB, at the expense of the 2 lower tag bits, which can no longer be used for other purporses
    -2 can compress addresses up to 8GB, at the expense of the lower tag bit, which can no longer be used for other purporses
    -1 can compress addresses up to 4GB, leaving the 3 lower tag bits to be used for other purporses
Setting the ALIGN_POINTERS macro can increase the number of lower bits available for compression, but will reduce usable memory
*/
    #define COMPRESS_POINTERS 4//3
    #define ALIGN_POINTERS 8
#else
    #define COMPRESS_POINTERS 0
    #define ALIGN_POINTERS 0
#endif

#define USE_BIT_FIELDS 1

#define CONVERT_DELEGATE(Type, Attribute, Field) \
    Attribute operator Type() const { return Field; } \
    Attribute operator const Type*() const { return &(Field); } \
    Attribute operator const Type&() const { return Field; } \
    Attribute operator Type*() { return &(Field); } \
    Attribute operator Type&() { return Field; }

#define FORWARD_DELEGATE(Type, Attribute, Field) \
    Attribute const Type& operator*() const { return Field; } \
    Attribute const Type* operator->() const { return &(Field); } \
    Attribute Type* operator->() { return &(Field); } \
    Attribute Type& operator*() { return Field; }

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
    PROP_CUSTOM_FIELD_NEW(Class, GetName, set##PropName, swap##PropName, with##PropName, Type, GetLevel, SetLevel, _##GetName)

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

#define CONSTRUCT_CMPS_PTR(Type, Class, Base) \
    inline Class(const Class& cloned) : Base(cloned) {} \
    inline Class(Class&& cloned) : Base(cloned) {} \
    inline Class(Type* const ptr) : Base(ptr) {} \
    inline Class(Type& ptr) : Base(ptr) {}

#define COMMA ,

namespace
{

    template <typename T, class P> class BasePtr
    {
    protected:
        inline BasePtr<T, P>(const P& cloned)
        {
            static_cast<P*>(this)->P::copy(cloned);
        }

        inline BasePtr<T, P>(P&& cloned)
        {
            static_cast<P*>(this)->P::move(std::forward<P&&>(cloned));
        }

        inline BasePtr<T, P>(T& ptr)
        {
            static_cast<P*>(this)->P::setAddr(&ptr);
        }

        inline BasePtr<T, P>(T* const ptr)
        {
            static_cast<P*>(this)->P::setAddr(ptr);
        }

        inline BasePtr<T, P>() {}
    };
#if COMPRESS_POINTERS > 0
    class PtrList
    {
    protected:
        inline static uint32_t _null_idx = 0U;
        inline static std::vector<void*> _ptr_list;
        //inline static std::mutex _locker;
        inline static QMutex _locker;

        inline static bool listed(const uint32_t ptr)
        {
            return (ptr & 1U) == 1U;
        }
    };

    template<typename T, const int own = 0, const int level = COMPRESS_POINTERS - 2> class BaseCmp : protected BasePtr<T, BaseCmp<T, own, level>>, protected PtrList
    {
        static constexpr uint32_t CmpsLengthShift(int cmpsLevel)
        {
            if (cmpsLevel == -1)
            {
                return 0;
            }
            if (cmpsLevel < -1)
            {
                cmpsLevel = (cmpsLevel * -1) - 1;
            }
#if ALIGN_POINTERS
            uint32_t bits = 0;
            uint32_t alignment = ALIGN_POINTERS;
            while (alignment >>= 1) bits += 1;
            return static_cast<uint32_t>(cmpsLevel) > bits ? bits : cmpsLevel;
#else
            return cmpsLevel > 1 ? 2 : cmpsLevel;
#endif
        }

        uint32_t _ptr = 0U;

    protected:
        static bool clearList(uint32_t ptr)
        {
            if (ptr == 0U)
            {
                return false;
            }
            if (listed(ptr))
            {
                //auto uniqueLocker = std::unique_lock(PtrList::_locker);
                auto uniqueLocker = QMutexLocker(&PtrList::_locker);
                //uniqueLocker.lock();
                //_locker.lock();
                auto ptrList = &_ptr_list;
                if ((ptr >>= 1) == ptrList->size())
                {
                    size_t ptrListLen;
                    do
                    {
                        ptrList->pop_back();
                    }
                    while ((ptrListLen = ptrList->size()) > 0 && (--ptr) == ptrListLen && ptrList->at(ptr - 1U) == nullptr);
                    if (_null_idx > ptrListLen - 2)
                    {
                        _null_idx = 0U;
                    }
                    ptrList->shrink_to_fit();
                }
                else
                {
                    auto idx = ptr - 1U;
                    (*ptrList)[idx] = nullptr;
                    if (idx < _null_idx)
                    {
                        _null_idx = idx;
                    }
                }
                uniqueLocker.unlock();
                //_locker.unlock();
            }
            return true;
        }

        void listPtr(T* const ptr)
        {
            //qDebug() << "listPtr: ptr = " << ptr;
            uint32_t oldPtr = this->_ptr;
            //auto uniqueLocker = std::unique_lock(PtrList::_locker);
            auto uniqueLocker = QMutexLocker(&PtrList::_locker);
            //uniqueLocker.lock();
            //_locker.lock();
            auto ptrList = &_ptr_list;
            if (listed(oldPtr))
            {
                oldPtr >>= 1;
                if (oldPtr > 0U)
                {
                    (*ptrList)[oldPtr - 1U] = const_cast<void*>(reinterpret_cast<const void*>(ptr));
                    return;
                }
            }
            uint32_t ptrListLen = ptrList->size();
            for (uint32_t i = _null_idx; i < ptrListLen; i += 1U)
            {
                if (ptrList->at(i) == nullptr)
                {
                    //_null_idx = i == ptrListLen - 1 ? 0U : i + 1U;
                    ptrList->operator[](i) = const_cast<void*>(reinterpret_cast<const void*>(ptr));
                    this->_ptr = (i << 1U) | 1U;
                    _null_idx = i + 1U;
                    return;
                }
            }
            _null_idx = ptrListLen;
            ptrList->push_back(const_cast<void*>(reinterpret_cast<const void*>(ptr)));
            this->_ptr = static_cast<uint32_t>(((ptrListLen + 1) << 1) | 1);
            //qDebug() << "listPtr: this->_ptr = " << _ptr;
            //uniqueLocker.unlock();
            //_locker.unlock();
        }

        inline void setAddr(std::nullptr_t)
        {
            if (clearList(this->_ptr))
            {
                //qDebug() << "setAddr: ptr = nullptr";
                this->_ptr = 0U;
            }
        }

        void setAddr(T* const ptr)
        {
            //qDebug() << "setAddr: ptr = " << QString::number(reinterpret_cast<uint64_t>(ptr), 2);
            if (this->ptr() == ptr)
            {
                return;
            }
            else if (ptr == nullptr)
            {
                this->setAddr(static_cast<std::nullptr_t>(nullptr));
                return;
            }
            if constexpr(level == -1)
            {
                this->listPtr(ptr);
            }
            else
            {
                uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
                if (addr < (4294967296UL << SHIFT_LEN))
                //if (addr < (10000UL))
                {
                    //if constexpr(own)
                    {
                        clearList(this->_ptr);
                    }
                    this->_ptr = static_cast<uint32_t>(addr >> SHIFT_LEN);
                    //qDebug() << "setAddr: this->_ptr = " << _ptr;
                }
                else
                {
                    this->listPtr(ptr);
                }
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
                return static_cast<T*>(_ptr_list[(ptr >> 1) - 1U]);
            }
            else
            {
                return reinterpret_cast<T*>(static_cast<uintptr_t>(this->_ptr) << SHIFT_LEN);
            }
        }

        inline BaseCmp() {}

        inline ~BaseCmp()
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
    template<typename T, const int own = 0, const int level = COMPRESS_POINTERS < -1 ? COMPRESS_POINTERS + 1 : 0> class BaseCmp : protected BasePtr<T, BaseCmp<T, own, level>>
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

        inline BaseCmp()
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

        uint32_t _ptr;

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

        inline BaseCmp()
        {
            this->_ptr = 0U;
        }
    #endif
    public:
        inline ~BaseCmp()
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

        inline void copy(const BaseCmp<T, own, level>& cloned)
        {
            static_assert(own < 1, "Attempting to clone unique pointer.");
            this->_ptr = cloned._ptr;
            if constexpr(own < 0)
            {
                const_cast<BaseCmp<T, own, level>&>(cloned)._ptr = 0U;
            }
        }

        inline void move(BaseCmp<T, own, level>&& cloned)
        {
            this->_ptr = cloned._ptr;
            cloned._ptr = 0U;
        }

    public:
        FORWARD_DELEGATE_PTR(T, inline, ptr());
        CONVERT_DELEGATE_PTR(T, inline, ptr());
        //CMPS_METHODS(BaseCmp<T COMMA own COMMA level>)

        inline void setPtr(std::nullptr_t)
        {
            static_assert(!own, "Attempting to change unique pointer.");
            this->setAddr(static_cast<std::nullptr_t>(nullptr));
        }

        inline void setPtr(T* const ptr)
        {
            static_assert(!own, "Attempting to change unique pointer.");
            this->setAddr(ptr);
        }

        template <typename A, class B> friend class BasePtr;

        CONSTRUCT_CMPS_PTR(T, BaseCmp<T COMMA own COMMA level>, BasePtr<T COMMA BaseCmp<T COMMA own COMMA level>>)
    };

    template <typename T, const int cow = 0, const bool weak = false, typename C = std::atomic<uint32_t>, const int level = COMPRESS_POINTERS> class BaseCnt : protected BasePtr<T, BaseCnt<T, cow, weak, C, level>>
    {
        static_assert(cow == 0 || !weak, "Copy-on-write not allowed for weak references.");
        BaseCmp<C, 0, 3> _ref_cnt;
        BaseCmp<T, 0, level> _ptr;

    protected:
        inline auto increase() -> std::enable_if<(!weak), void>
        {
            auto cnt = this->_ref_cnt;
            if (cnt)
            {
                (*cnt) += 1U;
            }
        }

        inline auto decrease() -> std::enable_if<(!weak), void>
        {
            auto ptr = this->_ptr.ptr();
            if (ptr)
            {
                auto cnt = this->_ref_cnt.ptr();
                if (--(*cnt) == 0U)
                {
                    delete ptr;
                    delete cnt;
                }
            }
        }

        void setAddr(T* const ptr)
        {
            this->_ptr.setPtr(ptr);            
            this->_ref_cnt.setPtr(ptr ? new C(1U) : nullptr);
        }

        inline void copy(const BaseCnt<T, cow, weak, C, level>& cloned)
        {
            if constexpr(!weak)
            {
                cloned.increase();
            }
            this->_ptr = cloned._ptr;
            this->_ref_cnt = cloned._ref_cnt;
        }

        inline void move(BaseCnt<T, cow, weak, C, level>&& cloned)
        {
            this->_ptr = cloned._ptr;
            this->_ref_cnt = cloned._ref_cnt;
            cloned._ref_cnt._ptr = 0U;
            cloned._ptr._ptr = 0U;
        }

    public:
        FORWARD_DELEGATE_PTR(T, inline, ptr())
        CONVERT_DELEGATE_PTR(T, inline, ptr())
        //CMPS_METHODS(BaseCnt<T COMMA C COMMA level>)
        inline auto detach(const bool always = true) const -> std::enable_if<(cow != 0), void>
        {
            auto ptr = this->_ptr.ptr();
            if (ptr)
            {
                if (always || (*this->_ref_cnt) > 1)
                {
                    this->setPtr(new T(*ptr));
                }
            }
        }

        inline const T* ptr() const
        {
            return this->_ptr.ptr();
        }

        inline T* ptr()
        {
            if constexpr(cow > 0)
            {
                this->detach(false);
            }
            return this->_ptr.ptr();
        }

        //inline auto setPtr(T* const ptr) -> std::enable_if<(!weak), void>
        inline void setPtr(T* const ptr)
        {
            this->decrease();
            this->setAddr(ptr);
        }

        /*inline CmprShr(const BaseCmp<T, false, level>& cloned)
        {
            this->setAddr(cloned.ptr());
        }*/

        inline BaseCnt<T, cow, weak, C, level>()
        {
            this->setAddr(nullptr);
        }

        inline ~BaseCnt<T, cow, weak, C, level>()
        {
            if constexpr(!weak)
            {
                this->decrease();
            }
        }

        template <typename A, class B> friend class BasePtr;

        CONSTRUCT_CMPS_PTR(T, BaseCnt<T COMMA cow COMMA weak COMMA C COMMA level>, BasePtr<T COMMA BaseCnt<T COMMA cow COMMA weak COMMA C COMMA level>>)
    };

    template<typename T, class P, const int opt = -1> class BaseOpt : public P
    {
        static_assert(std::is_base_of<BasePtr<T, P>, P>::value, "Only references to compressed pointer types can be extended.");

    protected:
        inline T& def() const
        {
            if constexpr(std::is_default_constructible<T>::value)
            {
                return (const_cast<BaseOpt<T, P, opt>*>(this))->refOrNew();
            }
            else
            {
                return this->refOrFail();
            }
        }

        inline const T& obj() const
        {
            return *this->P::ptr();
        }

        inline T& obj()
        {
            return *this->P::ptr();
        }

    public:
        using P::operator*;
        using P::operator->;

        //FORWARD_DELEGATE(T, inline, def())
        CONVERT_DELEGATE(T, inline explicit, obj())
        /*using P::operator bool;
        using P::operator==;
        using P::operator!=;
        using P::operator>=;
        using P::operator<=;
        using P::operator>;
        using P::operator<;
        using P::setRef;*/
        //using P::ref;

        inline auto ptr() -> std::enable_if<(opt > 1), T*>
        {
            return this->P::ptr();
        }

        inline auto ptr() const -> std::enable_if<(opt > 1), const T*>
        {
            return this->P::ptr();
        }

        inline auto swapPtr(T* const ptr) -> std::enable_if<(opt > 1), T*>
        {
            auto _ptr = this->ptr();
            this->P::setPtr(ptr);
            return _ptr;
        }

        inline auto takePtr() -> std::enable_if<(opt > 1), T*>
        {
            return this->swapPtr(nullptr);
        }

        inline auto withPtr(T* const ptr) -> std::enable_if<(opt > 1), BaseOpt<T, P, opt>&>
        {
            this->P::setPtr(ptr);
            return *this;
        }

        inline auto resetPtr(T* const ptr) -> std::enable_if<(opt > 1), void>
        {
            auto _ptr = this->ptr();
            if (_ptr)
            {
                delete _ptr;
            }
            this->P::setPtr(ptr);
        }

        inline T& operator()() const
        {
            return this->def();
        }

        inline operator bool() const
        {
            if constexpr(opt != 0)
            {
                return this->_ptr;
            }
            else
            {
                return true;
            }
        }

        inline auto operator=(std::nullptr_t) -> std::enable_if<(opt > 1), BaseOpt<T, P, opt>&>
        {
            this->P::setPtr(static_cast<std::nullptr_t>(nullptr));
            return *this;
        }

        inline auto operator=(T* const ptr) -> std::enable_if<(opt > 1), BaseOpt<T, P, opt>&>
        {
            this->P::setPtr(ptr);
            return *this;
        }

        inline bool operator<(const T* const ptr) const
        {
            return this->ptr() < ptr;
        }

        inline bool operator<(const BaseOpt<T, P, opt>& cloned) const
        {
            return this->_ptr < cloned._ptr;
        }

        inline bool operator>(const T* const ptr) const
        {
            return this->ptr() > ptr;
        }

        inline bool operator>(const BaseOpt<T, P, opt>& cloned) const
        {
            return this->_ptr > cloned._ptr;
        }

        inline bool operator<=(const T* const ptr) const
        {
            return this->ptr() <= ptr;
        }
        inline bool operator<=(const BaseOpt<T, P, opt>& cloned) const
        {
            return this->_ptr <= cloned._ptr;
        }

        inline bool operator>=(const T* const ptr) const
        {
            return this->ptr() >= ptr;
        }

        inline bool operator>=(const BaseOpt<T, P, opt>& cloned) const
        {
            return this->_ptr >= cloned._ptr;
        }

        inline bool operator==(const T* const ptr) const
        {
            return this->ptr() == ptr;
        }

        inline bool operator==(const BaseOpt<T, P, opt>& cloned) const
        {
            return this->_ptr == cloned._ptr;
        }

        inline bool operator!=(const T* const ptr) const
        {
            return this->ptr() != ptr;
        }

        inline bool operator!=(const BaseOpt<T, P, opt>& cloned)
        {
            return this->_ptr != cloned._ptr;
        }

        inline BaseOpt<T, P, opt>& operator=(BaseOpt<T, P, opt>&& cloned)
        {
            if (this != &cloned)
            {
                this->move(std::forward<BaseOpt<T, P, opt>&&>(cloned));
            }
            return *this;
        }

        inline BaseOpt<T, P, opt>& operator=(const BaseOpt<T, P, opt>& cloned)
        {
            if (this != &cloned)
            {
                this->copy(cloned);
            }
            return *this;
        }

        inline BaseOpt<T, P, opt>& operator=(T& cloned)
        {
            this->P::setPtr(&cloned);
            return *this;
        }

        inline auto setPtr(std::nullptr_t) -> std::enable_if<(opt > 0), void>
        {
            this->P::setPtr(static_cast<std::nullptr_t>(nullptr));
        }

        /*inline auto setPtr(T* const ptr) -> std::enable_if<(opt > 0), void>
        {
            this->P::setPtr(ptr);
        }*/

        inline void setRef(T& cloned)
        {
            this->P::setPtr(&cloned);
        }

        inline auto ref() -> std::enable_if<opt == 0, T&>
        {
            return this->obj();
        }

        inline auto ref() const -> std::enable_if<opt == 0, const T&>
        {
            return this->obj();
        }

        inline auto resetRef() -> std::enable_if<(opt > 0), void>
        {
            this->P::setPtr(nullptr);
        }

        inline auto hasRef() const -> std::enable_if<opt != 0, bool>
        {
            return this->operator bool();
        }

        inline auto refOrFail() -> std::enable_if<opt != 0, T&>
        {
            return this->obj();
        }

        inline auto refOrFail() const -> std::enable_if<opt != 0, const T&>
        {
            return this->obj();
        }

        inline auto refOrElse(T& def) const -> std::enable_if<opt != 0, T&>
        {
            T* ptr = this->P::ptr();
            return ptr ? *ptr : def;
        }

        template<typename... Args>
        inline auto refOrNew(Args&&... args) -> std::enable_if<opt != 0, T&>
        {
            T* ptr = this->P::ptr();
            if (ptr == nullptr)
            {
                ptr = new T(std::forward<Args>(args)...);
                this->P::setPtr(ptr);
            }
            return *ptr;
        }

        inline auto refOrDef() const -> std::enable_if<std::is_nothrow_default_constructible<T>::value, T&>
        {
            return this->def();
        }

        inline auto refOrSet(T& def) -> std::enable_if<opt != 0, T&>
        {
            T* ptr = this->P::ptr();
            if (ptr == nullptr)
            {
                ptr = &def;
                this->P::setPtr(ptr);
            }
            return *ptr;
        }

        inline auto setPtr(T* const ptr) -> std::enable_if<(opt > 1), void>
        {
            this->P::setPtr(ptr);
        }

        template<typename F>
        inline auto runIfRef(F callback) -> std::enable_if<opt != 0, void>
        {
            T* ptr = this->P::ptr();
            if (ptr)
            {
                callback(*ptr);
            }
        }

        template<typename F, typename R>
        inline auto callIfRef(F callback, R defValue) -> std::enable_if<opt != 0, R>
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

        /*inline BaseOpt<T, P, opt>& operator=(T& cloned)
        {
            return static_cast<BaseOpt<T, P, opt>&>(P::operator=(cloned));
        }

        inline BaseOpt<T, P>& operator=(T* const cloned) = delete;

        inline BaseOpt<T, P>& operator=(const P& cloned) = delete;*/

        inline BaseOpt<T, P, opt>(const T& cloned) : P(cloned) {}
        inline BaseOpt<T, P, opt>(const T* cloned) : P(cloned)
        {
            static_assert(opt > 1, "Construction from pointers not allowed.");
        }
        inline BaseOpt<T, P, opt>(P&& moved) : P(std::forward<P&&>(moved)) {}
        inline BaseOpt<T, P, opt>(const P& cloned) : P(cloned) {}
        inline BaseOpt<T, P, opt>(const P* cloned) : P(cloned)
        {
            static_assert(opt > 1, "Construction from pointers not allowed.");
        }
        inline BaseOpt<T, P, opt>() : P()
        {
            static_assert(opt, "Reference is not optional.");
        }
    };
}

#if ALIGN_POINTERS
#ifdef _MSC_VER
inline void* alloc(const std::size_t size)
{
    return _aligned_malloc(size, ALIGN_POINTERS);
}

inline void clear(void* ptr) noexcept
{
    _aligned_free(ptr);
}
#else
inline void* alloc(const std::size_t size)
{
    return std::aligned_alloc(ALIGN_POINTERS, size);
}

inline void clear(void* ptr) noexcept
{
    free(ptr);
}
#endif

inline void* operator new(const std::size_t size, std::nothrow_t)
{
    return alloc(size);
}

inline void* operator new(const std::size_t size)
{
    auto ptr = alloc(size);
    if (ptr)
    {
        return ptr;
    }
    else
    {
        throw std::bad_alloc {};
    }
}

inline void operator delete(void* ptr) noexcept
{
    clear(ptr);
}
#else
inline void* alloc(const std::size_t size)
{
    return std::malloc(size);
}

inline void clear(void* ptr) noexcept
{
    free(ptr);
}
#endif
namespace tbrpgsca
{
    template<typename T, const int own = 0, const int opt = -1, const int level = COMPRESS_POINTERS>
    using CmpsPtr = BaseOpt<T, BaseCmp<T, own, level>, opt>;
    template<typename T, const bool weak, const int cow = 0, const int opt = -1, typename C = std::atomic<uint32_t>, const int level = COMPRESS_POINTERS>
    using CmpsCnt = BaseOpt<T, BaseCnt<T, cow, weak, C, level>, opt>;

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
