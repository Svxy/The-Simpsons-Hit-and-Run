//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        kullcam.h
//
// Description: Blahblahblah
//
// History:     06/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef KULLCAM_H
#define KULLCAM_H

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

class KullCam : public SuperCam
{
public:
    KullCam();
    virtual ~KullCam();

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

    bool mIgnoreDebugController : 1;

private:

    ISuperCamTarget* mTarget;

    InputManager* mIm;

    //These functions are to allow real-time control of the settings of 
    //the supercam.
    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();

    //Prevent wasteful constructor creation.
    KullCam( const KullCam& kullcam );
    KullCam& operator=( const KullCam& kullcam );
};


//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************
//=============================================================================
// KullCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================
inline const char* const KullCam::GetName() const
{
    return "KULL_CAM";
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
inline SuperCam::Type KullCam::GetType()
{
    return KULL_CAM;
}

//=============================================================================
// KullCam::SetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void KullCam::SetTarget( ISuperCamTarget* target )
{
    mTarget = target;
}

//=============================================================================
// KullCam::AddTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void KullCam::AddTarget( ISuperCamTarget* target )
{
    rAssertMsg( false, "Only call SetTarget on the KullCam" );
}

//=============================================================================
// KullCam::GetNumTargets
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int KullCam::GetNumTargets() const
{
    if ( mTarget )
    {
        return 1;
    }

    return 0;
}

#endif //KULLCAM_H
