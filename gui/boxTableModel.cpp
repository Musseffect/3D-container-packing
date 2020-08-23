#include "boxTableModel.h"
#include "random.h"

BoxTableModel::BoxTableModel(QObject *parent): QAbstractTableModel(parent)
{
    boxes.push_back(Box(1,3,5,QColor(255,0,0)));
    boxes.push_back(Box(2,2.5,4,QColor(255,255,255)));
    boxes.push_back(Box(3,2,3,QColor(255,255,0)));
    boxes.push_back(Box(1,4,1,QColor(255,0,255)));
    boxes.push_back(Box(3,3,3,QColor(0,255,255)));
    boxes.push_back(Box(5,2,3,QColor(0,255,0)));
    boxes.push_back(Box(3,1,5,QColor(0,0,255)));
   /* boxes.push_back(Box(1,1,1,QColor(255,0,0)));
    boxes.push_back(Box(2,2,2,QColor(255,255,255)));
    boxes.push_back(Box(3,3,3,QColor(255,255,0)));
    boxes.push_back(Box(4,4,4,QColor(255,0,255)));
    boxes.push_back(Box(5,5,5,QColor(0,0,255)));*/
}
QVariant BoxTableModel::data(const QModelIndex &index, int role)const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= boxes.length() || index.row() < 0||index.column()<0||index.column()>4)
        return QVariant();

    if (role == Qt::DisplayRole||role==Qt::EditRole) {
        Box box=boxes[index.row()];

        switch (index.column()) {
            case 0:
                return box.width();

            case 1:
                return box.height();

            case 2:
                return box.length();

            case 3:
                return box.color();
        }
    }
    return QVariant();
}
int BoxTableModel::rowCount(const QModelIndex& parent) const
{
    //Q_UNUSED(parent);
    return parent.isValid() ? 0 :boxes.length();
}

int BoxTableModel::columnCount(const QModelIndex &parent) const
{
    //Q_UNUSED(parent);
    return parent.isValid() ? 0 : 4;
}
QVariant BoxTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                return tr("Цвет");

            default:
                return QVariant();
        }
    }else
    {
        //нумерация с единицы
        return QString::number(section+1);
    }
    return QVariant();
}
bool BoxTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::EditRole)
    {
        if(index.row()>=boxes.length())
            return false;
        switch(index.column())
        {
            case 0:
            boxes[index.row()].width()=value.toFloat();
                break;
            case 1:
            boxes[index.row()].height()=value.toFloat();
                break;
            case 2:
            boxes[index.row()].length()=value.toFloat();
                break;
            case 3:
            boxes[index.row()].color()=value.value<QColor>();
                break;
            default:
                return false;
        }
        emit(dataChanged(index, index));
    }
    return true;
}
const QList<Box>& BoxTableModel::getBoxes()const
{
    return boxes;
}
Qt::ItemFlags BoxTableModel::flags(const QModelIndex & index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid())
        theFlags |= Qt::ItemIsSelectable|Qt::ItemIsEditable|
        Qt::ItemIsEnabled;
    return theFlags;
}


bool BoxTableModel::insertRows(int row, int count, const QModelIndex&)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i)
    {
        boxes.insert(row,Box(1.0,1.0,1.0,QColor(Random::random(255),Random::random(255),Random::random(255),255)));
    }
    endInsertRows();
    return true;
}
bool BoxTableModel::removeRows(int row, int count, const QModelIndex&)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i)
        boxes.removeAt(row);
    endRemoveRows();
    return true;
}

void BoxTableModel::reset(QList<Box> &boxes)
{
    beginResetModel();
    this->boxes=boxes;
    endResetModel();
}
