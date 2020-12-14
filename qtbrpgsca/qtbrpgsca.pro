TEMPLATE = app

QT += qml quick core gui multimedia opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtbrpgsca
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

CONFIG += c++17 resources_big qtquickcompiler qmltypes

QML_IMPORT_NAME = com.codycostea.tbrpgsca
QML_IMPORT_MAJOR_VERSION = 1

SOURCES += \
    ability.cpp \
        actor.cpp \
        itemsmodel.cpp \
        costume.cpp \
        demolib.cpp \
        main.cpp \
        play.cpp \
        role.cpp \
        state.cpp \
        scene.cpp \
        skillsmodel.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    ability.h \
        actor.h \
        itemsmodel.h \
        costume.h \
        demolib.h \
        play.h \
        role.h \
        state.h \
        scene.h \
        skillsmodel.h

DISTFILES += \
    tbrpgsca_ro_RO.ts

CONFIG += mobility
MOBILITY =

QMAKE_CXXFLAGS += -O3 -fdeclspec

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LANGUAGES = ro_RO
# parameters: var, prepend, append
defineReplace(prependAll) {
    for(a,$$1):result += $$2$${a}$$3
    return($$result)
}

TRANSLATIONS = $$prependAll(LANGUAGES, $$PWD/tbrpgsca_, .ts)

TRANSLATIONS_FILES =

qtPrepareTool(LRELEASE, lrelease)
for(tsfile, TRANSLATIONS) {
    qmfile = $$shadowed($$tsfile)
    qmfile ~= s,.ts$,.qm,
    qmdir = $$dirname(qmfile)
    !exists($$qmdir) {
        mkpath($$qmdir)|error("Aborting.")
    }
    command = $$LRELEASE -removeidentical $$tsfile -qm $$qmfile
    system($$command)|error("Failed to run: $$command")
    TRANSLATIONS_FILES += $$qmfile
}

android
{
    #ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    #DISTFILES += android/AndroidManifest.xml
    trfiles.path = /assets
    trfiles.files = $$OUT_PWD/tbrpgsca_ro_RO.qm
    INSTALLS += trfiles
}
