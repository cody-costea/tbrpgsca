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
#include <QMediaPlaylist>
#include <QMediaPlayer>
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
#define USE_TEMPLATE 1

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
        #define FLAG_AI_TURN 8
        #define FLAG_AUTOMATIC 4
        #define FLAG_END_TURN 16

        #define TRG_SPR_DELAY -1//387

    #if USE_TEMPLATE
        #define SPR_ACTION this
        #define SPRITE_ACT &arena
    #else
    private:
        SpriteAct* _spr_act = nullptr;
        #define SPRITE_ACT static_cast<SpriteAct*>(arena._spr_act)
        #define SPR_ACTION static_cast<SpriteAct*>(_spr_act)
    #endif

        PROP_FLAG(ArenaWidget, AiTurn, FLAG_AI_TURN, public, public)
        PROP_FLAG(ArenaWidget, Automatic, FLAG_AUTOMATIC, public, public)
        PROP_FLAG(ArenaWidget, EndTurn, FLAG_END_TURN, public, public)
    public:
        void enableControls(bool const enable);

        void resizeEvent(QResizeEvent* const event) override;

        void resizeScene(const QSize& newSize, const QSize* const oldSize);

        explicit ArenaWidget(QWidget* const parent = nullptr);

        ArenaWidget(QWidget* parent, QSize size, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneRun*>* const events,
                    QString backImage, QString songName, int const surprise, int const mInit);

        ~ArenaWidget();

        void operator()(QSize size, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneRun*>* const events,
                                QString backImage, QString songName, int const surprise, int const mInit);
    protected:
        struct ActorSprite : QObject
        {
        public:
            void playActor(int const _spr);
            void playSkill(QString& sprPath);

            void relocate(QRect& location);

            ActorSprite(int const index, Actor& actor, QWidget* const widget, QRect& size, ArenaWidget& arena, QString& pos);

            ~ActorSprite();
        protected:
            Actor* _actor;
            ArenaWidget* _arena;
            QMovie* _actor_mov,* _skill_mov;
            ClickableLabel* _skill_lbl;
            QLabel* _actor_lbl;
            QString _pos;
            int _spr: 4;

            friend class ArenaWidget;
        };

        void afterAct();
        void afterPlay();
        void prepareItemsBox(Actor& actor);
        void prepareTargetBox(bool const freeMemory);
        Actor* getPlayerFromTargetBox(int const index);
        void prepareSkillsBox(Actor& actor, QVector<const Ability*>& skills);
        void recheckTargeting(int const trgIndex, int const skillIndex, int const itemIndex);

        void operator()(QSize& size, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneRun*>* const events,
                                QString& backImage, QString& arenaSong, int const surprise, int const mInit, bool const doScene);
    #if USE_TEMPLATE
        bool operator()(Scene& scene, Actor* const user, const Ability* const ability, bool const revive,
                       Actor* const target, const Ability* const counter);
    #endif

        QString* _ret_str;
        QLayout* _ctr_lyt,* _main_lyt;
        signed int _spr_runs: 16, _trg_count: 16;
        QPushButton* _act_btn,* _use_btn,* _flee_btn,* _auto_btn;
        QComboBox* _skills_box,* _items_box,* _target_box;
        QWidget* _ctr_wdt,* _act_wdt;
        TargetsModel* _targets_mdl;
        QLabel* _info_txt,* _back_img;
        QMediaPlayer* _ability_snd;
        QVBoxLayout* _act_lyt;
        QTextEdit* _acts_txt;
        Actor* _trg_actor;

    signals:

        friend class TargetsModel;
        friend class Ability;
        friend class Actor;
        friend class Scene;
        friend class Role;

    };

}

#endif // ARENAWIDGET_H
