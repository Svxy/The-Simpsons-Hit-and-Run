//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        actioneventlocatornode.h
//
// Description: Blahblahblah
//
// History:     29/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef ACTIONEVENTLOCATORNODE_H
#define ACTIONEVENTLOCATORNODE_H

//========================================
// Nested Includes
//========================================
#include "precompiled/PCH.h"
#include "../../../game/code/ai/actionnames.h"

//========================================
// Forward References
//========================================
class tlDataChunk;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================
BOOL CALLBACK ActionEventLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam );

class ActionEventLocatorNode : public MPxLocatorNode
{
public:
    enum { MAX_NAME_LEN = 256 };

    ActionEventLocatorNode();
    virtual ~ActionEventLocatorNode();

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
    static void SetNewObj( const char* name );
    static const char* const GetNewObj();
    static void SetNewJoint( const char* name );
    static const char* const GetNewJoint();

    static MTypeId      id;
    static const char*  stringId;

    static const char*  TRIGGERS_NAME_SHORT;
    static const char*  TRIGGERS_NAME_LONG;
    static MObject sTriggers;

    static const char*  OBJECT_NAME_SHORT;
    static const char*  OBJECT_NAME_LONG;
    static MObject sObject;

    static const char*  JOINT_NAME_SHORT;
    static const char*  JOINT_NAME_LONG;
    static MObject sJoint;

    static const char*  ACTION_NAME_SHORT;
    static const char*  ACTION_NAME_LONG;
    static MObject sActionType;

    static const char*  BUTTON_NAME_SHORT;
    static const char*  BUTTON_NAME_LONG;
    static MObject sButtonInput;

    static const char*  TRANSFORM_NAME_SHORT;
    static const char*  TRANSFORM_NAME_LONG;
    static MObject sTransform;

    static const char*  EXPORT_TRANSFORM_NAME_SHORT;
    static const char*  EXPORT_TRANSFORM_NAME_LONG;
    static MObject sExportTransform;
private:
    
    static const int ACTIVE_COLOUR;
    static const int INACTIVE_COLOUR;
    static const float SCALE;

    static char sNewName[MAX_NAME_LEN];
    static char sNewObj[MAX_NAME_LEN];
    static char sNewJoint[MAX_NAME_LEN];

    //We want the names in alphapetical order.
    static const char* names[ActionButton::ActionNameSize];

    //Prevent wasteful constructor creation.
    ActionEventLocatorNode( const ActionEventLocatorNode& actioneventlocatornode );
    ActionEventLocatorNode& operator=( const ActionEventLocatorNode& actioneventlocatornode );
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// ActionEventLocatorNode::SetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void ActionEventLocatorNode::SetNewName( const char* name )
{
    strncpy( sNewName, name, MAX_NAME_LEN);
}

//=============================================================================
// ActionEventLocatorNode::GetNewName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const ActionEventLocatorNode::GetNewName()
{
    return sNewName;
}

//=============================================================================
// ActionEventLocatorNode::SetNewObj
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void ActionEventLocatorNode::SetNewObj( const char* name )
{
    strncpy( sNewObj, name, MAX_NAME_LEN);
}

//=============================================================================
// ActionEventLocatorNode::GetNewObj
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const ActionEventLocatorNode::GetNewObj()
{
    return sNewObj;
}

//=============================================================================
// ActionEventLocatorNode::SetNewJoint
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void ActionEventLocatorNode::SetNewJoint( const char* name )
{
    strncpy( sNewJoint, name, MAX_NAME_LEN);
}

//=============================================================================
// ActionEventLocatorNode::GetNewJoint
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const
//
//=============================================================================
inline const char* const ActionEventLocatorNode::GetNewJoint()
{
    return sNewJoint;
}

#endif //ACTIONEVENTLOCATORNODE_H
