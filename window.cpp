#include "window.h"
#include "ui_window.h"
#include "solutiondialog.h"
#include "greedydialog.h"
#include "geneticdialog.h"
#include "bruteforcedialog.h"
#include "greedysolver.h"
#include "geneticsolver.h"
#include "bruteforcesolver.h"
#include "solverworker.h"
#include "helpwindow.h"
#include "boxtabledelegate.h"

#include <qcompilerdetection.h>

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);
    progressBar=new QProgressBar(ui->statusBar);
    ui->statusBar->addPermanentWidget(progressBar);
    progressBar->setValue(100);
    progressBar->setVisible(false);
    progressBar->setFixedWidth(100);
    progressBar->setFixedHeight(20);

    model=new BoxTableModel(this);

    ui->tableView->setModel(model);
    BoxTableDelegate* delegate=new BoxTableDelegate();
    ui->tableView->setItemDelegate(delegate);
    //ui->tableView->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);

    canBeClosed=true;
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
    qRegisterMetaType<Box>();
    qRegisterMetaType<BoxInfo>();
    qRegisterMetaType<QVector<BoxInfo>>();
    qRegisterMetaType<QVector<Box>>();

    //generate(QVector3D(7,7,7),QVector3D(0.5,0.5,0.5),QVector3D(0.5,0.5,0.5),10,2);
    generate(QVector3D(7,7,7),QVector3D(0.5,0.5,0.5),QVector3D(0.5,0.5,0.5),10,5);
    //generate(QVector3D(9,9,9),QVector3D(0.5,0.5,0.5),QVector3D(0.5,0.5,0.5),20,5);
    //generate(QVector3D(9,9,9),QVector3D(0.5,0.5,0.5),QVector3D(0.5,0.5,0.5),50,4);
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

    //2, 3, -3, 4, -3, 0, 1, 5, -3, -1, 6, -2, -2  !!!! without rotations
    QVector<int> genes=QVector<int>()<<
                                        2<<
                                        3<<
                                        -3<<
                                        1<<
                                        6<<
                                        -3<<
                                        4<<
                                        -1<<
                                        -2<<
                                        5<<
                                        0<<
                                        -1<<
                                        -3;

    QVector<BoxOrientation> orientations=QVector<BoxOrientation>()<<
        BoxOrientation::XYZ0<<
        BoxOrientation::XYZ0<<
        BoxOrientation::XYZ0<<
        BoxOrientation::XYZ0<<
        BoxOrientation::XYZ0<<
        BoxOrientation::XYZ0<<
        BoxOrientation::XYZ0<<
        BoxOrientation::XYZ0<<
        BoxOrientation::XYZ0<<
        BoxOrientation::XYZ0<<
        BoxOrientation::XYZ0<<
        BoxOrientation::XYZ0<<
        BoxOrientation::XYZ0;
    Chromosome c;
    c.genes=genes;
    c.orientations=orientations;
    float w,h,l;
    placements=calculatePlacements(c,boxes,false,w,h,l);
    SolutionDialog* solutionWindow=new SolutionDialog(this);
    solutionWindow->show();
    solutionWindow->setup(placements,boxes,bounds);

    placements=calculatePlacements(c,boxes,true,w,h,l);
    solutionWindow=new SolutionDialog(this);
    solutionWindow->show();
    solutionWindow->setup(placements,boxes,bounds);
}

