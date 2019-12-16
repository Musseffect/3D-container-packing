#include "geneticdialog.h"
#include "ui_geneticdialog.h"

GeneticDialog::GeneticDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeneticSolverDialog)
{
    ui->setupUi(this);
}

GeneticDialog::~GeneticDialog()
{
    delete ui;
}

int GeneticDialog::getPopulation() const
{
    return ui->populationSpinBox->value();
}
int GeneticDialog::getSelectionCount()const
{
    return ui->selectionSpinBox->value();
}
int GeneticDialog::getMaxIterations()const
{
    return ui->iterationSpinBox->value();
}
float GeneticDialog::getMaxTime()const
{
    return ui->timeSpinBox->value();
}
float GeneticDialog::getMutationProb()const
{
    return ui->mutationProbability->value()/100.0;
}
float GeneticDialog::getRequiredVolume()const
{
    return ui->requiredVolume->value()/100.0;
}
float GeneticDialog::getCrossingoverCount()const
{
    return ui->crossingoverCount->value();
}

bool GeneticDialog::getRotateBoxesValue() const
{
    return ui->rotateBoxes->isChecked();
}

int GeneticDialog::getRepairAttempts() const
{
    return ui->repairAttempts->value();
}

bool GeneticDialog::getCompressBoxesValue() const
{
    return ui->boxCompression->isChecked();
}

void GeneticDialog::on_populationSpinBox_valueChanged(int arg1)
{
    ui->selectionSpinBox->setMaximum(arg1);
}

void GeneticDialog::on_buttonBox_accepted()
{
    accept();
}

void GeneticDialog::on_buttonBox_rejected()
{
    reject();
}
