#include "bruteforcedialog.h"
#include "ui_bruteforcedialog.h"

BruteForceDialog::BruteForceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BruteForceDialog)
{
    ui->setupUi(this);
}

int BruteForceDialog::getCriteria() const
{
    return ui->criteria->currentIndex();
}

float BruteForceDialog::getMaxTime() const
{
    return ui->timeSpinBox->value();
}

bool BruteForceDialog::getRotateBoxesValue() const
{
    return ui->rotateBoxes->isChecked();
}

BruteForceDialog::~BruteForceDialog()
{
    delete ui;
}

void BruteForceDialog::on_buttonBox_accepted()
{
    accept();
}

void BruteForceDialog::on_buttonBox_rejected()
{
    reject();
}
