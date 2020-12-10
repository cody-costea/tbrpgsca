/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef PLAY_H
#define PLAY_H

#include <QObject>
#include <QApplication>
#include <QThread>
#include <QTimer>
#include <QtQml>

namespace tbrpgsca
{

#define QML_PACK_NAME "com.codycostea.tbrpgsca"
#define QML_PACK_VER 1
#define QML_PACK_REV 0


#define PROP_FIELD_GET_CUSTOM(Name, Type, Attribute, GetLevel, Field) \
    GetLevel: Attribute Type Name() const \
    { \
        return this->Field; \
    }

#define PROP_FIELD_GET_NEW(Name, Type, Attribute, GetLevel, Field, FieldLevel) \
    FieldLevel: Type Field; \
    PROP_FIELD_GET_CUSTOM(Name, Type, Attribute, GetLevel, Field)

#define PROP_FIELD_GET(Name, Type, Attribute, Level) \
    PROP_FIELD_GET_NEW(Name, Type, Attribute, Level, _##Name, private)

#define PROP_REF_GET(Name, Type, Attribute, Level, Field) \
    private: Attribute Type* Name##Ptr() const \
    { \
        return this->Field; \
    } \
    Level: Attribute Type& Name() const \
    { \
        return *this->Field; \
    } \
    Q_INVOKABLE Attribute bool has##Name() \
    { \
        return this->Field; \
    }

#define PROP_REF_GET_NEW(Name, Type, Attribute, Level, Field, FieldLevel) \
    FieldLevel: Type* Field = NIL; \
    PROP_REF_GET(Name, Type, Attribute, GetLevel, Field)

#define PROP_FIELD_SET(SetName, Type, Attribute, Level, GetName, Field) \
    Level: Q_SIGNAL void GetName##Changed(Type newValue, Type oldValue); \
    Q_SLOT Attribute void SetName(Type const value) \
    { \
        auto field = this->GetName(); \
        if (field != value) \
        { \
            this->Field = value; \
            emit GetName##Changed(value, field); \
        } \
    }

#define PROP_REF_SET(PropName, Type, Attribute, Level, GetName, Field) \
    private: Q_SLOT Attribute void set##PropName##Ptr(Type* const value) \
    { \
        auto field = this->GetName##Ptr(); \
        if (field != value) \
        { \
            this->Field = value; \
            emit GetName##Changed(value, field); \
        } \
    } \
    Level: Q_SIGNAL void GetName##Changed(Type* newValue, Type* oldValue); \
    Q_SLOT Attribute void reset##PropName(Type& value) \
    { \
        Type* const old = GetName##Ptr();\
        set##PropName##Ptr(&value); \
        if (old) \
        { \
            delete old; \
        } \
    } \
    Q_SLOT Attribute Type* swap##PropName(Type& value) \
    { \
        Type* const old = GetName##Ptr();\
        set##PropName##Ptr(&value); \
        return old; \
    }

#define PROP_FIELD_SWAP(SwapName, SetName, GetType, SetType, Attribute, Level, GetName) \
    Level: Attribute GetType SwapName(SetType value) \
    { \
        GetType old = this->GetName(); \
        this->SetName(value); \
        return old; \
    }

#define PROP_FIELD_WITH(Class, WithName, Type, Attribute, Level, SetName) \
    Level: Attribute Class& WithName(Type value) \
    { \
        this->SetName(value); \
        return *this; \
    }

#define PROP_REF_WITH(Class, PropName, WithName, Type, Attribute, Level) \
    Level: Attribute Class& WithName(Type& value) \
    { \
        this->set##PropName##Ptr(&value); \
        return *this; \
    }

#define PROP_FIELD_WITH_SWAP(Class, SetName, SwapName, WithName, Type, Attribute, Level, GetName) \
    PROP_FIELD_WITH(Class, WithName, Type, Attribute, Level, SetName) \
    PROP_FIELD_SWAP(SwapName, SetName, Type, Type, Attribute, Level, GetName)

#define PROP_FIELD_SET_ALL(Class, SetName, SwapName, WithName, Type, Attribute, Level, GetName, Field) \
    PROP_FIELD_SET(SetName, Type, Attribute, Level, GetName, Field) \
    PROP_FIELD_WITH_SWAP(Class, SetName, SwapName, WithName, Type, Attribute, Level, GetName)

#define PROP_REF_SET_ALL(Class, PropName, Type, Attribute, Level, GetName, Field) \
    PROP_REF_SET(PropName, Type, Attribute, Level, GetName, Field) \
    PROP_REF_WITH(Class, PropName, with##PropName, Type, Attribute, Level)

#define PROP_CUSTOM_FIELD_DATA(Class, GetName, SetName, SwapName, WithName, Type, Attribute, GetLevel, SetLevel, Field) \
    PROP_FIELD_GET_CUSTOM(GetName, Type, Attribute, GetLevel, Field) \
    PROP_FIELD_SET_ALL(Class, SetName, SwapName, WithName, Type, Attribute, SetLevel, GetName, Field)

#define PROP_CUSTOM_NEW_FIELD_DATA(Class, GetName, SetName, SwapName, WithName, Type, Attribute, GetLevel, SetLevel, Field) \
    PROP_FIELD_GET_NEW(GetName, Type, Attribute, GetLevel, Field, private) \
    PROP_FIELD_SET_ALL(Class, SetName, SwapName, WithName, Type, Attribute, SetLevel, GetName, Field)

#define PROP_CUSTOM_FIELD(Class, GetName, SetName, SwapName, WithName, Type, Attribute, GetLevel, SetLevel, Field) \
    PROP_CUSTOM_FIELD_DATA(Class, GetName, SetName, SwapName, WithName, Type, Attribute, GetLevel, SetLevel, Field) \
    Q_PROPERTY(Type GetName READ GetName WRITE SetName NOTIFY GetName##Changed)

#define PROP_CUSTOM_NEW_FIELD(Class, GetName, SetName, SwapName, WithName, Type, Attribute, GetLevel, SetLevel, Field) \
    PROP_CUSTOM_NEW_FIELD_DATA(Class, GetName, SetName, SwapName, WithName, Type, Attribute, GetLevel, SetLevel, Field) \
    Q_PROPERTY(Type GetName READ GetName WRITE SetName NOTIFY GetName##Changed)

#define PROP_READONLY_CUSTOM_NEW_FIELD(Class, PropName, GetName, Type, Attribute, GetLevel, Field) \
    PROP_CUSTOM_NEW_FIELD_DATA(Class, GetName, SetName, SwapName, WithName, Type, Attribute, GetLevel, protected, Field) \
    Q_PROPERTY(Type GetName READ GetName NOTIFY GetName##Changed)

#define PROP_READONLY_CUSTOM_FIELD(Class, GetName, SetName, SwapName, WithName, Type, Attribute, GetLevel, Field) \
    PROP_CUSTOM_FIELD_DATA(Class, GetName, SetName, SwapName, WithName, Type, Attribute, GetLevel, protected, Field) \
    Q_PROPERTY(Type GetName READ GetName NOTIFY GetName##Changed)

#define PROP_CAMEL_FIELD(Class, Name, Type, Attribute, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD(Class, get##Name, set##Name swap##Name, with##Name, Type, Attribute, GetLevel, SetLevel, _##Name)

#define PROP_SNAKE_FIELD(Class, Name, Type, Attribute, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD(Class, Name, set_##Name swap_##Name, with_##Name, Type, Attribute, GetLevel, SetLevel, _##Name)

#define PROP_PASCAL_FIELD(Class, Name, Type, Attribute, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD(Class, Get##Name, Set##Name, Swap##Name, With##Name, Type, Attribute, GetLevel, SetLevel, _##Name)

#define PROP_READONLY_CUSTOM_REF(Class, PropName, GetName, Type, Attribute, GetLevel, Field) \
    PROP_REF_GET(GetName, Type, Attribute, GetLevel, Field) \
    PROP_REF_SET_ALL(Class, PropName, Type, protected, GetName, Field) \
    Q_PROPERTY(Type* GetName READ GetName##Ptr NOTIFY GetName##Changed)

#define PROP_READONLY_CUSTOM_NEW_REF(Class, PropName, GetName, Type, Attribute, GetLevel, Field) \
    PROP_REF_GET_NEW(GetName, Type, Attribute, GetLevel, Field, private) \
    PROP_REF_SET_ALL(Class, PropName, Type, protected, GetName, Field) \
    Q_PROPERTY(Type* GetName READ GetName##Ptr NOTIFY GetName##Changed)

#define PROP_CUSTOM_REF(Class, PropName, GetName, Type, Attribute, GetLevel, SetLevel, Field) \
    PROP_REF_GET(GetName, Type, Attribute, GetLevel, Field) \
    PROP_REF_SET_ALL(Class, PropName, Type, Attribute, SetLevel, GetName, Field) \
    Q_PROPERTY(Type* GetName READ GetName##Ptr WRITE set##PropName##Ptr NOTIFY GetName##Changed)

#define PROP_CUSTOM_NEW_REF(Class, PropName, GetName, Type, Attribute, GetLevel, SetLevel, Field) \
    PROP_REF_GET_NEW(GetName, Type, Attribute, GetLevel, Field, private) \
    PROP_REF_SET_ALL(Class, PropName, Type, SetLevel, GetName, Field) \
    Q_PROPERTY(Type* GetName READ GetName##Ptr WRITE set##PropName##Ptr NOTIFY GetName##Changed)

#define PROP_FLAG_SET(SetName, Flag, Attribute, Level, GetName) \
    Level: Q_SIGNAL void GetName##Changed(bool const value); \
    Q_SLOT Attribute void SetName(bool const value) \
    { \
        bool const field = this->GetName(); \
        if (field != value) \
        { \
            this->setPlayFlag(Flag, value); \
            emit GetName##Changed(value); \
        } \
    }

#define PROP_FLAG_SET_ALL(Class, PropName, Flag, Attribute, Level, GetName) \
    PROP_FLAG_SET(set##PropName, Flag, Attribute, Level, GetName) \
    PROP_FIELD_WITH_SWAP(Class, set##PropName, swap##PropName, with##PropName, bool, Attribute, Level, GetName)

#define PROP_FLAG_GET(Name, Flag, Attribute, Level) \
    Level: Attribute bool Name() const \
    { \
        return this->hasAllPlayFlags(Flag); \
    }

#define PROP_FLAG_DATA(Class, PropName, QmlName, Flag, Attribute, GetLevel, SetLevel) \
    PROP_FLAG_GET(is##PropName, Flag, Attribute, GetLevel) \
    PROP_FLAG_SET_ALL(Class, PropName, Flag, Attribute, SetLevel, is##PropName) \

#if (!defined(__GNUC__) && (defined(__clang__) || defined(_MSC_VER)))
#define PROP_READONLY_NEW_FIELD(Class, PropName, GetName, Type, Attribute, GetLevel, Field) \
    PROP_READONLY_CUSTOM_NEW_FIELD(Class, GetName, set##PropName, swap##PropName, with##PropName, Type, Attribute, GetLevel, Field) \
    __declspec(property(get = GetName)) Type PropName;

#define PROP_READONLY_FIELD(Class, PropName, GetName, Type, Attribute, GetLevel, Field) \
    PROP_READONLY_CUSTOM_FIELD(Class, GetName, set##PropName, swap##PropName, with##PropName, Type, Attribute, GetLevel, Field) \
    __declspec(property(get = GetName)) Type PropName;

#define PROP_NEW_FIELD(Class, PropName, GetName, Type, Attribute, GetLevel, SetLevel) \
    PROP_CUSTOM_NEW_FIELD(Class, GetName, set##PropName, swap##PropName, with##PropName, Type, Attribute, GetLevel, SetLevel, _##GetName) \
    __declspec(property(put = set##PropName, get = GetName)) Type PropName;

#define PROP_FIELD(Class, PropName, GetName, Type, Attribute, GetLevel, SetLevel, Field) \
    PROP_CUSTOM_FIELD(Class, GetName, set##PropName, swap##PropName, with##PropName, Type, Attribute, GetLevel, SetLevel, Field) \
    __declspec(property(put = set##PropName, get = GetName)) Type PropName;

#define PROP_READONLY_FLAG(Class, PropName, QmlName, Flag, Attribute, GetLevel) \
    PROP_FLAG_DATA(Class, PropName, QmlName, Flag, Attribute, GetLevel, protected) \
    Q_PROPERTY(bool QmlName READ is##PropName NOTIFY is##PropName##Changed) \
    __declspec(property(get = is##QmlName)) bool PropName;

#define PROP_FLAG(Class, PropName, QmlName, Flag, Attribute, GetLevel, SetLevel) \
    PROP_FLAG_DATA(Class, PropName, QmlName, Flag, Attribute, GetLevel, SetLevel) \
    Q_PROPERTY(bool QmlName READ is##PropName WRITE set##PropName NOTIFY is##PropName##Changed) \
    __declspec(property(put = set##PropName, get = is##QmlName)) bool PropName;

#define PROP_READONLY_NEW_REF(Class, PropName, GetName, Type, Attribute, GetLevel) \
    PROP_READONLY_CUSTOM_NEW_REF(Class, PropName, GetName, Type, Attribute, GetLevel, _##GetName) \
    __declspec(property(get = GetName)) Type& PropName;

#define PROP_READONLY_REF(Class, GetName, SetName, Type, Attribute, GetLevel, Field) \
    PROP_READONLY_CUSTOM_REF(Class, PropName, GetName, Type, Attribute, GetLevel, Field) \
    __declspec(property(get = GetName)) Type& PropName;

#define PROP_NEW_REF(Class, PropName, GetName, Type, Attribute, GetLevel, SetLevel) \
    PROP_CUSTOM_NEW_REF(Class, PropName, GetName, Type, Attribute, GetLevel, SetLevel, _##GetName) \
    __declspec(property(put = swap##PropName, get = GetName)) Type& PropName;

#define PROP_REF(Class, GetName, SetName, Type, Attribute, GetLevel, SetLevel, Field) \
    PROP_CUSTOM_REF(Class, PropName, GetName, Type, Attribute, GetLevel, SetLevel, Field) \
    __declspec(property(put = swap##PropName, get = GetName)) Type& PropName;
#else
#define PROP_READONLY_NEW_FIELD(Class, PropName, GetName, Type, Attribute, GetLevel, Field) \
    PROP_READONLY_CUSTOM_NEW_FIELD(Class, GetName, set##PropName, swap##PropName, with##PropName, Type, Attribute, GetLevel, Field)

#define PROP_READONLY_FIELD(Class, PropName, GetName, Type, Attribute, GetLevel, Field) \
    PROP_READONLY_CUSTOM_FIELD(Class, GetName, set##PropName, swap##PropName, with##PropName, Type, Attribute, GetLevel, Field)

#define PROP_NEW_FIELD(Class, PropName, GetName, Type, Attribute, GetLevel, SetLevel) \
    PROP_CUSTOM_NEW_FIELD(Class, GetName, set##PropName, swap##PropName, with##PropName, Type, Attribute, GetLevel, SetLevel, _##GetName)

#define PROP_FIELD(Class, PropName, GetName, Type, Attribute, GetLevel, SetLevel, Field) \
    PROP_CUSTOM_FIELD(Class, GetName, set##PropName, swap##PropName, with##PropName, Type, Attribute, GetLevel, SetLevel, Field)

#define PROP_READONLY_FLAG(Class, PropName, QmlName, Flag, Attribute, GetLevel) \
    PROP_FLAG_DATA(Class, PropName, QmlName, Flag, Attribute, GetLevel, protected) \
    Q_PROPERTY(bool QmlName READ is##PropName NOTIFY is##PropName##Changed)

#define PROP_FLAG(Class, PropName, QmlName, Flag, Attribute, GetLevel, SetLevel) \
    PROP_FLAG_DATA(Class, PropName, QmlName, Flag, Attribute, GetLevel, SetLevel) \
    Q_PROPERTY(bool QmlName READ is##PropName WRITE set##PropName NOTIFY is##PropName##Changed)

#define PROP_READONLY_NEW_REF(Class, PropName, GetName, Type, Attribute, GetLevel) \
    PROP_READONLY_CUSTOM_NEW_REF(Class, PropName, GetName, Type, Attribute, GetLevel, _##GetName)

#define PROP_READONLY_REF(Class, GetName, SetName, Type, Attribute, GetLevel, Field) \
    PROP_READONLY_CUSTOM_REF(Class, PropName, GetName, Type, Attribute, GetLevel, Field)

#define PROP_NEW_REF(Class, PropName, GetName, Type, Attribute, GetLevel, SetLevel) \
    PROP_CUSTOM_NEW_REF(Class, PropName, GetName, Type, Attribute, GetLevel, SetLevel, _##GetName)

#define PROP_REF(Class, GetName, SetName, Type, Attribute, GetLevel, SetLevel, Field) \
    PROP_CUSTOM_REF(Class, PropName, GetName, Type, Attribute, GetLevel, SetLevel, Field)
#endif

#define REG_QML(Type) \
    qmlRegisterInterface<Type>(#Type, QML_PACK_VER); \
    qmlRegisterType<Type>(QML_PACK_NAME, QML_PACK_VER, QML_PACK_REV, #Type); \
    qRegisterMetaType<Type*>(#Type"*");

#define NIL nullptr

    class Play : public QObject
    {
        Q_OBJECT
        QML_ELEMENT
        PROP_FIELD(Play, PlayFlags, playFlags, int, inline, public, protected, _play_flags)
    public:
        template  <typename PlayAct>
        static void runOnMainThread(PlayAct fn, bool const newTimer)
        {
            QThread* const mainThr = qApp->thread();
            if (mainThr == QThread::currentThread())
            {
                fn();
                return;
            }
            QTimer* timer;
            if (newTimer)
            {
                timer = new QTimer();
                timer->moveToThread(mainThr);
                timer->setSingleShot(true);
                QObject::connect(timer, &QTimer::timeout, [fn, timer]()
                {
                    fn();
                    timer->deleteLater();
                });
            }
            else
            {
                timer = Play::_timer;
                if (!timer)
                {
                    timer = new QTimer();
                    Play::_timer = timer;
                    timer->setSingleShot(true);
                    timer->moveToThread(mainThr);
                }
                QMetaObject::Connection* conn = Play::_conn;
                if (!conn)
                {
                    conn = new QMetaObject::Connection();
                    Play::_conn = conn;
                }
                (*conn) = QObject::connect(timer, &QTimer::timeout, [fn, timer, conn]()
                {
                    fn();
                    QObject::disconnect(*conn);
                });
            }
            QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));
        }
        Q_INVOKABLE inline bool hasAllPlayFlags(int const flag) const
        {
            return (this->_play_flags & flag) == flag;
        }
        Q_INVOKABLE inline bool hasAnyPlayFlag(int const flag) const
        {
            return (this->_play_flags & flag) != 0;
        }
    protected:
        Q_INVOKABLE inline void setPlayFlag(int const flag, bool const value)
        {
            int const flags = this->_play_flags;
            if (value != ((flags & flag) == flag))
            {
                this->_play_flags = flags ^ flag;
            }
        }

        inline Play& withPlayFlag(int const flag, bool const value)
        {
            this->setPlayFlag(flag, value);
            return *this;
        }

        explicit Play(QObject* const parent = NIL, int const flags = 0);

        ~Play();
    private:
        static inline QMetaObject::Connection* _conn = NIL;
        static inline QTimer* _timer = NIL;

        int _play_flags = 0;

        friend class Role;
    };

}

//Q_DECLARE_METATYPE(tbrpgsca::Play)
Q_DECLARE_METATYPE(tbrpgsca::Play*)
Q_DECLARE_INTERFACE(tbrpgsca::Play, "com.codycostea.tbrpgsca.Play")

#endif // PLAY_H
