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
#ifndef SCENE_H
#define SCENE_H

#include "actor.h"

#include <QVector>


namespace tbrpgsca
{

    class Scene
    {
        #define STATUS_DEFEAT -2
        #define STATUS_RETREAT -1
        #define STATUS_ONGOING 0
        #define STATUS_VICTORY 1
        #define EVENT_BEGIN_SCENE 0
        #define EVENT_BEFORE_ACT 1
        #define EVENT_AFTER_ACT 2
        #define EVENT_NEW_TURN 3
        #define EVENT_END_SCENE 4
        #define MAX_INITIATIVE 50
    public:
        typedef bool* SceneAct(Scene& scene);

        static QString EscapeTxt;
        static QString VictoryTxt;
        static QString PerformsTxt;
        static QString FallenTxt;
        static QString FailTxt;

        QString escape();
        QString playAi(QString& ret);
        QString setNext(QString& ret, bool const endTurn);
        QString perform(QString& ret, Ability& ability, Actor& target, bool const item);

        int getAiSkill(Ability& defSkill, bool const nRestore) const;
        int getGuardian(Actor& target, Ability& skill) const;

        inline Actor* getPartyPlayer(int party, int player) const;
        inline bool hasPartyPlayer(int party, Actor* player) const;
        inline int getPartyPlayersSize(int party) const;
        inline int getPartiesSize() const;

        inline Actor* getOrderedPlayer(int n) const;
        inline bool hasOrderedPlayer(Actor* player) const;
        inline int getOrderedPlayersSize() const;

        inline Ability* getLastAbility() const;
        inline int getFirstTarget() const;
        inline int getLastTarget() const;
        inline int getCurrent() const;
        inline int getStatus() const;

        Scene(QVector<Actor*>& parties, QVector<SceneAct>* const events, int const surprise);
    private:
        Ability* lastAbility;
        QVector<SceneAct>* events;
        int current, surprise, fTarget, lTarget, status;
        QVector<QVector<Actor*>> parties;
        QVector<Actor*> players;
    };

}

#endif // SCENE_H
