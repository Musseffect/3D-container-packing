#include "geneticHeuristicSolver.h"
#include <QStack>
#include "QThread"
#include <time.h>
#include "random.h"

QString GeneticHeuristicSolver::placementNames[] = {"Объём AABB",
                                                    "L2 норма AABB",
                                                    "L1 норма AABB",
                                                    "L∞ норма AABB",
                                                    "L2 норма текущего края",
                                                    "L1 норма текущего края",
                                                    "L∞ норма текущего края",
                                                    "Наиболее плотная сторона",
                                                    "Наименее плотная сторона",
                                                    "Площадь поверхности AABB",
                                                    "Незаполненный объём"};

GeneticHeuristicSolver::GeneticHeuristicSolver():population(10),maxIterations(50),
    maxTime(300.0f),requiredVolume(50.0f),
    mutationProb(0.25f),
    selectionCount(5),
    crossingoverCount(5),
    repairAttempts(4),
    rotateBoxes(false),
    strategy(0)
{ 
}
QString printSolution(const GeneticHeuristicSolver::Chromosome& c)
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

void GeneticHeuristicSolver::init(int population, int maxIterations, float maxTime, float requiredVolume,
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

QVector<BoxInfo> GeneticHeuristicSolver::solve(const QVector<Box> &boxes, const Box &bounds)
{
    log.clear();
    clock_t t;
    t = clock();

    float boundsVolume=bounds.volume();

    float totalVolumeSum = 0.0;
    for(int i=0;i<boxes.length();i++)
        totalVolumeSum+=boxes[i].volume();

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
            GeneticHeuristicSolver::Chromosome child=crossingover(p1,p2);
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

        log+="Объём: "+QString::number(x*y*z)+"\n";
        log+="Плотность упаковки: " + QString::number(100.0*totalVolumeSum/(x*y*z)) + "%\n";
        log+="Габариты: "+QString::number(x)+"x"+QString::number(y)+"x"+QString::number(z)+"\n";
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
        log+="\nЗатраченное время: "+QString::number(qMax(totalTime,0.1f))+" секунд\n";

        log+="Решение получено генетическим алгоритмом с использованием эвристики по выбору места вставки блока\n";
        log+="Размер популяции: "+QString::number(population)+"\n";
        log+="Вероятность мутации: " + QString::number(mutationProb) + "\n";
        log+="Отбираемых особей: " + QString::number(selectionCount) + "\n";
        log+="Пар для кроссинговера: " + QString::number(crossingoverCount) + "\n";
        log+="Попыток исправления хромосом: " + QString::number(repairAttempts) + "\n";

        log+="Критерий расположения блоков: " + placementNames[strategy] + "\n";
        log+="Повороты: "+(this->rotateBoxes?"разрешены":"запрещены");
        return placements;
    }
    throw QString("Не удалось найти подходящее решение");
}

GeneticHeuristicSolver::Chromosome GeneticHeuristicSolver::Chromosome::operator=(const GeneticHeuristicSolver::Chromosome &b)
{
    this->genes=b.genes;
    this->orientations=b.orientations;
    return *this;
}

float GeneticHeuristicSolver::Chromosome::getBoxCount() const
{
    return genes.length();
}

GeneticHeuristicSolver::Chromosome::Chromosome():genes(),orientations()
{
}

GeneticHeuristicSolver::Chromosome::Chromosome(const Chromosome &b)
{
    this->genes=b.genes;
    this->orientations=b.orientations;
}

GeneticHeuristicSolver::Chromosome::Chromosome(int boxCount, bool rotateBoxes)
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

