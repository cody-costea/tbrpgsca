/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef ITEMSMODEL_H
#define ITEMSMODEL_H

#include <QAbstractListModel>
#include <QBrush>
#include <QColor>

#include "ability.hpp"
#include "actor.hpp"

namespace tbrpgsca
{

    class ItemsModel : public QAbstractListModel
    {
        Q_OBJECT
    public:
       ItemsModel(Actor& actor, QObject* const parent = 0);
       ~ItemsModel();

       int rowCount(const QModelIndex& parent = QModelIndex()) const;
       QVariant data(const QModelIndex& index, int role) const;

    private:
       Actor* _actor;
    };

}

#endif // ITEMSMODEL_H
