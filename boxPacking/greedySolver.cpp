#include "greedySolver.h"
#include <QStack>
#include <time.h>


enum GreedyCriteriaType
{
    EUCL_NORM = 0,
    MANH_NORM= 1,
    CHEB_NORM = 2,
    VOLUME = 3,
    EUCL_NORM_LOC = 4,
    MANH_NORM_LOC = 5,
    CHEB_NORM_LOC = 6,
    MAX_BOX_MIN_POS = 7,
    TIGHT_SIDE = 8,
    LOOSE_SIDE = 9,
    SURFACE_AREA = 10,
    EMPTY_VOLUME = 11
};

QString GreedySolver::criteriaNames[] = {"L2 норма AABB",
                                         "L1 норма AABB",
                                         "L∞ норма AABB",
                                         "Объём AABB",
                                         "L2 норма текущего края",
                                         "L1 норма текущего края",
                                         "L∞ норма текущего края",
                                         "Макс. объём в мин. позиции",
                                         "Наиболее плотная сторона",
                                         "Наименее плотная сторона",
                                         "Площадь поверхности AABB",
                                         "Незаполненный объём"};

GreedySolver::GreedySolver():rotateBoxes(false),greedCriteria(EUCL_NORM)
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
    QList<AABB> aabbArray;
    aabbArray.push_back(AABB{0.f,0.f,0.f,
                       bounds.w,bounds.h,bounds.l});
    float x=0.0f;
    float y=0.0f;
    float z=0.0f;

    int rotateBoxesIndex=rotateBoxes?6:1;

    while(availableBoxes.length()!=0)
    {
       emit BoxPackingSolver::progress(100-(availableBoxes.length()*100/boxes.length()));
       int bestContainer=-1;
       int bestAABB=-1;
       BoxOrientation bestOrientation=BoxOrientation::XYZ0;
       for(int j=0;j<availableBoxes.length();j++)
       {
           float minDist=10e8;
           bestContainer=-1;
           Box box=boxes[availableBoxes[j]];
           for(int i=0;i<aabbArray.length();i++)
           {
               AABB aabb=aabbArray[i];
               float w=aabb.maxx-aabb.minx;
               float h=aabb.maxy-aabb.miny;
               float l=aabb.maxz-aabb.minz;
               if(box.volume()>w*h*l)
                   continue;
               for(int k=0;k<rotateBoxesIndex;k++)
               {
                   BoxOrientation o=orientations[k];
                   box=boxes[availableBoxes[j]].getOrientation(o);
                   if(box.width()>w||box.height()>h||box.length()>l)
                       continue;
                   float dist=aabb.minx*aabb.minx+aabb.miny*aabb.miny+
                           aabb.minz*aabb.minz;
                   if(dist<minDist)
                   {
                       minDist=dist;
                       bestAABB=i;
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
           AABB aabb=aabbArray[bestAABB];
           Box& box=boxes[availableBoxes[bestContainer]].
                   getOrientation(bestOrientation);
           BoxInfo placement=BoxInfo{aabb.minx+box.width()*0.5,
                   aabb.miny+box.height()*0.5,
                   aabb.minz+box.length()*0.5,
                   availableBoxes[bestContainer],
                   bestOrientation};
           placements.push_back(placement);
           availableBoxes.removeAt(bestContainer);

           float minx=aabb.minx;
           float maxx=minx+box.width();
           float miny=aabb.miny;
           float maxy=miny+box.height();
           float minz=aabb.minz;
           float maxz=minz+box.length();
           x=qMax(x,maxx);
           y=qMax(y,maxy);
           z=qMax(z,maxz);

           //recompute all aabb's
           QStack<AABB> emptyspaces;
           for(int i=0;i<aabbArray.length();i++)
           {
               emptyspaces.push(aabbArray[i]);
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
    if(greedCriteria==MAX_BOX_MIN_POS)
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
    QList<AABB> aabbArray;
    aabbArray.push_back(AABB{0.f,0.f,0.f,
                       bounds.w,bounds.h,bounds.l});
    float x=0.0f;
    float y=0.0f;
    float z=0.0f;

    int rotateBoxesIndex=rotateBoxes?6:1;
    float totalVolumeSum = 0.0;
    while(availableBoxes.length()!=0)
    {
       emit BoxPackingSolver::progress(100-(availableBoxes.length()*100/boxes.length()));
       int bestContainer=-1;
       int bestAABB=-1;
       BoxOrientation bestOrientation=BoxOrientation::XYZ0;
       float bestFitValue=10.0e8;

       float bestDistance=10.0e8;

       for(int i=0;i<aabbArray.length();i++)
       {
           AABB aabb=aabbArray[i];
           float w=aabb.maxx-aabb.minx;
           float h=aabb.maxy-aabb.miny;
           float l=aabb.maxz-aabb.minz;
           float aabbVolume=w*h*l;
           for(int j=0;j<availableBoxes.length();j++)
           {
                //calc fittness
               //fitness=boxVolume/aabbVolume; the bigger the better
               Box box=boxes[availableBoxes[j]];
               float boxVolume=box.volume();
               if(boxVolume>aabbVolume)
                   continue;
               for(int k=0;k<rotateBoxesIndex;k++)
               {
                   BoxOrientation o=orientations[k];
                   Box box=boxes[availableBoxes[j]].getOrientation(o);
                   if(box.width()>w||box.height()>h||box.length()>l)
                       continue;
                   float fitness;


                   float maxx=aabb.minx+box.width();
                   float maxy=aabb.miny+box.height();
                   float maxz=aabb.minz+box.length();

                   float envelopeX=qMax(x,maxx);
                   float envelopeY=qMax(y,maxy);
                   float envelopeZ=qMax(z,maxz);
                   float distance=10e8;
                   if(bounds.w<envelopeX||bounds.h<envelopeY||bounds.l<envelopeZ)
                       continue;

                   switch(greedCriteria)
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
                        case TIGHT_SIDE:
                            fitness=qMin(aabb.maxx-maxx,
                                        qMin(aabb.maxy-maxy,
                                        aabb.maxz-maxz));
                            break;
                        case LOOSE_SIDE:
                            fitness=qMax(aabb.maxx-maxx,
                                       qMax(aabb.maxy-maxy,
                                       aabb.maxz-maxz));
                            break;
                        case SURFACE_AREA:
                            fitness = envelopeX*envelopeY + envelopeY*envelopeZ + envelopeZ*envelopeX;
                            break;
                        case EMPTY_VOLUME:
                            fitness = envelopeX*envelopeY*envelopeZ - totalVolumeSum - box.volume();
                            break;
                        default:
                            Q_ASSERT(false);

                   }
                   if(bestFitValue-fitness<0.001&&bestFitValue-fitness>=0&&distance<bestDistance)
                   {
                       bestContainer=j;
                       bestAABB=i;
                       bestFitValue=fitness;
                       bestDistance=distance;
                       bestOrientation=o;
                   }
                   else if(fitness<bestFitValue)
                   {
                       bestContainer=j;
                       bestAABB=i;
                       bestFitValue=fitness;
                       bestDistance=distance;
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
           AABB aabb=aabbArray[bestAABB];
           Box& box=boxes[availableBoxes[bestContainer]].
                   getOrientation(bestOrientation);
           totalVolumeSum+=box.volume();
           BoxInfo placement=BoxInfo{aabb.minx+box.width()*0.5,
                   aabb.miny+box.height()*0.5,
                   aabb.minz+box.length()*0.5,
                   availableBoxes[bestContainer],
                   bestOrientation};
           placements.push_back(placement);
           availableBoxes.removeAt(bestContainer);

           float minx=aabb.minx;
           float maxx=minx+box.width();
           float miny=aabb.miny;
           float maxy=miny+box.height();
           float minz=aabb.minz;
           float maxz=minz+box.length();
           x=qMax(x,maxx);
           y=qMax(y,maxy);
           z=qMax(z,maxz);


           //recompute all aabb's
           QStack<AABB> emptyspaces;
           for(int i=0;i<aabbArray.length();i++)
           {
               emptyspaces.push(aabbArray[i]);
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

               if(t_minx<t_maxx&&t_miny<t_maxy&&t_minz<t_maxz)
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
           for(int l=aabbArray.length()-1;l>0;l--)
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
    }
    float volume=x*y*z;
    t = clock()-t;
    log+="Объём: "+QString::number(volume)+".\n";
    log+="Плотность упаковки: " + QString::number(100.0*totalVolumeSum/volume) + "%\n";
    log+="Габариты: "+QString::number(x)+"x"+QString::number(y)+"x"+QString::number(z)+".\n";
    log+="Затраченное время: "+QString::number(qMax(((float)t)/CLOCKS_PER_SEC,0.1f))+" секунд\n";
    log+="Решение получено жадным алгоримом\n";
    log+="Критерий жадности: " + criteriaNames[greedCriteria] + "\n";
    log+="Повороты: "+(this->rotateBoxes?"разрешены":"запрещены");
    return placements;
}