QVector<BoxInfo> GeneticHeuristicSolver::Chromosome::placeBoxes(const QVector<Box> &boxes, const Box &bounds, int strategy,
                                                          float &width,float&height,float&length) const
{
    QList<AABB> aabbList;
    aabbList.push_back(AABB{0.f,0.f,0.f,
                                bounds.w,bounds.h,bounds.l});
    QVector<BoxInfo> placements;
    width=0.0f;
    height=0.0f;
    length=0.0f;
    float totalVolumeSum = 0.0;
    for(int i=0;i<genes.length();i++)
    {
        int gene=genes[i];
        const Box& box=boxes.at(gene).getOrientation(orientations[gene]);
        float bestFit=10e8;
        float bestDistance=10e8;
        int bestAABB=-1;
        for(int j=0;j<aabbList.length();j++)
        {
            AABB aabb=aabbList[j];
            float aabbw=aabb.maxx-aabb.minx;
            float aabbh=aabb.maxy-aabb.miny;
            float aabbl=aabb.maxz-aabb.minz;

            float x=aabb.minx+box.width();
            float y=aabb.miny+box.height();
            float z=aabb.minz+box.length();
            float envelopeX=qMax(x,width);
            float envelopeY=qMax(y,height);
            float envelopeZ=qMax(z,length);

            float fitness;
            float distance=x*x+y*y+z*z;
            if(box.width()>aabbw||box.height()>aabbh||box.length()>aabbl)
                continue;
            if(bounds.w<envelopeX||bounds.h<envelopeY||bounds.l<envelopeZ)
                continue;
            switch(strategy)
            {
            case VOLUME:
                fitness=envelopeX*envelopeY*envelopeZ;
                break;
            case EUCL_NORM:
                fitness=envelopeX*envelopeX+
                        envelopeY*envelopeY+
                        envelopeZ*envelopeZ;
                break;
            case MANH_NORM:
                fitness=envelopeX+
                        envelopeY+
                        envelopeZ;
                break;
            case CHEB_NORM:
                fitness=qMax(envelopeX,
                             qMax(envelopeY,
                                  envelopeZ));
                break;
            case EUCL_NORM_LOC:
                fitness=aabb.minx*aabb.minx+
                        aabb.miny*aabb.miny+
                        aabb.minz*aabb.minz;
                break;
            case MANH_NORM_LOC:
                fitness=aabb.minx+
                        aabb.miny+
                        aabb.minz;
                break;
            case CHEB_NORM_LOC:
                fitness=qMax(aabb.minx,
                             qMax(aabb.miny,
                                  aabb.minz));
                break;
            case TIGHT_SIDE:
                fitness=qMin(aabbw-box.width(),
                        qMin(aabbh-box.height(),
                        aabbl-box.length()));
                break;
            case LOOSE_SIDE:
                fitness=qMax(aabbw-box.width(),
                        qMax(aabbh-box.height(),
                        aabbl-box.length()));
                break;
            case SURFACE_AREA:
                fitness = envelopeX*envelopeY + envelopeY*envelopeZ + envelopeZ*envelopeX;
                break;
            case EMPTY_VOLUME:
                fitness = envelopeX*envelopeY*envelopeZ - totalVolumeSum - box.volume();
                break;
            }
            if(fitness<bestFit)
            {
                bestFit=fitness;
                bestDistance=distance;
                bestAABB=j;
            }else if(qAbs(fitness-bestFit)<0.001&&distance<bestDistance)
            {
                bestFit=fitness;
                bestDistance=distance;
                bestAABB=j;
            }
        }
        if(bestAABB==-1)
            Q_ASSERT(bestAABB!=-1);
        totalVolumeSum+=box.volume();
        AABB aabb=aabbList[bestAABB];
        BoxInfo boxInfo=BoxInfo{aabb.minx+box.width()*0.5,
                aabb.miny+box.height()*0.5,
                aabb.minz+box.length()*0.5,
                gene,
                orientations[gene]};
        placements.push_back(boxInfo);
        float minx=aabb.minx;
        float x=minx+box.width();
        float miny=aabb.miny;
        float y=miny+box.height();
        float minz=aabb.minz;
        float z=minz+box.length();
        width=qMax(x,width);
        height=qMax(y,height);
        length=qMax(z,length);


        //recompute all aabb's
        QStack<AABB> emptyspaces;
        for(int p=0;p<aabbList.length();p++)
        {
            emptyspaces.push(aabbList[p]);
        }
        aabbList.clear();
        while(emptyspaces.length()>0)
        {
            AABB t_aabb=emptyspaces.pop();
            float t_minx=qMax(minx,t_aabb.minx);
            float t_miny=qMax(miny,t_aabb.miny);
            float t_minz=qMax(minz,t_aabb.minz);

            float t_maxx=qMin(x,t_aabb.maxx);
            float t_maxy=qMin(y,t_aabb.maxy);
            float t_maxz=qMin(z,t_aabb.maxz);

            if(t_minx<=t_maxx&&t_miny<=t_maxy&&t_minz<=t_maxz)
            {
                //part of the box is inside aabb
                if(t_minx-t_aabb.minx>0.01)
                {
                    aabbList.push_back(AABB{t_aabb.minx,t_aabb.miny,t_aabb.minz,
                                                 t_minx,t_aabb.maxy,t_aabb.maxz});
                }
                if(t_aabb.maxx-t_maxx>0.01)
                {
                    aabbList.push_back(AABB{t_maxx,t_aabb.miny,t_aabb.minz,
                                                 t_aabb.maxx,t_aabb.maxy,t_aabb.maxz});
                }
                if(t_miny-t_aabb.miny>0.01)
                {
                    aabbList.push_back(AABB{t_aabb.minx,t_aabb.miny,t_aabb.minz,
                                                 t_aabb.maxx,t_miny,t_aabb.maxz});
                }
                if(t_aabb.maxy-t_maxy>0.01)
                {
                    aabbList.push_back(AABB{t_aabb.minx,t_maxy,t_aabb.minz,
                                                 t_aabb.maxx,t_aabb.maxy,t_aabb.maxz});
                }
                if(t_minz-t_aabb.minz>0.01)
                {
                    aabbList.push_back(AABB{t_aabb.minx,t_aabb.miny,t_aabb.minz,
                                                 t_aabb.maxx,t_aabb.maxy,t_minz});
                }
                if(t_aabb.maxz-t_maxz>0.01)
                {
                    aabbList.push_back(AABB{t_aabb.minx,t_aabb.miny,t_maxz,
                                                 t_aabb.maxx,t_aabb.maxy,t_aabb.maxz});
                }
            }else
            {
                aabbList.push_back(t_aabb);
            }
        }
        //check for inscribed spaces
        for(int l=aabbList.length()-1;l>=0;l--)
        {
            AABB c1=aabbList[l];
            for(int k=0;k<l;k++)
            {
                AABB c2=aabbList[k];
                if(c1.minx>=c2.minx&&c1.miny>=c2.miny&&c1.minz>=c2.minz&&
                        c1.maxx<=c2.maxx&&c1.maxy<=c2.maxy&&c1.maxz<=c2.maxz)
                {
                    aabbList.removeAt(l);
                    break;
                }
                if(c2.minx>=c1.minx&&c2.miny>=c1.miny&&c2.minz>=c1.minz&&
                        c2.maxx<=c1.maxx&&c2.maxy<=c1.maxy&&c2.maxz<=c1.maxz)
                {
                    aabbList.removeAt(k);
                    l--;
                    continue;
                }
            }
        }
    }
    return placements;
}

