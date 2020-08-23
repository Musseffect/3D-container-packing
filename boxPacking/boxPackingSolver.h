#ifndef BOXPACKINGSOLVER_H
#define BOXPACKINGSOLVER_H
#include "structs.h"
#include <QVector>
#include <QString>
#include <QObject>

struct AABB{
    float minx;
    float miny;
    float minz;
    float maxx;
    float maxy;
    float maxz;
};


class BoxPackingSolver:public QObject
{
    Q_OBJECT
protected:
    QString log;
public:
    virtual QVector<BoxInfo> solve(const QVector<Box>& boxes,const Box& bounds) = 0;
    QString getLog()const;
signals:
    void progress(int value);
};

#endif // BOXPACKINGSOLVER_H
