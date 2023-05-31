//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        supercammanager.cpp
//
// Description: Implementation for supercammanager class.
//
// History:     Implemented	                         --Devin [5/1/2002]
//========================================================================

//========================================
// System Includes
//========================================
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include <camera/supercammanager.h>
#include <memory/srrmemory.h>

#include <mission/gameplaymanager.h>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************
//
// Static pointer to instance of this singleton.
//
SuperCamManager* SuperCamManager::mspInstance = NULL;

//************************************************************************
//
// Public Member Functions : supercammanager Interface
//
//************************************************************************

//========================================================================
// supercammanager::
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
SuperCamManager* SuperCamManager::CreateInstance()
{
   if (mspInstance == NULL)
   {
       mspInstance = new(GMA_PERSISTENT) SuperCamManager;
   }

   return mspInstance;
}

//========================================================================
// supercammanager::
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
SuperCamManager* SuperCamManager::GetInstance()
{
   return mspInstance;
}

//========================================================================
// supercammanager::
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
void  SuperCamManager::DestroyInstance()
{
   rAssert(mspInstance != NULL);
   delete mspInstance;
}

//=============================================================================
// SuperCamManager::Init
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool shutdown )
// 
// Return:      void 
//
//=============================================================================
void SuperCamManager::Init( bool shutdown )
{
    int i;
    for ( i = 0; i < MAX_PLAYERS; ++i )
    {
        mSCCs[ i ].Init( shutdown ); 
    }
}

//========================================================================
// supercammanager::
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
/*
SuperCamCentral& SuperCamManager::GetSCC( int iPlayer )
{
   rAssert( iPlayer < MAX_PLAYERS && iPlayer >= 0 );
   return mSCCs[iPlayer];
}
*/

//=============================================================================
// SuperCamManager::GetSCC
//=============================================================================
// Description: Comment
//
// Parameters:  (int iPlayer)
//
// Return:      SuperCamCentral
//
//=============================================================================
SuperCamCentral* SuperCamManager::GetSCC(int iPlayer)
{
    if(iPlayer < MAX_PLAYERS && iPlayer >= 0)
    {
        return &(mSCCs[iPlayer]);
    }
    else
    {
        return 0;
    }
}


//=============================================================================
// SuperCamManager::PreCollisionPrep
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCamManager::PreCollisionPrep()
{
//    int i;
//    for( i = 0; i < MAX_PLAYERS; ++i )
//    {
//       mSCCs[i].PreCollisionPrep();
//    }

    //We're not doing multiple views, so there!
    mSCCs[0].PreCollisionPrep();
}


//========================================================================
// supercammanager::
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
void SuperCamManager::Update( unsigned int iElapsedTime, bool isFirstSubstep )
{
//    int i;

    //TODO: This should get setup when going into the game.  Also, 
    //it should only initialize the number of players equal to the number
    //playing.
//    for( i = 0; i < MAX_PLAYERS; ++i )
//    {
//       mSCCs[i].Update(iElapsedTime);
//    }

    //We're not doing multiple views, so there!
    mSCCs[ 0 ].Update( iElapsedTime, isFirstSubstep );
}

//=============================================================================
// SuperCamManager::SubmitStatics
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SuperCamManager::SubmitStatics()
{
//    int i;    

//    for(i = 0; i < GetGameplayManager()->GetNumPlayers(); i++)
//    {
        // TODO - should probably be a more robust way to tell which of these
        // is active    
//        mSCCs[i].SubmitStatics();        
//    }
    //We're not doing multiple views, so there!
    mSCCs[0].SubmitStatics();
}

//=============================================================================
// SuperCamManager::ToggleFirstPerson
//=============================================================================
// Description: Comment
//
// Parameters:  ( int controllerID )
//
// Return:      void 
//
//=============================================================================
void SuperCamManager::ToggleFirstPerson( int controllerID )
{
    int i;
    for ( i = 0; i < GetGameplayManager()->GetNumPlayers(); ++i )
    {
        mSCCs[i].ToggleFirstPerson( controllerID );
    }
}


//************************************************************************
//
// Protected Member Functions : supercammanager 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : supercammanager 
//
//************************************************************************
//========================================================================
// supercammanager::
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
SuperCamManager::SuperCamManager()
{
}

//========================================================================
// supercammanager::
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
SuperCamManager::~SuperCamManager()
{
}