GeneticHeuristicSolver::Chromosome crossingover(const GeneticHeuristicSolver::Chromosome &solution1, const GeneticHeuristicSolver::Chromosome &solution2)
{
    int boxCount=solution1.getBoxCount();
    if(boxCount<2)
        throw QString("Invalid box count");
    int cuttingPoint=Random::random(boxCount-2)+1;
    GeneticHeuristicSolver::Chromosome child;
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

void GeneticHeuristicSolver::Chromosome::mutation(bool rotateBoxes)
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

void GeneticHeuristicSolver::Chromosome::reduction(bool rotateBoxes)
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

void selection(QList<GeneticHeuristicSolver::Chromosome> &solutions, QList<float>& objectiveValues, float boundsVolume, int selectionCount)
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
    QList<GeneticHeuristicSolver::Chromosome> selectionArray;
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

float GeneticHeuristicSolver::Chromosome::objectiveFunction(const QVector<Box> &boxes, const Box &bounds, int strategy) const
{
    QList<AABB> aabbList;
    aabbList.push_back(AABB{0.f,0.f,0.f,bounds.w,bounds.h,bounds.l});
    float width=0.0f;
    float height=0.0f;
    float length=0.0f;
    float totalVolumeSum = 0.0;
    for(int i=0;i<genes.length();i++)
    {
        int gene=genes[i];
        const Box& box=boxes.at(gene).getOrientation(orientations[gene]);
        float bestFit=10e8;
        int bestAABB=-1;
        float bestDistance=10e8;
        for(int j=0;j<aabbList.length();j++)
        {
            AABB aabb=aabbList[j];
            float aabbw=aabb.maxx-aabb.minx;
            float aabbh=aabb.maxy-aabb.miny;
            float aabbl=aabb.maxz-aabb.minz;

            float x=aabb.minx+box.width();
            float y=aabb.miny+box.height();
            float z=aabb.minz+box.length();

            float envelopeX=qMax(x,width);
            float envelopeY=qMax(y,height);
            float envelopeZ=qMax(z,length);

            float fitness=0.0;
            float distance=x*x+y*y+z*z;
            if(box.width()>aabbw||box.height()>aabbh||box.length()>aabbl)
                continue;
            if(bounds.w<envelopeX||bounds.h<envelopeY||bounds.l<envelopeZ)
                continue;
            switch(strategy)
            {
            case VOLUME:
                fitness=envelopeX*envelopeY*envelopeZ;
                break;
            case EUCL_NORM:
                fitness=envelopeX*envelopeX+
                        envelopeY*envelopeY+
                        envelopeZ*envelopeZ;
                break;
            case MANH_NORM:
                fitness=envelopeX+
                        envelopeY+
                        envelopeZ;
                break;
            case CHEB_NORM:
                fitness=qMax(envelopeX,
                             qMax(envelopeY,
                                  envelopeZ));
                break;
            case EUCL_NORM_LOC:
                fitness=aabb.minx*aabb.minx+
                        aabb.miny*aabb.miny+
                        aabb.minz*aabb.minz;
                break;
            case MANH_NORM_LOC:
                fitness=aabb.minx+
                        aabb.miny+
                        aabb.minz;
                break;
            case CHEB_NORM_LOC:
                fitness=qMax(aabb.minx,
                             qMax(aabb.miny,
                                  aabb.minz));
                break;
            case TIGHT_SIDE:
                fitness=qMin(aabbw-box.width(),
                        qMin(aabbh-box.height(),
                        aabbl-box.length()));
                break;
            case LOOSE_SIDE:
                fitness=qMax(aabbw-box.width(),
                        qMax(aabbh-box.height(),
                        aabbl-box.length()));
                break;
            case SURFACE_AREA:
                fitness = envelopeX*envelopeY + envelopeY*envelopeZ + envelopeZ*envelopeX;
                break;
            case EMPTY_VOLUME:
                fitness = envelopeX*envelopeY*envelopeZ - totalVolumeSum - box.volume();
                break;
            }
            if(fitness<bestFit)
            {
                bestFit=fitness;
                bestDistance=distance;
                bestAABB=j;
            }else if(qAbs(fitness-bestFit)<0.001&&distance<bestDistance)
            {
                bestFit=fitness;
                bestDistance=distance;
                bestAABB=j;
            }
        }
        if(bestAABB==-1)
            return -1.0;
        totalVolumeSum+=box.volume();
        AABB aabb=aabbList[bestAABB];

        float minx=aabb.minx;
        float maxx=minx+box.width();
        float miny=aabb.miny;
        float maxy=miny+box.height();
        float minz=aabb.minz;
        float maxz=minz+box.length();
        width=qMax(maxx,width);
        height=qMax(maxy,height);
        length=qMax(maxz,length);


        //recompute all aabb's
        QStack<AABB> emptyspaces;
        for(int i=0;i<aabbList.length();i++)
        {
            emptyspaces.push(aabbList[i]);
        }
        aabbList.clear();
        while(emptyspaces.length()>0)
        {
            AABB t_aabb=emptyspaces.pop();
            float t_minx=qMax(minx,t_aabb.minx);
            float t_miny=qMax(miny,t_aabb.miny);
            float t_minz=qMax(minz,t_aabb.minz);

            float t_maxx=qMin(maxx,t_aabb.maxx);
            float t_maxy=qMin(maxy,t_aabb.maxy);
            float t_maxz=qMin(maxz,t_aabb.maxz);

            if(t_minx<=t_maxx&&t_miny<=t_maxy&&t_minz<=t_maxz)
            {
                //part of the box is inside aabb
                if(t_minx-t_aabb.minx>0.01)
                {
                    aabbList.push_back(AABB{t_aabb.minx,t_aabb.miny,t_aabb.minz,
                                                 t_minx,t_aabb.maxy,t_aabb.maxz});
                }
                if(t_aabb.maxx-t_maxx>0.01)
                {
                    aabbList.push_back(AABB{t_maxx,t_aabb.miny,t_aabb.minz,
                                                 t_aabb.maxx,t_aabb.maxy,t_aabb.maxz});
                }
                if(t_miny-t_aabb.miny>0.01)
                {
                    aabbList.push_back(AABB{t_aabb.minx,t_aabb.miny,t_aabb.minz,
                                                 t_aabb.maxx,t_miny,t_aabb.maxz});
                }
                if(t_aabb.maxy-t_maxy>0.01)
                {
                    aabbList.push_back(AABB{t_aabb.minx,t_maxy,t_aabb.minz,
                                                 t_aabb.maxx,t_aabb.maxy,t_aabb.maxz});
                }
                if(t_minz-t_aabb.minz>0.01)
                {
                    aabbList.push_back(AABB{t_aabb.minx,t_aabb.miny,t_aabb.minz,
                                                 t_aabb.maxx,t_aabb.maxy,t_minz});
                }
                if(t_aabb.maxz-t_maxz>0.01)
                {
                    aabbList.push_back(AABB{t_aabb.minx,t_aabb.miny,t_maxz,
                                                 t_aabb.maxx,t_aabb.maxy,t_aabb.maxz});
                }
            }else
            {
                aabbList.push_back(t_aabb);
            }
        }
        //check for inscribed spaces
        for(int l=aabbList.length()-1;l>=0;l--)
        {
            AABB c1=aabbList[l];
            for(int k=0;k<l;k++)
            {
                AABB c2=aabbList[k];
                if(c1.minx>=c2.minx&&c1.miny>=c2.miny&&c1.minz>=c2.minz&&
                        c1.maxx<=c2.maxx&&c1.maxy<=c2.maxy&&c1.maxz<=c2.maxz)
                {
                    aabbList.removeAt(l);
                    break;
                }
                if(c2.minx>=c1.minx&&c2.miny>=c1.miny&&c2.minz>=c1.minz&&
                        c2.maxx<=c1.maxx&&c2.maxy<=c1.maxy&&c2.maxz<=c1.maxz)
                {
                    aabbList.removeAt(k);
                    l--;
                    continue;
                }
            }
        }
    }
    return width*height*length;
}
