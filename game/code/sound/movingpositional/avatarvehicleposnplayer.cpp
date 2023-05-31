//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        avatarvehicleposnplayer.cpp
//
// Description: Implement AvatarVehiclePosnPlayer
//
// History:     3/7/2003 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radnamespace.hpp>

//========================================
// Project Includes
//========================================
#include <sound/movingpositional/avatarvehicleposnplayer.h>

#include <sound/avatar/carsoundparameters.h>
#include <sound/soundrenderer/soundrenderingmanager.h>

#include <events/eventmanager.h>
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
// AvatarVehiclePosnPlayer::AvatarVehiclePosnPlayer
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
AvatarVehiclePosnPlayer::AvatarVehiclePosnPlayer()
{
    //
    // Register events
    //
    GetEventManager()->AddListener( this, EVENT_GETINTOVEHICLE_END );
    GetEventManager()->AddListener( this, EVENT_GETOUTOFVEHICLE_END );
    GetEventManager()->AddListener( this, EVENT_USER_VEHICLE_ADDED_TO_WORLD );
    GetEventManager()->AddListener( this, EVENT_USER_VEHICLE_REMOVED_FROM_WORLD );
    GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED );
}

//=============================================================================
// AvatarVehiclePosnPlayer::~AvatarVehiclePosnPlayer
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
AvatarVehiclePosnPlayer::~AvatarVehiclePosnPlayer()
{
    GetEventManager()->RemoveAll( this );
}

//=============================================================================
// AvatarVehiclePosnPlayer::HandleEvent
//=============================================================================
// Description: Listen for events directing us to turn positional idle
//              sound on and off
//
// Parameters:  id - identifier for event being listened for
//              pEventData - unused
//
// Return:      void 
//
//=============================================================================
void AvatarVehiclePosnPlayer::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
        case EVENT_GETOUTOFVEHICLE_END:
            //Chuck: adding this for the stupid UFO beam, and all the wierdness that it generates. 
            if( GetGameplayManager()->GetCurrentVehicle()->IsVehicleDestroyed() == true)
            {
                Deactivate();
                return;
            }
            StartPositionalIdle();
            break;
        case EVENT_VEHICLE_DESTROYED:
            {
                if(m_vehicle == static_cast<Vehicle*>(pEventData))
                {
                    Deactivate();
                }
                break;
            }


        case EVENT_GETINTOVEHICLE_END:
            Deactivate();
            break;

        case EVENT_USER_VEHICLE_ADDED_TO_WORLD:
            StartPositionalIdle( static_cast<Vehicle*>(pEventData) );
            break;

        case EVENT_USER_VEHICLE_REMOVED_FROM_WORLD:
            Deactivate();
            break;

        default:
            rAssertMsg( false, "Unexpected event in AvatarVehiclePosnPlayer::HandleEvent\n" );
            break;
    }
}

//=============================================================================
// AvatarVehiclePosnPlayer::StartPositionalIdle
//=============================================================================
// Description: Activate this player with an idle engine clip in the vehicle
//              position
//
// Parameters:  carPtr - if vehicle is already known, this is non-NULL
//
// Return:      void 
//
//=============================================================================
void AvatarVehiclePosnPlayer::StartPositionalIdle( Vehicle* carPtr /* = NULL  */)
{
    Vehicle* theCar;
    IRadNameSpace* nameSpace;
    IRefCount* nameSpaceObj;
    carSoundParameters* parameters;
    positionalSoundSettings* soundSettings;

    //
    // Get name of idle sound clip
    //
    if( GetGameplayManager()->GetNumPlayers() == 1 )
    {
        if( carPtr != NULL )
        {
            theCar = carPtr;
        }
        else
        {
            theCar = GetGameplayManager()->GetCurrentVehicle();
            rAssert( theCar != NULL );
            
            if(!theCar)
            {
                rDebugString( "Couldn't find avatar vehicle\n" );
                return;
            }
        }

        nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
        rAssert( nameSpace != NULL );

        nameSpaceObj = nameSpace->GetInstance( theCar->GetName() );
        if( nameSpaceObj != NULL )
        {
            parameters = reinterpret_cast<carSoundParameters*>( nameSpaceObj );

            //
            // Now find some positional sound settings
            //
            nameSpaceObj = nameSpace->GetInstance( "avatar_idle" );
            if( nameSpaceObj != NULL )
            {
                soundSettings = reinterpret_cast<positionalSoundSettings*>( nameSpaceObj );

                Activate( soundSettings, parameters->GetEngineIdleClipName(), theCar );

                m_player.SetPitch( parameters->GetIdleEnginePitch() );
            }
            else
            {
                rDebugString( "Couldn't find positional settings for avatar vehicle\n" );
            }
        }
        else
        {
            rDebugString( "Couldn't find carSoundParameters for avatar vehicle\n" );
        }
    }
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************

