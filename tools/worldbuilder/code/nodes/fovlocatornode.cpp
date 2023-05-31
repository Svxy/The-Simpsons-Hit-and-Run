//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        FOVLocatorNode.cpp
//
// Description: Implement FOVLocatorNode
//
// History:     03/08/2002 + Created -- Cary Brisebois
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
#include "nodes/FOVLocatorNode.h"

#include "main/constants.h"
#include "main/worldbuilder.h"
#include "utility/glext.h"
#include "utility/mext.h"
#include "utility/nodehelper.h"
#include "nodes/triggervolumenode.h"

#include "resources/resource.h"

#include "../../../game/code/meta/locatortypes.h"


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
MTypeId FOVLocatorNode::id( WBConstants::TypeIDPrefix, WBConstants::NodeIDs::FOVLocator );
const char* FOVLocatorNode::stringId = "FOVLocatorNode";

const char* FOVLocatorNode::TRIGGERS_NAME_SHORT   = "trigs";
const char* FOVLocatorNode::TRIGGERS_NAME_LONG    = "triggers";
MObject FOVLocatorNode::sTriggers;

const char*  FOVLocatorNode::FOV_NAME_SHORT = "f";
const char*  FOVLocatorNode::FOV_NAME_LONG = "fov";
MObject FOVLocatorNode::sFOV;

const char*  FOVLocatorNode::TIME_NAME_SHORT = "tm";
const char*  FOVLocatorNode::TIME_NAME_LONG = "time";
MObject FOVLocatorNode::sTime;

const char*  FOVLocatorNode::RATE_NAME_SHORT = "ra";
const char*  FOVLocatorNode::RATE_NAME_LONG = "rate";
MObject FOVLocatorNode::sRate;

const int FOVLocatorNode::ACTIVE_COLOUR = 15;
const int FOVLocatorNode::INACTIVE_COLOUR = 12;
const float FOVLocatorNode::SCALE = 1.0f * WBConstants::Scale;

char FOVLocatorNode::sNewName[MAX_NAME_LEN];

//******************************************************************************
//
// Callbacks
//
//******************************************************************************

BOOL CALLBACK FOVLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        {
            SetDlgItemText( hWnd, IDC_EDIT2, WorldBuilder::GetPrefix() );
            return true;
        }
        break;
    case WM_COMMAND:
        {
            if ( LOWORD(wParam) == IDC_BUTTON1 || LOWORD(wParam) == IDOK )
            {
                //Get the entry in the text field.
                char name[FOVLocatorNode::MAX_NAME_LEN];
                GetDlgItemText( hWnd, IDC_EDIT1, name, FOVLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a new name for the FOV Locator!");
                    return false;
                }

                MString newName( WorldBuilder::GetPrefix() );
                newName += MString( name );
                
                FOVLocatorNode::SetNewName( newName.asChar() );
                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if( LOWORD(wParam) == IDCANCEL )
            {
                FOVLocatorNode::SetNewName( "" );
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
// FOVLocatorNode::FOVLocatorNode
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
FOVLocatorNode::FOVLocatorNode()
{
}

//==============================================================================
// FOVLocatorNode::~FOVLocatorNode
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
FOVLocatorNode::~FOVLocatorNode()
{
}

//=============================================================================
// FOVLocatorNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* FOVLocatorNode::creator()
{
    return new FOVLocatorNode();
}

//=============================================================================
// FOVLocatorNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void FOVLocatorNode::draw( M3dView& view, 
                             const MDagPath& path, 
                             M3dView::DisplayStyle displayStyle, 
                             M3dView::DisplayStatus displayStatus )
{
    if ( WorldBuilder::GetDisplayLevel() & WorldBuilder::CARSTART_LOCATORS )
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

        glPopAttrib();
        view.endGL();
    }
}

//=============================================================================
// FOVLocatorNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus FOVLocatorNode::initialize()
{
    MStatus status;

    MFnMessageAttribute msgAttr;
    sTriggers = msgAttr.create( TRIGGERS_NAME_LONG, TRIGGERS_NAME_SHORT, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( msgAttr.setReadable( false ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setArray( true ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setIndexMatters( false ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( sTriggers ) );

    MFnNumericAttribute numAttr;
    sFOV = numAttr.create( FOV_NAME_LONG, FOV_NAME_SHORT, MFnNumericData::kFloat, 90.0f, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setMin(0.1f) );
    RETURN_STATUS_ON_FAILURE( numAttr.setMax(180.0f) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sFOV ) );

    sTime = numAttr.create( TIME_NAME_LONG, TIME_NAME_SHORT, MFnNumericData::kFloat, 2.0f, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setMin(0.0f) );
    RETURN_STATUS_ON_FAILURE( numAttr.setMax(10.0f) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sTime ) );

    sRate = numAttr.create( RATE_NAME_LONG, RATE_NAME_SHORT, MFnNumericData::kFloat, 0.04f, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setMin(0.0f) );
    RETURN_STATUS_ON_FAILURE( numAttr.setMax(1.0f) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sRate ) );

    return MStatus::kSuccess;
}

//=============================================================================
// FOVLocatorNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FOVLocatorNode::postConstructor()
{
}

//=============================================================================
// FOVLocatorNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& FOVLocatorNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* FOVLocatorNode::Export( MObject& fovLocatorNode )
{
    MFnDagNode fnNode( fovLocatorNode );

    if ( fnNode.typeId() == FOVLocatorNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlWBLocatorChunk* locator = new tlWBLocatorChunk;

        locator->SetName( fnNode.name().asChar() );

        locator->SetType( LocatorType::FOV );

        float fov = 90.0f;
        float time = 2.0f;
        float rate = 0.04f;

        fnNode.findPlug( sFOV ).getValue( fov );
        fnNode.findPlug( sTime ).getValue( time );
        fnNode.findPlug( sRate ).getValue( rate );

        unsigned long data[3];
        memcpy( data, &fov, sizeof(float) );
        memcpy( &(data[1]), &time, sizeof(float) );
        memcpy( &(data[2]), &rate, sizeof(float) );

        locator->SetDataElements( data, 3 );
        locator->SetNumDataElements( 3 );

        MPoint thisPosition;
        MExt::GetWorldPosition( &thisPosition, fovLocatorNode );

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
