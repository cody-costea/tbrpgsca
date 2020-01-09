/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "arenawidget.h"

using namespace tbrpgsca;

ArenaWidget::ArenaWidget(QWidget* parent) : QWidget(parent), Scene()
{

}

ArenaWidget::ArenaWidget(QWidget* parent, QString& ret, QVector<QVector<Actor*>*>& parties, ActorAct* const actorEvent,
                         QVector<SceneAct*>* const events, int const surprise, int const mInit)
    : QWidget(parent), Scene(ret, parties, actorEvent, events, surprise, mInit)
{

}
