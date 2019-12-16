#ifndef SOLUTIONUI_H
#define SOLUTIONUI_H

#include <QDialog>
#include "solvers.h"
#include "boxscene.h"
#include "boxtablemodel.h"
#include "boxplacementtablemodel.h"



namespace Ui {
class SolutionDialog;
}

class SolutionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SolutionDialog(QWidget *parent = 0);
    ~SolutionDialog();
    void setup(QVector<BoxInfo>& placements, const QVector<Box> &boxes, const Box bounds);
    void setLog(QString logText);
private slots:
    void on_freeCamera_clicked();
    void on_oxy_clicked();
    void on_oxz_clicked();
    void on_ozy_clicked();
    void onSelectionChange(const QItemSelection &, const QItemSelection &);
    void on_horizontalSlider_valueChanged(int value);
    void updateSlider(float distance);
private:
    Ui::SolutionDialog *ui;
    BoxPlacementTableModel* model;
};

#endif // SOLUTIONUI_H
