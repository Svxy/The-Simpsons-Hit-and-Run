//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        occlusionlocatornode.h
//
// Description: Blahblahblah
//
// History:     28/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef OCCLUSIONLOCATORNODE_H
#define OCCLUSIONLOCATORNODE_H

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
BOOL CALLBACK OcclusionLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam );

class OcclusionLocatorNode : public MPxLocatorNode
{
public:
    enum { MAX_NAME_LEN = 256 };

    OcclusionLocatorNode();
    virtual ~OcclusionLocatorNode();

    static void*    creator();

    virtual void    draw( M3dView& view, 
                          const MDagPath& path, 
                          M3dView::DisplayStyle displayStyle, 
                          M3dView::DisplayStatus displayStatus 
                        );  
    static MStatus  initialize();    
    virtual void    postConstructor();

    //This is how you export one of these.
    static tlDataChunk* Export( MObject& occlusionLocatorNode );

    static void SetNewName( const char* name );
    static const char* const GetNewName();

    static MTypeId      id;
    static const char*  stringId;

    static const char*  TRIGGERS_NAME_SHORT;
    static const char*  TRIGGERS_NAME_LONG;
    static MObject sTriggers;

private:

    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;

    static char sNewName[MAX_NAME_LEN];

    //Prevent wasteful constructor creation.
    OcclusionLocatorNode( const OcclusionLocatorNode& occlusionlocatornode );
    OcclusionLocatorNode& operator=( const OcclusionLocatorNode& occlusionlocatornode );
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// OcclusionLocatorNode::SetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void OcclusionLocatorNode::SetNewName( const char* name )
{
    strncpy( sNewName, name, MAX_NAME_LEN);
}

//=============================================================================
// OcclusionLocatorNode::GetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const OcclusionLocatorNode::GetNewName()
{
    return sNewName;
}

#endif //OCCLUSIONLOCATORNODE_H
