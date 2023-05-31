//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement ChaseAI
//
// History:     10/07/2002 + Created -- NAME
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

#include <ai/vehicle/chaseai.h>

#include <mission/gameplaymanager.h>

#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>

#include <roads/roadsegment.h>
#include <roads/geometry.h>
#include <roads/roadmanager.h>
#include <roads/road.h>

#include <worldsim/avatar.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/worldphysicsmanager.h>
#include <worldsim/harass/chasemanager.h>

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

ChaseAI::ChaseAI( Vehicle* pVehicle, float beelineDist ) :

    VehicleAI( 
        pVehicle, 
        AI_CHASE, 
        true, 
        VehicleAI::DEFAULT_MIN_SHORTCUT_SKILL, 
        VehicleAI::DEFAULT_MAX_SHORTCUT_SKILL, 
        false ),

    mpTarget( NULL ),
    mTargetHasMovedToAnotherPathElement( true ),
    mTargetRoadSegment( NULL ),
    mTargetRoadSegmentT( 0.0f ),
    mTargetRoadT( 0.0f ),
    mBeelineDist( beelineDist )
{
    mTargetPathElement.elem = NULL;
    rAssert( beelineDist >= 0.0f );
}

ChaseAI::~ChaseAI()
{
}

void ChaseAI::Initialize()
{
    // VehicleAI::Initialize will call our reset, 
    // our reset will call VehicleAI::Reset, so it's all good
    VehicleAI::Initialize();
}

void ChaseAI::Reset()
{
    mpTarget = NULL;
    mTargetHasMovedToAnotherPathElement = true;
    mTargetPathElement.elem = NULL;
    mTargetRoadSegment = NULL;
    mTargetRoadSegmentT = 0.0f;
    mTargetRoadT = 0.0f;

    VehicleAI::Reset();
}

void ChaseAI::Update( float timeins )
{
    if ( GetState() == STATE_LIMBO)
    {
        return;
    }

    UpdateSelf();
    UpdateTarget();
    UpdateSegments();

    if( mpTarget )
    {
        if( GetState() == STATE_WAITING && mTargetPathElement.elem != NULL )
        {
            SetState( STATE_ACCEL );
        }
        rmt::Vector targetPos;
        mpTarget->GetPosition( targetPos );

        rmt::Vector mypos;
        GetVehicle()->GetPosition( &mypos );

        rmt::Vector nextDestination;
        GetNextDestination( nextDestination );

        rmt::Vector vec2next;
        vec2next.Sub( nextDestination, mypos );

        rmt::Vector vec2target;
        vec2target.Sub( targetPos, mypos );

        // Not using this currently
        //
        //float dp = vec2next.DotProduct( vec2target );

        float dist2target = vec2target.Magnitude(); // *** SQUARE ROOT! ***

        // If we're not close enough to player, don't beeline...
        if( dist2target >= mBeelineDist )
        {
            FollowRoad();
        }
        else
        {
            // If we're close enough to beeline, we still need 
            // to see if there are things in the way... 
            // 
            // First we construct a line between us and target
            // If this line crosses over a segment's flanking edges, 
            // then we don't beeline..
            //
            rmt::Vector start, end;
            GetPosition( &start );
            mpTarget->GetPosition( end );

            // we only need to worry if start & end aren't the same points
            if( !start.Equals( end, 0.001f ) )
            {
                bool beeline = true;

            #define CHASEAI_CHECK_STATICS
            #ifdef CHASEAI_CHECK_STATICS
                // grab list of statics
                WorldPhysicsManager::StaticsInCollisionDetection** statics = 
                    ::GetWorldPhysicsManager()->mCurrentStatics;
                WorldPhysicsManager::StaticsInCollisionDetection* myStatics = 
                    statics[ GetVehicle()->mCollisionAreaIndex ];
                int maxStatics = ::GetWorldPhysicsManager()->mMaxStatics;

                // iterate through list of statics
                for( int j=0; j<maxStatics; j++ )
                {
                    //rAssert( myStatics[j].clean );
                    StaticPhysDSG* obj = myStatics[j].mStaticPhysDSG;
                    if( !obj )
                    {
                        continue;
                    }
                    sim::SimState* simState = obj->GetSimState();
                    if( !simState )
                    {
                        continue;
                    }
                    sim::CollisionObject* colObj = simState->GetCollisionObject();
                    if( !colObj )
                    {
                        continue;
                    }
                    sim::CollisionVolume* colVol = colObj->GetCollisionVolume();
                    if( !colVol )
                    {
                        continue;
                    }

                    beeline = !TestIntersectBBox( start, end, colVol );
                    if( !beeline )
                    {
                        //rDebugPrintf( "Blocked by %s\n", colVol->GetCollisionObject()->GetName() );
                    }
                    /*
                    switch( colVol->Type() )
                    {
                    case sim::CollisionVolumeType:
                        rAssert( false );
                        break;
                    case sim::BBoxVolumeType:
                        beeline = TestIntersectBBox( start, end, colVol );
                        break;
                    case sim::SphereVolumeType:
                        beeline = TestIntersectSphere( start, end, colVol );
                        break;
                    case sim::CylinderVolumeType:
                        beeline = TestIntersectCylinder( start, end, colVol );
                        break;
                    case sim::OBBoxVolumeType:
                        beeline = TestIntersectOBBox( start, end, colVol );
                        break;
                    case sim::WallVolumeType:
                        beeline = TestIntersectWall( start, end, colVol );
                        break;
                    }
                    */
                }
            #endif

                // Beeline here... 
                if( !beeline )
                {
                    //rDebugPrintf( "**** FOLLOWING ROAD! ****\n" );
                    FollowRoad();
                }
                else
                {
                    //rDebugPrintf( "**** BEELINING ****\n" );
                    Beeline( dist2target );
                }
            }
            else
            {
                Beeline( dist2target );
            }
        }

        // if we just hit another vehicle (not the other way around), transit to stunned,
        // then reverse and try hitting again...
        if( GetVehicle()->mCollidedWithVehicle && !GetVehicle()->mWasHitByVehicle )
        {
            mSecondsStunned = 0.0f;
            SetState( STATE_STUNNED );
        }
    }
    else // no one to chase!
    {
        //rDebugPrintf( "*** CHASE AI: No one to chase! ***\n" );
        SetState( STATE_WAITING );
        FollowRoad();
    }

    DoSteering();

    // TODO:
    // Evade traffic or static?

    CheckState( timeins );
    
}//end update


