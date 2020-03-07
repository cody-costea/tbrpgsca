#include "demolib.h"
#include "arenawidget.h"
#include <QApplication>
//#include <QOpenGLWidget>
//#include <QGraphicsScene>
//#include <QGraphicsView>
//#include <QtOpenGL/QGL>
//#include <QGraphicsProxyWidget>
#include <QStackedLayout>
#include <qmainwindow.h>
#include <QScreen>

using namespace tbrpgsca;

int main(int argc, char* argv[])
{
    QSize size;
    QString ret;
    DemoLib demoLib;
    QApplication a(argc, argv);
    QVector<QVector<Actor*>*> parties;
    parties.append(&(demoLib.getPlayers()));
    parties.append(demoLib.getEnemies().at(0));
    auto screens = QApplication::screens();
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
