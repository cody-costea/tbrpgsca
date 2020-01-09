#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace tbrpgsca;

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    DemoLib(),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
