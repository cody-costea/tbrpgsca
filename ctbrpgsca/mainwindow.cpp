#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "arenawidget.h"

#include <QGridLayout>

using namespace tbrpgsca;

void MainWindow::resizeEvent(QResizeEvent* const event)
{
    //this->arena->resizeScene(event->size(), &(event->oldSize()));
    QWidget::resizeEvent(event);
}

MainWindow::MainWindow(ArenaWidget* const arena, QWidget* parent) :
    QMainWindow(parent)/*,
    DemoLib()*/
    //, ui(new Ui::MainWindow)
{
    this->arena = arena;
    /*(QString ret;
    ui->setupUi(this);
    QRect location(0, 0, 640, 480);
    QVector<QVector<Actor*>*> parties;
    parties.append(&(this->getPlayers()));
    parties.append(this->getEnemies().at(0));
    ui->arenaWidget->operator()(location, ret, parties, nullptr, -1, 0);*/
    //ui->arenaWidget->operator()(ret, parties, nullptr, 0, 0);
}

MainWindow::~MainWindow()
{
    //delete ui;
}
