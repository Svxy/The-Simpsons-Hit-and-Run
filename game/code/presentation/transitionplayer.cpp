//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        transitionplayer.cpp
//
// Description: Implement TransitionPlayer
//
// History:     02/05/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/view.hpp>

//========================================
// Project Includes
//========================================
#include <presentation/transitionplayer.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/renderlayer.h>


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
// TransitionPlayer::TransitionPlayer
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
TransitionPlayer::TransitionPlayer() :
    mpLayer1( NULL ),
    mpLayer2( NULL ),
    miIndex( static_cast< unsigned int >( -1 ) )
{
    //init msInfo?
}

//==============================================================================
// TransitionPlayer::~TransitionPlayer
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
TransitionPlayer::~TransitionPlayer()
{
}

void TransitionPlayer::SetTransition( TransitionInfo* info )
{
    rAssert( GetState() != ANIM_IDLE );

    msInfo = *info;

    RenderManager* rm = GetRenderManager();

    mpLayer1 = rm->mpLayer( msInfo.layer1 );
    mpLayer2 = rm->mpLayer( msInfo.layer2 );

    miIndex = 0;

    rAssert( msInfo.length != 0 );
}

void TransitionPlayer::Update( unsigned int elapsedTime )
{
    if ( GetState() == ANIM_PLAYING ) 
    {
        if (miIndex == 0)
        {
            if ( mpLayer2->IsFrozen() )
            {
                mpLayer2->Thaw();
            }
            else if ( mpLayer2->IsDead() )
            {
                mpLayer2->Resurrect();
            }
        }

        miIndex += elapsedTime;

        if ( miIndex < msInfo.length )
        {
            DoUpdate( elapsedTime ); 
        }
        else
        {
            mpLayer1->Freeze();

            Stop();
        }
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

void TransitionPlayer::DoUpdate( unsigned int elapsedTime )
{
    switch ( msInfo.type )
    {
    case TRANS_WIPE_RIGHT:
        {
            float f1 = static_cast<float>( miIndex );
            float f2 = static_cast<float>( msInfo.length );

            float r = f1 / f2;

            tView* view = mpLayer2->pView( 0 );
            view->SetWindow( 0.0f, 0.0f, r, 1.0f );

            view = mpLayer1->pView( 0 );
            view->SetWindow( r, 0.0f, 1.0f, 1.0f );
            break;
        }
    default :
        {
            //nothing
        }
    }
}

//=============================================================================
// TransitionPlayer::DoRender
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TransitionPlayer::DoRender()
{
}
