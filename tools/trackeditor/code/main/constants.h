#ifndef TE_CONSTANTS
#define TE_CONSTANTS

namespace TEConstants
{
    const unsigned int TypeIDPrefix = 0x00040200;
    const float Scale = 100.0f;

    namespace NodeIDs
    {
        const unsigned int WallLocator  = 0xc0;
        const unsigned int FenceLine    = 0xc1;
        const unsigned int TileDisplay  = 0xc2;
        const unsigned int Intersection = 0xc3;
        const unsigned int Road         = 0xc4;
        const unsigned int TreeLine     = 0xc5;
        const unsigned int PedPath      = 0xc6;
    }
}

#endif