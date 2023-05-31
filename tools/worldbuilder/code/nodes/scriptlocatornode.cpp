//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ScriptLocatorNode.cpp
//
// Description: Implement ScriptLocatorNode
//
// History:     27/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
#include "main/toolhack.h"
#include <toollib.hpp>

//========================================
// Project Includes
//========================================
#include "nodes/ScriptLocatorNode.h"
#include "main/constants.h"
#include "main/worldbuilder.h"
#include "utility/glext.h"
#include "utility/mext.h"
#include "utility/nodehelper.h"

#include "nodes/triggervolumenode.h"

#include "resources/resource.h"

#include "../../../game/code/meta/locatorevents.h"
#include "../../../game/code/meta/locatortypes.h"

#define HIINT(x) (((int)((x)>>32) & 0xFFFFFFFF))
#define LOINT(x) ((int)x)

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
MTypeId ScriptLocatorNode::id( WBConstants::TypeIDPrefix, WBConstants::NodeIDs::ScriptLocator );
const char* ScriptLocatorNode::stringId = "ScriptLocatorNode";

const int ScriptLocatorNode::ACTIVE_COLOUR = 15;
const int ScriptLocatorNode::INACTIVE_COLOUR = 12;
const float ScriptLocatorNode::SCALE = 1.0f * WBConstants::Scale;

const char* ScriptLocatorNode::TRIGGERS_NAME_SHORT   = "trigs";
const char* ScriptLocatorNode::TRIGGERS_NAME_LONG    = "triggers";
MObject ScriptLocatorNode::sTriggers;

const char* ScriptLocatorNode::SCRIPT_NAME_SHORT = "s";
const char* ScriptLocatorNode::SCRIPT_NAME_LONG = "script";
MObject ScriptLocatorNode::sScript;

char ScriptLocatorNode::sNewName[MAX_NAME_LEN];
char ScriptLocatorNode::sScriptName[MAX_NAME_LEN];

//******************************************************************************
//
// Callbacks
//
//******************************************************************************

BOOL CALLBACK ScriptLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        {
            SetDlgItemText( hWnd, IDC_EDIT2, WorldBuilder::GetPrefix() );
            SetDlgItemText( hWnd, IDC_EDIT3, "" );
            return true;
        }
        break;
    case WM_COMMAND:
        {
            if ( LOWORD(wParam) == IDC_BUTTON1 || LOWORD(wParam) == IDOK )
            {
                //Get the entry in the text field.
                char name[ScriptLocatorNode::MAX_NAME_LEN];
                GetDlgItemText( hWnd, IDC_EDIT1, name, ScriptLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a new name for the Script Locator!");
                    return false;
                }
                
                MString newName( WorldBuilder::GetPrefix() );
                newName += MString( name );

                ScriptLocatorNode::SetNewName( newName.asChar() );

                //Do the scripte name too...
                GetDlgItemText( hWnd, IDC_EDIT3, name, ScriptLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a script name!");
                    return false;
                }

                ScriptLocatorNode::SetScriptName( name );

                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if( LOWORD(wParam) == IDCANCEL )
            {
                ScriptLocatorNode::SetNewName( "" );
                ScriptLocatorNode::SetScriptName( "" );
                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
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
// ScriptLocatorNode::ScriptLocatorNode
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ScriptLocatorNode::ScriptLocatorNode()
{
}

//==============================================================================
// ScriptLocatorNode::~ScriptLocatorNode
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ScriptLocatorNode::~ScriptLocatorNode()
{
}

//=============================================================================
// ScriptLocatorNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* ScriptLocatorNode::creator()
{
    return new ScriptLocatorNode();
}

//=============================================================================
// ScriptLocatorNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void ScriptLocatorNode::draw( M3dView& view, 
                             const MDagPath& path, 
                             M3dView::DisplayStyle displayStyle, 
                             M3dView::DisplayStatus displayStatus )
{
    if ( WorldBuilder::GetDisplayLevel() & WorldBuilder::SCRIPT_LOCATORS )
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
            for ( i = 0; i < targets.length(); ++i )
            {
                //Draw a box around the source trigger volume.
                MPoint triggerWP, thisWP;
                MExt::GetWorldPosition( &triggerWP, sources[i].node() );

                MExt::GetWorldPosition( &thisWP, thisMObject() );

                MPoint triggerLP;
                triggerLP = triggerWP - thisWP;

                //TEMP
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
        GLExt::drawS( SCALE, 0,1,0, 5.0 );

        glPopAttrib();
        view.endGL();
    }
}

//=============================================================================
// ScriptLocatorNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus ScriptLocatorNode::initialize()
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
    sScript = typedAttr.create( SCRIPT_NAME_LONG, SCRIPT_NAME_SHORT, MFnData::kString, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( typedAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( typedAttr.setWritable( true ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( sScript ) );

    return MStatus::kSuccess;
}

//=============================================================================
// ScriptLocatorNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ScriptLocatorNode::postConstructor()
{
}

//=============================================================================
// ScriptLocatorNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& ScriptLocatorNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* ScriptLocatorNode::Export( MObject& scriptLocatorNode )
{
    MFnDagNode fnNode( scriptLocatorNode );

    if ( fnNode.typeId() == ScriptLocatorNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlWBLocatorChunk* locator = new tlWBLocatorChunk;

        locator->SetName( fnNode.name().asChar() );

        locator->SetType( LocatorType::SCRIPT );

        //The data here is the event associated with this locator.
        MString script;
        fnNode.findPlug( sScript ).getValue( script );

        unsigned int length = (script.length() / 4 + 1);

        unsigned long* data = new unsigned long[length];
        memset(data, 0, length * 4);
        memcpy( data, script.asChar(), script.length() );

        locator->SetDataElements( data, length );
        locator->SetNumDataElements( length );

        delete[] data;

        MPoint thisPosition;
        MExt::GetWorldPosition( &thisPosition, scriptLocatorNode );

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

        unsigned int i;
        for ( i = 0; i < sources.length(); ++i )
        {
            tlDataChunk* trigger = TriggerVolumeNode::Export( sources[ i ].node() );
            assert( trigger );

            locator->AppendSubChunk( trigger );
        }
 
        locator->SetNumTriggers( i );

        return locator;
    }    

    return NULL;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
