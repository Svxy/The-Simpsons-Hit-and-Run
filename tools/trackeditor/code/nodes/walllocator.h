#include "precompiled/PCH.h"

#ifndef WALL_LOCATOR
#define WALL_LOCATOR


#include "main/constants.h"

class tlDataChunk;

class WallLocatorNode : public MPxLocatorNode
{
public:
    WallLocatorNode();
    ~WallLocatorNode();

    static void*    creator();

    virtual void    draw( M3dView& view, 
                          const MDagPath& path, 
                          M3dView::DisplayStyle displayStyle, 
                          M3dView::DisplayStatus displayStatus 
                        );  
    static MStatus  initialize();    
    virtual MStatus legalConnection ( const MPlug & plug, const MPlug & otherPlug, bool asSrc, bool& result ) const; 
    virtual void    postConstructor();

    //This is how you export one of these.
    static tlDataChunk* Export( MObject& wallLocatorNode, tlHistory& history );
    static bool CalculateNormal( MObject& thisNode, MPoint& nextNodeWP, MVector* normal );

    static MTypeId      id;
    static const char*  stringId;

    //Custom to this object.
    static const char*  LEFTRIGHT_NAME_SHORT;
    static const char*  LEFTRIGHT_NAME_LONG;
    static MObject      mLeftRight;

    enum 
    {
        LEFT,
        RIGHT,
        NONE
    };

    static const char*  PREVNODE_NAME_SHORT;
    static const char*  PREVNODE_NAME_LONG;
    static MObject      mPrevNode;

    static const char*  NEXTNODE_NAME_SHORT;
    static const char*  NEXTNODE_NAME_LONG;
    static MObject      mNextNode;

    static const char*  ID_NAME_SHORT;
    static const char*  ID_NAME_LONG;
    static MObject      mCallbackId;

private:

    static void NodeAboutToDeleteCallback( MDGModifier& modifier, void* data );
    bool CalculateNormal( MPoint& nextNodeWP, MVector* normal );

    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;
};

#endif