//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ATCLoader.cpp
//
// Description: Implements ATCLoader, to process the ATC Chunk. 
//
// History:     03/08/2002 + Created -- Chuck Chow
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/chunkfile.hpp>

//========================================
// Project Includes
//========================================
#include <atc/atcloader.h>
#include <memory/srrmemory.h>
#include <atc/atcmanager.h>
#include <render/DSG/collisionentitydsg.h>

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

//==============================================================================
// ATCLoader::ATCLoader
//==============================================================================
// Description: Constructor.
//
// Parameters:	ChunkID of The ATTRIBUTE_TABLE
//
// Return:      N/A.
//
//==============================================================================
ATCLoader::ATCLoader():tSimpleChunkHandler( SRR2::ChunkID::ATTRIBUTE_TABLE )
{
    
   
}

//==============================================================================
// ATCLoader::~ATCLoader
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ATCLoader::~ATCLoader()
{
}

//=============================================================================
// ATCLoader::LoadObject
//=============================================================================
// Description: Process the ATC chunk, creates the ATC table an sets mp_ATCTable in the ATCManager.
//
// Parameters:  (tChunkFile* f, tEntityStore* store)
//
// Return:      NULL, This chunk does not get added to the inventory 
//
//=============================================================================


//Loads the ATC Chunk
tEntity* ATCLoader::LoadObject(tChunkFile* f, tEntityStore* store)
{   
    ATCManager* p_atcmanager =NULL;
    AttributeRow* p_AttributeRow = NULL;
    unsigned long numrows = f->GetUInt();
    
    //creating ATCTable
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_PERSISTENT );
    #endif

    p_AttributeRow = new AttributeRow [numrows];
    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_PERSISTENT );
    #endif

    //populating ATCTable
    //read in the array row until there is no more rows
    for(unsigned int i=0;i<numrows;i++)
    {
       f->GetString(p_AttributeRow[i].mSound);
       f->GetString(p_AttributeRow[i].mParticle);
       f->GetString(p_AttributeRow[i].mAnimation);
       p_AttributeRow[i].mFriction=f->GetFloat();
       p_AttributeRow[i].mMass=f->GetFloat();
       p_AttributeRow[i].mElasticity=f->GetFloat();
    }
    
    //setting mp_ATCTable in the ATCManager
    p_atcmanager=ATCManager::GetInstance();
    p_atcmanager->SetATCTable(p_AttributeRow,numrows);
   
    return NULL;
}



            

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
