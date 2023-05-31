#include "precompiled/PCH.h"


#ifndef ROAD_H
#define ROAD_H

#include "main/constants.h"

class tlDataChunk;

class RoadNode : public MPxLocatorNode
{
public:
    RoadNode();
    ~RoadNode();

    static void*    creator();
    static MStatus  initialize();    
    virtual void    postConstructor();

    //This is how you export one of these.
    static tlDataChunk* Export( MObject& roadNode, tlHistory& history, tlDataChunk* outChunk );

    static MTypeId      id;
    static const char*  stringId; 

    static const char*  ROAD_SEG_NAME_SHORT;
    static const char*  ROAD_SEG_NAME_LONG;
    static MObject      mRoadSegments;

    static const char*  INTERSECTION_START_SHORT;
    static const char*  INTERSECTION_START_LONG;
    static MObject      mIntersectionStart;

    static const char*  INTERSECTION_END_SHORT;
    static const char*  INTERSECTION_END_LONG;
    static MObject      mIntersectionEnd;

    static const char*  DENSITY_SHORT;
    static const char*  DENSITY_LONG;
    static MObject      mDensity;

    static const char*  SPEED_SHORT;
    static const char*  SPEED_LONG;
    static MObject      mSpeed;

    static const char*  DIFF_SHORT;
    static const char*  DIFF_LONG;
    static MObject      mDiff;

    static const char*  SHORTCUT_SHORT;
    static const char*  SHORTCUT_LONG;
    static MObject      mShortcut;
};

#endif