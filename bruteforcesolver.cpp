#include "bruteforcesolver.h"
#include <QStack>
#include <QDebug>
#include <time.h>


BruteForceSolver::BruteForceSolver():rotateBoxes(true),placeCriteria(BruteForceCriteriaType::VOLUMEMINGLOB),maxTime(30.f)
{
}
void BruteForceSolver::init(bool rotateBoxes,int placeCriteria,float maxTime)
{
    this->rotateBoxes=rotateBoxes;
    this->placeCriteria=placeCriteria;
    this->maxTime=maxTime;
}
QVector<BoxInfo> BruteForceSolver::decodeSolution(const QVector<int>& rotationSequence,const QVector<int>& boxSequence,const QVector<Box>& boxes,const Box& bounds,float&w,float &h,float &l)
{
    float containerWidth=0.0f;
    float containerHeight=0.0f;
    float containerLength=0.0f;

    QList<emsstruct> emsArray;
    emsArray.push_back(emsstruct{0.f,0.f,0.f,
                       bounds.w,bounds.h,bounds.l});
    QVector<BoxInfo> placements;
    for(int i=0;i<boxes.length();i++)
    {
        int boxIndex=boxSequence[i];
        BoxOrientation orientation=orientations[rotationSequence[boxIndex]];
        Box box=boxes[boxIndex].getOrientation(orientation);
        int bestEMS=-1;
        float bestFitValue=10e8;
        for(int j=0;j<emsArray.length();j++)
        {
            emsstruct ems=emsArray[j];
            float w=ems.maxx-ems.minx;
            float h=ems.maxy-ems.miny;
            float l=ems.maxz-ems.minz;
            if(box.width()>w||box.height()>h||box.length()>l)
                continue;
            float fitness;
            float maxx=ems.minx+box.width();
            float maxy=ems.miny+box.height();
            float maxz=ems.minz+box.length();

            float envelopeX=qMax(containerWidth,maxx);
            float envelopeY=qMax(containerHeight,maxy);
            float envelopeZ=qMax(containerLength,maxz);
            switch(placeCriteria)
            {
                case EUCLDISTMIN:
                    fitness=envelopeX*envelopeX+
                            envelopeY*envelopeY+
                            envelopeZ*envelopeZ;
                    break;
                case MANHDISTMIN:
                    fitness=envelopeX+
                            envelopeY+
                            envelopeZ;
                    break;
                case CHEBDISTMIN:
                    fitness=qMax(envelopeX,
                            qMax(envelopeY,
                            envelopeZ));
                    break;
                case VOLUMEMINGLOB:
                    fitness=envelopeX*
                            envelopeY*
                            envelopeZ;
                    break;
                case EUCLDISTMINLOC:
                    fitness=maxx*maxx+
                            maxy*maxy+
                            maxz*maxz;
                    break;
                case MANHDISTMINLOC:
                    fitness=maxx+
                            maxy+
                            maxz;
                    break;
                case CHEBDISTMINLOC:
                    fitness=qMax(maxx,
                            qMax(maxy,
                            maxz));
                    break;
            }
            if(bestFitValue>fitness)
            {
                bestFitValue=fitness;
                bestEMS=j;
            }
        }
        Q_ASSERT(bestEMS!=-1);
        emsstruct ems=emsArray[bestEMS];

        BoxInfo placement=BoxInfo{ems.minx+box.width()*0.5f,
                ems.miny+box.height()*0.5f,
                ems.minz+box.length()*0.5f,
                boxIndex,
                orientation};
        placements.push_back(placement);
        float minx=ems.minx;
        float maxx=minx+box.width();
        float miny=ems.miny;
        float maxy=miny+box.height();
        float minz=ems.minz;
        float maxz=minz+box.length();
        containerWidth=qMax(containerWidth,maxx);
        containerHeight=qMax(containerHeight,maxy);
        containerLength=qMax(containerLength,maxz);

        QStack<emsstruct> emptyspaces;
        for(int j=0;j<emsArray.length();j++)
        {
            emptyspaces.push(emsArray[j]);
        }
        emsArray.clear();
        while(emptyspaces.length()>0)
        {
            emsstruct t_ems=emptyspaces.pop();
            float t_minx=qMax(minx,t_ems.minx);
            float t_miny=qMax(miny,t_ems.miny);
            float t_minz=qMax(minz,t_ems.minz);

            float t_maxx=qMin(maxx,t_ems.maxx);
            float t_maxy=qMin(maxy,t_ems.maxy);
            float t_maxz=qMin(maxz,t_ems.maxz);

            if(t_minx<=t_maxx&&t_miny<=t_maxy&&t_minz<=t_maxz)
            {
                //part of the box is inside ems
                if(t_minx-t_ems.minx>0.01)
                {
                    emsArray.push_back(emsstruct{t_ems.minx,t_ems.miny,t_ems.minz,
                                                 t_minx,t_ems.maxy,t_ems.maxz});
                }
                if(t_ems.maxx-t_maxx>0.01)
                {
                    emsArray.push_back(emsstruct{t_maxx,t_ems.miny,t_ems.minz,
                                                 t_ems.maxx,t_ems.maxy,t_ems.maxz});
                }
                if(t_miny-t_ems.miny>0.01)
                {
                    emsArray.push_back(emsstruct{t_ems.minx,t_ems.miny,t_ems.minz,
                                                 t_ems.maxx,t_miny,t_ems.maxz});
                }
                if(t_ems.maxy-t_maxy>0.01)
                {
                    emsArray.push_back(emsstruct{t_ems.minx,t_maxy,t_ems.minz,
                                                 t_ems.maxx,t_ems.maxy,t_ems.maxz});
                }
                if(t_minz-t_ems.minz>0.01)
                {
                    emsArray.push_back(emsstruct{t_ems.minx,t_ems.miny,t_ems.minz,
                                                 t_ems.maxx,t_ems.maxy,t_minz});
                }
                if(t_ems.maxz-t_maxz>0.01)
                {
                    emsArray.push_back(emsstruct{t_ems.minx,t_ems.miny,t_maxz,
                                                 t_ems.maxx,t_ems.maxy,t_ems.maxz});
                }
            }else
            {
                emsArray.push_back(t_ems);
            }
        }
        //check for inscribed spaces
        for(int l=emsArray.length()-1;l>=0;l--)
        {
            emsstruct c1=emsArray[l];
            for(int k=0;k<l;k++)
            {
                emsstruct c2=emsArray[k];
                if(c1.minx>=c2.minx&&c1.miny>=c2.miny&&c1.minz>=c2.minz&&
                        c1.maxx<=c2.maxx&&c1.maxy<=c2.maxy&&c1.maxz<=c2.maxz)
                {
                    emsArray.removeAt(l);
                    break;
                }
                if(c2.minx>=c1.minx&&c2.miny>=c1.miny&&c2.minz>=c1.minz&&
                        c2.maxx<=c1.maxx&&c2.maxy<=c1.maxy&&c2.maxz<=c1.maxz)
                {
                    emsArray.removeAt(k);
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

    QList<emsstruct> emsArray;
    emsArray.push_back(emsstruct{0.f,0.f,0.f,
                       bounds.w,bounds.h,bounds.l});
    for(int i=0;i<boxes.length();i++)
    {
        int boxIndex=boxSequence[i];
        BoxOrientation orientation=orientations[rotationSequence[boxIndex]];
        Box box=boxes[boxIndex].getOrientation(orientation);
        int bestEMS=-1;
        float bestFitValue=10e8;
        for(int j=0;j<emsArray.length();j++)
        {
            emsstruct ems=emsArray[j];
            float w=ems.maxx-ems.minx;
            float h=ems.maxy-ems.miny;
            float l=ems.maxz-ems.minz;;
            if(box.width()>w||box.height()>h||box.length()>l)
                continue;
            float fitness;
            float maxx=ems.minx+box.width();
            float maxy=ems.miny+box.height();
            float maxz=ems.minz+box.length();

            float envelopeX=qMax(containerWidth,maxx);
            float envelopeY=qMax(containerHeight,maxy);
            float envelopeZ=qMax(containerLength,maxz);
            switch(placeCriteria)
            {
                case EUCLDISTMIN:
                    fitness=envelopeX*envelopeX+
                            envelopeY*envelopeY+
                            envelopeZ*envelopeZ;
                    break;
                case MANHDISTMIN:
                    fitness=envelopeX+
                            envelopeY+
                            envelopeZ;
                    break;
                case CHEBDISTMIN:
                    fitness=qMax(envelopeX,
                            qMax(envelopeY,
                            envelopeZ));
                    break;
                case VOLUMEMINGLOB:
                    fitness=envelopeX*
                            envelopeY*
                            envelopeZ;
                    break;
                case EUCLDISTMINLOC:
                    fitness=maxx*maxx+
                            maxy*maxy+
                            maxz*maxz;
                    break;
                case MANHDISTMINLOC:
                    fitness=maxx+
                            maxy+
                            maxz;
                    break;
                case CHEBDISTMINLOC:
                    fitness=qMax(maxx,
                            qMax(maxy,
                            maxz));
                    break;
            }
            if(bestFitValue>fitness)
            {
                bestFitValue=fitness;
                bestEMS=j;
            }
        }
        if(bestEMS==-1)
            return -1.0;
        emsstruct ems=emsArray[bestEMS];

        float minx=ems.minx;
        float maxx=minx+box.width();
        float miny=ems.miny;
        float maxy=miny+box.height();
        float minz=ems.minz;
        float maxz=minz+box.length();
        containerWidth=qMax(containerWidth,maxx);
        containerHeight=qMax(containerHeight,maxy);
        containerLength=qMax(containerLength,maxz);

        QStack<emsstruct> emptyspaces;
        for(int j=0;j<emsArray.length();j++)
        {
            emptyspaces.push(emsArray[j]);
        }
        emsArray.clear();
        while(emptyspaces.length()>0)
        {
            emsstruct t_ems=emptyspaces.pop();
            float t_minx=qMax(minx,t_ems.minx);
            float t_miny=qMax(miny,t_ems.miny);
            float t_minz=qMax(minz,t_ems.minz);

            float t_maxx=qMin(maxx,t_ems.maxx);
            float t_maxy=qMin(maxy,t_ems.maxy);
            float t_maxz=qMin(maxz,t_ems.maxz);

            if(t_minx<=t_maxx&&t_miny<=t_maxy&&t_minz<=t_maxz)
            {
                //part of the box is inside ems
                if(t_minx-t_ems.minx>0.01)
                {
                    emsArray.push_back(emsstruct{t_ems.minx,t_ems.miny,t_ems.minz,
                                                 t_minx,t_ems.maxy,t_ems.maxz});
                }
                if(t_ems.maxx-t_maxx>0.01)
                {
                    emsArray.push_back(emsstruct{t_maxx,t_ems.miny,t_ems.minz,
                                                 t_ems.maxx,t_ems.maxy,t_ems.maxz});
                }
                if(t_miny-t_ems.miny>0.01)
                {
                    emsArray.push_back(emsstruct{t_ems.minx,t_ems.miny,t_ems.minz,
                                                 t_ems.maxx,t_miny,t_ems.maxz});
                }
                if(t_ems.maxy-t_maxy>0.01)
                {
                    emsArray.push_back(emsstruct{t_ems.minx,t_maxy,t_ems.minz,
                                                 t_ems.maxx,t_ems.maxy,t_ems.maxz});
                }
                if(t_minz-t_ems.minz>0.01)
                {
                    emsArray.push_back(emsstruct{t_ems.minx,t_ems.miny,t_ems.minz,
                                                 t_ems.maxx,t_ems.maxy,t_minz});
                }
                if(t_ems.maxz-t_maxz>0.01)
                {
                    emsArray.push_back(emsstruct{t_ems.minx,t_ems.miny,t_maxz,
                                                 t_ems.maxx,t_ems.maxy,t_ems.maxz});
                }
            }else
            {
                emsArray.push_back(t_ems);
            }
        }
        //check for inscribed spaces
        for(int l=emsArray.length()-1;l>=0;l--)
        {
            emsstruct c1=emsArray[l];
            for(int k=0;k<l;k++)
            {
                emsstruct c2=emsArray[k];
                if(c1.minx>=c2.minx&&c1.miny>=c2.miny&&c1.minz>=c2.minz&&
                        c1.maxx<=c2.maxx&&c1.maxy<=c2.maxy&&c1.maxz<=c2.maxz)
                {
                    emsArray.removeAt(l);
                    break;
                }
                if(c2.minx>=c1.minx&&c2.miny>=c1.miny&&c2.minz>=c1.minz&&
                        c2.maxx<=c1.maxx&&c2.maxy<=c1.maxy&&c2.maxz<=c1.maxz)
                {
                    emsArray.removeAt(k);
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

                    log+="Объём: "+QString::number(bestSolution.volume)+".\n";
                    log+="Габариты: "+QString::number(w)+"x"+QString::number(h)+"x"+QString::number(l)+".\n";
                    float totalTime=currentTime;
                    log+="Затраченное время: "+QString::number(qMax(totalTime,0.1f))+" секунд";

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
        log+="Габариты: "+QString::number(w)+"x"+QString::number(h)+"x"+QString::number(l)+".\n";
        float totalTime=float(clock()-t)/CLOCKS_PER_SEC;
        log+="\nЗатраченное время: "+QString::number(qMax(totalTime,0.1f))+" секунд";
        return solution;


    }
    /*
    for each sequence box and rotation

    max number of variants:
    n!*6^n or n! without rotations
    criterias:
    closest point
    minimal volume
    3x minimal distance to maximal point*/

