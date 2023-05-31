//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        DSGFactory.cpp
//
// Description: Implementation for DSGFactory class.
//
// History:     Implemented	                         --Devin [5/6/2002]
//========================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <render/DSG/DSGFactory.h>
#include <render/DSG/IntersectDSG.h>
#include <memory/srrmemory.h>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************
//
// Static pointer to instance of this singleton.
//
DSGFactory* DSGFactory::mspInstance = NULL;

//************************************************************************
//
// Public Member Functions : DSGFactory Interface
//
//************************************************************************
//******************************************************************************
// Public Member Functions : Instance Interface
//******************************************************************************
//==============================================================================
// DSGFactory::CreateInstance
//==============================================================================
//
// Description: Create the DSGFactory controller if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the created DSGFactory controller.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
DSGFactory* DSGFactory::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "DSGFactory" );

   rAssert( mspInstance == NULL );
   mspInstance = new(GMA_PERSISTENT) DSGFactory();
MEMTRACK_POP_GROUP("DSGFactory");

   return mspInstance;
}

//==============================================================================
// DSGFactory::GetInstance
//==============================================================================
//
// Description: Get the DSGFactory controller if exists.
//
// Parameters:	None.
//
// Return:      Pointer to the created DSGFactory controller.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
DSGFactory* DSGFactory::GetInstance()
{
   rAssert( mspInstance != NULL );
   
   return mspInstance;
}


//==============================================================================
// DSGFactory::DestroyInstance
//==============================================================================
//
// Description: Destroy the DSGFactory controller.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void DSGFactory::DestroyInstance()
{
    //
    // Make sure this doesn't get called twice.
    //
    rAssert( mspInstance != NULL );
    delete mspInstance;
    mspInstance = NULL;
}
///////////////////////////////////////////////////////////////////////
//Meat Interface
///////////////////////////////////////////////////////////////////////
//========================================================================
// DSGFactory::CreateEntityDSG
//========================================================================
//
// Description: 
//
// Parameters:  tDrawable* 
//
// Return:      IEntityDSG*    
//
// Constraints: None.
//
//========================================================================
IEntityDSG* DSGFactory::CreateEntityDSG( tDrawable* ipDrawable )
{
   return (IEntityDSG*)(ipDrawable);
}
//========================================================================
// DSGFactory::CreateIntersectDSG
//========================================================================
//
// Description: 
//
// Parameters:  tGeometry* 
//
// Return:      IntersectDSG*  
//
// Constraints: None.
//
//========================================================================
IntersectDSG*  DSGFactory::CreateIntersectDSG(  tGeometry* ipGeometry )
{
MEMTRACK_PUSH_GROUP( "DSGFactory" );
   IntersectDSG* pIDSG =  new(GMA_LEVEL_ZONE) IntersectDSG( ipGeometry );
MEMTRACK_POP_GROUP("DSGFactory");
   return pIDSG;
}

//************************************************************************
//
// Protected Member Functions : DSGFactory 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : DSGFactory 
//
//************************************************************************
//========================================================================
// DSGFactory::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
DSGFactory::DSGFactory()
{
}
//========================================================================
// DSGFactory::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
DSGFactory::~DSGFactory()
{
}
