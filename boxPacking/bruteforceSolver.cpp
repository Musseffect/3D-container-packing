#include "bruteForceSolver.h"
#include <QStack>
#include <QDebug>
#include <time.h>


QString BruteForceSolver::criteriaNames[] = {"L2 норма AABB",
                           "L1 норма AABB",
                           "L∞ норма AABB",
                           "Объём AABB",
                           "L2 норма текущего края",
                           "L1 норма текущего края",
                           "L∞ норма текущего края",
                           "Площадь поверхности AABB",
                           "Незаполненный объём"
                          };


BruteForceSolver::BruteForceSolver():rotateBoxes(true),placementCriteria(BruteForceCriteriaType::VOLUME),maxTime(30.f)
{
}
void BruteForceSolver::init(bool rotateBoxes,int placementCriteria,float maxTime)
{
    this->rotateBoxes=rotateBoxes;
    this->placementCriteria=placementCriteria;
    this->maxTime=maxTime;
}
QVector<BoxInfo> BruteForceSolver::decodeSolution(const QVector<int>& rotationSequence,const QVector<int>& boxSequence,const QVector<Box>& boxes,const Box& bounds,float&w,float &h,float &l)
{
    float containerWidth=0.0f;
    float containerHeight=0.0f;
    float containerLength=0.0f;

    QList<AABB> aabbArray;
    aabbArray.push_back(AABB{0.f,0.f,0.f,
                       bounds.w,bounds.h,bounds.l});
    QVector<BoxInfo> placements;
    float totalVolumeSum = 0.;
    for(int i=0;i<boxes.length();i++)
    {
        int boxIndex=boxSequence[i];
        BoxOrientation orientation=orientations[rotationSequence[boxIndex]];
        Box box=boxes[boxIndex].getOrientation(orientation);
        int bestAABB=-1;
        float bestFitValue=10e8;
        for(int j=0;j<aabbArray.length();j++)
        {
            AABB aabb=aabbArray[j];
            float w=aabb.maxx-aabb.minx;
            float h=aabb.maxy-aabb.miny;
            float l=aabb.maxz-aabb.minz;
            if(box.width()>w||box.height()>h||box.length()>l)
                continue;
            float fitness;
            float maxx=aabb.minx+box.width();
            float maxy=aabb.miny+box.height();
            float maxz=aabb.minz+box.length();

            float envelopeX=qMax(containerWidth,maxx);
            float envelopeY=qMax(containerHeight,maxy);
            float envelopeZ=qMax(containerLength,maxz);
            switch(placementCriteria)
            {
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
                case VOLUME:
                    fitness=envelopeX*
                            envelopeY*
                            envelopeZ;
                    break;
                case EUCL_NORM_LOC:
                    fitness=maxx*maxx+
                            maxy*maxy+
                            maxz*maxz;
                    break;
                case MANH_NORM_LOC:
                    fitness=maxx+
                            maxy+
                            maxz;
                    break;
                case CHEB_NORM_LOC:
                    fitness=qMax(maxx,
                            qMax(maxy,
                            maxz));
                    break;
                case SURFACE_AREA:
                    fitness=envelopeX*envelopeY + envelopeY*envelopeZ + envelopeZ*envelopeX;
                    break;
                case EMPTY_VOLUME:
                    fitness=envelopeX*envelopeY*envelopeZ - totalVolumeSum + box.volume();
                    break;
            }
            if(bestFitValue>fitness)
            {
                bestFitValue=fitness;
                bestAABB=j;
            }
        }
        Q_ASSERT(bestAABB!=-1);
        totalVolumeSum+=box.volume();
        AABB aabb=aabbArray[bestAABB];
        BoxInfo placement=BoxInfo{aabb.minx+box.width()*0.5f,
                aabb.miny+box.height()*0.5f,
                aabb.minz+box.length()*0.5f,
                boxIndex,
                orientation};
        placements.push_back(placement);
        float minx=aabb.minx;
        float maxx=minx+box.width();
        float miny=aabb.miny;
        float maxy=miny+box.height();
        float minz=aabb.minz;
        float maxz=minz+box.length();
        containerWidth=qMax(containerWidth,maxx);
        containerHeight=qMax(containerHeight,maxy);
        containerLength=qMax(containerLength,maxz);

        QStack<AABB> emptyspaces;
        for(int j=0;j<aabbArray.length();j++)
        {
            emptyspaces.push(aabbArray[j]);
        }
        aabbArray.clear();
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
                    aabbArray.push_back(AABB{t_aabb.minx,t_aabb.miny,t_aabb.minz,
                                                 t_minx,t_aabb.maxy,t_aabb.maxz});
                }
                if(t_aabb.maxx-t_maxx>0.01)
                {
                    aabbArray.push_back(AABB{t_maxx,t_aabb.miny,t_aabb.minz,
                                                 t_aabb.maxx,t_aabb.maxy,t_aabb.maxz});
                }
                if(t_miny-t_aabb.miny>0.01)
                {
                    aabbArray.push_back(AABB{t_aabb.minx,t_aabb.miny,t_aabb.minz,
                                                 t_aabb.maxx,t_miny,t_aabb.maxz});
                }
                if(t_aabb.maxy-t_maxy>0.01)
                {
                    aabbArray.push_back(AABB{t_aabb.minx,t_maxy,t_aabb.minz,
                                                 t_aabb.maxx,t_aabb.maxy,t_aabb.maxz});
                }
                if(t_minz-t_aabb.minz>0.01)
                {
                    aabbArray.push_back(AABB{t_aabb.minx,t_aabb.miny,t_aabb.minz,
                                                 t_aabb.maxx,t_aabb.maxy,t_minz});
                }
                if(t_aabb.maxz-t_maxz>0.01)
                {
                    aabbArray.push_back(AABB{t_aabb.minx,t_aabb.miny,t_maxz,
                                                 t_aabb.maxx,t_aabb.maxy,t_aabb.maxz});
                }
            }else
            {
                aabbArray.push_back(t_aabb);
            }
        }
        //check for inscribed spaces
        for(int l=aabbArray.length()-1;l>=0;l--)
        {
            AABB c1=aabbArray[l];
            for(int k=0;k<l;k++)
            {
                AABB c2=aabbArray[k];
                if(c1.minx>=c2.minx&&c1.miny>=c2.miny&&c1.minz>=c2.minz&&
                        c1.maxx<=c2.maxx&&c1.maxy<=c2.maxy&&c1.maxz<=c2.maxz)
                {
                    aabbArray.removeAt(l);
                    break;
                }
                if(c2.minx>=c1.minx&&c2.miny>=c1.miny&&c2.minz>=c1.minz&&
                        c2.maxx<=c1.maxx&&c2.maxy<=c1.maxy&&c2.maxz<=c1.maxz)
                {
                    aabbArray.removeAt(k);
                    l--;
                    continue;
                }
            }
        }

    }
    w=containerWidth;
    h=containerHeight;
    l=containerLength;
    return placements;
}
float BruteForceSolver::evalSolution(const QVector<int>& rotationSequence,const QVector<int>& boxSequence,const QVector<Box>& boxes,const Box& bounds)
{
    float containerWidth=0.0f;
    float containerHeight=0.0f;
    float containerLength=0.0f;

    QList<AABB> aabbArray;
    aabbArray.push_back(AABB{0.f,0.f,0.f,
                       bounds.w,bounds.h,bounds.l});
    float totalVolumeSum = 0.0;
    for(int i=0;i<boxes.length();i++)
    {
        int boxIndex=boxSequence[i];
        BoxOrientation orientation=orientations[rotationSequence[boxIndex]];
        Box box=boxes[boxIndex].getOrientation(orientation);
        int bestAABB=-1;
        float bestFitValue=10e8;
        //for each possible placement
        for(int j=0;j<aabbArray.length();j++)
        {
            AABB aabb=aabbArray[j];
            float w=aabb.maxx-aabb.minx;
            float h=aabb.maxy-aabb.miny;
            float l=aabb.maxz-aabb.minz;;
            if(box.width()>w||box.height()>h||box.length()>l)
                continue;
            float fitness;
            float maxx=aabb.minx+box.width();
            float maxy=aabb.miny+box.height();
            float maxz=aabb.minz+box.length();

            float envelopeX=qMax(containerWidth,maxx);
            float envelopeY=qMax(containerHeight,maxy);
            float envelopeZ=qMax(containerLength,maxz);
            switch(placementCriteria)
            {
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
                case VOLUME:
                    fitness=envelopeX*
                            envelopeY*
                            envelopeZ;
                    break;
                case EUCL_NORM_LOC:
                    fitness=maxx*maxx+
                            maxy*maxy+
                            maxz*maxz;
                    break;
                case MANH_NORM_LOC:
                    fitness=maxx+
                            maxy+
                            maxz;
                    break;
                case CHEB_NORM_LOC:
                    fitness=qMax(maxx,
                            qMax(maxy,
                            maxz));
                    break;
                case SURFACE_AREA:
                    fitness=envelopeX*envelopeY + envelopeY*envelopeZ + envelopeZ*envelopeX;
                    break;
                case EMPTY_VOLUME:
                    fitness=envelopeX*envelopeY*envelopeZ - totalVolumeSum + box.volume();
                    break;
            }
            if(bestFitValue>fitness)
            {
                bestFitValue=fitness;
                bestAABB=j;
            }
        }
        if(bestAABB==-1)
            return -1.0;
        totalVolumeSum+=box.volume();
        AABB aabb=aabbArray[bestAABB];

        float minx=aabb.minx;
        float maxx=minx+box.width();
        float miny=aabb.miny;
        float maxy=miny+box.height();
        float minz=aabb.minz;
        float maxz=minz+box.length();
        containerWidth=qMax(containerWidth,maxx);
        containerHeight=qMax(containerHeight,maxy);
        containerLength=qMax(containerLength,maxz);

        QStack<AABB> emptyspaces;
        for(int j=0;j<aabbArray.length();j++)
        {
            emptyspaces.push(aabbArray[j]);
        }
        aabbArray.clear();
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
                    aabbArray.push_back(AABB{t_aabb.minx,t_aabb.miny,t_aabb.minz,
                                                 t_minx,t_aabb.maxy,t_aabb.maxz});
                }
                if(t_aabb.maxx-t_maxx>0.01)
                {
                    aabbArray.push_back(AABB{t_maxx,t_aabb.miny,t_aabb.minz,
                                                 t_aabb.maxx,t_aabb.maxy,t_aabb.maxz});
                }
                if(t_miny-t_aabb.miny>0.01)
                {
                    aabbArray.push_back(AABB{t_aabb.minx,t_aabb.miny,t_aabb.minz,
                                                 t_aabb.maxx,t_miny,t_aabb.maxz});
                }
                if(t_aabb.maxy-t_maxy>0.01)
                {
                    aabbArray.push_back(AABB{t_aabb.minx,t_maxy,t_aabb.minz,
                                                 t_aabb.maxx,t_aabb.maxy,t_aabb.maxz});
                }
                if(t_minz-t_aabb.minz>0.01)
                {
                    aabbArray.push_back(AABB{t_aabb.minx,t_aabb.miny,t_aabb.minz,
                                                 t_aabb.maxx,t_aabb.maxy,t_minz});
                }
                if(t_aabb.maxz-t_maxz>0.01)
                {
                    aabbArray.push_back(AABB{t_aabb.minx,t_aabb.miny,t_maxz,
                                                 t_aabb.maxx,t_aabb.maxy,t_aabb.maxz});
                }
            }else
            {
                aabbArray.push_back(t_aabb);
            }
        }
        //check for inscribed spaces
        for(int l=aabbArray.length()-1;l>=0;l--)
        {
            AABB c1=aabbArray[l];
            for(int k=0;k<l;k++)
            {
                AABB c2=aabbArray[k];
                if(c1.minx>=c2.minx&&c1.miny>=c2.miny&&c1.minz>=c2.minz&&
                        c1.maxx<=c2.maxx&&c1.maxy<=c2.maxy&&c1.maxz<=c2.maxz)
                {
                    aabbArray.removeAt(l);
                    break;
                }
                if(c2.minx>=c1.minx&&c2.miny>=c1.miny&&c2.minz>=c1.minz&&
                        c2.maxx<=c1.maxx&&c2.maxy<=c1.maxy&&c2.maxz<=c1.maxz)
                {
                    aabbArray.removeAt(k);
                    l--;
                    continue;
                }
            }
        }

    }

    return containerWidth*containerHeight*containerLength;
}
unsigned int factorial(unsigned int n)
{
    unsigned int result=1;
    for(int i=n;i>0;i--)
    {
        result*=i;
    }
    return result;
}
QVector<BoxInfo> BruteForceSolver::solve(const QVector<Box>& boxes,const Box& bounds)
{
    clock_t t;
    t = clock();
    int localRotations=1;
    unsigned int rotations=1;
    if(rotateBoxes)
    {
        localRotations=6;
        rotations=pow(6,boxes.length());
        if(boxes.length()>12)
        {
            rotations=4294967295;
        }
    }
    unsigned int permutations;
    if(boxes.length()>12)
    {
        permutations=4294967295;//max number for uint
    }else
        permutations=factorial(boxes.length());

    struct{unsigned int permutationIndex;unsigned int rotationIndex;float volume;bool flag;} bestSolution;
    bestSolution.flag=false;
    bestSolution.permutationIndex=0;
    bestSolution.rotationIndex=0;
    bestSolution.volume=10e8;

    QVector<int> permutationIndices;
    for(int i=0;i<boxes.length();i++)
        permutationIndices.push_back(i);

    float totalVolumeSum = 0.0;

    for(unsigned int j=0;j<rotations;j++)
    {
        //список ориентаций n ящиков
        QVector<int> rotationSequence;
        //разложение по поворотам
        unsigned int rotationIndex=j;
        for(int i=0;i<boxes.length();i++)
        {
            unsigned int index=rotationIndex%localRotations;
            rotationIndex=rotationIndex/localRotations;
            rotationSequence.push_back(index);
            totalVolumeSum +=boxes[i].volume();
        }
        for(unsigned int i=0;i<permutations;i++)
        {
            float progress=(float(i)/permutations)/rotations+float(j)/permutations;
            float currentTime=float(clock()-t)/CLOCKS_PER_SEC;
            progress=qMax(progress,currentTime/maxTime);
            emit BoxPackingSolver::progress(qMin(progress*100.f,100.f));

            QVector<int> boxSequence;
            //разложение по ящикам
            int permutationIndex=i;
            QVector<int> tPermutationIndices(permutationIndices);
            for(unsigned int k=0;k<boxes.length();k++)
            {
                unsigned int indices=boxes.length()-k;
                unsigned int index=permutationIndex%indices;
                permutationIndex=permutationIndex/indices;
                boxSequence.push_back(tPermutationIndices[index]);
                tPermutationIndices.removeAt(index);
            }
            float volume=evalSolution(rotationSequence,boxSequence,boxes,bounds);
            if(volume>=0.0f && volume<bestSolution.volume)
            {
                bestSolution.volume=volume;
                bestSolution.rotationIndex=j;
                bestSolution.permutationIndex=i;
                bestSolution.flag=true;
            }
            if(currentTime>maxTime)
            {
                if(bestSolution.flag==false)
                    throw QString("Не удалось найти допустимое решение за выделенное время.");

                QVector<int> rotationSequence;
                unsigned int rotationIndex=bestSolution.rotationIndex;
                for(int l=0;l<boxes.length();l++)
                {
                    unsigned int index=rotationIndex%localRotations;
                    rotationIndex=rotationIndex/localRotations;
                    rotationSequence.push_back(index);
                }
                QVector<int> boxSequence;
                unsigned int permutationIndex=bestSolution.permutationIndex;
                QVector<int> tPermutationIndices(permutationIndices);
                for(unsigned int k=0;k<boxes.length();k++)
                {
                    unsigned int indices=boxes.length()-k;
                    unsigned int index=permutationIndex%indices;
                    permutationIndex=permutationIndex/indices;
                    boxSequence.push_back(tPermutationIndices[index]);
                    tPermutationIndices.removeAt(index);
                }
                float w,h,l;
                QVector<BoxInfo> solution=decodeSolution(rotationSequence,boxSequence,boxes,bounds,w,h,l);

                log+="Объём: "+QString::number(bestSolution.volume)+"\n";
                log+="Плотность упаковки: " + QString::number(100.0*totalVolumeSum/bestSolution.volume) + "%\n";
                log+="Габариты: "+QString::number(w)+"x"+QString::number(h)+"x"+QString::number(l)+"\n";
                float totalTime=currentTime;
                log+="Затраченное время: "+QString::number(qMax(totalTime,0.1f))+" секунд\n";

                log+="Решение получено методом перебора.\n";
                log+="Критерий расположения блоков: " + criteriaNames[placementCriteria] + "\n";
                log+="Повороты: "+(this->rotateBoxes?"разрешены":"запрещены");
                return solution;
            }
        }
    }
    if(bestSolution.flag==false)
        throw QString("Не удалось найти допустимое решение за выделенное время.");

    QVector<int> rotationSequence;
    unsigned int rotationIndex=bestSolution.rotationIndex;
    for(int l=0;l<boxes.length();l++)
    {
        unsigned int index=rotationIndex%localRotations;
        rotationIndex=rotationIndex/localRotations;
        rotationSequence.push_back(index);
    }
    QVector<int> boxSequence;
    unsigned int permutationIndex=bestSolution.permutationIndex;
    QVector<int> tPermutationIndices(permutationIndices);
    for(unsigned int k=0;k<boxes.length();k++)
    {
        unsigned int indices=boxes.length()-k;
        unsigned int index=permutationIndex%indices;
        permutationIndex=permutationIndex/indices;
        boxSequence.push_back(tPermutationIndices[index]);
        tPermutationIndices.removeAt(index);
    }
    float w,h,l;
    QVector<BoxInfo> solution=decodeSolution(rotationSequence,boxSequence,boxes,bounds,w,h,l);

    log+="Объём: "+QString::number(bestSolution.volume)+".\n";
    log+="Плотность упаковки: " + QString::number(100.0*totalVolumeSum/bestSolution.volume) + "%\n";
    log+="Габариты: "+QString::number(w)+"x"+QString::number(h)+"x"+QString::number(l)+".\n";
    float totalTime=float(clock()-t)/CLOCKS_PER_SEC;
    log+="Затраченное время: "+QString::number(qMax(totalTime,0.1f))+" секунд\n";
    log+="Решение получено методом перебора.\n";
    log+="Критерий расположения блоков: " + criteriaNames[placementCriteria] + ".\n";
    log+="Повороты: "+(this->rotateBoxes?"разрешены":"запрещены");
    return solution;


}
