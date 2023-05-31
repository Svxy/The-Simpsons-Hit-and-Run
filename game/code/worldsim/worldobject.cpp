//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        worldobject.cpp
//
// Description: Implement WorldObject
//
// History:     14/05/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/matrixstack.hpp>
#include <p3d/utility.hpp>

//========================================
// Project Includes
//========================================
#include <worldsim/worldobject.h>
#include <debug/profiler.h>

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
// WorldObject::WorldObject
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
WorldObject::WorldObject( tDrawable* drawable )
{
    if( drawable != NULL )
    {
        mDrawable = drawable;
        mDrawable->AddRef();
    }
    else
    {
        mDrawable = NULL;
    }

    mTransform.Identity();
}

//==============================================================================
// WorldObject::~WorldObject
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
WorldObject::~WorldObject()
{
    if( mDrawable != NULL )
    {
        mDrawable->Release();
    }
}

//=============================================================================
// WorldObject::SetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector &position )
//
// Return:      void 
//
//=============================================================================
void WorldObject::SetPosition( rmt::Vector &position )
{
    mTransform.FillTranslate( position );
}

//=============================================================================
// WorldObject::Display
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WorldObject::Display()
{
    BEGIN_PROFILE("WorldObject::Display")
    if( mDrawable != NULL )
    {
        p3d::stack->PushMultiply( mTransform );

        mDrawable->Display();

        p3d::stack->Pop();
    }
    END_PROFILE("WorldObject::Display")
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
