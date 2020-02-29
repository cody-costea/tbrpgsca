#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "arenawidget.h"
#include "demolib.h"
#include <QMainWindow>

using namespace tbrpgsca;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow//, public DemoLib
{
    Q_OBJECT

public:
    void resizeEvent(QResizeEvent* const event) override;

    explicit MainWindow(ArenaWidget* const arena, QWidget* parent = nullptr);
    ~MainWindow();

private:
    ArenaWidget* arena;
    //Ui::MainWindow* ui;
};


#endif // MAINWINDOW_H
