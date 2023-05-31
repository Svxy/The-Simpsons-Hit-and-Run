//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        directionallocatornode.h
//
// Description: Blahblahblah
//
// History:     29/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef DIRECTIONALLOCATORNODE_H
#define DIRECTIONALLOCATORNODE_H

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
BOOL CALLBACK DirectionalLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam );

class DirectionalLocatorNode : public MPxLocatorNode
{
public:
    enum { MAX_NAME_LEN = 256 };

    DirectionalLocatorNode();
    virtual ~DirectionalLocatorNode();

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

private:
    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;

    static char sNewName[MAX_NAME_LEN];

    //Prevent wasteful constructor creation.
    DirectionalLocatorNode( const DirectionalLocatorNode& directionallocatornode );
    DirectionalLocatorNode& operator=( const DirectionalLocatorNode& directionallocatornode );
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// DirectionalLocatorNode::SetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void DirectionalLocatorNode::SetNewName( const char* name )
{
    strncpy( sNewName, name, MAX_NAME_LEN);
}

//=============================================================================
// DirectionalLocatorNode::GetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const DirectionalLocatorNode::GetNewName()
{
    return sNewName;
}

#endif //DIRECTIONALLOCATORNODE_H
