#include "geneticaltsolver.h"
#include <QStack>
#include "QThread"
#include <time.h>
#include "random.h"

GeneticAltSolver::GeneticAltSolver():population(10),maxIterations(50),
    maxTime(300.0f),requiredVolume(50.0f),
    mutationProb(0.25f),
    selectionCount(5),
    crossingoverCount(5),
    repairAttempts(4),
    rotateBoxes(false),
    strategy(0)
{ 
}
QString printSolution(const GeneticAltSolver::Chromosome& c)
{
    QString str;
    for(int i=0;i<c.genes.length();i++)
    {
        str+=QString::number(c.genes[i]);
        if(i!=c.genes.length()-1)
            str+=+", ";
    }
    return str;
}

void GeneticAltSolver::init(int population, int maxIterations, float maxTime, float requiredVolume,
                            float mutationProb, int selectionCount, int crossingoverCount, int repairAttempts,
                            bool rotateBoxes,int strategy)
{
    this->population=population;
    this->maxIterations=maxIterations;
    this->maxTime=maxTime;
    this->requiredVolume=requiredVolume;
    this->mutationProb=mutationProb;
    this->selectionCount=selectionCount;
    this->crossingoverCount=crossingoverCount;
    this->repairAttempts=repairAttempts;
    this->rotateBoxes=rotateBoxes;
    this->crossingoverCount=qMin(this->crossingoverCount,this->population-1);
    this->strategy=strategy;
}

