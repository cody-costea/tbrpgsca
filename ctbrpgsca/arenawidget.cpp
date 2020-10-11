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
#include <QTimer>

using namespace tbrpgsca;

void ArenaWidget::afterPlay()
{
    ArenaWidget& arena = *this;
    QString& ret = *(arena._ret_str);
    int const sprRuns = arena._spr_runs - 1;
    if (sprRuns > -1)
    {
        arena._spr_runs = sprRuns;
        if (sprRuns == 0)
        {
            Actor* crActor;
            {
               if (arena.isEndTurn())
               {
                   arena.setEndTurn(false);
               }
               else
               {
                   crActor = arena._cr_actor;
                   QVector<Costume*>* usrRoles;
                   if (((usrRoles = crActor->_dmg_roles)) && usrRoles->size() > 0)
                   {
                       arena.setEndTurn(true);
                       arena.endTurn(ret, crActor, this);
                       return;
                   }
                   else
                   {
                       arena.endTurn(ret, crActor, this);
                   }
               }
               crActor = arena._cr_actor;
               arena._acts_txt->append(ret); ret.clear();
               if (arena.isAutomatic() || crActor->_side != 0 || crActor->hasAnyFlag(FLAG_AI_PLAYER | FLAG_CONFUSE | FLAG_ENRAGED)
                       /*crActor->isAiPlayer() || crActor->Costume::isConfused() || crActor->Costume::isEnraged()*/)
               {
                   arena._info_txt->setText("");
                   arena.setAiTurn(true);
                   arena.playAi(ret, *crActor, this);
               }
               else
               {
                   arena.setAiTurn(false);
                   arena.enableControls(true);
                   arena.afterAct();
               }
            }
        }
    }
    else
    {
        arena.setEndTurn(false);
        arena.endTurn(ret, arena._cr_actor, this);
    }
    return;
}

void ArenaWidget::ActorSprite::playSkill(QString& sprName)
{
    ActorSprite& actSprite = *this;
    QMovie& movie = *(actSprite._skill_mov);
    movie.stop();
    movie.setFileName(QString(":/sprites/%1/abilities/%2.%1").arg(SPR_EXT, sprName));
    movie.jumpToFrame(0);    
    ++(actSprite._arena->_spr_runs);
    movie.start();
    return;
}

void ArenaWidget::ActorSprite::playActor(int const spr)
{
    ActorSprite& actSprite = *this;
    QMovie& movie = *(actSprite._actor_mov);
    Actor& sprActor = *(actSprite._actor);
    if (actSprite._spr != spr || sprActor.Costume::isShapeShifted())
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
            return;
        }
        movie.stop();
        actSprite._spr = spr;
        movie.setFileName(QString(":/sprites/%1/%2/bt_%3_%4.%1").arg(SPR_EXT, *(sprActor._sprite), actSprite._pos, s));
        /*movie.setFileName(QString(":/sprites/%1/%2/bt_%3_%4.%1").arg(SPR_EXT, *(sprActor._sprite), (actSprite.arena->surprise == 0
            ? (sprActor.oldSide == 0 ? POS_RIGHT : POS_LEFT) : (sprActor.oldSide == 0 ? POS_LEFT : POS_RIGHT)), s));*/
    }
    movie.jumpToFrame(0);
    if (spr == SPR_IDLE || spr == SPR_KO)
    {
        actSprite._actor_lbl->update();
    }
    else
    {
        ++(actSprite._arena->_spr_runs);
        movie.start();
    }
    return;
}

void ArenaWidget::ActorSprite::relocate(QRect& location)
{
    ActorSprite& actSprite = *this;
    int const width = location.width(), height = location.height();
    {
        QSize size(width, height);
        actSprite._actor_mov->setScaledSize(size);
        actSprite._skill_mov->setScaledSize(size);
    }
    actSprite._skill_lbl->setGeometry(0, 0, width, height);
    actSprite._actor_lbl->setGeometry(location);
    return;
}

