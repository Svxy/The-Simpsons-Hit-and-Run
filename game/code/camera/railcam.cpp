//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        RailCam.cpp
//
// Description: Implement RailCam
//
// History:     17/07/2002 + Created -- Cary Brisebois (Borrowed and adapted from J-L Duprat)
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
#ifndef WORLD_BUILDER
#include <camera/RailCam.h>
#include <camera/isupercamtarget.h>
#include <camera/supercamcontroller.h>
#include <camera/supercamconstants.h>
#include <debug/debuginfo.h>
#include <memory/classsizetracker.h>
#include <p3d/pointcamera.hpp>
//TODO: I only really need the tuner variables...  Break this file up.
#include <worldsim/character/character.h>
#else
#include "RailCam.h"
#include "isupercamtarget.h"
#include "supercamcontroller.h"
#include "supercamconstants.h"
#include "../../../tools/globalcode/utility/GLExt.h"
#include <maya/mpoint.h>
class tPointCamera
{
public:
    void GetFOV( float* fov, float* aspect ) { *fov = 1.5707f; *aspect = 4.0f / 3.0f; };
};

namespace CharacterTune
{
    static float sfMaxSpeed;
    static float sfDashBurstMax;
};
#endif

#ifdef DEBUGWATCH
float RAIL_CAM_MIN_FOV = SUPERCAM_DEFAULT_MIN_FOV;
#else
const float RAIL_CAM_MIN_FOV = SUPERCAM_DEFAULT_MIN_FOV;
#endif

const float RAIL_CAM_FOV_LAG = SUPERCAM_DEFAULT_FOV_LAG;



//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

const float DEFAULT_MIN_RADIUS = 6.0f;
const float DEFAULT_MAX_RADIUS = 12.0f;

const char* const RailCam::BehaviourNames[] = { "Distance", "Projection" };

#ifdef DEBUGWATCH
float MAX_STEP = 0.25f;
#else
const float MAX_STEP = 0.25f;
#endif

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// RailCam::RailCam
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
RailCam::RailCam() :
    mTarget( NULL ),
    mBehaviour( PROJECTION ),
    mMinRadius( DEFAULT_MIN_RADIUS ),
    mMaxRadius( DEFAULT_MAX_RADIUS ),
    mTrackDist( 0.0f ),
    mStartU( -1.0f ),
    mU( 0.0f ),
    mStep( MAX_STEP ),
    mPositionLag( 0.04f ),
    mTargetLag( 0.04f ),
    mFOVDelta( 0.0f ),
    mMaxFOV( 0.0f ),
    mFOVLag( RAIL_CAM_FOV_LAG ),
    mTrackRail( false ),
    mReverseSensing( false ),
    mDrawRail( false ),
    mDrawHull( false ),
    mDrawCylinder( false ),
    mDrawIntersections( false ),
    mAllowUpdate( true ),
    mReset( false ),
    mResetting( false )
{
    CLASSTRACKER_CREATE( RailCam );
    mQ.SetBasis( rmt::Spline::BSpline );
    mQd.SetBasis( rmt::Spline::DBSpline );

    mTargetOffset.Set( 0.0f, 0.0f, 0.0f );

    mPosition.Set( 0.0f, 0.0f, 0.0f );
    mPositionDelta.Set( 0.0f, 0.0f, 0.0f );

    mTargetPosition.Set( 0.0f, 0.0f, 0.0f );
    mTargetPositionDelta.Set( 0.0f, 0.0f, 0.0f );
}

//==============================================================================
// RailCam::~RailCam
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
RailCam::~RailCam()
{
    CLASSTRACKER_DESTROY( RailCam );
}

//=============================================================================
// RailCam::OnInit
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RailCam::OnInit()
{
    //Override the Near plane...
    SetNearPlane( mMinRadius );
    InitMyController();

    if ( !mReset )
    {
        //This is to make the cameras move to a better place before interpolation.
        DenyUpdate();
        DoFirstTime();
        DoCameraCut();
        Update( 16 );
        AllowUpdate();
    }
    else
    {
        mResetting = true;
    }
}

//=============================================================================
// RailCam::OnShutdown
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RailCam::OnShutdown()
{
    //Reset me to the origin of the spline.
    mU = 0.0f;
}

