//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        pedestrian.cpp
//
// Description: Implements Pedestrian class
//
// History:     09/18/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================
#include <string.h>
#include <raddebug.hpp> // for rAssert & other debug print outs
#include <radmath/radmath.hpp> // for rmt::Vector 

#include <worldsim/ped/pedestrian.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>
#include <worldsim/avatar.h>
#include <worldsim/avatarmanager.h>

#include <camera/supercammanager.h>
#include <camera/supercam.h>

#include <pedpaths/path.h>
#include <pedpaths/pathsegment.h>

#include <render/RenderManager/RenderManager.h>
#include <render/Culling/WorldScene.h>
#include <render/IntersectManager/IntersectManager.h>

#include <ai/sequencer/action.h>
#include <input/inputmanager.h>

#include <mission/gameplaymanager.h>

static const float SECONDS_BETW_ACTIVATE_POLLS = 0.0f;//0.5f;
static const float PEDESTRIAN_PATHFIND_BACK_SPEED_MPS = 1.0f;
static const float PEDESTRIAN_WALK_SPEED_MPS = 1.0f;
static const float PEDESTRIAN_BRISK_WALK_SPEED_MPS = 1.5f;
static const float PEDESTRIAN_RUN_SPEED_MPS = 3.0f;

Pedestrian::Pedestrian() :
    mMillisecondsOutOfSight( 0 ),
    mIsActive( false ),
    mPath( NULL ),
    mFollowPathSpeedMps( 0.0f ),
    mMarkedForActivation( false ),
    mSecondsTillActivateSelf( 0.0f )
{
}

Pedestrian::~Pedestrian()
{
}


/////////////////////////////////////////////////////////////////////////
// CharacterController
/////////////////////////////////////////////////////////////////////////
void Pedestrian::Update( float seconds )
{
    if( mMarkedForActivation )
    {
        rAssert( !mIsActive );

        
        //mSecondsTillActivateSelf -= seconds;
        //if( mSecondsTillActivateSelf < 0.0f )
        //{
        //    mSecondsTillActivateSelf = SECONDS_BETW_ACTIVATE_POLLS;
            if( ::GetCharacterManager()->IsModelLoaded( mModelName ) )
            {
                ActivateSelf();
            }
        //}
        else
        {
            // don't update if marked for activation but not yet activated
            return;
        }
    }

    // don't update if not active!
    if( !mIsActive )
    {
        return;
    }
    BEGIN_PROFILE( "Pedestrian::Update" );

    if( mState == NPCController::FOLLOWING_PATH )
    {

        // Detect if another ped is near me
        // if so, deal with "walk through one another" scenario if we're 
        // close enough to the player 

        rmt::Vector myPos;
        GetCharacter()->GetPosition( &myPos );

        rmt::Vector playerPos;
        GetAvatarManager()->GetAvatarForPlayer( 0 )->GetPosition( playerPos );

        const float MIN_DIST_SQR_FROM_PLAYER = 1225.0f;
        if( (myPos - playerPos).MagnitudeSqr() <= MIN_DIST_SQR_FROM_PLAYER ||
            GetInputManager()->GetGameState() == Input::ACTIVE_FIRST_PERSON )
        {

            int i = PedestrianManager::GetInstance()->mActivePeds.GetHead();
            while( 0 <= i && i < DListArray::MAX_ELEMS )
            {
                PedestrianManager::PedestrianStruct* pedStr = (PedestrianManager::PedestrianStruct*) 
                    PedestrianManager::GetInstance()->mActivePeds.GetDataAt(i);
                rAssert( pedStr );
                
                Pedestrian* ped = pedStr->ped;
                rAssert( ped );

                if( !ped->GetIsActive() ) 
                {
                    i = PedestrianManager::GetInstance()->mActivePeds.GetNextOf( i );
                    continue;
                }

                if( ped != this )
                {
                    rmt::Vector pedPos;
                    ped->GetCharacter()->GetPosition( &pedPos );

                    rmt::Vector toPed = pedPos - myPos;

                    static const float MINDISTSQR_FROM_ANOTHER_PED_SMALL = 2.25f;
                    static const float MINDISTSQR_FROM_ANOTHER_PED_LARGE = 4.00f;

                    float distToleranceFromAnotherPedSqr = (mFollowPathSpeedMps > 1.0f) ?
                        MINDISTSQR_FROM_ANOTHER_PED_SMALL : 
                        MINDISTSQR_FROM_ANOTHER_PED_LARGE;

                    if( toPed.MagnitudeSqr() < distToleranceFromAnotherPedSqr )
                    {
                        // here, we're too close to another ped on same path...
                        // If he's just following path as well, engage him in conversation!
                        if( ped->GetState() == NPCController::FOLLOWING_PATH )
                        {
                            // determine direction
                            toPed.Normalize(); // *** SQUARE ROOT! ***

                            // tell myself to transit to talking
                            TransitToState( NPCController::TALKING );
                            mTalkTarget = ped;
                            SetDirection( toPed );
                            mpCharacter->SetDesiredDir( choreo::GetWorldAngle(toPed.x,toPed.z) );

                            // tell him to transit to talking 
                            toPed.Scale( -1.0f );
                            ped->TransitToState( NPCController::TALKING );
                            ped->mTalkTarget = this;
                            ped->SetDirection( toPed );
                            ped->GetCharacter()->SetDesiredDir( choreo::GetWorldAngle(toPed.x,toPed.z) );

                            StartTalking();

                            break;
                        }
                        else if( ped->GetState() == NPCController::PANICKING )
                        {   
                            // He's... um.. panicking... We'd better panick too
                            IncitePanic();    
                            break;
                        }
                        else
                        {
                            // he's not available for talking, but he's still nearby
                            // check if he's in front of us. If so, STAND and wait.

                            rmt::Vector forward, right, up;
                            up.Set( 0.0f, 1.0f, 0.0f );

                            mpCharacter->GetFacing( forward );
                            rAssert( rmt::Epsilon(forward.MagnitudeSqr(), 1.0f, 0.001f ) );

                            right.CrossProduct( up, forward );
                            bool pedOnMyRight = false;
                            bool gonnaHit = WillCollide( myPos, 
                                forward, // heading (forward)
                                right, // right vector
                                1.0f, 3.0f, pedPos, pedOnMyRight );

                            if( gonnaHit )
                            {
                                TransitToState( STANDING );
                                break;
                            }
                        }
                    }
                }
                i = PedestrianManager::GetInstance()->mActivePeds.GetNextOf( i );
            } // end of WHILE
        }
    }
    NPCController::Update( seconds );

    END_PROFILE( "Pedestrian::Update" );

}

