#include "greedysolverdialog.h"
#include "ui_greedysolverdialog.h"

GreedySolverDialog::GreedySolverDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GreedySolverDialog)
{
    ui->setupUi(this);
}

int GreedySolverDialog::getCriteria() const
{
    return ui->comboBox->currentIndex();
}

bool GreedySolverDialog::getRotateBoxesValue() const
{
    return ui->rotateBoxes->isChecked();
}

GreedySolverDialog::~GreedySolverDialog()
{
    delete ui;
}

void GreedySolverDialog::on_buttonBox_accepted()
{
    accept();
}

void GreedySolverDialog::on_buttonBox_rejected()
{
    reject();
}

