#ifndef GENETICALTSOLVER_H
#define GENETICALTSOLVER_H

#include "boxpackingsolver.h"



enum PlacementStrategy
{
    VOLUME=0,
    EUCLIDIAN_NORM=1,
    MANHATTAN_NORM=2,
    CHEBYSHEV_NORM=3,
    CLOSEST_POINT_EUCL=4,
    CLOSEST_POINT_MANH=5,
    CLOSEST_POINT_CHEB=6
};


class GeneticAltSolver:public BoxPackingSolver
{
public:
    struct Chromosome
    {
        QVector<int> genes;
        QVector<BoxOrientation> orientations;


        float getBoxCount()const;
        Chromosome();
        Chromosome(const Chromosome &b);
        Chromosome(int boxCount,bool rotateBoxes);
        Chromosome Chromosome::operator=(const Chromosome &b);
        QVector<BoxInfo> placeBoxes(const QVector<Box>& boxes, const Box& bounds, int strategy, float &w, float &h, float &l)const;
        float objectiveFunction(const QVector<Box>& boxes, const Box& bounds, int strategy)const;
        friend Chromosome crossingover(const Chromosome& solution1,const Chromosome& solution2);
        void mutation(bool rotateBoxes);
        void reduction(bool rotateBoxes);
    };
private:
    int strategy;
    int population;
    int maxIterations;
    float maxTime;
    float requiredVolume;
    float mutationProb;
    int selectionCount;
    int crossingoverCount;
    bool rotateBoxes;
    int repairAttempts;
    void setupLog();
public:
    GeneticAltSolver();
    void init(int population,
              int maxIterations,
              float maxTime,
              float requiredVolume,
              float mutationProb,
              int selectionCount,
              int crossingoverCount,
              int repairAttempts,
              bool rotateBoxes,
              int strategy);
    virtual QVector<BoxInfo> solve(const QVector<Box>& boxes,const Box& bounds) override;
};
void selection(QList<GeneticAltSolver::Chromosome>& solutions,QList<float> objectiveValues,float boundsVolume,int selectionCount);

#endif // GENETICALTSOLVER_H
