#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include "boxtablemodel.h"
#include "solvers.h"
#include <QItemSelection>

namespace Ui {
class Window;
}

class Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = 0);
    ~Window();

private slots:

    void on_test_render_action_triggered();

    void on_greedy_action_triggered();

    void on_addBoxButton_clicked();

    void on_removeBoxButton_clicked();

    void on_genetic_action_triggered();
    void onSelectionChange(const QItemSelection & selected, const QItemSelection &deselected);
    void on_help_action_triggered();

private:
    Box getBounds();
    Ui::Window *ui;
    BoxTableModel * model;
};

#endif // WINDOW_H
