//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        aivehiclesoundplayer.cpp
//
// Description: Administers the playing of sound for an AI-controlled vehicle
//
// History:     1/4/2003 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <sound/movingpositional/aivehiclesoundplayer.h>

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
// AIVehicleSoundPlayer::AIVehicleSoundPlayer
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
AIVehicleSoundPlayer::AIVehicleSoundPlayer()
{
}

//=============================================================================
// AIVehicleSoundPlayer::~AIVehicleSoundPlayer
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
AIVehicleSoundPlayer::~AIVehicleSoundPlayer()
{
}

//=============================================================================
// AIVehicleSoundPlayer::ServiceOncePerFrame
//=============================================================================
// Description: Adjust the pitch for this vehicle
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void AIVehicleSoundPlayer::ServiceOncePerFrame()
{
    float pitch;

    VehiclePositionalSoundPlayer::ServiceOncePerFrame();

    //
    // Adjust pitch for vehicle speed if desired.  I'll probably need to
    // expose this for designer tuning later.
    //
    if( IsActive() && m_tiePitchToVelocity )
    {
        pitch = 0.3f + ( 0.7f * ( m_vehicle->GetSpeedKmh() / m_vehicle->mDesignerParams.mDpTopSpeedKmh ) );
        if( pitch > 1.2f )
        {
            pitch = 1.2f;
        }

        m_player.SetPitch( pitch );
    }
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