QVector<BoxInfo> GeneticAltSolver::solve(const QVector<Box> &boxes, const Box &bounds)
{
    log.clear();
    clock_t t;
    t = clock();

    float boundsVolume=bounds.volume();

    QList<Chromosome> solutions;
    QList<float> objectiveValues;

    float bestVolume=10e8;
    Chromosome bestSolution;
    float totalTime=0.0f;
    int currentIteration=0;

    for(int i=0;i<population;i++)
    {
        Chromosome c(boxes.length(),rotateBoxes);
        solutions.push_back(c);
        float value=c.objectiveFunction(boxes,bounds,strategy);
        objectiveValues.push_back(value);
        if(value>=0.0&&value<bestVolume)
        {
            bestVolume=value;
            bestSolution=c;
        }
    }
    int reductionIndex=0;
    while(true)
    {
        emit BoxPackingSolver::progress(qMax<int>(currentIteration*100/this->maxIterations,int(totalTime*100.f/this->maxTime)));
        do{
            for(int i=solutions.length();i<population;i++)
            {
                Chromosome c(boxes.length(),rotateBoxes);
                solutions.push_back(c);
                float value=c.objectiveFunction(boxes,bounds,strategy);
                objectiveValues.push_back(value);
                if(value>=0.0&&value<bestVolume)
                {
                    bestVolume=value;
                    bestSolution=c;
                }
            }
            for(int k=solutions.length()-1;k>=reductionIndex;k--)
            {
                if(objectiveValues[k]<0.0)
                {
                    Chromosome& c = solutions[k];
                    float objectiveValue=objectiveValues[k];
                    //Попробовать исправить
                    c.reduction(rotateBoxes);
                    objectiveValue=c.objectiveFunction(boxes,bounds,rotateBoxes);
                    if(objectiveValue>=0.0)
                    {
                        objectiveValues[k]=objectiveValue;
                        if(objectiveValue<bestVolume)
                        {
                            bestVolume=objectiveValue;
                            bestSolution=solutions[k];
                        }
                        continue;
                    }
                    solutions.removeAt(k);
                    objectiveValues.removeAt(k);
                }
            }
            float currentTime=float(clock()-t)/CLOCKS_PER_SEC;
            totalTime=currentTime;
            emit BoxPackingSolver::progress(qMax<int>(currentIteration*100.0f/this->maxIterations,int(totalTime*100.f/this->maxTime)));

            if(currentTime>maxTime)
            {
                goto END;
            }
        }while(solutions.length()<population);
        //кроссинговер
        for(int i=0;i<crossingoverCount;i++)
        {
            int index1=Random::random(population-1);
            int index2=(Random::random(population-2)+index1)%population;
            Chromosome& p1=solutions[index1];
            Chromosome& p2=solutions[index2];
            GeneticAltSolver::Chromosome child=crossingover(p1,p2);
            solutions.push_back(child);
            objectiveValues.push_back(child.objectiveFunction(boxes,bounds,strategy));

        }
        for(int i=0;i<population;i++)
        {
            if(Random::randomUnit()<=mutationProb)
            {
                Chromosome& c=solutions[i];
                //mutation
                c.mutation(rotateBoxes);
                objectiveValues[i]=c.objectiveFunction(boxes,bounds,strategy);
            }
        }
        currentIteration++;

        float currentTime=float(clock()-t)/CLOCKS_PER_SEC;
        if(currentTime>maxTime||currentIteration>=maxIterations||bestVolume<=requiredVolume*boundsVolume)
        {
            totalTime=currentTime;
            break;
        }

        //отбор
        for(int i=solutions.length()-1;i>=0;i--)
        {
            //Попробовать исправить
            //reduction
            if(objectiveValues[i]<0.0)
            {
                Chromosome& c = solutions[i];
                //Попробовать исправить
                float objectiveValue=objectiveValues[i];
                for(int j=0;j<repairAttempts;j++)
                {
                    c.reduction(rotateBoxes);

                    objectiveValue=c.objectiveFunction(boxes,bounds,strategy);
                    if(objectiveValue>=0.0)
                    {
                        if(objectiveValue<bestVolume)
                        {
                            bestSolution=solutions[i];
                            bestVolume=objectiveValue;
                        }
                        objectiveValues[i]=objectiveValue;
                        break;
                    }
                }
                if(objectiveValue<0.0)
                {
                    solutions.removeAt(i);
                    objectiveValues.removeAt(i);
                }
            }
        }
        reductionIndex=solutions.length();
        if(solutions.length()==0)
        {
            //Необходимо полное обновление популяции
            continue;
        }
        //Селекция
        selection(solutions,objectiveValues,boundsVolume,selectionCount);

        reductionIndex=solutions.length();
        QThread::msleep(5);
    }
    END:
    if(bestVolume<10e8)
    {
        float x,y,z;
        QVector<BoxInfo> placements=bestSolution.placeBoxes(boxes,bounds,strategy,x,y,z);

        log+="Объём: "+QString::number(x*y*z)+".\n";
        log+="Габариты: "+QString::number(x)+"x"+QString::number(y)+"x"+QString::number(z)+".\n";
        log+="Решение: ";
        for(int i=0;i<bestSolution.genes.length();i++)
        {
            log+=QString::number(bestSolution.genes[i]);
            if(i!=bestSolution.genes.length()-1)
                log+=+", ";
        }
        log+="\nВектор ориентаций: ";
        for(int i=0;i<bestSolution.orientations.length();i++)
        {
            log+=QString::number(orientationToInteger(bestSolution.orientations[i]));
            if(i!=bestSolution.orientations.length()-1)
                log+=+", ";
        }
        log+="\nКоличество итераций: "+QString::number(currentIteration);
        log+="\nЗатраченное время: "+QString::number(qMax(totalTime,0.1f))+" секунд";
        return placements;
    }
    throw QString("Не удалось найти подходящее решение");
}

GeneticAltSolver::Chromosome GeneticAltSolver::Chromosome::operator=(const GeneticAltSolver::Chromosome &b)
{
    this->genes=b.genes;
    this->orientations=b.orientations;
    return *this;
}

float GeneticAltSolver::Chromosome::getBoxCount() const
{
    return genes.length();
}

GeneticAltSolver::Chromosome::Chromosome():genes(),orientations()
{
}

GeneticAltSolver::Chromosome::Chromosome(const Chromosome &b)
{
    this->genes=b.genes;
    this->orientations=b.orientations;
}

GeneticAltSolver::Chromosome::Chromosome(int boxCount, bool rotateBoxes)
{
    QVector<int> availableIndexes;
    for(int i=0;i<boxCount;i++)
    {
        availableIndexes.push_back(i);
    }
    for(int i=0;i<boxCount;i++)
    {
        int randIndex=Random::random(boxCount-1-i);
        int index=availableIndexes[randIndex];
        qSwap(availableIndexes[randIndex],availableIndexes[boxCount-1-i]);
        genes.push_back(index);
        orientations.push_back(rotateBoxes?::orientations[Random::random(5)]:BoxOrientation::XYZ0);
    }
}

