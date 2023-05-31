//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ZoneEventLocatorNode.cpp
//
// Description: Implement ZoneEventLocatorNode
//
// History:     18/06/2002 + Created -- Cary Brisebois
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
#include "nodes/ZoneEventLocatorNode.h"
#include "main/constants.h"
#include "main/worldbuilder.h"
#include "utility/glext.h"
#include "utility/mext.h"
#include "utility/nodehelper.h"

#include "nodes/triggervolumenode.h"

#include "resources/resource.h"

#include "../../../game/code/meta/locatorevents.h"
#include "../../../game/code/meta/locatortypes.h"

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
MTypeId ZoneEventLocatorNode::id( WBConstants::TypeIDPrefix, WBConstants::NodeIDs::ZoneEventLocator );
const char* ZoneEventLocatorNode::stringId = "ZoneEventLocatorNode";

const int ZoneEventLocatorNode::ACTIVE_COLOUR = 15;
const int ZoneEventLocatorNode::INACTIVE_COLOUR = 12;
const float ZoneEventLocatorNode::SCALE = 1.0f * WBConstants::Scale;

const char* ZoneEventLocatorNode::TRIGGERS_NAME_SHORT   = "trigs";
const char* ZoneEventLocatorNode::TRIGGERS_NAME_LONG    = "triggers";
MObject ZoneEventLocatorNode::sTriggers;

const char* ZoneEventLocatorNode::ZONE_NAME_SHORT = "z";
const char* ZoneEventLocatorNode::ZONE_NAME_LONG = "zone";
MObject ZoneEventLocatorNode::sZone;

char ZoneEventLocatorNode::sNewName[MAX_NAME_LEN];
char ZoneEventLocatorNode::sZoneName[MAX_NAME_LEN];

//******************************************************************************
//
// Callbacks
//
//******************************************************************************

BOOL CALLBACK ZoneEventLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
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
                char name[ZoneEventLocatorNode::MAX_NAME_LEN];
                GetDlgItemText( hWnd, IDC_EDIT1, name, ZoneEventLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a new name for the Zone Event Locator!");
                    return false;
                }

                MString newName( WorldBuilder::GetPrefix() );
                newName += MString( name );
                
                ZoneEventLocatorNode::SetNewName( newName.asChar() );

                //Do the zone name too...
                GetDlgItemText( hWnd, IDC_EDIT3, name, ZoneEventLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a file name for the zone!");
                    return false;
                }

                ZoneEventLocatorNode::SetZoneName( name );

                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if( LOWORD(wParam) == IDCANCEL )
            {
                ZoneEventLocatorNode::SetNewName( "" );
                ZoneEventLocatorNode::SetZoneName( "" );
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
// ZoneEventLocatorNode::ZoneEventLocatorNode
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ZoneEventLocatorNode::ZoneEventLocatorNode()
{
}

//==============================================================================
// ZoneEventLocatorNode::~ZoneEventLocatorNode
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ZoneEventLocatorNode::~ZoneEventLocatorNode()
{
}

//=============================================================================
// ZoneEventLocatorNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* ZoneEventLocatorNode::creator()
{
    return new ZoneEventLocatorNode();
}

//=============================================================================
// ZoneEventLocatorNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void ZoneEventLocatorNode::draw( M3dView& view, 
                             const MDagPath& path, 
                             M3dView::DisplayStyle displayStyle, 
                             M3dView::DisplayStatus displayStatus )
{
    if ( WorldBuilder::GetDisplayLevel() & WorldBuilder::ZONE_EVENT_LOCATORS )
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
        GLExt::drawLBolt( SCALE, 0,1,0, 5.0 );

        glPopAttrib();
        view.endGL();
    }
}

//=============================================================================
// ZoneEventLocatorNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus ZoneEventLocatorNode::initialize()
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
// ZoneEventLocatorNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ZoneEventLocatorNode::postConstructor()
{
}

//=============================================================================
// ZoneEventLocatorNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& eventLocatorNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* ZoneEventLocatorNode::Export( MObject& zoneEventLocatorNode )
{
    MFnDagNode fnNode( zoneEventLocatorNode );

    if ( fnNode.typeId() == ZoneEventLocatorNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlWBLocatorChunk* locator = new tlWBLocatorChunk;

        locator->SetName( fnNode.name().asChar() );

        locator->SetType( LocatorType::DYNAMIC_ZONE );

        //The data here is the zone associated with this locator.
        MString zone;
        fnNode.findPlug( sZone ).getValue( zone );

        unsigned int length = zone.length() / 4 + 1;

        unsigned long* data;

        data = new unsigned long[ length ];

        memcpy( data, zone.asChar(), zone.length() );
        ((char*)(data))[zone.length()] = '\0';

        locator->SetDataElements( data, length );
        locator->SetNumDataElements( length );

        MPoint thisPosition;
        MExt::GetWorldPosition( &thisPosition, zoneEventLocatorNode );

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
