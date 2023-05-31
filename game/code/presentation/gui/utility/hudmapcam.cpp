//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   HudMapCam
//
// Description: Implementation of the CHudMap class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/10      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/utility/hudmapcam.h>

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>

#include <raddebug.hpp>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

HudMapCam::HudMapCam( int playerID )
:   KullCam(),
    m_camera( NULL ),
    m_originalHeading( -1, 0, 0 )
{
    SetPlayerID( playerID );
    mIgnoreDebugController = true;

    m_camera = new tPointCamera;
    m_camera->AddRef();
    SetCamera( m_camera );

    // override default KullCam parameters
    mElevation = 0.0f;
    mMagnitude = 150.0f;

    // fix hud map cam FOV
    //
    this->SetFOV( rmt::PI_BY2 );
}

HudMapCam::~HudMapCam()
{
    if( m_camera != NULL )
    {
        m_camera->Release();
        m_camera = NULL;
    }
}

void
HudMapCam::Update( unsigned int milliseconds )
{
    // adjust kull cam rotation to match main camera heading
    //
    rmt::Vector camHeading;
    GetSuperCamManager()->GetSCC( this->GetPlayerID() )->GetActiveSuperCam()->GetHeading( &camHeading );
    camHeading.y = 0;

    if( camHeading.MagnitudeSqr() > 0 )
    {
        float ratio = camHeading.DotProduct( m_originalHeading ) / camHeading.Magnitude();
        if( ratio > 1.0f )
        {
            mRotation = 0.0f;
        }
        else if( ratio < -1.0f )
        {
            mRotation = rmt::PI;
        }
        else
        {
            mRotation = rmt::ACos( ratio );
        }

        rAssert( !rmt::IsNan( mRotation ) );

        rmt::Vector normal = camHeading;
        normal.CrossProduct( m_originalHeading );
        if( normal.y < 0 )
        {
            mRotation = -mRotation;
        }
    }

    KullCam::Update( milliseconds );

    // fix hud map cam at 'mMagnitude' above sea level
    //
    rmt::Vector camPosition = m_camera->GetPosition();
    camPosition.y = mMagnitude;
    m_camera->SetPosition( camPosition );
}

void
HudMapCam::SetHeight( float height )
{
    rAssert( height > 0.0f );
    mMagnitude = height;
}

