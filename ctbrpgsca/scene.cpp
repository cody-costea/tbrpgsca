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

Actor& Scene::getGuardian(Actor& target, const Ability& skill) const
{
    if (!skill.isRanged())
    {
        int side = target.side;
        //Actor* guardian = &target
        QVector<Actor*>& party = *(this->parties[side]);
        int pos = party.indexOf(&target);
        int pSize = party.size() - 1;
        if (pos > 0 && pos < pSize)
        {
            Actor* fGuard = nullptr,* lGuard = nullptr;
            for (int i = 0; i < pos; i++)
            {
                Actor* guardian = party[i];
                if (guardian->isGuarding())
                {
                    fGuard = guardian;
                    break;
                }
            }
            for (int j = pSize; j > pos; j--)
            {
                Actor* guardian = party[j];
                if (guardian->isGuarding())
                {
                    lGuard = guardian;
                    break;
                }
            }
            if (fGuard != nullptr && lGuard != nullptr)
            {
                return *((pos > pSize / 2) ? lGuard : fGuard);
            }
        }
    }
    return target;
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