float Pedestrian::GetValue( int buttonId ) const
{
    return 0.0f;
}

bool Pedestrian::IsButtonDown( int buttonId ) const
{
    return false;
}
/////////////////////////////////////////////////////////////////////////

 

void Pedestrian::Activate( 
    Path* path, 
    PathSegment* pathSegment, 
    rmt::Vector spawnPos,
    const char* modelName
    )

{
    rAssert( path );
    rAssert( pathSegment );
    rAssert( modelName );
    rAssert( path->IsClosed() );

    mStartPanicking = false;

    // set where I am on the path
    mPath = path;

    /////////////////////////////////////////////////////////
    // Add NPC Waypoints based on the path given...
    int numSegs = path->GetNumPathSegments();  
    int numPts = numSegs; // a closed path has same number of points as segments

    rAssert( 1 < numPts && numPts <= MAX_NPC_WAYPOINTS );

    int currSegIndex = pathSegment->GetIndexToParentPath();

    mNumNPCWaypoints = 0;
    while( mNumNPCWaypoints < numPts )
    {
        rmt::Vector segEndPos;
        pathSegment->GetEndPos( segEndPos );

        AddNPCWaypoint( segEndPos );

        // next segment!
        currSegIndex = ( currSegIndex + 1 ) % numSegs;
        rAssert( 0 <= currSegIndex && currSegIndex < numSegs );
        pathSegment = path->GetPathSegmentByIndex( currSegIndex );
    }
    mCurrNPCWaypoint = 0;


    //////////////////////////////////////////////////////////
    rAssert( modelName != NULL );
    int modelNameLen = strlen(modelName);
    rAssert( modelNameLen <= Pedestrian::MAX_STRING_LEN );
    strncpy( mModelName, modelName, Pedestrian::MAX_STRING_LEN );
    mModelName[ modelNameLen ] = '\0';

    mMarkedForActivation = true;
    mSecondsTillActivateSelf = SECONDS_BETW_ACTIVATE_POLLS;

    // set the Character's new spawn position (so whenever we do 
    // a Character::GetPosition, we get the correct position)
    GetCharacter()->RelocateAndReset(spawnPos, 0.0f);

    // diff between spawnPos & actual character position at this point
    // is the ground fix up... spawnpos is the actual ON SEGMENT 
    // location itself..
    mSpawnPos = spawnPos; 
}

