//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        interiorentrancelocatornode.h
//
// Description: Blahblahblah
//
// History:     29/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef INTERIORENTRANCELOCATORNODE_H
#define INTERIORENTRANCELOCATORNODE_H

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
BOOL CALLBACK InteriorEntranceLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam );


class InteriorEntranceLocatorNode  : public MPxLocatorNode
{
public:
    enum { MAX_NAME_LEN = 256 };

    InteriorEntranceLocatorNode();
    virtual ~InteriorEntranceLocatorNode();

    static void*    creator();

    virtual void    draw( M3dView& view, 
                          const MDagPath& path, 
                          M3dView::DisplayStyle displayStyle, 
                          M3dView::DisplayStatus displayStatus 
                        );  
    static MStatus  initialize();    
    virtual void    postConstructor();

    //This is how you export one of these.
    static tlDataChunk* Export( MObject& interiorEntranceLocatorNode );

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
    InteriorEntranceLocatorNode( const InteriorEntranceLocatorNode& interiorentrancelocatornode );
    InteriorEntranceLocatorNode& operator=( const InteriorEntranceLocatorNode& interiorentrancelocatornode );
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// InteriorEntranceLocatorNode::SetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void InteriorEntranceLocatorNode::SetNewName( const char* name )
{
    strncpy( sNewName, name, MAX_NAME_LEN);
}

//=============================================================================
// InteriorEntranceLocatorNode::GetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const InteriorEntranceLocatorNode::GetNewName()
{
    return sNewName;
}

//=============================================================================
// InteriorEntranceLocatorNode::SetZoneName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void InteriorEntranceLocatorNode::SetZoneName( const char* name )
{
    strncpy( sZoneName, name, MAX_NAME_LEN);
}

//=============================================================================
// InteriorEntranceLocatorNode::GetZoneName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const InteriorEntranceLocatorNode::GetZoneName()
{
    return sZoneName;
}


#endif //INTERIORENTRANCELOCATORNODE_H
