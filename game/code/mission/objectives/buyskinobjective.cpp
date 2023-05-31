//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        buyskinobjective.cpp
//
// Description: Implement BuySkinObjective
//
// History:     6/3/2003 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include <mission/objectives/buyskinobjective.h>
#include <mission/gameplaymanager.h>

#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// BuySkinObjective::BuySkinObjective
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
BuySkinObjective::BuySkinObjective() :
    mSkinName( NULL )
{
}

//=============================================================================
// BuySkinObjective::~BuySkinObjective
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
BuySkinObjective::~BuySkinObjective()
{
    if ( mSkinName )
    {
        delete[] mSkinName;
        mSkinName = NULL;
    }
}

//=============================================================================
// BuySkinObjective::SetSkinName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
void BuySkinObjective::SetSkinName( const char* name )
{
    int length = strlen( name );

    rTuneAssertMsg( mSkinName == NULL, "Can not set the name of the skin to be purchased twice!\n");

    if ( mSkinName )
    {
        delete[] mSkinName;
    }

    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    mSkinName = new char[ length + 1 ];
    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );

    strcpy( mSkinName, name );
    mSkinName[ length ] = '\0';
}

//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// BuySkinObjective::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void BuySkinObjective::OnUpdate( unsigned int elapsedTime )
{
    Character* character = GetCharacterManager()->GetCharacter( 0 );

    if ( strcmp( GetCharacterManager()->GetModelName( character ), mSkinName ) == 0 )
    {
        SetFinished( true );
    }
}
