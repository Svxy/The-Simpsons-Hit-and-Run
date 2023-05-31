#include "precompiled/PCH.h"

#ifndef INTERSECTION_LOCATOR
#define INTERSECTION_LOCATOR


#include "main/constants.h"

class tlDataChunk;

class IntersectionLocatorNode : public MPxLocatorNode
{
public:
    IntersectionLocatorNode();
    ~IntersectionLocatorNode();

    static void*    creator();

    virtual void    draw( M3dView& view, 
                          const MDagPath& path, 
                          M3dView::DisplayStyle displayStyle, 
                          M3dView::DisplayStatus displayStatus 
                        );  
    static MStatus  initialize();    
    virtual void    postConstructor();

    //This is how you export one of these.
    static tlDataChunk* Export( MObject& intersectionLocatorNode, tlHistory& history );

    static const char* TYPE_NAME_LONG;
    static const char* TYPE_NAME_SHORT;
    static MObject  mType;

    static const char* ROAD_LONG;
    static const char* ROAD_SHORT;
    static MObject  mRoads; //This is an out message to all the roads this intersection connects.

    static MTypeId      id;
    static const char*  stringId;

private:

    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;
};

#endif