//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        zoneeventlocatornode.h
//
// Description: Blahblahblah
//
// History:     18/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef ZONEEVENTLOCATORNODE_H
#define ZONEEVENTLOCATORNODE_H

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
BOOL CALLBACK ZoneEventLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam );

class ZoneEventLocatorNode : public MPxLocatorNode
{
public:
    enum { MAX_NAME_LEN = 256 };

    ZoneEventLocatorNode();
    virtual ~ZoneEventLocatorNode();

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

    static void SetNewName( const char* name );
    static const char* const GetNewName();
    static void SetZoneName( const char* name );
    static const char* const GetZoneName();

    static MTypeId      id;
    static const char*  stringId;

    static const char*  TRIGGERS_NAME_SHORT;
    static const char*  TRIGGERS_NAME_LONG;
    static MObject sTriggers;

    static const char*  ZONE_NAME_SHORT;
    static const char*  ZONE_NAME_LONG;
    static MObject sZone;

private:
    
    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;

    static char sNewName[MAX_NAME_LEN];
    static char sZoneName[MAX_NAME_LEN];

    //Prevent wasteful constructor creation.
    ZoneEventLocatorNode( const ZoneEventLocatorNode& zoneeventlocatornode );
    ZoneEventLocatorNode& operator=( const ZoneEventLocatorNode& zoneeventlocatornode );
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// ZoneEventLocatorNode::SetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void ZoneEventLocatorNode::SetNewName( const char* name )
{
    strncpy( sNewName, name, MAX_NAME_LEN);
}

//=============================================================================
// ZoneEventLocatorNode::GetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const ZoneEventLocatorNode::GetNewName()
{
    return sNewName;
}

//=============================================================================
// ZoneEventLocatorNode::SetZoneName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void ZoneEventLocatorNode::SetZoneName( const char* name )
{
    strncpy( sZoneName, name, MAX_NAME_LEN);
}

//=============================================================================
// ZoneEventLocatorNode::GetZoneName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const ZoneEventLocatorNode::GetZoneName()
{
    return sZoneName;
}



#endif //ZONEEVENTLOCATORNODE_H
