//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implementation of class AvatarManager
//
// History:     4/3/2002 + Created -- NAME
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
#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>
#include <memory/srrmemory.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>
#include <mission/gameplaymanager.h>
#include <events/eventenum.h>
#include <events/eventmanager.h>
#include <meta/locatorevents.h>
#include <meta/eventlocator.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guiwindow.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreeniriswipe.h>
#include <input/inputmanager.h>

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>
#include <camera/supercam.h>

#include <gameflow/gameflow.h>
#include <contexts/context.h>

#include <ai/sequencer/actioncontroller.h>
#include <supersprint/supersprintmanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
AvatarManager* AvatarManager::spAvatarManager = 0;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

AvatarManager* AvatarManager::CreateInstance( void )
{
	rAssertMsg( spAvatarManager == 0, "AvatarManager already created.\n" );
	spAvatarManager = new ( GMA_PERSISTENT ) AvatarManager;
    return AvatarManager::GetInstance();
}

AvatarManager* AvatarManager::GetInstance( void )
{
	rAssertMsg( spAvatarManager != 0, "AvatarManager has not been created yet.\n" );
	return spAvatarManager;
}


void AvatarManager::DestroyInstance( void )
{
	rAssertMsg( spAvatarManager != 0, "AvatarManager has not been created.\n" );
	delete ( GMA_PERSISTENT, spAvatarManager );
}
//==============================================================================
// AvatarManager::AvatarManager
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
AvatarManager::AvatarManager()
:
mNumAvatars( 0 )
{
}

//==============================================================================
// AvatarManager::~AvatarManager
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
AvatarManager::~AvatarManager()
{
    Destroy( );
}

void AvatarManager::Destroy( void )
{
    int i;
    for ( i = 0; i < mNumAvatars; i++ )
    {
        mAvatarArray[ i ]->Destroy();
        delete mAvatarArray[i];
    }
    mNumAvatars = 0;

    GetEventManager()->RemoveAll( this );
}


