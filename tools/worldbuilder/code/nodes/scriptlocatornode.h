//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        scriptlocatornode.h
//
// Description: Blahblahblah
//
// History:     27/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef SCRIPTLOCATORNODE_H
#define SCRIPTLOCATORNODE_H

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
BOOL CALLBACK ScriptLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam );

class ScriptLocatorNode : public MPxLocatorNode
{
public:
    enum { MAX_NAME_LEN = 256 };

    ScriptLocatorNode();
    virtual ~ScriptLocatorNode();

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

    static void SetScriptName( const char* name );
    static const char* const GetScriptName();

    static MTypeId      id;
    static const char*  stringId;

    static const char*  TRIGGERS_NAME_SHORT;
    static const char*  TRIGGERS_NAME_LONG;
    static MObject sTriggers;

    static const char*  SCRIPT_NAME_SHORT;
    static const char*  SCRIPT_NAME_LONG;
    static MObject sScript;

private:
    
    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;

    static char sNewName[MAX_NAME_LEN];
    static char sScriptName[MAX_NAME_LEN];

    //Prevent wasteful constructor creation.
    ScriptLocatorNode( const ScriptLocatorNode& scriptlocatornode );
    ScriptLocatorNode& operator=( const ScriptLocatorNode& scriptlocatornode );
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// ScriptLocatorNode::SetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void ScriptLocatorNode::SetNewName( const char* name )
{
    strncpy( sNewName, name, MAX_NAME_LEN);
}

//=============================================================================
// ScriptLocatorNode::GetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const ScriptLocatorNode::GetNewName()
{
    return sNewName;
}

//=============================================================================
// ScriptLocatorNode::SetScriptName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void ScriptLocatorNode::SetScriptName( const char* name )
{
    strncpy( sScriptName, name, MAX_NAME_LEN);
}

//=============================================================================
// ScriptLocatorNode::GetScriptName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const ScriptLocatorNode::GetScriptName()
{
    return sScriptName;
}
#endif //SCRIPTLOCATORNODE_H
