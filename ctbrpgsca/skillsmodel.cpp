#include "skillsmodel.h"

using namespace tbrpgsca;

SkillsModel::SkillsModel(Actor& actor, QObject* const parent) : QAbstractListModel(parent)
{
    this->actor = &actor;
}

SkillsModel::~SkillsModel()
{

}

int SkillsModel::rowCount(const QModelIndex& parent) const
{
    return this->actor->aSkills->size();
}

QVariant SkillsModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid())
   {
       return QVariant();
   }

   Actor& actor = *(this->actor);
   Ability& ability = *(actor.aSkills->at(index.row()));

   switch (role)
   {
   case Qt::TextColorRole:
       return QBrush(ability.canPerform(actor) ? Qt::yellow : Qt::gray);
   case Qt::DisplayRole:
   {
       int usesQty;
       QMap<Ability*, int>* skillsQty = actor.skillsCrQty;
       return QString(tr("%1 (Lv: %2, HPc: %3, MPc: %4, RPc: %5, Qty: %6)")).arg(ability.name, QString::number(ability.lvRq),
            QString::number(ability.mHp), QString::number(ability.mMp), QString::number(ability.mSp), (skillsQty == nullptr
            || (usesQty = skillsQty->value(&ability, -1)) < 0 ? "∞" : QString::number(usesQty)));
   }
   default:
       return QVariant();
   }
}
