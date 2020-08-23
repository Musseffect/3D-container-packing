#include "solvers.h"
#include "QThread"


#ifdef SOLVERSOLD

QVector<BoxInfo> solveGreedy(QVector<Box>& boxes, Box& bounds, int criteria,bool rotateBoxes, QString &log)
{
    //empty maximal spaces
    QList<int> availableBoxes;
    for(int i=0;i<boxes.length();i++)
    {
        availableBoxes.push_back(i);
    }

    if(criteria==MAXBOXMINPOS)
    {
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
       int bestContainer=-1;
       int bestEMS=-1;
       BoxOrientation bestOrientation=BoxOrientation::XYZ0;
       if(criteria==MAXBOXMINPOS)
       {
           for(int j=0;j<availableBoxes.length();j++)
           {
               Box box=boxes[availableBoxes[j]];
               float minDist=10e8;
               bestContainer=-1;
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
                       box.setOrientation(o);
                       if(box.getW()>w||box.getH()>h||box.getL()>l)
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
           }
           if(bestContainer!=-1)
           {
               break;
           }
       }else
       {
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
                       box.setOrientation(o);
                       if(box.getW()>w||box.getH()>h||box.getL()>l)
                           continue;
                       float fitness;

                       float maxx=ems.minx+box.getW();
                       float maxy=ems.miny+box.getH();
                       float maxz=ems.minz+box.getL();

                       float envelopeX=qMax(x,maxx);
                       float envelopeY=qMax(y,maxy);
                       float envelopeZ=qMax(z,maxz);

                       switch(criteria)
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
                           fitness=envelopeX+
                                   envelopeY+
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
       }
       if(bestContainer==-1)
       {
           //не удалось найти решение, удовлетворяющее ограничениям
           throw QString("Не удалось найти решение, удовлетворяющее ограничениям");
       }
       else
       {
           emsstruct ems=emsArray[bestEMS];
           Box& box=boxes[availableBoxes[bestContainer]];
           box.setOrientation(bestOrientation);
           BoxInfo placement=BoxInfo{ems.minx+box.getW()*0.5,
                   ems.miny+box.getH()*0.5,
                   ems.minz+box.getL()*0.5,
                   availableBoxes[bestContainer]};
           placements.push_back(placement);
           availableBoxes.removeAt(bestContainer);

           float minx=ems.minx;
           float maxx=minx+box.getW();
           float miny=ems.miny;
           float maxy=miny+box.getH();
           float minz=ems.minz;
           float maxz=minz+box.getL();
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
    log+="Объём: "+QString::number(volume)+".\n";
    return placements;
}

Chromosome generate(int boxCount)
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
    return solution;
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

float objectiveFunction(const Chromosome& chromosome,const QVector<Box>& boxes,const Box& bound)
{
    typedef BoxInfo boxplacement;
    struct placementEms{float maxx;float maxy;float maxz;QList<boxplacement>* bpList;QList<emsstruct>* emsList;};

    QStack<placementEms> operands;
    for(QVector<int>::const_iterator iter=chromosome.genes.constBegin();iter!=chromosome.genes.constEnd();++iter)
    {
        int currentGene=*iter;
        if(currentGene<0)
        {
            if(operands.length()<2)
                throw QString("Ошибка в записи ОПН");
            placementEms a=operands.pop();//left operand
            placementEms b=operands.pop();//right operand
            delete b.emsList;
            placementEms result;
            QList<emsstruct>* emsList=a.emsList;
            switch(currentGene)
            {
                case -1:
                    {//sort b boxes based on a x coordinate
                    //for each box in b
                    /*
                      for each ems find ems with minimal x coordinate and place current box
                      for each ems recompute it with new box

                    */
                    //sort in ascending order
                    qSort(b.bpList->begin(),b.bpList->end(),xCoordLessThan);
                    float maxxResult=a.maxx;
                    for(QList<boxplacement>::iterator bpit=b.bpList->begin();bpit!=b.bpList->end();++bpit)
                    {
                        int emsIndex=-1;
                        bpit->x+=a.maxx;
                        const Box& box=boxes.at(bpit->boxID);
                        float minx=bpit->x;
                        float miny=bpit->y;
                        float minz=bpit->z;
                        float maxx=bpit->x+box.w;
                        float maxy=bpit->y+box.h;
                        float maxz=bpit->z+box.l;
                        for(int j=0;j<emsList->length();j++)
                        {
                            const emsstruct &ems=emsList->at(j);
                            if(ems.minx<=minx&&ems.maxx>=maxx&&
                                    ems.miny<=miny&&ems.maxy>=maxy&&
                                    ems.minz<=minz&&ems.maxz>=maxz)
                            {
                                emsIndex=j;
                                minx=ems.minx;
                            }
                        }
                        bpit->x=minx;
                        maxx=minx+box.w;
                        maxxResult=qMax(maxx,maxxResult);
                        //recompute ems
                        recompute(emsList,minx,maxx,miny,maxy,minz,maxz);
                        a.bpList->append(*bpit);
                    }
                    reduceEMS(emsList);
                    //x
                    result.maxx=maxxResult;
                    result.maxy=qMax(a.maxy,b.maxy);
                    result.maxz=qMax(a.maxz,b.maxz);
                    break;
                    }
                case -2:
            {
                    //sort in ascending order
                    qSort(b.bpList->begin(),b.bpList->end(),yCoordLessThan);
                    float maxyResult=a.maxy;
                    for(QList<boxplacement>::iterator bpit=b.bpList->begin();bpit!=b.bpList->end();++bpit)
                    {
                        int emsIndex=-1;
                        bpit->y+=a.maxy;
                        const Box& box=boxes.at(bpit->boxID);
                        float minx=bpit->x;
                        float miny=bpit->y;
                        float minz=bpit->z;
                        float maxx=bpit->x+box.w;
                        float maxy=bpit->y+box.h;
                        float maxz=bpit->z+box.l;
                        for(int j=0;j<emsList->length();j++)
                        {
                            const emsstruct &ems=emsList->at(j);
                            if(ems.minx<=minx&&ems.maxx>=maxx&&
                                    ems.miny<=miny&&ems.maxy>=maxy&&
                                    ems.minz<=minz&&ems.maxz>=maxz)
                            {
                                emsIndex=j;
                                miny=ems.miny;
                            }
                        }
                        bpit->y=miny;
                        maxy=miny+box.h;
                        maxyResult=qMax(maxy,maxyResult);
                        //recompute ems
                        recompute(emsList,minx,maxx,miny,maxy,minz,maxz);
                        a.bpList->append(*bpit);
                    }
                    reduceEMS(emsList);
                    //y
                    result.maxx=qMax(a.maxx,b.maxx);
                    result.maxy=maxyResult;
                    result.maxz=qMax(a.maxz,b.maxz);
                    break;
            }
                case -3:
            {
                    //sort in ascending order
                    qSort(b.bpList->begin(),b.bpList->end(),zCoordLessThan);
                    float maxzResult=a.maxz;
                    for(QList<boxplacement>::iterator bpit=b.bpList->begin();bpit!=b.bpList->end();++bpit)
                    {
                        int emsIndex=-1;
                        bpit->z+=a.maxz;
                        const Box& box=boxes.at(bpit->boxID);
                        float minx=bpit->x;
                        float miny=bpit->y;
                        float minz=bpit->z;
                        float maxx=bpit->x+box.w;
                        float maxy=bpit->y+box.h;
                        float maxz=bpit->z+box.l;
                        for(int j=0;j<emsList->length();j++)
                        {
                            const emsstruct &ems=emsList->at(j);
                            if(ems.minx<=minx&&ems.maxx>=maxx&&
                                    ems.miny<=miny&&ems.maxy>=maxy&&
                                    ems.minz<=minz&&ems.maxz>=maxz)
                            {
                                emsIndex=j;
                                minz=ems.minz;
                            }
                        }
                        bpit->z=minz;
                        maxz=minz+box.l;
                        maxzResult=qMax(maxz,maxzResult);
                        //recompute ems
                        recompute(emsList,minx,maxx,miny,maxy,minz,maxz);
                        a.bpList->append(*bpit);
                    }
                    reduceEMS(emsList);
                    delete b.bpList;
                    //z
                    result.maxx=qMax(a.maxx,b.maxx);
                    result.maxy=qMax(a.maxy,b.maxy);
                    result.maxz=maxzResult;
                    break;
            }
            }
            result.bpList=a.bpList;
            result.emsList=a.emsList;
            operands.push(result);
        }else
        {
            Box box=boxes[currentGene];
            QList<boxplacement>* bpList=new QList<boxplacement>();
            bpList->append(boxplacement{0.f,0.f,0.f,currentGene});
            QList<emsstruct>* emsList=new QList<emsstruct>();
            emsList->append(emsstruct{box.w,0.f,0.f,10e8,10e8,10e8});
            emsList->append(emsstruct{0.f,box.h,0.f,10e8,10e8,10e8});
            emsList->append(emsstruct{0.f,0.f,box.l,10e8,10e8,10e8});
            operands.push(placementEms{box.w,box.h,box.l,bpList,emsList});
        }
    }
    if(operands.length()!=1)
    {
        for(int i=0;i<operands.length();i++)
        {
            delete operands[i].bpList;
            delete operands[i].emsList;
        }
        throw QString("Ошибка в записи ОПН");
    }
    placementEms last=operands.pop();
    delete last.bpList;
    delete last.emsList;
    float volume=last.maxx*last.maxy*last.maxz;
    if(last.maxx>bound.w||last.maxy>bound.h||last.maxz>bound.l)
    {
        return -volume;
    }
    //static_assert(false,"Not implemented");
    return volume;
}
QVector<BoxInfo> calculatePlacements(const Chromosome& chromosome,const QVector<Box>& boxes)
{    typedef BoxInfo boxplacement;
     struct placementEms{float maxx;float maxy;float maxz;QList<boxplacement>* bpList;QList<emsstruct>* emsList;};


     QStack<placementEms> operands;
     for(QVector<int>::const_iterator iter=chromosome.genes.constBegin();iter!=chromosome.genes.constEnd();++iter)
     {
         int currentGene=*iter;
         if(currentGene<0)
         {
             if(operands.length()<2)
                 throw QString("Ошибка в записи ОПН");
             placementEms a=operands.pop();//left operand
             placementEms b=operands.pop();//right operand
             delete b.emsList;
             placementEms result;
             QList<emsstruct>* emsList=a.emsList;
             switch(currentGene)
             {
                 case -1:
                     {//sort b boxes based on a x coordinate
                     //for each box in b
                     /*
                       for each ems find ems with minimal x coordinate and place current box
                       for each ems recompute it with new box

                     */
                     //sort in ascending order
                     qSort(b.bpList->begin(),b.bpList->end(),xCoordLessThan);
                     float maxxResult=a.maxx;
                     for(QList<boxplacement>::iterator bpit=b.bpList->begin();bpit!=b.bpList->end();++bpit)
                     {
                         int emsIndex=-1;
                         bpit->x+=a.maxx;
                         const Box& box=boxes.at(bpit->boxID);
                         float minx=bpit->x;
                         float miny=bpit->y;
                         float minz=bpit->z;
                         float maxx=bpit->x+box.w;
                         float maxy=bpit->y+box.h;
                         float maxz=bpit->z+box.l;
                         for(int j=0;j<emsList->length();j++)
                         {
                             const emsstruct &ems=emsList->at(j);
                             if(ems.minx<=minx&&ems.maxx>=maxx&&
                                     ems.miny<=miny&&ems.maxy>=maxy&&
                                     ems.minz<=minz&&ems.maxz>=maxz)
                             {
                                 emsIndex=j;
                                 minx=ems.minx;
                             }
                         }
                         bpit->x=minx;
                         maxx=minx+box.w;
                         maxxResult=qMax(maxx,maxxResult);
                         //recompute ems
                         recompute(emsList,minx,maxx,miny,maxy,minz,maxz);
                         a.bpList->append(*bpit);
                     }
                     reduceEMS(emsList);
                     //x
                     result.maxx=maxxResult;
                     result.maxy=qMax(a.maxy,b.maxy);
                     result.maxz=qMax(a.maxz,b.maxz);
                     break;
                     }
                 case -2:
             {
                     //sort in ascending order
                     qSort(b.bpList->begin(),b.bpList->end(),yCoordLessThan);
                     float maxyResult=a.maxy;
                     for(QList<boxplacement>::iterator bpit=b.bpList->begin();bpit!=b.bpList->end();++bpit)
                     {
                         int emsIndex=-1;
                         bpit->y+=a.maxy;
                         const Box& box=boxes.at(bpit->boxID);
                         float minx=bpit->x;
                         float miny=bpit->y;
                         float minz=bpit->z;
                         float maxx=bpit->x+box.w;
                         float maxy=bpit->y+box.h;
                         float maxz=bpit->z+box.l;
                         for(int j=0;j<emsList->length();j++)
                         {
                             const emsstruct &ems=emsList->at(j);
                             if(ems.minx<=minx&&ems.maxx>=maxx&&
                                     ems.miny<=miny&&ems.maxy>=maxy&&
                                     ems.minz<=minz&&ems.maxz>=maxz)
                             {
                                 emsIndex=j;
                                 miny=ems.miny;
                             }
                         }
                         bpit->y=miny;
                         maxy=miny+box.h;
                         maxyResult=qMax(maxy,maxyResult);
                         //recompute ems
                         recompute(emsList,minx,maxx,miny,maxy,minz,maxz);
                         a.bpList->append(*bpit);
                     }
                     reduceEMS(emsList);
                     //y
                     result.maxx=qMax(a.maxx,b.maxx);
                     result.maxy=maxyResult;
                     result.maxz=qMax(a.maxz,b.maxz);
                     break;
             }
                 case -3:
             {
                     //sort in ascending order
                     qSort(b.bpList->begin(),b.bpList->end(),zCoordLessThan);
                     float maxzResult=a.maxz;
                     for(QList<boxplacement>::iterator bpit=b.bpList->begin();bpit!=b.bpList->end();++bpit)
                     {
                         int emsIndex=-1;
                         bpit->z+=a.maxz;
                         const Box& box=boxes.at(bpit->boxID);
                         float minx=bpit->x;
                         float miny=bpit->y;
                         float minz=bpit->z;
                         float maxx=bpit->x+box.w;
                         float maxy=bpit->y+box.h;
                         float maxz=bpit->z+box.l;
                         for(int j=0;j<emsList->length();j++)
                         {
                             const emsstruct &ems=emsList->at(j);
                             if(ems.minx<=minx&&ems.maxx>=maxx&&
                                     ems.miny<=miny&&ems.maxy>=maxy&&
                                     ems.minz<=minz&&ems.maxz>=maxz)
                             {
                                 emsIndex=j;
                                 minz=ems.minz;
                             }
                         }
                         bpit->z=minz;
                         maxz=minz+box.l;
                         maxzResult=qMax(maxz,maxzResult);
                         //recompute ems
                         recompute(emsList,minx,maxx,miny,maxy,minz,maxz);
                         a.bpList->append(*bpit);
                     }
                     reduceEMS(emsList);
                     delete b.bpList;
                     //z
                     result.maxx=qMax(a.maxx,b.maxx);
                     result.maxy=qMax(a.maxy,b.maxy);
                     result.maxz=maxzResult;
                     break;
             }
             }
             result.bpList=a.bpList;
             result.emsList=a.emsList;
             operands.push(result);
         }else
         {
             Box box=boxes[currentGene];
             QList<boxplacement>* bpList=new QList<boxplacement>();
             bpList->append(boxplacement{0.f,0.f,0.f,currentGene});
             QList<emsstruct>* emsList=new QList<emsstruct>();
             emsList->append(emsstruct{box.w,0.f,0.f,10e8,10e8,10e8});
             emsList->append(emsstruct{0.f,box.h,0.f,10e8,10e8,10e8});
             emsList->append(emsstruct{0.f,0.f,box.l,10e8,10e8,10e8});
             operands.push(placementEms{box.w,box.h,box.l,bpList,emsList});
         }
     }
     if(operands.length()!=1)
     {
         for(int i=0;i<operands.length();i++)
         {
             delete operands[i].bpList;
             delete operands[i].emsList;
         }
         throw QString("Ошибка в записи ОПН");
     }
     placementEms last=operands.pop();
     QVector<BoxInfo> placements=QVector<BoxInfo>::fromList(*last.bpList);
     for(QVector<BoxInfo>::iterator iter=placements.begin();iter!=placements.end();++iter)
     {
         const Box& box=boxes[iter->boxID];
         iter->x+=box.w*0.5f;
         iter->y+=box.h*0.5f;
         iter->z+=box.l*0.5f;
     }
     delete last.bpList;
     delete last.emsList;
     return placements;
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
                  const int population,
                  const int chromosomeLength)
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
        child1.genes.resize(chromosomeLength);
        Chromosome child2;
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
                }
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
                }
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
        objectiveValues.push_back(objectiveFunction(child1,boxes,bounds));
        objectiveValues.push_back(objectiveFunction(child2,boxes,bounds));
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
                chekcList[*iterGene]=true;
        }
        for(auto iterCheckList=checkList.begin();iterCheckList!=checkList.end();++iterCheckList)
        {
            if(*iterCheckList==false)
            {
                qDebug<<"Invalid solution";
            }
        }
    }
}


