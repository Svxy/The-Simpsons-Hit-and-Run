//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ActionEventLocatorNode.cpp
//
// Description: Implement ActionEventLocatorNode
//
// History:     30/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <afxwin.h>

#include "main/toolhack.h"
#include <toollib.hpp>

//========================================
// Project Includes
//========================================
#include "nodes/ActionEventLocatorNode.h"
#include "main/constants.h"
#include "main/worldbuilder.h"
#include "utility/glext.h"
#include "utility/mext.h"
#include "utility/nodehelper.h"
#include "utility/transformmatrix.h"
#include "nodes/triggervolumenode.h"

#include "resources/resource.h"

#include "../../../game/code/meta/locatortypes.h"
#include "../../../game/code/ai/actionnames.h"
#include "../../../game/code/worldsim/character/charactercontroller.h"

int SortFunc( const void* pName1, const void* pName2 )
{
    return stricmp( *(char**)pName1, *(char**)pName2 );
}

void UpdateJoints( HWND hWnd, MObject& root )
{
    CWnd* wnd = CWnd::FromHandle( GetDlgItem( hWnd, IDC_COMBO2 ) );
    CComboBox* jointList = (CComboBox*)( wnd );

    assert( jointList );

    jointList->ResetContent();

    MObjectArray jointArray;
    if ( MExt::FindAllTransforms( &jointArray, root ) )
    {
        unsigned int i;
        for ( i = 0; i < jointArray.length(); ++i )
        {
            MFnDependencyNode fnNode( jointArray[i] );

            jointList->AddString( fnNode.name().asChar() );
        }
    }
    else
    {
        jointList->AddString( "NO JOINTS!" );
    }

    jointList->SetCurSel( 0 );
}

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
MTypeId ActionEventLocatorNode::id( WBConstants::TypeIDPrefix, WBConstants::NodeIDs::ActionEventLocator );
const char* ActionEventLocatorNode::stringId = "ActionEventLocatorNode";

const int ActionEventLocatorNode::ACTIVE_COLOUR = 15;
const int ActionEventLocatorNode::INACTIVE_COLOUR = 12;
const float ActionEventLocatorNode::SCALE = 1.0f * WBConstants::Scale;

const char* ActionEventLocatorNode::TRIGGERS_NAME_SHORT   = "trigs";
const char* ActionEventLocatorNode::TRIGGERS_NAME_LONG    = "triggers";
MObject ActionEventLocatorNode::sTriggers;

const char*  ActionEventLocatorNode::OBJECT_NAME_SHORT = "acnobjt";
const char*  ActionEventLocatorNode::OBJECT_NAME_LONG = "actionObject";
MObject ActionEventLocatorNode::sObject;

const char*  ActionEventLocatorNode::JOINT_NAME_SHORT = "jnt";
const char*  ActionEventLocatorNode::JOINT_NAME_LONG = "joint";
MObject ActionEventLocatorNode::sJoint;

const char*  ActionEventLocatorNode::ACTION_NAME_SHORT = "actn";
const char*  ActionEventLocatorNode::ACTION_NAME_LONG = "action";
MObject ActionEventLocatorNode::sActionType;

const char*  ActionEventLocatorNode::BUTTON_NAME_SHORT = "btninpt";
const char*  ActionEventLocatorNode::BUTTON_NAME_LONG = "buttonInput";
MObject ActionEventLocatorNode::sButtonInput;

const char*  ActionEventLocatorNode::TRANSFORM_NAME_SHORT = "st";
const char*  ActionEventLocatorNode::TRANSFORM_NAME_LONG = "shouldTransform";
MObject ActionEventLocatorNode::sTransform;

const char*  ActionEventLocatorNode::EXPORT_TRANSFORM_NAME_SHORT = "exptTrans";
const char*  ActionEventLocatorNode::EXPORT_TRANSFORM_NAME_LONG = "exportTransform";
MObject ActionEventLocatorNode::sExportTransform;

char ActionEventLocatorNode::sNewName[MAX_NAME_LEN];
char ActionEventLocatorNode::sNewObj[MAX_NAME_LEN];
char ActionEventLocatorNode::sNewJoint[MAX_NAME_LEN];

const char* ActionEventLocatorNode::names[ActionButton::ActionNameSize];

//******************************************************************************
//
// Callbacks
//
//******************************************************************************