//=============================================================================
// RailCam::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void RailCam::Update( unsigned int milliseconds )
{
    float timeMod = milliseconds / 16.0f;

    rmt::Vector targetPos;
    GetTargetPosition( &targetPos, true );

//    rmt::Vector camPos;
//    GetPosition( &camPos );

    rmt::Vector desiredPos;

    if ( mResetting )
    {
        mU = 0.0f;
        mResetting = false;
    }

    // compute camera position
    switch(mBehaviour)
    {
    case DISTANCE:
    default:
        desiredPos = FindCameraPosition_Distance(targetPos, mMaxRadius);
        break;
    case PROJECTION:
        desiredPos = FindCameraPosition_Projection(targetPos, mMaxRadius);
        break;
    }

    rmt::Vector desiredTarget;
    desiredTarget = FindCameraLookAt( targetPos, desiredPos );

    bool cut, firstTime;
    cut = GetFlag( (Flag)CUT );
    firstTime = GetFlag( (Flag)FIRST_TIME );

    if ( cut || firstTime )
    {
        //Reset the smoothing stuff.
        mPosition = desiredPos;
        mPositionDelta.Set( 0.0f, 0.0f, 0.0f );

        mTargetPosition = desiredTarget;
        mTargetPositionDelta.Set( 0.0f, 0.0f, 0.0f );

        //SetFOV( mMaxFOV );
        mFOVDelta = 0.0f;

        if ( cut )
        {
            if ( GetFlag( (Flag)START_TRANSITION ) )
            {
                SetFlag( (Flag)START_TRANSITION, false );
            }
            else if ( GetFlag( (Flag)TRANSITION ) )
            {
                SetFlag( (Flag)END_TRANSITION, true );
            }

            SetFlag( (Flag)CUT, false );
        }
        else
        {
            //Not cutting, so let's try this.
            float fov, aspect = 0.0f;
            GetCamera()->GetFOV( &fov, &aspect );
            if ( mAllowUpdate )
            {
                SetFOV( fov );
            }
        }

        SetFlag( (Flag)FIRST_TIME, false );
    }
    else
    {
        //Let's smooth out the motion...
        float posLag = mPositionLag * timeMod;
        CLAMP_TO_ONE(posLag);
        MotionCubic( &mPosition.x, &mPositionDelta.x, desiredPos.x, posLag );
        MotionCubic( &mPosition.y, &mPositionDelta.y, desiredPos.y, posLag );
        MotionCubic( &mPosition.z, &mPositionDelta.z, desiredPos.z, posLag );

        float targLag = mTargetLag * timeMod;
        CLAMP_TO_ONE(targLag);
        MotionCubic( &mTargetPosition.x, &mTargetPositionDelta.x, desiredTarget.x, targLag );
        MotionCubic( &mTargetPosition.y, &mTargetPositionDelta.y, desiredTarget.y, targLag );
        MotionCubic( &mTargetPosition.z, &mTargetPositionDelta.z, desiredTarget.z, targLag );
    }

    //---------  Goofin' with the FOV

    if ( GetFlag( (Flag)TRANSITION ) || mTarget->IsCar() )
    {
        if ( mAllowUpdate )
        {
            SetFOV( mMaxFOV );
        }
        mFOVDelta = 0.0f;  //reset
    }
    else
    {
#ifndef WORLD_BUILDER
        float zoom = mController->GetValue( SuperCamController::zToggle );
#else
        float zoom = 0.0f;
#endif
        float FOV = GetFOV();
        FilterFov( zoom, RAIL_CAM_MIN_FOV, mMaxFOV, FOV, mFOVDelta, mFOVLag, timeMod );

        if ( mAllowUpdate )
        {
            SetFOV( FOV );
        }
    }

    //---------  Set values.

    if ( mAllowUpdate )
    {
        SetCameraValues( milliseconds, mPosition, mTargetPosition );
    }

    DrawRail( true );
    DrawHull( true );
    DrawCylinder( targetPos );
}

//=============================================================================
// RailCam::InitController
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RailCam::InitController()
{
}

//=============================================================================
// RailCam::LoadSettings
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned char* settings )
//
// Return:      void 
//
//=============================================================================
void RailCam::LoadSettings( unsigned char* settings )
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// RailCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RailCam::OnRegisterDebugControls()
{
#ifdef DEBUGWATCH
    char nameSpace[256];
    sprintf( nameSpace, "SuperCam\\Player%d\\Rail", GetPlayerID() );

    radDbgWatchAddUnsignedInt( ((unsigned int*)(&mBehaviour)), "Behaviour", nameSpace, NULL, NULL, RailCam::DISTANCE, RailCam::PROJECTION );
    radDbgWatchAddFloat( &mMinRadius, "Min. Radius", nameSpace, NULL, NULL, 0.0f, 12.0f );
    radDbgWatchAddFloat( &mMaxRadius, "Max. Radius", nameSpace, NULL, NULL, 0.0f, 12.0f );
    radDbgWatchAddBoolean( &mTrackRail, "Track Rail", nameSpace, NULL, NULL );
    radDbgWatchAddFloat( &mTrackDist, "Track Distance", nameSpace, NULL, NULL, -10.0f, 10.0f );
    radDbgWatchAddBoolean( &mReverseSensing, "Reverse Sense", nameSpace, NULL, NULL );
    radDbgWatchAddVector( &mTargetOffset.x, "Target Offset", nameSpace, NULL, NULL, -5.0f, 5.0f );
    radDbgWatchAddVector( &mAxisPlay.x, "Axis Play", nameSpace, NULL, NULL, 0.0f, rmt::PI_BY2 );
    radDbgWatchAddBoolean( &mDrawRail, "Draw Rails", nameSpace, NULL, NULL );
    radDbgWatchAddBoolean( &mDrawHull, "Draw Hulls", nameSpace, NULL, NULL );
    radDbgWatchAddBoolean( &mDrawCylinder, "Draw Cylinder", nameSpace, NULL, NULL );
    radDbgWatchAddBoolean( &mDrawIntersections, "Draw Intersections", nameSpace, NULL, NULL );
    radDbgWatchAddFloat( &MAX_STEP, "Max Step", nameSpace, NULL, NULL, 0.0f, 1.0f );
    radDbgWatchAddFloat( &mPositionLag, "Position Lag Factor", nameSpace, NULL, NULL, 0.0f, 1.0f );
    radDbgWatchAddFloat( &mTargetLag, "Target Lag Factor", nameSpace, NULL, NULL, 0.0f, 1.0f );

    radDbgWatchAddFloat( &mFOVLag, "FOV Lag", nameSpace, NULL, NULL, 0.0f, 1.0f );
    radDbgWatchAddFloat( &RAIL_CAM_MIN_FOV, "FOV Min", nameSpace, NULL, NULL, 0.0f, rmt::PI_BY2 );

#endif
}