QVector<BoxInfo> solveGenetic(const QVector<Box> &boxes, Box bounds, int population,
                              int maxIterations, float maxTime, float requiredVolume,
                              float mutationProb,
                              int selectionCount,
                              int crossingoverCount,
                              int repairAttempts,
                              bool rotateBoxes,
                              bool compressBoxes,
                              QString& log)
{

    crossingoverCount=qMin(crossingoverCount,population-1);

    int boxCount=boxes.length();//количество ящиков
    //int operatorCount=boxCount-1;//количество операторов в записи обратной польской нотации
    int chromosomeLength=boxCount*2-1;//количество элементов в записи обратной польской нотации
    QElapsedTimer timer;

    QList<Chromosome> solutions;

    int currentIteration=0;
    float bestVolume=10e8;
    Chromosome bestSolution;

    float boundsVolume=bounds.w*bounds.l*bounds.h;//Максимальный допустимый объём
    timer.start();
    QList<float> objectiveValues;

    //Вспомогательные массивы
    QList<int> crossingoverIndexes;
    for(int i=4,j=0;i<chromosomeLength;i+=2,j++)
        crossingoverIndexes.push_back(i);

    QVector<bool> genePool;
    genePool.fill(true);


    float boundsTotal=bounds.w+bounds.h+bounds.l;
    float wProb=bounds.w/boundsTotal;
    float hProb=wProb+bounds.l/boundsTotal;
    //float lProb=hProb+bounds.h/boundsTotal;

    float totalTime=0.0f;
    while(true)
    {
        int reductionIndex=solutions.length();
        do{
            for(int i=solutions.length();i<population;i++)
            {
                Chromosome c=generate(boxCount);
                solutions.push_back(c);
                float value=objectiveFunction(c,boxes,bounds);
                objectiveValues.push_back(value);
                if(value>=0.0&&value<bestVolume)
                {
                    bestVolume=value;
                    bestSolution=c;
                }
            }
            for(int i=solutions.length()-1;i>=reductionIndex;i--)
            {
                Chromosome& c = solutions[i];
                if(objectiveValues[i]<0.0)
                {
                    float objectiveValue=objectiveValues[i];
                    //Попробовать исправить
                    for(int j=0;j<repairAttempts;j++)
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
                            }
                        }
                        objectiveValue=objectiveFunction(c,boxes,bounds);
                        if(objectiveValue>=0.0)
                        {
                            objectiveValues[i]=objectiveValue;
                            if(objectiveValue<bestVolume)
                            {
                                bestVolume=objectiveValue;
                                bestSolution=solutions[i];
                            }
                            break;
                        }
                    }
                    if(objectiveValue<0.0)
                    {
                        solutions.removeAt(i);
                        objectiveValues.removeAt(i);
                    }


                    /*//Попробовать исправить
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
                        }
                    }
                    float objectiveValue=objectiveFunction(c,boxes,bounds);
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
                    objectiveValues.removeAt(i);*/
                }
            }
            float currentTime=timer.nsecsElapsed()/10e9;
            if(currentTime>maxTime)
            {
                if(bestVolume<10e8)
                {
                    log+="Объём: "+QString::number(bestVolume)+".\n";
                    log+="Решение: ";
                    for(auto iter=bestSolution.genes.begin();iter!=bestSolution.genes.end();++iter)
                    {
                        log+=QString::number(*iter)+" ";
                    }
                    log+="\nКоличество итераций: "+currentIteration;
                    log+="\nЗатраченное время: "+QString::number(currentTime)+" секунд";
                    return calculatePlacements(bestSolution,boxes);
                }
                throw QString("Не удалось найти подходящее решение");
                break;
            }
        }while(solutions.length()<population);

        //crossingover
        for(int i=0;i<crossingoverCount;i++)
        {
            crossingover(solutions,objectiveValues,crossingoverIndexes,population);

#ifdef OLD
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
                child1.genes.resize(chromosomeLength);
                Chromosome child2;
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
                        }
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
                        }
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
                objectiveValues.push_back(objectiveFunction(child1,boxes,bounds));
                objectiveValues.push_back(objectiveFunction(child2,boxes,bounds));
            }