ArenaWidget::ActorSprite::ActorSprite(int const index, Actor& actor, QWidget* const widget, QRect& location, ArenaWidget& arena, QString& pos)
{
    this->_spr = -1;
    this->_pos = pos;
    this->_actor = &actor;
    this->_arena = &arena;
    QLabel* const actorLabel = new QLabel(widget);
    ClickableLabel* skillLabel = new ClickableLabel(actorLabel);
    QMovie* const actorMovie = new QMovie(),* skillMovie = new QMovie();
    actorMovie->setCacheMode(QMovie::CacheAll);
    skillMovie->setCacheMode(QMovie::CacheAll);
    auto run = [&arena]()
    {
        arena.afterPlay();
    };
    actorLabel->setScaledContents(true);
    skillLabel->setScaledContents(true);
    connect(actorMovie, &QMovie::finished, run);
    connect(skillMovie, &QMovie::finished, run);
    connect(skillLabel, &ClickableLabel::clicked, [&arena, index]()
    {
        QComboBox& targetBox = *(arena._target_box);
        if (targetBox.currentIndex() == index)
        {
            arena._act_btn->click();
        }
        else
        {
            targetBox.setCurrentIndex(index);
        }
    });
    actorLabel->setMovie(actorMovie);
    skillLabel->setMovie(skillMovie);
    this->_actor_mov = actorMovie;
    this->_actor_lbl = actorLabel;
    this->_skill_mov = skillMovie;
    this->_skill_lbl = skillLabel;
    this->relocate(location);
    //actor._extra = this;
    actorLabel->show();
    skillLabel->show();
}

ArenaWidget::ActorSprite::~ActorSprite()
{
    QMovie* const actorMovie = this->_actor_mov,* skillMovie = this->_skill_mov;
    this->_actor_lbl = nullptr;
    this->_actor_mov = nullptr;
    this->_skill_lbl = nullptr;
    this->_skill_mov = nullptr;
    delete actorMovie;
    delete skillMovie;
}

void ArenaWidget::enableControls(bool const enable)
{
    ArenaWidget& arena = *this;
    QComboBox& skillsBox = *(arena._skills_box);
    QComboBox& itemsBox = *(arena._items_box);
    skillsBox.setEnabled(enable);
    if (enable)
    {
        arena.recheckTargeting(arena._target_box->currentIndex(), skillsBox.currentIndex(), itemsBox.currentIndex());
        arena._auto_btn->setEnabled(true);
    }
    else
    {
        arena._act_btn->setEnabled(false);
        arena._use_btn->setEnabled(false);
    }
    arena._flee_btn->setEnabled(enable);
    itemsBox.setEnabled(enable);
    return;
}

void ArenaWidget::prepareTargetBox(bool const freeMemory)
{
    ArenaWidget& arena = *this;
    QComboBox* targetBox = arena._target_box;
    TargetsModel* targetsModel;
    if (freeMemory)
    {
        targetsModel = arena._targets_mdl;
        if (targetsModel)
        {
            delete targetsModel;
        }
    }
    targetsModel = new TargetsModel(arena);
    arena._target_box->setModel(targetsModel);
    arena._targets_mdl = targetsModel;
    connect(targetBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&arena](int const i)
    {
        Actor& crActor = *(arena._cr_actor);
        QComboBox& skillsBox = *(arena._skills_box);
        int const oldSkillIndex = skillsBox.currentIndex();
        bool const restore = i < arena._target_box->count() / 2;
        int const newSkillIndex = arena.getAiSkill(crActor, *(crActor._a_skills), restore ? 1 : 0, restore);
        arena._trg_actor = arena.getPlayerFromTargetBox(i);
        skillsBox.setCurrentIndex(newSkillIndex);
        if (newSkillIndex == oldSkillIndex)
        {
            arena.recheckTargeting(i, newSkillIndex, arena._items_box->currentIndex());
        }
    });
    targetBox->setCurrentIndex(arena._trg_count / 2);
    return;
}

