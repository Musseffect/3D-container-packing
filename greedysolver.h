#ifndef GREEDYSOLVER_H
#define GREEDYSOLVER_H
#include "boxpackingsolver.h"

enum GreedyCriteriaType
{
    EUCLDISTMINGLOB=0,
    MANHDISTMINGLOB=1,
    CHEBDISTMINGLOB=2,
    VOLUMEMINGLOB=3,
    EUCLDISTMINLOC=4,
    MANHDISTMINLOC=5,
    CHEBDISTMINLOC=6,
    MAXBOXMINPOS=7
};

class GreedySolver:public BoxPackingSolver
{
private:
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
