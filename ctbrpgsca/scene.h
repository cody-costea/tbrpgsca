/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef SCENE_H
#define SCENE_H

#include "actor.h"
#include "play.h"

#include <QVector>
#include <QStringBuilder>

namespace tbrpgsca
{

    class Scene : public Play
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
        #define FLAG_USE_GUARDS 1
        #define FLAG_HAS_COVERS 2
        #define ALLOW_NO_GUARDS 1
        #define ALLOW_COVERING 1
        #define MIN_ROUND INT_MIN

        PROP_FLAG_GET(hasCovers, FLAG_HAS_COVERS, public)
        PROP_FLAG_GET(usesGuards, FLAG_USE_GUARDS, public)
        PROP_FLAG_SET_ALL(Scene, UseGuards, FLAG_USE_GUARDS, protected, usesGuards)
        PROP_FLAG_SET_ALL(Scene, HasCovers, FLAG_HAS_COVERS, protected, hasCovers)
    public:
        typedef std::function<bool(Scene& scene, QString* const ret)> SceneRun;
        typedef std::function<bool(Scene& scene, Actor* const user, Ability* const ability, bool const revive,
                                   Actor* const target, Ability* const counter)> SpriteCall;

        static QString EscapeTxt;
        static QString VictoryTxt;
        static QString PerformsTxt;
        static QString FallenTxt;
        static QString FailTxt;

        static bool actorAgiComp(Actor* const a, Actor* const b);

        template <typename SpriteRun>
        void endTurn(QString& ret, Actor* const actor, SpriteRun* const spriteRun);

        template <typename SpriteRun>
        void perform(QString& ret, Actor& user, Actor& target, Ability& ability, bool const item, SpriteRun* const spriteRun);

        template <typename SpriteRun>
        void playAi(QString& ret, Actor& player, SpriteRun* const spriteRun);

        void checkStatus(QString& ret);
        void escape(QString& ret);

        bool canTarget(Actor& user, Ability& ability, Actor& target);

        int getAiSkill(Actor& user, QVector<Ability*>& skills, int const index, bool const nRestore) const;
        Actor* getGuardian(Actor& user, Actor* target, Ability& skill) const;

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

        template <typename SpriteRun>
        void operator()(QString& ret, QVector<QVector<Actor*>*>& parties, SpriteRun* const actorRun, QVector<SceneRun*>* const events,
                          bool const useGuards, int const surprise, int const mInit);

        template <typename SpriteRun>
        Scene(QString& ret, QVector<QVector<Actor*>*>& parties, SpriteRun* const actorRun, QVector<SceneRun*>* const events,
              bool const useGuards, int const surprise, int const mInit);

        Scene();

        ~Scene();
    protected:
        Ability* _last_ability;
        QVector<SceneRun*>* _events;
        int _flags, _current, _original, _surprise, _f_target, _l_target, _status, _m_init;
        QVector<Actor*>* _players,* _targets;
        QVector<QVector<Actor*>*> _parties;
        //SpriteRun* _actor_run;
        Actor* _cr_actor;

        void agiCalc();
        void resetTurn(Actor& actor);

        template <typename SpriteRun>
        void execute(QString& ret, Actor& user, Actor* target, Ability& ability, bool const applyCosts, SpriteRun* const actorEvent);

        friend class Actor;
        friend class Ability;
        friend class ArenaWidget;
        friend class TargetsModel;
        friend class Costume;
        friend class State;
        friend class Role;
    };

}

#endif // SCENE_H
