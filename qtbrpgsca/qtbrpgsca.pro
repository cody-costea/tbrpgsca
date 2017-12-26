TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp \
    race.cpp \
    job.cpp \
    actor.cpp \
    ability.cpp \
    state.cpp \
    sceneact.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    race.h \
    job.h \
    actor.h \
    ability.h \
    state.h \
    sceneact.h \
    demolib.h
