#ifndef BOXPACKINGSOLVER_H
#define BOXPACKINGSOLVER_H
#include "structs.h"
#include <QVector>
#include <QString>

struct emsstruct{
    float minx;
    float miny;
    float minz;
    float maxx;
    float maxy;
    float maxz;
};


class BoxPackingSolver
{
protected:
    QString log;
public:
    virtual QVector<BoxInfo> solve(const QVector<Box>& boxes,const Box& bounds) = 0;
    QString getLog()const;
};

#endif // BOXPACKINGSOLVER_H
