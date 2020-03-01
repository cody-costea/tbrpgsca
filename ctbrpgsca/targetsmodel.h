/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef TARGETSMODEL_H
#define TARGETSMODEL_H

#include <QAbstractListModel>

#include "actor.h"

namespace tbrpgsca
{

    class ArenaWidget;

    class TargetsModel : public QAbstractListModel
    {
        Q_OBJECT
    public:
        TargetsModel(ArenaWidget& scene, QObject* const parent = 0);
        ~TargetsModel();

        int rowCount(const QModelIndex& parent = QModelIndex()) const;
        QVariant data(const QModelIndex& index, int role) const;

     private:
        ArenaWidget* scene;
    };

}

#endif // TARGETSMODEL_H
