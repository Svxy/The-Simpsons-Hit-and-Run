//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement TriggerVolume
//
// History:     03/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#ifdef WORLD_BUILDER
#include "main/toolhack.h"
#endif

#include <p3d/utility.hpp>
#include <raddebug.hpp>
#include <raddebugwatch.hpp>

//========================================
// Project Includes
//========================================
#ifndef WORLD_BUILDER
#include <meta/triggervolume.h>
#include <meta/triggerlocator.h>
#include <meta/locator.h>
#include <meta/triggervolumetracker.h>
#else
#include "triggervolume.h"
#include "triggerlocator.h"
#include "locator.h"
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
// TriggerVolume::TriggerVolume
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
TriggerVolume::TriggerVolume() :
    mLocator( NULL ),
    mPosition( rmt::Vector( 0.0f, 0.0f, 0.0f ) ),
    mTrackingPlayers( 0 ),
    mFrameUsed( 0 ),
    mUser( 0 )
    {
#ifndef WORLD_BUILDER
#ifndef RAD_RELEASE
    verts = 0;
    faces = 0;
#endif
#endif
}

//==============================================================================
// TriggerVolume::~TriggerVolume
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
TriggerVolume::~TriggerVolume()
{
    ClearPoints();

    if ( mLocator )
    {
        mLocator = NULL;
    }
}


void TriggerVolume::Trigger( unsigned int playerID, bool bActive )
{
    rAssert( mLocator != NULL );

    mLocator->Trigger( playerID, bActive );
}

//=============================================================================
// TriggerVolume::SetLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ( TriggerLocator* locator )
//
// Return:      void 
//
//=============================================================================
void TriggerVolume::SetLocator( TriggerLocator* locator )
{
    rAssert( NULL == mLocator);
    mLocator = locator;
    //mLocator->AddRef();
}

//=============================================================================
// TriggerVolume::IsActive
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool TriggerVolume::IsActive()
{
    rAssert( mLocator != NULL );

    return( mLocator->GetFlag( Locator::ACTIVE ) );
}

//=============================================================================
// TriggerVolume::Render
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TriggerVolume::Render()
{
#ifndef WORLD_BUILDER
#ifndef RAD_RELEASE

    if ((verts == 0) || (faces == 0))
    {
        InitPoints ();
    }

    CalcPoints ();

    pddiCullMode cm = p3d::pddi->GetCullMode();
    p3d::pddi->SetCullMode( PDDI_CULL_INVERTED );

    pddiPrimStream* stream = p3d::pddi->BeginPrims( GetTriggerVolumeTracker()->GetMaterial(), PDDI_PRIM_TRIANGLES, PDDI_V_C, numFaces );

    unsigned int vert = 0;
    pddiColour colour( 128, 128, 128, 64 );

    for( int i = 0; i < numFaces; i++ )
    {
        vert = faces[ i ];

        stream->Colour( colour );
        stream->Coord( verts[ vert ].x, verts[ vert ].y, verts[ vert ].z );
    }

    p3d::pddi->EndPrims( stream );

    p3d::pddi->SetCullMode( cm );
#endif
#endif
}
//========================================================================
// triggervolume::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void TriggerVolume::GetBoundingBox(rmt::Box3D* box)
{
    rAssert(false);
}
//========================================================================
// triggervolume::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void TriggerVolume::GetBoundingSphere(rmt::Sphere* sphere)
{
    rAssert(false);
}
//========================================================================
// triggervolume::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void TriggerVolume::Display()
{
    if(( GetLocator()->GetFlag( Locator::ACTIVE )) 
        && ( GetLocator()->GetFlag( Locator::DRAWN )))
        Render();
}

//=============================================================================
// TriggerVolume::TriggerAllowed
//=============================================================================
// Description: Comment
//
// Parameters:  ( int playerID )
//
// Return:      bool 
//
//=============================================================================
bool TriggerVolume::TriggerAllowed( int playerID )
{
    if ( mLocator == NULL )
    {
        return true;
    }

    return mLocator->TriggerAllowed( playerID );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// TriggerVolume::ClearPoints
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TriggerVolume::ClearPoints()
{
#ifndef WORLD_BUILDER
#ifndef RAD_RELEASE
    delete[] verts;
    delete[] faces;
    verts = 0;
    faces = 0;

    numVerts = 0;
    numFaces = 0;
#endif
#endif
}
