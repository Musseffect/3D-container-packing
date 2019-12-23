#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include "boxtablemodel.h"
#include "solvers.h"
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

    void on_test_render_action_triggered();

    void on_greedy_action_triggered();

    void on_addBoxButton_clicked();

    void on_removeBoxButton_clicked();

    void on_genetic_action_triggered();
    void onSelectionChange(const QItemSelection & selected, const QItemSelection &deselected);
    void on_help_action_triggered();

    void on_openAction_triggered();

    void on_saveAction_triggered();

    void on_bruteforce_action_triggered();

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
