#include "precompiled/PCH.h"


#ifndef TILE_DSIPLAY_H
#define TILE_DSIPLAY_H

class TileDisplayNode : public MPxLocatorNode
{
public:
    TileDisplayNode();
    ~TileDisplayNode();

    static void*    creator();
    virtual void    draw( M3dView& view, 
                          const MDagPath& path, 
                          M3dView::DisplayStyle displayStyle, 
                          M3dView::DisplayStatus displayStatus 
                        );  
    static MStatus  initialize();    

    static MTypeId      id;
    static const char*  stringId;

private:
    static const int ORIGIN_COLOUR;
    static const int ROAD_COLOUR;
    static const int TOP_COLOUR;
    static const int BOTTOM_COLOUR;
    static const int LANE_COLOUR;
    static const float SCALE;
    static const float LINE_WIDTH;
    static const float Y_OFFSET;
};

#endif