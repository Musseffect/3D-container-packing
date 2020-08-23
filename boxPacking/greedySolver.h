#ifndef GREEDYSOLVER_H
#define GREEDYSOLVER_H
#include "boxPackingSolver.h"


class GreedySolver:public BoxPackingSolver
{
private:
    static QString criteriaNames[];
    bool rotateBoxes;
    int greedCriteria;
    QVector<BoxInfo> solveMaxBoxMinPos(const QVector<Box> &boxes, const Box &bounds);
public slots:
public:
    GreedySolver();
    void init(bool rotateBoxes,int greedCriteria);
    virtual QVector<BoxInfo> solve(const QVector<Box>& boxes,const Box& bounds) override;

};

#endif // GREEDYSOLVER_H
