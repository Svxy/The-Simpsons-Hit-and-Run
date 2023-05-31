//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        nisplayer.cpp
//
// Description: Implement NISPlayer
//
// History:     16/04/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/utility.hpp>

//========================================
// Project Includes
//========================================
#include <presentation/nisplayer.h>
#include <loading/filehandlerenum.h>
#include <loading/loadingmanager.h>

#include <render/rendermanager/rendermanager.h>


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
// NISPlayer::NISPlayer
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
NISPlayer::NISPlayer() :
    mpSceneGraph( NULL )
{
    SetExclusive( false );    
}

//==============================================================================
// NISPlayer::~NISPlayer
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
NISPlayer::~NISPlayer()
{
    
}


//=============================================================================
// NISPlayer::ClearData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void NISPlayer::ClearData()
{
    SimpleAnimationPlayer::ClearData();

    if( mpSceneGraph != NULL )
    {
        GetRenderManager()->mEntityDeletionList.Add((tRefCounted*&)mpSceneGraph);
        mpSceneGraph = NULL;
    }
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// NISPlayer::DoLoaded
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void NISPlayer::DoLoaded()
{
    SimpleAnimationPlayer::DoLoaded();

    tRefCounted::Release(mpSceneGraph);
    tRefCounted::Assign(mpSceneGraph, (tDrawable*)p3d::find<Scenegraph::Scenegraph>( GetAnimationName() ));
    if(!mpSceneGraph)
    {
        tRefCounted::Assign(mpSceneGraph, (tDrawable*)p3d::find<tCompositeDrawable>( GetAnimationName() ));
    }
}

//=============================================================================
// NISPlayer::DoRender
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void NISPlayer::DoRender()
{
    SimpleAnimationPlayer::DoRender();
    
    if(mpSceneGraph)
        mpSceneGraph->Display();
}

//=============================================================================
// NISPlayer::GetBoundingBox
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Box3D* box )
//
// Return:      true if box filled out, false otherwise 
//
//=============================================================================
bool NISPlayer::GetBoundingBox( rmt::Box3D* box )
{
    bool retval = false;

    if( mpSceneGraph )
    {
        mpSceneGraph->GetBoundingBox( box );
        retval = true;
    }

    return( retval );
}
