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
    parties.append(&(this->getPlayers()));
    //ui->arenaWidget->operator()(ret, parties, nullptr, nullptr, -1, 0);
    //QGridLayout* layout = new QGridLayout(this);
    ArenaWidget& arena = *(new ArenaWidget(this));
    arena(ret, parties, nullptr, nullptr, -1, 0);
    //layout->addWidget(&arena);
    this->layout()->addWidget(&arena);
    /*QLabel* l = new QLabel;
    this->layout()->addWidget(l);
    l->setText("TESTING!");
    QMovie* spr = new QMovie();
    spr->setFileName(":/sprites/gif/Hero/bt_l_act.gif");
    l->setMovie(spr);
    QSize s(128, 128);
    spr->setScaledSize(s);
    spr->start();*/
}

MainWindow::~MainWindow()
{
    delete ui;
}
