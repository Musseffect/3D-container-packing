#ifndef COLORDIALOGWIDGET_H
#define COLORDIALOGWIDGET_H
#include <QToolButton>
#include <QColorDialog>
#include <QModelIndex>

class ColorDialogWidget:public QToolButton
{
    Q_OBJECT
public:
    ColorDialogWidget(const QModelIndex &index, QWidget *parent = 0);
    QColor getColor();
    void setColor(QColor color);
protected:
    void paintEvent(QPaintEvent *event);
signals:

private slots:
    void pressButton ( );
private:
    QModelIndex indexSource;
    QColor color;
};

#endif // COLORDIALOGWIDGET_H