void ChaseAI::DoCatchUp( float timeins )
{
    // Nothing for now
    mDesiredSpeedKmh = GetVehicle()->mDesignerParams.mDpTopSpeedKmh;
    mShortcutSkillMod = 0;
}



bool ChaseAI::MustRepopulateSegments()
{
    // get target's position. 
    if( mpTarget == NULL )
    {
        // if no target, don't repopulate segments
        return false;
    }
    return mTargetHasMovedToAnotherPathElement;
}

//
// given a line segment from point "start" to point "end", determine if any point on 
// this line is close enough to the current target waypoint
// 
bool ChaseAI::TestReachedTarget( const rmt::Vector& start, const rmt::Vector& end )
{
    if( mpTarget )
    {
        rmt::Vector targetPos;
        mpTarget->GetPosition( targetPos );

        rmt::Vector closestPos;
        FindClosestPointOnLine( start, end, targetPos, closestPos );

        float distSqr = (closestPos - targetPos).MagnitudeSqr();

        // NOTE:
        // We don't use mBeelineDist here because we want to still 
        // know how to get closer to the target if, for example, 
        // we fail to beeline because we detected some statics are in
        // the way...
        const float REACHED_TARGET_DIST_SQR = 100.0f;
        return( distSqr <= REACHED_TARGET_DIST_SQR ); //(mBeelineDist*mBeelineDist) );
    }
    // if no target, then no waypoint, then we don't need to reach any waypoint... 
    return true;
}

void ChaseAI::GetClosestPathElementToTarget( 
    rmt::Vector& targetPos,
    RoadManager::PathElement& elem,
    RoadSegment*& seg,
    float& segT,
    float& roadT )
{
    elem.elem = NULL;
    seg = NULL;
    segT = 0.0f;
    roadT = 0.0f;

    if( mpTarget )
    {
        mpTarget->GetPosition( targetPos );
        elem = mTargetPathElement;
        seg = (RoadSegment*) mTargetRoadSegment;
        segT = mTargetRoadSegmentT;
        roadT = mTargetRoadT;
    }
}


void ChaseAI::UpdateTarget()
{
    mTargetHasMovedToAnotherPathElement = false;

    mpTarget = GetAvatarManager()->GetAvatarForPlayer( 0 );
    if( mpTarget )
    {
        // the Avatar keeps a notion of its nearest road/intersection entities

        RoadManager::PathElement elem;
        elem.elem = NULL;
        RoadSegment* seg = NULL;
        float segT = 0.0f;
        float roadT = 0.0f;

        mpTarget->GetLastPathInfo( elem, seg, segT, roadT );

        if( elem.elem != NULL )
        {
            // update old values only if the new values are good..
            if( mTargetPathElement != elem )
            {
                mTargetHasMovedToAnotherPathElement = true;
                mTargetPathElement = elem;
            }
            if( seg )
            {
                if( mTargetRoadSegment != seg )
                {
                    mTargetRoadSegment = seg;
                }
                mTargetRoadSegmentT = segT;
                mTargetRoadT = roadT;
            }
        }
        if( mTargetPathElement.elem == NULL ) // no target, so ... ya... 
        {
            SetState(STATE_WAITING);
        }
        else
        {
            if( GetState() == STATE_WAITING )
            {
                SetState(STATE_ACCEL);
            }
        }
    }
}



//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************


