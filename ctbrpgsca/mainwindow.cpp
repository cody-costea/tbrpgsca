#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "arenawidget.h"

#include <QGridLayout>

using namespace tbrpgsca;

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    DemoLib(),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString ret;
    QVector<QVector<Actor*>*> parties;
    parties.append(&(this->getPlayers()));
    parties.append(this->getEnemies().at(0));
    //ui->arenaWidget->operator()(ret, parties, nullptr, nullptr, -1, 0);
    ui->arenaWidget->operator()(ret, parties, nullptr, nullptr, 0, 0);
}

MainWindow::~MainWindow()
{
    delete ui;
}
