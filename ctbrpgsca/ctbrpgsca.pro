#-------------------------------------------------
#
# Project created by QtCreator 2019-11-10T17:55:43
#
#-------------------------------------------------

QT += core gui
QT += multimedia
QT += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ctbrpgsca
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES *= QT_USE_QSTRINGBUILDER

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17
CONFIG += resources_big
CONFIG += qtquickcompiler

SOURCES += \
        ability.cpp \
        actor.cpp \
        arenawidget.cpp \
        clickablelabel.cpp \
        costume.cpp \
        demolib.cpp \
        itemsmodel.cpp \
        main.cpp \
        play.cpp \
        role.cpp \
        scene.cpp \
        skillsmodel.cpp \
        state.cpp \
        targetsmodel.cpp

HEADERS += \
        ability.h \
        actor.h \
        arenawidget.h \
        clickablelabel.h \
        costume.h \
        demolib.h \
        itemsmodel.h \
        play.h \
        role.h \
        scene.h \
        skillsmodel.h \
        state.h \
        targetsmodel.h

FORMS +=

CONFIG += mobility
MOBILITY = 

QMAKE_CXXFLAGS += -O3 #-fdeclspec

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += sprites.qrc
