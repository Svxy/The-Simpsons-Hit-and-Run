//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        staticcameralocatornode.h
//
// Description: Blahblahblah
//
// History:     9/17/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef STATICCAMERALOCATORNODE_H
#define STATICCAMERALOCATORNODE_H

//========================================
// Nested Includes
//========================================
#include "precompiled/PCH.h"

//========================================
// Forward References
//========================================
class tlDataChunk;
class StaticCam;
class WBCamTarget;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================
BOOL CALLBACK StaticCameraLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam );

class StaticCameraLocatorNode : public MPxLocatorNode
{
public:
    enum { MAX_NAME_LEN = 256 };

    StaticCameraLocatorNode();
	virtual ~StaticCameraLocatorNode();

    static void*    creator();

    virtual void    draw( M3dView& view, 
                          const MDagPath& path, 
                          M3dView::DisplayStyle displayStyle, 
                          M3dView::DisplayStatus displayStatus 
                        );  
    static MStatus  initialize();    
    virtual void    postConstructor();

    //This is how you export one of these.
    static tlDataChunk* Export( MObject& staticCameraLocatorNode );

    static void SetNewName( const char* name );
    static const char* const GetNewName();

    static MTypeId      id;
    static const char*  stringId;

    static const char*  TRIGGERS_NAME_SHORT;
    static const char*  TRIGGERS_NAME_LONG;
    static MObject sTriggers;

    static const char*  FOV_NAME_SHORT;
    static const char*  FOV_NAME_LONG;
    static MObject sFOV;

    static const char*  TARGET_NAME_SHORT;
    static const char*  TARGET_NAME_LONG;
    static MObject sTarget;

    static const char*  FACING_OFFSET_NAME_SHORT;
    static const char*  FACING_OFFSET_NAME_LONG;
    static MObject sFacingOffset;

    static const char* TRACKING_NAME_SHORT;
    static const char* TRACKING_NAME_LONG;
    static MObject sTracking;

    static const char*  TARGET_LAG_NAME_SHORT;
    static const char*  TARGET_LAG_NAME_LONG;
    static MObject sTargetLag;

    static const char*  ACTIVE_NAME_SHORT;
    static const char*  ACTIVE_NAME_LONG;
    static MObject sActive;
    
    static const char* TRANSITION_TO_RATE_NAME_LONG;
    static const char* TRANSITION_TO_RATE_NAME_SHORT;
    static MObject sTransitionToRate;

    static const char* ONESHOT_NAME_LONG;
    static const char* ONESHOT_NAME_SHORT;
    static MObject sOneShot;

    static const char* NOFOV_NAME_LONG;
    static const char* NOFOV_NAME_SHORT;
    static MObject sNoFOV;
    
    static const char* CUTALL_NAME_LONG;
    static const char* CUTALL_NAME_SHORT;
    static MObject sCutAll;

    static const char* CAR_ONLY_NAME_LONG;
    static const char* CAR_ONLY_NAME_SHORT;
    static MObject sCarOnly;

    static const char* ON_FOOT_ONLY_NAME_LONG;
    static const char* ON_FOOT_ONLY_NAME_SHORT;
    static MObject sOnFootOnly;

    //MPxNode stuff...
    StaticCam* GetStaticCam();
    WBCamTarget* GetTarget();

private:
    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;

    static char sNewName[MAX_NAME_LEN];

    StaticCam* mStaticCam;
    WBCamTarget* mCamTarget;

    //Prevent wasteful constructor creation.
	StaticCameraLocatorNode( const StaticCameraLocatorNode& staticcameralocatornode );
	StaticCameraLocatorNode& operator=( const StaticCameraLocatorNode& staticcameralocatornode );
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// StaticCameraLocatorNode::SetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void StaticCameraLocatorNode::SetNewName( const char* name )
{
    strncpy( sNewName, name, MAX_NAME_LEN);
}

//=============================================================================
// StaticCameraLocatorNode::GetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const StaticCameraLocatorNode::GetNewName()
{
    return sNewName;
}

//=============================================================================
// StaticCameraLocatorNode::GetStaticCam
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      StaticCam
//
//=============================================================================
inline StaticCam* StaticCameraLocatorNode::GetStaticCam()
{
    return mStaticCam;
}

//=============================================================================
// StaticCameraLocatorNode::GetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      WBCamTarget
//
//=============================================================================
inline WBCamTarget* StaticCameraLocatorNode::GetTarget()
{
    return mCamTarget;
}

#endif //STATICCAMERALOCATORNODE_H
