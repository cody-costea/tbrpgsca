/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "arenawidget.h"

#include <QGridLayout>
#include <QDebug>

using namespace tbrpgsca;

ArenaWidget::ActorSprite& ArenaWidget::ActorSprite::playSkill(QString& sprName)
{
    ActorSprite& actSprite = *this;
    QMovie& movie = *(actSprite.skillMovie);
    /*if (movie == nullptr)
    {
        QLabel* label = actSprite.skillLabel;
        if (label == nullptr)
        {
            label = new QLabel(actSprite.actorLabel);
            actSprite.skillLabel = label;
        }
        movie = new QMovie();
        actSprite.skillMovie = movie;
    }*/
    movie.stop();
    movie.setFileName(QString(":/sprites/%1/abilities/%2.%1").arg(SPR_EXT, sprName));
    movie.jumpToFrame(0);
    movie.start();
    return actSprite;
}

ArenaWidget::ActorSprite& ArenaWidget::ActorSprite::playActor(int const spr, int const pos)
{
    ActorSprite& actSprite = *this;
    QMovie& movie = *(actSprite.actorMovie);
    if (actSprite.spr != spr || actSprite.pos != pos)
    {
        QString s;
        switch (spr)
        {
        case SPR_IDLE:
            s = "idle";
            //label->setPixmap(QPixmap(QString(":/sprites/%1/%2/bt_%3_%4.%1").arg(SPR_EXT, *(this->actor->sprite), pos == POS_LEFT ? "l" : "r", s)));
            //return;
            break;
        case SPR_KO:
            s = "ko";
            break;
        case SPR_HIT:
            s = "hit";
        break;
        case SPR_FALL:
            s = "fallen";
            break;
        case SPR_RISE:
            s = "restored";
            break;
        case SPR_ACT:
            s = "act";
            break;
        case SPR_CAST:
            s = "cast";
            break;
        default:
            return actSprite;
        }
        actSprite.spr = spr;
        actSprite.pos = pos;
        movie.stop();
        movie.setFileName(QString(":/sprites/%1/%2/bt_%3_%4.%1").arg(SPR_EXT, *(actSprite.actor->sprite), pos == POS_LEFT ? "l" : "r", s));
    }
    movie.jumpToFrame(0);
    if (spr == SPR_IDLE || spr == SPR_KO)
    {
        actSprite.actorLabel->update();
    }
    else
    {
        ++(actSprite.arena->sprRuns);
        movie.start();
    }
    return actSprite;
}

ArenaWidget::ActorSprite::ActorSprite(Actor* const actor, QWidget* const widget, QRect location, ArenaWidget& arena)
{
    this->spr = -1;
    this->pos = -1;
    this->actor = actor;
    this->arena = &arena;
    QMovie* const actorMovie = new QMovie(),* skillMovie = new QMovie();
    QLabel* const actorLabel = new QLabel(widget),* skillLabel = new QLabel(actorLabel);
    actorMovie->setCacheMode(QMovie::CacheAll);
    skillMovie->setCacheMode(QMovie::CacheAll);
    auto run = [&arena]()
    {
       if ((--(arena.sprRuns)) < 1)
       {
           QString ret;
           arena.endTurn(ret, arena.crActor);
           Actor& crActor = *(arena.crActor);
           if (crActor.isAiPlayer() || crActor.isConfused() || crActor.isEnraged())
           {
               arena.playAi(ret, crActor);
           }
           else
           {
               arena.enableControls(true);
           }
       }
    };
    connect(actorMovie, &QMovie::finished, run);
    connect(skillMovie, &QMovie::finished, run);
    skillLabel->setGeometry(0, 0, location.width(), location.height());
    actorLabel->setGeometry(location);
    actorLabel->setMovie(actorMovie);
    skillLabel->setMovie(skillMovie);
    this->actorMovie = actorMovie;
    this->actorLabel = actorLabel;
    this->skillMovie = skillMovie;
    this->skillLabel = skillLabel;
    actor->extra = this;
    actorLabel->show();
    skillLabel->show();
}

ArenaWidget::ActorSprite::~ActorSprite()
{
    QMovie* const actorMovie = this->actorMovie,* skillMovie = this->skillMovie;
    QLabel* const actorLabel = this->actorLabel,* skillLabel = this->skillLabel;
    this->actorLabel = nullptr;
    this->actorMovie = nullptr;
    this->skillLabel = nullptr;
    this->skillMovie = nullptr;
    delete actorLabel;
    delete actorMovie;
    delete skillLabel;
    delete skillMovie;
}

