//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ActorAnimationWasp
//
// Description: Actor animation driver for wasps. Encodes the figure eight
//              pattern that they fly procedurally
//
// Authors:     Michael Riegger
//
//===========================================================================


//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <ai/actor/actoranimationwasp.h>
#include <radmath/trig.hpp>
#include <radmath/vector.hpp>
#include <radmath/matrix.hpp>
#include <radmath/quaternion.hpp>
#include <p3d/anim/compositedrawable.hpp>
#include <radtime.hpp>
#include <raddebug.hpp>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Member Functions
//===========================================================================

ActorAnimationWasp::ActorAnimationWasp():
m_CurrentState( 0 ),
m_CurrentYOffset( 0 ),
m_CurrentXOffset( 0,0,0 ),
m_YBias( 0.5f ),
m_XBias( 0.5f )
{

}

ActorAnimationWasp::~ActorAnimationWasp()
{

}
        
void ActorAnimationWasp::SetState( int state )
{
    
}

bool ActorAnimationWasp::Update( const rmt::Matrix& currTransform, rmt::Matrix* newTransform, float deltaTime, tCompositeDrawable* compDraw )
{
 
    // Fix the magnitude of this thing so that the input angle to sin and cos is in a decent range (0 - 2PI)
    unsigned int iCurrentTime = radTimeGetMilliseconds();
 
    static float C = 317;
    /*
    {
        static bool initedWatcher = false;
        if ( inited == false )
        {
            radDbgWatchAddFloat( &C, "wasp updates ", "wasp", NULL, NULL, 100.0f, 600.0f );         
            inited = true;
        }
    }*/

    unsigned int iClippedTime = iCurrentTime % (int)( C * rmt::PI_2 );
    float fClampedTime = (float)iClippedTime / C;
    rAssert( fClampedTime >= 0 && fClampedTime <= rmt::PI_2 );
    
    
    float fClippedTime = static_cast< float > ( iClippedTime );
    float fCurrentTime = static_cast< float > ( iCurrentTime );

    *newTransform = currTransform;
    float oldY = m_CurrentYOffset;

    m_CurrentYOffset = rmt::Sin( fClampedTime * 2.0f ) * m_YBias;
    float scalarXOffset = rmt::Cos( fClampedTime ) * m_XBias;

    // Apply the height offset the given transform matrix
    newTransform->m[3][1] += m_CurrentYOffset - oldY;

    // Undo old transform
    newTransform->Row(3) -= m_CurrentXOffset;
    // Apply the newtransform
    m_CurrentXOffset = scalarXOffset * newTransform->Row(0);
    newTransform->Row(3) += m_CurrentXOffset;

    

    return true;
}

WingAnimator::WingAnimator( float FrameRate ) :
    mTranKeys( 0 ),
    mRotKeys( 0 ),
    mAnimSpeed( 0.03f / FrameRate ),
    mTime( 0.0f ),
    mFrame( 0.0f ),
    mRWingIndex( -1 ),
    mLWingIndex( -1 ),
    mNumKeys( 2 )
{
    mTranKeys = new rmt::Vector[ mNumKeys ];
    rAssert( mTranKeys );
    mRotKeys = new rmt::Quaternion[ mNumKeys ];
    rAssert( mRotKeys );
    mTranKeys[ 0 ].Set(  0.0743f, -0.0114f, -0.0003f );
    mTranKeys[ 1 ].Set( -0.1024f,  0.1154f,  0.1085f );
    mRotKeys[ 0 ].x = -0.0017f;
    mRotKeys[ 0 ].y =  0.0000f;
    mRotKeys[ 0 ].z = -0.3592f;
    mRotKeys[ 0 ].w = -0.9332f;
    mRotKeys[ 1 ].x =  0.3550f;
    mRotKeys[ 1 ].y = -0.2419f;
    mRotKeys[ 1 ].z =  0.5907f;
    mRotKeys[ 1 ].w = -0.6830f;
}

WingAnimator::~WingAnimator() 
{
    delete [] mTranKeys;
    delete [] mRotKeys;
};

void WingAnimator::Advance( float Deltams )
{
    const static float ANIM_TIME = 4.0f / 30.0f;
    const static float FRAME_RATIO = 1.0f / ( ANIM_TIME / 2.0f );
    mTime += ( Deltams * mAnimSpeed );
    while( mTime >= ANIM_TIME )
    {
        mTime -= ANIM_TIME;
    }
    mFrame = mTime * FRAME_RATIO;
    // Ping-pong the frame number.
    if( mFrame > 1.0f )
    {
        mFrame = 2.0f - mFrame;
    }
}

void WingAnimator::UpdateForRender( tCompositeDrawable* Drawable )
{
    tPose* pose = Drawable->GetPose();
    if( mRWingIndex < 0 )
    {
        mRWingIndex = pose->FindJointIndex( "Wing_R" );
        rAssert( mRWingIndex >= 0 );
    }
    if( mLWingIndex < 0 )
    {
        mLWingIndex = pose->FindJointIndex( "Wing_L" );
        rAssert( mLWingIndex >= 0 );
    }
    rmt::Vector tran;
    tran.Scale( 1.0f - mFrame, mTranKeys[ 0 ] );
    tran.ScaleAdd( mFrame, mTranKeys[ 1 ] );
    rmt::Quaternion rot;
    rot.Slerp(mRotKeys[ 0 ], mRotKeys[ 1 ], mFrame );

    tPose::Joint* joint;
    joint = pose->GetJoint( mLWingIndex );
    rot.ConvertToMatrix( &joint->objectMatrix );
    joint->objectMatrix.FillTranslate( tran );
    tran.x *= -1.0f;
    rot.y *= -1.0f;
    rot.z *= -1.0f;
    joint = pose->GetJoint( mRWingIndex );
    rot.ConvertToMatrix( &joint->objectMatrix );
    joint->objectMatrix.FillTranslate( tran );
    pose->SetPoseReady( false );
}