//=============================================================================
// RailCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RailCam::OnUnregisterDebugControls()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &mBehaviour );
    radDbgWatchDelete( &mMinRadius );
    radDbgWatchDelete( &mMaxRadius );
    radDbgWatchDelete( &mTrackRail );
    radDbgWatchDelete( &mTrackDist );
    radDbgWatchDelete( &mReverseSensing );
    radDbgWatchDelete( &mTargetOffset.x );
    radDbgWatchDelete( &mAxisPlay.x );
    radDbgWatchDelete( &mDrawRail );
    radDbgWatchDelete( &mDrawHull );
    radDbgWatchDelete( &mDrawCylinder );
    radDbgWatchDelete( &mDrawIntersections );
    radDbgWatchDelete( &MAX_STEP );
    radDbgWatchDelete( &mPositionLag );
    radDbgWatchDelete( &mTargetLag );
    radDbgWatchDelete( &RAIL_CAM_MIN_FOV );
    radDbgWatchDelete( &mFOVLag );
#endif
}

//=============================================================================
// RailCam::GetTargetSpeedModifier
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
float RailCam::GetTargetSpeedModifier()
{
    rmt::Vector vel;
    mTarget->GetVelocity( &vel );
    float speed = vel.Magnitude();  //Square root!

    //TODO:I wish this was a const.
    if ( speed < CharacterTune::sfMaxSpeed || rmt::Epsilon( speed, CharacterTune::sfMaxSpeed, 0.01f ) )
    {
        return 1.0f;
    }

    float maxMod = CharacterTune::sfMaxSpeed + CharacterTune::sfDashBurstMax / CharacterTune::sfMaxSpeed;
    float modifier = (speed - CharacterTune::sfMaxSpeed) / CharacterTune::sfDashBurstMax * maxMod;  
    rAssert( modifier > 0.01f );

    return modifier;
}

//=============================================================================
// RailCam::GetTargetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* position, bool withOffset )
//
// Return:      void 
//
//=============================================================================
void RailCam::GetTargetPosition( rmt::Vector* position, 
                                 bool withOffset )
{
    rAssert( mTarget );

    mTarget->GetPosition( position );
}

//=============================================================================
// RailCam::IntervalClamp
//=============================================================================
// Description: Comment
//
// Parameters:  ( float &t )
//
// Return:      SolutionType 
//
//=============================================================================
RailCam::SolutionType RailCam::IntervalClamp( float &t ) const
{
    if(t>1.0f)
    {
        t=1.0f;
        return WORSTCASE;
    }
    if(t<0.0f)
    {
        t=0.0f;
        return WORSTCASE;
    }
    return APPROX;
}

//=============================================================================
// RailCam::ProjectPointOnLine
//=============================================================================
// Description: Comment
//
// Parameters:  (const rmt::Vector& A, const rmt::Vector& B, const rmt::Vector& O, float& t)
//
// Return:      float
//
//=============================================================================
float& RailCam::ProjectPointOnLine(const rmt::Vector& A, const rmt::Vector& B, const rmt::Vector& O, float& t) const
{
    // Projects the point O on the line that goes through A and B
    // Caller needs to clamp the parameter t such that it always lies between A and B

    if( A == B )
        t = 0.0f; // not really a segment...
    else
        t = -((B.x-A.x)*(A.x-O.x) + (B.y-A.y)*(A.y-O.y) + (B.z-A.z)*(A.z-O.z))/((B.x-A.x)*(B.x-A.x) + (B.y-A.y)*(B.y-A.y) + (B.z-A.z)*(B.z-A.z));
    return t;
}

