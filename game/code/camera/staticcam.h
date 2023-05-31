//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        staticcam.h
//
// Description: Blahblahblah
//
// History:     9/18/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef STATICCAM_H
#define STATICCAM_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>

#ifndef WORLD_BUILDER
#include <camera/supercam.h>
#else
#include "supercam.h"
#endif

//========================================
// Forward References
//========================================
class ISuperCamTarget;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class StaticCam : public SuperCam
{
public:
    StaticCam();
	virtual ~StaticCam();

    //Update: Called when you want the super cam to update its state.
    virtual void Update( unsigned int milliseconds );

    //Returns the name of the super cam.  
    //This can be used in the FE or debug info
    virtual const char* const GetName() const;

    virtual Type GetType();

    //These are for favourable support of this command
    virtual void SetTarget( ISuperCamTarget* target ); 
    virtual void AddTarget( ISuperCamTarget* target );
    virtual unsigned int GetNumTargets() const;

    void SetTargetLag( float lag );
    void SetTargetOffset( const rmt::Vector& offset );
    void SetPosition( const rmt::Vector& position );
    void SetTracking( bool isTracking );

    void SetMaxFOV( float fov );
    void SetFOVLag( float lag );

protected:
    //Init...  This gets called when the camera is registered
    virtual void OnInit() { InitMyController(); };

    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();

    virtual float GetTargetSpeedModifier();

private:
    ISuperCamTarget* mTarget;
    float mTargetLag;
    rmt::Vector mTargetOffset;
    rmt::Vector mTargetPosition;
    rmt::Vector mTargetPositionDelta;
    rmt::Vector mPosition;
    bool mTracking;

    float mFOVDelta;
    float mMaxFOV;
    float mFOVLag;

    //Prevent wasteful constructor creation.
	StaticCam( const StaticCam& staticcam );
	StaticCam& operator=( const StaticCam& staticcam );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************
//=============================================================================
// StaticCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================
inline const char* const StaticCam::GetName() const
{
    return "STATIC_CAM";
}

//=============================================================================
// DebugCam::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
inline SuperCam::Type StaticCam::GetType()
{
    return STATIC_CAM;
}

//=============================================================================
// StaticCam::SetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void StaticCam::SetTarget( ISuperCamTarget* target )
{
    mTarget = target;
}

//=============================================================================
// StaticCam::AddTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void StaticCam::AddTarget( ISuperCamTarget* target )
{
    rAssertMsg( false, "Only call SetTarget on the StaticCam" );
}

//=============================================================================
// StaticCam::GetNumTargets
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int StaticCam::GetNumTargets() const
{
    if ( mTarget )
    {
        return 1;
    }

    return 0;
}

//=============================================================================
// StaticCam::SetTargetLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void StaticCam::SetTargetLag( float lag )
{
    mTargetLag = lag;
}

//=============================================================================
// StaticCam::SetTargetOffset
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& offset )
//
// Return:      void 
//
//=============================================================================
inline void StaticCam::SetTargetOffset( const rmt::Vector& offset )
{
    mTargetOffset = offset;
}

//=============================================================================
// StaticCam::SetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& position )
//
// Return:      void 
//
//=============================================================================
inline void StaticCam::SetPosition( const rmt::Vector& position )
{
    mPosition = position;
}

//=============================================================================
// StaticCam::SetTracking
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool isTracking )
//
// Return:      void 
//
//=============================================================================
inline void StaticCam::SetTracking( bool isTracking )
{
    mTracking = isTracking;
}

//=============================================================================
// StaticCam::SetMaxFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( float fov )
//
// Return:      void 
//
//=============================================================================
inline void StaticCam::SetMaxFOV( float fov )
{
    mMaxFOV = fov;
}

//=============================================================================
// StaticCam::SetFOVLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void StaticCam::SetFOVLag( float lag )
{
    mFOVLag = lag;
}


#endif //STATICCAM_H
