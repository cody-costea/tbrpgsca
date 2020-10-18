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
    PROP_FIELD_GET_NEW(Name, Type, Level, _##Name, private)

#define PROP_REF_GET(Name, Type, Level, Field) \
    Level: inline Type& Name() const \
    { \
        return *this->Field; \
    }

#define PROP_REF_GET_NEW(Name, Type, Level, Field, FieldLevel) \
    FieldLevel: Type* Field; \
    PROP_REF_GET(Name, Type, GetLevel, Field)

#define PROP_FIELD_SET(SetName, Type, Level, GetName, Field) \
    Level: Q_SIGNAL void GetName##Changed(Type const value); \
    Q_SLOT inline void SetName(Type const value) \
    { \
        auto field = this->GetName(); \
        if (field != value) \
        { \
            this->Field = value; \
            emit GetName##Changed(value); \
        } \
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
    PROP_FIELD_SET(SetName, Type, Level, GetName, Field) \
    PROP_FIELD_WITH_SWAP(Class, SetName, SwapName, WithName, Type, Level, GetName)

#define PROP_CUSTOM_FIELD(Class, GetName, SetName, SwapName, WithName, Type, GetLevel, SetLevel, Field) \
    PROP_FIELD_GET_NEW(GetName, Type, GetLevel, Field, private) \
    PROP_FIELD_SET_ALL(Class, SetName, SwapName, WithName, Type, SetLevel, GetName, Field) \
    Q_PROPERTY(Type GetName READ GetName WRITE SetName NOTIFY GetName##Changed)

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
    PROP_REF_GET_NEW(GetName, Type, GetLevel, Field, private) \
    PROP_FIELD_SET_ALL(Class, SetName, SwapName, WithName, Type*, SetLevel, GetName, Field)

#define PROP_REF(Class, GetName, SetName, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_REF(Class, GetName, set##SetName, swap##SetName, with##SetName, Type, GetLevel, SetLevel, _##GetName)

#define PROP_FLAG_SET(SetName, Flag, Level, GetName) \
    Level: Q_SIGNAL void GetName##Changed(bool const value); \
    Q_SLOT inline void SetName(bool const value) \
    { \
        bool const field = this->GetName(); \
        if (field != value) \
        { \
            this->setFlag(Flag, value); \
            emit GetName##Changed(value); \
        } \
    }

#define PROP_FLAG_SET_ALL(Class, PropName, Flag, Level, GetName) \
    PROP_FLAG_SET(set##PropName, Flag, Level, GetName) \
    PROP_FIELD_WITH_SWAP(Class, set##PropName, swap##PropName, with##PropName, bool, Level, GetName)

#define PROP_FLAG_GET(Name, Flag, Level) \
    Level: inline bool Name() const \
    { \
        return this->hasAllFlags(Flag); \
    }

#define PROP_FLAG(Class, QmlName, PropName, Flag, GetLevel, SetLevel) \
    PROP_FLAG_GET(is##PropName, Flag, GetLevel) \
    PROP_FLAG_SET_ALL(Class, PropName, Flag, SetLevel, is##PropName) \
    Q_PROPERTY(bool QmlName READ is##PropName WRITE PropName NOTIFY is##PropName##Changed)

    class Play : public QObject
    {
        Q_OBJECT
        PROP_FIELD(Play, Flags, flags, int, public, protected)
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
        Q_INVOKABLE inline bool hasAllFlags(int const flag) const
        {
            return (this->_flags & flag) == flag;
        }
        Q_INVOKABLE inline bool hasAnyFlag(int const flag) const
        {
            return (this->_flags & flag) != 0;
        }
    protected:
        Q_INVOKABLE inline void setFlag(int const flag, bool const value)
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

        Play(QObject* const parent = nullptr, int const flags = 0);

        ~Play();
    private:
        static inline QMetaObject::Connection* _conn = nullptr;
        static inline QTimer* _timer = nullptr;
    };

}

#endif // PLAY_H
