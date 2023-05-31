//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        reversecam.h
//
// Description: Blahblahblah
//
// History:     11/14/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef REVERSECAM_H
#define REVERSECAM_H

//========================================
// Nested Includes
//========================================
#include <camera/supercam.h>

//========================================
// Forward References
//========================================
class ISuperCamTarget;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class ReverseCam : public SuperCam
{
public:
    ReverseCam();
	virtual ~ReverseCam();

   //Update: Called when you want the super cam to update its state.
    virtual void Update( unsigned int milliseconds );
    virtual void UpdateForPhysics( unsigned int milliseconds );

    //Returns the name of the super cam.  
    //This can be used in the FE or debug info
    virtual const char* const GetName() const;

    //This loads the off-line created settings for the camera.  
    //It is passed in as a byte stream of some data of known size.
    virtual void LoadSettings( unsigned char* settings ) {}; 

    virtual Type GetType();

    //These are for favourable support of this command
    virtual void SetTarget( ISuperCamTarget* target ); 
    virtual void AddTarget( ISuperCamTarget* target );
    
    unsigned int GetNumTargets() const;

    void EnableShake();
    void DisableShake();

    //Support for colliding with the world.
    void SetCollisionOffset( const rmt::Vector* offset, unsigned int numCollisions, const rmt::Vector& groundOffset );
    float GetCollisionRadius() const { return mCollisionRadius; };

protected:
    bool CanSwitch();
    virtual void OnInit() { InitMyController(); };

    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();

private:
    ISuperCamTarget* mTarget;

    rmt::Vector mPosition;
    rmt::Vector mPositionDelta;

    rmt::Vector mTargetPosition;
    rmt::Vector mTargetPositionDelta;

    float mYHeight;
    float mMagnitude;
    float mLag;
    float mTargetLag;

    const rmt::Vector* mCollisionOffset;
    float mCollisionRadius;
    unsigned int mNumCollisions;
    rmt::Vector mGroundOffset;

    float mOtherY;

    //Prevent wasteful constructor creation.
	ReverseCam( const ReverseCam& reversecam );
	ReverseCam& operator=( const ReverseCam& reversecam );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// ReverseCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================

inline const char* const ReverseCam::GetName() const
{
    return "REVERSE_CAM";
}

//=============================================================================
// ReverseCam::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
inline SuperCam::Type ReverseCam::GetType()
{
    return REVERSE_CAM;
}


//=============================================================================
// ReverseCam::SetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void ReverseCam::SetTarget( ISuperCamTarget* target )
{
    mTarget = target;
}

//=============================================================================
// ReverseCam::AddTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void ReverseCam::AddTarget( ISuperCamTarget* target )
{
    rAssertMsg( false, "Only call SetTarget on the ReverseCam" );
}

//=============================================================================
// ReverseCam::GetNumTargets
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int ReverseCam::GetNumTargets() const
{
    if ( mTarget )
    {
        return 1;
    }

    return 0;
}

//=============================================================================
// ReverseCam::SetCollisionOffset
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& offset, unsigned int numCollisions, const rmt::Vector& groundOffset )
//
// Return:      void 
//
//=============================================================================
inline void ReverseCam::SetCollisionOffset( const rmt::Vector* offset, unsigned int numCollisions, const rmt::Vector& groundOffset )
{
    mCollisionOffset = offset;
    mNumCollisions = numCollisions;
    mGroundOffset = groundOffset;
}

#endif //REVERSECAM_H
