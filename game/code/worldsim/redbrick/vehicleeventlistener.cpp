//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehicleeventlistener.cpp
//
// Description: 
//
// History:     created, Nov 29, 2002, gmayer       
//
//=============================================================================


#include <worldsim/redbrick/vehicleeventlistener.h>

#include <events/eventmanager.h>
#include <events/eventenum.h>

#include <meta/locatorevents.h>
#include <meta/eventlocator.h>
#include <meta/triggervolumetracker.h>

#include <worldsim/redbrick/vehicle.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/parkedcars/parkedcarmanager.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>

#include <gameflow/gameflow.h>

#include <mission/objectives/missionobjective.h>
#include <mission/objectives/destroyobjective.h>

#include <mission/gameplaymanager.h>
#include <supersprint/supersprintmanager.h>

//-----------------------------------------------------------------------------
VehicleEventListener::VehicleEventListener()
{
    mVehicleOwner = 0;
}


//-----------------------------------------------------------------------------
VehicleEventListener::VehicleEventListener(Vehicle* owner)
{
    mVehicleOwner = owner;
    GetEventManager()->AddListener( this, EVENT_DEATH_VOLUME_SCREEN_BLANK );
    GetEventManager()->AddListener( this, (EventEnum)(EVENT_LOCATOR + LocatorEvent::WHACKY_GRAVITY));
    GetEventManager()->AddListener( this, (EventEnum)(EVENT_LOCATOR + LocatorEvent::DEATH) );
}


//-----------------------------------------------------------------------------
VehicleEventListener::~VehicleEventListener()
{
    GetEventManager()->RemoveAll( this );
}


