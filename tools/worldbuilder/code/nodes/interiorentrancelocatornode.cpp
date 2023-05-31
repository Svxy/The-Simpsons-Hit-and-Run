//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        InteriorEntranceLocatorNode.cpp
//
// Description: Implement InteriorEntranceLocatorNode
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
#include "nodes/InteriorEntranceLocatorNode.h"
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
MTypeId InteriorEntranceLocatorNode::id( WBConstants::TypeIDPrefix, WBConstants::NodeIDs::InteriorEntranceLocator );
const char* InteriorEntranceLocatorNode::stringId = "InteriorEntranceLocatorNode";

const int InteriorEntranceLocatorNode::ACTIVE_COLOUR = 15;
const int InteriorEntranceLocatorNode::INACTIVE_COLOUR = 12;
const float InteriorEntranceLocatorNode::SCALE = 1.0f * WBConstants::Scale;

const char* InteriorEntranceLocatorNode::TRIGGERS_NAME_SHORT   = "trigs";
const char* InteriorEntranceLocatorNode::TRIGGERS_NAME_LONG    = "triggers";
MObject InteriorEntranceLocatorNode::sTriggers;

const char* InteriorEntranceLocatorNode::ZONE_NAME_SHORT = "z";
const char* InteriorEntranceLocatorNode::ZONE_NAME_LONG = "zone";
MObject InteriorEntranceLocatorNode::sZone;

char InteriorEntranceLocatorNode::sNewName[MAX_NAME_LEN];
char InteriorEntranceLocatorNode::sZoneName[MAX_NAME_LEN];

//******************************************************************************
//
// Callbacks
//
//******************************************************************************

BOOL CALLBACK InteriorEntranceLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
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
                char name[InteriorEntranceLocatorNode::MAX_NAME_LEN];
                GetDlgItemText( hWnd, IDC_EDIT1, name, InteriorEntranceLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a new name for the Interior Entrance Locator!");
                    return false;
                }

                MString newName( WorldBuilder::GetPrefix() );
                newName += MString( name );
                
                InteriorEntranceLocatorNode::SetNewName( newName.asChar() );

                //Do the zone name too...
                GetDlgItemText( hWnd, IDC_EDIT3, name, InteriorEntranceLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a file name for the zone!");
                    return false;
                }

                InteriorEntranceLocatorNode::SetZoneName( name );

                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if( LOWORD(wParam) == IDCANCEL )
            {
                InteriorEntranceLocatorNode::SetNewName( "" );
                InteriorEntranceLocatorNode::SetZoneName( "" );
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
// InteriorEntranceLocatorNode::InteriorEntranceLocatorNode
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
InteriorEntranceLocatorNode::InteriorEntranceLocatorNode()
{
}

//==============================================================================
// InteriorEntranceLocatorNode::~InteriorEntranceLocatorNode
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
InteriorEntranceLocatorNode::~InteriorEntranceLocatorNode()
{
}

//=============================================================================
// InteriorEntranceLocatorNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* InteriorEntranceLocatorNode::creator()
{
    return new InteriorEntranceLocatorNode();
}

//=============================================================================
// InteriorEntranceLocatorNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void InteriorEntranceLocatorNode::draw( M3dView& view, 
                             const MDagPath& path, 
                             M3dView::DisplayStyle displayStyle, 
                             M3dView::DisplayStatus displayStatus )
{
    if ( WorldBuilder::GetDisplayLevel() & WorldBuilder::INTERIOR_LOCATORS )
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
        GLExt::drawArrow( MPoint( 0, 0, 0 ), MPoint( 0, 0, -1 * WBConstants::Scale ), 5.0 );
        GLExt::drawD( SCALE, 0,1,0, 5.0 );

        glPopAttrib();
        view.endGL();
    }
}

//=============================================================================
// InteriorEntranceLocatorNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus InteriorEntranceLocatorNode::initialize()
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

    MFnTypedAttribute typAttr;
    sZone = typAttr.create( ZONE_NAME_LONG, ZONE_NAME_SHORT, MFnData::kString, MObject::kNullObj, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( typAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( typAttr.setWritable( true ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( sZone ) );

    return MStatus::kSuccess;
}

//=============================================================================
// InteriorEntranceLocatorNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void InteriorEntranceLocatorNode::postConstructor()
{
}

//=============================================================================
// InteriorEntranceLocatorNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& interiorEntranceLocatorNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* InteriorEntranceLocatorNode::Export( MObject& interiorEntranceLocatorNode )
{
    MFnDagNode fnNode( interiorEntranceLocatorNode );

    if ( fnNode.typeId() == InteriorEntranceLocatorNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlWBLocatorChunk* locator = new tlWBLocatorChunk;

        locator->SetName( fnNode.name().asChar() );

        locator->SetType( LocatorType::INTERIOR_ENTRANCE );

        //The data here is the zone associated with this locator.
        MString zone;
        fnNode.findPlug( sZone ).getValue( zone );

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


        unsigned int length = (zone.length() / 4 + 1) + 3 * 3; //string + 3 vectors

        unsigned long* data;

        data = new unsigned long[ length ];
        unsigned int i;
        for ( i = 0; i < length; ++i )
        {
            data[i] = 0;
        }

        memcpy( data, zone.asChar(), zone.length() );
        ((char*)(data))[zone.length()] = '\0';

        unsigned int row;
        for ( row = 0; row < 3; ++row )
        {
            tlPoint point = hmatrix.GetRow( row );

            memcpy( &data[(zone.length() / 4 + 1) + row * 3], &point.x, sizeof(float) * 3 );
        }

        locator->SetDataElements( data, length );
        locator->SetNumDataElements( length );

        MPoint thisPosition;
        MExt::GetWorldPosition( &thisPosition, interiorEntranceLocatorNode );

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

        delete data;

        return locator;
    }

    return NULL;
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
