#include "genetic2dialog.h"
#include "ui_genetic2dialog.h"

genetic2dialog::genetic2dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::genetic2dialog)
{
    ui->setupUi(this);
}
genetic2dialog::~genetic2dialog()
{
    delete ui;
}
int genetic2dialog::getPopulation() const
{
    return ui->populationSpinBox->value();
}
int genetic2dialog::getSelectionCount()const
{
    return ui->selectionSpinBox->value();
}
int genetic2dialog::getMaxIterations()const
{
    return ui->iterationSpinBox->value();
}
float genetic2dialog::getMaxTime()const
{
    return ui->timeSpinBox->value();
}
float genetic2dialog::getMutationProb()const
{
    return ui->mutationProbability->value()/100.0;
}
float genetic2dialog::getRequiredVolume()const
{
    return ui->requiredVolume->value()/100.0;
}
float genetic2dialog::getCrossingoverCount()const
{
    return ui->crossingoverCount->value();
}
bool genetic2dialog::getRotateBoxesValue() const
{
    return ui->rotateBoxes->isChecked();
}
int genetic2dialog::getRepairAttempts() const
{
    return ui->repairAttempts->value();
}

int genetic2dialog::getStrategy() const
{
    return ui->strategy->currentIndex();
}
void genetic2dialog::on_populationSpinBox_valueChanged(int arg1)
{
    ui->selectionSpinBox->setMaximum(arg1);
}

void genetic2dialog::on_buttonBox_accepted()
{
    accept();
}

void genetic2dialog::on_buttonBox_rejected()
{
    reject();
}
