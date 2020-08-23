#ifndef GENETICHEURISTICSOLVER_H
#define GENETICHEURISTICSOLVER_H

#include "boxPackingSolver.h"



enum PlacementStrategy
{
    VOLUME=0,
    EUCL_NORM=1,
    MANH_NORM=2,
    CHEB_NORM=3,
    EUCL_NORM_LOC=4,
    MANH_NORM_LOC=5,
    CHEB_NORM_LOC=6,
    TIGHT_SIDE=7,
    LOOSE_SIDE=8,
    SURFACE_AREA=9,
    EMPTY_VOLUME=10
};


class GeneticHeuristicSolver:public BoxPackingSolver
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
    static QString placementNames[];
    void setupLog();
public:
    GeneticHeuristicSolver();
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
void selection(QList<GeneticHeuristicSolver::Chromosome>& solutions, QList<float> &objectiveValues, float boundsVolume, int selectionCount);

#endif // GENETICHEURISTICSOLVER_H
