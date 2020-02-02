#include "mainwindow.h"
#include "arenawidget.h"
#include <QApplication>
#include <QOpenGLWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QtOpenGL/QGL>
#include <QGraphicsProxyWidget>
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
    int const width = size.width(), height = size.height();
    QRect geometry(0, 0, (width / 2), (height / 2));*/
    //QRect geometry(0, 0, 1280, 800);
    QRect geometry(0, 0, 800, 480);
    //QRect geometry(0, 0, 640, 480);
    //QRect geometry(0, 0, 320, 240);
    ArenaWidget arena(nullptr, geometry, ret, parties, nullptr, 0, 0);
    //arena.setGeometry(geometry);
    QGraphicsProxyWidget* arenaProxy = scene.addWidget(&arena);
    QGraphicsView view(&scene);
    //arenaProxy->setScale(1.5f);
    arenaProxy->update();
    /*int const width = geometry.width(), height = geometry.height();
    view.setFixedWidth(width + width / 3);
    view.setFixedHeight(height + height / 3);*/
    /*view.fitInView(geometry, Qt::KeepAspectRatio);
    view.setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers | QGL::DirectRendering)));
    view.setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    view.update();*/
    view.show();

    return a.exec();
}
