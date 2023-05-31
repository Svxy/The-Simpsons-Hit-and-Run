//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        breakablecameralocatornode.h
//
// Description: Blahblahblah
//
// History:     9/17/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef BREAKABLECAMERALOCATORNODE_H
#define BREAKABLECAMERALOCATORNODE_H

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
BOOL CALLBACK BreakableCameraLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam );

class BreakableCameraLocatorNode : public MPxLocatorNode
{
public:
    enum { MAX_NAME_LEN = 256 };

    BreakableCameraLocatorNode();
	virtual ~BreakableCameraLocatorNode();

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

    static const char*  FOV_NAME_SHORT;
    static const char*  FOV_NAME_LONG;
    static MObject sFOV;

private:
    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;

    static char sNewName[MAX_NAME_LEN];

    //Prevent wasteful constructor creation.
	BreakableCameraLocatorNode( const BreakableCameraLocatorNode& breakablecameralocatornode );
	BreakableCameraLocatorNode& operator=( const BreakableCameraLocatorNode& breakablecameralocatornode );
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// BreakableCameraLocatorNode::SetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void BreakableCameraLocatorNode::SetNewName( const char* name )
{
    strncpy( sNewName, name, MAX_NAME_LEN);
}

//=============================================================================
// BreakableCameraLocatorNode::GetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const BreakableCameraLocatorNode::GetNewName()
{
    return sNewName;
}


#endif //BREAKABLECAMERALOCATORNODE_H
