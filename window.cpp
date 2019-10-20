#include "window.h"
#include "ui_window.h"
#include "solutionui.h"
#include "geneticsolverdialog.h"
#include "boxtabledelegate.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);
    model=new BoxTableModel(this);

    ui->tableView->setModel(model);
    BoxTableDelegate* delegate=new BoxTableDelegate();
    ui->tableView->setItemDelegate(delegate);
    //ui->tableView->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);

    //https://habr.com/ru/post/457292/
        //order independed transparency
    ui->removeBoxButton->setEnabled(false);

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(
                ui->tableView->selectionModel(),
                SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                this,
                SLOT(onSelectionChange(const QItemSelection &, const QItemSelection &)));
    //FOR TEST PURPOSES ONLY
    //on_test_render_action_triggered();
}
void Window::onSelectionChange(const QItemSelection & selected, const QItemSelection &deselected)
{
    if(selected.isEmpty())
    {
        ui->removeBoxButton->setEnabled(false);
    }else
        ui->removeBoxButton->setEnabled(true);
}
Window::~Window()
{
    delete ui;
    delete model;
}
Box Window::getBounds()
{
    Box bounds;
    bounds.w=ui->widthSpinBox->value();
    bounds.h=ui->heightSpinBox->value();
    bounds.l=ui->lengthSpinBox->value();
    return bounds;
}

void Window::on_test_render_action_triggered()
{
    QVector<BoxInfo> placements;
    QList<Box> boxes=model->getBoxes();
    Box bounds=getBounds();

    solutionUI* solutionWindow=new solutionUI(this);
    solutionWindow->show();
    solutionWindow->setup(placements,boxes,bounds);
}

void Window::on_greedy_action_triggered()
{
    QList<Box> boxes=model->getBoxes();
    Box bounds=getBounds();
    try{
        QString log;
        QVector<BoxInfo> placements=solveGreedy(boxes,bounds,log);
        solutionUI* solutionWindow=new solutionUI(this);
        solutionWindow->setLog(log);
        solutionWindow->show();
        solutionWindow->setup(placements,boxes,bounds);
    }catch(QString exc)
    {
        QMessageBox msgBox;
        msgBox.setText(exc);
        msgBox.exec();
    }
}
void Window::on_genetic_action_triggered()
{
    QList<Box> boxes=model->getBoxes();
    Box bounds=getBounds();
    GeneticSolverDialog* dialog=new GeneticSolverDialog();
    dialog->setModal(true);
    if(dialog->exec()==QDialog::Rejected)
    {
        delete dialog;
        return;
    }
    int population=dialog->getPopulation();
    int maxIterations=dialog->getMaxIterations();
    float maxTime=dialog->getMaxTime();
    float requiredVolume=dialog->getRequiredVolume();
    float mutationProb=dialog->getMutationProb();
    int selectionCount=dialog->getSelectionCount();
    float crossingoverCount=dialog->getCrossingoverCount();
    delete dialog;

    try{
        QString log;
        QVector<BoxInfo> placements=solveGenetic(boxes,bounds,population,
                                                 maxIterations,maxTime,requiredVolume,
                                                 mutationProb,selectionCount,crossingoverCount,log);
        solutionUI* solutionWindow=new solutionUI(this);
        solutionWindow->setLog(log);
        solutionWindow->show();
        solutionWindow->setup(placements,boxes,bounds);
    }catch(QString exc)
    {
        QMessageBox msgBox;
        msgBox.setText(exc);
        msgBox.exec();
    }
}

void Window::on_addBoxButton_clicked()
{
    //TODO:
    model->insertRows(model->rowCount(),1,QModelIndex());
}

void Window::on_removeBoxButton_clicked()
{
    //TODO:
    QItemSelection selection=ui->tableView->selectionModel()->selection();
    ui->tableView->selectionModel()->clearSelection();

    //model->removeRows(,,);

}