//=============================================================================
// RailCam::IntersectLineCylinder
//=============================================================================
// Description: Comment
//
// Parameters:  (const int segment, const rmt::Vector& origin, const float radius, const rmt::Vector& neighbour, float& t)
//
// Return:      RailCam
//
//=============================================================================
RailCam::SolutionType RailCam::IntersectLineCylinder(const int segment, const rmt::Vector& origin, const float radius, const rmt::Vector& neighbour, float& t)
{
    // Intersects the line segment defined by the knots at segment and segment+1 with a 
    // infinitely long cylinder centered on origin and of specified radius.  If there are 
    // multiple solutions (potentially an infinity), pick the one closest to neighbour.
    // Returns true if there is a solution and t is set to the parametric position of intersection 
    // along the line segment...  If there is no solution, return false and t is set to the closest
    // point to the cylinder along the line segment

    // To simplify the math, we consider that the spline segments are straight lines between the knots
    // we return the t at which the intersection occured on the line, but camera will be moved to that 
    // parametric position along the curve.

    // vertical axis of cylinder is Y
    
    rmt::Vector A = mQ.Evaluate(float(segment));
    rmt::Vector B = mQ.Evaluate(float(segment+1));

    if(rmt::Epsilon(B.x, A.x) && rmt::Epsilon(B.z, A.z))
    {
        // curve segment is parallel to cylinder (zero or infinity of intersections)
        // project origin on segment instead
        ProjectPointOnLine(A, B, origin, t);

        rmt::Vector L(A.x, origin.y, A.z);
        if(rmt::Epsilon(L.Magnitude(), radius))
            return EXACT;   // edge is along the cylinder
        else
        {
            return IntervalClamp(t);  // edge is inside the cylinder
        }
    }


    // There are 0, 1 or 2 intersections along the segment

    // Replace the parametrized line equation into the equation for the cylinder
    // and solve for the parameter t.  The algebra is ugly, but here goes: We are 
    // solving a quadratic and use the expected names for variables...

    float a = (A.x-B.x)*(A.x-B.x)+(A.z-B.z)*(A.z-B.z);
    float b = 2.0f* (- A.x*A.x - A.z*A.z + A.x*B.x + A.z*B.z + A.x*origin.x - B.x*origin.x + A.z*origin.z - B.z*origin.z);
    float c = (A.x-origin.x)*(A.x-origin.x)+(A.z-origin.z)*(A.z-origin.z)-radius*radius;
    
    float b2m4ac = b*b-4.0f*a*c;
    if(b2m4ac>0.0f)
    {
        float t1 = (-b+rmt::Sqrt(b2m4ac))/(2.0f*a);
        float t2 = (-b-rmt::Sqrt(b2m4ac))/(2.0f*a);

        if((t1<0.0f || t1>1.0f) && (t2<0.0f || t2>1.0f))
        {
            // both intersections are outside of the segment, project origin on segment
            return IntervalClamp(ProjectPointOnLine(A, B, origin, t));
        }
        if(t1<0.0f || t1>1.0f)
        {
            // only t2 is on the segment
            t = t2;
            return EXACT;
        }
        if(t2<0.0f || t2>1.0f)
        {
            // only t1 is on the segment
            t = t1;
            return EXACT;
        }

        // we have two intersections on the segment, pick the one closest to neighbour
        rmt::Vector X1;
        X1.Interpolate(A, B, t1);
        rmt::Vector L;
        L.Sub(neighbour, X1);
        float l1 = L.MagnitudeSqr();
        rmt::Vector X2;
        X2.Interpolate(A, B, t2);
        L.Sub(neighbour, X2);
        float l2 = L.MagnitudeSqr();

        if(l1<l2)
            t = t1;  // X1 is closer
        else
            t = t2;  // X2 is closer
        return EXACT;
    }
    else 
    {
        // no intersection, project origin on segment
        return IntervalClamp(ProjectPointOnLine(A, B, origin, t));
    }
}