BOOL CALLBACK ActionEventLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        {
            SetDlgItemText( hWnd, IDC_EDIT2, WorldBuilder::GetPrefix() );
            SetDlgItemText( hWnd, IDC_EDIT3, "" );

            CWnd* wnd = CWnd::FromHandle( GetDlgItem( hWnd, IDC_COMBO1 ) );
            CComboBox* objectList = (CComboBox*)( wnd );
           
            assert( objectList );

            objectList->ResetContent();

            bool foundSkeletonRoots = false;
            MObjectArray skeletonRoots;

            if ( MExt::FindAllSkeletonRoots( &skeletonRoots ) )
            {
                //Fill up with all the roots.
                unsigned int i;
                for ( i = 0; i < skeletonRoots.length(); ++i )
                {
                    MFnDependencyNode fnNode( skeletonRoots[i] );

                    objectList->AddString( fnNode.name().asChar() );
                }

                foundSkeletonRoots = true;
            }
            else
            {
                objectList->AddString( "NO SKELETON ROOTS!" );
            }

            objectList->SetCurSel( 0 );

            char objName[256];
            GetDlgItemText( hWnd, IDC_COMBO1, objName, 256 );
            
            MDagPath path;
            MExt::FindDagNodeByName( &path, MString( objName ) );

            UpdateJoints( hWnd, path.node() );

            return true;
        }
        break;
    case WM_COMMAND:
        {
            if ( LOWORD(wParam) == IDC_BUTTON1 || LOWORD(wParam) == IDOK )
            {
                //Get the entry in the text field.
                char name[ActionEventLocatorNode::MAX_NAME_LEN];
                GetDlgItemText( hWnd, IDC_EDIT1, name, ActionEventLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a new name for the Action Event Locator!");
                    return false;
                }
                
                MString newName( WorldBuilder::GetPrefix() );
                newName += MString( name );

                ActionEventLocatorNode::SetNewName( newName.asChar() );

                //Do the Object name too...
                GetDlgItemText( hWnd, IDC_COMBO1, name, ActionEventLocatorNode::MAX_NAME_LEN );

                MString possibleName( name );
                MString corrected = possibleName;
                int index = possibleName.index( ':' );
                if ( index != -1 )
                {
                    corrected = possibleName.substring( index + 1, possibleName.length() - 1 );
                }
                else

                if ( strcmp(corrected.asChar(), "") == 0 )
                {
                    MExt::DisplayWarning("You must input an object name!");
                    return false;
                }

                ActionEventLocatorNode::SetNewObj( corrected.asChar() );

                //Do the Object name too...
                GetDlgItemText( hWnd, IDC_COMBO2, name, ActionEventLocatorNode::MAX_NAME_LEN );

                possibleName = MString( name );
                corrected = possibleName;
                index = possibleName.index( ':' );
                if ( index != -1 )
                {
                    corrected = possibleName.substring( index + 1, possibleName.length() - 1 );
                }

                if ( strcmp(corrected.asChar(), "") == 0 )
                {
                    MExt::DisplayWarning("You must input a joint name!");
                    return false;
                }

                ActionEventLocatorNode::SetNewJoint( corrected.asChar() );

                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if( LOWORD(wParam) == IDCANCEL )
            {
                ActionEventLocatorNode::SetNewName( "" );
                ActionEventLocatorNode::SetNewObj( "" );
                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if ( LOWORD(wParam) == IDC_COMBO1 )
            {
                DWORD hiWord = HIWORD(wParam);

                if ( hiWord == CBN_SELCHANGE )
                {
                    char objName[256];
                    GetDlgItemText( hWnd, IDC_COMBO1, objName, 256 );
            
                    MDagPath path;
                    MExt::FindDagNodeByName( &path, MString( objName ) );

                    UpdateJoints( hWnd, path.node() );
                }
            }

            return false;
        }
        break;
    default:
        {
            return false;
        }
        break;
    }
}

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// ActionEventLocatorNode::ActionEventLocatorNode
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ActionEventLocatorNode::ActionEventLocatorNode()
{
    unsigned int i;
    for ( i = 0; i < MAX_NAME_LEN; ++i )
    {
        sNewJoint[i] = '\0';
    }
}

//==============================================================================
// ActionEventLocatorNode::~ActionEventLocatorNode
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ActionEventLocatorNode::~ActionEventLocatorNode()
{
}

//=============================================================================
// ActionEventLocatorNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* ActionEventLocatorNode::creator()
{
    return new ActionEventLocatorNode();
}

