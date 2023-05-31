//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        EventLocatorNode.cpp
//
// Description: Implement EventLocatorNode
//
// History:     16/05/2002 + Created -- Cary Brisebois
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
#include "EventLocatorNode.h"
#include "main/constants.h"
#include "main/worldbuilder.h"
#include "utility/glext.h"
#include "utility/mext.h"
#include "utility/nodehelper.h"

#include "nodes/triggervolumenode.h"

#include "resources/resource.h"

#include "../../../game/code/meta/locatorevents.h"
#include "../../../game/code/meta/locatortypes.h"

#include "utility/transformmatrix.h"

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
MTypeId EventLocatorNode::id( WBConstants::TypeIDPrefix, WBConstants::NodeIDs::EventLocator );
const char* EventLocatorNode::stringId = "EventLocatorNode";

const int EventLocatorNode::ACTIVE_COLOUR = 15;
const int EventLocatorNode::INACTIVE_COLOUR = 12;
const float EventLocatorNode::SCALE = 1.0f * WBConstants::Scale;

const char* EventLocatorNode::TRIGGERS_NAME_SHORT   = "trigs";
const char* EventLocatorNode::TRIGGERS_NAME_LONG    = "triggers";
MObject EventLocatorNode::sTriggers;

const char* EventLocatorNode::EVENT_NAME_SHORT = "e";
const char* EventLocatorNode::EVENT_NAME_LONG = "event";
MObject EventLocatorNode::sEvent;

const char* EventLocatorNode::EXTRA_NAME_SHORT = "ex";
const char* EventLocatorNode::EXTRA_NAME_LONG = "extraData";
MObject EventLocatorNode::sExtraData;

char EventLocatorNode::sNewName[MAX_NAME_LEN];

//******************************************************************************
//
// Callbacks
//
//******************************************************************************

BOOL CALLBACK EventLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
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
                char name[EventLocatorNode::MAX_NAME_LEN];
                GetDlgItemText( hWnd, IDC_EDIT1, name, EventLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a new name for the Event Locator!");
                    return false;
                }

                MString newName( WorldBuilder::GetPrefix() );
                newName += MString( name );
                
                EventLocatorNode::SetNewName( newName.asChar() );
                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if( LOWORD(wParam) == IDCANCEL )
            {
                EventLocatorNode::SetNewName( "" );
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
// EventLocatorNode::EventLocatorNode
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
EventLocatorNode::EventLocatorNode()
{
}

//==============================================================================
// EventLocatorNode::~EventLocatorNode
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
EventLocatorNode::~EventLocatorNode()
{
}

//=============================================================================
// EventLocatorNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* EventLocatorNode::creator()
{
    return new EventLocatorNode();
}

//=============================================================================
// EventLocatorNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void EventLocatorNode::draw( M3dView& view, 
                             const MDagPath& path, 
                             M3dView::DisplayStyle displayStyle, 
                             M3dView::DisplayStatus displayStatus )
{
    if ( WorldBuilder::GetDisplayLevel() & WorldBuilder::EVENT_LOCATORS )
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
        GLExt::drawPyramid( SCALE, 0,0,0, 5.0 );
        GLExt::drawE( SCALE, 0,1,0, 5.0 );

        MFnDagNode fnNode( thisMObject() );
        int event;
        fnNode.findPlug( sEvent ).getValue( event );

        if ( event == LocatorEvent::DEATH || 
             event == LocatorEvent::WHACKY_GRAVITY ||
             event == LocatorEvent::CHECK_POINT )
        {
            GLExt::drawArrow( MPoint( 0, 0, 0 ), MPoint( 0, 0, -1 * WBConstants::Scale ), 5.0 );
        }

        glPopAttrib();
        view.endGL();
    }
}

//=============================================================================
// EventLocatorNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus EventLocatorNode::initialize()
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

    MFnEnumAttribute enumAttr;
    sEvent = enumAttr.create( EVENT_NAME_LONG, EVENT_NAME_SHORT, LocatorEvent::CHECK_POINT, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( enumAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( enumAttr.setWritable( true ) );

    unsigned int i;
    for ( i = 0; i < LocatorEvent::SPECIAL; ++i )  //This is the part in the 
                                                   //enum where the regular 
                                                   //events end.
    {
        RETURN_STATUS_ON_FAILURE( enumAttr.addField( MString( LocatorEvent::Name[i] ), i ) );
    }

    RETURN_STATUS_ON_FAILURE( addAttribute( sEvent ) );
    
    MFnTypedAttribute typAttr;
    sExtraData = typAttr.create( EXTRA_NAME_LONG, EXTRA_NAME_SHORT, MFnData::kString, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( typAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( typAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sExtraData ) );

    return MStatus::kSuccess;
}

//=============================================================================
// EventLocatorNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void EventLocatorNode::postConstructor()
{
}

//=============================================================================
// EventLocatorNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& eventLocatorNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* EventLocatorNode::Export( MObject& eventLocatorNode )
{
    MFnDagNode fnNode( eventLocatorNode );

    if ( fnNode.typeId() == EventLocatorNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlWBLocatorChunk* locator = new tlWBLocatorChunk;

        locator->SetName( fnNode.name().asChar() );

        locator->SetType( LocatorType::EVENT );

        //The data here is the event associated with this locator.
        int event;
        fnNode.findPlug( sEvent ).getValue( event );

        if ( event == static_cast<int>( LocatorEvent::FAR_PLANE ) || 
             event == static_cast<int>( LocatorEvent::GOO_DAMAGE ) ||
             event == static_cast<int>( LocatorEvent::COIN_ZONE ) ||
             event == static_cast<int>( LocatorEvent::CHECK_POINT ) ||
             event == static_cast<int>( LocatorEvent::TRAP ) ||
             event == static_cast<int>( LocatorEvent::LIGHT_CHANGE ) )
        {
            //Add some extra data.
            unsigned long data[2];
            data[0] = event;

            MString extraData;
            fnNode.findPlug( sExtraData ).getValue( extraData );
            assert( extraData.isInt() );
            if ( extraData.isInt() )
            {
                int extra = extraData.asInt();
                memcpy( &data[1], &extra, sizeof(int) );
            } 
            else
            {
                MExt::DisplayWarning( "Event locator: %s has bad extra data, it is probably an old version.(defaulting to 0)", fnNode.name().asChar() );
                data[1] = 0;
            }

            locator->SetDataElements( data, 2 ); 
            locator->SetNumDataElements( 2 );
        }
        else
        {
            //Do the old way.
            unsigned long data = event;
            locator->SetDataElements( &data, 1 );
            locator->SetNumDataElements( 1 ); 
        }

        MPoint thisPosition;
        MExt::GetWorldPosition( &thisPosition, eventLocatorNode );

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

        if ( event == LocatorEvent::DEATH || 
             event == LocatorEvent::WHACKY_GRAVITY || 
             event == LocatorEvent::CHECK_POINT )
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
