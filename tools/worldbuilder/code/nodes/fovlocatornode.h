//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        fovlocatornode.h
//
// Description: Blahblahblah
//
// History:     03/08/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef FOVLOCATORNODE_H
#define FOVLOCATORNODE_H

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
BOOL CALLBACK FOVLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam );

class FOVLocatorNode : public MPxLocatorNode
{
public:
    enum { MAX_NAME_LEN = 256 };

    FOVLocatorNode();
    virtual ~FOVLocatorNode();

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

    static MTypeId      id;
    static const char*  stringId;

    static const char*  TRIGGERS_NAME_SHORT;
    static const char*  TRIGGERS_NAME_LONG;
    static MObject sTriggers;

    static const char*  FOV_NAME_SHORT;
    static const char*  FOV_NAME_LONG;
    static MObject sFOV;

    static const char*  TIME_NAME_SHORT;
    static const char*  TIME_NAME_LONG;
    static MObject sTime;

    static const char*  RATE_NAME_SHORT;
    static const char*  RATE_NAME_LONG;
    static MObject sRate;

private:
    
    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;

    static char sNewName[MAX_NAME_LEN];

    //Prevent wasteful constructor creation.
    FOVLocatorNode( const FOVLocatorNode& fovlocatornode );
    FOVLocatorNode& operator=( const FOVLocatorNode& fovlocatornode );
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// FOVLocatorNode::SetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void FOVLocatorNode::SetNewName( const char* name )
{
    strncpy( sNewName, name, MAX_NAME_LEN);
}

//=============================================================================
// FOVLocatorNode::GetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const FOVLocatorNode::GetNewName()
{
    return sNewName;
}

#endif //FOVLOCATORNODE_H
