//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        chasecam.h
//
// Description: Blahblahblah
//
// History:     24/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef CHASECAM_H
#define CHASECAM_H

//========================================
// Nested Includes
//========================================
//These are included in the precompiled headers on XBOX and GC
#include <radmath/radmath.hpp>

#include <camera/supercam.h>
#include <camera/ChaseCamData.h>

//========================================
// Forward References
//========================================
class ISuperCamTarget;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class ChaseCam : public SuperCam
{
public:
    ChaseCam();
    virtual ~ChaseCam();

   //Update: Called when you want the super cam to update its state.
    virtual void Update( unsigned int milliseconds );

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

private:
    ISuperCamTarget* mTarget;
    ChaseCamData mData;

    rmt::Vector mPosition;
    rmt::Vector mPositionDelta;
    rmt::Vector mTargetPos;
    rmt::Vector mTargetDelta;

    float mFOVDelta;

    //These functions are to allow real-time control of the settings of 
    //the supercam.
    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();

    void DoCameraCut();

    //Prevent wasteful constructor creation.
    ChaseCam( const ChaseCam& chasecam );
    ChaseCam& operator=( const ChaseCam& chasecam );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// ChaseCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================
inline const char* const ChaseCam::GetName() const
{
    return "CHASE_CAM";
}

//=============================================================================
// ChaseCam::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
inline SuperCam::Type ChaseCam::GetType()
{
    return CHASE_CAM;
}

//=============================================================================
// ChaseCam::SetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void ChaseCam::SetTarget( ISuperCamTarget* target )
{
    mTarget = target;
}

//=============================================================================
// ChaseCam::AddTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void ChaseCam::AddTarget( ISuperCamTarget* target )
{
    rAssertMsg( false, "Only call SetTarget on the ChaseCam" );
}

//=============================================================================
// ChaseCam::GetNumTargets
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int ChaseCam::GetNumTargets() const
{
    if ( mTarget )
    {
        return 1;
    }

    return 0;
}

#endif //CHASECAM_H
