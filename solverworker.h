#ifndef WORKERS_H
#define WORKERS_H

#include "structs.h"
#include "boxpackingsolver.h"
#include <QThread>
#include <QObject>

class SolverThread:public QThread
{
    Q_OBJECT
    void run() override {
        connect(this,SIGNAL(progress(int)),solver,SIGNAL(progress(int)));
        try{
            QVector<BoxInfo> placements=solver->solve(boxes,bounds);
            emit showSolution(placements,boxes,bounds,solver->getLog());
        }catch(QString exc)
        {
            emit error(exc);
        }
    }
public:
    SolverThread(QObject * parent = 0):QThread(parent)
    {
    }
    ~SolverThread()
    {
        delete solver;
    }
    void init(BoxPackingSolver* solver,QVector<Box> &boxes,Box &bounds)
    {
        this->solver=solver;
        this->boxes=boxes;
        this->bounds=bounds;
    }
signals:
    void showSolution(QVector<BoxInfo> placements,const QVector<Box> boxes,const Box &bounds,const QString& log);
    void error(QString& err);
    void progress(int value);
private:
    SolverThread(const SolverThread&);
    BoxPackingSolver* solver;
    QVector<Box> boxes;
    Box bounds;

};


/*
class SolverWorker: public QObject
{
    Q_OBJECT
public:
    SolverWorker(BoxPackingSolver* solver,QVector<Box> &boxes,Box &bounds)
    {
        this->solver=solver;
        this->boxes=boxes;
        this->bounds=bounds;
    }
    void process()
    {
        try{
            QVector<BoxInfo> placements=solver->solve(boxes,bounds);
            emit showSolution(placements,boxes,bounds,solver->getLog());
        }catch(QString exc)
        {
            emit error(exc);
        }
        emit finished();
    }
    ~SolverWorker()
    {
        delete solver;
    }
signals:
    void showSolution(QVector<BoxInfo>&,QVector<Box>&,Box&,QString& log);
    void finished();
    void error(QString err);
private:
    BoxPackingSolver* solver;
    QVector<Box> boxes;
    Box bounds;
};
*/

#endif // WORKERS_H