#endif
        }
        for(int i=0;i<solutions.length();i++)
        {
            if(Random::randomUnit()<=mutationProb)
            {
                Chromosome& c=solutions[i];
                //mutation
                mutation(c);
                //inversion
                inversion(c);
                objectiveValues[i]=objectiveFunction(c,boxes,bounds);
            }
        }

        currentIteration++;

        float currentTime=timer.nsecsElapsed()/10e9;
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
                //TODO: reduction
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
                    }
                }
                float objectiveValue=objectiveFunction(c,boxes,bounds);
                if(objectiveValue>=0.0)
                {
                    if(objectiveValue<bestVolume)
                    {
                        bestSolution=solutions[i];
                        bestVolume=objectiveValue;
                    }
                    objectiveValues[i]=objectiveValue;
                    continue;
                }
                solutions.removeAt(i);
                objectiveValues.removeAt(i);
            }
        }
        if(solutions.length()==0)
        {
            //Необходимо полное обновление популяции
            continue;
        }
        //selection
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

        //блок эволюционной адаптации
        QThread::msleep(10);
    }
    if(bestVolume<10e8)
    {
        log+="Объём: "+QString::number(bestVolume)+".\n";
        log+="Решение: ";
        for(auto iter=bestSolution.genes.begin();iter!=bestSolution.genes.end();++iter)
        {
            log+=QString::number(*iter)+" ";
        }
        log+="\nКоличество итераций: "+currentIteration;
        log+="\nЗатраченное время: "+QString::number(totalTime)+" секунд";
        return calculatePlacements(bestSolution,boxes);
    }
    throw QString("Не удалось найти подходящее решение");
}

