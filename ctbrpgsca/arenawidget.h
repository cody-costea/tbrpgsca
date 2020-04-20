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
#include <QResizeEvent>
#include <QTextEdit>
#include <QLayout>
#include <QWidget>
#include <QLabel>

#include "clickablelabel.h"
#include "targetsmodel.h"
#include "skillsmodel.h"
#include "itemsmodel.h"
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
        #define FLAG_END_TURN 4
    public:
        inline bool isAiTurn() const;
        inline bool isAutomatic() const;
        inline bool isEndTurn() const;

        ArenaWidget& afterAct();
        ArenaWidget& enableControls(bool const enable);

        void resizeEvent(QResizeEvent* const event) override;

        ArenaWidget& resizeScene(const QSize& newSize, const QSize* const oldSize);

        explicit ArenaWidget(QWidget* const parent = nullptr);

        ArenaWidget(QWidget* parent, QSize size, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events, int const surprise, int const mInit);

        ~ArenaWidget();

        ArenaWidget& operator()(QSize size, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events, int const surprise, int const mInit);
    protected:
        struct ActorSprite : QObject
        {
        public:
            ActorSprite& playActor(int const spr);
            ActorSprite& playSkill(QString& sprPath);

            ActorSprite& relocate(QRect& location);

            ActorSprite(int const index, Actor& actor, QWidget* const widget, QRect& size, ArenaWidget& arena, QString& pos);

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
        ArenaWidget& prepareItemsBox(Actor& actor);
        ArenaWidget& prepareTargetBox(bool const freeMemory);
        ArenaWidget& prepareSkillsBox(Actor& actor, QVector<Ability*>& skills);
        ArenaWidget& recheckTargeting(int const trgIndex, int const skillIndex, int const itemIndex);
        inline ArenaWidget& setAutomatic(bool const automatic);
        inline ArenaWidget& setEndTurn(bool const endTurn);
        inline ArenaWidget& setAiTurn(bool const aiTurn);
        Actor* getPlayerFromTargetBox(int const index);

        ArenaWidget& operator()(QSize& size, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events,
                                int const surprise, int const mInit, bool const doScene);

        QString* returnTxt;
        int sprRuns, flags, trgCount;
        QWidget* ctrWidget,* actWidget;
        QLayout* ctrLayout,* mainLayout;
        QPushButton* actBtn,* useBtn,* fleeBtn,* autoBtn;
        QComboBox* skillsBox,* itemsBox,* targetBox;
        TargetsModel* targetsModel;
        QLabel* infoTxt,* arenaImg;
        QVBoxLayout* actLayout;
        QTextEdit* actionsTxt;
        Actor* trgActor;

    signals:

        friend class TargetsModel;

    };

}

#endif // ARENAWIDGET_H
