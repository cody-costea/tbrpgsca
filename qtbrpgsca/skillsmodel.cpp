/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "skillsmodel.h"

using namespace tbrpgsca;

SkillsModel::SkillsModel(Actor& actor, QObject* const parent) : QAbstractListModel(parent)
{
    this->_actor = &actor;
}

SkillsModel::~SkillsModel()
{

}

int SkillsModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return this->_actor->_costume_data->_a_skills->size();
}

QVariant SkillsModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid())
   {
       return QVariant();
   }

   Actor* const actor = (this->_actor);
   Ability& ability = *(actor->_costume_data->_a_skills->at(index.row()));

   switch (role)
   {
   case Qt::TextColorRole:
       return QBrush(ability.canPerform(actor) ? Qt::yellow : Qt::gray);
   case Qt::DisplayRole:
   {
       int usesQty;
       QMap<const Ability*, int>* skillsQty = actor->_actor_data->_skills_cr_qty;
       return QString(tr("%1 (Lv: %2, HPc: %3, MPc: %4, RPc: %5, Qty: %6)")).arg(ability.name(), QString::number(ability.requiredLevel()),
            QString::number(ability.maximumHp()), QString::number(ability.maximumMp()), QString::number(ability.maximumRp()), (skillsQty == nullptr
            || (usesQty = skillsQty->value(&ability, -1)) < 0 ? "∞" : QString::number(usesQty)));
   }
   default:
       return QVariant();
   }
}
