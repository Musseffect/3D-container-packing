#ifndef BOXPOSITIONTABLEMODEL_H
#define BOXPOSITIONTABLEMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QColor>
#include "structs.h"


class BoxPositionTableModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    BoxPositionTableModel(QObject *parent);
    void init(QList<BoxPositionStruct>& data);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex & index) const Q_DECL_OVERRIDE ;
signals:

public slots:
private:
    QList<BoxPositionStruct> modelData;
};

#endif
