#ifndef BRUTEFORCESOLVER_H
#define BRUTEFORCESOLVER_H

#include "boxpackingsolver.h"



class BruteForceSolver:public BoxPackingSolver
{
    struct solutionFitness
    {
        float volume;
        int skip;
    };
public:
    enum BruteForceCriteriaType
    {
        EUCLDISTMIN=0,
        MANHDISTMIN=1,
        CHEBDISTMIN=2,
        VOLUMEMINGLOB=3,
        EUCLDISTMINLOC=4,
        MANHDISTMINLOC=5,
        CHEBDISTMINLOC=6
    };
private:
    bool rotateBoxes;
    int placeCriteria;
    float maxTime;
public slots:
public:
    BruteForceSolver();
    void init(bool rotateBoxes,int placeCriteria,float maxTime);
    virtual QVector<BoxInfo> solve(const QVector<Box>& boxes,const Box& bounds) override;
    QVector<BoxInfo> BruteForceSolver::decodeSolution(const QVector<int>& rotationSequence,const QVector<int>& boxSequence,const QVector<Box>& boxes,const Box& bounds,float&w,float&h,float&l);
    float BruteForceSolver::evalSolution(const QVector<int>& rotationSequence,const QVector<int>& boxSequence,const QVector<Box>& boxes,const Box& bounds);
    /*
    for each sequence box and rotation

    max number of variants:
    n!*6^n or n! without rotations
    criterias:
    closest point
    minimal volume
    3x minimal distance to maximal point
    */
};

#endif // BRUTEFORCESOLVER_H
