#ifndef GENETIC2DIALOG_H
#define GENETIC2DIALOG_H

#include <QDialog>

namespace Ui {
class genetic2dialog;
}

class genetic2dialog : public QDialog
{
    Q_OBJECT

public:
    explicit genetic2dialog(QWidget *parent = 0);
    ~genetic2dialog();
    int getPopulation() const;
    int getSelectionCount()const;
    int getMaxIterations()const;
    float getMaxTime()const;
    float getMutationProb()const;
    float getRequiredVolume()const;
    float getCrossingoverCount()const;
    bool getRotateBoxesValue()const;
    int getRepairAttempts()const;
    int getStrategy()const;


private slots:
    void on_populationSpinBox_valueChanged(int arg1);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::genetic2dialog *ui;
};

#endif // GENETIC2DIALOG_H
