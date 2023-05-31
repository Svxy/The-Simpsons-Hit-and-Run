//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        eventlocatornode.h
//
// Description: Blahblahblah
//
// History:     16/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef EVENTLOCATORNODE_H
#define EVENTLOCATORNODE_H

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
BOOL CALLBACK EventLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam );

class EventLocatorNode : public MPxLocatorNode
{
public:
    enum { MAX_NAME_LEN = 256 };

    EventLocatorNode();
    virtual ~EventLocatorNode();

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

    static const char*  EVENT_NAME_SHORT;
    static const char*  EVENT_NAME_LONG;
    static MObject sEvent;
    
    static const char*  EXTRA_NAME_SHORT;
    static const char*  EXTRA_NAME_LONG;
    static MObject sExtraData;

private:
    
    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;

    static char sNewName[MAX_NAME_LEN];

    //Prevent wasteful constructor creation.
    EventLocatorNode( const EventLocatorNode& eventlocatornode );
    EventLocatorNode& operator=( const EventLocatorNode& eventlocatornode );
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// EventLocatorNode::SetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void EventLocatorNode::SetNewName( const char* name )
{
    strncpy( sNewName, name, MAX_NAME_LEN);
}

//=============================================================================
// EventLocatorNode::GetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const EventLocatorNode::GetNewName()
{
    return sNewName;
}

#endif //EVENTLOCATORNODE_H
