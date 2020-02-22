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
        int trgCount;
        ArenaWidget* scene;
    };

}

#endif // TARGETSMODEL_H
