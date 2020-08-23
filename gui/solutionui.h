#ifndef SOLUTIONUI_H
#define SOLUTIONUI_H

#include <QDialog>
#include "solvers.h"
#include "boxscene.h"
#include "boxtablemodel.h"



namespace Ui {
class solutionUI;
}

class solutionUI : public QDialog
{
    Q_OBJECT

public:
    explicit solutionUI(QWidget *parent = 0);
    ~solutionUI();
    void setup(QVector<BoxInfo>& placements, QList<Box>& boxes, Box bounds);
    void initList(QVector<BoxArrayStruct>& boxArray);
    void setLog(QString logText);
private:
    Ui::solutionUI *ui;
};

#endif // SOLUTIONUI_H
