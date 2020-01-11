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
    this->movie->setFileName(":/sprites/gif/Hero/bt_l_act.gif");
    this->movie->start();
}

ArenaWidget::ActorSprite::ActorSprite(Actor* actor, QMovie* movie)
{
    this->actor = actor;
    this->movie = movie;
    actor->setExtra(this);
}

ArenaWidget& ArenaWidget::operator()(QString& ret, QVector<QVector<Actor*>*>& parties, ActorAct* const actorEvent,
                        QVector<SceneAct*>* const events, int const surprise, int const mInit)
{
    ArenaWidget& arena = *this;
    //arena.Scene::operator()(ret, parties, actorEvent, events, surprise, mInit);
    bool const portrait = this->height() > this->width();
    QGridLayout* gridLayout = new QGridLayout(this);
    this->setLayout(gridLayout);
    {
        int const sprSize = SPR_SIZE / 2;
        ActorSprite** sprites = this->sprites;
        //QVector<QVector<Actor*>*>& parties = parties;
        {
            QVector<Actor*>& party = *(parties[0]);
            int const pSize = party.size();
            for (int i = 0; i < pSize; ++i)
            {
                sprites[i] = i < sprSize ? (new ActorSprite(party[i], new QMovie())) : nullptr;
            }
        }
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
                        QMovie* movie = new QMovie();
                        ActorSprite* spr = new ActorSprite(party[i], movie);
                        sprites[i] = spr;
                        QLabel* img = new QLabel();
                        movie->setScaledSize(QSize(128, 128));
                        img->setMovie(movie);
                        gridLayout->addWidget(img);
                        spr->play(0, 0);
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
    this->operator()(ret, parties, actorEvent, events, surprise, mInit);
}

ArenaWidget::~ArenaWidget()
{

}
