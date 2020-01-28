#include "mainwindow.h"
#include "arenawidget.h"
#include <QApplication>
#include <QOpenGLWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QtOpenGL/QGL>
#include <QScreen>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    /*MainWindow w;
    w.show();*/
    QString ret;
    DemoLib demoLib;
    QGraphicsScene scene;
    QVector<QVector<Actor*>*> parties;
    parties.append(&(demoLib.getPlayers()));
    parties.append(demoLib.getEnemies().at(0));
    /*const QSize& size = QApplication::screens().at(0)->availableSize();
    QRect geometry(0, 0, size.width(), size.height());
    scene.setSceneRect(geometry);*/
    ArenaWidget arena(nullptr, ret, parties, nullptr, -1, 0);
    //arena.setGeometry(geometry);
    scene.addWidget(&arena);
    QGraphicsView view(&scene);
    /*view.fitInView(geometry, Qt::KeepAspectRatioByExpanding);
    view.setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers | QGL::DirectRendering)));
    view.setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    view.update();*/
    view.show();

    return a.exec();
}
