/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef ARENAWIDGET_H
#define ARENAWIDGET_H

#include <QWidget>
#include "scene.h"

namespace tbrpgsca
{

    class ArenaWidget : public QWidget, public Scene
    {
        Q_OBJECT
    public:
        explicit ArenaWidget(QWidget* parent = nullptr);
        ArenaWidget(QWidget* parent, QString& ret, QVector<QVector<Actor*>*>& parties, ActorAct* const actorEvent,
                    QVector<SceneAct*>* const events, int const surprise, int const mInit);

    signals:

    };

}

#endif // ARENAWIDGET_H