void ChaseAI::Beeline( float dist2target )
{
    if( mpTarget != NULL )
    {
        // we want to re-pathfind after we come out of beeline
        // so we do this.. 
        ResetSegments();

        rmt::Vector targetPos;
        mpTarget->GetPosition( targetPos );

        rmt::Vector vel;

        float speed = GetVehicle()->mSpeed;
        float timetoimpact = 1000.0f;
        if( speed > 0.0f )
        {
            timetoimpact = dist2target / speed;
        }
        rAssert( !rmt::IsNan( timetoimpact ) );

        mpTarget->GetVelocity( vel ) ;
        speed = mpTarget->GetSpeedMps();
        float scale = 3.0f;
        if( speed > 0.0f )
        {
            scale = timetoimpact / speed;
        }
        rAssert( !rmt::IsNan( scale ) );

        vel.Scale( scale, scale, scale );

        rmt::Vector nexttargetpos;
        nexttargetpos.Add( targetPos, vel );

        SetDestination( nexttargetpos );
        SetNextDestination( nexttargetpos );
    }
}

bool ChaseAI::TestIntersectBBox( rmt::Vector start, rmt::Vector end, sim::CollisionVolume* colVol )
{
    rAssert( colVol != NULL );
    //rAssert( colVol->Type() == sim::BBoxVolumeType );

    // work in 2D
    start.y = 0.0f;
    end.y = 0.0f;

    sim::CollisionVolume* bbox = colVol;
    //sim::BBoxVolume* bbox = (sim::BBoxVolume*) colVol;
    rmt::Vector pos = bbox->mPosition; // center
    rmt::Vector size = bbox->mBoxSize / 2.0f; // vector to one of corners

    // obtain the vertices and test that they're all on same side of 
    // the line (from us to target). Since volume is axis-aligned, 
    // and since we're working only in 2D, only need the top 4 vertices
    // of the box.
    rmt::Vector tmp;
    rmt::Vector vec0,vec1;

    tmp = size * 2.0f;

    // first point 
    vec0 = pos + size;
    vec0.y = 0.0f;

    // second point
    vec1 = vec0;
    vec1.x -= tmp.x;

    if( !PointsOnSameSideOfLine( vec0, vec1, start, end ) )
    {
        return true;
    }

    // third point
    vec1.z -= tmp.z;
    if( !PointsOnSameSideOfLine( vec0, vec1, start, end ) )
    {
        return true;
    }

    // fourth point
    vec1.x += tmp.x;
    if( !PointsOnSameSideOfLine( vec0, vec1, start, end ) )
    {
        return true;
    }
    return false;
}

/*** HOPE I DON"T HAVE TO IMPLEMENT THESE JUST YET

bool ChaseAI::TestIntersectOBBox( rmt::Vector start, rmt::Vector end, sim::CollisionVolume* colVol )
{
    rAssert( colVol != NULL );
    rAssert( colVol->Type() == sim::OBBoxVolumeType );

    sim::OBBoxVolume* obbox = (sim::OBBoxVolume*)colVol;

    rmt::Vector a, p1, p2;
    for (int i=0; i<4; i++)
    {
    }

    return false;
}
bool ChaseAI::TestIntersectSphere( rmt::Vector start, rmt::Vector end, sim::CollisionVolume* colVol )
{
    rAssert( colVol != NULL );
    rAssert( colVol->Type() == sim::SphereVolumeType );

    sim::SphereVolume* sphere = (sim::SphereVolume*)colVol;

    rmt::Sphere s( sphere->mPosition, sphere->mSphereRadius );

    return TestIntersectLineSphere( start, end, sphere );
}

bool ChaseAI::TestIntersectCylinder( rmt::Vector start, rmt::Vector end, sim::CollisionVolume* colVol )
{
    rAssert( colVol != NULL );
    rAssert( colVol->Type() == sim::CylinderVolumeType );

    sim::CylinderVolume* cyl = (sim::CylinderVolume*)colVol;
    return false;
}

bool ChaseAI::TestIntersectWall( rmt::Vector start, rmt::Vector end, sim::CollisionVolume* colVol )
{
    rAssert( colVol != NULL );
    rAssert( colVol->Type() == sim::WallVolumeType );

    sim::WallVolume* wall = (sim::WallVolume*)colVol;
    return false;
}

****/

int ChaseAI::RegisterHudMapIcon()
{
    int iconID = -1;

    rmt::Vector initialLoc;
    GetVehicle()->GetPosition( &initialLoc );

    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud != NULL )
    {
        ChaseManager* chaseManager = GetGameplayManager()->GetChaseManager( 0 );
        rAssert( chaseManager != NULL );
        if( chaseManager->IsChaseVehicle( this->GetVehicle() ) )
        {
            iconID = currentHud->GetHudMap( 0 )->RegisterIcon( HudMapIcon::ICON_AI_HIT_N_RUN, initialLoc, this );
        }
        else
        {
            iconID = currentHud->GetHudMap( 0 )->RegisterIcon( HudMapIcon::ICON_AI_CHASE, initialLoc, this );
        }
    }

    return iconID;
}