//-----------------------------------------------------------------------------
void VehicleEventListener::HandleEvent( EventEnum id, void* pEventData )
{
    ////////////////////////////////////////////////////////////////
    // WHACKY GRAVITY (FOR JUMP VOLUMES)

    if( id == EVENT_LOCATOR + LocatorEvent::WHACKY_GRAVITY )
    {
        EventLocator* pLocator = static_cast<EventLocator*>( pEventData );
        rAssert( pLocator );

        Vehicle* testVehicle = NULL;
        if ( pLocator->GetPlayerID() < static_cast<unsigned int>(MAX_PLAYERS) )
        {
            //This is a player vehicle
            testVehicle = GetAvatarManager()->GetAvatarForPlayer( pLocator->GetPlayerID() )->GetVehicle();
        }
        else
        {
            testVehicle = GetTriggerVolumeTracker()->GetAI( pLocator->GetPlayerID() );
        }

        if ( testVehicle != mVehicleOwner )
        {
            //If this isn't for me, ignore it.
            return;
        }

        // if in supersprint, ignore jump boost for player
        if( GetGameFlow()->GetCurrentContext() == CONTEXT_SUPERSPRINT && 
            mVehicleOwner->mVehicleType == VT_USER )
        {
            return;
        }

        if ( pLocator->GetPlayerEntered() )
        {
            int stophere = 1;
            // only apply this if we are moderately aligned to it
        
            rmt::Matrix mat = pLocator->GetMatrix();
            rmt::Vector facing = mat.Row(2);
        
            if(mVehicleOwner->mVehicleFacing.DotProduct(facing) > 0.0f)
            {
                mVehicleOwner->EnteringJumpBoostVolume();
            }
        }
        else
        {
            // should be leaving
            int stophere = 1;
            mVehicleOwner->ExitingJumpBoostVolume();            
        }
    }


    ////////////////////////////////////////////////////////////////
    // DEATH VOLUMES
    
    else if( id == EVENT_LOCATOR + LocatorEvent::DEATH )
    {
        EventLocator* pLocator = static_cast<EventLocator*>( pEventData );
        rAssert( pLocator );
        if( pLocator == NULL )
        {
            return;
        }

        Vehicle* testVehicle = NULL;
        if ( pLocator->GetPlayerID() < static_cast<unsigned int>(MAX_PLAYERS) )
        {
            //This is a player vehicle
            testVehicle = GetAvatarManager()->GetAvatarForPlayer( pLocator->GetPlayerID() )->GetVehicle();
        }
        else
        {
            testVehicle = GetTriggerVolumeTracker()->GetAI( pLocator->GetPlayerID() );
        }

        if ( testVehicle != mVehicleOwner )
        {
            //If this isn't for me, ignore it.
            return;
        }

        // Ok, it is for me... 

        GameplayManager::GameTypeEnum gameType = GetGameplayManager()->GetGameType();

        //
        // In supersprint, we want to handle DEATH event for AI & player vehicles,
        // but if in other contexts, we handle DEATH event only for AI (player vehicles
        // will need to listen to the fade to DEATH_VOLUME_SCREEN_BLANK event because
        // we don't want death to be instantaneous)
        //
        VehicleType type = mVehicleOwner->mVehicleType;
        if( ( gameType != GameplayManager::GT_SUPERSPRINT ) &&
            ( type != VT_AI ) )
        {
            // there's some stuff that we want to do right away
            if ( pLocator->GetPlayerEntered() )
            {
                GetEventManager()->TriggerEvent( EVENT_DEATH_VOLUME_SOUND, mVehicleOwner );
            }
            return;
        }

        /*
        //
        // If this is a destroy objective, we want to kill the target AI vehicle 
        // (thus triggering the destroyed condition, completing the mission...
        //
        if( GetGameFlow()->GetCurrentContext() != CONTEXT_SUPERSPRINT )
        {
            MissionStage* ms = GetGameplayManager()->GetCurrentMission()->GetCurrentStage();
            if( ms )
            {
                MissionObjective* mobj = ms->GetObjective();
                if( mobj->GetObjectiveType() == MissionObjective::OBJ_DESTROY )
                {
                    DestroyObjective* dobj = (DestroyObjective*) mobj;
                    if( dobj->GetTargetVehicle() == mVehicleOwner )
                    {
                        mVehicleOwner->mHitPoints = 0.0f;
                        mVehicleOwner->mVehicleDestroyed = true;
                        return;
                    }
                }
            }
        }
        */

        rAssert( mVehicleOwner->mVehicleCentralIndex != -1 );

        // Only do something upon entry
        if ( pLocator->GetPlayerEntered() )
        {
            // We're here if we're a car that the player is driving
            // or if we're an AI car...

            rmt::Matrix mat = pLocator->GetMatrix();

            //Let's turn the car around in specific cases.
            if ( GetGameplayManager()->GetGameType() == GameplayManager::GT_SUPERSPRINT )
            {
                if ( GetSSM()->IsTrackReversed() )
                {
                    rmt::Vector heading;
                    mat.GetHeading( heading );
                    heading *= -1.0f;
                    mat.FillHeadingXZ( heading );  //Turn, turn around
                }

                //
                // Dusit said I should do this here
                //
                GetEventManager()->TriggerEvent( EVENT_DEATH_VOLUME_SOUND, mVehicleOwner );
            }

            // John McLean here,
            // locator is snapped to ground, so we need to spawn at some rest 
            // height above ground. Yippe ka yay!
            rmt::Vector loc;
            pLocator->GetLocation(&loc);
            float yAug = mVehicleOwner->GetRestHeightAboveGround();
            loc.y += yAug;
            mat.Row(3).y = loc.y;

            mVehicleOwner->SetTransform(mat);

            // do reset flags, do reset AI info (if an AI car)
            // don't reset pos (already done manually be setting transform), 
            // don't reset damage states
            mVehicleOwner->ResetFlagsOnly(false); 
        }
    }
    else if( id == EVENT_DEATH_VOLUME_SCREEN_BLANK )
    {
        // Only handle the player vehicle triggering this. This includes:
        //   - player default vehicle (could be under AI control in Demo) in Demo or Normal modes
        //   - any traffic car or parked car or husk that the player is driving

        EventLocator* pLocator = static_cast<EventLocator*>( pEventData );
        rAssert( pLocator );
        if( pLocator == NULL )
        {
            return;
        }

        rAssert( pLocator->GetPlayerID() < static_cast<unsigned int>(MAX_PLAYERS) );

        //This is a player vehicle
        Vehicle* testVehicle = GetAvatarManager()->GetAvatarForPlayer( 
            pLocator->GetPlayerID() )->GetVehicle();

        if( testVehicle != mVehicleOwner )
        {
            //If this isn't for me, ignore it.
            return;
        }

        // Ok, it is for me... 
        rAssert( mVehicleOwner->mVehicleCentralIndex != -1 );
        rAssert( GetGameplayManager()->GetGameType() != GameplayManager::GT_SUPERSPRINT );

        // Only do something upon entry
        if ( pLocator->GetPlayerEntered() )
        {
            // We're here if we're a car that the player is driving
            // or if we're an AI-controller player car (in Demo mode)

            rmt::Matrix mat = pLocator->GetMatrix();

            // John McLean here,
            // locator is snapped to ground, so we need to spawn at some rest 
            // height above ground. Yippe ka yay!
            rmt::Vector loc;
            pLocator->GetLocation(&loc);
            float yAug = mVehicleOwner->GetRestHeightAboveGround();
            loc.y += yAug;
            mat.Row(3).y = loc.y;

            //
            // what if there is already a car at the location we want to put htis car
            //
            const rmt::Vector& position = mat.Row( 3 );
            rmt::Sphere s;
            s.centre = position;
            s.radius = 4.0;
            TrafficManager::GetInstance()->ClearTrafficInSphere( s );
            GetGameplayManager()->DumpCurrentCarIfInSphere( s );
            GetPCM().RemoveFreeCarIfClose( position );

            mVehicleOwner->SetTransform(mat);

            // do reset flags, do reset AI info (if an AI car)
            // don't reset pos (already done manually be setting transform), 
            // don't reset damage states
            mVehicleOwner->ResetFlagsOnly(false); 
        }
    }
}

