/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "arenawidget.h"

#include <QGridLayout>

using namespace tbrpgsca;

void ArenaWidget::ActorSprite::play(int const spr, int const pos)
{
    if (this->spr != spr || this->pos != pos)
    {
        QString s;
        switch (spr)
        {
        case SPR_IDLE:
            s = "idle";
            break;
        case SPR_KO:
            s = "ko";
            break;
        case SPR_HIT:
            s = "hit";
        break;
        case SPR_FALLEN:
            s = "fallen";
            break;
        case SPR_RISEN:
            s = "restored";
            break;
        case SPR_ACT:
            s = "act";
            break;
        case SPR_CAST:
            s = "cast";
            break;
        default:
            return;
        }
        this->spr = spr;
        this->pos = pos;
        this->movie->setFileName(QString(":/sprites/%1/%2/bt_%3_%4.%1").arg(SPR_EXT, *(this->actor->sprite), pos == POS_LEFT ? "l" : "r", s));
    }
    ++(this->arena->sprRuns);
    this->movie->start();
}

ArenaWidget::ActorSprite::ActorSprite(Actor* const actor, QLabel* const label, ArenaWidget* const arena)
{
    this->actor = actor;
    this->arena = arena;
    QMovie* movie = new QMovie();
    connect(movie, &QMovie::finished, [arena]()
    {
        --(arena->sprRuns);
    });
    label->setMovie(movie);
    this->movie = movie;
    this->label = label;
    actor->extra = this;
}

ArenaWidget::ActorSprite::~ActorSprite()
{
    QMovie* movie = this->movie;
    this->movie = nullptr;
    delete movie;
}

ArenaWidget& ArenaWidget::operator()(QString& ret, QVector<QVector<Actor*>*>& parties, ActorAct* const actorEvent,
                                     QVector<SceneAct*>* const events, int const surprise, int const mInit)
{
    return this->operator()(ret, parties, actorEvent, events, surprise, mInit, true);
}

ArenaWidget& ArenaWidget::operator()(QString& ret, QVector<QVector<Actor*>*>& parties, ActorAct* const actorEvent,
                        QVector<SceneAct*>* const events, int const surprise, int const mInit, bool const doScene)
{
    ArenaWidget& arena = *this;
    if (doScene)
    {
        arena.Scene::operator()(ret, parties, actorEvent, events, surprise, mInit);
    }
    int sprWidth;
    arena.sprRuns = 0;
    int const width = arena.width();
    int const height = arena.height();
    QWidget* ctrWidget = new QWidget(this);
    QLayout* ctrLayout;
    {
        QLabel* infoTxt = new QLabel(this);
        QLabel* actionsTxt = new QLabel(this);
        QPushButton* actBtn = new QPushButton(this);
        QPushButton* autoBtn = new QPushButton(this);
        QPushButton* fleeBtn = new QPushButton(this);
        QPushButton* useBtn = new QPushButton(this);
        QComboBox* targetBox = new QComboBox(this);
        QComboBox* skillsBox = new QComboBox(this);
        QComboBox* itemsBox = new QComboBox(this);
        //bool portrait;
        if (height > width)
        {
            if (height < 640 || height > 1024)
            {
                sprWidth = height / 5;
            }
            else
            {
                sprWidth = 128;
            }
            ctrLayout = new QGridLayout(this);
            //portrait = true;
        }
        else
        {
            if (width < 640 || width > 1024)
            {
                sprWidth = width / 5;
            }
            else
            {
                sprWidth = 128;
            }
            ctrLayout = new QVBoxLayout(ctrWidget);
            ctrLayout->addWidget(autoBtn);
            ctrLayout->addWidget(skillsBox);
            ctrLayout->addWidget(actBtn);
            ctrLayout->addWidget(targetBox);
            ctrLayout->addWidget(useBtn);
            ctrLayout->addWidget(itemsBox);
            ctrLayout->addWidget(fleeBtn);
            //ctrLayout->setGeometry(QRect(0, 0, sprWidth, height));
            ctrWidget->setMaximumWidth(sprWidth);
            ctrWidget->setLayout(ctrLayout);
            arena.ctrWidget = ctrWidget;
            arena.ctrLayout = ctrLayout;
            //portrait = false;
        }
    }
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(ctrWidget);
    arena.mainLayout = layout;
    arena.setLayout(layout);
    {
        int const sprSize = SPR_SIZE / 2;
        ActorSprite** sprites = arena.sprites;
        //QVector<QVector<Actor*>*>& parties = par
        {
            int k = 0;
            int const pSize = parties.size();
            for (int j = 0; j < pSize; ++j)
            {
                QVector<Actor*>& party = *(parties[j]);
                int const sSize = party.size();
                for (int i = 0; i < sSize; ++i)
                {
                    if (i < sprSize)
                    {                        
                        QLabel* img = new QLabel(this);
                        ActorSprite* spr = new ActorSprite(party[i], img, this);
                        img->setGeometry(sprWidth * (i), sprWidth * (j + i), sprWidth, sprWidth);
                        spr->play(SPR_CAST, party[i]->side == 0 ? POS_LEFT : POS_RIGHT);
                        //layout->addWidget(img);
                        sprites[i] = spr;
                    }
                    else
                    {
                        sprites[i] = nullptr;
                    }
                    if ((++k) == SPR_SIZE)
                    {
                        goto after;
                    }
                }
            }
        }
    }
    after:
    return arena;
}

ArenaWidget::ArenaWidget(QWidget* parent) : QWidget(parent), Scene()
{

}

ArenaWidget::ArenaWidget(QWidget* parent, QString& ret, QVector<QVector<Actor*>*>& parties, ActorAct* const actorEvent,
                         QVector<SceneAct*>* const events, int const surprise, int const mInit)
    : QWidget(parent), Scene(ret, parties, actorEvent, events, surprise, mInit)
{
    this->operator()(ret, parties, actorEvent, events, surprise, mInit, false);
}

ArenaWidget::~ArenaWidget()
{
    delete this->actBtn;
    delete this->useBtn;
    delete this->fleeBtn;
    delete this->autoBtn;
    delete this->skillsBox;
    delete this->targetBox;
    delete this->itemsBox;
    delete this->infoTxt;
    delete this->actionsTxt;
    delete this->ctrLayout;
    delete this->ctrWidget;
    delete this->mainLayout;
    for (auto spr : this->sprites)
    {
        delete spr->label;
        delete spr;
    }
}
