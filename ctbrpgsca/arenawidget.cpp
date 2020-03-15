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

inline bool ArenaWidget::isEndTurn() const
{
    return (this->flags & FLAG_END_TURN) == FLAG_END_TURN;
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

inline ArenaWidget& ArenaWidget::setEndTurn(const bool endTurn)
{
    ArenaWidget& arena = *this;
    int const flags = arena.flags;
    if (endTurn != ((flags & FLAG_END_TURN) == FLAG_END_TURN))
    {
        arena.flags = flags ^ FLAG_END_TURN;
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

ArenaWidget::ActorSprite& ArenaWidget::ActorSprite::relocate(QRect& location)
{
    ActorSprite& actSprite = *this;
    int const width = location.width(), height = location.height();
    {
        QSize size(width, height);
        actSprite.actorMovie->setScaledSize(size);
        actSprite.skillMovie->setScaledSize(size);
    }
    actSprite.skillLabel->setGeometry(0, 0, width, height);
    actSprite.actorLabel->setGeometry(location);
    return actSprite;
}

ArenaWidget::ActorSprite::ActorSprite(int const index, Actor& actor, QWidget* const widget, QRect& location, ArenaWidget& arena, QString& pos)
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
       int const sprRuns = arena.sprRuns - 1;
       if (sprRuns > -1)
       {
           arena.sprRuns = sprRuns;
           if (sprRuns == 0)
           {
               Actor* crActor;
               QString& ret = *(arena.returnTxt);
               {
                   if (arena.isEndTurn())
                   {
                       arena.setEndTurn(false);
                   }
                   else
                   {
                       crActor = arena.crActor;
                       QVector<Costume*>* usrRoles;
                       if (((usrRoles = crActor->dmgRoles) != nullptr) && usrRoles->size() > 0)
                       {
                           arena.setEndTurn(true);
                           arena.endTurn(ret, crActor);
                           return;
                       }
                       else
                       {
                           arena.endTurn(ret, crActor);
                       }
                   }
                   crActor = arena.crActor;
                   arena.actionsTxt->append(ret); ret.clear();
                   if (arena.isAutomatic() || crActor->side != 0 || crActor->isAiPlayer() || crActor->isConfused() || crActor->isEnraged())
                   {
                       arena.infoTxt->setText("");
                       arena.setAiTurn(true).playAi(ret, *crActor);
                   }
                   else
                   {
                       arena.setAiTurn(false).enableControls(true).afterAct();
                   }
               }
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
    actorLabel->setMovie(actorMovie);
    skillLabel->setMovie(skillMovie);
    this->actorMovie = actorMovie;
    this->actorLabel = actorLabel;
    this->skillMovie = skillMovie;
    this->skillLabel = skillLabel;
    this->relocate(location);
    //actor.extra = this;
    actorLabel->show();
    skillLabel->show();
}

ArenaWidget::ActorSprite::~ActorSprite()
{
    QMovie* const actorMovie = this->actorMovie,* skillMovie = this->skillMovie;
    this->actorLabel = nullptr;
    this->actorMovie = nullptr;
    this->skillLabel = nullptr;
    this->skillMovie = nullptr;
    delete actorMovie;
    delete skillMovie;
}

ArenaWidget& ArenaWidget::enableControls(bool const enable)
{
    ArenaWidget& arena = *this;
    QComboBox& skillsBox = *(arena.skillsBox);
    QComboBox& itemsBox = *(arena.itemsBox);
    skillsBox.setEnabled(enable);
    if (enable)
    {
        arena.recheckTargeting(arena.targetBox->currentIndex(), skillsBox.currentIndex(), itemsBox.currentIndex());
        arena.autoBtn->setEnabled(true);
    }
    else
    {
        arena.actBtn->setEnabled(false);
        arena.useBtn->setEnabled(false);
    }
    arena.fleeBtn->setEnabled(enable);
    itemsBox.setEnabled(enable);
    return arena;
}

ArenaWidget& ArenaWidget::prepareTargetBox(bool const freeMemory)
{
    ArenaWidget& arena = *this;
    QComboBox* targetBox = arena.targetBox;
    TargetsModel* targetsModel;
    if (freeMemory)
    {
        targetsModel = arena.targetsModel;
        if (targetsModel != nullptr)
        {
            delete targetsModel;
        }
    }
    targetsModel = new TargetsModel(arena);
    arena.targetBox->setModel(targetsModel);
    arena.targetsModel = targetsModel;
    connect(targetBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&arena](int const i)
    {
        Actor& crActor = *(arena.crActor);
        QComboBox& skillsBox = *(arena.skillsBox);
        int const oldSkillIndex = skillsBox.currentIndex();
        bool const restore = i < arena.targetBox->count() / 2;
        int const newSkillIndex = arena.getAiSkill(crActor, *(crActor.aSkills), restore ? 1 : 0, restore);
        arena.trgActor = arena.getPlayerFromTargetBox(i);
        skillsBox.setCurrentIndex(newSkillIndex);
        if (newSkillIndex == oldSkillIndex)
        {
            arena.recheckTargeting(i, newSkillIndex, arena.itemsBox->currentIndex());
        }
    });
    targetBox->setCurrentIndex(arena.trgCount / 2);
    return arena;
}

ArenaWidget& ArenaWidget::prepareSkillsBox(Actor& actor, QVector<Ability*>& skills)
{
    ArenaWidget& arena = *this;
    QComboBox& skillBox = *(arena.skillsBox);
    skillsBox->setModel(static_cast<SkillsModel*>(static_cast<void**>(actor.extra)[1]));
    QComboBox& targetBox = *(arena.targetBox);
    bool const restore = targetBox.currentIndex() < targetBox.count() / 2;
    skillBox.setCurrentIndex(arena.getAiSkill(actor, skills, restore ? 1 : 0, restore));
    return arena;
}

ArenaWidget& ArenaWidget::prepareItemsBox(Actor& actor)
{
    ArenaWidget& arena = *this;
    QComboBox& itemsBox = *(arena.itemsBox);
    void* const extra = (static_cast<void**>(actor.extra))[2];
    if (extra == nullptr)
    {
        arena.useBtn->setEnabled(false);
        itemsBox.setEnabled(false);
    }
    else
    {
        arena.useBtn->setEnabled(true);
        itemsBox.setModel(static_cast<ItemsModel*>(extra));
        itemsBox.setEnabled(true);
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
        Actor& trgActor = *(arena.trgActor);
        if (skillIndex > -1)
        {
            arena.actBtn->setEnabled(arena.canTarget(crActor, *(crActor.aSkills->at(skillIndex)), trgActor));
        }
        if (itemIndex > -1)
        {
            QMap<Ability*, int>* const items = crActor.items;
            auto const it = items->cbegin() + itemIndex;
            arena.actBtn->setEnabled(it.value() > 0 && arena.canTarget(crActor, *(it.key()), trgActor));
        }
    }
    return arena;
}

ArenaWidget& ArenaWidget::afterAct()
{
    ArenaWidget& arena = *this;
    Actor& crActor = *(arena.crActor);
    arena.infoTxt->setText(QString(tr("%1 (Lv: %2/%3, %4, XP: %5/%6)")).arg(crActor.name, QString::number(crActor.lv), QString::number(crActor.maxLv),
         QString(tr("HP: %1/%2, MP: %3/%4, RP: %5/%6")).arg(QString::number(crActor.hp), QString::number(crActor.mHp), QString::number(crActor.mp),
         QString::number(crActor.mMp), QString::number(crActor.sp), QString::number(crActor.mSp)), QString::number(crActor.xp), QString::number(crActor.maxp)));
    arena.prepareSkillsBox(crActor, *(crActor.aSkills));
    arena.prepareItemsBox(crActor);
    return arena;
}

ArenaWidget& ArenaWidget::resizeScene(const QSize& newSize, const QSize* const oldSize)
{
    ArenaWidget& arena = *this;
    {
        int sprLength, imgWidth, imgHeight, btnHeight, infoHeight;
        int const width = newSize.width(), height = newSize.height();
        arena.setGeometry(0, 0, width, height);
        {
            QLabel* infoTxt = arena.infoTxt;
            QTextEdit* actionsTxt = arena.actionsTxt;
            QWidget* ctrWidget = arena.ctrWidget,* actWidget = arena.actWidget;
            QLayout* layout = arena.mainLayout,* ctrLayout = arena.ctrLayout,* actLayout = arena.actLayout;
            QComboBox* itemsBox = arena.itemsBox,* targetBox = arena.targetBox,* skillsBox = arena.skillsBox;
            QPushButton* actBtn = arena.actBtn,* useBtn = arena.useBtn,* autoBtn = arena.autoBtn,* fleeBtn = arena.fleeBtn;
            bool const portrait = height > width, deleted = (oldSize == nullptr || (portrait != (oldSize->height() > oldSize->width())));
            if (deleted)
            {
                if (layout != nullptr)
                {
                    delete layout;
                }
                if (ctrLayout != nullptr)
                {
                    delete ctrLayout;
                }
                actLayout->removeWidget(infoTxt);
                actLayout->removeWidget(arenaImg);
                actLayout->removeWidget(actionsTxt);
            }
            if (portrait)
            {
                btnHeight = infoHeight = height / 16;
                sprLength = (btnHeight * 4 + (btnHeight / 3));
                imgHeight = height - infoHeight - (sprLength += sprLength / 3);
                if (deleted)
                {
                    layout = new QVBoxLayout(this);
                    ctrLayout = new QGridLayout(ctrWidget);
                    (static_cast<QGridLayout*>(ctrLayout))->addWidget(actionsTxt, 1, 0, 1, 4);
                    (static_cast<QGridLayout*>(ctrLayout))->addWidget(skillsBox, 2, 0, 1, 2);
                    (static_cast<QGridLayout*>(ctrLayout))->addWidget(actBtn, 2, 2, 1, 2);
                    (static_cast<QGridLayout*>(ctrLayout))->addWidget(targetBox, 3, 0, 1, 2);
                    (static_cast<QGridLayout*>(ctrLayout))->addWidget(autoBtn, 3, 2, 1, 1);
                    (static_cast<QGridLayout*>(ctrLayout))->addWidget(fleeBtn, 3, 3, 1, 1);
                    (static_cast<QGridLayout*>(ctrLayout))->addWidget(itemsBox, 4, 0, 1, 2);
                    (static_cast<QGridLayout*>(ctrLayout))->addWidget(useBtn, 4, 2, 1, 2);
                    actLayout->addWidget(infoTxt);
                    actLayout->addWidget(arenaImg);
                    layout->addWidget(actWidget);
                    layout->addWidget(ctrWidget);
                }
                else
                {
                    ctrLayout = arena.ctrLayout;
                    layout = arena.mainLayout;
                }
                ctrWidget->setFixedHeight(sprLength);
                actWidget->setFixedHeight(imgHeight + infoHeight);
                actionsTxt->setGeometry(0, 0, 0, sprLength / 2);
                sprLength = (width + width / 3) / 5;
                ctrWidget->setFixedWidth(width);
                imgWidth = width;
            }
            else //landscape
            {
                sprLength = width / 5;
                btnHeight = height / 7;
                infoHeight = height / 16;
                int const actHeight = height / 5;
                imgHeight = height - actHeight - infoHeight;
                if (deleted)
                {
                    layout = new QHBoxLayout(this);
                    ctrLayout = new QVBoxLayout(ctrWidget);
                    ctrLayout->addWidget(autoBtn);
                    ctrLayout->addWidget(skillsBox);
                    ctrLayout->addWidget(actBtn);
                    ctrLayout->addWidget(targetBox);
                    ctrLayout->addWidget(useBtn);
                    ctrLayout->addWidget(itemsBox);
                    ctrLayout->addWidget(fleeBtn);
                    actLayout->addWidget(infoTxt);
                    actLayout->addWidget(arenaImg);
                    actLayout->addWidget(actionsTxt);
                    layout->addWidget(ctrWidget);
                    layout->addWidget(actWidget);
                }
                else
                {
                    ctrLayout = arena.ctrLayout;
                    layout = arena.mainLayout;
                }
                ctrWidget->setFixedWidth(sprLength);
                sprLength = (imgHeight + imgHeight / 11) / 3;
                actionsTxt->setFixedHeight(actHeight);
                actWidget->setFixedHeight(height);
                ctrWidget->setFixedHeight(height);
                imgWidth = width - sprLength;
            }
            if (deleted)
            {
                ctrLayout->setSpacing(1);
                actWidget->setLayout(actLayout);
                ctrWidget->setLayout(ctrLayout);
                arena.actLayout = static_cast<QVBoxLayout*>(actLayout);
                arena.ctrLayout = ctrLayout;
                arena.mainLayout = layout;
                arena.setLayout(layout);
            }
            arenaImg->setFixedWidth(imgWidth);
            arenaImg->setFixedHeight(imgHeight);
            actWidget->setFixedWidth(imgWidth);
            skillsBox->setFixedHeight(btnHeight);
            actionsTxt->setFixedWidth(imgWidth - imgWidth / 10);
            targetBox->setFixedHeight(btnHeight);
            infoTxt->setFixedHeight(infoHeight);
            itemsBox->setFixedHeight(btnHeight);
            autoBtn->setFixedHeight(btnHeight);
            fleeBtn->setFixedHeight(btnHeight);
            actBtn->setFixedHeight(btnHeight);
            useBtn->setFixedHeight(btnHeight);
        }
        {
            int const sprSize = SPR_SIZE / 2;
            {
                int k = 0, trgCount = 0;
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
                    trgCount += sSize;
                    for (int i = 0; i < sSize; ++i)
                    {
                        void** extras;
                        Actor& actor = *(party[i]);
                        {
                            void* const extra = actor.extra;
                            if (extra == nullptr)
                            {
                                extras = new void*[3] { nullptr, nullptr, nullptr };
                                actor.extra = extras;
                            }
                            else
                            {
                                extras = (static_cast<void**>(extra));
                            }
                        }
                        if (k < SPR_SIZE)
                        {
                            if (i < sprSize)
                            {
                                QRect loc;
                                switch (j > 1 ? k : i)
                                {
                                case 0:
                                case 4:
                                    loc.setRect(xCentre - (sprFactor * x), yCentre - (sprHeight * x), sprLength, sprLength);
                                    break;
                                case 1:
                                case 5:
                                    loc.setRect(xCentre - (sprWidth * x), yCentre - (sprDistance * x), sprLength, sprLength);
                                    break;
                                case 2:
                                case 6:
                                    loc.setRect(xCentre - (sprWidth * x), yCentre - (sprHeight * -1 * x), sprLength, sprLength);
                                    break;
                                case 3:
                                case 7:
                                    loc.setRect(xCentre - (sprFactor * x), yCentre - (sprDistance * -1 * x), sprLength, sprLength);
                                    break;
                                }
                                void* const sprExtra = extras[0];
                                if (sprExtra == nullptr)
                                {
                                    ActorSprite* const spr = new ActorSprite(k, actor, arenaImg, loc, arena, pos);
                                    spr->playActor(actor.hp > 0 ? SPR_IDLE : SPR_KO);
                                    extras[0] = spr;
                                }
                                else
                                {
                                    static_cast<ActorSprite*>(sprExtra)->relocate(loc);
                                }
                            }
                            ++k;
                        }
                        if (j == 0)
                        {
                            if (extras[1] == nullptr)
                            {
                                extras[1] = new SkillsModel(actor);
                            }
                            if (extras[2] == nullptr && actor.items != nullptr)
                            {
                                extras[2] = new ItemsModel(actor);
                            }
                        }
                    }
                }
                this->trgCount = trgCount;
            }
        }
    }
    return arena;
}

void ArenaWidget::resizeEvent(QResizeEvent* const event)
{
    const QSize& newSize = event->size();
    const QSize& oldSize = (event->oldSize());
    int width = newSize.width(), height = newSize.height();
    bool const landscape = (width > height);
    if (landscape != (oldSize.width() > oldSize.height()))
    {
        this->resizeScene(newSize, &oldSize);
    }
    QWidget::resizeEvent(event);
}

ArenaWidget& ArenaWidget::operator()(QSize size, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events, int const surprise, int const mInit)
{
    return this->operator()(size, ret, parties, events, surprise, mInit, true);
}

ArenaWidget& ArenaWidget::operator()(QSize& size, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events,
                                     int const surprise, int const mInit, bool const doScene)
{
    ArenaWidget& arena = *this;
    arena.setAutoFillBackground(true);
    arena.flags = 0;
    {
        QPalette palette = arena.palette();
        QLabel* arenaImg = new QLabel(this);
        QWidget* actWidget = new QWidget(this);
        QWidget* ctrWidget = new QWidget(this);
        palette.setColor(QPalette::Base, Qt::black);
        palette.setColor(QPalette::Window, Qt::black);
        palette.setColor(QPalette::Button, Qt::black);
        palette.setColor(QPalette::Background, Qt::black);
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::Highlight, Qt::darkYellow);
        palette.setColor(QPalette::HighlightedText, Qt::black);
        palette.setColor(QPalette::AlternateBase, Qt::black);
        palette.setColor(QPalette::WindowText, Qt::yellow);
        palette.setColor(QPalette::ToolTipText, Qt::yellow);
        palette.setColor(QPalette::Foreground, Qt::yellow);
        palette.setColor(QPalette::ButtonText, Qt::yellow);
        palette.setColor(QPalette::BrightText, Qt::white);
        palette.setColor(QPalette::Text, Qt::yellow);
        QVBoxLayout* actLayout = new QVBoxLayout(this);
        QTextEdit* actionsTxt = new QTextEdit(this);
        QPushButton* actBtn = new QPushButton(this);
        QPushButton* autoBtn = new QPushButton(this);
        QPushButton* fleeBtn = new QPushButton(this);
        QPushButton* useBtn = new QPushButton(this);
        QComboBox* targetBox = new QComboBox(this);
        QComboBox* skillsBox = new QComboBox(this);
        QComboBox* itemsBox = new QComboBox(this);
        QLabel* infoTxt = new QLabel(this);
        targetBox->setAutoFillBackground(true);
        skillsBox->setAutoFillBackground(true);
        itemsBox->setAutoFillBackground(true);
        autoBtn->setAutoFillBackground(true);
        actionsTxt->setPalette(palette);
        targetBox->setPalette(palette);
        skillsBox->setPalette(palette);
        itemsBox->setPalette(palette);
        actBtn->setText(tr("Execute"));
        actBtn->setPalette(palette);
        autoBtn->setText(tr("Auto"));
        autoBtn->setPalette(palette);
        fleeBtn->setText(tr("Flee"));
        fleeBtn->setPalette(palette);
        useBtn->setText(tr("Use"));
        useBtn->setPalette(palette);
        infoTxt->setPalette(palette);
        arena.setPalette(palette);
        autoBtn->setFlat(true);
        actBtn->setFlat(true);
        useBtn->setFlat(true);
        fleeBtn->setFlat(true);
        actionsTxt->setReadOnly(true);
        targetBox->setStyleSheet("background-color: black; color: yellow;");
        skillsBox->setStyleSheet("background-color: black; color: yellow;");
        itemsBox->setStyleSheet("background-color: black; color: yellow;");
        actWidget->setLayout(actLayout);
        arena.actionsTxt = actionsTxt;
        arena.actLayout = actLayout;
        arena.actWidget = actWidget;
        arena.ctrWidget = ctrWidget;
        arena.targetBox = targetBox;
        arena.skillsBox = skillsBox;
        arena.mainLayout = nullptr;
        arena.ctrLayout = nullptr;
        arena.itemsBox = itemsBox;
        arena.arenaImg = arenaImg;
        arena.infoTxt = infoTxt;
        arena.fleeBtn = fleeBtn;
        arena.autoBtn = autoBtn;
        arena.actBtn = actBtn;
        arena.useBtn = useBtn;
        connect(autoBtn, &QPushButton::clicked, [&arena]()
        {
            bool const automatic = arena.isAutomatic();
            if (automatic)
            {
                arena.setAutomatic(false).autoBtn->setEnabled(false);
                arena.autoBtn->setText(tr("Auto"));
            }
            else
            {
                Actor* const crActor = arena.crActor;
                QString& ret = *(arena.returnTxt); ret.clear();
                arena.setAutomatic(true).autoBtn->setText(tr("Manual"));
                if (crActor->side == 0 || !(crActor->isAiPlayer() || crActor->isConfused() || crActor->isEnraged()))
                {
                    arena.playAi(ret, *crActor);
                    arena.enableControls(false);
                }
            }
        });
        connect(actBtn, &QPushButton::clicked, [&arena]()
        {
            Actor* const crActor = arena.crActor;
            Actor* const trgActor = arena.trgActor;
            QString& ret = *(arena.returnTxt); ret.clear();
            arena.perform(ret, *crActor, *trgActor, *(crActor->aSkills->at(arena.skillsBox->currentIndex())), false);
            arena.enableControls(false);
        });        
        connect(useBtn, &QPushButton::clicked, [&arena]()
        {
            Actor* const crActor = arena.crActor;
            Actor* const trgActor = arena.trgActor;
            QString& ret = *(arena.returnTxt); ret.clear();
            QMap<Ability*, int>* const items = crActor->items;
            auto const it = items->cbegin() + arena.itemsBox->currentIndex();
            Ability* const ability = it.key(); items->operator[](ability) = it.value() - 1;
            arena.perform(ret, *crActor, *trgActor, *ability, false);
            arena.enableControls(false);
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
    arena.sprRuns = 0;
    auto actorRun = new ActorAct([](Scene& scene, Actor& user, Ability* const ability, bool const revive,
                                 Actor* const target, Ability* const counter) -> bool
    {
        ActorSprite* const userSpr = (static_cast<ActorSprite*>(static_cast<void**>(user.extra)[0]));
        if (target != nullptr)
        {
            ArenaWidget& arena = static_cast<ArenaWidget&>(scene);
            //QString& ret = *(arena.returnTxt);
            ActorSprite* targetSpr;
            if (target == &user)
            {
                targetSpr = userSpr;
            }
            else
            {
                targetSpr = (static_cast<ActorSprite*>(static_cast<void**>(target->extra)[0]));
                targetSpr->playActor(target->hp > 0 ? (revive ? SPR_RISE : (counter == nullptr ? SPR_HIT
                    : (((counter->dmgType & DMG_TYPE_ATK) == DMG_TYPE_ATK) ? SPR_ACT : SPR_CAST))) : SPR_FALL);
            }
            if (ability != nullptr)
            {
                QString* const spr = ability->sprite;
                if (spr != nullptr && spr->length() > 0)
                {
                    targetSpr->playSkill(*spr);
                }
            }
            /*QVector<Costume*>* usrRoles;
            if (user.actions > 1 || ((usrRoles = user.dmgRoles) == nullptr) || usrRoles->size() < 1)
            {
                arena.endTurn(ret, &user);
                arena.actionsTxt->append(ret);
            }
            else
            {
                arena.actionsTxt->append(ret % ".");
                arena.setEndTurn(true);
                ret.clear();
            }*/
        }
        userSpr->playActor(user.hp < 1 ? SPR_FALL : (ability == nullptr ? (revive ? SPR_IDLE : SPR_HIT)
            : ((ability->dmgType & DMG_TYPE_ATK) == DMG_TYPE_ATK ? SPR_ACT : SPR_CAST)));
        return false;
    });
    QString* const returnTxt = new QString(ret);
    arena.resizeScene(size, nullptr);
    arena.prepareTargetBox(false);
    arena.returnTxt = returnTxt;
    if (doScene)
    {
        arena.Scene::operator()(*returnTxt, parties, actorRun, events, surprise, mInit);
    }
    else
    {
        arena.actorEvent = actorRun;
    }
    if (crActor->side != 0 || crActor->isAiPlayer() || crActor->isConfused() || crActor->isEnraged())
    {
        arena.playAi(*returnTxt, *crActor);
        arena.enableControls(false);
    }
    else
    {
        arena.afterAct();
    }
    return arena;
}

ArenaWidget::ArenaWidget(QWidget* parent) : QWidget(parent), Scene()
{

}

ArenaWidget::ArenaWidget(QWidget* parent, QSize size, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events, int const surprise, int const mInit)
    : QWidget(parent), Scene(ret, parties, nullptr, events, surprise, mInit)
{
    this->operator()(size, ret, parties, events, surprise, mInit, false);
}

ArenaWidget::~ArenaWidget()
{
    delete this->actorEvent;
    delete this->targetsModel;
    delete this->returnTxt;
    //for (QVector<Actor*>* const party : this->parties)
    {
        //for (Actor* const player : *party)
        for (Actor* const player : *(this->parties[0]))
        {
            void** const extra = static_cast<void**>(player->extra);
            player->extra = nullptr;
            ActorSprite* const spr = static_cast<ActorSprite*>(extra[0]);
            if (spr != nullptr)
            {
                delete spr;
            }
            delete (static_cast<SkillsModel*>(extra[1]));
            void* const itemsExtra = extra[2];
            if (itemsExtra != nullptr)
            {
                delete (static_cast<ItemsModel*>(itemsExtra));
            }
            delete[] extra;
        }
    }
}
