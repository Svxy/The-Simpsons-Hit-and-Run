//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        bumpercam.h
//
// Description: Blahblahblah
//
// History:     24/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef BUMPERCAM_H
#define BUMPERCAM_H

//========================================
// Nested Includes
//========================================
#include <camera/supercam.h>
#include <camera/bumpercamdata.h>

//========================================
// Forward References
//========================================
class ISuperCamTarget;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class BumperCam : public SuperCam
{
public:

    enum BumperFlag
    {
        READY = SUPERCAM_END   //This is to carry on from the supercam flags
    };
    
    BumperCam();
    virtual ~BumperCam();

   //Update: Called when you want the super cam to update its state.
    virtual void Update( unsigned int milliseconds );
    virtual void UpdateForPhysics( unsigned int milliseconds );

    //Returns the name of the super cam.  
    //This can be used in the FE or debug info
    virtual const char* const GetName() const;

    //This loads the off-line created settings for the camera.  
    //It is passed in as a byte stream of some data of known size.
    virtual void LoadSettings( unsigned char* settings ); 
 
    virtual Type GetType();

    //These are for favourable support of this command
    virtual void SetTarget( ISuperCamTarget* target ); 
    virtual void AddTarget( ISuperCamTarget* target );
    
    unsigned int GetNumTargets() const;

    //Support for colliding with the world.
    void SetCollisionOffset( const rmt::Vector* offset, unsigned int numCollisions, const rmt::Vector& groundOffset );
    float GetCollisionRadius() const { return GetNearPlane(); };

private:

    //These functions are to allow real-time control of the settings of 
    //the supercam.
    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();

    BumperCamData mData;
    ISuperCamTarget* mTarget;

    rmt::Vector mGroundOffset;

    //Prevent wasteful constructor creation.
    BumperCam( const BumperCam& bumpercam );
    BumperCam& operator=( const BumperCam& bumpercam );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// BumperCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================
inline const char* const BumperCam::GetName() const
{
    return "BUMPER_CAM";
}

//=============================================================================
// BumperCam::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
inline SuperCam::Type BumperCam::GetType()
{
    return BUMPER_CAM;
}

//=============================================================================
// BumperCam::SetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void BumperCam::SetTarget( ISuperCamTarget* target )
{
    mTarget = target;
}

//=============================================================================
// BumperCam::AddTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void BumperCam::AddTarget( ISuperCamTarget* target )
{
    rAssertMsg( false, "Only call SetTarget on the BumperCam" );
}

//=============================================================================
// BumperCam::GetNumTargets
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int BumperCam::GetNumTargets() const
{
    if ( mTarget )
    {
        return 1;
    }

    return 0;
}


#endif //BUMPERCAM_H