//=============================================================================
// RailCam::FindCameraPosition_Distance
//=============================================================================
// Description: Comment
//
// Parameters:  (const rmt::Vector& target, const float radius)
//
// Return:      rmt
//
//=============================================================================
rmt::Vector RailCam::FindCameraPosition_Distance(const rmt::Vector& target, const float radius)
{
    // finds the position of camera along rail where the camera is at distance radius
    // from origin (target) and closest to prevRailPos.  Each segment provides a candidate and the closest
    // one to previous camera position is used...
	if(mQ.GetNumSegments()==0)
        return rmt::Vector(0.0f, 0.0f, 0.0f);

    unsigned int i;
    for(i=0; i<3; i++)
        mCandidates[i].Reset();

    if(mReverseSensing)
    {
       mU=mQ.GetEndParam()-mU; // we need to know where the camera really is for the evaluation
    }

    rmt::Vector prevRailPos;
//    if( GetFlag( (Flag)CUT ) || GetFlag( (Flag)FIRST_TIME ) )
//    {
//        // when initializing camera minimize distance to actor
//        prevRailPos = target;
//    } 
//    else
//    {
//        // when not initializing camera we want to minize movement between frames
        prevRailPos = mQ.Evaluate(mU);  // previous camera position on the rail
//    }    
    
    for(i=0; i<mQ.GetNumSegments(); i++)
    {
        float segT = 0.0f;

        // each segment will return the best it can do (best solution type would be APPROX in this case)
        SolutionType index = IntersectLineCylinder(i, target, radius, prevRailPos, segT);
        rmt::Vector p=mQ.Evaluate(i+segT);
        rmt::Vector delta(p.x-prevRailPos.x, p.y-prevRailPos.y, p.z-prevRailPos.z);
        float len = delta.MagnitudeSqr();

#ifdef DEBUGINFO_ENABLED
        if( mDrawIntersections )
        {
            // mark the actual intersection of the cylinder and the spline curve
            tColour col;
            if(EXACT==index)
                col.Set(255,255,255);  // this should not happen in corridors
            else if(APPROX==index)
                col.Set(255,255,0);
            else
                col.Set(255,0,0);

            if ( DEBUGINFO_PUSH_SECTION( "Rail Cam" ) )
            {
                DEBUGINFO_ADDSTAR( p, col, 0.5f );
            }

            DEBUGINFO_POP_SECTION();
        }
#endif

        if(len<mCandidates[index].dist) 
        {
            mCandidates[index].dist = len;
            mCandidates[index].segment = i;
            mCandidates[index].u = segT;
            mCandidates[index].pu = p;
            mCandidates[index].pDist = rmt::Abs(segT+i-mU);
            if(mQ.GetClosed() && mCandidates[index].pDist>mQ.GetEndParam()/2.0f) 
                mCandidates[index].pDist = rmt::Abs(mCandidates[index].pDist-mQ.GetEndParam()); // its shorter going the other way!
        }
    }

    rmt::Vector railPosition;

    // Decide whether to use the exact or approx solution to minimize camera travel
    if(mCandidates[EXACT].segment != -1 && mCandidates[EXACT].pDist>mStep)
    {
        // closest intersection point is quite far from current parametric position and would cause camera to jump.

        // if approximate solution is closer use that instead...
        if(mCandidates[APPROX].pDist<=mStep)
        {
            railPosition = FinalizeRailPosition(APPROX);
        }
        else 
        {
            // Pick the closest (parameter space) point and move towards it.
            SolutionType index = (mCandidates[EXACT].pDist<=mCandidates[APPROX].pDist)?EXACT:APPROX;
            railPosition = FinalizeRailPosition(index);
            // we never considered WorstCase solutions
        }
    }
    else if(mCandidates[EXACT].segment != -1)
    {
        // use the proper intersection, its close enough
        railPosition = FinalizeRailPosition(EXACT);
    } 
    else if(mCandidates[APPROX].segment != -1)
    {
        // use the closest approximation
        railPosition = FinalizeRailPosition(APPROX);
    }
    else 
        railPosition = FinalizeRailPosition(WORSTCASE);

    return railPosition;
}

