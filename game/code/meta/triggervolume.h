//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        triggervolume.h
//
// Description: Triggervolume class
//
// History:     03/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef TRIGGERVOLUME_H
#define TRIGGERVOLUME_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>
//#include <p3d/entity.hpp>
#ifdef WORLD_BUILDER
#include "../render/DSG/IEntityDSG.h"
#else
#include <render/DSG/IEntityDSG.h>
#endif //WORLD_BUILDER

#include <raddebugwatch.hpp>

//========================================
// Forward References
//========================================

class TriggerLocator;
class pddiShader;

//=============================================================================
//
// Synopsis:    Trigger volumes ahoy
//
//=============================================================================

class TriggerVolume : public IEntityDSG//public tEntity
{
public:
    enum Type
    {
        SPHERE,
        RECTANGLE
    };

    TriggerVolume();
    virtual ~TriggerVolume();

    //Intersect tests
    virtual bool Contains( const rmt::Vector& point, 
                           float epsilon = 0.0f ) const = 0;
    virtual bool IntersectsBox( const rmt::Vector& p1, 
                                const rmt::Vector& p2, 
                                const rmt::Vector& p3, 
                                const rmt::Vector& p4 ) const = 0;

    virtual bool IntersectsBox( const rmt::Box3D& box ) = 0;

    virtual bool IntersectsSphere( const rmt::Vector& position, 
                                   float radius ) const = 0;

    virtual bool IntersectLine( const rmt::Vector& p1, 
                                const rmt::Vector& p2 ) const = 0;

    virtual bool GetBoundingBox( rmt::Vector& p1, rmt::Vector& p2 ) const = 0;
    virtual Type GetType() const = 0;

    TriggerLocator* GetLocator();
    void            SetLocator( TriggerLocator* locator );
    
    void    SetFrameUsed( unsigned int frame, int user );
    unsigned int GetFrameUsed() const;
    int     GetUser() const;

    virtual void Trigger( unsigned int playerID, bool bActive );
    bool IsActive();

    const rmt::Vector&  GetPosition() const;
    void                SetPosition( const rmt::Vector& pos );

    bool IsPlayerTracking( int playerID );
    void SetTrackingPlayer( int playerID, bool on );

    bool TriggerAllowed( int playerID );

    //////////////////////////////////////////////////////////////////////////
    // IEntityDSG Interface
    //////////////////////////////////////////////////////////////////////////
    void DisplayBoundingBox(tColour colour = tColour(0,255,0));
    void DisplayBoundingSphere(tColour colour = tColour(0,255,0));

    virtual void GetBoundingBox(rmt::Box3D* box);
    virtual void GetBoundingSphere(rmt::Sphere* sphere);
    void Display();

    rmt::Vector*       pPosition();
    const rmt::Vector& rPosition();
    void GetPosition( rmt::Vector* ipPosn );

    // Debug drawing stuff
    void Render();

protected:

#ifndef RAD_RELEASE
    // Cache for mesh to draw
    int numVerts;
    int numFaces;

    rmt::Vector* verts;
    unsigned short* faces;
#endif

    virtual void InitPoints() = 0;
    virtual void CalcPoints() = 0;

    void ClearPoints();
private:
    TriggerLocator* mLocator;
    rmt::Vector     mPosition;

    unsigned char mTrackingPlayers; //This is a bit flag.

    //I think these will go away.
    unsigned int    mFrameUsed;
    int             mUser;

    //Prevent wasteful constructor creation.
    TriggerVolume( const TriggerVolume& triggervolume );
    TriggerVolume& operator=( const TriggerVolume& triggervolume );
};

//******************************************************************************
//
// Inline Public Member Functions
//
//******************************************************************************

//=============================================================================
// TriggerVolume::GetLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      TriggerLocator
//
//=============================================================================
inline TriggerLocator* TriggerVolume::GetLocator()
{
    return mLocator;
}

//=============================================================================
// TriggerVolume::SetFrameUsed
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int frame, int user )
//
// Return:      void 
//
//=============================================================================
inline void TriggerVolume::SetFrameUsed( unsigned int frame, int user )
{
    mFrameUsed = frame;
    mUser  = user;
}

//=============================================================================
// TriggerVolume::GetFrameUsed
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned int 
//
//=============================================================================
inline unsigned int TriggerVolume::GetFrameUsed() const
{
    return mFrameUsed;
}

//=============================================================================
// TriggerVolume::GetUser
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      int 
//
//=============================================================================
inline int TriggerVolume::GetUser() const
{
    return mUser;
}

//=============================================================================
// TriggerVolume::GetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline const rmt::Vector& TriggerVolume::GetPosition() const
{
    return( mPosition );
}

//=============================================================================
// TriggerVolume::SetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& pos )
//
// Return:      inline 
//
//=============================================================================
inline void TriggerVolume::SetPosition( const rmt::Vector& pos )
{
    mPosition = pos;
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
inline void TriggerVolume::DisplayBoundingBox(tColour colour)
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
inline void TriggerVolume::DisplayBoundingSphere(tColour colour)
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
inline rmt::Vector* TriggerVolume::pPosition()
{
    return &mPosition;
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
inline const rmt::Vector& TriggerVolume::rPosition()
{
    return mPosition;
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
inline void TriggerVolume::GetPosition( rmt::Vector* ipPosn )
{
    *ipPosn = mPosition;
}

//=============================================================================
// TriggerVolume::IsPlayerTracking
//=============================================================================
// Description: Comment
//
// Parameters:  ( int playerID )
//
// Return:      bool 
//
//=============================================================================
inline bool TriggerVolume::IsPlayerTracking( int playerID )
{
    rAssert( playerID < 32 );
    return ( mTrackingPlayers & ( 1 << playerID ) ) != 0;
}

//=============================================================================
// TriggerVolume::SetTrackingPlayer
//=============================================================================
// Description: Comment
//
// Parameters:  ( int playerID, bool on )
//
// Return:      void 
//
//=============================================================================
inline void TriggerVolume::SetTrackingPlayer( int playerID, bool on )
{
    rAssert( playerID < 32 );

    if ( on )
    {
        mTrackingPlayers |= ( 1 << playerID );
    }
    else
    {
        mTrackingPlayers &= ~( 1 << playerID );
    }
}

#endif //TRIGGERVOLUME_H
