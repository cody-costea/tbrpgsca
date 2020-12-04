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
#define USE_TARGET_LIST 1
#define ALLOW_NO_GUARDS 1
#define ALLOW_COVERING 1

        #define MIN_ROUND -134217728
        #define MAX_ROUND 134217727

        Q_OBJECT
        PROP_FLAG_GET(hasCovers, Attribute::HasCovers, inline, public)
        PROP_FLAG_GET(usesGuards, Attribute::UseGuards, inline, public)
        PROP_FLAG_SET_ALL(Scene, HasCovers, Attribute::HasCovers, inline, protected, hasCovers)
        PROP_FLAG_SET_ALL(Scene, UseGuards, Attribute::UseGuards, inline, protected, usesGuards)
        //PROP_FIELD(Scene, ActText, actText, QString*, inline, public, protected, _scene_data->_ret)
        PROP_FIELD(Scene, Parties, parties, QVector<QVector<Actor*>*>, inline, public, protected, _scene_data->_parties)

        Q_PROPERTY(bool usesGuards READ usesGuards WRITE setUseGuards NOTIFY usesGuardsChanged)
        Q_PROPERTY(bool hasCovers READ hasCovers WRITE setHasCovers NOTIFY hasCoversChanged)
    public:
        enum Events
        {
            Ending = 4,
            Beginning = 0,
            BeforeAct = 2,
            AfterAct = 3,
            NewTurn = 1
        };
        Q_ENUM(Events)

        enum Status
        {
            Defeat = -2,
            Retreat = -1,
            Ongoing = 0,
            Victory = 1
        };
        Q_ENUM(Status)

        enum Attribute {
            UseGuards = 1,
            HasCovers = 2
        };
        Q_DECLARE_FLAGS(Attributes, Attribute)
        Q_FLAG(Attributes)

        typedef std::function<void(QString* const ret)> SceneRun;
        typedef std::function<void(Actor* const user, Ability* const ability, bool const revive,
                                   Actor* const target, Ability* const counter)> SpriteAct;

        static QString EscapeTxt;
        static QString VictoryTxt;
        static QString PerformsTxt;
        static QString FallenTxt;
        static QString FailTxt;

        inline static bool actorAgiComp(const Actor* const a, const Actor* const b)
        {
            return (a->agility() > b->agility());
        }

        Q_INVOKABLE void playAi(QString* const ret, Actor* const player);
        Q_INVOKABLE void perform(QString* const ret, Actor* const user, Actor* const target, Ability* const ability, bool const item);
        Q_INVOKABLE void checkStatus(QString* const ret);
        Q_INVOKABLE void endTurn(QString* const ret);
        Q_INVOKABLE void escape(QString* const ret);

        Q_INVOKABLE bool canTarget(Actor* const user, Ability* const ability, Actor* const target);

        Q_INVOKABLE int getAiSkill(Actor* const user, QVector<Ability*>* const skills, int const index, bool const nRestore) const;
        Q_INVOKABLE Actor* getGuardian(Actor* const user, Actor* target, Ability* const skill) const;

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

        //Q_INVOKABLE Ability* getLastAbility() const;
        Q_INVOKABLE Actor* getCurrentPlayer() const;
        Q_INVOKABLE int getCurrentParty() const;
        Q_INVOKABLE int getCurrent() const;
        Q_INVOKABLE int getStatus() const;

        Q_INVOKABLE inline void initialize(QString& ret, bool const useGuards, int const surprise, int const mInit)
        {
            this->operator()(ret, &this->_scene_data->_parties, NIL, NIL, useGuards, surprise, mInit);
        }

        void operator()(QString& ret, QVector<QVector<Actor*>*>* parties, SpriteAct* const actorRun, QVector<SceneRun*>* const events,
                          bool const useGuards, int const surprise, int const mInit);

        Scene(QString& ret, QVector<QVector<Actor*>*>& parties, SpriteAct* const actorRun, QVector<SceneRun*>* const events,
              bool const useGuards, int const surprise, int const mInit, QObject* const parent = NIL);

        Scene(QObject* const parent = NIL);

        Scene(const Scene& scene);

        ~Scene();

    signals:
        void spriteAct(Actor* const user, Ability* const ability, bool const revive,
                       Actor* const target, Ability* const counter);
        void ending(QString* const ret);
        void beginning(QString* const ret);
        void beforeAct(QString* const ret, Actor* const user, Actor* const target, Ability* const ability);
        void afterAct(QString* const ret, Actor* const user, Actor* const target, Ability* const ability);
        void newTurn(QString* const ret, Actor* const current, Actor* const previous);

    protected:
        class SceneData : public QSharedData
        {
        public:
            ~SceneData();

        protected:
            //Ability* _last_ability;
            //QVector<SceneRun*>* _events;
            signed int _current, _original, _surprise: 2, _f_target, _l_target, _status: 2, _m_init: 28;
            QVector<QVector<Actor*>*> _parties;
            QVector<Actor*>* _players;
    #if USE_TARGET_LIST
            QVector<Actor*>* _targets;
    #endif
            //SpriteAct* _actor_run;
            Actor* _cr_actor;
            QString* _ret;

            friend class Actor;
            friend class Scene;
            friend class Ability;
            friend class ArenaWidget;
            friend class TargetsModel;
            friend class Costume;
            friend class State;
            friend class Role;
        };

        QSharedDataPointer<SceneData> _scene_data;

        void execute(QString& ret, Actor& user, Actor* target, Ability& ability, bool const applyCosts);
        void resetTurn(Actor& actor);

    protected slots:
        void agiCalc();
        void actorHpChanged(int const newValue, int const oldValue);

        friend class Actor;
        friend class Ability;
        friend class ArenaWidget;
        friend class TargetsModel;
        friend class Costume;
        friend class State;
        friend class Role;
    };

}

Q_DECLARE_OPERATORS_FOR_FLAGS(tbrpgsca::Scene::Attributes)

#endif // SCENE_H
