/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "itemsmodel.h"

using namespace tbrpgsca;

ItemsModel::ItemsModel(Actor& actor, QObject* const parent) : QAbstractListModel(parent)
{
    this->_actor = &actor;
}

ItemsModel::~ItemsModel()
{

}

int ItemsModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    const auto& items = this->_actor->actorData()._items;
    return items == nullptr ? 0 : items->size();
}

QVariant ItemsModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid())
   {
       return QVariant();
   }

   Actor* const actor = this->_actor;
   const auto& items = actor->actorData()._items;
   if (items == nullptr)
   {
       return QVariant();
   }
   auto const it = items->cbegin() + index.row();
   if (it == items->cend())
   {
       return QVariant();
   }

   Ability& ability = *(it.key());

   switch (role)
   {
   case Qt::TextColorRole:
       return QBrush(ability.canPerform(actor) ? Qt::yellow : Qt::gray);
   case Qt::DisplayRole:
   {
       int usesQty;
       //QMap<const Ability*, int>* skillsQty = actor->actorData()._skills_cr_qty;
       return QString(tr("%7 X %1 (Lv: %2, HPc: %3, MPc: %4, RPc: %5, Qty: %6)")).arg(ability.name(), QString::number(ability.requiredLevel()),
            QString::number(ability.maximumHp()), QString::number(ability.maximumMp()), QString::number(ability.maximumRp()), (ability.maximumUses() == 0
            ? "∞" : ((usesQty = ability.property("currentUses").toInt()) < 0 ? "!" : QString::number(usesQty))), QString::number(it.value()));
   }
   default:
       return QVariant();
   }
}
