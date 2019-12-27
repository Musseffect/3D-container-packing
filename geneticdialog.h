#ifndef GENETICSOLVERDIALOG_H
#define GENETICSOLVERDIALOG_H

#include <QDialog>

namespace Ui {
class GeneticSolverDialog;
}

class GeneticDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GeneticDialog(QWidget *parent = 0);
    ~GeneticDialog();
    int getPopulation() const;
    int getSelectionCount()const;
    int getMaxIterations()const;
    float getMaxTime()const;
    float getMutationProb()const;
    float getRequiredVolume()const;
    float getCrossingoverCount()const;
    int getRepairAttempts()const;
    bool getRotateBoxesValue()const;
    bool getCompressBoxesValue()const;

private slots:
    void on_populationSpinBox_valueChanged(int arg1);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::GeneticSolverDialog *ui;
};

#endif // GENETICSOLVERDIALOG_H
