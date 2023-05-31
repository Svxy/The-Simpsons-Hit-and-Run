//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   No component
//
// Description: string names of blobby shadows and the meshes they match up with
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef BLOBSHADOWNAMES_H
#define BLOBSHADOWNAMES_H


//===========================================================================
// Nested Includes
//===========================================================================

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================

namespace BlobbyShadowNames
{
	struct BlobbyShadows
	{
		const char* searchString;
		const char* drawableName;
	};
	
	const BlobbyShadows shadowList[] = 
	{
        { "treesmdead", "deadtree_shadowShape" },
        { "treedead", "deadtree_shadowShape" },
        { "treesm", "treeshadowsmall" },
		{ "treebig", "treeshadowbig" },
		{ "treeevermed", "treeshadoweversmallShape" },
		{ "treecypress", "treeshadowsmall" },
        { "l5_streetlampold_Shape","l5_streetlamp_lightpoolShape" },
        { "l5_oldstreetlamp_Shape","l5_streetlamp_lightpoolShape" }
	};

	// Retrieve the name of the tDrawable that
	// is associated with the shadow identifier substring
	// When an object is loaded, call FindShadowName to
	// determine if a shadow needs to be placed under it
	inline const char* FindShadowName( const char* objectName )
	{
		const int NUM_SHADOW_NAMES = sizeof( shadowList ) / sizeof( shadowList[0] );
		const char* result = NULL;
		for (int i = 0 ; i < NUM_SHADOW_NAMES ; i++)
		{
			if ( strstr( objectName, shadowList[i].searchString ) )
			{
				result = shadowList[i].drawableName;
				break;
			}
			
		}
		return result;
	}

};


#endif