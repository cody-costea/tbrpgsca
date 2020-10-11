#include "demolib.h"
#include "arenawidget.h"
#include <QApplication>
#include <QMainWindow>
#include <QScreen>
#include <time.h>

using namespace tbrpgsca;

/*extern template void Scene::endTurn<ArenaWidget>(QString& ret, Actor* const actor, ArenaWidget* const spriteRun);
extern template void Scene::playAi<ArenaWidget>(QString& ret, Actor& player, ArenaWidget* const spriteRun);
extern template void Scene::perform<ArenaWidget>(QString& ret, Actor& user, Actor& target, Ability& ability, bool const item, ArenaWidget* const spriteRun);
extern template void Scene::operator()<ArenaWidget>(QString& ret, QVector<QVector<Actor*>*>& parties, ArenaWidget* const actorRun, QVector<SceneRun*>* const events,
                                                    bool const useGuards, int const surprise, int const mInit);
extern template Scene::Scene(QString& ret, QVector<QVector<Actor*>*>& parties, ArenaWidget* const actorRun, QVector<SceneRun*>* const events,
                             bool const useGuards, int const surprise, int const mInit);*/


int main(int argc, char* argv[])
{
    srand(time(0));
    QSize size;//(240, 320);
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
    ArenaWidget arena(nullptr, size, ret, parties, nullptr, "", "", -1, 0);
    window.setCentralWidget(&arena);
    //window.showMaximized();
    window.showFullScreen();
    //window.show();
    return a.exec();
}
