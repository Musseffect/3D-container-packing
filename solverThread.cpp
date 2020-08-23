#include "solverThread.h"


void SolverThread::run() {
    connect(this,SIGNAL(progress(int)),solver,SIGNAL(progress(int)));
    try{
        QVector<BoxInfo> placements=solver->solve(boxes,bounds);
        emit showSolution(placements,boxes,bounds,solver->getLog());
    }catch(QString exc)
    {
        emit error(exc);
    }
}

SolverThread::SolverThread(QObject *parent):QThread(parent)
{
}

SolverThread::~SolverThread()
{
    delete solver;
}

void SolverThread::init(BoxPackingSolver *solver, QVector<Box> &boxes, Box &bounds)
{
    this->solver=solver;
    this->boxes=boxes;
    this->bounds=bounds;
}
