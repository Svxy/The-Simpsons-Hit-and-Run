//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        cameraplayer.cpp
//
// Description: Implement CameraPlayer
//
// History:     22/04/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/utility.hpp>

//========================================
// Project Includes
//========================================
#include <presentation/cameraplayer.h>

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
// CameraPlayer::CameraPlayer
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
CameraPlayer::CameraPlayer() :
    mpAnimation( NULL )
{
    SetExclusive( false );    
}

//==============================================================================
// CameraPlayer::~CameraPlayer
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
CameraPlayer::~CameraPlayer()
{
}

//=============================================================================
// CameraPlayer::ClearData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void CameraPlayer::ClearData()
{
    SimpleAnimationPlayer::ClearData();

    if( mpAnimation != NULL )
    {
        mpAnimation->Release();
        mpAnimation = NULL;
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// CameraPlayer::DoLoaded
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void CameraPlayer::DoLoaded()
{
    SimpleAnimationPlayer::DoLoaded();
    
    mpAnimation = p3d::find<tAnimation>( GetAnimationName() );
    rAssert( mpAnimation );
    mpAnimation->AddRef();
}

