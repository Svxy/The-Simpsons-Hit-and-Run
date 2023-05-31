//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        SineCosShaker.cpp
//
// Description: Implement SineCosShaker
//
// History:     02/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <raddebugwatch.hpp>

#ifdef WORLD_BUILDER
#include "main/toolhack.h"
#endif

#include <p3d/pointcamera.hpp>

//========================================
// Project Includes
//========================================
#ifndef WORLD_BUILDER
#include <camera/SineCosShaker.h>
#include <camera/SuperCam.h>
#include <camera/supercamconstants.h>
#include <events/eventdata.h>
#else
#include "SineCosShaker.h"
#include "SuperCam.h"
#include "supercamconstants.h"
#include "events/eventdata.h"
#endif



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

//==============================================================================
// SineCosShaker::SineCosShaker
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SineCosShaker::SineCosShaker() :
    mTime( 180 ),
    mCurrentTime( 0 ),
    mSpeed( 0.0f ),
    mCamera( NULL ),
    mIsCameraRelative( true ),
    mAmpYIncrement( 0.942478f ),
    mAmplitudeY( 0.87f ),
    mAmpScale( 1.0f ),
    mAmpScaleMax( 1.10f ),
    mLooping( false )
{
    mDirection.Set( 0.0f, 1.0f, 0.0f );
}

//==============================================================================
// SineCosShaker::~SineCosShaker
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SineCosShaker::~SineCosShaker()
{
    tRefCounted::Release( mCamera );
}

//=============================================================================
// SineCosShaker::SetCamera
//=============================================================================
// Description: Comment
//
// Parameters:  ( tCamera* camera )
//
// Return:      void 
//
//=============================================================================
void SineCosShaker::SetCamera( tPointCamera* camera )
{
    tRefCounted::Assign( mCamera, camera );
}

//=============================================================================
// SineCosShaker::ShakeCamera
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* pos, 
//                rmt::Vector* targ, 
//                unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void SineCosShaker::ShakeCamera( rmt::Vector* pos, 
                                 rmt::Vector* targ, 
                                 unsigned int milliseconds )
{
    if ( mCurrentTime < mTime || mLooping )
    {
        //This is to adjust interpolation when we're running substeps.
        float timeMod = (float)milliseconds / EXPECTED_FRAME_RATE * mSpeed;
        
        mAmplitudeY += mAmpYIncrement * timeMod;
        
        float amp = rmt::Sin( mAmplitudeY );

        if ( !mLooping )
        {
            mAmpScale = mAmpScaleMax - ( ( mAmpScaleMax ) * ( (float)mCurrentTime / (float)mTime ) );
        }
        else
        {
            mAmpScale = 1.0f;
        }


        rmt::Vector dir = mDirection;
//        rmt::Vector dir = rmt::Vector( 1.0f, 0.0f, 0.0f );

//        const rmt::Matrix mat = mCamera->GetWorldToCameraMatrix();

//        dir.Transform( mat );
//        dir.NormalizeSafe();
//        dir.Scale( amp * mAmpScale );  

        dir.Scale( amp * mAmpScale );   

        pos->Sub( dir );
    }

    mCurrentTime += milliseconds;
}

//=============================================================================
// SineCosShaker::RegisterDebugInfo
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SineCosShaker::RegisterDebugInfo()
{
#ifdef DEBUGWATCH
    radDbgWatchAddUnsignedInt( &mTime, "Total Time", "SuperCam\\Shaker\\SineCos", NULL, NULL, 0, 10000 );
    radDbgWatchAddFloat( &mSpeed, "Speed", "SuperCam\\Shaker\\SineCos", NULL, NULL, 0.0f, 100.0f );
    radDbgWatchAddFloat( &mAmpYIncrement, "Amplitude Y Increment", "SuperCam\\Shaker\\SineCos", NULL, NULL, 0.0f, rmt::PI_2 );
    radDbgWatchAddFloat( &mAmpScaleMax, "Amplitude Scale Max", "SuperCam\\Shaker\\SineCos", NULL, NULL, 0.0f, 100.0f );
    radDbgWatchAddBoolean( &mIsCameraRelative, "Camera Relative", "SuperCam\\Shaker\\SineCos" );
    radDbgWatchAddBoolean( &mLooping, "Loop", "SuperCam\\Shaker\\SineCos" );
#endif
}

//=============================================================================
// SineCosShaker::UnregisterDebugInfo
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SineCosShaker::UnregisterDebugInfo()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &mTime );
    radDbgWatchDelete( &mSpeed );
    radDbgWatchDelete( &mAmpYIncrement );
    radDbgWatchDelete( &mAmpScaleMax );
    radDbgWatchDelete( &mIsCameraRelative );
    radDbgWatchDelete( &mLooping );
#endif
}

//=============================================================================
// SineCosShaker::SetShakeData
//=============================================================================
// Description: Comment
//
// Parameters:  ( const ShakeEventData* data )
//
// Return:      void 
//
//=============================================================================
void SineCosShaker::SetShakeData( const ShakeEventData* data )
{
    mDirection = data->direction;
    SetLooping( data->looping );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

