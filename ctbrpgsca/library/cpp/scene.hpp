/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef SCENE_HPP
#define SCENE_HPP

#include "actor.hpp"
#include "play.hpp"

#include <QVector>
#include <QStringBuilder>

namespace tbrpgsca
{

    class Scene : public Play
    {
#define USE_TARGET_LIST 0
#define ALLOW_NO_GUARDS 0
#define ALLOW_COVERING 0

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
        #define MIN_ROUND -134217728
        #define MAX_ROUND 134217727

        PROP_FLAG_GET(hasCovers, FLAG_HAS_COVERS, public)
        PROP_FLAG_GET(usesGuards, FLAG_USE_GUARDS, public)
        PROP_FLAG_SET_ALL(Scene, UseGuards, FLAG_USE_GUARDS, protected, usesGuards)
        PROP_FLAG_SET_ALL(Scene, HasCovers, FLAG_HAS_COVERS, protected, hasCovers)
    public:
        typedef std::function<bool(Scene& scene, QString* const ret)> SceneRun;
        typedef std::function<bool(Scene& scene, Actor* const user, const Ability* const ability, bool const revive,
                                   Actor* const target, const Ability* const counter)> SpriteAct;

        inline static const QString PerformsTxt = TR_TXT_SCENE_PERFORMS;

        template <typename SpriteRun>
        void endTurn(QString& ret, const SpriteRun* const spriteRun, Actor* const actor);

        template <typename SpriteRun>
        void perform(QString& ret, const SpriteRun* const spriteRun, Actor& user, Actor& target, const Ability& ability, bool const item);

        template <typename SpriteRun>
        void playAi(QString& ret, const SpriteRun* const spriteRun, Actor& player);

        void checkStatus(QString& ret);
        void escape(QString& ret);

        bool canTarget(Actor& user, const Ability& ability, Actor& target);

        int getAiSkill(Actor& user, QVector<const Ability*>& skills, int const index, bool const nRestore) const;
        Actor* getGuardian(Actor& user, Actor* target, const Ability& skill) const;

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

        const Ability* getLastAbility() const;
        Actor* getCurrentPlayer() const;
        int getCurrentParty() const;
        int getCurrent() const;
        int getStatus() const;

        template <typename SpriteRun>
        void operator()(QString& ret, QVector<QVector<Actor*>*>& parties, const SpriteRun* const actorRun, QVector<SceneRun*>* const events,
                        bool const useGuards, int const surprise, int const mInit);

        template <typename SpriteRun>
        Scene(QString& ret, QVector<QVector<Actor*>*>& parties, const SpriteRun* const actorRun, QVector<SceneRun*>* const events,
              bool const useGuards, int const surprise, int const mInit);

        Scene();

        ~Scene();
    protected:
        CmpsPtr<Ability> _last_ability;
        QVector<SceneRun*>* _events;
#if USE_BIT_FIELDS
        signed int _current: 8, _original: 8, _f_target: 8, _l_target: 8, _surprise: 2, _status: 2, _m_init: 28;
#else
        signed int _current, _original, _f_target, _l_target, _surprise, _status, _m_init;
#endif
        QVector<QVector<Actor*>*> _parties;
        QVector<Actor*>* _players;
#if USE_TARGET_LIST
        QVector<Actor*>* _targets;
#endif
        //SpriteRun* _actor_run;
        Actor* _cr_actor;

        void agiCalc();
        void resetTurn(Actor& actor);

        template <typename SpriteRun>
        void execute(QString& ret, const SpriteRun* const actorEvent, Actor& user, Actor* target, const Ability& ability, bool const applyCosts, bool const ko);

        friend class Actor;
        friend class Ability;
        friend class ArenaWidget;
        friend class TargetsModel;
        friend class Costume;
        friend class State;
        friend class Role;
    };

}

#endif // SCENE_HPP
