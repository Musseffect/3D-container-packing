#include "colorDialogWidget.h"

#include <QPainter>


ColorDialogWidget::ColorDialogWidget(const QModelIndex &index, QWidget *parent):QToolButton(parent)
{
    indexSource=index;
    connect(this,SIGNAL(clicked(bool)),this,SLOT(pressButton()));
}

QColor ColorDialogWidget::getColor()
{
    return color;
}

void ColorDialogWidget::setColor(QColor color)
{
    this->color=color;
}

void ColorDialogWidget::paintEvent(QPaintEvent *event)
{
    QToolButton::paintEvent(event);
    QRect r(0, 0, width()*0.9, height()*0.9);
    r.moveTo(rect().center()-r.center());
    QPainter painter(this);
    painter.setBrush(color);
    painter.drawRect(r);
}

void ColorDialogWidget::pressButton()
{
    color = QColorDialog::getColor(this->color, this, "Выберите цвет", QColorDialog::DontUseNativeDialog);
}
