//
// Copyright (C) 2002 Radical Entertainment 
// 
// File: pluginMain.cpp
//
// Author: Maya SDK Wizard
//

#include <maya/MFnPlugin.h>

//This is a warning provided by the STL...  It seems that toollib gets whacky when there 
//is other templates made...  Sigh...
#pragma warning(disable:4786)

#include "pluginmain.h"
#include "worldbuilder.h"

#include "utility/mayahandles.h"
#include "utility/mext.h"

#include "gameengine/gameengine.h"

#include "../../../game/code/meta/locatorevents.h"

//Nodes
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


//Contexts
#include "contexts/LocatorContext.h"
#include "contexts/TriggerContext.h"

//Commands
#include "commands/worldbuildercommands.h"
#include "commands/export.h"

#include <radtime.hpp>
#include <radmemory.hpp>
#include <radthread.hpp>

WorldBuilder* gWB;

MStatus initializePlugin( MObject obj )
//
//	Description:
//		this method is called when the plug-in is loaded into Maya.  It 
//		registers all of the services that this plug-in provides with 
//		Maya.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{ 
	MStatus   status;


    MayaHandles::SetHInstance( (void*)(MhInstPlugin) );

	MFnPlugin plugin( obj, "Radical Entertainment", "4.0.1", "Any", &status );
    assert( status );

    radTimeInitialize(); 
    radMemoryInitialize();
    radThreadInitialize();

	// Add plug-in feature registration here
	//

    //This is a test due to changes in the SRR stuff.
    bool test = LocatorEvent::TestEvents();
    assert( test );
    

    //Register Nodes
    REGISTER_LOCATOR( plugin, ScriptLocatorNode );
    REGISTER_LOCATOR( plugin, GenericLocatorNode );
    REGISTER_LOCATOR( plugin, CarStartLocatorNode );
    REGISTER_LOCATOR( plugin, SplineLocatorNode );
    REGISTER_LOCATOR( plugin, TriggerVolumeNode );
    REGISTER_LOCATOR( plugin, EventLocatorNode );
    REGISTER_LOCATOR( plugin, ZoneEventLocatorNode );
    REGISTER_LOCATOR( plugin, OcclusionLocatorNode );
    REGISTER_LOCATOR( plugin, RailCamLocatorNode );
    REGISTER_LOCATOR( plugin, InteriorEntranceLocatorNode );
    REGISTER_LOCATOR( plugin, DirectionalLocatorNode );
    REGISTER_LOCATOR( plugin, ActionEventLocatorNode );
    REGISTER_LOCATOR( plugin, FOVLocatorNode );
    REGISTER_LOCATOR( plugin, BreakableCameraLocatorNode );
    REGISTER_LOCATOR( plugin, StaticCameraLocatorNode );
    REGISTER_LOCATOR( plugin, PedGroupLocatorNode );

    //Register Contexts
    REGISTER_CONTEXT( plugin, LocatorContext );
    REGISTER_CONTEXT( plugin, TriggerContext );

    //Register Commands
    REGISTER_COMMAND( plugin, WBChangeDisplayCommand );
    REGISTER_COMMAND( plugin, WBSetLocatorTypeCmd );
    REGISTER_COMMAND( plugin, WBSetPrefixCmd );
    REGISTER_COMMAND( plugin, WBSnapLocatorCmd );
    REGISTER_COMMAND( plugin, ExportCommand );
    REGISTER_COMMAND( plugin, WBSplineCompleteCmd );
    REGISTER_COMMAND( plugin, WBCoinSplineCompleteCmd );
    REGISTER_COMMAND( plugin, WBSelectObjectCmd );

    gWB = new WorldBuilder();

    //Run any startup scripts.
    MGlobal::sourceFile( MString( "wb_main.mel" ) );

    GameEngine::CreateInstance();

    return status;
}

MStatus uninitializePlugin( MObject obj )
//
//	Description:
//		this method is called when the plug-in is unloaded from Maya. It 
//		deregisters all of the services that it was providing.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{
	MStatus   status;
	MFnPlugin plugin( obj );

	// Add plug-in feature deregistration here
	//

    //Run any cleanup scripts.
    MGlobal::sourceFile( MString( "wb_cleanup.mel" ) );

    if ( gWB )
    {
        delete gWB;
    }

    //Deregister Commands
    DEREGISTER_COMMAND( plugin, WBSelectObjectCmd );
    DEREGISTER_COMMAND( plugin, WBSplineCompleteCmd );
    DEREGISTER_COMMAND( plugin, WBCoinSplineCompleteCmd );
    DEREGISTER_COMMAND( plugin, ExportCommand );
    DEREGISTER_COMMAND( plugin, WBSnapLocatorCmd );
    DEREGISTER_COMMAND( plugin, WBSetPrefixCmd );
    DEREGISTER_COMMAND( plugin, WBChangeDisplayCommand );
    DEREGISTER_COMMAND( plugin, WBSetLocatorTypeCmd );

    //Deregister Contexts
    DEREGISTER_CONTEXT( plugin, TriggerContext );
    DEREGISTER_CONTEXT( plugin, LocatorContext );

    //Deregister Nodes
    DEREGISTER_NODE( plugin, SplineLocatorNode );
    DEREGISTER_NODE( plugin, TriggerVolumeNode );
    DEREGISTER_NODE( plugin, CarStartLocatorNode);
    DEREGISTER_NODE( plugin, GenericLocatorNode );
    DEREGISTER_NODE( plugin, ScriptLocatorNode );
    DEREGISTER_NODE( plugin, EventLocatorNode );
    DEREGISTER_NODE( plugin, ZoneEventLocatorNode );
    DEREGISTER_NODE( plugin, OcclusionLocatorNode );
    DEREGISTER_NODE( plugin, RailCamLocatorNode );
    DEREGISTER_NODE( plugin, InteriorEntranceLocatorNode );
    DEREGISTER_NODE( plugin, DirectionalLocatorNode );
    DEREGISTER_NODE( plugin, ActionEventLocatorNode );
    DEREGISTER_NODE( plugin, FOVLocatorNode );
    DEREGISTER_NODE( plugin, BreakableCameraLocatorNode );
    DEREGISTER_NODE( plugin, StaticCameraLocatorNode );
    DEREGISTER_NODE( plugin, PedGroupLocatorNode );

    GameEngine::DestroyInstance();

    radTimeTerminate();    
    
	return status;
}
 