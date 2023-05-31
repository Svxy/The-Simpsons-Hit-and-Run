//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        triggervolumenode.h
//
// Description: Blahblahblah
//
// History:     23/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef TRIGGERVOLUMENODE_H
#define TRIGGERVOLUMENODE_H

//========================================
// Nested Includes
//========================================
#include "precompiled/PCH.h"
#include "main/toolhack.h"
#include <toollib.hpp>

//========================================
// Forward References
//========================================
class tlDataChunk;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class TriggerVolumeNode : public MPxLocatorNode
{
public:

    enum Type
    { 
        SPHERE,
        RECTANGLE
    };
    
    TriggerVolumeNode();
    virtual ~TriggerVolumeNode();

    static void*    creator();

    virtual void    draw( M3dView& view, 
                          const MDagPath& path, 
                          M3dView::DisplayStyle displayStyle, 
                          M3dView::DisplayStatus displayStatus 
                        );  
    static MStatus  initialize();    
    virtual void    postConstructor();

    //This is how you export one of these.
    static tlDataChunk* Export( MObject& triggerVolumeNode );
    static void GetScaleAndMatrix( MObject& triggerVolumeNode, tlMatrix& mat, tlPoint& point);

    static MTypeId      id;
    static const char*  stringId;

    static const char*  LOCATOR_NAME_SHORT;
    static const char*  LOCATOR_NAME_LONG;
    static MObject sLocator;

    static const char*  TYPE_NAME_SHORT;
    static const char*  TYPE_NAME_LONG;
    static MObject sType;

private:

    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;
    static const float LINE_WIDTH;

    Type mType;

    //Prevent wasteful constructor creation.
    TriggerVolumeNode( const TriggerVolumeNode& triggervolumenode );
    TriggerVolumeNode& operator=( const TriggerVolumeNode& triggervolumenode );
};


#endif //TRIGGERVOLUMENODE_H
