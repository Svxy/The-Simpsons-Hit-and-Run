#include "precompiled/PCH.h"


#ifndef FENCELINE
#define FENCELINE

#include "main/constants.h"

class tlDataChunk;

class FenceLineNode : public MPxLocatorNode
{
public:
    FenceLineNode();
    ~FenceLineNode();

    static void*    creator();
    static MStatus  initialize();    
    virtual void    postConstructor();

    static void     AddWall( MObject& fenceLine, MObject& wall );

    //This is how you export one of these.
    static tlDataChunk* Export( MObject& fenceNode, tlHistory& history );

    static MTypeId      id;
    static const char*  stringId;
};

#endif