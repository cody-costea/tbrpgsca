#include "demolib.h"
#include "arenawidget.h"
#include <QApplication>
#include <QMainWindow>
#include <QScreen>

using namespace tbrpgsca;

int main(int argc, char* argv[])
{
    QSize size;//(480, 800);
    QString ret;
    DemoLib demoLib;
    QApplication a(argc, argv);
    QVector<QVector<Actor*>*> parties;
    parties.append(&(demoLib.getPlayers()));
    parties.append(demoLib.getEnemies().at(0));
    const QList<QScreen*>& screens = QApplication::screens();
    if (screens.size() > 0)
    {
        const QSize& scrSize = screens.at(0)->size();
        size.setHeight(scrSize.height());
        size.setWidth(scrSize.width());
    }
    else
    {
        size.setWidth(800);
        size.setHeight(480);
    }
    QMainWindow window;
    ArenaWidget arena(nullptr, size, ret, parties, nullptr, -1, 0);
    window.setCentralWidget(&arena);
    //window.showMaximized();
    window.showFullScreen();
    return a.exec();
}
