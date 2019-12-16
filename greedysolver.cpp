#include "greedysolver.h"
#include <QStack>

#include <time.h>


GreedySolver::GreedySolver():rotateBoxes(false),greedCriteria(EUCLDISTMINGLOB)
{
}

void GreedySolver::init(bool rotateBoxes, int greedCriteria)
{
    this->rotateBoxes=rotateBoxes;
    this->greedCriteria=greedCriteria;
}
//Вынесено отдельно
QVector<BoxInfo> GreedySolver::solveMaxBoxMinPos(const QVector<Box> &boxes, const Box &bounds)
{
    clock_t t;
    t = clock();
    QList<int> availableBoxes;
    for(int i=0;i<boxes.length();i++)
    {
        availableBoxes.push_back(i);
    }
    for(int i=0;i<boxes.length();i++)
    {
        int minIndex=i;
        float maxVolume=-1.0;
        for(int j=i;j<boxes.length();j++)
        {
            Box box=boxes[availableBoxes[j]];
            float boxVolume=box.volume();
            if(boxVolume>maxVolume)
            {
                minIndex=j;
                maxVolume=boxVolume;
            }
        }
        qSwap(availableBoxes[i],availableBoxes[minIndex]);
    }
    QVector<BoxInfo> placements;
    QList<emsstruct> emsArray;
    emsArray.push_back(emsstruct{0.f,0.f,0.f,
                       bounds.w,bounds.h,bounds.l});
    float x=0.0f;
    float y=0.0f;
    float z=0.0f;

    int rotateBoxesIndex=rotateBoxes?6:1;

    while(availableBoxes.length()!=0)
    {
       emit BoxPackingSolver::progress(100-(availableBoxes.length()*100/boxes.length()));
       int bestContainer=-1;
       int bestEMS=-1;
       BoxOrientation bestOrientation=BoxOrientation::XYZ0;
       for(int j=0;j<availableBoxes.length();j++)
       {
           float minDist=10e8;
           bestContainer=-1;
           Box box=boxes[availableBoxes[j]];
           for(int i=0;i<emsArray.length();i++)
           {
               emsstruct ems=emsArray[i];
               float w=ems.maxx-ems.minx;
               float h=ems.maxy-ems.miny;
               float l=ems.maxz-ems.minz;
               if(box.volume()>w*h*l)
                   continue;
               for(int k=0;k<rotateBoxesIndex;k++)
               {
                   BoxOrientation o=orientations[k];
                   box=boxes[availableBoxes[j]].getOrientation(o);
                   if(box.width()>w||box.height()>h||box.length()>l)
                       continue;
                   float dist=ems.minx*ems.minx+ems.miny*ems.miny+
                           ems.minz*ems.minz;
                   if(dist<minDist)
                   {
                       minDist=dist;
                       bestEMS=i;
                       bestContainer=j;
                       bestOrientation=o;
                   }
               }
            }
           if(bestContainer!=-1)
           {
               break;
           }
       }
       if(bestContainer==-1)
       {
           //не удалось найти решение, удовлетворяющее ограничениям
           throw QString("Не удалось найти решение, удовлетворяющее ограничениям");
       }
       else
       {
           emsstruct ems=emsArray[bestEMS];
           Box& box=boxes[availableBoxes[bestContainer]].
                   getOrientation(bestOrientation);
           BoxInfo placement=BoxInfo{ems.minx+box.width()*0.5,
                   ems.miny+box.height()*0.5,
                   ems.minz+box.length()*0.5,
                   availableBoxes[bestContainer],
                   bestOrientation};
           placements.push_back(placement);
           availableBoxes.removeAt(bestContainer);

           float minx=ems.minx;
           float maxx=minx+box.width();
           float miny=ems.miny;
           float maxy=miny+box.height();
           float minz=ems.minz;
           float maxz=minz+box.length();
           x=qMax(x,maxx);
           y=qMax(y,maxy);
           z=qMax(z,maxz);

           //recompute all ems's
           QStack<emsstruct> emptyspaces;
           for(int i=0;i<emsArray.length();i++)
           {
               emptyspaces.push(emsArray[i]);
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
    }
    float volume=x*y*z;
    t = clock()-t;
    log+="Объём: "+QString::number(volume)+".\n";
    log+="Габариты: "+QString::number(x)+"x"+QString::number(y)+"x"+QString::number(z)+".\n";
    log+="Затраченное время: "+QString::number(qMax(((float)t)/CLOCKS_PER_SEC,0.1f))+" секунд";
    return placements;
}

QVector<BoxInfo> GreedySolver::solve(const QVector<Box> &boxes, const Box &bounds)
{
    //Отдельная функция для данного критерия жадности
    if(greedCriteria==MAXBOXMINPOS)
        return solveMaxBoxMinPos(boxes,bounds);
    clock_t t;
    t = clock();
    //empty maximal spaces
    QList<int> availableBoxes;
    for(int i=0;i<boxes.length();i++)
    {
        availableBoxes.push_back(i);
    }
    QVector<BoxInfo> placements;
    QList<emsstruct> emsArray;
    emsArray.push_back(emsstruct{0.f,0.f,0.f,
                       bounds.w,bounds.h,bounds.l});
    float x=0.0f;
    float y=0.0f;
    float z=0.0f;

    int rotateBoxesIndex=rotateBoxes?6:1;

    while(availableBoxes.length()!=0)
    {
       emit BoxPackingSolver::progress(100-(availableBoxes.length()*100/boxes.length()));
       int bestContainer=-1;
       int bestEMS=-1;
       BoxOrientation bestOrientation=BoxOrientation::XYZ0;
       float bestFitValue=10.0e8;
       for(int i=0;i<emsArray.length();i++)
       {
           emsstruct ems=emsArray[i];
           float w=ems.maxx-ems.minx;
           float h=ems.maxy-ems.miny;
           float l=ems.maxz-ems.minz;
           float emsVolume=w*h*l;
           for(int j=0;j<availableBoxes.length();j++)
           {
                //calc fittness
               //fitness=boxVolume/emsVolume; the bigger the better
               Box box=boxes[availableBoxes[j]];
               float boxVolume=box.volume();
               if(boxVolume>emsVolume)
                   continue;
               for(int k=0;k<rotateBoxesIndex;k++)
               {
                   BoxOrientation o=orientations[k];
                   Box box=boxes[availableBoxes[j]].getOrientation(o);
                   if(box.width()>w||box.height()>h||box.length()>l)
                       continue;
                   float fitness;

                   float maxx=ems.minx+box.width();
                   float maxy=ems.miny+box.height();
                   float maxz=ems.minz+box.length();

                   float envelopeX=qMax(x,maxx);
                   float envelopeY=qMax(y,maxy);
                   float envelopeZ=qMax(z,maxz);

                   switch(greedCriteria)
                   {
                       case EUCLDISTMINGLOB:
                         fitness=envelopeX*envelopeX+
                                 envelopeY*envelopeY+
                                 envelopeZ*envelopeZ;
                           break;
                       case MANHDISTMINGLOB:
                           fitness=envelopeX+
                                   envelopeY+
                                   envelopeZ;
                           break;
                       case CHEBDISTMINGLOB:
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
                        default:
                            Q_ASSERT(false);

                   }
                   if(fitness<=bestFitValue)
                   {
                       bestContainer=j;
                       bestEMS=i;
                       bestFitValue=fitness;
                       bestOrientation=o;
                   }
               }
           }
       }
       if(bestContainer==-1)
       {
           //не удалось найти решение, удовлетворяющее ограничениям
           throw QString("Не удалось найти решение, удовлетворяющее ограничениям");
       }
       else
       {
           emsstruct ems=emsArray[bestEMS];
           Box& box=boxes[availableBoxes[bestContainer]].
                   getOrientation(bestOrientation);
           BoxInfo placement=BoxInfo{ems.minx+box.width()*0.5,
                   ems.miny+box.height()*0.5,
                   ems.minz+box.length()*0.5,
                   availableBoxes[bestContainer],
                   bestOrientation};
           placements.push_back(placement);
           availableBoxes.removeAt(bestContainer);

           float minx=ems.minx;
           float maxx=minx+box.width();
           float miny=ems.miny;
           float maxy=miny+box.height();
           float minz=ems.minz;
           float maxz=minz+box.length();
           x=qMax(x,maxx);
           y=qMax(y,maxy);
           z=qMax(z,maxz);


           //recompute all ems's
           QStack<emsstruct> emptyspaces;
           for(int i=0;i<emsArray.length();i++)
           {
               emptyspaces.push(emsArray[i]);
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
    }
    float volume=x*y*z;
    t = clock()-t;
    log+="Объём: "+QString::number(volume)+".\n";
    log+="Габариты: "+QString::number(x)+"x"+QString::number(y)+"x"+QString::number(z)+".\n";
    log+="Затраченное время: "+QString::number(qMax(((float)t)/CLOCKS_PER_SEC,0.1f))+" секунд";
    return placements;
}

