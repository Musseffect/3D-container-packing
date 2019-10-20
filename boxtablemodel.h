#ifndef BOXTABLEMODEL_H
#define BOXTABLEMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include "structs.h"

class BoxTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    BoxTableModel(QObject *parent);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex & index) const Q_DECL_OVERRIDE ;
    const QList<Box>& getBoxes()const;
    bool insertRows(int row, int count, const QModelIndex&);
    bool removeRows(int row, int count, const QModelIndex&);
private:
    QList<Box> boxes;
signals:
    void editCompleted(const QString &);
};

#endif // BOXTABLEMODEL_H
