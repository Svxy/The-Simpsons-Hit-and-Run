//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        scriptlocatornode.h
//
// Description: Blahblahblah
//
// History:     16/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef SPLINELOCATORNODE_H
#define SPLINELOCATORNODE_H

//========================================
// Nested Includes
//========================================
#include "precompiled/PCH.h"

//========================================
// Forward References
//========================================
class tlDataChunk;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class SplineLocatorNode : public MPxLocatorNode
{
public:
    SplineLocatorNode();
    virtual ~SplineLocatorNode();

    static void*    creator();

    virtual void    draw( M3dView& view, 
                          const MDagPath& path, 
                          M3dView::DisplayStyle displayStyle, 
                          M3dView::DisplayStatus displayStatus 
                        );  
    static MStatus  initialize();    
    virtual void    postConstructor();

    //This is how you export one of these.
    static tlDataChunk* Export( MObject& eventLocatorNode );

    static MTypeId      id;
    static const char*  stringId;

    static const char*  TRIGGERS_NAME_SHORT;
    static const char*  TRIGGERS_NAME_LONG;
    static MObject sTriggers;

    static const char*  SPLINE_NAME_SHORT;
    static const char*  SPLINE_NAME_LONG;
    static MObject sSpline;

    static const char*  CAMERA_NAME_SHORT;
    static const char*  CAMERA_NAME_LONG;
    static MObject sCamera;
    
    static const char* IS_COIN_SHORT;
    static const char* IS_COIN_LONG;
    static MObject sIsCoin;

private:
    
    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;

    //Prevent wasteful constructor creation.
    SplineLocatorNode( const SplineLocatorNode& eventlocatornode );
    SplineLocatorNode& operator=( const SplineLocatorNode& eventlocatornode );
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// SplineLocatorNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
inline void* SplineLocatorNode::creator()
{
    return new SplineLocatorNode();
}

#endif //SPLINELOCATORNODE_H
