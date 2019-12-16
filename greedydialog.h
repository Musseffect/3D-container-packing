#ifndef GREEDYSOLVERDIALOG_H
#define GREEDYSOLVERDIALOG_H

#include <QDialog>

namespace Ui {
class GreedySolverDialog;
}

class GreedyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GreedyDialog(QWidget *parent = 0);
    int getCriteria()const;
    bool getRotateBoxesValue()const;
    ~GreedyDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::GreedySolverDialog *ui;
};

#endif // GREEDYSOLVERDIALOG_H
