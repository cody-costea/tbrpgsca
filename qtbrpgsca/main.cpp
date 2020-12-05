#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>

#include "demolib.h"
#include "ability.h"
#include "costume.h"
#include "state.h"
#include "actor.h"
#include "scene.h"
#include "role.h"
#include "play.h"

using namespace tbrpgsca;

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    /*DemoLib* demoLib = new DemoLib();
    SceneAct* scene = new SceneAct("", "", demoLib->PARTY, 4, false, demoLib->ENEMY, 4, true, 0);
    engine.rootContext()->setContextProperty("arena", scene);*/

    REG_QML(Play)
    REG_QML(Role)
    REG_QML(Scene)
    REG_QML(Ability)
    REG_QML(Costume)
    REG_QML(State)
    REG_QML(Actor)

    QTranslator translator;
    if (
    #ifdef Q_OS_ANDROID
        translator.load(QString("assets:/%1").arg("tbrpgsca_ro_RO.qm"))
    #else
        translator.load("tbrpgsca_ro_RO.qm")
    #endif
        )
    {
        app.installTranslator(&translator);
    }

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