ArenaWidget& ArenaWidget::enableControls(bool const enable)
{
    ArenaWidget& arena = *this;
    arena.actBtn->setEnabled(enable);
    arena.autoBtn->setEnabled(enable);
    arena.skillsBox->setEnabled(enable);
    arena.itemsBox->setEnabled(enable);
    arena.fleeBtn->setEnabled(enable);
    arena.useBtn->setEnabled(enable);
    return arena;
}

ArenaWidget& ArenaWidget::operator()(QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events, int const surprise, int const mInit)
{
    return this->operator()(ret, parties, events, surprise, mInit, true);
}

ArenaWidget& ArenaWidget::operator()(QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events,
                                     int const surprise, int const mInit, bool const doScene)
{
    ArenaWidget& arena = *this;
    int sprWidth, imgWidth, imgHeight;
    QLabel* arenaImg = new QLabel(this);
    QWidget* actWidget = new QWidget(this);
    QWidget* ctrWidget = new QWidget(this);
    QLayout* layout;
    {
        QLayout* ctrLayout;
        int const width = arena.width();
        int const height = arena.height();
        QLabel* infoTxt = new QLabel(this);
        QLabel* actionsTxt = new QLabel(this);
        QPushButton* actBtn = new QPushButton(this);
        actBtn->setText(tr("Execute"));
        QPushButton* autoBtn = new QPushButton(this);
        autoBtn->setText(tr("Auto"));
        QPushButton* fleeBtn = new QPushButton(this);
        fleeBtn->setText(tr("Flee"));
        QPushButton* useBtn = new QPushButton(this);
        useBtn->setText(tr("Use"));
        QComboBox* targetBox = new QComboBox(this);
        QComboBox* skillsBox = new QComboBox(this);
        QComboBox* itemsBox = new QComboBox(this);
        QVBoxLayout* actLayout = new QVBoxLayout(this);
        arena.targetBox = targetBox;
        arena.skillsBox = skillsBox;
        arena.itemsBox = itemsBox;
        arena.arenaImg = arenaImg;
        arena.actionsTxt = actionsTxt;
        arena.infoTxt = infoTxt;
        arena.fleeBtn = fleeBtn;
        arena.autoBtn = autoBtn;
        arena.actBtn = actBtn;
        arena.useBtn = useBtn;
        {
            int const actHeight = height / 10;
            imgHeight = height - actHeight;
            arenaImg->setFixedHeight(imgHeight);
            actionsTxt->setFixedHeight(actHeight);
            actLayout->addWidget(arenaImg);
            actLayout->addWidget(actionsTxt);
            actWidget->setLayout(actLayout);
        }
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
            layout = new QVBoxLayout(this);
            imgWidth = width;
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
            layout = new QHBoxLayout(this);
            ctrLayout = new QVBoxLayout(ctrWidget);
            ctrLayout->addWidget(autoBtn);
            ctrLayout->addWidget(skillsBox);
            ctrLayout->addWidget(actBtn);
            ctrLayout->addWidget(targetBox);
            ctrLayout->addWidget(useBtn);
            ctrLayout->addWidget(itemsBox);
            ctrLayout->addWidget(fleeBtn);
            //ctrLayout->setGeometry(QRect(0, 0, sprWidth, height));
            ctrWidget->setFixedWidth(sprWidth);
            ctrWidget->setLayout(ctrLayout);
            arena.ctrWidget = ctrWidget;
            arena.ctrLayout = ctrLayout;
            imgWidth = width - sprWidth;
            //portrait = false;
        }
        int const btnHeight = height / 7;
        actWidget->setFixedWidth(imgWidth);
        actionsTxt->setFixedWidth(imgWidth);
        arenaImg->setFixedWidth(imgWidth);
        skillsBox->setFixedHeight(btnHeight);
        targetBox->setFixedHeight(btnHeight);
        itemsBox->setFixedHeight(btnHeight);
        actBtn->setFixedHeight(btnHeight);
        autoBtn->setFixedHeight(btnHeight);
        fleeBtn->setFixedHeight(btnHeight);
        useBtn->setFixedHeight(btnHeight);
    }
    layout->addWidget(ctrWidget);
    layout->addWidget(actWidget);
    arena.mainLayout = layout;
    arena.setLayout(layout);
    arena.sprRuns = 0;
    {
        int const sprSize = SPR_SIZE / 2;
        ActorSprite** sprites = arena.sprites;
        qDebug() << "arenaImg->width(): " << arenaImg->width();
        qDebug() << "sprWidth: " << sprWidth;
        //QVector<QVector<Actor*>*>& parties = par
        {
            int k = 0;
            int const pSize = parties.size(), xCentre = imgWidth / 3, yCentre = imgHeight / 3, sprFactor = sprWidth / 2;
            qDebug() << "sprFactor: " << sprFactor;
            qDebug() << "xCentre: " << xCentre;
            qDebug() << "yCentre: " << yCentre;
            for (int j = 0; j < pSize; ++j)
            {
                int x, pos;
                if (j == 0)
                {
                    if (surprise == 0)
                    {
                        x = -1;
                        pos = POS_RIGHT;
                    }
                    else
                    {
                        x = 1;
                        pos = POS_LEFT;
                    }
                }
                else
                {
                    if (surprise == 0)
                    {
                        x = 1;
                        pos = POS_LEFT;
                    }
                    else
                    {
                        x = -1;
                        pos = POS_RIGHT;
                    }
                }
                QVector<Actor*>& party = *(parties[j]);
                int const sSize = party.size();
                for (int i = 0; i < sSize; ++i)
                {
                    if (i < sprSize)
                    {
                        ActorSprite* spr;
                        Actor* const actor = party[i];
                        switch (j > 1 ? k : i)
                        {
                        case 0:
                        case 4:
                            spr = new ActorSprite(actor, arenaImg, QRect(xCentre - (sprFactor * x), yCentre - (sprFactor * x), sprWidth, sprWidth), arena);
                            break;
                        case 1:
                        case 5:
                            spr = new ActorSprite(actor, arenaImg, QRect(xCentre - ((sprWidth + sprFactor) * x), yCentre - (sprWidth * x), sprWidth, sprWidth), arena);
                            break;
                        case 2:
                        case 6:
                            spr = new ActorSprite(actor, arenaImg, QRect(xCentre - ((sprWidth + sprFactor) * x), yCentre - (sprFactor * -1 * x), sprWidth, sprWidth), arena);
                            break;
                        case 3:
                        case 7:
                            spr = new ActorSprite(actor, arenaImg, QRect(xCentre - (sprFactor * x), yCentre - (sprWidth * -1 * x), sprWidth, sprWidth), arena);
                            break;
                        }
                        //img->setGeometry(sprWidth * (i), sprWidth * (j + i), sprWidth, sprWidth);
                        if (actor->hp > 0)
                        {
                            spr->playActor(SPR_IDLE, pos);
                            //spr->play(SPR_IDLE, pos);
                        }
                        else
                        {
                            spr->playActor(SPR_KO, pos);
                            //spr->play(SPR_KO, pos);
                        }
                        //spr->play(actor->hp > 0 ? SPR_IDLE : SPR_KO, pos);
                        //actLayout->addWidget(img);
                        sprites[k] = spr;
                    }
                    if ((++k) == SPR_SIZE)
                    {
                        goto after;
                    }
                }
            }
            for (;k < SPR_SIZE; ++k)
            {
                sprites[k] = nullptr;
            }
        }
    }
    after:
    auto actorRun = new ActorAct([](Scene& scene, Actor& user, Ability& ability, bool const revive, Actor& target, Ability* counter) -> bool
    {
        //(static_cast<ArenaWidget&>(scene)).sprRuns = 0;
        (static_cast<ActorSprite*>(user.extra))->playActor((ability.dmgType & DMG_TYPE_ATK) == DMG_TYPE_ATK ? SPR_ACT : SPR_CAST, user.oldSide);
        ActorSprite& targetSpr = *(static_cast<ActorSprite*>(target.extra));
        QString* spr = ability.sprite;
        if (spr != nullptr && spr->length() > 0)
        {
            targetSpr.playSkill(*spr);
        }
        targetSpr.playActor(target.hp > 0 ? (revive ? SPR_RISE : (counter == nullptr ? SPR_HIT : (((counter->dmgType & DMG_TYPE_ATK) == DMG_TYPE_ATK)
                                                                             ? SPR_ACT : SPR_CAST))) : SPR_FALL, target.oldSide);
        return false;
    });
    if (doScene)
    {
        arena.Scene::operator()(ret, parties, actorRun, events, surprise, mInit);
    }
    else
    {
        arena.actorEvent = actorRun;
    }
    return arena;
}

ArenaWidget::ArenaWidget(QWidget* parent) : QWidget(parent), Scene()
{

}

ArenaWidget::ArenaWidget(QWidget* parent, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events, int const surprise, int const mInit)
    : QWidget(parent), Scene(ret, parties, nullptr, events, surprise, mInit)
{
    this->operator()(ret, parties, events, surprise, mInit, false);
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
    delete this->arenaImg;
    delete this->actionsTxt;
    delete this->ctrLayout;
    delete this->ctrWidget;
    delete this->actWidget;
    delete this->actLayout;
    delete this->mainLayout;
    delete this->actorEvent;
    for (auto spr : this->sprites)
    {
        if (spr != nullptr)
        {
            delete spr;
        }
    }
}
