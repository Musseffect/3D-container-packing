#include "geneticHeuristicDialog.h"
#include "ui_geneticHeuristicDialog.h"

GeneticHeuristicDialog::GeneticHeuristicDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeneticHeuristicDialog)
{
    ui->setupUi(this);
}
GeneticHeuristicDialog::~GeneticHeuristicDialog()
{
    delete ui;
}
int GeneticHeuristicDialog::getPopulation() const
{
    return ui->populationSpinBox->value();
}
int GeneticHeuristicDialog::getSelectionCount()const
{
    return ui->selectionSpinBox->value();
}
int GeneticHeuristicDialog::getMaxIterations()const
{
    return ui->iterationSpinBox->value();
}
float GeneticHeuristicDialog::getMaxTime()const
{
    return ui->timeSpinBox->value();
}
float GeneticHeuristicDialog::getMutationProb()const
{
    return ui->mutationProbability->value()/100.0;
}
float GeneticHeuristicDialog::getRequiredVolume()const
{
    return ui->requiredVolume->value()/100.0;
}
float GeneticHeuristicDialog::getCrossingoverCount()const
{
    return ui->crossingoverCount->value();
}
bool GeneticHeuristicDialog::getRotateBoxesValue() const
{
    return ui->rotateBoxes->isChecked();
}
int GeneticHeuristicDialog::getRepairAttempts() const
{
    return ui->repairAttempts->value();
}

int GeneticHeuristicDialog::getStrategy() const
{
    return ui->strategy->currentIndex();
}
void GeneticHeuristicDialog::on_populationSpinBox_valueChanged(int arg1)
{
    ui->selectionSpinBox->setMaximum(arg1);
}

void GeneticHeuristicDialog::on_buttonBox_accepted()
{
    accept();
}

void GeneticHeuristicDialog::on_buttonBox_rejected()
{
    reject();
}
