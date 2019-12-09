#include "boxtabledelegate.h"
#include <QDoubleSpinBox>
#include <QPainter>

BoxTableDelegate::BoxTableDelegate(QObject *parent)
 : QStyledItemDelegate(parent)
{

}

QWidget *BoxTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const
{
    if (index.column() == 0||index.column()==1||index.column()==2)
    {
        QDoubleSpinBox *editor=new QDoubleSpinBox(parent);
        editor->setFrame(false);
        editor->setMinimum(0);
        editor->setMaximum(100);
        editor->setValue(1);
        editor->setSingleStep(0.5);
        return editor;
    }
    else if(index.column()==3)
    {
        ColorDialogWidget *editor=new ColorDialogWidget(index,parent);
        editor->resize(140,140);
        return editor;
    }
    QStyledItemDelegate::createEditor(parent, option, index);
   // emit sizeHintChanged(index);
}
void BoxTableDelegate::paint(QPainter*painter,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
    if(index.column()==3)
    {
        QRect r(option.rect.x()+0.1*option.rect.width(),
                option.rect.y()+0.1*option.rect.height(),
                option.rect.width()*0.8, option.rect.height()*0.8);

        if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());
        else
            painter->fillRect(option.rect,option.palette.background());
        painter->fillRect(r,index.data().value<QColor>());
    }else
        QStyledItemDelegate::paint(painter, option, index);
}
void BoxTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == 0||index.column()==1||index.column()==2)
    {
        float value = index.model()->data(index, Qt::EditRole).toFloat();

        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->setValue(value);
    }else if(index.column()==3)
    {
        QColor color=index.model()->data(index,Qt::DisplayRole).value<QColor>();
        ColorDialogWidget* colorDialogWidget=static_cast<ColorDialogWidget*>(editor);
        colorDialogWidget->setColor(color);
    }
}

void BoxTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                  const QModelIndex &index) const
{
    if(index.column() == 0||index.column()==1||index.column()==2)
    {
        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->interpretText();
        float value = spinBox->value();

        model->setData(index, value, Qt::EditRole);
    }else if(index.column()==3)
    {
        ColorDialogWidget* colorDialogWidget=static_cast<ColorDialogWidget*>(editor);
        model->setData(index, colorDialogWidget->getColor());
    }
}

void BoxTableDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