//=============================================================================
// ActionEventLocatorNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void ActionEventLocatorNode::draw( M3dView& view, 
                             const MDagPath& path, 
                             M3dView::DisplayStyle displayStyle, 
                             M3dView::DisplayStatus displayStatus )
{
    if ( WorldBuilder::GetDisplayLevel() & WorldBuilder::ACTION_EVENT_LOCATORS )
    {
        view.beginGL();
        glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );

        //When we are in render mode, we draw the lines between the nodes.
        //If this was in GL_SELECTION_MODE, we would not draw the lines, so they won't interfere
        //with selection.
        GLint value;
        glGetIntegerv( GL_RENDER_MODE, &value );

        //Draw things here we don't want selectable.
        if ( (value == GL_RENDER) )
        {
            MPlugArray sources, targets;
            MFnDagNode fnNode( thisMObject() );
            MPlug triggersPlug = fnNode.findPlug( sTriggers );
            MExt::ResolveConnections( &sources, &targets, triggersPlug, true, false );

            unsigned int i;
            for ( i = 0; i < sources.length(); ++i )
            {
                //Draw a box around the source trigger volume.
                MPoint triggerWP, thisWP;

                MExt::GetWorldPosition( &triggerWP, sources[i].node() );

                //MExt::GetWorldPosition( &thisWP, thisMObject() );
                MMatrix mat = MExt::GetWorldMatrix( thisMObject() );
                MMatrix invMat = mat.inverse();

                MPoint triggerLP;
                //triggerLP = triggerWP - thisWP;
                triggerLP = triggerWP * invMat;

                view.setDrawColor( 8, M3dView::kActiveColors );

                GLExt::drawLine( MPoint(0,0,0), triggerLP, 5.0f );                
            }
         }

        if ( displayStatus == M3dView::kDormant ) 
        {
            int colour = NodeHelper::OverrideNodeColour( thisMObject(), INACTIVE_COLOUR );

	        view.setDrawColor( colour, M3dView::kDormantColors );
        }  
        else
        {
	        view.setDrawColor( ACTIVE_COLOUR, M3dView::kActiveColors );
        }

        //Draw a star to represent the locator.
        GLExt::drawCrossHair3D( SCALE, 0,0,0, 5.0 );
        GLExt::drawPyramid( SCALE, 0,0,0, 5.0 );
        GLExt::drawA( SCALE, 0,1,0, 5.0 );
        GLExt::drawArrow( MPoint( 0, 0, 0 ), MPoint( 0, 0, -1 * WBConstants::Scale ), 5.0 );

        glPopAttrib();
        view.endGL();
    }
}