/*
==============================================================================
AvatarManager::EnterGame
==============================================================================
Description:    This will be called to create the avatars just before the 
                game session begins.

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void AvatarManager::EnterGame( void )
{

    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );

    GetEventManager()->AddListener( this, EVENT_GETINTOVEHICLE_START );
    GetEventManager()->AddListener( this, EVENT_GETINTOVEHICLE_END );
    GetEventManager()->AddListener( this, EVENT_GETOUTOFVEHICLE_START );
    GetEventManager()->AddListener( this, EVENT_GETOUTOFVEHICLE_END );
    GetEventManager()->AddListener( this, EVENT_CAMERA_CHANGE );

    int i;
    for (i = 0; i < MAX_AVATARS; i++)
    {
        mAvatarArray[i] = 0;
    }
    mNumAvatars = GetGameplayManager()->GetNumPlayers();

    for ( i = 0; i < mNumAvatars; i++ )
    {
        mAvatarArray[i] = new Avatar;

        Vehicle* pVehicle = GetVehicleCentral()->GetVehicle( i );// GetNewVehicle();
        int controllerID = GetInputManager()->GetControllerIDforPlayer( i );
        rWarningMsg( controllerID != -1, "No controller ID registered for player avatar!" );

        mAvatarArray[ i ]->SetControllerId( controllerID );
        mAvatarArray[ i ]->SetPlayerId( i );
        Character* pCharacter = GetCharacterManager( )->GetCharacter( i );
        rAssert( pCharacter );
        mAvatarArray[ i ]->SetCharacter( pCharacter );
        rmt::Vector characterPosition;

        // Single player set up.
        //
        pCharacter->SetInCar( false );
        mAvatarArray[ i ]->SetVehicle( (Vehicle*)0 );
        mAvatarArray[ i ]->SetCameraTargetToCharacter( true );
        mAvatarArray[ i ]->SetOutOfCarController( );

        //Chuck Adding support for loading of the last used skin for the level

        if ( pVehicle )
        {
            // Hack.  MS8.
            // Position the character near the vehicle.
            //
            characterPosition.Set( 3.1f, 0.0f, 0.0f );

            rmt::Matrix carToWorld = pVehicle->GetTransform();
            characterPosition.Transform( carToWorld );
            pCharacter->SetPosition( characterPosition );
            pCharacter->UpdateTransformToLoco();
        }
    }   

    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
}

void AvatarManager::ExitGame ()
{
    Destroy ();
}

void AvatarManager::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
    case EVENT_GETINTOVEHICLE_START:
        {
            Character* pCharacter = static_cast<Character*>( pEventData );
            Avatar* pAvatar = FindAvatarForCharacter( pCharacter );
            if ( pAvatar )
            {
                Vehicle* pVehicle = pCharacter->GetTargetVehicle();
                pAvatar->GetIntoVehicleStart( pVehicle );
            }
            break;
        }
    case EVENT_GETINTOVEHICLE_END:
        {
            Character* pCharacter = static_cast<Character*>( pEventData );
            Avatar* pAvatar = FindAvatarForCharacter( pCharacter );
            if ( pAvatar )
            {
                Vehicle* pVehicle = pCharacter->GetTargetVehicle();
                pAvatar->GetIntoVehicleEnd( pVehicle );
            }
            break;
        }
    case EVENT_GETOUTOFVEHICLE_START:
        {
            Character* pCharacter = static_cast<Character*>( pEventData );
            Avatar* pAvatar = FindAvatarForCharacter( pCharacter );
            if ( pAvatar )
            {
                Vehicle* pVehicle = pAvatar->GetVehicle();
                pAvatar->GetOutOfVehicleStart( pVehicle );
            }
            break;
        }    
    case EVENT_GETOUTOFVEHICLE_END:
        {
            Character* pCharacter = static_cast<Character*>( pEventData );
            Avatar* pAvatar = FindAvatarForCharacter( pCharacter );
            if ( pAvatar )
            {
                Vehicle* pVehicle = pAvatar->GetVehicle();
                pAvatar->GetOutOfVehicleEnd( pVehicle );
            }
            break;
        }
    case EVENT_CAMERA_CHANGE:
        {
            SuperCam* sc = static_cast<SuperCam*>(pEventData);

            int playerID = 0; // normal game defaults player to zero
            if( GetGameFlow()->GetCurrentContext() == CONTEXT_LOADING_SUPERSPRINT ||
                GetGameFlow()->GetCurrentContext() == CONTEXT_SUPERSPRINT )
            {
                playerID = SuperSprintManager::GetInstance()->GetOnlyHumanPlayerID();
                /*
                if( GetAvatarForPlayer( playerID )->GetPlayerId() != playerID )
                {
                    break;
                }
                */
            }

            // can be -1 if in supersprint and more than 1 human is participating...
            if( playerID == -1 ) 
            {
                break;
            }

            if ( sc == GetSuperCamManager()->GetSCC( 0 )->GetActiveSuperCam() )
            {
                //Find the vehicle owned by this car.
                //Vehicle* car = GetGameplayManager()->GetCurrentVehicle();
                Vehicle* car = GetAvatarForPlayer( playerID )->GetVehicle();
                

                if ( car )
                {
                    if ( sc->GetType() == SuperCam::BUMPER_CAM )
                    {
                        //Only stop rendering for bumpercams.
                        car->DrawVehicle( false );
                    }
                    else
                    {
                        car->DrawVehicle( true );
                    }
                }
            }

            break;
        }
    default:
        {
            break;
        }
    }
}

Avatar* AvatarManager::GetAvatarForPlayer( int playerId )
{
    if( playerId < this->mNumAvatars )
    {
        return mAvatarArray[ playerId ];
    }

    return( NULL );
}

