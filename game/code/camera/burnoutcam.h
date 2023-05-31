//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        burnoutcam.h
//
// Description: Blahblahblah
//
// History:     10/7/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef BURNOUTCAM_H
#define BURNOUTCAM_H

//========================================
// Nested Includes
//========================================
#include <camera/supercam.h>
#include <camera/isupercamtarget.h>

//========================================
// Forward References
//========================================
class InputManager;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class BurnoutCam : public SuperCam
{
public:
    BurnoutCam();
	virtual ~BurnoutCam();

    //Update: Called when you want the super cam to update its state.
    void Update( unsigned int milliseconds );

    //Returns the name of the super cam.  
    //This can be used in the FE or debug info
    const char* const GetName() const;

    Type GetType();

    //These are for favourable support of this command
    void SetTarget( ISuperCamTarget* target ); 
    void AddTarget( ISuperCamTarget* target );

    unsigned int GetNumTargets() const;

protected:

    float mRotation;
    float mElevation;
    float mMagnitude;

private:

    ISuperCamTarget* mTarget;

    InputManager* mIm;

    rmt::Vector mTargetOffset;

    //These functions are to allow real-time control of the settings of 
    //the supercam.
    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();

    //Prevent wasteful constructor creation.
	BurnoutCam( const BurnoutCam& burnoutcam );
	BurnoutCam& operator=( const BurnoutCam& burnoutcam );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************
//=============================================================================
// BurnoutCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================
inline const char* const BurnoutCam::GetName() const
{
    return "BURNOUT_CAM";
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
inline SuperCam::Type BurnoutCam::GetType()
{
    return BURNOUT_CAM;
}

//=============================================================================
// BurnoutCam::SetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void BurnoutCam::SetTarget( ISuperCamTarget* target )
{
    mTarget = target;
}

//=============================================================================
// BurnoutCam::AddTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void BurnoutCam::AddTarget( ISuperCamTarget* target )
{
    rAssertMsg( false, "Only call SetTarget on the BurnoutCam" );
}

//=============================================================================
// BurnoutCam::GetNumTargets
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int BurnoutCam::GetNumTargets() const
{
    if ( mTarget )
    {
        return 1;
    }

    return 0;
}

#endif //BURNOUTCAM_H
