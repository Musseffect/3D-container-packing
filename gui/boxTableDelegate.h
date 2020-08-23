#ifndef SPINBOXDELEGATE_H
#define SPINBOXDELEGATE_H
#include <QWidget>
#include <QStyledItemDelegate>
#include "colorDialogWidget.h"

class BoxTableDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    BoxTableDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const Q_DECL_OVERRIDE;

    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const Q_DECL_OVERRIDE;
    void BoxTableDelegate::paint(QPainter*painter,const QStyleOptionViewItem& option,const QModelIndex& index)const Q_DECL_OVERRIDE;
    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // SPINBOXDELEGATE_H
