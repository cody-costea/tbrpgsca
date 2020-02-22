#include "targetsmodel.h"

using namespace tbrpgsca;

TargetsModel::TargetsModel(ArenaWidget& scene, QObject* const parent) : QAbstractListModel(parent)
{
    this->scene = &scene;
    this->trgCount = -1;
}

TargetsModel::~TargetsModel()
{

}

int TargetsModel::rowCount(const QModelIndex& parent) const
{
    int count = 0;//this->trgCount;
    //if (count < 0)
    {
        for (auto party : this->scene->parties)
        {
            count += party->size();
        }
        //this->trgCount = count;
    }
    return count;
}

QVariant TargetsModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid())
   {
       return QVariant();
   }

   int const row = index.row();
   Actor& actor = *(this->scene->getPlayerFromTargetBox(row));

   switch (role)
   {
   /*case Qt::TextColorRole:
       return QBrush(Qt::black);*/
   case Qt::DisplayRole:
   {
       return row < this->scene->parties[0]->size() ? QString(tr("%1 (HP: %2/%3, MP: %4/%5, RPc: %6/%7)")).arg(actor.name, QString::number(actor.hp),
            QString::number(actor.mHp), QString::number(actor.mp), QString::number(actor.mMp), QString::number(actor.sp), QString::number(actor.mSp))
               : QString(tr("%1 (HP: %2%%)")).arg(actor.name, QString::number((((float)actor.hp) / ((float)actor.mHp)) * 100.0));
   }
   default:
       return QVariant();
   }
}