QVector<BoxInfo> GeneticAltSolver::Chromosome::placeBoxes(const QVector<Box> &boxes, const Box &bounds, int strategy,
                                                          float &width,float&height,float&length) const
{
    QList<emsstruct> emsList;
    emsList.push_back(emsstruct{0.f,0.f,0.f,
                                bounds.w,bounds.h,bounds.l});
    QVector<BoxInfo> placements;
    float maxx=0.0f;
    float maxy=0.0f;
    float maxz=0.0f;
    for(int i=0;i<genes.length();i++)
    {
        int gene=genes[i];
        const Box& box=boxes.at(gene).getOrientation(orientations[gene]);
        float bestFit=10e8;
        float bestDistance=10e8;
        int bestEms=-1;
        for(int j=0;j<emsList.length();j++)
        {
            emsstruct ems=emsList[j];
            float w=ems.maxx-ems.minx;
            float h=ems.maxy-ems.miny;
            float l=ems.maxz-ems.minz;

            float x=ems.minx+box.width();
            float y=ems.miny+box.height();
            float z=ems.minz+box.length();
            float envelopeX=qMax(x,maxx);
            float envelopeY=qMax(y,maxy);
            float envelopeZ=qMax(z,maxz);

            float fitness;
            float distance=x*x+y*y+z*z;
            if(box.width()>w||box.height()>h||box.length()>l)
                continue;
            switch(strategy)
            {
            case VOLUME:
                fitness=envelopeX*envelopeY*envelopeZ;
                break;
            case EUCLIDIAN_NORM:
                fitness=envelopeX*envelopeX+
                        envelopeY*envelopeY+
                        envelopeZ*envelopeZ;
                break;
            case MANHATTAN_NORM:
                fitness=envelopeX+
                        envelopeY+
                        envelopeZ;
                break;
            case CHEBYSHEV_NORM:
                fitness=qMax(envelopeX,
                             qMax(envelopeY,
                                  envelopeZ));
                break;
            case CLOSEST_POINT_EUCL:
                fitness=ems.minx*ems.minx+
                        ems.miny*ems.miny+
                        ems.minz*ems.minz;
                break;
            case CLOSEST_POINT_MANH:
                fitness=ems.minx+
                        ems.miny+
                        ems.minz;
                break;
            case CLOSEST_POINT_CHEB:
                fitness=qMax(ems.minx,
                             qMax(ems.miny,
                                  ems.minz));
                break;
            }
            if(fitness<bestFit)
            {
                bestFit=fitness;
                bestDistance=distance;
                bestEms=j;
            }else if(qAbs(fitness-bestFit)<0.001&&distance<bestDistance)
            {
                bestFit=fitness;
                bestDistance=distance;
                bestEms=j;
            }
        }
        if(bestEms==-1)
        Q_ASSERT(bestEms!=-1);
        emsstruct ems=emsList[bestEms];
        BoxInfo boxInfo=BoxInfo{ems.minx+box.width()*0.5,
                ems.miny+box.height()*0.5,
                ems.minz+box.length()*0.5,
                gene,
                orientations[gene]};
        placements.push_back(boxInfo);

        float minx=ems.minx;
        float x=minx+box.width();
        float miny=ems.miny;
        float y=miny+box.height();
        float minz=ems.minz;
        float z=minz+box.length();
        maxx=qMax(x,maxx);
        maxy=qMax(y,maxy);
        maxz=qMax(z,maxz);


        //recompute all ems's
        QStack<emsstruct> emptyspaces;
        for(int p=0;p<emsList.length();p++)
        {
            emptyspaces.push(emsList[p]);
        }
        emsList.clear();
        while(emptyspaces.length()>0)
        {
            emsstruct t_ems=emptyspaces.pop();
            float t_minx=qMax(minx,t_ems.minx);
            float t_miny=qMax(miny,t_ems.miny);
            float t_minz=qMax(minz,t_ems.minz);

            float t_maxx=qMin(x,t_ems.maxx);
            float t_maxy=qMin(y,t_ems.maxy);
            float t_maxz=qMin(z,t_ems.maxz);

            if(t_minx<=t_maxx&&t_miny<=t_maxy&&t_minz<=t_maxz)
            {
                //part of the box is inside ems
                if(t_minx-t_ems.minx>0.01)
                {
                    emsList.push_back(emsstruct{t_ems.minx,t_ems.miny,t_ems.minz,
                                                 t_minx,t_ems.maxy,t_ems.maxz});
                }
                if(t_ems.maxx-t_maxx>0.01)
                {
                    emsList.push_back(emsstruct{t_maxx,t_ems.miny,t_ems.minz,
                                                 t_ems.maxx,t_ems.maxy,t_ems.maxz});
                }
                if(t_miny-t_ems.miny>0.01)
                {
                    emsList.push_back(emsstruct{t_ems.minx,t_ems.miny,t_ems.minz,
                                                 t_ems.maxx,t_miny,t_ems.maxz});
                }
                if(t_ems.maxy-t_maxy>0.01)
                {
                    emsList.push_back(emsstruct{t_ems.minx,t_maxy,t_ems.minz,
                                                 t_ems.maxx,t_ems.maxy,t_ems.maxz});
                }
                if(t_minz-t_ems.minz>0.01)
                {
                    emsList.push_back(emsstruct{t_ems.minx,t_ems.miny,t_ems.minz,
                                                 t_ems.maxx,t_ems.maxy,t_minz});
                }
                if(t_ems.maxz-t_maxz>0.01)
                {
                    emsList.push_back(emsstruct{t_ems.minx,t_ems.miny,t_maxz,
                                                 t_ems.maxx,t_ems.maxy,t_ems.maxz});
                }
            }else
            {
                emsList.push_back(t_ems);
            }
        }
        //check for inscribed spaces
        for(int l=emsList.length()-1;l>=0;l--)
        {
            emsstruct c1=emsList[l];
            for(int k=0;k<l;k++)
            {
                emsstruct c2=emsList[k];
                if(c1.minx>=c2.minx&&c1.miny>=c2.miny&&c1.minz>=c2.minz&&
                        c1.maxx<=c2.maxx&&c1.maxy<=c2.maxy&&c1.maxz<=c2.maxz)
                {
                    emsList.removeAt(l);
                    break;
                }
                if(c2.minx>=c1.minx&&c2.miny>=c1.miny&&c2.minz>=c1.minz&&
                        c2.maxx<=c1.maxx&&c2.maxy<=c1.maxy&&c2.maxz<=c1.maxz)
                {
                    emsList.removeAt(k);
                    l--;
                    continue;
                }
            }
        }
    }
    width=maxx;
    height=maxy;
    length=maxz;
    return placements;
}

