#ifndef BRUTEFORCEDIALOG_H
#define BRUTEFORCEDIALOG_H

#include <QDialog>

namespace Ui {
class BruteForceDialog;
}

class BruteForceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BruteForceDialog(QWidget *parent = 0);
    int getCriteria()const;
    float getMaxTime()const;
    bool getRotateBoxesValue()const;
    ~BruteForceDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::BruteForceDialog *ui;
};

#endif // BRUTEFORCEDIALOG_H
