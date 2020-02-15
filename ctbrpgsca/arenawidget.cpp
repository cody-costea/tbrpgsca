/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "arenawidget.h"

#include <QGridLayout>
#include <QComboBox>
#include <QDebug>

using namespace tbrpgsca;

inline bool ArenaWidget::isAiTurn() const
{
    return (this->flags & FLAG_AI_TURN) == FLAG_AI_TURN;
}

inline bool ArenaWidget::isAutomatic() const
{
    return (this->flags & FLAG_AUTOMATIC) == FLAG_AUTOMATIC;
}

inline ArenaWidget& ArenaWidget::setAiTurn(const bool aiTurn)
{
    ArenaWidget& arena = *this;
    int const flags = arena.flags;
    if (aiTurn != ((flags & FLAG_AI_TURN) == FLAG_AI_TURN))
    {
        arena.flags = flags ^ FLAG_AI_TURN;
    }
    return arena;
}

inline ArenaWidget& ArenaWidget::setAutomatic(const bool automatic)
{
    ArenaWidget& arena = *this;
    int const flags = arena.flags;
    if (automatic != ((flags & FLAG_AUTOMATIC) == FLAG_AUTOMATIC))
    {
        arena.flags = flags ^ FLAG_AUTOMATIC;
    }
    return arena;
}

ArenaWidget::ActorSprite& ArenaWidget::ActorSprite::playSkill(QString& sprName)
{
    ActorSprite& actSprite = *this;
    QMovie& movie = *(actSprite.skillMovie);
    movie.stop();
    movie.setFileName(QString(":/sprites/%1/abilities/%2.%1").arg(SPR_EXT, sprName));
    movie.jumpToFrame(0);    
    ++(actSprite.arena->sprRuns);
    movie.start();
    return actSprite;
}

ArenaWidget::ActorSprite& ArenaWidget::ActorSprite::playActor(int const spr)
{
    ActorSprite& actSprite = *this;
    QMovie& movie = *(actSprite.actorMovie);
    Actor& sprActor = *(actSprite.actor);
    if (actSprite.spr != spr || sprActor.isShapeShifted())
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
        movie.stop();
        actSprite.spr = spr;
        movie.setFileName(QString(":/sprites/%1/%2/bt_%3_%4.%1").arg(SPR_EXT, *(sprActor.sprite), actSprite.pos, s));
        /*movie.setFileName(QString(":/sprites/%1/%2/bt_%3_%4.%1").arg(SPR_EXT, *(sprActor.sprite), (actSprite.arena->surprise == 0
            ? (sprActor.oldSide == 0 ? POS_RIGHT : POS_LEFT) : (sprActor.oldSide == 0 ? POS_LEFT : POS_RIGHT)), s));*/
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

ArenaWidget::ActorSprite::ActorSprite(int const index, Actor& actor, QWidget* const widget, QRect location, ArenaWidget& arena, QString& pos)
{
    this->spr = -1;
    this->pos = pos;
    this->actor = &actor;
    this->arena = &arena;
    QLabel* const actorLabel = new QLabel(widget);
    ClickableLabel* skillLabel = new ClickableLabel(actorLabel);
    QMovie* const actorMovie = new QMovie(),* skillMovie = new QMovie();
    actorMovie->setCacheMode(QMovie::CacheAll);
    skillMovie->setCacheMode(QMovie::CacheAll);
    auto run = [&arena]()
    {
       if ((--(arena.sprRuns)) < 1)
       {
           QString ret;
           //arena.endTurn(ret, arena.crActor);
           Actor& crActor = *(arena.crActor);
           if (arena.isAutomatic() || crActor.isAiPlayer() || crActor.isConfused() || crActor.isEnraged())
           {
               arena.setAiTurn(true).playAi(ret, crActor).endTurn(ret, arena.crActor);
               arena.actionsTxt->append(ret);
           }
           else
           {
               arena.setAiTurn(false).enableControls(true).afterAct();
           }
       }
    };
    actorLabel->setScaledContents(true);
    skillLabel->setScaledContents(true);
    connect(actorMovie, &QMovie::finished, run);
    connect(skillMovie, &QMovie::finished, run);
    connect(skillLabel, &ClickableLabel::clicked, [&arena, index]()
    {
        QComboBox& targetBox = *(arena.targetBox);
        if (targetBox.currentIndex() == index)
        {
            arena.actBtn->click();
        }
        else
        {
            targetBox.setCurrentIndex(index);
        }
    });
    int const width = location.width(), height = location.height();
    if (width != 128 || height != 128)
    {
        QSize size(width, height);
        actorMovie->setScaledSize(size);
        skillMovie->setScaledSize(size);
    }
    skillLabel->setGeometry(0, 0, width, height);
    actorLabel->setGeometry(location);
    actorLabel->setMovie(actorMovie);
    skillLabel->setMovie(skillMovie);
    this->actorMovie = actorMovie;
    this->actorLabel = actorLabel;
    this->skillMovie = skillMovie;
    this->skillLabel = skillLabel;
    actor.extra = this;
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
    arena.skillsBox->setEnabled(enable);
    if (enable)
    {
        //arena.recheckTargeting(arena.targetBox->currentIndex(), arena.skillsBox->currentIndex(), arena.itemsBox->currentIndex());
        arena.autoBtn->setEnabled(true);
    }
    else
    {
        arena.actBtn->setEnabled(false);
        arena.useBtn->setEnabled(false);
    }
    arena.itemsBox->setEnabled(enable);
    arena.fleeBtn->setEnabled(enable);
    return arena;
}

ArenaWidget& ArenaWidget::prepareTargetBox(QVector<QVector<Actor*>*>& parties)
{
    ArenaWidget& arena = *this;
    QComboBox* targetBox = arena.targetBox;
    targetBox->clear();
    for (QVector<Actor*>* const party : parties)
    {
        for (Actor* const actor : *party)
        {
            targetBox->addItem(actor->name);
        }
    }
    connect(targetBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&arena](int const i)
    {
        Actor& crActor = *(arena.crActor);
        QComboBox& skillsBox = *(arena.skillsBox);
        int const oldSkillIndex = skillsBox.currentIndex();
        bool const restore = i < arena.targetBox->count() / 2;
        int const newSkillIndex = arena.getAiSkill(crActor, *(crActor.aSkills), restore ? 1 : 0, restore);
        arena.skillsBox->setCurrentIndex(newSkillIndex);
        if (newSkillIndex == oldSkillIndex)
        {
            arena.recheckTargeting(i, newSkillIndex, arena.itemsBox->currentIndex());
        }
    });
    targetBox->setCurrentIndex(targetBox->count() / 2);
    return arena;
}

