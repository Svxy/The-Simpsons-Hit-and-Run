//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        genericlocatornode.h
//
// Description: Blahblahblah
//
// History:     27/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef GENERICLOCATORNODE_H
#define GENERICLOCATORNODE_H

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
BOOL CALLBACK GenericLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam );

class GenericLocatorNode : public MPxLocatorNode
{
public:
    enum { MAX_NAME_LEN = 256 };

    GenericLocatorNode();
    virtual ~GenericLocatorNode();

    static void*    creator();

    virtual void    draw( M3dView& view, 
                          const MDagPath& path, 
                          M3dView::DisplayStyle displayStyle, 
                          M3dView::DisplayStatus displayStatus 
                        );  
    static MStatus  initialize();    
    virtual void    postConstructor();

    //This is how you export one of these.
    static tlDataChunk* Export( MObject& GenericLocatorNode );

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
    GenericLocatorNode( const GenericLocatorNode& genericlocatornode );
    GenericLocatorNode& operator=( const GenericLocatorNode& genericlocatornode );
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// GenericLocatorNode::SetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void GenericLocatorNode::SetNewName( const char* name )
{
    strncpy( sNewName, name, MAX_NAME_LEN);
}

//=============================================================================
// GenericLocatorNode::GetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const GenericLocatorNode::GetNewName()
{
    return sNewName;
}

#endif //GENERICLOCATORNODE_H
