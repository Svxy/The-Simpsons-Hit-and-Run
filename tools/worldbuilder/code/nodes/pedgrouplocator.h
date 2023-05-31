//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        pedgrouplocatornode.h
//
// Description: Blahblahblah
//
// History:     29/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef PEDGROUPLOCATORNODE_H
#define PEDGROUPLOCATORNODE_H

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
BOOL CALLBACK PedGroupLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam );

class PedGroupLocatorNode : public MPxLocatorNode
{
public:
    enum { MAX_NAME_LEN = 256 };

    PedGroupLocatorNode();
    virtual ~PedGroupLocatorNode();

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

    static MTypeId      id;
    static const char*  stringId;

    static const char*  TRIGGERS_NAME_SHORT;
    static const char*  TRIGGERS_NAME_LONG;
    static MObject sTriggers;

    static const char*  GROUP_NAME_SHORT;
    static const char*  GROUP_NAME_LONG;
    static MObject sGroup;

private:
    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;

    static char sNewName[MAX_NAME_LEN];

    //Prevent wasteful constructor creation.
    PedGroupLocatorNode( const PedGroupLocatorNode& pedgrouplocatornode );
    PedGroupLocatorNode& operator=( const PedGroupLocatorNode& pedgrouplocatornode );
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// PedGroupLocatorNode::SetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void PedGroupLocatorNode::SetNewName( const char* name )
{
    strncpy( sNewName, name, MAX_NAME_LEN);
}

//=============================================================================
// PedGroupLocatorNode::GetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const PedGroupLocatorNode::GetNewName()
{
    return sNewName;
}

#endif //PEDGROUPLOCATORNODE_H
