//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        StaticCamLocator.cpp
//
// Description: Implement StaticCamLocator
//
// History:     9/18/2002 + Created -- Cary Brisebois
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
#include <meta/StaticCamLocator.h>

#include <camera/staticcam.h>
#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>
#include <camera/isupercamtarget.h>

#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>

#include <interiors/interiormanager.h>

const tUID hypecam = tEntity::MakeUID("hypecam");
const tUID ufocam = tEntity::MakeUID("z3_ufocam");

//AAAAARRRRRGH, this code won't work for 2 players....

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
extern const float MIN_SPEED_FOR_CAM_TRIGGER = 20.0f;

//******************************************************************************
//
// Public Member Functions 
//
//******************************************************************************

//==============================================================================
// StaticCamLocator::StaticCamLocator
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
StaticCamLocator::StaticCamLocator() :
    mStaticCam( NULL ),
    mCamNum( -1 ),
    mTriggerCount( 0 ),
    mLastSuperCam( NULL ),
    mOneShot( false ),
    mOneShotted( false ),
    mCarOnly( false ),
    mOnFootOnly( false ),
    mCutInOut( false )
{
}

//==============================================================================
// StaticCamLocator::~StaticCamLocator
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
StaticCamLocator::~StaticCamLocator()
{
    if ( hypecam == GetUID() )
    {
        GetSuperCamManager()->GetSCC( 0 )->NastyHypeCamHackEnable( false );
    }

    if ( mStaticCam )
    {
        if ( mStaticCam->IsRegistered() )
        {
            GetSuperCamManager()->GetSCC( 0 )->UnregisterSuperCam( mStaticCam );
            mCamNum = -1;
        }

        mStaticCam->Release();
        mStaticCam = NULL;
    }

    if ( mLastSuperCam )
    {
        mLastSuperCam->Release();
        mLastSuperCam = NULL;
    }
}

//=============================================================================
// StaticCamLocator::SetRailCam
//=============================================================================
// Description: Comment
//
// Parameters:  ( StaticCam* railCam )
//
// Return:      void 
//
//=============================================================================
void StaticCamLocator::SetStaticCam( StaticCam* cam )
{
    rAssert( mStaticCam == NULL );
    mStaticCam = cam;
    mStaticCam->AddRef();
}

//=============================================================================
// StaticCamLocator::TriggerAllowed
//=============================================================================
// Description: Comment
//
// Parameters:  ( int playerID )
//
// Return:      bool 
//
//=============================================================================
bool StaticCamLocator::TriggerAllowed( int playerID )
{
    //Always allow the triggering of the UFO cam
    if ( GetUID() == ufocam )
    {
        return true;
    }

    Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer( playerID );
    float speed = playerAvatar->GetSpeedMps();
    bool isInCar = playerAvatar->IsInCar();
    SuperCamCentral* scc = GetSuperCamManager()->GetSCC( playerID );

    if ( ( mCarOnly && ( !isInCar || speed < MIN_SPEED_FOR_CAM_TRIGGER ) ) ||
         ( mOnFootOnly && isInCar ) ||
         ( !mOnFootOnly && !scc->JumpCamsEnabled() ) ||
         !scc->AllowAutoCameraChange() ) 
    {
        return false;
    }

    return true;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// StaticCamLocator::OnTrigger
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int playerID )
//
// Return:      void 
//
//=============================================================================
void StaticCamLocator::OnTrigger( unsigned int playerID )
{
BEGIN_PROFILE( "SCL OnTrigger" );
    SuperCamCentral* scc = GetSuperCamManager()->GetSCC( playerID );

    //Test the one shottedness of this trigger.
    if ( mOneShot )
    {
        if ( mOneShotted )
        {
            END_PROFILE( "SCL OnTrigger" );
            return;
        }
        else if ( !GetPlayerEntered() ) //Only set this on leaving the volume
        {
            mOneShotted = true;
        }
    }

    unsigned int transitionTime = 1000;
    int extraFlags = 0;

    InteriorManager* im = GetInteriorManager();

    if ( mCutInOut || scc->GetTarget()->IsCar() ||
         im->IsEntering() || im->IsExiting() )
    {
        transitionTime = 0;
        extraFlags = SuperCamCentral::CUT;
    }

    if ( GetSuperCamManager()->GetSCC( playerID )->IsInitialCamera())
    {
        transitionTime = 0;
        extraFlags = SuperCamCentral::CUT;
        GetSuperCamManager()->GetSCC( playerID )->SetIsInitialCamera(false);
    }


    if( GetPlayerEntered() )
    {
        if ( mTriggerCount == 0 )
        {
            //Add only once.
            mCamNum = scc->RegisterSuperCam( mStaticCam );
        }

        //Let's not go back to ourself if these are the same.
        if ( scc->GetActiveSuperCam() != mStaticCam )
        {
            //Get the last cam
            tRefCounted::Assign( mLastSuperCam, scc->GetActiveSuperCam() );

            if ( mLastSuperCam->GetType() == SuperCam::RAIL_CAM ||
                mLastSuperCam->GetType() == SuperCam::STATIC_CAM )
            {
                //Going from rail to rail
                scc->SelectSuperCam( mCamNum, SuperCamCentral::QUICK | SuperCamCentral::FORCE | extraFlags, transitionTime ); 
            }
            else
            {
                scc->SelectSuperCam( mCamNum, SuperCamCentral::FORCE | extraFlags, transitionTime ); 
            }
        }

        //This is for the frickin' camera on level 6 where the user should never
        //go, but can and will enevitably fuck the camera system.
        if ( hypecam == this->GetUID() )
        {
            scc->NastyHypeCamHackEnable( true );
        }

        ++mTriggerCount;
    }
    else
    {
        if ( mTriggerCount > 0 )
        {
            mTriggerCount--;

            if ( mTriggerCount == 0 )
            {
                //This is for the frickin' camera on level 6 where the user should never
                //go, but can and will enevitably fuck the camera system.
                if ( hypecam == this->GetUID() )
                {
                    scc->NastyHypeCamHackEnable( false );
                }

                bool isActive = scc->GetActiveSuperCam() == mStaticCam;

                if ( isActive )
                {
                    //Test to make sure the last cam we knew of is still registered
                    //with the super cam central
                    if ( mLastSuperCam && mLastSuperCam->IsRegistered() )
                    {
                        if ( mLastSuperCam != NULL &&
                            ( mLastSuperCam->GetType() == SuperCam::RAIL_CAM ||
                            mLastSuperCam->GetType() == SuperCam::STATIC_CAM ) )
                        {
                            //Going from rail to rail
                            scc->SelectSuperCam( mLastSuperCam, SuperCamCentral::QUICK | SuperCamCentral::FORCE | extraFlags, transitionTime );
                        }
                    }
                }

                if ( mCutInOut || scc->GetTarget()->IsCar() )
                {
                    scc->DoCameraCut();
                }

                tRefCounted::Release(mLastSuperCam);
                mLastSuperCam = NULL;
                scc->UnregisterSuperCam( mCamNum );

                mCamNum = -1;
            }
        }
    }
END_PROFILE( "SCL OnTrigger" );
}
	
