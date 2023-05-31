//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        listener.cpp
//
// Description: Implement Listener class, which tracks the camera and
//              updates the RadSound listener position/orientation to match
//
// History:     02/08/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radmath/radmath.hpp>
#include <radsoundmath.hpp>
#include <radsound_hal.hpp>
#include <p3d/utility.hpp>
#include <p3d/vectorcamera.hpp>

//========================================
// Project Includes
//========================================
#include <sound/listener.h>

#include <sound/soundrenderer/soundrenderingmanager.h>

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>
#include <camera/supercam.h>
#include <worldsim/avatarmanager.h>
#include <mission/gameplaymanager.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

static const float MAX_LISTENER_DIST_FROM_AVATAR = 10.0f;
static const float MAX_LISTENER_DIST_FROM_AVATAR_SQR = 100.0f;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// Listener::Listener
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Listener::Listener() :
    m_theListener( NULL ),
    m_feCamera( NULL )
{
}

//==============================================================================
// Listener::~Listener
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Listener::~Listener()
{
}

//=============================================================================
// Listener::Initialize
//=============================================================================
// Description: Get a handle to the RadSound listener
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void Listener::Initialize( Sound::daSoundRenderingManager& renderMgr )
{
    m_theListener = renderMgr.GetTheListener();
}

//=============================================================================
// Listener::Update
//=============================================================================
// Description: Update the listener's position/orientation
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void Listener::Update( ContextEnum context )
{
    SuperCamCentral* scc;
    SuperCam* camera;
    rmt::Vector position;
    rmt::Vector velocity;
    rmt::Vector heading;
    rmt::Vector up;
    radSoundVector soundVector1;
    radSoundVector soundVector2;
    rmt::Vector diff;
    Avatar* theAvatar;
    rmt::Vector avatarPosn;


    //
    // Don't do anything if we don't have a listener yet
    //
    if( m_theListener == NULL )
    {
        return;
    }

    //
    // Get the camera positioning and pass it on to the listener.
    // QUESTION: is it valid to assume that we'll strictly use player 1?
    // H2H positional sound hasn't really been resolved...
    //
    if( context == CONTEXT_FRONTEND )
    {
        if( m_feCamera == NULL )
        {
            m_feCamera = p3d::find<tVectorCamera>( "FE_Camera" );
            if( m_feCamera == NULL )
            {
                //
                // Camera hasn't been loaded yet, I guess.  Early exit.
                //
                return;
            }
        }

        m_feCamera->GetPosition( &position );
        m_feCamera->GetDirection( &heading );
        m_feCamera->GetUpVector( &up );
        //
        // This class doesn't provide velocities, assume zero.
        //
        velocity.Clear();
    }
    else
    {
        m_feCamera = NULL;

        scc = GetSuperCamManager()->GetSCC( 0 );
        rAssert( scc != NULL );

        camera = scc->GetActiveSuperCam();
        if( camera == NULL )
        {
            //
            // No camera, nothing to update to.
            //
            return;
        }

        camera->GetPosition( &position );
        camera->GetVelocity( &velocity );
        camera->GetHeading( &heading );
        heading.NormalizeSafe();
        //
        // Sadly, the camera defaults allow for a nonsense zero-length
        // heading vector.  Test for this case, and make up a more
        // lenient default if found
        //
        if( ( heading.x == 0.0f ) && ( heading.y == 0.0f ) && ( heading.z == 0.0f ) )
        {
            heading.x = 1.0f;
        }
        camera->GetCameraUp( &up );
        up.NormalizeSafe();

        //
        // We don't want the position to get too far away from the character, so clamp
        // the distance to some arbitrary maximum
        //
        theAvatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
        if( ( theAvatar != NULL )
            && ( GetGameplayManager() != NULL ) 
            && ( !(GetGameplayManager()->IsSuperSprint()) ) )
        {
            theAvatar->GetPosition( avatarPosn );
            diff = position - avatarPosn;
            if( diff.MagnitudeSqr() > MAX_LISTENER_DIST_FROM_AVATAR_SQR )
            {
                diff.Normalize();
                diff.Scale( MAX_LISTENER_DIST_FROM_AVATAR );
                position.Add( avatarPosn, diff );
            }
        }
    }

    // Position
    soundVector1.SetElements( position.x, position.y, position.z );
    m_theListener->SetPosition( &soundVector1 );

    // Velocity
    soundVector1.SetElements( velocity.x, velocity.y, velocity.z );
    m_theListener->SetVelocity( &soundVector1 );

    // Orientation
    soundVector1.SetElements( heading.x, heading.y, heading.z );
    soundVector2.SetElements( up.x, up.y, up.z );

    //
    // Double-check the values, I'm not sure that DirectSound likes
    // zero vectors
    //
    rAssert( ( heading.x != 0.0f )
             || ( heading.y != 0.0f )
             || ( heading.z != 0.0f ) );
    rAssert( ( up.x != 0.0f ) || ( up.y != 0.0f ) || ( up.z != 0.0f ) );

    m_theListener->SetOrientation( &soundVector1, &soundVector2 );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
