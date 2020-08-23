#ifndef HELPWINDOW_H
#define HELPWINDOW_H
#include <QDialog>


class HelpWindow : public QDialog
{
    Q_OBJECT
public:
    explicit HelpWindow(QWidget *parent = 0);
};

#endif // HELPWINDOW_H
