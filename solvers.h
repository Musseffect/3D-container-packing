#ifndef PACKINGSOLVERS_H
#define PACKINGSOLVERS_H
#include <QVector>
#include <QElapsedTimer>
#include "random.h"
#include <QStack>
#include "structs.h"

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
    Chromosome();
    Chromosome(const Chromosome& b);
    Chromosome operator=(const Chromosome& b);
};

QVector<BoxInfo> solveGreedy(const QList<Box>& boxes, Box &bounds, QString &log);

Chromosome generate(int boxCount);
float objectiveFunction(const Chromosome& chromosome, const QList<Box> &boxes, const Box& bound);
QVector<BoxInfo> calculatePlacements(const Chromosome& chromosome,const QList<Box>& boxes);
void inversion(Chromosome& c);
void mutation(Chromosome& c);
QVector<BoxInfo> solveGenetic(const QList<Box>& boxes,Box bounds,int population,
                              int maxIterations,float maxTime,float requiredVolume,
                              float mutationProb,
                              int selectionCount,
                              int crossingoverCount, QString& log);

#endif // 3DPACKINGSOLVERS_H