ArenaWidget& ArenaWidget::prepareSkillsBox(Actor& actor, QVector<Ability*>& skills)
{
    ArenaWidget& arena = *this;
    QComboBox& skillBox = *(arena.skillsBox);
    skillBox.clear();
    for (Ability* const skill : skills)
    {
        skillBox.addItem(skill->name);
    }
    QComboBox& targetBox = *(arena.targetBox);
    bool const restore = targetBox.currentIndex() < targetBox.count() / 2;
    skillBox.setCurrentIndex(arena.getAiSkill(actor, *(actor.aSkills), restore ? 1 : 0, restore));
    return arena;
}

ArenaWidget& ArenaWidget::prepareItemsBox(QMap<Ability*, int>& items)
{
    ArenaWidget& arena = *this;
    QComboBox& itemsBox = *(arena.itemsBox);
    itemsBox.clear();
    auto const iLast = items.cend();
    for (auto iIt = items.cbegin(); iIt != iLast; ++iIt)
    {
        itemsBox.addItem(iIt.key()->name % QString(iIt.value()));
    }
    return arena;
}

Actor* ArenaWidget::getPlayerFromTargetBox(int index)
{
    for (QVector<Actor*>* party : this->parties)
    {
        int const size = party->size();
        if (index < size)
        {
            return party->at(index);
        }
        else
        {
            index -= size;
        }
    }
    return nullptr;
}

inline ArenaWidget& ArenaWidget::recheckTargeting(int const trgIndex, int const skillIndex, int const itemIndex)
{
    ArenaWidget& arena = *this;
    if ((!arena.isAiTurn()) && trgIndex > -1)
    {
        Actor& crActor = *(arena.crActor);
        Actor* target = arena.getPlayerFromTargetBox(trgIndex);
        arena.trgActor = target;
        if (skillIndex > -1)
        {
            arena.actBtn->setEnabled(arena.canTarget(crActor, *(crActor.aSkills->at(skillIndex)), *target));
        }
        if (itemIndex > -1)
        {

        }
    }
    return arena;
}

