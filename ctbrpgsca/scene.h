/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef SCENE_H
#define SCENE_H

#include "actor.h"

#include <QVector>
#include <QStringBuilder>

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
        #define MIN_ROUND INT_MIN
    public:
        typedef std::function<bool(Scene& scene, QString& ret)> SceneAct;
        typedef std::function<bool(Scene& scene, Actor& user, Ability& ability, bool const revive, Actor& target, Ability* counter)> ActorAct;

        static QString EscapeTxt;
        static QString VictoryTxt;
        static QString PerformsTxt;
        static QString FallenTxt;
        static QString FailTxt;

        static bool actorAgiComp(Actor* const a, Actor* const b);

        Scene& endTurn(QString& ret, Actor* actor);
        Scene& playAi(QString& ret, Actor& player);
        Scene& perform(QString& ret, Actor& user, Actor& target, Ability& ability, bool const item);
        Scene& checkStatus(QString& ret);
        Scene& escape(QString& ret);

        bool canTarget(Actor& user, Ability& ability, Actor& target);

        int getAiSkill(Actor& user, QVector<Ability*>& skills, int const index, bool const nRestore) const;
        Actor& getGuardian(Actor& user, Actor& target, Ability& skill) const;

        Actor& getPartyPlayer(int const party, int const player) const;
        bool hasPartyPlayer(int const party, Actor& player) const;
        int getPartyPlayersSize(int const party) const;
        int getPartiesSize() const;

        Actor& getOrderedPlayer(int const n) const;
        bool hasOrderedPlayer(Actor& player) const;
        int getOrderedPlayersSize() const;

        Actor& getTargetedPlayer(int const n) const;
        bool hasTargetedPlayer(Actor& player) const;
        int getTargetedPlayersSize() const;

        Ability* getLastAbility() const;
        Actor* getCurrentPlayer() const;
        int getCurrentParty() const;
        int getCurrent() const;
        int getStatus() const;

        Scene& operator()(QString& ret, QVector<QVector<Actor*>*>& parties, ActorAct* const actorEvent, QVector<SceneAct*>* const events, int const surprise, int const mInit);

        Scene(QString& ret, QVector<QVector<Actor*>*>& parties, ActorAct* const actorEvent, QVector<SceneAct*>* const events, int const surprise, int const mInit);

        Scene();

        ~Scene();
    protected:
        Ability* lastAbility;
        QVector<SceneAct*>* events;
        int current, oldCurrent, surprise, fTarget, lTarget, status, mInit;
        QVector<Actor*>* players,* targets;
        QVector<QVector<Actor*>*> parties;
        ActorAct* actorEvent;
        Actor* crActor;

        Scene& execute(QString& ret, Actor& user, Actor* target, Ability& ability, bool const applyCosts);
        void resetTurn(Actor& actor);
        void agiCalc();

        friend class Actor;
        friend class Ability;
        friend class ArenaWidget;
        friend class Costume;
        friend class State;
        friend class Role;
    };

}

#endif // SCENE_H
