//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        atcmanager.cpp
//
// Description: Implementation for the ATCManager class, Stolen from Darwin's EventManager .
//
// History:     + Created -- Chuck Chow
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include <atc/atcmanager.h>
#include <memory/srrmemory.h>
#include <render/DSG/collisionentitydsg.h>
#include <loading/loadingmanager.h>
#include <constants/physprop.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.

ATCManager* ATCManager::spInstance = NULL;


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// ATCManager::CreateInstance
//==============================================================================
//
// Description: Creates the ATCManager.
//
// Parameters:	None.
//
// Return:      Pointer to the ATCManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
ATCManager* ATCManager::CreateInstance()
{
    rAssert( spInstance == NULL );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_PERSISTENT );
    #endif

    spInstance = new ATCManager;
    rAssert( spInstance );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_PERSISTENT );
    #endif
    
    return spInstance;
}

//==============================================================================
// ATCManager::GetInstance
//==============================================================================
//
// Description: - Access point for the ATCManager singleton.  
//
// Parameters:	None.
//
// Return:      Pointer to the ATCManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
ATCManager* ATCManager::GetInstance()
{
    rAssert( spInstance != NULL );
    
    return spInstance;
}


//==============================================================================
// ATCManager::DestroyInstance
//==============================================================================
//
// Description: Destroy the ATCManager.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void ATCManager::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete spInstance;
    spInstance = NULL;
}


// Call this method to Create a CollisionAttribute Class
// Input is the classtypeid and the physprop from the OTC Chunk found in a DSG object in a PURE3D file.


// NOTE this Method is incomplete since we dont have a particle system manager, I'm creating the particle pointer set to NULL
//
// volume is for moveable things to properly calculate the density for the physics properties
// it defaults to 0.0f, in which case it's ignored
CollisionAttributes* ATCManager::CreateCollisionAttributes(unsigned int classtypeid,unsigned int physpropid, float volume)
{

    //checking that physpropid is in the valid range
    rAssert( (physpropid >= 0) && (physpropid < mNumRows) );
   
    //Creates a CollisionAttributes Class and returns a pointer to it.
    CollisionAttributes* p_collisionattributes = new (GMA_LEVEL_OTHER) CollisionAttributes (mp_ATCTable[physpropid].mSound,mp_ATCTable[physpropid].mParticle,mp_ATCTable[physpropid].mAnimation, 
                                                                                            mp_ATCTable[physpropid].mFriction,mp_ATCTable[physpropid].mMass,mp_ATCTable[physpropid].mElasticity,
                                                                                            classtypeid, volume);
    return p_collisionattributes;


}


//Initializes the Manager's ATCTable
//by asking LoadingManager to load ATC chunk
void  ATCManager::Init (void)
{  
    GetLoadingManager()->AddRequest(FILEHANDLER_PURE3D, "art\\atc\\atc.p3d", GMA_PERSISTENT, "Default");
}

//Sets the internal pointer to a Vaild ATC Table. and number of rows in the table
void ATCManager::SetATCTable(AttributeRow* p_attributerow,unsigned int rows)
{
    mp_ATCTable=p_attributerow;
    mNumRows =rows;

    strcpy(mp_ATCTable[0].mAnimation,"eNull");
    strcpy(mp_ATCTable[0].mSound,"smash");
    strcpy(mp_ATCTable[0].mParticle,"eNull");
    mp_ATCTable[0].mMass=100.0f;
    mp_ATCTable[0].mFriction=1.0f;
    mp_ATCTable[0].mElasticity=1.0f;

}



//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************


//==============================================================================
// ATCManager::ATCManager
//==============================================================================
//
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================// 
ATCManager::ATCManager()
{
    mp_ATCTable =NULL;
    mNumRows=0;
}


//==============================================================================
// ATCManager::~ATCManager
//==============================================================================
//
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================// 
ATCManager::~ATCManager()
{
    delete  [] mp_ATCTable;
}

