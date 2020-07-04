/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "targetsmodel.h"
#include "arenawidget.h"

using namespace tbrpgsca;

TargetsModel::TargetsModel(ArenaWidget& scene, QObject* const parent) : QAbstractListModel(parent)
{
    this->_scene = &scene;
}

TargetsModel::~TargetsModel()
{

}

int TargetsModel::rowCount(const QModelIndex& parent) const
{
    /*int count = 0;
    {
        for (auto party : this->scene->parties)
        {
            count += party->size();
        }
    }*/
    return this->_scene->trgCount;//count;
}

QVariant TargetsModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid())
   {
       return QVariant();
   }

   int const row = index.row();
   Actor& actor = *(this->_scene->getPlayerFromTargetBox(row));

   switch (role)
   {
   case Qt::TextColorRole:
       return QBrush(actor._hp > 0 ? Qt::yellow : Qt::gray);
   case Qt::DisplayRole:
   {
       return row < this->_scene->_parties[0]->size() ? QString(tr("%1 (HP: %2/%3, MP: %4/%5, RP: %6/%7)")).arg(actor._name, QString::number(actor._hp),
            QString::number(actor._m_hp), QString::number(actor._mp), QString::number(actor._m_mp), QString::number(actor._sp), QString::number(actor._m_sp))
               : QString(tr("%1 (HP: %2%)")).arg(actor._name, QString::number((((float)actor._hp) / ((float)actor._m_hp)) * 100.0, 'f', 0));
   }
   default:
       return QVariant();
   }
}
