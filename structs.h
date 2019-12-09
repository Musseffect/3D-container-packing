#ifndef STRUCTS
#define STRUCTS
#include <QColor>
#include <QString>
#include <QJsonObject>
#include <QMetaType>
#include <QVector>

enum BoxOrientation
{
  XYZ0=0x0024,//012
  XZY1=0x0018,//021
  YXZ2=0x0021,//102
  YZX3=0x0009,//120
  ZXY4=0x0012,//201
  ZYX5=0x0006//210
};

static BoxOrientation orientations[6]={BoxOrientation::XYZ0,BoxOrientation::XZY1,
                              BoxOrientation::YXZ2,BoxOrientation::YZX3,
                              BoxOrientation::ZXY4,BoxOrientation::ZYX5};

QString orientationToString(BoxOrientation o);
int orientationToInteger(BoxOrientation o);

class Box
{
private:
    //compiler doesnt hide field of an nested anonymous struct
    union{
        struct{
            float w;
            float h;
            float l;
        };
        float a[3];
    };
    QColor c;
public:
    Box():w(0.0f),h(0.0f),l(0.0f),c()
    {
    }
    Box(const float w,const float h,const float l,const QColor& c):w(w),h(h),l(l),c(c)
    {
    }
    Box getOrientation(BoxOrientation o) const
    {
        Box result;
        result.w=a[o&0x0003];
        result.h=a[(o>>2)&0x0003];
        result.l=a[(o>>4)&0x0003];
        result.c=c;
        return result;
    }
    float volume()const
    {
        return w*h*l;
    }
    float& width()
    {
        return w;
    }
    float& height()
    {
        return h;
    }
    float& length()
    {
        return l;
    }
    float width()const
    {
        return w;
    }
    float height()const
    {
        return h;
    }
    float length()const
    {
        return l;
    }
    QColor color()const
    {
        return c;
    }
    QColor& color()
    {
        return c;
    }
    void read(const QJsonObject &json)
    {
        if (json.contains("w") && json["w"].isDouble())
            w = json["w"].toDouble();
        if (json.contains("h") && json["h"].isDouble())
            h = json["h"].toDouble();
        if (json.contains("l") && json["l"].isDouble())
            l = json["l"].toDouble();
        if(json.contains("color")&&json["color"].isDouble())
            c.fromRgb(json["color"].toInt());
    }
    void write(QJsonObject &json)
    {
        json["w"] = w;
        json["h"] = h;
        json["l"] = l;
        json["color"]=(double)c.rgb();
    }
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
    BoxOrientation o;
};
Q_DECLARE_METATYPE(Box);
Q_DECLARE_METATYPE(BoxInfo);
Q_DECLARE_METATYPE(QVector<BoxInfo>);
Q_DECLARE_METATYPE(QVector<Box>);

#endif // STRUCTS

