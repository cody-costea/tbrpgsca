/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef ARENAWIDGET_H
#define ARENAWIDGET_H

#include <QMovie>
#include <QComboBox>
#include <QPushButton>
#include <QWidget>
#include <QLabel>
#include "scene.h"

namespace tbrpgsca
{

    class ArenaWidget : public QWidget, public Scene
    {
        Q_OBJECT
        #define SPR_SIZE 8
        #define SPR_IDLE 0
        #define SPR_KO 1
        #define SPR_HIT 2
        #define SPR_FALLEN 3
        #define SPR_RISEN 4
        #define SPR_ACT 5
        #define SPR_CAST 6
        #define POS_LEFT 0
        #define POS_RIGHT 1
        #define SPR_EXT "gif"
    public:
        explicit ArenaWidget(QWidget* parent = nullptr);

        ArenaWidget(QWidget* parent, QString& ret, QVector<QVector<Actor*>*>& parties, ActorAct* const actorEvent,
                    QVector<SceneAct*>* const events, int const surprise, int const mInit);

        ~ArenaWidget();

        ArenaWidget& operator()(QString& ret, QVector<QVector<Actor*>*>& parties, ActorAct* const actorEvent,
                                QVector<SceneAct*>* const events, int const surprise, int const mInit);
    protected:
        struct ActorSprite : QObject
        {
        public:
            void play(int const spr, int const pos);

            ActorSprite(Actor* const actor, QLabel* const label, ArenaWidget* const arena);

            ~ActorSprite();
        protected:
            Actor* actor;
            ArenaWidget* arena;
            QMovie* movie;
            QLabel* label;
            int spr, pos;

            friend class ArenaWidget;
        };        

        ArenaWidget& operator()(QString& ret, QVector<QVector<Actor*>*>& parties, ActorAct* const actorEvent,
                                QVector<SceneAct*>* const events, int const surprise, int const mInit, bool const doScene);

        int sprRuns;
        ActorSprite* sprites[SPR_SIZE];
        QPushButton* actBtn,* useBtn,* fleeBtn,* autoBtn;
        QLabel* infoTxt,* actionsTxt;
        QComboBox* skills,* items;

    signals:

    };

}

#endif // ARENAWIDGET_H
