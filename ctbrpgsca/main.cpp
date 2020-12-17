#include <QApplication>
#include <QMainWindow>
#include <QScreen>
#include <time.h>

#include "library/cpp/demolib.hpp"
#include "library/cpp/arenawidget.hpp"

using namespace tbrpgsca;

#if USE_TEMPLATE
extern template void Scene::playAi(QString& ret, const ArenaWidget* const spriteRun, Actor& player);
extern template void Scene::endTurn(QString& ret, const ArenaWidget* const spriteRun, Actor* const actor);
extern template void Scene::perform(QString& ret, const ArenaWidget* const spriteRun, Actor& user, Actor& target, const Ability& ability, bool const item);
extern template void Scene::execute(QString& ret, const ArenaWidget* const actorEvent, Actor& user, Actor* const target, const Ability& ability, bool const applyCosts);
extern template void Scene::operator()(QString& ret, QVector<QVector<Actor*>*>& parties, const ArenaWidget* const actorRun, QVector<SceneRun*>* const events,
                            bool const useGuards, int const surprise, int const mInit);
extern template Scene::Scene(QString& ret, QVector<QVector<Actor*>*>& parties, const ArenaWidget* const actorRun, QVector<SceneRun*>* const events,
                  bool const useGuards, int const surprise, int const mInit);
extern template void Ability::execute(QString& ret, Scene* const scene, const ArenaWidget* const spriteRun, Actor& user, Actor* target, bool const applyCosts) const;
extern template void Actor::applyDmgRoles(QString& ret, Scene* const scene, const ArenaWidget* const actorEvent);
extern template void Actor::applyStates(QString* const ret, Scene* const scene, const ArenaWidget* const spriteRun, bool const consume);
extern template void Actor::setCurrentHp(QString* const ret, Scene* const scene, const ArenaWidget* const actorEvent, int const hp, bool const survive);
extern template void Role::damage(QString& ret, Scene* const scene, const ArenaWidget* const spriteRun, Actor* const absorber, Actor& target, int dmg, bool const percent) const;
#endif

extern template void Scene::playAi(QString& ret, const Scene::SpriteAct* const spriteRun, Actor& player);
extern template void Scene::endTurn(QString& ret, const Scene::SpriteAct* const spriteRun, Actor* const actor);
extern template void Scene::perform(QString& ret, const Scene::SpriteAct* const spriteRun, Actor& user, Actor& target, const Ability& ability, bool const item);
extern template void Scene::execute(QString& ret, const Scene::SpriteAct* const actorEvent, Actor& user, Actor* const target, const Ability& ability, bool const applyCosts);
extern template void Scene::operator()(QString& ret, QVector<QVector<Actor*>*>& parties, const Scene::SpriteAct* const actorRun, QVector<SceneRun*>* const events,
                                bool const useGuards, int const surprise, int const mInit);
extern template Scene::Scene(QString& ret, QVector<QVector<Actor*>*>& parties, const Scene::SpriteAct* const actorRun, QVector<SceneRun*>* const events,
                      bool const useGuards, int const surprise, int const mInit);
extern template void Ability::execute(QString& ret, Scene* const scene, const Scene::SpriteAct* const spriteRun, Actor& user, Actor* target, bool const applyCosts) const;
extern template void Actor::applyDmgRoles(QString& ret, Scene* const scene, const Scene::SpriteAct* const actorEvent);
extern template void Actor::applyStates(QString* const ret, Scene* const scene, const Scene::SpriteAct* const spriteRun, bool const consume);
extern template void Actor::setCurrentHp(QString* const ret, Scene* const scene, const Scene::SpriteAct* const actorEvent, int const hp, bool const survive);
extern template void Role::damage(QString& ret, Scene* const scene, const Scene::SpriteAct* const spriteRun, Actor* const absorber, Actor& target, int dmg, bool const percent) const;

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