//=============================================================================
// RailCam::FindCameraPosition_Projection
//=============================================================================
// Description: Comment
//
// Parameters:  (const rmt::Vector& target, const float pOffset)
//
// Return:      rmt
//
//=============================================================================
rmt::Vector RailCam::FindCameraPosition_Projection(const rmt::Vector& target, const float pOffset)
{
    // finds the position of camera along rail where the camera is at fixed parametric distance from
    // from projection of target on the rail.  Each segment provides a candidate projection and the closest
    // one to previous camera position is used...

	if(mQ.GetNumSegments()==0)
        return rmt::Vector(0.0f, 0.0f, 0.0f);

    unsigned int i;
    for(i=0; i<3; i++)
        mCandidates[i].Reset();

    if( mReverseSensing )
       mU=mQ.GetEndParam()-mU; // we need to know where the camera really is for the evaluation

    rmt::Vector prevRailPos;
    if( GetFlag( (Flag)CUT ) || GetFlag( (Flag)FIRST_TIME ) )
    {
        // when initializing camera minimize distance to actor
        prevRailPos = target;
    } 
    else
    {
        // when not initializing camera we want to minize movement between frames
        prevRailPos = mQ.Evaluate(mU);  // previous camera position on the rail
    }

    for(i=0; i<mQ.GetNumSegments(); i++)
    {
        float segT = 0.0f;

        // each segment will return the best it can do

        SolutionType index = IntervalClamp(ProjectPointOnLine(mQ.GetKnot(i), mQ.GetKnot(i+1), target, segT));
        float curveT = i + segT + pOffset;
        if(curveT < 0.0f)
        {
            if(mQ.GetClosed())
                while(curveT<0.0f) curveT += mQ.GetEndParam();
            else
                curveT = 0.0f;
        }
        if(curveT > mQ.GetEndParam())
        {
            if(mQ.GetClosed())
                while(curveT>mQ.GetEndParam()) curveT -= mQ.GetEndParam();
            else
                curveT = 0.0f;
        }

        rmt::Vector p=mQ.Evaluate(curveT);
        //rmt::Vector delta(p.x-prevRailPos.x, p.y-prevRailPos.y, p.z-prevRailPos.z);
        rmt::Vector delta( p.x-target.x, p.y-target.y, p.z-target.z);
        float len = delta.MagnitudeSqr();

#ifdef DEBUGINFO_ENABLED
        if( mDrawIntersections )
        {
            // mark the actual intersection of the cylinder and the spline curve
            tColour col;
            if(EXACT==index)
                col.Set(255,255,255);  // this should not happen in corridors
            else if(APPROX==index)
                col.Set(255,255,0);
            else
                col.Set(255,0,0);

            if ( DEBUGINFO_PUSH_SECTION( "Rail Cam" ) )
            {
                DEBUGINFO_ADDSTAR( mQ.Evaluate(i+segT), col, 0.5f );
            }

            DEBUGINFO_POP_SECTION();
        }
#endif

        if(len<mCandidates[index].dist) 
        {
            mCandidates[index].dist = len;
            mCandidates[index].segment = int(rmt::Floor(curveT));
            mCandidates[index].u = curveT-rmt::Floor(curveT);
            mCandidates[index].pu = p;
            mCandidates[index].pDist = rmt::Abs(curveT-mU);
            if(mQ.GetClosed() && mCandidates[index].pDist>mQ.GetEndParam()/2.0f) 
                mCandidates[index].pDist = rmt::Abs(mCandidates[index].pDist-mQ.GetEndParam()); // its shorter going the other way!
        }
    }

    // TODO: this could be bad because if the second intersection is a better choice than
    // the best approximation it will be ignored...

    rmt::Vector railPosition;

    // Decide whether to use the exact or approx solution to minimize camera travel
    if(mCandidates[EXACT].segment != -1 && mCandidates[EXACT].pDist>mStep)
    {
        // closest intersection point is quite far from current parametric position and would cause camera to jump.

        // if approximate solution is closer use that instead...
        if(mCandidates[APPROX].pDist<=mStep)
        {
            railPosition = FinalizeRailPosition(APPROX);
        }
        else 
        {
            // Pick the closest (parameter space) point and move towards it.
            SolutionType index = (mCandidates[EXACT].pDist<=mCandidates[APPROX].pDist)?EXACT:APPROX;
            railPosition = FinalizeRailPosition(index);
            // we never considered WorstCase solutions
        }
    }
    else if(mCandidates[EXACT].segment != -1)
    {
        // use the proper intersection, its close enough
        railPosition = FinalizeRailPosition(EXACT);
    } 
    else if(mCandidates[APPROX].segment != -1)
    {
        // use the closest approximation
        railPosition = FinalizeRailPosition(APPROX);
    }
    else 
        railPosition = FinalizeRailPosition(WORSTCASE);

    return railPosition;

}

//=============================================================================
// RailCam::FinalizeRailPosition
//=============================================================================
// Description: Comment
//
// Parameters:  (SolutionType index)
//
// Return:      rmt
//
//=============================================================================
rmt::Vector RailCam::FinalizeRailPosition(SolutionType index)
{
    rAssert(mCandidates[index].segment!=-1);

    if ( GetFlag( (Flag)CUT ) )
    {
        // when we are cutting to a camera we can jump straight to the parametric position selected
        mU = mCandidates[index].u+mCandidates[index].segment;

        if(mReverseSensing)
        {
            mU=mQ.GetEndParam()-mU; // reverse camera if needed
            return mQ.Evaluate(mU);
        }
        else
        {
            return mCandidates[index].pu;
        }
    }

    //if(mCandidates[index].pDist>0.5f)
    //{
    //    if(mReverseSensing)
    //        mU=mQ.GetEndParam()-mU; // reverse camera if needed
    //    return mQ.Evaluate(mU); // target is too far along rail, just stay put.
    //}

    // TODO: take care of acceleration for start/stop on the rail

    if(mCandidates[index].pDist<=mStep)
    {
        // target is within step size, snap to it
        mU = mCandidates[index].u+mCandidates[index].segment;
        if(mReverseSensing)
        {
            mU=mQ.GetEndParam()-mU; // reverse camera if needed
            return mQ.Evaluate(mU);
        }
        else
        {
            return mCandidates[index].pu;
        }

        return mCandidates[index].pu;
    }

    // target is too far, walk towards it
    if(mCandidates[index].u+mCandidates[index].segment>mU)
        mU+=mStep;
    else
        mU-=mStep;

    if(mReverseSensing)
        mU=mQ.GetEndParam()-mU; // reverse camera if needed
    
    return mQ.Evaluate(mU);
}

