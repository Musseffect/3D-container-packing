#include "greedydialog.h"
#include "ui_greedydialog.h"

GreedyDialog::GreedyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GreedySolverDialog)
{
    ui->setupUi(this);
}

int GreedyDialog::getCriteria() const
{
    return ui->criteria->currentIndex();
}

bool GreedyDialog::getRotateBoxesValue() const
{
    return ui->rotateBoxes->isChecked();
}

GreedyDialog::~GreedyDialog()
{
    delete ui;
}

void GreedyDialog::on_buttonBox_accepted()
{
    accept();
}

void GreedyDialog::on_buttonBox_rejected()
{
    reject();
}

