//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        PedGroupLocatorNode.cpp
//
// Description: Implement PedGroupLocatorNode
//
// History:     29/07/2002 + Created -- Cary Brisebois
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
#include "nodes/PedGroupLocator.h"
#include "main/constants.h"
#include "main/worldbuilder.h"
#include "utility/glext.h"
#include "utility/mext.h"
#include "utility/nodehelper.h"
#include "utility/transformmatrix.h"
#include "nodes/triggervolumenode.h"

#include "resources/resource.h"

#include "../../../game/code/meta/locatortypes.h"



//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
MTypeId PedGroupLocatorNode::id( WBConstants::TypeIDPrefix, WBConstants::NodeIDs::PedGroupLocator );
const char* PedGroupLocatorNode::stringId = "PedGroupLocatorNode";

const int PedGroupLocatorNode::ACTIVE_COLOUR = 15;
const int PedGroupLocatorNode::INACTIVE_COLOUR = 12;
const float PedGroupLocatorNode::SCALE = 1.0f * WBConstants::Scale;

const char* PedGroupLocatorNode::TRIGGERS_NAME_SHORT   = "trigs";
const char* PedGroupLocatorNode::TRIGGERS_NAME_LONG    = "triggers";
MObject PedGroupLocatorNode::sTriggers;

const char* PedGroupLocatorNode::GROUP_NAME_SHORT = "grp";
const char* PedGroupLocatorNode::GROUP_NAME_LONG = "group";
MObject PedGroupLocatorNode::sGroup;

char PedGroupLocatorNode::sNewName[MAX_NAME_LEN];

//******************************************************************************
//
// Callbacks
//
//******************************************************************************

BOOL CALLBACK PedGroupLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
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
                char name[PedGroupLocatorNode::MAX_NAME_LEN];
                GetDlgItemText( hWnd, IDC_EDIT1, name, PedGroupLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a new name for the Ped Group Locator!");
                    return false;
                }

                MString newName( WorldBuilder::GetPrefix() );
                newName += MString( name );
                
                PedGroupLocatorNode::SetNewName( newName.asChar() );
                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if( LOWORD(wParam) == IDCANCEL )
            {
                PedGroupLocatorNode::SetNewName( "" );
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
// PedGroupLocatorNode::PedGroupLocatorNode
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
PedGroupLocatorNode::PedGroupLocatorNode()
{
}

//==============================================================================
// PedGroupLocatorNode::~PedGroupLocatorNode
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
PedGroupLocatorNode::~PedGroupLocatorNode()
{
}

//=============================================================================
// PedGroupLocatorNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* PedGroupLocatorNode::creator()
{
    return new PedGroupLocatorNode();
}

//=============================================================================
// PedGroupLocatorNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void PedGroupLocatorNode::draw( M3dView& view, 
                             const MDagPath& path, 
                             M3dView::DisplayStyle displayStyle, 
                             M3dView::DisplayStatus displayStatus )
{
    if ( WorldBuilder::GetDisplayLevel() & WorldBuilder::DIRECTIONAL_LOCATORS )
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

            //Need to undo funny transformations...
            MMatrix mat;
            mat.setToIdentity();

            MObject transform;
            transform = fnNode.parent( 0 );
            MFnDependencyNode transNode( transform );
            MDagPath transPath;
            MExt::FindDagNodeByName( &transPath, transNode.name() );
            MFnTransform fnTransform( transPath );

            mat = fnTransform.transformationMatrix();

            mat = mat.inverse();

            MPoint triggerWP, thisWP;
            MExt::GetWorldPosition( &thisWP, thisMObject() );
            
            unsigned int i;
            for ( i = 0; i < targets.length(); ++i )
            {
                //Draw a box around the source trigger volume.
                MExt::GetWorldPosition( &triggerWP, sources[i].node() );

                MPoint triggerLP;
                triggerLP = triggerWP; // - thisWP;

                triggerLP *= mat;

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
        GLExt::drawStickMan( SCALE, 0, 1, 0, 5.0 );
        GLExt::drawStickMan( SCALE, 0.25, 1, 0, 5.0 );
        GLExt::drawStickMan( SCALE, -0.25, 1, 0, 5.0 );

        glPopAttrib();
        view.endGL();
    }
}

//=============================================================================
// PedGroupLocatorNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus PedGroupLocatorNode::initialize()
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

    MFnNumericAttribute numericAttr;
    sGroup = numericAttr.create( GROUP_NAME_LONG, GROUP_NAME_SHORT, MFnNumericData::kInt, 0, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( false ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( sGroup ) );

    return MStatus::kSuccess;
}

//=============================================================================
// PedGroupLocatorNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PedGroupLocatorNode::postConstructor()
{
}

//=============================================================================
// PedGroupLocatorNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& directionalLocatorNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* PedGroupLocatorNode::Export( MObject& pedGroupLocatorNode )
{
    MFnDagNode fnNode( pedGroupLocatorNode );

    if ( fnNode.typeId() == PedGroupLocatorNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlWBLocatorChunk* locator = new tlWBLocatorChunk;

        locator->SetName( fnNode.name().asChar() );

        locator->SetType( LocatorType::PED_GROUP );

        //The data here is the event associated with this locator.
        int group;
        fnNode.findPlug( sGroup ).getValue( group );

        unsigned long data = group;
        locator->SetDataElements( &data, 1 );
        locator->SetNumDataElements( 1 );

        MPoint thisPosition;
        MExt::GetWorldPosition( &thisPosition, pedGroupLocatorNode );

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
