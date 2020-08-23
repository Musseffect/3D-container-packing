#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include "boxTableModel.h"
#include <QItemSelection>
#include <QtConcurrent/QtConcurrent>
#include <QProgressBar>
#include <QVector3D>

namespace Ui {
class Window;
}

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = 0);
    ~Window();
public slots:
    void lockUI();
    void workFinished();
    void showSolution(QVector<BoxInfo> placements, const QVector<Box> boxes, const Box &bounds, const QString &log);
    void showError(QString error);
private slots:
    void on_addBoxButton_clicked();
    void on_removeBoxButton_clicked();
    void onSelectionChange(const QItemSelection & selected, const QItemSelection &deselected);
    void on_helpAction_triggered();
    void on_openAction_triggered();
    void on_saveAction_triggered();
    void on_bruteForceAction_triggered();
    void on_geneticHeuristicAction_triggered();
    void on_greedyAction_triggered();
    void on_geneticRpnAction_triggered();
private:
    void closeEvent(QCloseEvent* e);
    void read(const QJsonObject& json);
    void write(QJsonObject& json);
    void generate(QVector3D max,QVector3D min,QVector3D step,int types,int quantity);
    Box getBounds();
    Ui::Window *ui;
    BoxTableModel * model;
    QProgressBar *progressBar;
    bool canBeClosed;
};

#endif // WINDOW_H
