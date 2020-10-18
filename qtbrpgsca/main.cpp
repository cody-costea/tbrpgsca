#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <demolib.h>

using namespace tbrpgsca;

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    /*DemoLib* demoLib = new DemoLib();
    SceneAct* scene = new SceneAct("", "", demoLib->PARTY, 4, false, demoLib->ENEMY, 4, true, 0);
    engine.rootContext()->setContextProperty("arena", scene);*/
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
