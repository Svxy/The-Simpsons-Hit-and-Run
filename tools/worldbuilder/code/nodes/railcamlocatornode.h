//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        railcamlocatornode.h
//
// Description: Blahblahblah
//
// History:     18/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef RAILCAMLOCATORNODE_H
#define RAILCAMLOCATORNODE_H

//========================================
// Nested Includes
//========================================
#include "precompiled/PCH.h"

//========================================
// Forward References
//========================================
class tlDataChunk;
class RailCam;
class WBCamTarget;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================
BOOL CALLBACK RailCamLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam );

class RailCamLocatorNode : public MPxLocatorNode
{
public:
    enum { MAX_NAME_LEN = 256 };

    RailCamLocatorNode();
    virtual ~RailCamLocatorNode();

    static void*    creator();

    virtual void    draw( M3dView& view, 
                          const MDagPath& path, 
                          M3dView::DisplayStyle displayStyle, 
                          M3dView::DisplayStatus displayStatus 
                        );  
    static MStatus  initialize();    
    virtual void    postConstructor();

    //This is how you export one of these.
    static tlDataChunk* Export( MObject& railCamLocatorNode );

    static void SetNewName( const char* name );
    static const char* const GetNewName();

    static MTypeId      id;
    static const char*  stringId;

    static const char*  TRIGGERS_NAME_SHORT;
    static const char*  TRIGGERS_NAME_LONG;
    static MObject sTriggers;

    static const char*  RAIL_NAME_SHORT;
    static const char*  RAIL_NAME_LONG;
    static MObject sRail;

    static const char*  BEHAVIOUR_NAME_SHORT;
    static const char*  BEHAVIOUR_NAME_LONG;
    static MObject sBehaviour;

    static const char*  MIN_RADIUS_NAME_SHORT;
    static const char*  MIN_RADIUS_NAME_LONG;
    static MObject sMinRadius;

    static const char*  MAX_RADIUS_NAME_SHORT;
    static const char*  MAX_RADIUS_NAME_LONG;
    static MObject sMaxRadius;

    static const char*  TRACK_RAIL_NAME_SHORT;
    static const char*  TRACK_RAIL_NAME_LONG;
    static MObject sTrackRail;

    static const char*  TRACK_DIST_NAME_SHORT;
    static const char*  TRACK_DIST_NAME_LONG;
    static MObject sTrackDist;
    
    static const char*  REVERSE_SENSE_NAME_SHORT;
    static const char*  REVERSE_SENSE_NAME_LONG;
    static MObject sReverseSense;


    static const char*  FOV_NAME_SHORT;
    static const char*  FOV_NAME_LONG;
    static MObject sFOV;

    static const char*  FACING_OFFSET_NAME_SHORT;
    static const char*  FACING_OFFSET_NAME_LONG;
    static MObject sFacingOffset;
    
    static const char*  AXIS_PLAY_NAME_SHORT;
    static const char*  AXIS_PLAY_NAME_LONG;
    static MObject sAxisPlay;

    static const char*  ACTIVE_NAME_SHORT;
    static const char*  ACTIVE_NAME_LONG;
    static MObject sActive;

    static const char*  TARGET_NAME_SHORT;
    static const char*  TARGET_NAME_LONG;
    static MObject sTarget;

    static const char*  POS_LAG_NAME_SHORT;
    static const char*  POS_LAG_NAME_LONG;
    static MObject sPositionLag;

    static const char*  TARGET_LAG_NAME_SHORT;
    static const char*  TARGET_LAG_NAME_LONG;
    static MObject sTargetLag;

    static const char* TRANSITION_TO_RATE_NAME_LONG;
    static const char* TRANSITION_TO_RATE_NAME_SHORT;
    static MObject sTransitionToRate;
    
    static const char* NOFOV_NAME_LONG;
    static const char* NOFOV_NAME_SHORT;
    static MObject sNoFOV;
    
    static const char* CAR_ONLY_NAME_LONG;
    static const char* CAR_ONLY_NAME_SHORT;
    static MObject sCarOnly;

    static const char* ON_FOOT_ONLY_NAME_LONG;
    static const char* ON_FOOT_ONLY_NAME_SHORT;
    static MObject sOnFootOnly;
    
    static const char* CUTALL_NAME_LONG;
    static const char* CUTALL_NAME_SHORT;
    static MObject sCutAll;

    static const char* RESET_NAME_LONG;
    static const char* RESET_NAME_SHORT;
    static MObject sReset;
    
    //MPxNode stuff...
    RailCam* GetRailCam();
    WBCamTarget* GetTarget();

private:
    
    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;

    static char sNewName[MAX_NAME_LEN];

    RailCam* mRailCam;
    WBCamTarget* mCamTarget;

    //Prevent wasteful constructor creation.
    RailCamLocatorNode( const RailCamLocatorNode& eventlocatornode );
    RailCamLocatorNode& operator=( const RailCamLocatorNode& railCamlocatornode );
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// RailCamLocatorNode::SetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void RailCamLocatorNode::SetNewName( const char* name )
{
    strncpy( sNewName, name, MAX_NAME_LEN);
}

//=============================================================================
// RailCamLocatorNode::GetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const RailCamLocatorNode::GetNewName()
{
    return sNewName;
}

//=============================================================================
// RailCamLocatorNode::GetRailCam
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      RailCam
//
//=============================================================================
inline RailCam* RailCamLocatorNode::GetRailCam()
{
    return mRailCam;
}

//=============================================================================
// RailCamLocatorNode::GetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      WBCamTarget
//
//=============================================================================
inline WBCamTarget* RailCamLocatorNode::GetTarget()
{
    return mCamTarget;
}

#endif //RAILCAMLOCATORNODE_H