void ArenaWidget::prepareSkillsBox(Actor& actor, QVector<Ability*>& skills)
{
    ArenaWidget& arena = *this;
    QComboBox& skillBox = *(arena._skills_box);
    skillBox.setModel(static_cast<SkillsModel*>(static_cast<void**>(actor._extra)[1]));
    QComboBox& targetBox = *(arena._target_box);
    bool const restore = targetBox.currentIndex() < targetBox.count() / 2;
    skillBox.setCurrentIndex(arena.getAiSkill(actor, skills, restore ? 1 : 0, restore));
    return;
}

void ArenaWidget::prepareItemsBox(Actor& actor)
{
    ArenaWidget& arena = *this;
    QComboBox& itemsBox = *(arena._items_box);
    void* const extra = (static_cast<void**>(actor._extra))[2];
    if (extra == nullptr)
    {
        arena._use_btn->setEnabled(false);
        itemsBox.setEnabled(false);
    }
    else
    {
        arena._use_btn->setEnabled(true);
        itemsBox.setModel(static_cast<ItemsModel*>(extra));
        itemsBox.setEnabled(true);
    }
    return;
}

Actor* ArenaWidget::getPlayerFromTargetBox(int index)
{
    for (QVector<Actor*>* party : this->_parties)
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

void ArenaWidget::recheckTargeting(int const trgIndex, int const skillIndex, int const itemIndex)
{
    ArenaWidget& arena = *this;
    if (arena._spr_runs == 0 && trgIndex > -1 && (!arena.isAiTurn()))
    {
        Actor& crActor = *(arena._cr_actor);
        Actor& trgActor = *(arena._trg_actor);
        if (skillIndex > -1)
        {
            arena._act_btn->setEnabled(arena.canTarget(crActor, *(crActor._a_skills->at(skillIndex)), trgActor));
        }
        if (itemIndex > -1)
        {
            QMap<Ability*, int>* const items = crActor._items;
            auto const it = items->cbegin() + itemIndex;
            arena._act_btn->setEnabled(it.value() > 0 && arena.canTarget(crActor, *(it.key()), trgActor));
        }
    }
    return;
}

void ArenaWidget::afterAct()
{
    ArenaWidget& arena = *this;
    Actor& crActor = *(arena._cr_actor);
    arena._info_txt->setText(QString(tr("%1 (Lv: %2/%3, %4, XP: %5/%6)")).arg(crActor._name, QString::number(crActor._lv), QString::number(crActor._max_lv),
         QString(tr("HP: %1/%2, MP: %3/%4, RP: %5/%6")).arg(QString::number(crActor._hp), QString::number(crActor._m_hp), QString::number(crActor._mp),
         QString::number(crActor._m_mp), QString::number(crActor._sp), QString::number(crActor._m_sp)), QString::number(crActor._xp), QString::number(crActor._maxp)));
    arena.prepareSkillsBox(crActor, *(crActor._a_skills));
    arena.prepareItemsBox(crActor);
    return;
}

void ArenaWidget::resizeScene(const QSize& newSize, const QSize* const oldSize)
{
    ArenaWidget& arena = *this;
    {
        int sprLength, imgWidth, imgHeight, btnHeight, infoHeight;
        int const width = newSize.width(), height = newSize.height();
        arena.setGeometry(0, 0, width, height);
        {
            QLabel* infoTxt = arena._info_txt;
            QTextEdit* actionsTxt = arena._acts_txt;
            QWidget* ctrWidget = arena._ctr_wdt,* actWidget = arena._act_wdt;
            QLayout* layout = arena._main_lyt,* ctrLayout = arena._ctr_lyt,* actLayout = arena._act_lyt;
            QComboBox* itemsBox = arena._items_box,* targetBox = arena._target_box,* skillsBox = arena._skills_box;
            QPushButton* actBtn = arena._act_btn,* useBtn = arena._use_btn,* autoBtn = arena._auto_btn,* fleeBtn = arena._flee_btn;
            bool const portrait = height > width, deleted = (oldSize == nullptr || (portrait != (oldSize->height() > oldSize->width())));
            if (deleted)
            {
                if (layout)
                {
                    delete layout;
                }
                if (ctrLayout)
                {
                    delete ctrLayout;
                }
                actLayout->removeWidget(infoTxt);
                actLayout->removeWidget(_back_img);
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
                    actLayout->addWidget(_back_img);
                    layout->addWidget(actWidget);
                    layout->addWidget(ctrWidget);
                }
                else
                {
                    ctrLayout = arena._ctr_lyt;
                    layout = arena._main_lyt;
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
                    actLayout->addWidget(_back_img);
                    actLayout->addWidget(actionsTxt);
                    layout->addWidget(ctrWidget);
                    layout->addWidget(actWidget);
                }
                else
                {
                    ctrLayout = arena._ctr_lyt;
                    layout = arena._main_lyt;
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
                arena._act_lyt = static_cast<QVBoxLayout*>(actLayout);
                arena._ctr_lyt = ctrLayout;
                arena._main_lyt = layout;
                arena.setLayout(layout);
            }
            _back_img->setFixedWidth(imgWidth);
            _back_img->setFixedHeight(imgHeight);
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
                int k = 0, trgCount = 0, surprise = arena._surprise;
                QVector<QVector<Actor*>*>& parties = arena._parties;
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
                            void* const extra = actor._extra;
                            if (extra == nullptr)
                            {
                                extras = new void*[3] { nullptr, nullptr, nullptr };
                                actor._extra = extras;
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
                                    ActorSprite* const spr = new ActorSprite(k, actor, _back_img, loc, arena, pos);
                                    spr->playActor(actor._hp > 0 ? SPR_IDLE : SPR_KO);
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
                            if (extras[2] == nullptr && actor._items)
                            {
                                extras[2] = new ItemsModel(actor);
                            }
                        }
                    }
                }
                this->_trg_count = trgCount;
            }
        }
    }
    return;
}

