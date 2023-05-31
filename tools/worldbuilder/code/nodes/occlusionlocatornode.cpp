//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        OcclusionLocatorNode.cpp
//
// Description: Implement OcclusionLocatorNode
//
// History:     28/06/2002 + Created -- Cary Brisebois 
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
#include "nodes/OcclusionLocatorNode.h"
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
MTypeId OcclusionLocatorNode::id( WBConstants::TypeIDPrefix, WBConstants::NodeIDs::OcclusionLocator );
const char* OcclusionLocatorNode::stringId = "OcclusionLocatorNode";

const int OcclusionLocatorNode::ACTIVE_COLOUR = 15;
const int OcclusionLocatorNode::INACTIVE_COLOUR = 12;
const float OcclusionLocatorNode::SCALE = 1.0f * WBConstants::Scale;

const char* OcclusionLocatorNode::TRIGGERS_NAME_SHORT   = "trigs";
const char* OcclusionLocatorNode::TRIGGERS_NAME_LONG    = "triggers";
MObject OcclusionLocatorNode::sTriggers;

char OcclusionLocatorNode::sNewName[MAX_NAME_LEN];

//******************************************************************************
//
// Callbacks
//
//******************************************************************************

BOOL CALLBACK OcclusionLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
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
                char name[OcclusionLocatorNode::MAX_NAME_LEN];
                GetDlgItemText( hWnd, IDC_EDIT1, name, OcclusionLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a new name for the Occlusion Locator!");
                    return false;
                }
                
                MString newName( WorldBuilder::GetPrefix() );
                newName += MString( name );

                OcclusionLocatorNode::SetNewName( newName.asChar() );

                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if( LOWORD(wParam) == IDCANCEL )
            {
                OcclusionLocatorNode::SetNewName( "" );
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
// OcclusionLocatorNode::OcclusionLocatorNode
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
OcclusionLocatorNode::OcclusionLocatorNode()
{
}

//==============================================================================
// OcclusionLocatorNode::~OcclusionLocatorNode
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
OcclusionLocatorNode::~OcclusionLocatorNode()
{
}

//=============================================================================
// OcclusionLocatorNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* OcclusionLocatorNode::creator()
{
    return new OcclusionLocatorNode();
}

//=============================================================================
// OcclusionLocatorNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void OcclusionLocatorNode::draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
{
    if ( WorldBuilder::GetDisplayLevel() & WorldBuilder::OCCLUSION_LOCATORS )
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

                if ( i == 0 )
                {
                    //This is the triggering volume
                    view.setDrawColor( 3, M3dView::kActiveColors );
                }
                else
                {
                    //These are the occluding volumes.
                    view.setDrawColor( 8, M3dView::kActiveColors );
                }

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
        GLExt::drawO( SCALE, 0,1,0, 5.0 );

        glPopAttrib();
        view.endGL();
    }
}

//=============================================================================
// OcclusionLocatorNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus OcclusionLocatorNode::initialize()
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

    return MStatus::kSuccess;
}

//=============================================================================
// OcclusionLocatorNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void OcclusionLocatorNode::postConstructor()
{
}

//=============================================================================
// OcclusionLocatorNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& occlusionLocatorNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* OcclusionLocatorNode::Export( MObject& occlusionLocatorNode )
{
    MFnDagNode fnNode( occlusionLocatorNode );

    if ( fnNode.typeId() == OcclusionLocatorNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlWBLocatorChunk* locator = new tlWBLocatorChunk;

        locator->SetName( fnNode.name().asChar() );

        locator->SetType( LocatorType::OCCLUSION );

        locator->SetNumDataElements( 0 );

        MPoint thisPosition;
        MExt::GetWorldPosition( &thisPosition, occlusionLocatorNode );

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


        //Hold onto the visiblers until the end.
        tlDataChunk* visiblers = new tlDataChunk;
        unsigned long numVisiblers = 0;
        unsigned long numRegular = 0;

        unsigned int i;
        for ( i = 0; i < sources.length(); ++i )
        {
            tlDataChunk* trigger = TriggerVolumeNode::Export( sources[ i ].node() );
            assert( trigger );

            MFnDagNode fnTrigNode( sources[ i ].node() );

            if ( fnTrigNode.name().substring( 0, 0 ) == MString("V") )
            {
                visiblers->AppendSubChunk( trigger, 0 );
                ++numVisiblers;
            }
            else
            {
                locator->AppendSubChunk( trigger );
                ++numRegular;
            }

        }

        
        //Subtract one from the num regular to not cound the primary trigger.
        numRegular--;

        //Now add the visiblers to the end of the list.
        if ( numVisiblers )
        {
            visiblers->TransferSubChunks( locator );

            //Record how many of these triggers are regular.
            locator->SetDataElements( &numRegular, 1 );
            locator->SetNumDataElements( 1 );
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
