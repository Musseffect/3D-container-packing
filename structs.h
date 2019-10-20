#ifndef STRUCTS
#define STRUCTS
#include <QColor>

struct Box
{
    float w;
    float h;
    float l;
    QColor color;
};
struct BoxArrayStruct
{
    float cx;
    float cy;
    float cz;
    float sx;
    float sy;
    float sz;
    unsigned int r;
    unsigned int g;
    unsigned int b;
};
struct BoxInfo
{
    float x;
    float y;
    float z;
    int boxID;
};

#endif // STRUCTS