Chromosome::Chromosome():genes()
{

}

Chromosome::Chromosome(const Chromosome &b)
{
    this->genes=b.genes;
}

Chromosome Chromosome::operator=(const Chromosome &b)
{
    this->genes=b.genes;
    return *this;
}



#ifdef OLDOBJECTIVE

float objectiveFunction(const Chromosome& chromosome,const QList<Box>& boxes,const Box& bound)
{
    struct placementEms{float maxx;float maxy;float maxz;};

    QStack<placementEms> operands;
    for(QVector<int>::const_iterator iter=chromosome.genes.constBegin();iter!=chromosome.genes.constEnd();++iter)
    {
        int currentGene=*iter;
        if(currentGene<0)
        {
            if(operands.length()<2)
                throw QString("Ошибка в записи ОПН");
            placementEms a=operands.pop();
            placementEms b=operands.pop();
            placementEms result;
            switch(currentGene)
            {
                case -1:
                    //x
                    result.maxx=a.maxx+b.maxx;
                    result.maxy=qMax(a.maxy,b.maxy);
                    result.maxz=qMax(a.maxz,b.maxz);
                    break;
                case -2:
                    //y
                    result.maxx=qMax(a.maxx,b.maxx);
                    result.maxy=a.maxy+b.maxy;
                    result.maxz=qMax(a.maxz,b.maxz);
                    break;
                case -3:
                    //z
                    result.maxx=qMax(a.maxx,b.maxx);
                    result.maxy=qMax(a.maxy,b.maxy);
                    result.maxz=a.maxz+b.maxz;
                    break;
            }
            operands.push(result);
        }else
        {
            Box box=boxes[currentGene];
            operands.push(placementEms{box.w,box.h,box.l});
        }
    }
    if(operands.length()!=1)
        throw QString("Ошибка в записи ОПН");
    placementEms last=operands.pop();
    float volume=last.maxx*last.maxy*last.maxz;
    if(last.maxx>bound.w||last.maxy>bound.h||last.maxz>bound.l)
    {
        return -volume;
    }
    return volume;
}

