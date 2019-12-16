#include "geneticsolver.h"
#include "QThread"
#include <QElapsedTimer>
#include <QStack>
#include "random.h"
#include <time.h>
#include <QDebug>

//#define PACKING

GeneticSolver::GeneticSolver():population(10),maxIterations(50),
    maxTime(300.0f),requiredVolume(50.0f),
    mutationProb(0.25f),
    selectionCount(5),
    crossingoverCount(5),
    repairAttempts(4),
    rotateBoxes(false),
    compressBoxes(false)
{
}

void GeneticSolver::init(int population, int maxIterations, float maxTime, float requiredVolume, float mutationProb, int selectionCount, int crossingoverCount, int repairAttempts, bool rotateBoxes, bool compressBoxes)
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
    this->compressBoxes=compressBoxes;
    this->crossingoverCount=qMin(this->crossingoverCount,this->population-1);
}

QVector<BoxInfo> GeneticSolver::solve(const QVector<Box> &boxes, const Box &bounds)
{
    log.clear();

    int boxCount=boxes.length();//количество ящиков
    //int operatorCount=boxCount-1;//количество операторов в записи обратной польской нотации
    int chromosomeLength=boxCount*2-1;//количество элементов в записи обратной польской нотации
    QElapsedTimer timer;
    clock_t t;
    t = clock();
    timer.start();

    QList<Chromosome> solutions;
    QList<float> objectiveValues;

    Chromosome bestSolution;
    float bestVolume=10e8;

    float boundsVolume=bounds.w*bounds.l*bounds.h;//Максимальный допустимый объём

    //Вспомогательные массивы
    QList<int> crossingoverIndexes;
    for(int i=2;i<chromosomeLength-1;i++)
        crossingoverIndexes.push_back(i);

    QVector<bool> genePool;
    genePool.fill(true,boxCount);

    float boundsTotal=bounds.w+bounds.h+bounds.l;
    float wProb=bounds.w/boundsTotal;
    float hProb=wProb+bounds.l/boundsTotal;
    //float lProb=hProb+bounds.h/boundsTotal;

    float totalTime=0.0f;

    int currentIteration=0;
    while(true)
    {
        emit BoxPackingSolver::progress(qMax<int>(currentIteration*100/this->maxIterations,int(totalTime*100.f/this->maxTime)));
        int reductionIndex=solutions.length();
        do{
            for(int i=solutions.length();i<population;i++)
            {
                Chromosome c=generate(boxCount,rotateBoxes);
                solutions.push_back(c);
                float value=objectiveFunction(c,boxes,bounds,compressBoxes);
                objectiveValues.push_back(value);
                if(value>=0.0&&value<bestVolume)
                {
                    bestVolume=value;
                    bestSolution=c;
                }
            }
            for(int i=solutions.length()-1;i>=reductionIndex;i--)
            {
                if(objectiveValues[i]<0.0)
                {
                    Chromosome& c = solutions[i];
                    float objectiveValue=objectiveValues[i];
                    //Попробовать исправить
                    reduction(c,wProb,hProb,rotateBoxes);
                    objectiveValue=objectiveFunction(c,boxes,bounds,compressBoxes);
                    if(objectiveValue>=0.0)
                    {
                        objectiveValues[i]=objectiveValue;
                        if(objectiveValue<bestVolume)
                        {
                            bestVolume=objectiveValue;
                            bestSolution=solutions[i];
                        }
                        continue;
                    }
                    solutions.removeAt(i);
                    objectiveValues.removeAt(i);
                }
            }
            float currentTime=timer.elapsed()/10e2;
            currentTime=float(clock()-t)/CLOCKS_PER_SEC;
            totalTime=currentTime;
            emit BoxPackingSolver::progress(qMax<int>(currentIteration*100.0f/this->maxIterations,int(totalTime*100.f/this->maxTime)));

            if(currentTime>maxTime)
            {
                goto END;
            }
        }while(solutions.length()<population);

        //!!!!!!!!!!!!!!!!!!!!!
        //checkSolutions(solutions,boxCount);

        //crossingover
        for(int i=0;i<crossingoverCount;i++)
        {
            crossingover(solutions,objectiveValues,crossingoverIndexes,genePool,boxes,bounds,population,chromosomeLength,boxCount,compressBoxes);
        }
        //!!!!!!!!!!!!!!!!!!!!!
        //checkSolutions(solutions,boxCount);
        for(int i=0;i<solutions.length();i++)
        {
            if(Random::randomUnit()<=mutationProb)
            {
                Chromosome& c=solutions[i];
                //mutation
                mutation(c);
                //inversion
                inversion(c);
                objectiveValues[i]=objectiveFunction(c,boxes,bounds,compressBoxes);
            }
        }
        //!!!!!!!!!!!!!!!!!!!!!
        //checkSolutions(solutions,boxCount);

        currentIteration++;

        float currentTime=timer.elapsed()/10e2;
        currentTime=float(clock()-t)/CLOCKS_PER_SEC;
        if(currentTime>maxTime||currentIteration>=maxIterations||bestVolume<=requiredVolume*boundsVolume)
        {
            totalTime=currentTime;
            break;
        }

        //модифицированный оператор отбора
        //Исправление аутсайдеров и удаление в случае неудачи
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
                    reduction(c,wProb,hProb,rotateBoxes);

                    objectiveValue=objectiveFunction(c,boxes,bounds,compressBoxes);
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
        if(solutions.length()==0)
        {
            //Необходимо полное обновление популяции
            continue;
        }

        //!!!!!!!!!!!!!!!!!!!!!
        //checkSolutions(solutions,boxCount);
        //Селекция
        selection(solutions,objectiveValues,boundsVolume,selectionCount);
        QThread::msleep(10);
    }
    END:
    if(bestVolume<10e8)
    {
        float x,y,z;
        QVector<BoxInfo> placements=calculatePlacements(bestSolution,boxes,compressBoxes,x,y,z);
        log+="Объём: "+QString::number(bestVolume)+".\n";
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

Chromosome generate(int boxCount,bool rotateBoxes)
{

    //generate tree and convert to RPN
    struct treenode{int index;int left;int right;};
    QList<treenode> nodes;
    QList<treenode> oldNodes;
    for(int i=0;i<boxCount;i++)
    {
        nodes.push_back(treenode{i,-4,-4});
    }

    while(nodes.length()!=1)
    {
     //choose two random nodes and merge them with random operator
        int first=Random::random(nodes.length()-1);
        int second=(first+Random::random(nodes.length()-2,0)+1)%nodes.length();

        int f=oldNodes.length();
        oldNodes.push_back(nodes[first]);
        int s=oldNodes.length();
        oldNodes.push_back(nodes[second]);
        int op=Random::random(-3,-1);
        nodes.removeAt(qMax(first,second));
        nodes.removeAt(qMin(second,first));
        Q_ASSERT(first!=second);
        nodes.push_back(treenode{op,f,s});
    }
    Chromosome solution;
    QStack<treenode> stack;
    stack.push(nodes.first());
    while(stack.length()>0)
    {
        treenode node=stack.pop();
        solution.genes.push_front(node.index);
        if(node.index<0)
        {
            stack.push(oldNodes[node.left]);
            stack.push(oldNodes[node.right]);
        }
    }
    solution.orientations.fill(BoxOrientation::XYZ0,boxCount);
    if(rotateBoxes)
    {
        for(int i=0;i<boxCount;i++)
        {
            BoxOrientation o=orientations[Random::random(5)];
            solution.orientations[i]=o;
        }
    }
    return solution;
}
void reduction(Chromosome& c,const float wProb,const float hProb,const bool rotateBoxes)
{
    for(int j=0;j<c.genes.length();j++)
    {
        if(c.genes[j]<0)
        {
           //choose orientation according to container dimensions
           float rand=Random::randomUnit();
           int replacement=-3;
           if(rand<wProb)
               replacement=-1;
            else if(rand<hProb)
               replacement=-2;
           c.genes[j]=replacement;
        }else if(rotateBoxes)
        {
            BoxOrientation o=orientations[Random::random(5)];
            c.orientations[c.genes[j]]=o;
        }
    }
}
void selection(QList<Chromosome>& solutions,QList<float>& objectiveValues,float boundsVolume,int selectionCount)
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
    QList<Chromosome> selectionArray;
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

void mutation(Chromosome& c)
{
    int chromosomeLength=c.genes.length();
    int boxCount=(chromosomeLength+1)/2;
    int operatorCount=boxCount-1;

    //Выбрать случайный ген и поменять с другим таким же
    int geneNumber=Random::random(chromosomeLength-1);
    bool geneSign=c.genes[geneNumber]<0;
    int otherGene=geneSign?Random::random(operatorCount-1):Random::random(boxCount-1);
    for(int i=0,j=0;i<chromosomeLength;i++)
    {
        bool sign=c.genes[i]<0;
        if(sign==geneSign)
        {
            if(j==otherGene)
            {
                //swap
                int tmp=c.genes[geneNumber];
                c.genes[geneNumber]=c.genes[i];
                c.genes[i]=tmp;
                break;
            }
            j++;
        }
    }
}
void inversion(Chromosome& c)
{
    struct Segment
    {
        int index;
        int length;
        int distance;
    };
    Segment bestSegment=Segment{-1,0,0};
    for(int j=0;j<c.genes.length();)
    {
        bool sign=c.genes[j]<0;
        int k=j+1;
        bool flag=true;
        while(flag)
        {
            if(k!=c.genes.length())
            {
                bool currentSign=c.genes[k]<0;
                if(currentSign==sign)
                {
                    k++;
                    continue;
                }
                flag=false;
            }
            int segmentLength=k-j;
            if(segmentLength<2)
            {
                j=k;
                break;
            }
            //calulate hemming distance
            int distance=0;
            for(int l=0;l<segmentLength;l++)
            {
                if(c.genes[j+l]!=c.genes[k-1-l])
                {
                    distance++;
                }
            }
            if(distance>bestSegment.distance)
            {
                bestSegment=Segment{j,segmentLength,distance};
            }
            j=k;
        }
    }
    if(bestSegment.length>1)
    {
        for(int i=0;2*i<bestSegment.length-1;i++)
        {
            int index1=i+bestSegment.index;
            int index2=bestSegment.index+bestSegment.length-1-i;
            int tmp=c.genes[index1];
            c.genes[index1]=c.genes[index2];
            c.genes[index2]=tmp;
        }
    }
}
void crossingover(QList<Chromosome>& solutions,QList<float>& objectiveValues,const QList<int>& crossingoverIndexes,
                  const QVector<bool>& genePool,
                  const QVector<Box>& boxes,
                  const Box& bounds,
                  const int population,
                  const int chromosomeLength,
                  const int boxCount,
                  const bool tightPacking)
{
    //выбрать две хромосомы
    int index1=Random::random(population-1);
    //int index2=Random::random(population-1);
    /*if(index1==index2)
        continue;*/
    int index2=(index1+Random::random(population-1,1))%population;
    Chromosome c1=solutions[index1];
    Chromosome c2=solutions[index2];
    QList<int> tIndexes(crossingoverIndexes);
    int j=0;
    while((j=tIndexes.length())>0)
    {
        //выбрать точку разреза
        int t=Random::random(j-1);
        int cutIndex=tIndexes[t];
        tIndexes.removeAt(t);
        //TODO: проверить точку разреза
        if(c1.genes[cutIndex]<0&&c2.genes[cutIndex]<0)
        {
            //Проверить количество операторов
            int counter=0;
            for(int k=cutIndex+1;k<chromosomeLength;k++)
            {
                counter+=(c1.genes[k]<0);
                counter-=(c2.genes[k]<0);
            }
            if(counter!=0)
            {
                //соотношение не соблюдается
                continue;
            }

        }else
        {
            continue;
        }
        Chromosome child1;
        child1.orientations=QVector<BoxOrientation>(c1.orientations);
        child1.genes.resize(chromosomeLength);
        Chromosome child2;
        child2.orientations=QVector<BoxOrientation>(c2.orientations);
        child2.genes.resize(chromosomeLength);
        QVector<bool> genePool1(genePool);
        QVector<bool> genePool2(genePool);
        for(int k=0;k<cutIndex+1;k++)
        {
            int gene1=c1.genes[k];
            int gene2=c2.genes[k];
            child1.genes[k]=gene1;
            child2.genes[k]=gene2;
            if(gene1>=0)
                genePool1[gene1]=false;
            if(gene2>=0)
                genePool2[gene2]=false;
        }
        for(int k=cutIndex+1;k<chromosomeLength;k++)
        {
            int gene1=c1.genes[k];
            if(gene1<0)
            {
                child2.genes[k]=gene1;
            }
            else
            {
                if(genePool2[gene1]==false)
                {
                    gene1=-4;//special value
                }else
                    genePool2[gene1]=false;
                child2.genes[k]=gene1;
            }
            int gene2=c2.genes[k];
            if(gene2<0)
            {
                child1.genes[k]=gene2;
            }
            else
            {
                if(genePool1[gene2]==false)
                {
                    gene2=-4;//special value
                }else
                    genePool1[gene2]=false;
                child1.genes[k]=gene2;
            }
        }
        QVector<int> freeGenes1;
        QVector<int> freeGenes2;
        for(int k=0;k<boxCount;k++)
        {
            if(genePool1[k]==true)
                freeGenes1.push_back(k);
            if(genePool2[k]==true)
                freeGenes2.push_back(k);
        }
        for(int k=cutIndex+1;k<chromosomeLength;k++)
        {
            int gene1=child1.genes[k];
            if(gene1==-4)
            {
                child1.genes[k]=freeGenes1.last();
                freeGenes1.pop_back();
            }
            int gene2=child2.genes[k];
            if(gene2==-4)
            {
                child2.genes[k]=freeGenes2.last();
                freeGenes2.pop_back();
            }
        }
        solutions.push_back(child1);
        solutions.push_back(child2);
        objectiveValues.push_back(objectiveFunction(child1,boxes,bounds,tightPacking));
        objectiveValues.push_back(objectiveFunction(child2,boxes,bounds,tightPacking));
    }
}

void checkSolutions(const QList<Chromosome>& solutions,int boxCount)
{
    for(auto iter=solutions.begin();iter!=solutions.end();++iter)
    {
        QVector<bool> checkList;
        checkList.resize(boxCount);
        checkList.fill(false);
        for(auto iterGene=iter->genes.begin();iterGene!=iter->genes.end();++iterGene)
        {
            if(*iterGene>=0)
                checkList[*iterGene]=true;
        }
        for(auto iterCheckList=checkList.begin();iterCheckList!=checkList.end();++iterCheckList)
        {
            if(*iterCheckList==false)
            {
                qDebug()<<"Invalid solution";
            }
        }
    }
}

bool xCoordLessThan(const BoxInfo &d1, const BoxInfo &d2)
{
    return d1.x < d2.x; // sort by x
}
bool yCoordLessThan(const BoxInfo &d1, const BoxInfo &d2)
{
    return d1.y < d2.y; // sort by y
}
bool zCoordLessThan(const BoxInfo &d1, const BoxInfo &d2)
{
    return d1.z < d2.z; // sort by z
}

#define ALTERNATIVE
#ifdef ALTERNATIVE
float objectiveFunction(const Chromosome& chromosome,const QVector<Box>& boxes,const Box& bound,const bool tightPacking)
{
    struct block{float maxx;float maxy;float maxz;int left;int operation;};
    QList<block> blocks;
    QStack<block> operands;
    for(QVector<int>::const_iterator iter=chromosome.genes.constBegin();iter!=chromosome.genes.constEnd();++iter)
    {
        int currentGene=*iter;
        if(currentGene<0)
        {
            if(operands.length()<2)
                throw QString("Ошибка в записи ОПН");
            block a=operands.pop();//left operand
            block b=operands.pop();//right operand
            block result;
            result.left=blocks.length();
            blocks.push(a);
            blocks.push(b);
            result.operation=currentGene;
            switch(currentGene)
            {
                case -1:
                    result.maxx=a.maxx+b.maxx;
                    result.maxy=qMax(a.maxy,b.maxy);
                    result.maxz=qMax(a.maxz,b.maxz);
                    break;
                case -2:
                    result.maxx=qMax(a.maxx,b.maxx);
                    result.maxy=a.maxy+b.maxy;
                    result.maxz=qMax(a.maxz,b.maxz);
                    break;
                case -3:
                    result.maxx=qMax(a.maxx,b.maxx);
                    result.maxy=qMax(a.maxy,b.maxy);
                    result.maxz=a.maxz+b.maxz;
                    break;
            }
            operands.push(result);
        }else
        {
            Box box=boxes[currentGene].getOrientation(chromosome.orientations[currentGene]);

            operands.push(block{box.width(),box.height(),box.length(),-1,currentGene});
        }
    }
    block root=operands.pop();
    //objective function
    float w=root.maxx;
    float h=root.maxy;
    float l=root.maxz;
    return w*h*l;


    struct blockPos{float x;float y;float z;int index;};
    QStack<blockPos> stack;
    stack.push(blockPos{0.f,0.f,0.f,blocks.length()});
    blocks.push(root);
    QVector<BoxInfo> placements;
    while(!stack.isEmpty())
    {
        blockPos current=stack.pop();
        block currentBlock=blocks[current.index];
        if(parent.operation>=0)//not an operator
        {
            BoxInfo boxInfo;
            boxInfo.boxID=currentBlock.operation;
            boxInfo.x=current.x+currentBlock.maxx*0.5f;
            boxInfo.y=current.y+currentBlock.maxy*0.5f;
            boxInfo.z=current.z+currentBlock.maxz*0.5f;
            boxInfo.o=chromosome.orientations[currentBlock.operation];
            placements.push_back(placements);
        }else{
            float x1,y1,z1;
            x1=current.x;
            y1=current.y;
            z1=current.z;
            float x2,y2,z2;
            x2=x1;
            y2=y1;
            z2=z1;
            block left=blocks[currentBlock.left];
            switch(parent.operation)
            {
                case -1:
                x2+=left.maxx;
                break;
                case -2:
                y2+=left.maxy;
                break;
                case -3:
                z2+=left.maxz;
                break;
            }
            stack.push(blockPos{x1,y1,z1,currentBlock.left});
            stack.push(blockPos{x2,y2,z2,currentBlock.left+1});
        }
    }
    return placements;
#endif

float objectiveFunction(const Chromosome& chromosome,const QVector<Box>& boxes,const Box& bound,const bool tightPacking)
{
    typedef BoxInfo boxplacement;
    struct placement{float maxx;float maxy;float maxz;QList<boxplacement>* bpList;};

    QStack<placement> operands;
    for(QVector<int>::const_iterator iter=chromosome.genes.constBegin();iter!=chromosome.genes.constEnd();++iter)
    {
        int currentGene=*iter;
        if(currentGene<0)
        {
            if(operands.length()<2)
                throw QString("Ошибка в записи ОПН");
            placement a=operands.pop();//left operand
            placement b=operands.pop();//right operand
            placement result;
            switch(currentGene)
            {
                case -1:
                    {
                    for(QList<boxplacement>::iterator bpit=b.bpList->begin();bpit!=b.bpList->end();++bpit)
                    {
                        bpit->x+=a.maxx;
                        const Box& box=boxes.at(bpit->boxID).getOrientation(bpit->o);
                        float minx=bpit->x;
                        float miny=bpit->y;
                        float minz=bpit->z;
                        float maxx=bpit->x+box.width();
                        float maxy=bpit->y+box.height();
                        float maxz=bpit->z+box.length();
                        maxx=minx+box.width();
                        a.bpList->append(*bpit);
                    }
                    //reduceEMS(emsList);
                    result.maxx=a.maxx+b.maxx;
                    result.maxy=qMax(a.maxy,b.maxy);
                    result.maxz=qMax(a.maxz,b.maxz);
                    break;
                    }
                case -2:
            {
                for(QList<boxplacement>::iterator bpit=b.bpList->begin();bpit!=b.bpList->end();++bpit)
                {
                    bpit->y+=a.maxy;
                    const Box& box=boxes.at(bpit->boxID).getOrientation(bpit->o);
                    float minx=bpit->x;
                    float miny=bpit->y;
                    float minz=bpit->z;
                    float maxx=bpit->x+box.width();
                    float maxy=bpit->y+box.height();
                    float maxz=bpit->z+box.length();
                    maxy=miny+box.height();
                    //recompute ems
                    //recompute(emsList,minx,maxx,miny,maxy,minz,maxz);
                    a.bpList->append(*bpit);
                }
                //reduceEMS(emsList);
                //y
                result.maxx=qMax(a.maxx,b.maxx);
                result.maxy=a.maxy+b.maxy;
                result.maxz=qMax(a.maxz,b.maxz);
                    break;
            }
                case -3:
            {
                for(QList<boxplacement>::iterator bpit=b.bpList->begin();bpit!=b.bpList->end();++bpit)
                {
                    bpit->z+=a.maxz;
                    const Box& box=boxes.at(bpit->boxID).getOrientation(bpit->o);
                    float minx=bpit->x;
                    float miny=bpit->y;
                    float minz=bpit->z;
                    float maxx=bpit->x+box.width();
                    float maxy=bpit->y+box.height();
                    float maxz=bpit->z+box.length();
                    maxz=minz+box.length();
                    //recompute ems
                    //recompute(emsList,minx,maxx,miny,maxy,minz,maxz);
                    a.bpList->append(*bpit);
                }
                //reduceEMS(emsList);
                //z
                result.maxx=qMax(a.maxx,b.maxx);
                result.maxy=qMax(a.maxy,b.maxy);
                result.maxz=a.maxz+b.maxz;
                    break;
            }
            }
            result.bpList=a.bpList;
            delete b.bpList;
            operands.push(result);
        }else
        {
            Box box=boxes[currentGene].getOrientation(chromosome.orientations[currentGene]);
            QList<boxplacement>* bpList=new QList<boxplacement>();
            bpList->append(boxplacement{0.f,0.f,0.f,currentGene,chromosome.orientations[currentGene]});
            operands.push(placement{box.width(),box.height(),box.length(),bpList});
        }
    }
    if(operands.length()!=1)
    {
        for(int i=0;i<operands.length();i++)
        {
            delete operands[i].bpList;
        }
        throw QString("Ошибка в записи ОПН");
    }
    placement last=operands.pop();

    if(tightPacking)
    {
        last.maxx=0.0;
        last.maxy=0.0;
        last.maxz=0.0;
        //x
        qSort(last.bpList->begin(),last.bpList->end(),xCoordLessThan);
        for(QList<boxplacement>::iterator bpit=(last.bpList->begin());bpit!=last.bpList->end();++bpit)
        {
            const Box& box=boxes.at(bpit->boxID).getOrientation(bpit->o);
            float minx=bpit->x;
            float miny=bpit->y;
            float minz=bpit->z;
            minx=0.0;
            float maxx=minx+box.width();
            float maxy=bpit->y+box.height();
            float maxz=bpit->z+box.length();

            for(QList<boxplacement>::iterator apit=last.bpList->begin();apit!=bpit;++apit)
            {
                const Box& abox=boxes.at(apit->boxID).getOrientation(apit->o);
                float aminx=apit->x;
                float aminy=apit->y;
                float aminz=apit->z;
                float amaxx=aminx+abox.width();
                float amaxy=aminy+abox.height();
                float amaxz=aminz+abox.length();
                aminx=0.0f;
                if((aminx < maxx && amaxx > minx) &&
                         (aminy < maxy && amaxy > miny) &&
                         (aminz < maxz && amaxz > minz))
                {
                    minx=amaxx;
                    maxx=minx+box.width();
                }
            }
            bpit->x=minx;
            maxx=minx+box.width();
            last.maxx=qMax(maxx,last.maxx);
        }
        //y
        qSort(last.bpList->begin(),last.bpList->end(),yCoordLessThan);
        for(QList<boxplacement>::iterator bpit=(last.bpList->begin());bpit!=last.bpList->end();++bpit)
        {
            const Box& box=boxes.at(bpit->boxID).getOrientation(bpit->o);
            float minx=bpit->x;
            float miny=bpit->y;
            float minz=bpit->z;
            miny=0.0;
            float maxx=bpit->x+box.width();
            float maxy=miny+box.height();
            float maxz=bpit->z+box.length();
            for(QList<boxplacement>::iterator apit=last.bpList->begin();apit!=bpit;++apit)
            {
                const Box& abox=boxes.at(apit->boxID).getOrientation(apit->o);
                float aminx=apit->x;
                float aminy=apit->y;
                float aminz=apit->z;
                float amaxx=aminx+abox.width();
                float amaxy=aminy+abox.height();
                float amaxz=aminz+abox.length();
                aminy=0.0f;
                if((aminx < maxx && amaxx > minx) &&
                         (aminy < maxy && amaxy > miny) &&
                         (aminz < maxz && amaxz > minz))
                {
                    miny=amaxy;
                    maxy=miny+box.height();
                }
            }
            bpit->y=miny;
            maxy=miny+box.height();
            last.maxy=qMax(maxy,last.maxy);
        }
        //z
        qSort(last.bpList->begin(),last.bpList->end(),zCoordLessThan);
        for(QList<boxplacement>::iterator bpit=last.bpList->begin();bpit!=last.bpList->end();++bpit)
        {
            const Box& box=boxes.at(bpit->boxID).getOrientation(bpit->o);
            float minx=bpit->x;
            float miny=bpit->y;
            float minz=bpit->z;
            minz=0.0f;
            float maxx=bpit->x+box.width();
            float maxy=bpit->y+box.height();
            float maxz=minz+box.length();
            for(QList<boxplacement>::iterator apit=last.bpList->begin();apit!=bpit;++apit)
            {
                const Box& abox=boxes.at(apit->boxID).getOrientation(apit->o);
                float aminx=apit->x;
                float aminy=apit->y;
                float aminz=apit->z;
                float amaxx=aminx+abox.width();
                float amaxy=aminy+abox.height();
                float amaxz=aminz+abox.length();
                aminz=0.0f;
                if((aminx <= maxx && amaxx >= minx) &&
                         (aminy <= maxy && amaxy >= miny) &&
                         (amaxz <= bpit->z))
                {
                    minz=qMax(amaxz,minz);
                }
            }
            bpit->z=minz;
            maxz=minz+box.length();
            last.maxz=qMax(maxz,last.maxz);
        }
    }
    delete last.bpList;
    float volume=last.maxx*last.maxy*last.maxz;
    if(last.maxx>bound.w||last.maxy>bound.h||last.maxz>bound.l)
    {
        return -volume;
    }
    //static_assert(false,"Not implemented");
    return volume;
}
QVector<BoxInfo> calculatePlacements(const Chromosome& chromosome,const QVector<Box>& boxes,const bool tightPacking,float&w,float&h,float&l)
{    typedef BoxInfo boxplacement;
     struct placement{float maxx;float maxy;float maxz;QList<boxplacement>* bpList;};


     QStack<placement> operands;
     for(QVector<int>::const_iterator iter=chromosome.genes.constBegin();iter!=chromosome.genes.constEnd();++iter)
     {
         int currentGene=*iter;
         if(currentGene<0)
         {
             if(operands.length()<2)
                 throw QString("Ошибка в записи ОПН");
             placement a=operands.pop();//left operand
             placement b=operands.pop();//right operand
             placement result;
             switch(currentGene)
             {
                 case -1:
                     {
                     for(QList<boxplacement>::iterator bpit=b.bpList->begin();bpit!=b.bpList->end();++bpit)
                     {
                         bpit->x+=a.maxx;
                         const Box& box=boxes.at(bpit->boxID).getOrientation(bpit->o);
                         float minx=bpit->x;
                         float miny=bpit->y;
                         float minz=bpit->z;
                         float maxx=bpit->x+box.width();
                         float maxy=bpit->y+box.height();
                         float maxz=bpit->z+box.length();
                         maxx=minx+box.width();
                         //recompute ems
                         //recompute(emsList,minx,maxx,miny,maxy,minz,maxz);
                         a.bpList->append(*bpit);
                     }
                     //reduceEMS(emsList);
                     result.maxx=a.maxx+b.maxx;
                     result.maxy=qMax(a.maxy,b.maxy);
                     result.maxz=qMax(a.maxz,b.maxz);
                    break;
                     }
                 case -2:
             {
                     for(QList<boxplacement>::iterator bpit=b.bpList->begin();bpit!=b.bpList->end();++bpit)
                     {
                         bpit->y+=a.maxy;
                         const Box& box=boxes.at(bpit->boxID).getOrientation(bpit->o);
                         float minx=bpit->x;
                         float miny=bpit->y;
                         float minz=bpit->z;
                         float maxx=bpit->x+box.width();
                         float maxy=bpit->y+box.height();
                         float maxz=bpit->z+box.length();
                         maxy=miny+box.height();
                         //recompute ems
                         //recompute(emsList,minx,maxx,miny,maxy,minz,maxz);
                         a.bpList->append(*bpit);
                     }
                     //reduceEMS(emsList);
                     //y
                     result.maxx=qMax(a.maxx,b.maxx);
                     result.maxy=a.maxy+b.maxy;
                     result.maxz=qMax(a.maxz,b.maxz);
                     break;
             }
                 case -3:
             {
                     for(QList<boxplacement>::iterator bpit=b.bpList->begin();bpit!=b.bpList->end();++bpit)
                     {
                         bpit->z+=a.maxz;
                         const Box& box=boxes.at(bpit->boxID).getOrientation(bpit->o);
                         float minx=bpit->x;
                         float miny=bpit->y;
                         float minz=bpit->z;
                         float maxx=bpit->x+box.width();
                         float maxy=bpit->y+box.height();
                         float maxz=bpit->z+box.length();
                         maxz=minz+box.length();
                         //recompute ems
                         //recompute(emsList,minx,maxx,miny,maxy,minz,maxz);
                         a.bpList->append(*bpit);
                     }
                     //reduceEMS(emsList);
                     //z
                     result.maxx=qMax(a.maxx,b.maxx);
                     result.maxy=qMax(a.maxy,b.maxy);
                     result.maxz=a.maxz+b.maxz;
                     break;
             }
             }
             result.bpList=a.bpList;
             operands.push(result);
             delete b.bpList;
         }else
         {
             Box box=boxes[currentGene].getOrientation(chromosome.orientations[currentGene]);
             QList<boxplacement>* bpList=new QList<boxplacement>();
             bpList->append(boxplacement{0.f,0.f,0.f,currentGene,chromosome.orientations[currentGene]});
             operands.push(placement{box.width(),box.height(),box.length(),bpList});
         }
     }
     if(operands.length()!=1)
     {
         for(int i=0;i<operands.length();i++)
         {
             delete operands[i].bpList;
         }
         throw QString("Ошибка в записи ОПН");
     }
     placement last=operands.pop();

      if(tightPacking)
      {
          last.maxx=0.0;
          last.maxy=0.0;
          last.maxz=0.0;
          //x
          qSort(last.bpList->begin(),last.bpList->end(),xCoordLessThan);
          for(QList<boxplacement>::iterator bpit=(last.bpList->begin());bpit!=last.bpList->end();++bpit)
          {
              const Box& box=boxes.at(bpit->boxID).getOrientation(bpit->o);
              float minx=bpit->x;
              float miny=bpit->y;
              float minz=bpit->z;
              minx=0.0;
              float maxx=minx+box.width();
              float maxy=bpit->y+box.height();
              float maxz=bpit->z+box.length();

              for(QList<boxplacement>::iterator apit=last.bpList->begin();apit!=bpit;++apit)
              {
                  const Box& abox=boxes.at(apit->boxID).getOrientation(apit->o);
                  float aminx=apit->x;
                  float aminy=apit->y;
                  float aminz=apit->z;
                  float amaxx=aminx+abox.width();
                  float amaxy=aminy+abox.height();
                  float amaxz=aminz+abox.length();
                  aminx=0.0f;
                  if((aminy < maxy && amaxy > miny) &&
                           (aminz < maxz && amaxz > minz) &&
                           (amaxx <= bpit->x))
                  {
                      minx=qMax(amaxx,minx);
                  }
              }
              bpit->x=minx;
              maxx=minx+box.width();
              last.maxx=qMax(maxx,last.maxx);
          }
          //y
          qSort(last.bpList->begin(),last.bpList->end(),yCoordLessThan);
          for(QList<boxplacement>::iterator bpit=(last.bpList->begin());bpit!=last.bpList->end();++bpit)
          {
              const Box& box=boxes.at(bpit->boxID).getOrientation(bpit->o);
              float minx=bpit->x;
              float miny=bpit->y;
              float minz=bpit->z;
              miny=0.0;
              float maxx=bpit->x+box.width();
              float maxy=miny+box.height();
              float maxz=bpit->z+box.length();
              for(QList<boxplacement>::iterator apit=last.bpList->begin();apit!=bpit;++apit)
              {
                  const Box& abox=boxes.at(apit->boxID).getOrientation(apit->o);
                  float aminx=apit->x;
                  float aminy=apit->y;
                  float aminz=apit->z;
                  float amaxx=aminx+abox.width();
                  float amaxy=aminy+abox.height();
                  float amaxz=aminz+abox.length();
                  aminy=0.0f;
                  if((aminx < maxx && amaxx > minx) &&
                           (aminz < maxz && amaxz > minz) &&
                           (amaxy <= bpit->y))
                  {
                      miny=qMax(amaxy,miny);
                  }
              }
              bpit->y=miny;
              maxy=miny+box.height();
              last.maxy=qMax(maxy,last.maxy);
          }
          //z
          qSort(last.bpList->begin(),last.bpList->end(),zCoordLessThan);
          for(QList<boxplacement>::iterator bpit=last.bpList->begin();bpit!=last.bpList->end();++bpit)
          {
              const Box& box=boxes.at(bpit->boxID).getOrientation(bpit->o);
              float minx=bpit->x;
              float miny=bpit->y;
              float minz=bpit->z;
              minz=0.0f;
              float maxx=bpit->x+box.width();
              float maxy=bpit->y+box.height();
              float maxz=minz+box.length();
              for(QList<boxplacement>::iterator apit=last.bpList->begin();apit!=bpit;++apit)
              {
                  const Box& abox=boxes.at(apit->boxID).getOrientation(apit->o);
                  float aminx=apit->x;
                  float aminy=apit->y;
                  float aminz=apit->z;
                  float amaxx=aminx+abox.width();
                  float amaxy=aminy+abox.height();
                  float amaxz=aminz+abox.length();
                  aminz=0.0f;
                  if((aminx < maxx && amaxx > minx) &&
                           (aminy < maxy && amaxy > miny) &&
                           (amaxz <= bpit->z))
                  {
                      minz=qMax(amaxz,minz);
                  }
              }
              bpit->z=minz;
              maxz=minz+box.length();
              last.maxz=qMax(maxz,last.maxz);
          }
      }

     QVector<BoxInfo> placements=QVector<BoxInfo>::fromList(*last.bpList);
     for(QVector<BoxInfo>::iterator iter=placements.begin();iter!=placements.end();++iter)
     {
         const Box& box=boxes[iter->boxID].getOrientation(iter->o);
         iter->x+=box.width()*0.5f;
         iter->y+=box.height()*0.5f;
         iter->z+=box.length()*0.5f;
     }
     delete last.bpList;
     w=last.maxx;
     h=last.maxy;
     l=last.maxz;
     return placements;
}

void recompute(QList<emsstruct>* emsList,float minx,float maxx,float miny,float maxy,float minz,float maxz)
{
    int listSize=emsList->count();
    for(int j=0;j<listSize;j++)
    {
        emsstruct t_ems=emsList->operator[](j);
        float t_minx=qMax(minx,t_ems.minx);
        float t_miny=qMax(miny,t_ems.miny);
        float t_minz=qMax(minz,t_ems.minz);

        float t_maxx=qMin(maxx,t_ems.maxx);
        float t_maxy=qMin(maxy,t_ems.maxy);
        float t_maxz=qMin(maxz,t_ems.maxz);

        if(t_minx<=t_maxx&&t_miny<=t_maxy&&t_minz<=t_maxz)
        {
            emsList->removeAt(j);
            j--;
            listSize--;
            //part of the box is inside ems
            if(t_minx-t_ems.minx>0.01)
            {
                emsList->push_back(emsstruct{t_ems.minx,t_ems.miny,t_ems.minz,
                                             t_minx,t_ems.maxy,t_ems.maxz});
            }
            if(t_ems.maxx-t_maxx>0.01)
            {
                emsList->push_back(emsstruct{t_maxx,t_ems.miny,t_ems.minz,
                                             t_ems.maxx,t_ems.maxy,t_ems.maxz});
            }
            if(t_miny-t_ems.miny>0.01)
            {
                emsList->push_back(emsstruct{t_ems.minx,t_ems.miny,t_ems.minz,
                                             t_ems.maxx,t_miny,t_ems.maxz});
            }
            if(t_ems.maxy-t_maxy>0.01)
            {
                emsList->push_back(emsstruct{t_ems.minx,t_maxy,t_ems.minz,
                                             t_ems.maxx,t_ems.maxy,t_ems.maxz});
            }
            if(t_minz-t_ems.minz>0.01)
            {
                emsList->push_back(emsstruct{t_ems.minx,t_ems.miny,t_ems.minz,
                                             t_ems.maxx,t_ems.maxy,t_minz});
            }
            if(t_ems.maxz-t_maxz>0.01)
            {
                emsList->push_back(emsstruct{t_ems.minx,t_ems.miny,t_maxz,
                                             t_ems.maxx,t_ems.maxy,t_ems.maxz});
            }
        }
    }
}
void reduceEMS(QList<emsstruct>* emsList)
{
    //check for inscribed spaces
    for(int l=emsList->length()-1;l>=0;l--)
    {
        emsstruct c1=emsList->operator [](l);

        if(c1.maxx-c1.minx<0.01||c1.maxy-c1.miny<0.01||c1.maxz-c1.minz<0.01)
        {
            emsList->removeAt(l);
            continue;
        }
        for(int k=0;k<l;k++)
        {
            emsstruct c2=emsList->operator [](k);
            if(c1.minx>=c2.minx&&c1.miny>=c2.miny&&c1.minz>=c2.minz&&
                    c1.maxx<=c2.maxx&&c1.maxy<=c2.maxy&&c1.maxz<=c2.maxz)
            {
                emsList->removeAt(l);
                break;
            }
            if(c2.minx>=c1.minx&&c2.miny>=c1.miny&&c2.minz>=c1.minz&&
                    c2.maxx<=c1.maxx&&c2.maxy<=c1.maxy&&c2.maxz<=c1.maxz)
            {
                emsList->removeAt(k);
                l--;
                continue;
            }
        }
    }
}


