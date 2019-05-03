TEMPLATE = app

QT += qml quick
CONFIG += c++11
CONFIG += resources_big
CONFIG += qtquickcompiler

SOURCES += \
    ability.cpp \
    actor.cpp \
    job.cpp \
    main.cpp \
    race.cpp \
    sceneact.cpp \
    state.cpp

RESOURCES += qml.qrc

QSG_RENDER_LOOP = windows

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    ability.h \
    actor.h \
    demolib.h \
    job.h \
    race.h \
    sceneact.h \
    state.h

DISTFILES +=
