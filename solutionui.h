#ifndef SOLUTIONUI_H
#define SOLUTIONUI_H

#include <QDialog>
#include "solvers.h"
#include "boxscene.h"
#include "boxtablemodel.h"



namespace Ui {
class solutionUI;
}

class solutionUI : public QDialog
{
    Q_OBJECT

public:
    explicit solutionUI(QWidget *parent = 0);
    ~solutionUI();
    void setup(QVector<BoxInfo>& placements, QVector<Box> &boxes, Box bounds);
    void initList(QVector<BoxArrayStruct>& boxArray);
    void setLog(QString logText);
private slots:
    void on_freeCamera_clicked();

    void on_oxy_clicked();

    void on_oxz_clicked();

    void on_ozy_clicked();

private:
    Ui::solutionUI *ui;
};

#endif // SOLUTIONUI_H