//=============================================================================
// AvatarManager::PutCharacterInCar
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* pCharacter, Vehicle* pVehicle )
//
// Return:      void 
//
//=============================================================================
void AvatarManager::PutCharacterInCar( Character* pCharacter, Vehicle* pVehicle )
{
    rAssert( pCharacter != NULL );
    rAssert( pVehicle != NULL );

    int i;
    for ( i = 0; i < mNumAvatars; i++ )
    {
        if ( mAvatarArray[ i ]->GetCharacter( ) == pCharacter )
        {
            if(pCharacter->IsInCar() && (pCharacter->GetTargetVehicle() == pVehicle))
            {
                return;
            }

            pCharacter->GetActionController()->Clear();

            pCharacter->SetInCar( true );
            pCharacter->UpdateTransformToInCar( );
            pCharacter->SetTargetVehicle( pVehicle );

            // transit the vehicle into VL_PHYSICS!
            if(pVehicle->GetLocomotionType() == VL_TRAFFIC)
            {
                pVehicle->SetLocomotion(VL_PHYSICS);
                pVehicle->mHijackedByUser = true;
            }

            mAvatarArray[ i ]->SetVehicle( pVehicle );
            mAvatarArray[ i ]->SetCameraTargetToVehicle( true ); //CUT the camera
            mAvatarArray[ i ]->SetInCarController( );
           
            pCharacter->UpdateTransformToInCar();
            if(pCharacter->GetStateManager()->GetState() == CharacterAi::INCAR)
            {
                pCharacter->GetStateManager()->ResetState();
            }
            else
            {
                pCharacter->GetStateManager()->SetState<CharacterAi::InCar>();
                //pCharacter->GetStateManager()->Update( 16 );
            }

            pCharacter->SetDesiredSpeed(0.0f);

            if ( GetCurrentHud() )
            {
                GetCurrentHud()->GetHudMap( i )->UnregisterIcon( 0 );
                GetCurrentHud()->GetHudMap( i )->RegisterIcon( HudMapIcon::ICON_PLAYER,
                    rmt::Vector( 0, 0, 0 ),
                    pVehicle );
            }

            return;
        }
    }
}

//=============================================================================
// AvatarManager::PutCharacterOnGround
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* pCharacter, Vehicle* pVehicle )
//
// Return:      void 
//
//=============================================================================
void AvatarManager::PutCharacterOnGround( Character* pCharacter, Vehicle* pVehicle )
{
    rAssert( pCharacter != NULL );
    rAssert( pVehicle != NULL );

    int i;
    for ( i = 0; i < mNumAvatars; i++ )
    {
        if ( mAvatarArray[ i ]->GetCharacter( ) == pCharacter )
        {
            mAvatarArray[ i ]->GetOutOfVehicleStart( pVehicle );
            mAvatarArray[ i ]->GetOutOfVehicleEnd( pVehicle );

            pCharacter->SetInCar( false );

            pCharacter->UpdateTransformToLoco();
            pCharacter->GetStateManager()->SetState<CharacterAi::Loco>();
            pCharacter->GetStateManager()->Update( 16 );

            return;
        }
    }
}

/*
==============================================================================
AvatarManager::Update
==============================================================================
Description:    Comment

Parameters:     (float dt)

Return:         void 

=============================================================================
*/
void AvatarManager::Update(float dt)
{
    int i;
    for ( i = 0; i < mNumAvatars; i++ )
    {
        mAvatarArray[ i ]->Update( dt );
    }
}
//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

Avatar* AvatarManager::FindAvatarForCharacter( Character* pCharacter )
{
    int i;
    for ( i = 0; i < mNumAvatars; i++ )
    {
        if ( mAvatarArray[ i ]->GetCharacter( ) == pCharacter )
        {
            return mAvatarArray[ i ];
        }
    }
    return 0;
}


//=============================================================================
// AvatarManager::GetAvatarForVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  (Vehicle* vehicle)
//
// Return:      Avatar
//
//=============================================================================
Avatar* AvatarManager::GetAvatarForVehicle(Vehicle* vehicle)
{
    int i;
    for ( i = 0; i < mNumAvatars; i++ )
    {
        if (mAvatarArray[i]->GetVehicle() == vehicle)
        {
            return mAvatarArray[i];
        }
    }
    return 0;
}

//=============================================================================
// AvatarManager::IsAvatarGettingInOrOutOfCar
//=============================================================================
// Description: returns  true if the chacter for playerId is in a get into or get out of car transition state.
//
// Parameters:  int playerId
//
// Return:      bool 
//
//=============================================================================
bool AvatarManager::IsAvatarGettingInOrOutOfCar(int playerId)
{
   if (     
       GetAvatarForPlayer(0)->GetCharacter()->GetStateManager()->GetState() == CharacterAi::GET_IN
            ||
                GetAvatarForPlayer(0)->GetCharacter()->GetStateManager()->GetState() == CharacterAi::GET_OUT
       )

   {
       return true;
   }
   else
   {
       return false;
   }
}

            


