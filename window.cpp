#include "window.h"
#include "ui_window.h"
#include "solutionui.h"
#include "greedysolverdialog.h"
#include "geneticsolverdialog.h"
#include "boxtabledelegate.h"
#include "greedysolver.h"
#include "geneticsolver.h"

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
    if(ui->tableView->selectionModel()->selection().isEmpty())
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
    QVector<Box> boxes=model->getBoxes().toVector();
    Box bounds=getBounds();
    //2 3 -3 1 6 -3 4 -1 -2 5 0 -1 -3
    //packing test without rotations
    //5 4 -2 0 6 1 3 -3 2 -1 -2 -1 -3
    solutionUI* solutionWindow=new solutionUI(this);
    solutionWindow->show();
    solutionWindow->setup(placements,boxes,bounds);
}

void Window::on_greedy_action_triggered()
{
    QVector<Box> boxes=model->getBoxes().toVector();
    Box bounds=getBounds();
    GreedySolverDialog* dialog=new GreedySolverDialog();
    dialog->setModal(true);
    if(dialog->exec()==QDialog::Rejected)
    {
        delete dialog;
        return;
    }
    int criteria=dialog->getCriteria();
    bool rotateBoxes=dialog->getRotateBoxesValue();
    delete dialog;
    try{
        GreedySolver solver;
        solver.init(rotateBoxes,criteria);
        QVector<BoxInfo> placements=solver.solve(boxes,bounds);
        //QVector<BoxInfo> placements=solveGreedy(boxes,bounds,criteria,rotateBoxes,log);
        solutionUI* solutionWindow=new solutionUI(this);
        solutionWindow->setLog(solver.getLog());
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
    QVector<Box> boxes=model->getBoxes().toVector();
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
    bool rotateBoxes=dialog->getRotateBoxesValue();
    bool compressBoxes=dialog->getCompressBoxesValue();
    int repairAttempts=dialog->getRepairAttempts();
    delete dialog;

    try{
        GeneticSolver solver;
        solver.init(population,
                    maxIterations,
                    maxTime,
                    requiredVolume,
                    mutationProb,
                    selectionCount,
                    crossingoverCount,
                    repairAttempts,
                    rotateBoxes,
                    compressBoxes);
        /*QVector<BoxInfo> placements=solveGenetic(boxes,bounds,population,
                                                 maxIterations,maxTime,requiredVolume,
                                                 mutationProb,selectionCount,crossingoverCount
                                                 ,repairAttempts,
                                                 rotateBoxes,compressBoxes,log);*/
        QVector<BoxInfo> placements=solver.solve(boxes,bounds);
        solutionUI* solutionWindow=new solutionUI(this);
        solutionWindow->setLog(solver.getLog());
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
    ui->removeBoxButton->clearFocus();
    ui->tableView->selectionModel()->clearSelection();

    //model->removeRows(,,);
    for(auto iter=selection.begin();iter!=selection.end();iter++)
    {
        //qDebug()<<"bottom-top"<<iter->bottom()<<iter->top();
        //model->removeRows(iter->bottom(),1,QModelIndex());
        model->removeRows(iter->top(),iter->top()-iter->bottom()+1,QModelIndex());
    }
}

void Window::on_help_action_triggered()
{

}
