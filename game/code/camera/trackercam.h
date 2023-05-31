//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        trackercam.h
//
// Description: Blahblahblah
//
// History:     08/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef TRACKERCAM_H
#define TRACKERCAM_H

//========================================
// Nested Includes
//========================================
#include <camera/supercam.h>
#include <camera/trackercamdata.h>

//========================================
// Forward References
//========================================
class ISuperCamTarget;
class InputManager;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class TrackerCam : public SuperCam
{
public:
    TrackerCam();
    virtual ~TrackerCam();

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

private:
    ISuperCamTarget* mTarget;

    InputManager* mIm;

    TrackerCamData mData;

    float mFOVDelta;
    
    //These functions are to allow real-time control of the settings of 
    //the supercam.
    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();

    //Prevent wasteful constructor creation.
    TrackerCam( const TrackerCam& trackercam );
    TrackerCam& operator=( const TrackerCam& trackercam );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// TrackerCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================
inline const char* const TrackerCam::GetName() const
{
    return "TRACKER_CAM";
}

//=============================================================================
// TrackerCam::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
inline SuperCam::Type TrackerCam::GetType()
{
    return TRACKER_CAM;
}

//=============================================================================
// TrackerCam::SetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void TrackerCam::SetTarget( ISuperCamTarget* target )
{
    mTarget = target;
}

//=============================================================================
// TrackerCam::AddTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void TrackerCam::AddTarget( ISuperCamTarget* target )
{
    rAssertMsg( false, "Only call SetTarget on the TrackerCam" );
}

//=============================================================================
// TrackerCam::GetNumTargets
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int TrackerCam::GetNumTargets() const
{
    if ( mTarget )
    {
        return 1;
    }

    return 0;
}

#endif //TRACKERCAM_H
