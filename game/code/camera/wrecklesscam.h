//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        wrecklesscam.h
//
// Description: Blahblahblah
//
// History:     25/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef WRECKLESSCAM_H
#define WRECKLESSCAM_H

//========================================
// Nested Includes
//========================================
#include <camera/supercam.h>
#include <camera/wrecklesseventlistener.h>

//========================================
// Forward References
//========================================
class ISuperCamTarget;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class WrecklessCam : public SuperCam
{
public:
    enum
    {
        MAX_TARGETS = 3
    };

    WrecklessCam();
    virtual ~WrecklessCam();

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

    EventListener* GetEventListener();

protected:
    virtual void OnInit();
    virtual void OnShutdown();

private:
    ISuperCamTarget* mTargets[ MAX_TARGETS ];
    unsigned int mNumTargets;
    unsigned int mActiveTarget;

    WrecklessEventListener mEventListener;

    float mFOV;
    float mFOVDelta;

    float mMaxFOV;
    float mMinFOV;
    float mMaxDistance;
    float mMinDistance;
    float mLag;

    //These functions are to allow real-time control of the settings of 
    //the supercam.
    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();

    //Prevent wasteful constructor creation.
    WrecklessCam( const WrecklessCam& wrecklesscam );
    WrecklessCam& operator=( const WrecklessCam& wrecklesscam );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// WrecklessCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================
inline const char* const WrecklessCam::GetName() const
{
    return "WRECKLESS_CAM";
}

//=============================================================================
// WrecklessCam::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
inline SuperCam::Type WrecklessCam::GetType()
{
    return WRECKLESS_CAM;
}

//=============================================================================
// WrecklessCam::GetNumTargets
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int WrecklessCam::GetNumTargets() const
{
    return mNumTargets;
}

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// WrecklessCam::GetEventListener
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      EventListener
//
//=============================================================================
inline EventListener* WrecklessCam::GetEventListener()
{
    return &mEventListener;
}

#endif //WRECKLESSCAM_H
