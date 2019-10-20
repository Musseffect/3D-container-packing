#include "geneticsolverdialog.h"
#include "ui_geneticsolverdialog.h"

GeneticSolverDialog::GeneticSolverDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeneticSolverDialog)
{
    ui->setupUi(this);
}

GeneticSolverDialog::~GeneticSolverDialog()
{
    delete ui;
}

int GeneticSolverDialog::getPopulation() const
{
    return ui->populationSpinBox->value();
}
int GeneticSolverDialog::getSelectionCount()const
{
    return ui->selectionSpinBox->value();
}
int GeneticSolverDialog::getMaxIterations()const
{
    return ui->iterationSpinBox->value();
}
float GeneticSolverDialog::getMaxTime()const
{
    return ui->timeSpinBox->value();
}
float GeneticSolverDialog::getMutationProb()const
{
    return ui->mutationProbability->value()/100.0;
}
float GeneticSolverDialog::getRequiredVolume()const
{
    return ui->requiredVolume->value()/100.0;
}
float GeneticSolverDialog::getCrossingoverCount()const
{
    return ui->crossingoverCount->value();
}

void GeneticSolverDialog::on_populationSpinBox_valueChanged(int arg1)
{
    ui->selectionSpinBox->setMaximum(arg1);
}

void GeneticSolverDialog::on_buttonBox_accepted()
{
    accept();
}

void GeneticSolverDialog::on_buttonBox_rejected()
{
    reject();
}
