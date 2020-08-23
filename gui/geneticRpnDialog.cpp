#include "geneticRpnDialog.h"
#include "ui_geneticRpnDialog.h"

GeneticRpnDialog::GeneticRpnDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeneticRpnDialog)
{
    ui->setupUi(this);
}

GeneticRpnDialog::~GeneticRpnDialog()
{
    delete ui;
}

int GeneticRpnDialog::getPopulation() const
{
    return ui->populationSpinBox->value();
}
int GeneticRpnDialog::getSelectionCount()const
{
    return ui->selectionSpinBox->value();
}
int GeneticRpnDialog::getMaxIterations()const
{
    return ui->iterationSpinBox->value();
}
float GeneticRpnDialog::getMaxTime()const
{
    return ui->timeSpinBox->value();
}
float GeneticRpnDialog::getMutationProb()const
{
    return ui->mutationProbability->value()/100.0;
}
float GeneticRpnDialog::getRequiredVolume()const
{
    return ui->requiredVolume->value()/100.0;
}
float GeneticRpnDialog::getCrossingoverCount()const
{
    return ui->crossingoverCount->value();
}

bool GeneticRpnDialog::getRotateBoxesValue() const
{
    return ui->rotateBoxes->isChecked();
}

int GeneticRpnDialog::getRepairAttempts() const
{
    return ui->repairAttempts->value();
}

bool GeneticRpnDialog::getCompressBoxesValue() const
{
    return ui->boxCompression->isChecked();
}

void GeneticRpnDialog::on_populationSpinBox_valueChanged(int arg1)
{
    ui->selectionSpinBox->setMaximum(arg1);
}

void GeneticRpnDialog::on_buttonBox_accepted()
{
    accept();
}

void GeneticRpnDialog::on_buttonBox_rejected()
{
    reject();
}