//=============================================================================
// ActionEventLocatorNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus ActionEventLocatorNode::initialize()
{
    MFnMessageAttribute msgAttr;
    MStatus status;

    sTriggers = msgAttr.create( TRIGGERS_NAME_LONG, TRIGGERS_NAME_SHORT, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( msgAttr.setReadable( false ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setArray( true ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setIndexMatters( false ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( sTriggers ) );

    MFnTypedAttribute typedAttr;
    sObject = typedAttr.create( OBJECT_NAME_LONG, OBJECT_NAME_SHORT, MFnData::kString, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( typedAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( typedAttr.setWritable( true ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( sObject ) );

    sJoint = typedAttr.create( JOINT_NAME_LONG, JOINT_NAME_SHORT, MFnData::kString, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( typedAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( typedAttr.setWritable( true ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( sJoint ) );

    MFnEnumAttribute enumAttr;
    sActionType = enumAttr.create( ACTION_NAME_LONG, ACTION_NAME_SHORT, 0, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( enumAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( enumAttr.setWritable( true ) );

    unsigned int i;
    for ( i = 0; i < ActionButton::ActionNameSize; ++i )
    {
        names[i] = ActionButton::ActionName[i];
    }

    // Sorting seems to make a mess of things.
    // commenting out for now.
    // TBJ [8/9/2002] 
    //
    //qsort( names, ActionButton::ActionNameSize, sizeof( char* ), SortFunc );    

    for ( i = 0; i < ActionButton::ActionNameSize; ++i ) 
    {
        RETURN_STATUS_ON_FAILURE( enumAttr.addField( MString( names[i] ), i ) );
    }

    RETURN_STATUS_ON_FAILURE( addAttribute( sActionType ) );

    sButtonInput = enumAttr.create( BUTTON_NAME_LONG, BUTTON_NAME_SHORT, CharacterController::DoAction, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( enumAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( enumAttr.setWritable( true ) );
    for ( i = 0; i < ActionButton::ButtonNameListSize; ++i )
    {
        RETURN_STATUS_ON_FAILURE( enumAttr.addField( MString( ActionButton::ButtonName[i] ), i ) );
    }

    RETURN_STATUS_ON_FAILURE( addAttribute( sButtonInput ) );

    MFnNumericAttribute numericAttr;
    sTransform = numericAttr.create( TRANSFORM_NAME_LONG, TRANSFORM_NAME_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sTransform ) );
    
    sExportTransform = numericAttr.create( EXPORT_TRANSFORM_NAME_LONG, EXPORT_TRANSFORM_NAME_SHORT, MFnNumericData::kBoolean, true, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sExportTransform ) );

    return MStatus::kSuccess;
}

//=============================================================================
// ActionEventLocatorNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ActionEventLocatorNode::postConstructor()
{
}

//=============================================================================
// ActionEventLocatorNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& actionEventLocatorNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* ActionEventLocatorNode::Export( MObject& actionEventLocatorNode )
{
    MFnDagNode fnNode( actionEventLocatorNode );

    if ( fnNode.typeId() == ActionEventLocatorNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlWBLocatorChunk* locator = new tlWBLocatorChunk;

        locator->SetName( fnNode.name().asChar() );

        locator->SetType( LocatorType::ACTION );

        MString objName;
        MString jointName;
        int actionInt;
        MString actionName;
        int button;
        bool shouldTransform;

        //Get the data
        fnNode.findPlug( ActionEventLocatorNode::sObject ).getValue( objName );
        if ( objName.length() == 0 )
        {
            objName.set( "<null>" );
        }
        
        fnNode.findPlug( ActionEventLocatorNode::sJoint ).getValue( jointName );
        if ( jointName.length() == 0 )
        {
            jointName.set( "<null>" );
        }
        fnNode.findPlug( ActionEventLocatorNode::sActionType ).getValue( actionInt );

        actionName = MString( names[actionInt] );

        fnNode.findPlug( ActionEventLocatorNode::sButtonInput ).getValue( button );
        fnNode.findPlug( ActionEventLocatorNode::sTransform ).getValue( shouldTransform );

        //Save it out.
        unsigned int length = (objName.length() / 4 + 1) + 
                              (jointName.length() / 4 + 1) + 
                              (actionName.length() / 4 + 1) + 
                              2; //button and shouldTransform

        unsigned long* data = new unsigned long[length];

        unsigned int i;
        for ( i = 0; i < length; ++i )
        {
            data[i] = 0;
        }
        
        unsigned int next = 0;
        memcpy( data, objName.asChar(), objName.length() );
        next = objName.length() / 4 + 1;

        memcpy( &data[next], jointName.asChar(), jointName.length() );
        next += jointName.length() / 4 + 1;

        memcpy( &data[next], actionName.asChar(), actionName.length() );
        next += actionName.length() / 4 + 1;

        memcpy( &data[next], &button, sizeof(int) );
        next++;

        memcpy( &data[next], &shouldTransform, sizeof(bool) );

        locator->SetDataElements( data, length );
        locator->SetNumDataElements( length );

        delete data;

        MPoint thisPosition;
        MExt::GetWorldPosition( &thisPosition, actionEventLocatorNode );

       //Set the values.
        tlPoint point;

        point[0] = thisPosition[0] / WBConstants::Scale;
        point[1] = thisPosition[1] / WBConstants::Scale;
        point[2] = -thisPosition[2] / WBConstants::Scale;  //Maya vs. P3D...
        locator->SetPosition( point );

        //Make the trigger volumes a sub-chunk of the locator...
        MPlugArray sources, targets;
        MPlug triggersPlug = fnNode.findPlug( sTriggers );
        MExt::ResolveConnections( &sources, &targets, triggersPlug, true, false );

        for ( i = 0; i < sources.length(); ++i )
        {
            tlDataChunk* trigger = TriggerVolumeNode::Export( sources[ i ].node() );
            assert( trigger );

            locator->AppendSubChunk( trigger );
        }

        locator->SetNumTriggers( i );

        //Add the matrix if we're supposed to.
        bool exportTrans = false;
        fnNode.findPlug( sExportTransform ).getValue( exportTrans );
        if ( exportTrans )
        {
            //Also get the direction.
            MObject transform;
            transform = fnNode.parent( 0 );
            MFnTransform fnTransform( transform );

            MDagPath dagPath;
            MExt::FindDagNodeByName( &dagPath, fnTransform.name() );
            TransformMatrix tm( dagPath );

            tlMatrix hmatrix;
            tm.GetHierarchyMatrixLHS( hmatrix );
            //Make this p3d friendly...
            hmatrix.element[3][0] /= WBConstants::Scale;
            hmatrix.element[3][1] /= WBConstants::Scale;
            hmatrix.element[3][2] /= WBConstants::Scale;
            
            tlExtraMatrixChunk* emChunk = new tlExtraMatrixChunk();
            emChunk->SetMatrix( hmatrix );

            locator->AppendSubChunk( emChunk );
        }
 
        return locator;
    }    

    return NULL;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
