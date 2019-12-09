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

int orientationToInteger(BoxOrientation o)
{
    switch(o)
    {
        case XYZ0:
        return 0;
        case XZY1:
        return 1;
        case YXZ2:
        return 2;
        case YZX3:
        return 3;
        case ZXY4:
        return 4;
        case ZYX5:
        return 5;
    }
    return -1;
}
