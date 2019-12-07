/*
Copyright (C) AD 2013-2019 Claudiu-Stefan Costea

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "scene.h"

using namespace tbrpgsca;

bool Scene::actorAgiComp(const Actor& a, const Actor& b)
{
    return (a.agi > b.agi);
}

Scene& Scene::endTurn(QString& ret)
{
    Scene& scene = *this;
    int current = scene.current;
    QVector<Actor*>& players = *(scene.players);
    int playersSize = players.size();
    Actor* crActor = players[current];
    crActor->actions--;
    while (crActor->actions < 1)
    {
        int mInit = scene.mInit;
        if (mInit > 0)
        {
            //int next = current;
            //Actor* nxActor = crActor;
            int cInit = crActor->init - mInit;
            crActor->init = cInit;
            do
            {
                for (int i = 0; i < playersSize; i++)
                {
                    Actor* iPlayer = players[i];
                    if (iPlayer->hp > 0)
                    {
                        int iInit = iPlayer->init + iPlayer->agi;
                        iPlayer->init = iInit;
                        if (iInit > cInit)
                        {
                            cInit = iInit;
                            crActor = iPlayer;
                            current = i;
                        }
                    }
                }
            }
            while (crActor->init < mInit);
            //crActor = nxActor;
            //current = next;
        }
        else
        {
            do
            {
                if (++current == playersSize)
                {
                    current = 0;
                }
                crActor = players[current];
            }
            while (crActor->hp < 1);
        }
        crActor->actions = crActor->mActions;
        crActor->applyStates(ret, false);
    }
    QMap<Ability*, int>* regSkills = crActor->skillsRgTurn;
    if (regSkills != nullptr)
    {
        QMap<Ability*, int>* skillsQty = crActor->skillsCrQty;
        if (skillsQty == nullptr)
        {
            skillsQty = new QMap<Ability*, int>();
            crActor->skillsCrQty = skillsQty;
        }
        for (Ability* skill : regSkills->keys())
        {
            int skillMaxQty = skill->mQty;
            int skillCrQty = skillsQty->value(skill, skillMaxQty);
            if (skillCrQty < skillMaxQty)
            {
                int skillRgTurn = regSkills->value(skill, 0);
                if (skillRgTurn == skill->rQty)
                {
                    skillsQty->operator[](skill) = skillCrQty + 1;
                    regSkills->operator[](skill) = 0;
                }
                else
                {
                    regSkills->operator[](skill) = skillRgTurn + 1;
                }
            }
        }
    }
    QVector<SceneAct*>* events = scene.events;
    if (events != nullptr && events->size() > 1)
    {
        auto event = events->at(2);
        if (event != nullptr && event(scene, ret) && crActor->isAutomated())
        {
            scene.playAi(ret);
        }
    }
    scene.current = current;
    return scene;
}

Actor& Scene::getGuardian(Actor& user, Actor& target, const Ability& skill) const
{
    if (!user.isRanged() || (skill.isOnlyMelee() && !skill.isRanged()))
    {
        int pos = -1;
        int side = target.side;
        Actor* fGuard = nullptr,* lGuard = nullptr;
        Actor** guardPos = &fGuard;
        QVector<Actor*>& party = *(this->parties[side]);
        int pSize = party.size();
        for (int i = 0; i < pSize; i++)
        {
            Actor* const guardian = party[i];
            if (guardian == &target)
            {
                if (fGuard == nullptr || i == pSize - 1)
                {
                    break;
                }
                else
                {
                    pos = i;
                    guardPos = &lGuard;
                    continue;
                }
            }
            else if ((fGuard == nullptr || pos != -1) && guardian->isGuarding())
            {
                (*guardPos) = guardian;
            }
        }
        if (fGuard != nullptr && lGuard != nullptr)
        {
            return *((pos < (pSize / 2)) ? fGuard : lGuard);
        }
    }
    return target;
}

Scene& Scene::execute(QString& ret, Actor& user, Actor* target, Ability& ability, bool const applyCosts)
{
    Scene& scene = *this;
    bool healing = ability.mHp < 0;
    if ((healing && ability.isReviving()) || target->hp > 0)
    {
        ability.execute(ret, user, *target, applyCosts);
        if ((!healing) && target != &user && target->hp > 0 && target->isCountering() && target->isGuarding())
        {
            //TODO: counter.execute(ret, user, target, false);
        }
    }
    return scene;
}

Scene& Scene::perform(QString& ret, Actor& user, Actor& target, Ability& ability, bool const item)
{
    Scene& scene = *this;
    QVector<SceneAct*>* events = scene.events;
    if (events != nullptr && events->size() > 0)
    {
        auto event = events->at(1);
        if (event != nullptr && event(scene, ret))
        {
            return scene;
        }
    }
    if (ability.isRanged() && ability.targetsAll())
    {
        bool applyCosts = true;
        int pSize = scene.players->size();
        bool sideTarget = ability.targetsSide();
        bool noSelfTarget = !ability.targetsSelf();
        for (int i = 0; i < pSize; i++)
        {
            Actor* trg = players->at(i);
            if (sideTarget && noSelfTarget && trg->side == user.side)
            {
                continue;
            }
            else if (trg == &user)
            {
                if (noSelfTarget)
                {
                    continue;
                }
                else
                {
                    ability.execute(ret, user, user, applyCosts);
                }
            }
            else
            {
                scene.execute(ret, user, trg, ability, applyCosts);
            }
            applyCosts = false;
        }
    }
    else if (ability.targetsSide())
    {
        int side = ability.targetsSelf() ? user.side : target.side;
        QVector<Actor*>& party = *(scene.parties[side]);
        int pSize = party.size();
        for (int i = 0; i < pSize; i++)
        {
            scene.execute(ret, user, party[i], ability, i == 0);
        }
    }
    else
    {
        if (&user == &target || ability.targetsSelf())
        {
            ability.execute(ret, user, user, true);
        }
        else
        {
            scene.execute(ret, user, &(scene.getGuardian(user, target, ability)), ability, true);
        }
    }
    if (item)
    {
        QMap<Ability*, int>* items = user.items;
        if (items != nullptr)
        {
            items->operator[](&ability) = items->value(&ability, 1) - 1;
        }
    }
    return scene;
}

Scene::Scene(QString& ret, const QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events, int const surprise, int const mInit)
{
    int partiesSize = parties.size();
    assert(partiesSize > 1);
    Scene& scene = *this;
    scene.mInit = mInit;
    scene.events = events;
    scene.parties = parties;
    QVector<Actor*>& players = *(scene.players);
    for (int i = 0; i < partiesSize; i++)
    {
        bool aiPlayer = i > 0;
        bool surprised = (surprise == i);
        QVector<Actor*>& party = *(parties[i]);
        int pSize = party.size();
        for (int j = 0; j < pSize; j++)
        {
            Actor& player = *(party[j]);
            player.actions = surprised ? 0 : player.mActions;
            if (aiPlayer)
            {
                player.setAiPlayer(true);
            }
            player.side = i;
        }
        players.append(party);
    }
    std::sort(players.begin(), players.end(), Scene::actorAgiComp);
    scene.endTurn(ret);
}
