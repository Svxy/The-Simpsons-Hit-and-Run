//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        WorldBuilder.cpp
//
// Description: Implement WorldBuilder
//
// History:     16/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech

//========================================
// Project Includes
//========================================
#include "WorldBuilder.h"
#include "utility/mext.h"

#include "nodes/eventlocatornode.h"
#include "nodes/scriptlocatornode.h"
#include "nodes/genericlocatornode.h"
#include "nodes/carstartlocatornode.h"
#include "nodes/splinelocatornode.h"
#include "nodes/triggervolumenode.h"
#include "nodes/zoneeventlocatornode.h"
#include "nodes/occlusionlocatornode.h"
#include "nodes/railcamlocatornode.h"
#include "nodes/interiorentrancelocatornode.h"
#include "nodes/directionallocatornode.h"
#include "nodes/actioneventlocatornode.h"
#include "nodes/fovlocatornode.h"
#include "nodes/BreakableCameraLocatorNode.h"
#include "nodes/staticcameralocatornode.h"
#include "nodes/pedgrouplocator.h"

 
//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

unsigned int WorldBuilder::sDisplayLevel = ALL;
LocatorType::Type WorldBuilder::sLocatorType = LocatorType::EVENT;

const char* WorldBuilder::sName = "WORLD_BUILDER_NAME";

char WorldBuilder::sPrefix[MAX_PREFIX_LENGTH + 1];
char WorldBuilder::sSelectedLocator[MAX_NAME_LENGTH + 1];

//==============================================================================
// WorldBuilder::WorldBuilder
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
WorldBuilder::WorldBuilder()
{
    sPrefix[0] = '\0';
    sPrefix[MAX_PREFIX_LENGTH] = '\0';
}

//==============================================================================
// WorldBuilder::~WorldBuilder
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
WorldBuilder::~WorldBuilder()
{
}

//=============================================================================
// WorldBuilder::Exists
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool WorldBuilder::Exists()
{
    MDagPath pathToWorldBuilder;
    return MExt::FindDagNodeByName( &pathToWorldBuilder, MString( WorldBuilder::sName ) );
}

//=============================================================================
// WorldBuilder::AddChild
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& obj )
//
// Return:      MStatus 
//
//=============================================================================
MStatus WorldBuilder::AddChild( MObject& obj )
{
    //Make sure this exists.
    CreateWorldBuilderNode();

    MDagPath pathToWorldBuilder;

    bool good = false;

    if ( MExt::FindDagNodeByName( &pathToWorldBuilder, MString( WorldBuilder::sName ) ) )
    {
        good = true;
    }

    if ( good )
    {
        MFnDagNode fnDagNodeWB;

        //Which type?
        MFnDagNode fnDagNodeObj( obj );

        if ( fnDagNodeObj.typeId() == EventLocatorNode::id ||
             fnDagNodeObj.typeId() == ScriptLocatorNode::id ||
             fnDagNodeObj.typeId() == CarStartLocatorNode::id ||
             fnDagNodeObj.typeId() == SplineLocatorNode::id ||
             fnDagNodeObj.typeId() == ZoneEventLocatorNode::id ||
             fnDagNodeObj.typeId() == GenericLocatorNode::id ||
             fnDagNodeObj.typeId() == RailCamLocatorNode::id ||
             fnDagNodeObj.typeId() == ActionEventLocatorNode::id ||
             fnDagNodeObj.typeId() == DirectionalLocatorNode::id ||
             fnDagNodeObj.typeId() == InteriorEntranceLocatorNode::id ||
             fnDagNodeObj.typeId() == FOVLocatorNode::id ||
             fnDagNodeObj.typeId() == BreakableCameraLocatorNode::id ||
             fnDagNodeObj.typeId() == StaticCameraLocatorNode::id ||
             fnDagNodeObj.typeId() == OcclusionLocatorNode::id ||
             fnDagNodeObj.typeId() == PedGroupLocatorNode::id )
        {
            //This is a locator, parent to the "Locator" node.
            MDagPath dagPath;
            if ( MExt::FindDagNodeByName( &dagPath, MString(LOCATORS_NAME), pathToWorldBuilder.node() ) )
            {
                fnDagNodeWB.setObject( dagPath.node() );
            }
            else
            {
                MExt::DisplayError( "Someone has deleted World Builder nodes!!" );
            }
        }
        else if ( fnDagNodeObj.typeId() == TriggerVolumeNode::id )
        {
            //This is a locator, parent to the "TriggerVolumes" node. 
            MDagPath dagPath;
            if ( MExt::FindDagNodeByName( &dagPath, MString(TRIGGER_VOLUMES_NAME), pathToWorldBuilder.node() ) )
            {
                fnDagNodeWB.setObject( dagPath.node() );
            }
            else
            {
                MExt::DisplayError( "Someone has deleted World Builder nodes!!" );
            }
        }
        else
        {
            MStatus status;
            MFnNurbsCurve fnNurbs( obj, &status );
            if ( status )
            {
                MDagPath dagPath;
                if ( MExt::FindDagNodeByName( &dagPath, MString(SPLINES_NAME), pathToWorldBuilder.node() ) )
                {
                    fnDagNodeWB.setObject( dagPath.node() );
                }
                else
                {
                    MExt::DisplayError( "Someone has deleted World Builder nodes!!" );
                }
            }
            else
            {
                fnDagNodeWB.setObject( pathToWorldBuilder.node() );
            }
        }

        MObject objT = fnDagNodeObj.parent( 0 );

        return fnDagNodeWB.addChild( objT );
    }

    return MS::kFailure;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// WorldBuilder::CreateWorldBuilderNode
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WorldBuilder::CreateWorldBuilderNode()
{
    MStatus status;

    MFnTransform transform;
    MObject wb, loc, tv, sp;

    MDagPath pathToWorldBuilder, testPath;

    if ( !MExt::FindDagNodeByName( &pathToWorldBuilder, MString( WorldBuilder::sName ) ) )
    {
        //Create the worldbuilder transform.
        wb = transform.create( MObject::kNullObj, &status );
        assert( status );
        transform.setName( MString( WorldBuilder::sName ) );
    }
    else
    {
        wb = pathToWorldBuilder.node();
    }

    if ( !MExt::FindDagNodeByName( &pathToWorldBuilder, MString( LOCATORS_NAME ) ) )
    {
        loc = transform.create( wb, &status );
        assert( status );
        transform.setName( MString( LOCATORS_NAME ) );
    }

    if ( !MExt::FindDagNodeByName( &pathToWorldBuilder, MString( TRIGGER_VOLUMES_NAME ) ) )
    {
        tv = transform.create( wb, &status );
        assert( status );
        transform.setName( MString( TRIGGER_VOLUMES_NAME ) );
    }

    if ( !MExt::FindDagNodeByName( &pathToWorldBuilder, MString( SPLINES_NAME ) ) )
    {
        sp = transform.create( wb, &status );
        assert( status );
        transform.setName( MString( SPLINES_NAME ) );
    }

    MGlobal::executeCommand( "wb_Create_WorldBuilderNode()" );   
}