void Window::on_greedy_action_triggered()
{
    /*QVector<Box> boxes=model->getBoxes().toVector();
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
    }*/

#define MULTITHREAD
#ifdef MULTITHREAD

    QVector<Box> boxes=model->getBoxes().toVector();
    Box bounds=getBounds();

    GreedyDialog* dialog=new GreedyDialog();
    dialog->setModal(true);
    if(dialog->exec()==QDialog::Rejected)
    {
        delete dialog;
        return;
    }
    progressBar->setValue(0);
    progressBar->setVisible(true);
    lockUI();
    int criteria=dialog->getCriteria();
    bool rotateBoxes=dialog->getRotateBoxesValue();
    delete dialog;
    GreedySolver* solver=new GreedySolver();
    solver->init(rotateBoxes,criteria);

    SolverThread *solverThread = new SolverThread(this);
    solverThread->init(solver,boxes,bounds);
    connect(solverThread, &SolverThread::showSolution, this, &Window::showSolution);
    connect(solverThread, &SolverThread::error, this, &Window::showError);
    connect(solverThread, &SolverThread::finished, solverThread, &SolverThread::deleteLater);
    connect(solver, SIGNAL(progress(int)), progressBar, SLOT(setValue(int)));
    connect(qApp, SIGNAL(aboutToQuit()), solverThread, SLOT(deleteLater()));
    solverThread->start();

#endif

}
void Window::closeEvent(QCloseEvent* e)
{
    if ( !canBeClosed )
       e->ignore();
    else
       __super::closeEvent(e);
}
void Window::lockUI()
{
    canBeClosed=false;
    this->ui->solutionMenu->setEnabled(false);
}
void Window::workFinished()
{
    canBeClosed=true;
    progressBar->setValue(100);
    progressBar->setVisible(false);
    this->ui->solutionMenu->setEnabled(true);
}
void Window::showSolution(QVector<BoxInfo> placements,const QVector<Box> boxes,const Box& bounds,const QString& log)
{
    SolutionDialog* solutionWindow=new SolutionDialog(this);
    solutionWindow->setLog(log);
    solutionWindow->show();
    solutionWindow->setup(placements,boxes,bounds);
    workFinished();
}
void Window::showError(QString error)
{
    QMessageBox msgBox;
    msgBox.setText(error);
    msgBox.exec();
    workFinished();
}
void Window::on_genetic_action_triggered()
{
    QVector<Box> boxes=model->getBoxes().toVector();
    Box bounds=getBounds();
    GeneticDialog* dialog=new GeneticDialog();
    dialog->setModal(true);
    if(dialog->exec()==QDialog::Rejected)
    {
        delete dialog;
        return;
    }
    progressBar->setValue(0);
    progressBar->setVisible(true);
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
    lockUI();
    GeneticSolver* solver=new GeneticSolver();
    solver->init(population,
                maxIterations,
                maxTime,
                requiredVolume,
                mutationProb,
                selectionCount,
                crossingoverCount,
                repairAttempts,
                rotateBoxes,
                compressBoxes);
    connect(solver, SIGNAL(progress(int)), progressBar, SLOT(setValue(int)));

    SolverThread *solverThread = new SolverThread(this);
    solverThread->init(solver,boxes,bounds);
    connect(solverThread, &SolverThread::showSolution, this, &Window::showSolution);
    connect(solverThread, &SolverThread::error, this, &Window::showError);
    connect(solverThread, &SolverThread::finished, solverThread, &SolverThread::deleteLater);
    connect(qApp, SIGNAL(aboutToQuit()), solverThread, SLOT(deleteLater()));

    solverThread->start();
}

void Window::on_addBoxButton_clicked()
{
    model->insertRows(model->rowCount(),1,QModelIndex());
}

void Window::on_removeBoxButton_clicked()
{
    QItemSelection selection=ui->tableView->selectionModel()->selection();
    ui->removeBoxButton->clearFocus();
    ui->tableView->selectionModel()->clearSelection();

    for(auto iter=selection.begin();iter!=selection.end();iter++)
    {
        model->removeRows(iter->top(),iter->top()-iter->bottom()+1,QModelIndex());
    }
}

void Window::on_help_action_triggered()
{
    HelpWindow *hw=new HelpWindow(this);
    hw->show();
}

