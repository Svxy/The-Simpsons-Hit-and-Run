//
// Copyright (C) 2002 Radical Entertainment 
// 
// File: pluginMain.cpp
//
// Author: Maya SDK Wizard
//
#include "precompiled/PCH.h"

#include <maya/MFnPlugin.h>

//This is a warning provided by the STL...  It seems that toollib gets whacky when there 
//is other templates made...  Sigh...
#pragma warning(disable:4786)

#include "pluginmain.h"
#include "trackeditor.h"

#include "utility/mayahandles.h"
#include "utility/mext.h"

//Nodes
#include "nodes/walllocator.h"
#include "nodes/fenceline.h"
#include "nodes/tiledisplay.h"
#include "nodes/intersection.h"
#include "nodes/road.h"
#include "nodes/treelineshapenode.h"
#include "nodes/pedpath.h"

//Contexts
#include "contexts/bvcontext.h"
#include "contexts/intersectioncontext.h"
#include "contexts/treelinecontext.h"
#include "contexts/ppcontext.h"

//Commands
#include "commands/export.h"
#include "commands/trackeditorcommands.h"
#include "commands/intersectioncommands.h"
#include "commands/treelinecommand.h"


TrackEditor* gTE = 0;

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

	MFnPlugin plugin( obj, "Radical Entertainment", "4.0.1", "Any");

	// Add plug-in feature registration here
	//

    //Register Nodes
    REGISTER_LOCATOR( plugin, WallLocatorNode );
    REGISTER_LOCATOR( plugin, FenceLineNode );
    REGISTER_LOCATOR( plugin, TileDisplayNode );
    REGISTER_LOCATOR( plugin, IntersectionLocatorNode );
    REGISTER_LOCATOR( plugin, RoadNode );
    REGISTER_LOCATOR( plugin, PedPathNode );

    REGISTER_SHAPE( plugin, TETreeLine::TreelineShapeNode );

    //Register Contexts
    REGISTER_CONTEXT( plugin, BVContext );
    REGISTER_CONTEXT( plugin, IntersectionContext );
    REGISTER_CONTEXT( plugin, TreeLineContext );
    REGISTER_CONTEXT( plugin, PPContext );

    //Register Commands
    REGISTER_COMMAND( plugin, PPSplitCmd );
    REGISTER_COMMAND( plugin, BVSplitCmd );
    REGISTER_COMMAND( plugin, ExportCommand );
    REGISTER_COMMAND( plugin, TEStateChangeCommand );
    REGISTER_COMMAND( plugin, TEGetSelectedVertexPosition );
    REGISTER_COMMAND( plugin, TEGetSelectedVertexIndex );
    REGISTER_COMMAND( plugin, CreateRoadCmd );
    REGISTER_COMMAND( plugin, AddIntersectionToRoadCmd );
    REGISTER_COMMAND( plugin, ShowRoadCmd );
    REGISTER_COMMAND( plugin, DestroyRoadCmd );
    REGISTER_COMMAND( plugin, SnapSelectedTreelines );
    REGISTER_COMMAND( plugin, ConvertTreelineToGeometry );
    REGISTER_COMMAND( plugin, SetDeleteTreeline );
    

    //Create the TrackEditor.
    gTE = new TrackEditor();

    //Run any startup scripts.
    MGlobal::sourceFile( MString( "te_main.mel" ) );

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
    MGlobal::sourceFile( MString( "te_cleanup.mel" ) );

    if ( gTE )
    {
        delete gTE;
    }

    //Unregister Commands
    DEREGISTER_COMMAND( plugin, SetDeleteTreeline );
    DEREGISTER_COMMAND( plugin, ConvertTreelineToGeometry );
    DEREGISTER_COMMAND( plugin, SnapSelectedTreelines );
    DEREGISTER_COMMAND( plugin, DestroyRoadCmd );
    DEREGISTER_COMMAND( plugin, ShowRoadCmd );
    DEREGISTER_COMMAND( plugin, AddIntersectionToRoadCmd );
    DEREGISTER_COMMAND( plugin, CreateRoadCmd );
    DEREGISTER_COMMAND( plugin, TEGetSelectedVertexIndex );
    DEREGISTER_COMMAND( plugin, TEGetSelectedVertexPosition );
    DEREGISTER_COMMAND( plugin, TEStateChangeCommand );
    DEREGISTER_COMMAND( plugin, ExportCommand );
    DEREGISTER_COMMAND( plugin, BVSplitCmd );

    //Unregister Contexts
    DEREGISTER_CONTEXT( plugin, PPContext );
    DEREGISTER_CONTEXT( plugin, TreeLineContext );
    DEREGISTER_CONTEXT( plugin, IntersectionContext );
    DEREGISTER_CONTEXT( plugin, BVContext );
    DEREGISTER_CONTEXT( plugin, PPContext );

    //Unregister Nodes
    DEREGISTER_NODE( plugin, PedPathNode );
    DEREGISTER_NODE( plugin, TETreeLine::TreelineShapeNode );
    DEREGISTER_NODE( plugin, RoadNode );
    DEREGISTER_NODE( plugin, IntersectionLocatorNode );
    DEREGISTER_NODE( plugin, TileDisplayNode );
    DEREGISTER_NODE( plugin, FenceLineNode );
    DEREGISTER_NODE( plugin, WallLocatorNode );

	return status;
}