void ArenaWidget::resizeEvent(QResizeEvent* const event)
{
    const QSize& newSize = event->size();
    const QSize& oldSize = (event->oldSize());
    int const width = newSize.width(), height = newSize.height();
    bool const landscape = (width > height);
    if (landscape != (oldSize.width() > oldSize.height()))
    {
        this->resizeScene(newSize, &oldSize);
    }
    QWidget::resizeEvent(event);
}

bool ArenaWidget::operator()(Scene& scene, Actor* const user, Ability* const ability, bool const revive,
                            Actor* const target, Ability* const counter)
{
   ArenaWidget& arena = static_cast<ArenaWidget&>(scene);
#if TRG_SPR_DELAY < 0
   if (target)
   {
#else
   if (target && target != user)
   {
       ++(arena._spr_runs);
       QTimer::singleShot(TRG_SPR_DELAY, &arena, [&arena, target, revive, counter, ability]()
       {
           --(arena._spr_runs);
#endif
       ActorSprite& targetSpr = *(static_cast<ActorSprite*>(static_cast<void**>(target->_extra)[0]));
       targetSpr.playActor(target->_hp > 0 ? (revive ? SPR_RISE : (counter == nullptr ? SPR_HIT
           : (((counter->_dmg_type & DMG_TYPE_ATK) == DMG_TYPE_ATK) ? SPR_ACT : SPR_CAST))) : SPR_FALL);
       if (ability)
       {
           QString* const spr = ability->_sprite;
           if (spr && spr->length() > 0)
           {
               targetSpr.playSkill(*spr);
           }
#if TRG_SPR_DELAY < 0
           QString* const sndName = ability->_sound;
           if (sndName)
           {
               QMediaPlayer* sound = arena._ability_snd;
               if (sound == nullptr)
               {
                   sound = new QMediaPlayer(&arena);
                   connect(sound, &QMediaPlayer::stateChanged, [&arena](QMediaPlayer::State state)
                   {
                       if (state == QMediaPlayer::StoppedState)
                       {
                           arena.afterPlay();
                       }
                   });
                   arena._ability_snd = sound;
               }
               else if (sound->state() == QMediaPlayer::PlayingState)
               {
                   goto usrSprPlay;
               }
               arena._spr_runs++;
               sound->setMedia(QUrl(QString("qrc:/audio/%0").arg(*sndName)));
               sound->play();
           }
       }
   }
   usrSprPlay:
   if (user && user != target)
   {
#else
       }});
   }
   if (user)
   {
       if (ability)
       {
           QString* const sndName = ability->_sound;
           if (sndName)
           {
               QMediaPlayer* sound = arena._ability_snd;
               if (sound == nullptr)
               {
                   sound = new QMediaPlayer(&arena);
                   connect(sound, &QMediaPlayer::stateChanged, [&arena](QMediaPlayer::State state)
                   {
                       if (state == QMediaPlayer::StoppedState)
                       {
                           arena.afterPlay();
                       }
                   });
                   arena._ability_snd = sound;
               }
               else if (sound->state() == QMediaPlayer::PlayingState)
               {
                   goto usrSprPlay;
               }
               arena._spr_runs++;
               sound->setMedia(QUrl(QString("qrc:/audio/%0").arg(*sndName)));
               sound->play();
           }
       }
       usrSprPlay:
#endif
       (*(static_cast<ActorSprite*>(static_cast<void**>(user->_extra)[0]))).playActor(
           user->_hp < 1 ? SPR_FALL : (ability == nullptr ? (revive ? SPR_IDLE : SPR_HIT)
       : ((ability->_dmg_type & DMG_TYPE_ATK) == DMG_TYPE_ATK ? SPR_ACT : SPR_CAST)));
   }
   return false;
}

