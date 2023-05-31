#ifndef DIRECTIONALARROWENUM_H
#define DIRECTIONALARROWENUM_H

namespace DirectionalArrowEnum
{
    enum TYPE
    {
        INTERSECTION = 0x01,
        NEAREST_ROAD = 0x10,
        BOTH         = 0x11,
        NEITHER      = 0x100,

        NUM_TYPES    = 4
    };
};

#endif