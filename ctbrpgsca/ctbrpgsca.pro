#-------------------------------------------------
#
# Project created by QtCreator 2019-11-10T17:55:43
#
#-------------------------------------------------

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

lessThan(QT_MAJOR_VERSION, 6): QT += multimedia

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

CONFIG += c11 c++17 resources_big qtquickcompiler #objective_c

SOURCES += \
        library/c/trability.c \
        library/c/tractor.c \
        library/c/trcostume.c \
        library/c/trplay.c \
        library/c/trrole.c \
        library/c/trscene.c \
        library/c/trstate.c \
        library/cpp/ability.cpp \
        library/cpp/actor.cpp \
        library/cpp/arenawidget.cpp \
        library/cpp/clickablelabel.cpp \
        library/cpp/costume.cpp \
        library/cpp/itemsmodel.cpp \
        library/cpp/play.cpp \
        library/cpp/role.cpp \
        library/cpp/scene.cpp \
        library/cpp/skillsmodel.cpp \
        library/cpp/state.cpp \
        library/cpp/targetsmodel.cpp \
        main.cpp

HEADERS += \
        library/c/trability.h \
        library/c/tractor.h \
        library/c/trcostume.h \
        library/c/trplay.h \
        library/c/trrole.h \
        library/c/trscene.h \
        library/c/trstate.h \
        library/cpp/ability.hpp \
        library/cpp/actor.hpp \
        library/cpp/arenawidget.hpp \
        library/cpp/clickablelabel.hpp \
        library/cpp/costume.hpp \
        library/cpp/itemsmodel.hpp \
        library/cpp/play.hpp \
        library/cpp/role.hpp \
        library/cpp/scene.hpp \
        library/cpp/skillsmodel.hpp \
        library/cpp/state.hpp \
        library/cpp/targetsmodel.hpp \
        library/translations.h

#equals(QT_ARCH, arm): LIBS += library/dbc/arm32/*.obj \
#                              library/rust/arm32/*.obj
#equals(QT_ARCH, arm64-v8a): LIBS += library/dbc/arm64/*.obj \
#                                    library/rust/arm64/*.obj
#equals(QT_ARCH, x86_64): LIBS += library/dbc/x64/*.obj \
#                                 library/rust/x64/*.obj
#equals(QT_ARCH, i386): LIBS += library/dbc/x86/*.obj \
#                               library/rust/x86/*.obj

#LIBS += -lobjc -framework Foundation

#OBJECTIVE_HEADERS += \

#OBJECTIVE_SOURCES += \

FORMS +=

CONFIG += mobility
MOBILITY = 

QMAKE_CXXFLAGS += -O3 #-fms-extensions #-fdeclspec

ANDROID_ABIS = armeabi-v7a arm64-v8a x86 x86_64

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

android {
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    DISTFILES += android/AndroidManifest.xml
}
RESOURCES += sprites.qrc
