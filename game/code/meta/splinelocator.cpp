//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement SplineLocator
//
// History:     05/06/2002 + Created -- Cary Brisebois
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
#include <meta/splinelocator.h>

#include <camera/supercam.h>
#include <camera/supercammanager.h>
#include <camera/railcam.h>
#include <camera/isupercamtarget.h>

#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h> 

#include <interiors/interiormanager.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
extern const float MIN_SPEED_FOR_CAM_TRIGGER;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// SplineLocator::SplineLocator
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
SplineLocator::SplineLocator() :
    mRailCam( NULL ),
    mRailNum( -1 ),
    mTriggerCount( 0 ),
    mLastSuperCam( NULL ),
    mCarOnly( false ),
    mOnFootOnly( false ),
    mCutInOut( false ),
    mReset( false )
{
}

//==============================================================================
// SplineLocator::~SplineLocator
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
SplineLocator::~SplineLocator()
{
    if ( mRailCam )
    {
        if ( mRailCam->IsRegistered() )
        {
            SuperCamCentral* scc = GetSuperCamManager()->GetSCC( 0 );
            scc->UnregisterSuperCam( mRailCam );
            mRailNum = -1;
        }

        tRefCounted::Release(mRailCam);
    }

    tRefCounted::Release(mLastSuperCam);

}

//=============================================================================
// SplineLocator::SetRailCam
//=============================================================================
// Description: Comment
//
// Parameters:  ( RailCam* railCam )
//
// Return:      inline 
//
//=============================================================================
void SplineLocator::SetRailCam( RailCam* railCam )
{
    rAssert( mRailCam == NULL );
    mRailCam = railCam;
    mRailCam->AddRef();
}

//=============================================================================
// SplineLocator::TriggerAllowed
//=============================================================================
// Description: Comment
//
// Parameters:  ( int playerID )
//
// Return:      bool 
//
//=============================================================================
bool SplineLocator::TriggerAllowed( int playerID )
{
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
// SplineLocator::OnTrigger
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int playerID )
//
// Return:      void 
//
//=============================================================================
void SplineLocator::OnTrigger( unsigned int playerID )
{
    rAssert(playerID == 0); // shouldn' be multiple players right now, and if there were, the shutdown logic would break

BEGIN_PROFILE( "SL OnTrigger" );
    SuperCamCentral* scc = GetSuperCamManager()->GetSCC( playerID );

    //Don't trigger on enter if the conditions are not met.  Allow the on exit trigger though in any case.

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
            mRailNum = scc->RegisterSuperCam( mRailCam );
        }

        if ( mReset )
        {
            mRailCam->SetReset( mReset );
        }

        //Let's not go back to ourself if these are the same.
        if ( scc->GetActiveSuperCam() != mRailCam )
        {
            //Get the last cam
            tRefCounted::Assign( mLastSuperCam, scc->GetActiveSuperCam() );

            if ( mLastSuperCam->GetType() == SuperCam::RAIL_CAM ||
                mLastSuperCam->GetType() == SuperCam::STATIC_CAM )
            {
                //Going from rail to rail
                scc->SelectSuperCam( mRailNum, SuperCamCentral::QUICK | SuperCamCentral::FORCE | extraFlags, transitionTime ); 
            }
            else
            {
                scc->SelectSuperCam( mRailNum, SuperCamCentral::FORCE | extraFlags, transitionTime ); 
            }
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
                bool isActive = scc->GetActiveSuperCam() == mRailCam;

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
                scc->UnregisterSuperCam( mRailNum );

                mRailNum = -1;
            }
        }
    }
END_PROFILE( "SL OnTrigger" );
}
