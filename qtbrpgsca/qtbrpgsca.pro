TEMPLATE = app

QT += qml quick
CONFIG += c++17 resources_big qtquickcompiler

SOURCES += \
    ability.cpp \
	actor.cpp \
	arenawidget.cpp \
	clickablelabel.cpp \
	itemsmodel.cpp \
	costume.cpp \
	demolib.cpp \
	main.cpp \
	play.cpp \
	role.cpp \
	state.cpp \
	scene.cpp \
	skillsmodel.cpp \
	targetsmodel.cpp \
    sceneact.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    ability.h \
	actor.h \
	arenawidget.h \
	clickablelabel.h \
	itemsmodel.h \
	costume.h \
	demolib.h \
	play.h \
	role.h \
	state.h \
	scene.h \
	skillsmodel.h \
	targetsmodel.h \
    sceneact.h

DISTFILES +=

CONFIG += mobility
MOBILITY = 

QMAKE_CXXFLAGS += -O3 #-fdeclspec

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