GeneticAltSolver::Chromosome crossingover(const GeneticAltSolver::Chromosome &solution1, const GeneticAltSolver::Chromosome &solution2)
{
    int boxCount=solution1.getBoxCount();
    if(boxCount<2)
        throw QString("Invalid box count");
    int cuttingPoint=Random::random(boxCount-2)+1;
    GeneticAltSolver::Chromosome child;
    child.orientations.fill(BoxOrientation::XYZ0,boxCount);
    QVector<bool> genePool;
    genePool.fill(true,boxCount);
    for(int i=0;i<cuttingPoint;i++)
    {
        int gene=solution1.genes[i];
        child.genes.push_back(gene);
        child.orientations[gene]=solution1.orientations[gene];
        genePool[gene]=false;
    }
    for(int i=cuttingPoint;i<boxCount;i++)
    {
        int gene=solution2.genes[i];
        if(genePool[gene]==false)
        {
            child.genes.push_back(-1);
        }else
        {
            genePool[gene]=false;
            child.genes.push_back(gene);
            child.orientations[gene]=solution2.orientations[gene];
        }
    }
    QVector<int> freeGenes;
    for(int i=0;i<boxCount;i++)
    {
        int gene=solution2.genes[i];
        if(genePool[gene]==true)
            freeGenes.push_back(gene);
    }
    for(int i=cuttingPoint;i<boxCount;i++)
    {
        int gene=child.genes[i];
        if(gene==-1)
        {
            gene=freeGenes.first();
            child.genes[i]=gene;
            freeGenes.pop_front();
            child.orientations[gene]=solution2.orientations[gene];
        }
    }
    return child;
}

