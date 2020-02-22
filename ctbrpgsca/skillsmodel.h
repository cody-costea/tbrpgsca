#ifndef SKILLSMODEL_H
#define SKILLSMODEL_H

#include <QAbstractListModel>
#include <QBrush>
#include <QColor>

#include "ability.h"

namespace tbrpgsca
{

    class SkillsModel : public QAbstractListModel
    {
        Q_OBJECT
    public:
       SkillsModel(Actor& actor, QVector<Ability*>& abilities, QObject* const parent = 0);
       ~SkillsModel();

       int rowCount(const QModelIndex& parent = QModelIndex()) const;
       QVariant data(const QModelIndex& index, int role) const;

    private:
       QVector<Ability*>* abilities;
       Actor* actor;
    };

}

#endif // SKILLSMODEL_H
