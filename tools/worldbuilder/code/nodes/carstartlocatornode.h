//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        carstartlocatornode.h
//
// Description: Blahblahblah
//
// History:     28/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef CARSTARTLOCATORNODE_H
#define CARSTARTLOCATORNODE_H

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
BOOL CALLBACK CarStartLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam );


class CarStartLocatorNode : public MPxLocatorNode
{
public:
    enum { MAX_NAME_LEN = 256 };

    CarStartLocatorNode();
    virtual ~CarStartLocatorNode();

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

    static const char*  ISPARKED_NAME_SHORT;
    static const char*  ISPARKED_NAME_LONG;
    static MObject sIsParked;
    
    static const char*  SPECIAL_NAME_SHORT;
    static const char*  SPECIAL_NAME_LONG;
    static MObject sSpecialCarName;

private:
    
    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;

    static char sNewName[MAX_NAME_LEN];

    //Prevent wasteful constructor creation.
    CarStartLocatorNode( const CarStartLocatorNode& carstartlocatornode );
    CarStartLocatorNode& operator=( const CarStartLocatorNode& carstartlocatornode );
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// CarStartLocatorNode::SetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void CarStartLocatorNode::SetNewName( const char* name )
{
    strncpy( sNewName, name, MAX_NAME_LEN);
}

//=============================================================================
// CarStartLocatorNode::GetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const CarStartLocatorNode::GetNewName()
{
    return sNewName;
}

#endif //CARSTARTLOCATORNODE_H
