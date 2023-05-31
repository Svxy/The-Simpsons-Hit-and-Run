//
// Copyright (C) 2002 Radical Entertainment 
// 
// File: pluginMain.cpp
//
// Author: Maya SDK Wizard
//
#include <maya/MFnPlugin.h>

#include "utility/mayahandles.h"

#include "pluginmain.h"
#include "commands/snapselected.h"

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

    //Register Contexts

    //Register Commands
    REGISTER_COMMAND( plugin, SnapSelectedCmd );
    

    //Run any startup scripts.
    MGlobal::sourceFile( "os_main.mel" );

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
    MGlobal::sourceFile( "os_cleanup.mel" );

    //Unregister Commands

    //Unregister Contexts

    //Unregister Nodes

	return status;
}