QVector<BoxInfo> calculatePlacements(const Chromosome& chromosome,const QList<Box>& boxes)
{
    struct placementTreeNode{float maxx;float maxy;float maxz;int left;int right;int operation;};
    QVector<BoxInfo> placements;
    QStack<placementTreeNode> operands;
    QVector<placementTreeNode> nodes;
    for(QVector<int>::const_iterator iter=chromosome.genes.constBegin();iter!=chromosome.genes.constEnd();iter++)
    {
        int currentGene=*iter;
        if(currentGene<0)
        {
            if(operands.length()<2)
                throw QString("Ошибка в записи ОПН");
            placementTreeNode a=operands.pop();
            placementTreeNode b=operands.pop();
            placementTreeNode result;

            switch(currentGene)
            {
                case -1:
                    //x
                    result.maxx=a.maxx+b.maxx;
                    result.maxy=qMax(a.maxy,b.maxy);
                    result.maxz=qMax(a.maxz,b.maxz);
                    break;
                case -2:
                    //y
                    result.maxx=qMax(a.maxx,b.maxx);
                    result.maxy=a.maxy+b.maxy;
                    result.maxz=qMax(a.maxz,b.maxz);
                    break;
                case -3:
                    //z
                    result.maxx=qMax(a.maxx,b.maxx);
                    result.maxy=qMax(a.maxy,b.maxy);
                    result.maxz=a.maxz+b.maxz;
                    break;
            }
            result.operation=currentGene;
            result.left=nodes.length();
            nodes.push_back(a);
            result.right=nodes.length();
            nodes.push_back(b);
            operands.push(result);
        }else
        {
            Box box=boxes[currentGene];
            placementTreeNode node=placementTreeNode{box.w,box.h,box.l,0,0,currentGene};
            operands.push(node);
        }
    }
    if(operands.length()!=1)
        throw QString("Ошибка в записи ОПН");
    placementTreeNode last=operands.pop();
    struct node{int id;float minx;float miny;float minz;};
    QStack<node> stack;
    stack.push({nodes.length(),0.0f,0.0f,0.0f});
    nodes.push_back(last);
    while(stack.length()!=0)
    {
     node nd=stack.pop();
     placementTreeNode tnd=nodes[nd.id];
     int left=tnd.left;
     int right=tnd.right;
     int operation=tnd.operation;
     if(operation>=0)//лист с номером гена/ящика
     {
         Box box=boxes[operation];
         placements.push_back(BoxInfo{nd.minx+0.5*box.w,nd.miny+0.5*box.h,nd.minz+0.5*box.l,operation});
     }else
     {
         placementTreeNode ltnd=nodes[left];
         switch(operation)
         {
         case -1:
             //x
             stack.push(node{left,nd.minx,nd.miny,nd.minz});
             stack.push(node{right,nd.minx+ltnd.maxx,nd.miny,nd.minz});
             break;
         case -2:
             //y
             stack.push(node{left,nd.minx,nd.miny,nd.minz});
             stack.push(node{right,nd.minx,nd.miny+ltnd.maxy,nd.minz});
             break;
         case -3:
             //z
             stack.push(node{left,nd.minx,nd.miny,nd.minz});
             stack.push(node{right,nd.minx,nd.miny,nd.minz+ltnd.maxz});
             break;
         }
     }
    }
    return placements;
}
#endif
#endif
