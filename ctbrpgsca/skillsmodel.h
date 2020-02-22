#ifndef SKILLSMODEL_H
#define SKILLSMODEL_H

#include <QAbstractListModel>
#include <QBrush>
#include <QColor>

#include "ability.h"
#include "actor.h"

namespace tbrpgsca
{

    class SkillsModel : public QAbstractListModel
    {
        Q_OBJECT
    public:
       SkillsModel(Actor& actor, QObject* const parent = 0);
       ~SkillsModel();

       int rowCount(const QModelIndex& parent = QModelIndex()) const;
       QVariant data(const QModelIndex& index, int role) const;

    private:
       Actor* actor;
    };

}

#endif // SKILLSMODEL_H