void GeneticAltSolver::Chromosome::mutation(bool rotateBoxes)
{
    //switch two random positions;
    int index1=Random::random(genes.length()-1);
    int index2=(Random::random(genes.length()-2)+1+index1)%genes.length();
    qSwap(genes[index1],genes[index2]);
    if(rotateBoxes)
    {
        int index=Random::random(genes.length()-1);
        orientations[index]=::orientations[orientationToInteger(orientations[index])+Random::random(4)+1];
    }
}

void GeneticAltSolver::Chromosome::reduction(bool rotateBoxes)
{
    int index1=Random::random(genes.length()-1);
    int index2=(Random::random(genes.length()-2)+1+index1)%genes.length();
    qSwap(genes[index1],genes[index2]);
    if(rotateBoxes)
    {
        int index=Random::random(genes.length()-1);
        orientations[index]=::orientations[orientationToInteger(orientations[index])+Random::random(4)+1];
    }
}

void selection(QList<GeneticAltSolver::Chromosome> &solutions, QList<float>& objectiveValues, float boundsVolume, int selectionCount)
{
    QVector<float> probabilities;
    float normalizationFactor=0.0f;//значение для нормализации вероятностей
    for(int i=0;i<objectiveValues.length();i++)
    {
        float value=boundsVolume-objectiveValues[i];//Вероятность пропорциональна разнице максимального объёма и значения целевой функции
        normalizationFactor+=value;
        probabilities.push_back(normalizationFactor);
    }
    for(int i=0;i<objectiveValues.length();i++)
    {
        probabilities[i]/=normalizationFactor;
    }
    QList<GeneticAltSolver::Chromosome> selectionArray;
    QList<float> selectionObjectiveValues;
    for(int i=0;i<selectionCount;i++)
    {
        float randomValue=Random::randomUnit();
        for(int j=0;j<probabilities.length();j++)
        {
            if(probabilities[j]>=randomValue)
            {
                selectionArray.push_back(solutions[j]);
                selectionObjectiveValues.push_back(objectiveValues[j]);
                break;
            }
        }
    }
    //remove other chromosomes
    solutions=selectionArray;
    objectiveValues=selectionObjectiveValues;
}