void Pedestrian::ActivateSelf()
{
    mMillisecondsOutOfSight = 0;
    mMarkedForActivation = false;

    // Test to see how close we are to the player... if too close
    // just deactivate...
    rmt::Vector myPos; 
    mpCharacter->GetPosition( myPos );

    rmt::Vector playerPos, playerVel;

    Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
    rAssert( avatar != NULL );
    avatar->GetPosition( playerPos );

    /*
    rmt::Vector camTarget;
    SuperCam* pCam = GetSuperCamManager()->GetSCC(0)->GetActiveSuperCam();
    pCam->GetHeadingNormalized( &camTarget );
    rAssert( rmt::Epsilon(camTarget.MagnitudeSqr(), 1.0f, 0.0005f) );

    rmt::Vector center = playerPos + camTarget * PedestrianManager::CENTER_OFFSET;

    float distSqr = (myPos - center).MagnitudeSqr();
    */
    float distSqr = (myPos - playerPos).MagnitudeSqr();
    float removeDistSqr = PedestrianManager::GetInstance()->FADE_RADIUS;
    removeDistSqr *= removeDistSqr;

    if( GetGameplayManager()->TestPosInFrustrumOfPlayer( myPos, 0 ) && 
        distSqr < removeDistSqr )
    {
        PedestrianManager::GetInstance()->RemovePed( mpCharacter );
        return;
    }
    
    // make sure we don't activate too near another ped...
    const float TOO_CLOSE_TO_ANOTHER_PED_SQR = 2.25f;
    rmt::Vector pedPos; 
    int i = PedestrianManager::GetInstance()->mActivePeds.GetHead();
    while( 0 <= i && i < DListArray::MAX_ELEMS )
    {
        PedestrianManager::PedestrianStruct* pedStr = (PedestrianManager::PedestrianStruct*) 
            PedestrianManager::GetInstance()->mActivePeds.GetDataAt(i);
        rAssert( pedStr != NULL );

        if( pedStr->ped == this )
        {
            i = PedestrianManager::GetInstance()->mActivePeds.GetNextOf( i );
            continue; // I'm this ped! Ignore...
        }

        if( !pedStr->ped->GetIsActive() )
        {
            i = PedestrianManager::GetInstance()->mActivePeds.GetNextOf( i );
            continue; // ped is waiting to self-activate, ignore it
        }

        pedStr->ped->GetCharacter()->GetPosition( &pedPos );
        pedPos.y = myPos.y; // ignore diff in y

        if( (pedPos-myPos).MagnitudeSqr() < TOO_CLOSE_TO_ANOTHER_PED_SQR )
        {
            PedestrianManager::GetInstance()->RemovePed( mpCharacter );
            return; 
        }
        i = PedestrianManager::GetInstance()->mActivePeds.GetNextOf( i );
    }


    /////////////////////////////////
    // Otherwise, we're free to activate self

    mIsActive = true;

    // set my state
    TransitToState(FOLLOWING_PATH);
    mOffPath = false;
    DetermineFollowPathSpeed();

    mpCharacter->ResetCollisions();
    mpCharacter->AddToWorldScene( );
    mpCharacter->SetHasBeenHit(false);

    mpCharacter->AddToPhysics();
}

void Pedestrian::InitZero()
{
    mPath = NULL;

    mMillisecondsOutOfSight = 0;
    mMarkedForActivation = false;
    mIsActive = false;

    TransitToState(STOPPED);
    mOffPath = false;
}

void Pedestrian::Deactivate()
{
    InitZero();

    // transit back to locomotion state
    if( mpCharacter->GetStateManager()->GetState() != CharacterAi::LOCO )
    {
        mpCharacter->GetStateManager()->SetState<CharacterAi::Loco>();
    }

    // transit back to AI control...
    sim::SimState* simState = mpCharacter->GetSimState();
    if( simState != NULL )
    {
        simState->SetControl( sim::simAICtrl );
    }

    //mpCharacter->SetSolveCollisions( false );
    mpCharacter->RemoveFromPhysics();
    mpCharacter->RemoveFromWorldScene( );
}

void Pedestrian::OnReachedWaypoint()
{
    // do nothing
}


float Pedestrian::GetFollowPathSpeedMps() const
{
    // when offpath, don't just RUN BACK on the path
    // this can make us thrash back and forth (Oh we're off path!
    // Oh now we're off path on the other side! Oh we're off path! So on...)
    if( mOffPath )
    {
        return PEDESTRIAN_PATHFIND_BACK_SPEED_MPS;
    }
    return mFollowPathSpeedMps;
}

void Pedestrian::GetBoundingSphere( rmt::Sphere& s )
{
    rmt::Sphere sphere;
    mpCharacter->GetBoundingSphere( &sphere );

    // make sure the bounding sphere has same position as character!!!
    s = sphere;    
}


void Pedestrian::DetermineFollowPathSpeed()
{
    int coinflip = rand() % 10;
    if( 0 <= coinflip && coinflip < 4 )
    {
        mFollowPathSpeedMps = PEDESTRIAN_WALK_SPEED_MPS;
    }
    else if( 4 <= coinflip && coinflip < 8 )
    {
        mFollowPathSpeedMps = PEDESTRIAN_BRISK_WALK_SPEED_MPS;
    }
    else
    {
        mFollowPathSpeedMps = PEDESTRIAN_RUN_SPEED_MPS;
    }
}

