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
#include <QTextEdit>
#include <QLayout>
#include <QWidget>
#include <QLabel>
#include "clickablelabel.h"
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
        #define SPR_FALL 3
        #define SPR_RISE 4
        #define SPR_ACT 5
        #define SPR_CAST 6
        #define POS_LEFT "l"
        #define POS_RIGHT "r"
        #define SPR_EXT "gif"
        #define FLAG_AUTOMATIC 1
        #define FLAG_AI_TURN 2
    public:
        bool isAiTurn() const;
        bool isAutomatic() const;

        ArenaWidget& afterAct();
        ArenaWidget& enableControls(bool const enable);

        explicit ArenaWidget(QWidget* parent = nullptr);

        ArenaWidget(QWidget* parent, QRect location, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events, int const surprise, int const mInit);

        ~ArenaWidget();

        ArenaWidget& operator()(QRect location, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events, int const surprise, int const mInit);
    protected:
        struct ActorSprite : QObject
        {
        public:
            ActorSprite& playActor(int const spr);
            ActorSprite& playSkill(QString& sprPath);

            ActorSprite(int const index, Actor& actor, QWidget* const widget, QRect size, ArenaWidget& arena, QString& pos);

            ~ActorSprite();
        protected:
            Actor* actor;
            ArenaWidget* arena;
            QMovie* actorMovie,* skillMovie;
            ClickableLabel* skillLabel;
            QLabel* actorLabel;
            QString pos;
            int spr;

            friend class ArenaWidget;
        };

        ArenaWidget& prepareTargetBox(QVector<Actor*>& players);
        ArenaWidget& prepareSkillsBox(QVector<Ability*>& skills);
        ArenaWidget& prepareItemsBox(QMap<Ability*, int>& items);
        inline ArenaWidget& recheckTargeting(int const trgIndex, int const skillIndex, int const itemIndex);
        ArenaWidget& setAutomatic(bool const automatic);
        ArenaWidget& setAiTurn(bool const aiTurn);

        ArenaWidget& operator()(QRect& location, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events,
                                int const surprise, int const mInit, bool const doScene);

        int sprRuns, flags;
        ActorSprite* sprites[SPR_SIZE];

        QWidget* ctrWidget,* actWidget;
        QLayout* ctrLayout,* actLayout,* mainLayout;
        QPushButton* actBtn,* useBtn,* fleeBtn,* autoBtn;
        QStringList skillsList, targetList,* itemsList;
        QComboBox* skillsBox,* itemsBox,* targetBox;
        QLabel* infoTxt,* arenaImg;
        QTextEdit* actionsTxt;

    signals:

    };

}

#endif // ARENAWIDGET_H
