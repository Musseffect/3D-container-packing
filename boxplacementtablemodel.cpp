#include "boxplacementtablemodel.h"


BoxPlacementTableModel::BoxPlacementTableModel(QObject *parent)
{

}
void BoxPlacementTableModel::init(QList<BoxPlacementStruct>& data)
{
    beginResetModel();
    modelData=data;
    endResetModel();
}
QVariant BoxPlacementTableModel::headerData(int section, Qt::Orientation orientation, int role)const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("w");
            case 1:
                return tr("h");
            case 2:
                return tr("l");
            case 3:
                return tr("x");
            case 4:
                return tr("y");
            case 5:
                return tr("z");
            case 6:
                return tr("Цвет");
        }
    }else
    {
        //нумерация с единицы
        return QString::number(section+1);
    }
    return QVariant();
}
int BoxPlacementTableModel::rowCount(const QModelIndex &parent)const
{
    return parent.isValid() ? 0 :modelData.length();
}
int BoxPlacementTableModel::columnCount(const QModelIndex &parent)const
{
    return parent.isValid() ? 0 : 7;
}
QVariant BoxPlacementTableModel::data(const QModelIndex &index, int role)const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= modelData.length() || index.row() < 0||index.column()<0||index.column()>6)
        return QVariant();

    if (role == Qt::DisplayRole||role==Qt::EditRole) {
        BoxPlacementStruct element=modelData[index.row()];

        switch (index.column()) {
            case 0:
                return element.w;
            case 1:
                return element.h;
            case 2:
                return element.l;
            case 3:
                return element.px;
            case 4:
                return element.py;
            case 5:
                return element.pz;
            case 6:
                return element.c;
        }
    }
    return QVariant();
}
Qt::ItemFlags BoxPlacementTableModel::flags(const QModelIndex & index)const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid())
        theFlags |= Qt::ItemIsSelectable|
        Qt::ItemIsEnabled;
    return theFlags;
}
