#ifndef GENETICSOLVER_H
#define GENETICSOLVER_H

#include "boxpackingsolver.h"
struct Chromosome
{
    QVector<int> genes;
    QVector<BoxOrientation> orientations;
    Chromosome():genes(),orientations()
    {

    }
    Chromosome(const Chromosome &b)
    {
        this->genes=b.genes;
        this->orientations=b.orientations;
    }
    Chromosome Chromosome::operator=(const Chromosome &b)
    {
        this->genes=b.genes;
        this->orientations=b.orientations;
        return *this;
    }
};

class GeneticSolver:public BoxPackingSolver
{
private:
    int population;
    int maxIterations;
    float maxTime;
    float requiredVolume;
    float mutationProb;
    int selectionCount;
    int crossingoverCount;
    int repairAttempts;
    bool rotateBoxes;
    bool compressBoxes;
    void setupLog();
public:
    GeneticSolver();
    void init(int population,
              int maxIterations,
              float maxTime,
              float requiredVolume,
              float mutationProb,
              int selectionCount,
              int crossingoverCount,
              int repairAttempts,
              bool rotateBoxes,
              bool compressBoxes);
    virtual QVector<BoxInfo> solve(const QVector<Box>& boxes,const Box& bounds) override;
};

void checkSolutions(const QList<Chromosome>& solutions,int boxCount);

Chromosome generate(int boxCount,bool rotateBoxes);
void reduction(Chromosome& c,const float wProb,const float hProb,const bool rotateBoxes);
void selection(QList<Chromosome>& solutions, QList<float>& objectiveValues, float boundsVolume, int selectionCount);
void mutation(Chromosome& c);
void inversion(Chromosome& c);
void crossingover(QList<Chromosome>& solutions,QList<float>& objectiveValues,const QList<int>& crossingoverIndexes,
                  const QVector<bool>& genePool,
                  const QVector<Box>& boxes,
                  const Box& bounds,
                  const int population,
                  const int chromosomeLength,
                  const int boxCount);
bool xCoordLessThan(const BoxInfo &d1, const BoxInfo &d2);
bool yCoordLessThan(const BoxInfo &d1, const BoxInfo &d2);
bool zCoordLessThan(const BoxInfo &d1, const BoxInfo &d2);

float objectiveFunction(const Chromosome& chromosome,const QVector<Box>& boxes,const Box& bound);
QVector<BoxInfo> calculatePlacements(const Chromosome& chromosome,const QVector<Box>& boxes);

void recompute(QList<emsstruct>* emsList,float minx,float maxx,float miny,float maxy,float minz,float maxz);
void reduceEMS(QList<emsstruct>* emsList);
#endif // GENETICSOLVER_H