void ArenaWidget::operator()(QSize size, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneRun*>* const events,
                                     QString backImage, QString songName, int const surprise, int const mInit)
{
    return this->operator()(size, ret, parties, events, backImage, songName, surprise, mInit, true);
}

void ArenaWidget::operator()(QSize& size, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneRun*>* const events,
                                     QString& backImage, QString& songName, int const surprise, int const mInit, bool const doScene)
{
    ArenaWidget& arena = *this;
    arena.setAutoFillBackground(true);
    if (songName.length() > 0)
    {
        QMediaPlayer* const songPlayer = new QMediaPlayer(this);
        QMediaPlaylist* const playList = new QMediaPlaylist(this);
        playList->addMedia(QUrl(QString("qrc:/audio/%0").arg(songName)));
        playList->setPlaybackMode(QMediaPlaylist::Loop);
        songPlayer->setPlaylist(playList);
        songPlayer->play();
        //arena.song = songPlayer;
    }
    arena._ability_snd = nullptr;
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
        if (backImage.length() > 0)
        {
            arenaImg->setPixmap(QPixmap(QString("qrc:/images/%0").arg(backImage)));
        }
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
        arena._acts_txt = actionsTxt;
        arena._act_lyt = actLayout;
        arena._act_wdt = actWidget;
        arena._ctr_wdt = ctrWidget;
        arena._target_box = targetBox;
        arena._skills_box = skillsBox;
        arena._main_lyt = nullptr;
        arena._ctr_lyt = nullptr;
        arena._items_box = itemsBox;
        arena._back_img = arenaImg;
        arena._info_txt = infoTxt;
        arena._flee_btn = fleeBtn;
        arena._auto_btn = autoBtn;
        arena._act_btn = actBtn;
        arena._use_btn = useBtn;
        connect(autoBtn, &QPushButton::clicked, [&arena]()
        {
            bool const automatic = arena.isAutomatic();
            if (automatic)
            {
                arena.setAutomatic(false);
                arena._auto_btn->setEnabled(false);
                arena._auto_btn->setText(tr("Auto"));
            }
            else if (arena._act_btn->isEnabled())
            {
                Actor* const crActor = arena._cr_actor;
                QString& ret = *(arena._ret_str); ret.clear();
                arena.setAutomatic(true);
                arena._auto_btn->setText(tr("Manual"));
                if (crActor->_side == 0 || !(crActor->hasAnyFlag(FLAG_AI_PLAYER | FLAG_CONFUSE | FLAG_ENRAGED)
                    /*crActor->isAiPlayer() || crActor->Costume::isConfused() || crActor->Costume::isEnraged()*/))
                {
                    arena.playAi(ret, *crActor, &arena);
                    arena.enableControls(false);
                }
            }
        });
        connect(actBtn, &QPushButton::clicked, [&arena]()
        {
            arena.enableControls(false);
            Actor* const crActor = arena._cr_actor;
            Actor* const trgActor = arena._trg_actor;
            QString& ret = *(arena._ret_str); ret.clear();
            arena.perform(ret, *crActor, *trgActor, *(crActor->_a_skills->at(arena._skills_box->currentIndex())), false, &arena);
        });        
        connect(useBtn, &QPushButton::clicked, [&arena]()
        {
            arena.enableControls(false);
            Actor* const crActor = arena._cr_actor;
            Actor* const trgActor = arena._trg_actor;
            QString& ret = *(arena._ret_str); ret.clear();
            QMap<Ability*, int>* const items = crActor->_items;
            auto const it = items->cbegin() + arena._items_box->currentIndex();
            Ability* const ability = it.key(); items->operator[](ability) = it.value() - 1;
            arena.perform(ret, *crActor, *trgActor, *ability, false, &arena);
        });
        connect(skillsBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&arena](int const i)
        {
            arena.recheckTargeting(arena._target_box->currentIndex(), i, arena._items_box->currentIndex());
        });
        connect(itemsBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&arena](int const i)
        {
            arena.recheckTargeting(arena._target_box->currentIndex(), arena._skills_box->currentIndex(), i);
        });
    }
    arena._spr_runs = 0;
    /*auto actorRun = new SpriteCall([](Scene& scene, Actor* const user, Ability* const ability, bool const revive,
                                 Actor* const target, Ability* const counter) -> bool
    {
        ArenaWidget& arena = static_cast<ArenaWidget&>(scene);
#if TRG_SPR_DELAY < 0
        if (target)
        {
#else
        if (target && target != user)
        {
            ++(arena._spr_runs);
            QTimer::singleShot(TRG_SPR_DELAY, &arena, [&arena, target, revive, counter, ability]()
            {
                --(arena._spr_runs);
#endif
            ActorSprite& targetSpr = *(static_cast<ActorSprite*>(static_cast<void**>(target->_extra)[0]));
            targetSpr.playActor(target->_hp > 0 ? (revive ? SPR_RISE : (counter == nullptr ? SPR_HIT
                : (((counter->_dmg_type & DMG_TYPE_ATK) == DMG_TYPE_ATK) ? SPR_ACT : SPR_CAST))) : SPR_FALL);
            if (ability)
            {
                QString* const spr = ability->_sprite;
                if (spr && spr->length() > 0)
                {
                    targetSpr.playSkill(*spr);
                }                
#if TRG_SPR_DELAY < 0
                QString* const sndName = ability->_sound;
                if (sndName)
                {
                    QMediaPlayer* sound = arena._ability_snd;
                    if (sound == nullptr)
                    {
                        sound = new QMediaPlayer(&arena);
                        connect(sound, &QMediaPlayer::stateChanged, [&arena](QMediaPlayer::State state)
                        {
                            if (state == QMediaPlayer::StoppedState)
                            {
                                arena.afterPlay();
                            }
                        });
                        arena._ability_snd = sound;
                    }
                    else if (sound->state() == QMediaPlayer::PlayingState)
                    {
                        goto usrSprPlay;
                    }
                    arena._spr_runs++;
                    sound->setMedia(QUrl(QString("qrc:/audio/%0").arg(*sndName)));
                    sound->play();
                }
            }
        }
        usrSprPlay:
        if (user && user != target)
        {
#else
            }});
        }
        if (user)
        {
            if (ability)
            {
                QString* const sndName = ability->_sound;
                if (sndName)
                {
                    QMediaPlayer* sound = arena._ability_snd;
                    if (sound == nullptr)
                    {
                        sound = new QMediaPlayer(&arena);
                        connect(sound, &QMediaPlayer::stateChanged, [&arena](QMediaPlayer::State state)
                        {
                            if (state == QMediaPlayer::StoppedState)
                            {
                                arena.afterPlay();
                            }
                        });
                        arena._ability_snd = sound;
                    }
                    else if (sound->state() == QMediaPlayer::PlayingState)
                    {
                        goto usrSprPlay;
                    }
                    arena._spr_runs++;
                    sound->setMedia(QUrl(QString("qrc:/audio/%0").arg(*sndName)));
                    sound->play();
                }
            }
            usrSprPlay:
#endif
            (*(static_cast<ActorSprite*>(static_cast<void**>(user->_extra)[0]))).playActor(
                user->_hp < 1 ? SPR_FALL : (ability == nullptr ? (revive ? SPR_IDLE : SPR_HIT)
            : ((ability->_dmg_type & DMG_TYPE_ATK) == DMG_TYPE_ATK ? SPR_ACT : SPR_CAST)));
        }
        return false;
    });*/
    QString* const returnTxt = new QString(ret);
    arena.resizeScene(size, nullptr);
    arena.prepareTargetBox(false);
    arena._ret_str = returnTxt;
    if (doScene)
    {
        arena.Scene::operator()(*returnTxt, parties, this, events, true, surprise, mInit);
    }
    /*else
    {
        arena._actor_run = actorRun;
    }*/
    Actor* const crActor = arena._cr_actor;
    if (crActor->_side != 0 || crActor->hasAnyFlag(FLAG_AI_PLAYER | FLAG_CONFUSE | FLAG_ENRAGED)
        /*crActor->isAiPlayer() || crActor->Costume::isConfused() || crActor->Costume::isEnraged()*/)
    {
        arena.playAi(*returnTxt, *crActor, this);
        arena.enableControls(false);
    }
    else
    {
        arena.afterAct();
    }
    return;
}