ArenaWidget& ArenaWidget::afterAct()
{
    ArenaWidget& arena = *this;
    Actor& crActor = *(arena.crActor);
    arena.prepareSkillsBox(crActor, *(crActor.aSkills));
    QMap<Ability*, int>* crItems = crActor.items;
    if (crItems == nullptr)
    {
        arena.itemsList = nullptr;
    }
    else
    {
        arena.prepareItemsBox(*crItems);
    }
    return arena;
}

ArenaWidget& ArenaWidget::operator()(QRect size, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events, int const surprise, int const mInit)
{
    return this->operator()(size, ret, parties, events, surprise, mInit, true);
}

ArenaWidget& ArenaWidget::operator()(QRect& size, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events,
                                     int const surprise, int const mInit, bool const doScene)
{
    ArenaWidget& arena = *this;
    int sprLength, imgWidth, imgHeight;
    QLabel* arenaImg = new QLabel(this);
    QWidget* actWidget = new QWidget(this);
    QWidget* ctrWidget = new QWidget(this);
    arena.setGeometry(size);
    arena.flags = 0;
    QLayout* layout;
    {
        QLayout* ctrLayout;
        int const width = size.width();
        int const height = size.height();
        QLabel* infoTxt = new QLabel(this);
        QTextEdit* actionsTxt = new QTextEdit(this);
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
            int const actHeight = height / 5, infoHeight = height / 16;
            imgHeight = height - actHeight - infoHeight;
            arenaImg->setFixedHeight(imgHeight);
            actionsTxt->setFixedHeight(actHeight);
            infoTxt->setFixedHeight(infoHeight);
            infoTxt->setText("INFO TEXT TEST!!!");
        }
        if (height > width) //portrait
        {
            if (height < 480 || height > 1080)
            {
                sprLength = height / 5;
            }
            else
            {
                sprLength = 128;
            }
            ctrLayout = new QGridLayout(this);
            layout = new QVBoxLayout(this);
            imgWidth = width;
        }
        else //landscape
        {
            sprLength = width / 5;
            layout = new QHBoxLayout(this);
            ctrLayout = new QVBoxLayout(ctrWidget);
            ctrLayout->addWidget(autoBtn);
            ctrLayout->addWidget(skillsBox);
            ctrLayout->addWidget(actBtn);
            ctrLayout->addWidget(targetBox);
            ctrLayout->addWidget(useBtn);
            ctrLayout->addWidget(itemsBox);
            ctrLayout->addWidget(fleeBtn);
            ctrWidget->setFixedWidth(sprLength);
            ctrWidget->setLayout(ctrLayout);
            arena.ctrWidget = ctrWidget;
            arena.ctrLayout = ctrLayout;
            imgWidth = width - sprLength;
            sprLength = (imgHeight + imgHeight / 11) / 3;
        }
        int const btnHeight = height / 7;
        actWidget->setFixedWidth(imgWidth);
        //actionsTxt->setFixedWidth(imgWidth - sprWidth);
        //arenaImg->setFixedWidth(imgWidth);
        skillsBox->setFixedHeight(btnHeight);
        targetBox->setFixedHeight(btnHeight);
        itemsBox->setFixedHeight(btnHeight);
        actBtn->setFixedHeight(btnHeight);
        autoBtn->setFixedHeight(btnHeight);
        fleeBtn->setFixedHeight(btnHeight);
        useBtn->setFixedHeight(btnHeight);
        actionsTxt->setReadOnly(true);
        actLayout->addWidget(infoTxt);
        actLayout->addWidget(arenaImg);
        actLayout->addWidget(actionsTxt);
        actWidget->setLayout(actLayout);
        connect(autoBtn, &QPushButton::clicked, [&arena]()
        {
            QString ret;
            bool const automatic = arena.isAutomatic();
            if (automatic)
            {
                arena.setAutomatic(false).autoBtn->setEnabled(false);
            }
            else
            {
                Actor* const crActor = arena.crActor;
                arena.setAutomatic(true).playAi(ret, *crActor).endTurn(ret, crActor);
                arena.enableControls(false).actionsTxt->append(ret);
            }
        });
        connect(actBtn, &QPushButton::clicked, [&arena]()
        {
            QString ret;
            Actor* crActor = arena.crActor;
            arena.perform(ret, *crActor, *(arena.trgActor), *(crActor->aSkills->at(arena.skillsBox->currentIndex())), false);
            arena.enableControls(false);
            arena.endTurn(ret, crActor);
            arena.actionsTxt->append(ret);
        });
        connect(skillsBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&arena](int const i)
        {
            arena.recheckTargeting(arena.targetBox->currentIndex(), i, arena.itemsBox->currentIndex());
        });
        connect(itemsBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&arena](int const i)
        {
            arena.recheckTargeting(arena.targetBox->currentIndex(), arena.skillsBox->currentIndex(), i);
        });
    }
    layout->addWidget(ctrWidget);
    layout->addWidget(actWidget);
    arena.mainLayout = layout;
    arena.setLayout(layout);
    arena.sprRuns = 0;
    {
        int const sprSize = SPR_SIZE / 2;
        ActorSprite** sprites = arena.sprites;
        {
            int k = 0;
            int const pSize = parties.size(), sprFactor = sprLength / 3 + sprLength / 10, sprWidth = (sprLength / 2) + sprFactor + sprFactor / 3, sprHeight = imgHeight / 10,
                    sprDistance = (sprHeight * 2) + (sprHeight), xCentre = imgWidth / 2 - (sprLength / 2 + sprLength / 7), yCentre = imgHeight / 2 - sprLength / 2;
            for (int j = 0; j < pSize; ++j)
            {
                int x;
                QString pos;
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
                        Actor& actor = *(party[i]);
                        switch (j > 1 ? k : i)
                        {
                        case 0:
                        case 4:
                            spr = new ActorSprite(k, actor, arenaImg, QRect(xCentre - (sprFactor * x), yCentre - (sprHeight * x), sprLength, sprLength), arena, pos);
                            break;
                        case 1:
                        case 5:
                            spr = new ActorSprite(k, actor, arenaImg, QRect(xCentre - (sprWidth * x), yCentre - (sprDistance * x), sprLength, sprLength), arena, pos);
                            break;
                        case 2:
                        case 6:
                            spr = new ActorSprite(k, actor, arenaImg, QRect(xCentre - (sprWidth * x), yCentre - (sprHeight * -1 * x), sprLength, sprLength), arena, pos);
                            break;
                        case 3:
                        case 7:
                            spr = new ActorSprite(k, actor, arenaImg, QRect(xCentre - (sprFactor * x), yCentre - (sprDistance * -1 * x), sprLength, sprLength), arena, pos);
                            break;
                        }
                        spr->playActor(actor.hp > 0 ? SPR_IDLE : SPR_KO);
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
        if (&target != &user)
        {
            (static_cast<ActorSprite*>(user.extra))->playActor((ability.dmgType & DMG_TYPE_ATK) == DMG_TYPE_ATK ? SPR_ACT : SPR_CAST);
        }
        ActorSprite& targetSpr = *(static_cast<ActorSprite*>(target.extra));
        QString* spr = ability.sprite;
        if (spr != nullptr && spr->length() > 0)
        {
            targetSpr.playSkill(*spr);
        }
        targetSpr.playActor(target.hp > 0 ? (revive ? SPR_RISE : (counter == nullptr ? SPR_HIT : (((counter->dmgType & DMG_TYPE_ATK) == DMG_TYPE_ATK)
                                                                             ? SPR_ACT : SPR_CAST))) : SPR_FALL);
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
    arena.prepareTargetBox(parties);
    arena.afterAct();
    return arena;
}

ArenaWidget::ArenaWidget(QWidget* parent) : QWidget(parent), Scene()
{

}

ArenaWidget::ArenaWidget(QWidget* parent, QRect size, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events, int const surprise, int const mInit)
    : QWidget(parent), Scene(ret, parties, nullptr, events, surprise, mInit)
{
    this->operator()(size, ret, parties, events, surprise, mInit, false);
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
    QStringList* itemsList = this->itemsList;
    if (itemsList != nullptr)
    {
        delete itemsList;
    }
    for (auto spr : this->sprites)
    {
        if (spr != nullptr)
        {
            delete spr;
        }
    }
}
