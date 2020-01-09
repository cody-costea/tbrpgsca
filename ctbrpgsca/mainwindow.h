#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "demolib.h"
#include <QMainWindow>

using namespace tbrpgsca;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow, public DemoLib
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
};


#endif // MAINWINDOW_H
