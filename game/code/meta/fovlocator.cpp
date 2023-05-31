//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        FOVLocator.cpp
//
// Description: Implement FOVLocator
//
// History:     04/08/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <raddebugwatch.hpp>

//========================================
// Project Includes
//========================================
#include <meta/FOVLocator.h>
#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>


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
// FOVLocator::FOVLocator
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
FOVLocator::FOVLocator()
{
    RegisterDebugData();
}

//==============================================================================
// FOVLocator::~FOVLocator
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
FOVLocator::~FOVLocator()
{
    UnRegisterDebugData();
}

//=============================================================================
// FOVLocator::RegisterDebugData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FOVLocator::RegisterDebugData()
{
#ifdef DEBUGWATCH
    radDbgWatchAddFloat( &mFOV, "FOV", "FOV Override", NULL, NULL, 0.01f, rmt::PI );
    radDbgWatchAddFloat( &mTime, "Max Transition Time (seconds)", "FOV Override", NULL, NULL, 0.0f, 5.0f );
    radDbgWatchAddFloat( &mRate, "Transition Rate", "FOV Override", NULL, NULL, 0.0f, 1.0f );
#endif
}

//=============================================================================
// FOVLocator::UnRegisterDebugData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FOVLocator::UnRegisterDebugData()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &mFOV );
    radDbgWatchDelete( &mTime );
    radDbgWatchDelete( &mRate );
#endif
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

void FOVLocator::OnTrigger( unsigned int playerID )
{
BEGIN_PROFILE( "FOVL OnTrigger" );
    if ( GetPlayerEntered() )
    {
        SuperCamManager::GetInstance()->GetSCC( playerID )->RegisterFOVLocator( this );
    }
    else
    {
        SuperCamManager::GetInstance()->GetSCC( playerID )->UnregisterFOVLocator();
    }
END_PROFILE( "FOVL OnTrigger" );
}