float GeneticAltSolver::Chromosome::objectiveFunction(const QVector<Box> &boxes, const Box &bounds, int strategy) const
{
    QList<emsstruct> emsList;
    emsList.push_back(emsstruct{0.f,0.f,0.f,bounds.w,bounds.h,bounds.l});
    float w=0.0f;
    float h=0.0f;
    float l=0.0f;
    for(int i=0;i<genes.length();i++)
    {
        int gene=genes[i];
        const Box& box=boxes.at(gene).getOrientation(orientations[gene]);
        float bestFit=10e8;
        int bestEms=-1;
        for(int j=0;j<emsList.length();j++)
        {
            emsstruct ems=emsList[j];
            float w=ems.maxx-ems.minx;
            float h=ems.maxy-ems.miny;
            float l=ems.maxz-ems.minz;

            float x=ems.minx+box.width();
            float y=ems.miny+box.height();
            float z=ems.minz+box.length();
            float envelopeX=qMax(x,maxx);
            float envelopeY=qMax(y,maxy);
            float envelopeZ=qMax(z,maxz);

            float fitness=0.0;
            if(box.width()>w||box.height()>h||box.length()>l)
                continue;
            switch(strategy)
            {
            case VOLUME:
                fitness=envelopeX*envelopeY*envelopeZ;
                break;
            case EUCLIDIAN_NORM:
                fitness=envelopeX*envelopeX+
                        envelopeY*envelopeY+
                        envelopeZ*envelopeZ;
                break;
            case MANHATTAN_NORM:
                fitness=envelopeX+
                        envelopeY+
                        envelopeZ;
                break;
            case CHEBYSHEV_NORM:
                fitness=qMax(envelopeX,
                             qMax(envelopeY,
                                  envelopeZ));
                break;
            case CLOSEST_POINT_EUCL:
                fitness=ems.minx*ems.minx+
                        ems.miny*ems.miny+
                        ems.minz*ems.minz;
                break;
            case CLOSEST_POINT_MANH:
                fitness=ems.minx+
                        ems.miny+
                        ems.minz;
                break;
            case CLOSEST_POINT_CHEB:
                fitness=qMax(ems.minx,
                             qMax(ems.miny,
                                  ems.minz));
                break;
            }
            if(fitness<bestFit)
            {
                bestFit=fitness;
                bestEms=j;
            }
        }
        if(bestEms==-1)
            return -1.0;
        emsstruct ems=emsList[bestEms];

        float minx=ems.minx;
        float maxx=minx+box.width();
        float miny=ems.miny;
        float maxy=miny+box.height();
        float minz=ems.minz;
        float maxz=minz+box.length();
        w=qMax(maxx,w);
        h=qMax(maxy,h);
        l=qMax(maxz,l);


        //recompute all ems's
        QStack<emsstruct> emptyspaces;
        for(int i=0;i<emsList.length();i++)
        {
            emptyspaces.push(emsList[i]);
        }
        emsList.clear();
        while(emptyspaces.length()>0)
        {
            emsstruct t_ems=emptyspaces.pop();
            float t_minx=qMax(minx,t_ems.minx);
            float t_miny=qMax(miny,t_ems.miny);
            float t_minz=qMax(minz,t_ems.minz);

            float t_maxx=qMin(x,t_ems.maxx);
            float t_maxy=qMin(y,t_ems.maxy);
            float t_maxz=qMin(z,t_ems.maxz);

            if(t_minx<=t_maxx&&t_miny<=t_maxy&&t_minz<=t_maxz)
            {
                //part of the box is inside ems
                if(t_minx-t_ems.minx>0.01)
                {
                    emsList.push_back(emsstruct{t_ems.minx,t_ems.miny,t_ems.minz,
                                                 t_minx,t_ems.maxy,t_ems.maxz});
                }
                if(t_ems.maxx-t_maxx>0.01)
                {
                    emsList.push_back(emsstruct{t_maxx,t_ems.miny,t_ems.minz,
                                                 t_ems.maxx,t_ems.maxy,t_ems.maxz});
                }
                if(t_miny-t_ems.miny>0.01)
                {
                    emsList.push_back(emsstruct{t_ems.minx,t_ems.miny,t_ems.minz,
                                                 t_ems.maxx,t_miny,t_ems.maxz});
                }
                if(t_ems.maxy-t_maxy>0.01)
                {
                    emsList.push_back(emsstruct{t_ems.minx,t_maxy,t_ems.minz,
                                                 t_ems.maxx,t_ems.maxy,t_ems.maxz});
                }
                if(t_minz-t_ems.minz>0.01)
                {
                    emsList.push_back(emsstruct{t_ems.minx,t_ems.miny,t_ems.minz,
                                                 t_ems.maxx,t_ems.maxy,t_minz});
                }
                if(t_ems.maxz-t_maxz>0.01)
                {
                    emsList.push_back(emsstruct{t_ems.minx,t_ems.miny,t_maxz,
                                                 t_ems.maxx,t_ems.maxy,t_ems.maxz});
                }
            }else
            {
                emsList.push_back(t_ems);
            }
        }
        //check for inscribed spaces
        for(int l=emsList.length()-1;l>=0;l--)
        {
            emsstruct c1=emsList[l];
            for(int k=0;k<l;k++)
            {
                emsstruct c2=emsList[k];
                if(c1.minx>=c2.minx&&c1.miny>=c2.miny&&c1.minz>=c2.minz&&
                        c1.maxx<=c2.maxx&&c1.maxy<=c2.maxy&&c1.maxz<=c2.maxz)
                {
                    emsList.removeAt(l);
                    break;
                }
                if(c2.minx>=c1.minx&&c2.miny>=c1.miny&&c2.minz>=c1.minz&&
                        c2.maxx<=c1.maxx&&c2.maxy<=c1.maxy&&c2.maxz<=c1.maxz)
                {
                    emsList.removeAt(k);
                    l--;
                    continue;
                }
            }
        }
    }
    return maxz*maxy*maxz;
}