//=============================================================================
// RailCam::FindCameraLookAt
//=============================================================================
// Description: Comment
//
// Parameters:  (const rmt::Vector& target, const rmt::Vector& desiredPos)
//
// Return:      rmt
//
//=============================================================================
rmt::Vector RailCam::FindCameraLookAt(const rmt::Vector& target, const rmt::Vector& desiredPos)
{
    rmt::Vector lookAt(target);
    
    if(mTrackRail && rmt::Abs(mTrackDist)>0.001f)
    {
        float trackU = mU + mTrackDist;

        lookAt = TestEval( trackU );
    }
    

/*
#ifdef DEBUGINFO_ENABLED
    if(mTrackRail && mDrawRail)
    {
        if ( DEBUGINFO_PUSH_SECTION( "Rail Cam" ) )
        {
            DEBUGINFO_ADDLINE(mQ.Evaluate(trackU), lookAt, tColour(0,0,255));
            DEBUGINFO_ADDSTAR(lookAt, tColour(0,0,255), 0.5f);
       }

        DEBUGINFO_POP_SECTION();
    }
#endif
*/

    //Take the offset and apply it to the look at.
    //If the trackDist is 0, then do a little track dist anyway and use it to calculate the heading.
    if ( mTargetOffset.x != 0 ||
         mTargetOffset.y != 0 ||
         mTargetOffset.z != 0 )
    {
        rmt::Vector offset;

        //Make a transformation that puts the offset into the rotation space of
        //the camera target.

        offset = lookAt - desiredPos;
        rmt::Vector vup = UpdateVUP( desiredPos, lookAt );

        rmt::Matrix mat;
        mat.Identity();
        mat.FillHeading( offset, vup );

        //reuse offset.
        offset = mTargetOffset;

        offset.Transform( mat );

        lookAt.Add( offset );

        CorrectDist( desiredPos, lookAt );
    }

    return lookAt;
}
 
//=============================================================================
// RailCam::TestEval
//=============================================================================
// Description: Comment
//
// Parameters:  ( float u )
//
// Return:      rmt
//
//=============================================================================
rmt::Vector RailCam::TestEval( float u )
{
    rmt::Vector lookAt;
    
    if(u > mQ.GetEndParam())
    {
        // past end of curve
        if(mQ.GetClosed())
        {
            u -= mQ.GetEndParam(); // cycle on closed curve
            lookAt = mQ.Evaluate(u);
        }
        else
        {
            // Interpolate along curve tangent at endpoint
            float mult = rmt::Floor(u/mQ.GetEndParam());
            u -= mult*mQ.GetEndParam(); // distance beyond end of curve

            // if curve has triple end-knots, derivative there is zero so move in...
            float evalAt=mQ.GetEndParam();
            int index = mQ.GetNumVertices();
            if(mQ.GetCntrlVertex(index-1) == mQ.GetCntrlVertex(index-2) &&
               mQ.GetCntrlVertex(index-2) == mQ.GetCntrlVertex(index-3))
                evalAt -= 1.0f;

            rmt::Vector p = mQ.Evaluate(mQ.GetEndParam()); // point at the end of the spline
            rmt::Vector t = mQd.Evaluate(evalAt); // tangent at the end of the spline
            t.Scale(u);
            lookAt.Add(p, t);
        }
    }
    else if(u < 0.0f)
    {
        // before begining of curve
        if(mQ.GetClosed())
        {
            u += mQ.GetEndParam(); // cycle on closed curve
            lookAt = mQ.Evaluate(u);
        }
        else
        {
            // Interpolate along curve tangent at endpoint
            float mult = rmt::Floor(-u/mQ.GetEndParam());
            u += mult*mQ.GetEndParam(); // distance beyond end of curve
            
            // if curve has triple end-knots, derivative there is zero so move in...
            float evalAt=0.0f;
            if(mQ.GetCntrlVertex(0) == mQ.GetCntrlVertex(1) &&
               mQ.GetCntrlVertex(1) == mQ.GetCntrlVertex(2))
                evalAt = 1.0f;

            rmt::Vector p = mQ.Evaluate(0.0f); // point at the end of the spline
            rmt::Vector t = mQd.Evaluate(evalAt); // tangent at the end of the spline
            t.Scale(u);
            lookAt.Add(p, t);
        }
    }
    else
    {
        // along the curve this is easy
        lookAt = mQ.Evaluate(u);
    }

    return lookAt;
}

