#include "skillsmodel.h"

using namespace tbrpgsca;

SkillsModel::SkillsModel(Actor& actor, QVector<Ability*>& abilities, QObject* const parent) : QAbstractListModel(parent)
{
    this->abilities = &abilities;
    this->actor = &actor;
}

SkillsModel::~SkillsModel()
{

}

int SkillsModel::rowCount(const QModelIndex& parent) const
{
    return this->abilities->size();
}

QVariant SkillsModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid())
   {
       return QVariant();
   }

   Ability& ability = *(this->abilities->at(index.row()));
   //Actor& actor = *(this->actor);

   switch (role)
   {
   case Qt::TextColorRole:
       return QBrush(ability.canPerform(*this->actor) ? Qt::black : Qt::gray);
   case Qt::DisplayRole:
       return QString("%1 (Lv: %2, HPc: %3, MPc: %4, RPc: %5)").arg(ability.name, QString::number(ability.lvRq),
            QString::number(ability.mHp), QString::number(ability.mMp), QString::number(ability.mSp));
   default:
       return QVariant();
   }
}
