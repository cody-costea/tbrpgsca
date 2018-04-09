#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <sceneact.h>
#include <QQmlContext>
#include <demolib.h>

using namespace qtbrpgsca;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    DemoLib* demoLib = new DemoLib();
    SceneAct scene("", demoLib->PARTY, 4, false, demoLib->ENEMY, 4, true, 0);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("arena", &scene);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