ArenaWidget::ArenaWidget(QWidget* parent) : QWidget(parent), Scene()
{

}

ArenaWidget::ArenaWidget(QWidget* parent, QSize size, QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneRun*>* const events,
                         QString backImage, QString songName, int const surprise, int const mInit)
    : QWidget(parent), Scene(ret, parties, this, events, true, surprise, mInit)
{
    this->operator()(size, ret, parties, events, backImage, songName, surprise, mInit, false);
}

ArenaWidget::~ArenaWidget()
{
    //delete this->_actor_run;
    delete this->_targets_mdl;
    delete this->_ret_str;
    //for (QVector<Actor*>* const party : this->_parties)
    {
        //for (Actor* const player : *party)
        for (Actor* const player : *(this->_parties[0]))
        {
            void** const extra = static_cast<void**>(player->_extra);
            player->_extra = nullptr;
            ActorSprite* const spr = static_cast<ActorSprite*>(extra[0]);
            if (spr)
            {
                delete spr;
            }
            delete (static_cast<SkillsModel*>(extra[1]));
            void* const itemsExtra = extra[2];
            if (itemsExtra)
            {
                delete (static_cast<ItemsModel*>(itemsExtra));
            }
            delete[] extra;
        }
    }
    /*auto song = this->song;
    if (song)
    {
        delete song;
    }
    auto abilitySnd = this->abilitySnd;
    if (abilitySnd)
    {
        delete abilitySnd;
    }*/
}
