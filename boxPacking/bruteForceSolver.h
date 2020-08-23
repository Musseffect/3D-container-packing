#ifndef BRUTEFORCESOLVER_H
#define BRUTEFORCESOLVER_H

#include "boxPackingSolver.h"



class BruteForceSolver:public BoxPackingSolver
{
    struct SolutionFitness
    {
        float volume;
        int skip;
    };
public:
    enum BruteForceCriteriaType
    {
        EUCL_NORM=0,
        MANH_NORM=1,
        CHEB_NORM=2,
        VOLUME=3,
        EUCL_NORM_LOC=4,
        MANH_NORM_LOC=5,
        CHEB_NORM_LOC=6,
        SURFACE_AREA=7,
        EMPTY_VOLUME=8
    };
private:
    bool rotateBoxes;
    int placementCriteria;
    float maxTime;
public slots:
public:
    static QString criteriaNames[];
    BruteForceSolver();
    void init(bool rotateBoxes,int placementCriteria,float maxTime);
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