//=============================================================================
// RailCam::DrawRail
//=============================================================================
// Description: Comment
//
// Parameters:  (bool active)
//
// Return:      void 
//
//=============================================================================
void RailCam::DrawRail(bool active)
{
#ifdef DEBUGINFO_ENABLED
   if(!mDrawRail)
        return; // not set to draw

    if ( DEBUGINFO_PUSH_SECTION( "Rail Cam" ) )
    {
        tColour cRail;
        if(active)
            cRail.Set(255,255,255);
        else
            cRail.Set(255,255,0);

        // spline path
        const unsigned int numSteps = 10;
        rmt::Vector p0, p1;

        p0=mQ.InitForwardDifferencing(numSteps);
        unsigned int j;
        for(j=0; j<numSteps*mQ.GetNumSegments(); j++)
        {
            p1=mQ.Forward();
            DEBUGINFO_ADDLINE(p0, p1, cRail);
            if(j%numSteps==0)
                DEBUGINFO_ADDSTAR(p0, cRail, 0.5f);
            p0=p1;
        }
        DEBUGINFO_ADDSTAR(p1, cRail, 0.5f);
    }

    DEBUGINFO_POP_SECTION();

#endif
}

//=============================================================================
// RailCam::DrawHull
//=============================================================================
// Description: Comment
//
// Parameters:  (bool active)
//
// Return:      void 
//
//=============================================================================
void RailCam::DrawHull(bool active)
{
#ifdef DEBUGINFO_ENABLED
   if(!mDrawHull)
        return; // not set to draw

    if ( DEBUGINFO_PUSH_SECTION( "Rail Cam" ) )
    {
        tColour cHull;
        if(active)
            cHull.Set(255,255,0);
        else
            cHull.Set(255,0,0);

        int numCVs = mQ.GetNumVertices();
        int i;
        for(i=1; i<numCVs; i++)
        {
            DEBUGINFO_ADDSTAR(mQ.GetCntrlVertex(i-1), cHull, 0.5f);
            DEBUGINFO_ADDLINE(mQ.GetCntrlVertex(i-1), mQ.GetCntrlVertex(i), cHull);
        }
        DEBUGINFO_ADDSTAR(mQ.GetCntrlVertex(numCVs-1), cHull, 0.5f);
    
        if(mQ.GetClosed())
            DEBUGINFO_ADDLINE(mQ.GetCntrlVertex(0), mQ.GetCntrlVertex(numCVs-1), cHull);  // close the hull    
    }
        
    DEBUGINFO_POP_SECTION();
#endif
}

//=============================================================================
// RailCam::DrawCylinder
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& origin)
//
// Return:      void 
//
//=============================================================================
void RailCam::DrawCylinder( const rmt::Vector& origin)
{
#ifdef DEBUGINFO_ENABLED
   if(!mDrawCylinder)
        return; // not set to draw

    if ( DEBUGINFO_PUSH_SECTION( "Rail Cam" ) )
    {
        tColour cCyln(255,255,0);

        rmt::Vector bottomC(origin);
        bottomC.y -= 1.2f;
        rmt::Vector topC(origin);
        topC.y += 2.0f;

        // bottom cap
        //g_pDebug->AddCircle(bottomC, m_minRadius, cCyln);
        DEBUGINFO_ADDCIRCLE(bottomC, mMaxRadius, cCyln);
        DEBUGINFO_ADDCIRCLE(bottomC, mMinRadius, cCyln);

        // top cap
        //g_pDebug->AddCircle(topC, m_minRadius, cCyln);
        DEBUGINFO_ADDCIRCLE(topC, mMaxRadius, cCyln);
        DEBUGINFO_ADDCIRCLE(topC, mMinRadius, cCyln);

        // sides
        int max = 8;
        int i;
        for(i=0; i<max; i++)
        {
            rmt::Vector A(bottomC.x+mMaxRadius*rmt::Cos(rmt::DegToRadian(360.0f/max*i)), bottomC.y, bottomC.z+mMaxRadius*rmt::Sin(rmt::DegToRadian(360.0f/max*i)));
            rmt::Vector B(topC.x+mMaxRadius*rmt::Cos(rmt::DegToRadian(360.0f/max*i)), topC.y, topC.z+mMaxRadius*rmt::Sin(rmt::DegToRadian(360.0f/max*i)));
            DEBUGINFO_ADDLINE(A, B, cCyln);
        }
    }

    DEBUGINFO_POP_SECTION();
#endif

#ifdef WORLD_BUILDER
    GLExt::drawSphere( mMaxRadius * 100.0f, MPoint( 0, 0, 0 ) );
#endif
}

//=============================================================================
// RailCam::GetWatcherName
//=============================================================================
// Description: the name of the class for the watcher or other debug purposes
//
// Parameters:  NONE
//
// Return:      const char* - the name of the class 
//
//=============================================================================
#ifdef DEBUGWATCH
const char* RailCam::GetWatcherName() const
{
    return "RailCam";
}
#endif
