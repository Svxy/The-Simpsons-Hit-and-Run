//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        buycarobjective.cpp
//
// Description: Implement BuyCarObjective
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
#include <mission/objectives/buycarobjective.h>
#include <mission/gameplaymanager.h>

#include <worldsim/redbrick/vehicle.h>


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
// BuyCarObjective::BuyCarObjective
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
BuyCarObjective::BuyCarObjective() :
    mVehicleName( NULL )
{
}

//=============================================================================
// BuyCarObjective::~BuyCarObjective
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
BuyCarObjective::~BuyCarObjective()
{
    if ( mVehicleName )
    {
        delete[] mVehicleName;
        mVehicleName = NULL;
    }
}

//=============================================================================
// BuyCarObjective::SetVehicleName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
void BuyCarObjective::SetVehicleName( const char* name )
{
    int length = strlen( name );

    rTuneAssertMsg( mVehicleName == NULL, "Can not set the name of the car to be purchased twice!\n");

    if ( mVehicleName )
    {
        delete[] mVehicleName;
    }

    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    mVehicleName = new char[ length + 1 ];
    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );

    strcpy( mVehicleName, name );
    mVehicleName[ length ] = '\0';
}

//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// BuyCarObjective::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void BuyCarObjective::OnUpdate( unsigned int elapsedTime )
{
    if ( strcmp( GetGameplayManager()->GetCurrentVehicle()->GetName(), mVehicleName ) == 0 )
    {
        SetFinished( true );
    }
}