void Window::read(const QJsonObject& json)
{
    //static_assert();
    QList<Box> boxes;
    if(json.contains("boxes")&&json["boxes"].isArray())
    {
        QJsonArray boxArray = json["boxes"].toArray();
        boxes.reserve(boxArray.size());
        for (int i = 0; i < boxArray.size(); ++i) {
            QJsonObject boxObject = boxArray[i].toObject();
            Box box;
            box.read(boxObject);
            boxes.append(box);
        }
    }
    model->reset(boxes);
    if(json.contains("scene")&&json["scene"].isObject())
    {
            Box scene;
            scene.read(json["scene"].toObject());
            ui->widthSpinBox->setValue(scene.w);
            ui->heightSpinBox->setValue(scene.h);
            ui->lengthSpinBox->setValue(scene.l);
    }
}
void Window::write(QJsonObject& json)
{
    //static_assert();
    QJsonArray boxesArray;
    QVector<Box> boxes=model->getBoxes().toVector();
    for(int i=0;i<boxes.size();++i)
    {
        QJsonObject box;
        boxes[i].write(box);
        boxesArray.append(box);
    }
    json["boxes"]=boxesArray;
    Box scene=getBounds();
    QJsonObject sceneJSON;
    scene.write(sceneJSON);
    json["scene"]=sceneJSON;
}
void Window::on_openAction_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::FileMode::ExistingFile);
    dialog.setNameFilter(tr("Json files (*.json)"));
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setDirectory(QCoreApplication::applicationDirPath());

    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    QString fileName;
    if(!dialog.exec())
        return;
    fileName=dialog.selectedFiles().first();

    QFile loadFile(fileName);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    read(loadDoc.object());

    return;
}

void Window::on_saveAction_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::FileMode::AnyFile);
    dialog.setNameFilter(tr("Json files (*.json)"));
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setDirectory(QCoreApplication::applicationDirPath());
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    QString fileName;
    if(!dialog.exec())
        return;
    fileName=dialog.selectedFiles().first();
    QFile saveFile(fileName);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QJsonObject gameObject;
    write(gameObject);
    QJsonDocument saveDoc(gameObject);
    saveFile.write(saveDoc.toJson());
    return;
}

void Window::on_bruteforce_action_triggered()
{
    QVector<Box> boxes=model->getBoxes().toVector();
    Box bounds=getBounds();
    BruteForceDialog* dialog=new BruteForceDialog();
    dialog->setModal(true);
    if(dialog->exec()==QDialog::Rejected)
    {
        delete dialog;
        return;
    }
    progressBar->setValue(0);
    progressBar->setVisible(true);
    int criteria=dialog->getCriteria();
    float maxTime=dialog->getMaxTime();
    bool rotateBoxes=dialog->getRotateBoxesValue();
    delete dialog;
    lockUI();
    BruteForceSolver* solver=new BruteForceSolver();
    solver->init(rotateBoxes,criteria,maxTime);
    connect(solver, SIGNAL(progress(int)), progressBar, SLOT(setValue(int)));

    SolverThread *solverThread = new SolverThread(this);
    solverThread->init(solver,boxes,bounds);
    connect(solverThread, &SolverThread::showSolution, this, &Window::showSolution);
    connect(solverThread, &SolverThread::error, this, &Window::showError);
    connect(solverThread, &SolverThread::finished, solverThread, &SolverThread::deleteLater);
    connect(qApp, SIGNAL(aboutToQuit()), solverThread, SLOT(deleteLater()));

    solverThread->start();
}

void Window::generate(QVector3D max,QVector3D min,QVector3D step,int types,int quantity)
{
    QList<Box> boxes;
    for(int i=0;i<types;i++)
    {
        float w=Random::random(min.x(),max.x());
        w=step.x()*ceil(w/step.x());
        float h=Random::random(min.y(),max.y());
        h=step.y()*ceil(h/step.y());
        float l=Random::random(min.z(),max.z());
        l=step.z()*ceil(l/step.z());
        QColor color=QColor(Random::random(255),Random::random(255),Random::random(255));
        for(int j=0;j<quantity;j++)
            boxes.append(Box(w,h,l,color));
    }
    model->reset(boxes);
}


