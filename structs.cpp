#include "structs.h"



QString orientationToString(BoxOrientation o)
{
    switch(o)
    {
        case XYZ0:
        return "XYZ";
        case XZY1:
        return "XZY";
        case YXZ2:
        return "YXZ";
        case YZX3:
        return "YZX";
        case ZXY4:
        return "ZXY";
        case ZYX5:
        return "ZYX";
    }
    return "__";
}
