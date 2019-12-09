#ifndef BOXPLACEMENTTABLEMODEL_H
#define BOXPLACEMENTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QColor>

struct BoxPlacementStruct
{
    float w;
    float h;
    float l;
    QColor c;
    float px;
    float py;
    float pz;
};


class BoxPlacementTableModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    BoxPlacementTableModel(QObject *parent);
    void init(QList<BoxPlacementStruct>& data);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex & index) const Q_DECL_OVERRIDE ;
signals:

public slots:
private:
    QList<BoxPlacementStruct> modelData;
};

#endif
