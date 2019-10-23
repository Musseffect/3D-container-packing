#ifndef PACKINGSOLVERS_H
#define PACKINGSOLVERS_H
#include <QVector>
#include <QElapsedTimer>
#include "random.h"
#include <QStack>
#include "structs.h"

#ifdef SOLVERSOLD

struct emsstruct{
    float minx;
    float miny;
    float minz;
    float maxx;
    float maxy;
    float maxz;
};

struct Chromosome
{
    QVector<int> genes;
    QVector<BoxOrientation> orientations;
    Chromosome();
    Chromosome(const Chromosome& b);
    Chromosome operator=(const Chromosome& b);
};

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

QVector<BoxInfo> solveGreedy(QVector<Box>& boxes, Box &bounds,int criteria,bool rotateBoxes, QString &log);

Chromosome generate(int boxCount);
float objectiveFunction(const Chromosome& chromosome, const QVector<Box> &boxes, const Box& bound);
QVector<BoxInfo> calculatePlacements(const Chromosome& chromosome, const QVector<Box> &boxes);
void crossingover(QList<Chromosome>& solutions, QList<float>& objectiveValues, const QList<int>& crossingoverIndexes, const QVector<bool> &genePool, const int population, const int chromosomeLength);
void inversion(Chromosome& c);
void mutation(Chromosome& c);
QVector<BoxInfo> solveGenetic(const QVector<Box>& boxes,Box bounds,int population,
                              int maxIterations,float maxTime,float requiredVolume,
                              float mutationProb,
                              int selectionCount,
                              int crossingoverCount,
                              int repairAttempts,
                              bool rotateBoxes,
                              bool compressBoxes,
                              QString& log);

void checkSolutions(const QList<Chromosome>& solutions,int boxCount);


void recompute(QList<emsstruct>* emsList,float minx,float maxx,float miny,float maxy,float minz,float maxz);
void reduceEMS(QList<emsstruct>* emsList);
#endif
#endif // 3DPACKINGSOLVERS_H

