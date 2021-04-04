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
#define USE_TARGET_LIST 0
#define ALLOW_NO_GUARDS 1
#define ALLOW_COVERING 1

        #define MIN_ROUND -134217728
        #define MAX_ROUND 134217727

        Q_OBJECT
        QML_ELEMENT
        PROP_FLAG_GET(hasCovers, Attribute::HAS_COVERS, inline, public)
        PROP_FLAG_GET(usesGuards, Attribute::USE_GUARDS, inline, public)
        PROP_FLAG_SET_ALL(Scene, HasCovers, Attribute::HAS_COVERS, inline, protected, hasCovers)
        PROP_FLAG_SET_ALL(Scene, UseGuards, Attribute::USE_GUARDS, inline, protected, usesGuards)
        //PROP_FIELD(Scene, ActText, actText, QString*, inline, public, protected, sceneData()._ret)
        PROP_FIELD(Scene, Parties, parties, QVector<QVector<Actor*>*>, inline, public, protected, sceneData()._parties)

        Q_PROPERTY(bool usesGuards READ usesGuards WRITE setUseGuards NOTIFY usesGuardsChanged)
        Q_PROPERTY(bool hasCovers READ hasCovers WRITE setHasCovers NOTIFY hasCoversChanged)
    public:
        enum Events
        {
            ENDING = 4,
            BEGINNING = 0,
            BEFORE_ACT = 2,
            AFTER_ACT = 3,
            NEW_TURN = 1
        };
        Q_ENUM(Events)

        enum Status
        {
            DEFEAT = -2,
            RETREAT = -1,
            ONGOING = 0,
            VICTORY = 1
        };
        Q_ENUM(Status)

        enum Attribute {
            USE_GUARDS = 1,
            HAS_COVERS = 2
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

        Q_INVOKABLE int getAiSkill(Actor* const user, QList<Ability>* const skills, int const index, bool const nRestore) const;
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
            this->operator()(ret, &this->sceneMutData()._parties, NIL, NIL, useGuards, surprise, mInit);
        }

        void operator()(QString& ret, QVector<QVector<Actor*>*>* parties, SpriteAct* const actorRun, QVector<SceneRun*>* const events,
                          bool const useGuards, int const surprise, int const mInit);

        Scene(QString& ret, QVector<QVector<Actor*>*>& parties, SpriteAct* const actorRun, QVector<SceneRun*>* const events,
              bool const useGuards, int const surprise, int const mInit, QObject* const parent = NIL);

        Scene(QString&& ret, QVector<QVector<Actor*>*>&& parties, SpriteAct* const actorRun, QVector<SceneRun*>* const events,
              bool const useGuards, int const surprise, int const mInit, QObject* const parent = NIL);

        explicit Scene(QObject* const parent = NIL);

        Scene(const Scene&& scene);

        Scene(const Scene& scene);

        virtual ~Scene();

    signals:
        void spriteAct(Actor* const user, Ability* const ability, bool const revive,
                       Actor* const target, Ability* const counter);
        void ending(QString* const ret);
        void beginning(QString* const ret);
        void beforeAct(QString* const ret, Actor* const user, Actor* const target, Ability* const ability);
        void afterAct(QString* const ret, Actor* const user, Actor* const target, Ability* const ability);
        void newTurn(QString* const ret, Actor* const current, Actor* const previous);

    protected:
        class SceneSheet : public PlaySheet
        {
        public:
            virtual ~SceneSheet();

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

            explicit SceneSheet();

            friend class Actor;
            friend class Scene;
            friend class Ability;
            friend class ArenaWidget;
            friend class TargetsModel;
            friend class Costume;
            friend class Ailment;
            friend class Role;
        };

        inline SceneSheet& sceneMutData()
        {
            return (*static_cast<SceneSheet*>(this->_play_data.data()));
        }

        inline const SceneSheet& sceneData() const
        {
            return (*static_cast<const SceneSheet*>(this->_play_data.data()));
        }

        void execute(QString& ret, Actor& user, Actor* target, Ability& ability, bool const applyCosts, bool const ko);
        void resetTurn(Actor& actor);

        explicit Scene(QObject* const parent, SceneSheet* const dataPtr);

    protected slots:
        void agiCalc();
        void actorHpChanged(int const newValue, int const oldValue);

        friend class Actor;
        friend class Ability;
        friend class ArenaWidget;
        friend class TargetsModel;
        friend class Costume;
        friend class Ailment;
        friend class Role;
    };

}

Q_DECLARE_METATYPE(tbrpgsca::Scene)
Q_DECLARE_METATYPE(tbrpgsca::Scene*)
Q_DECLARE_INTERFACE(tbrpgsca::Scene, "com.codycostea.tbrpgsca.Scene")

Q_DECLARE_OPERATORS_FOR_FLAGS(tbrpgsca::Scene::Attributes)

#endif // SCENE_H
