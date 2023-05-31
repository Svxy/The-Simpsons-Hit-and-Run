#include "precompiled/PCH.h"


#ifndef PED_PATH
#define PED_PATH

#include "main/constants.h"

class tlDataChunk;

class PedPathNode : public MPxLocatorNode
{
public:
    PedPathNode();
    ~PedPathNode();

    static void*    creator();
    static MStatus  initialize();    
    virtual void    postConstructor();

    static void     AddWall( MObject& fenceLine, MObject& wall );

    //This is how you export one of these.
    static tlDataChunk* Export( MObject& fenceNode, tlHistory& history );

    static MTypeId      id;
    static const char*  stringId;
};

#endif //PED_PATH