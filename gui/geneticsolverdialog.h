#ifndef GENETICSOLVERDIALOG_H
#define GENETICSOLVERDIALOG_H

#include <QDialog>

namespace Ui {
class GeneticSolverDialog;
}

class GeneticSolverDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GeneticSolverDialog(QWidget *parent = 0);
    ~GeneticSolverDialog();
    int getPopulation() const;
    int getSelectionCount()const;
    int getMaxIterations()const;
    float getMaxTime()const;
    float getMutationProb()const;
    float getRequiredVolume()const;
    float getCrossingoverCount()const;

private slots:
    void on_populationSpinBox_valueChanged(int arg1);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::GeneticSolverDialog *ui;
};

#endif // GENETICSOLVERDIALOG_H
