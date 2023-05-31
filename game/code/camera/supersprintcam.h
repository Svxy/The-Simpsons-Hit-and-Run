//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        supersprintcam.h
//
// Description: Blahblahblah
//
// History:     2/8/2003 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef SUPERSPRINTCAM_H
#define SUPERSPRINTCAM_H

//========================================
// Nested Includes
//========================================
#include <camera/supercam.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class SuperSprintCam : public SuperCam
{
public:
        SuperSprintCam();
        virtual ~SuperSprintCam();

        virtual void Update( unsigned int milliseconds );

        //Returns the name of the super cam.  
        //This can be used in the FE or debug info
        virtual const char* const GetName() const;

        virtual Type GetType();

protected:
    virtual void OnInit();
    virtual void OnShutdown();

    //You'll need to overload these if you want debug watcher or other debug controls.
    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();

    float mNear, mFar, mFOV, mAspect;
    rmt::Vector mPos, mTarg, mUp;

    rmt::Vector mPosDelta;
    float mLag;
    float mDist;

private:
    static unsigned int mSprintCamCount;

    //Prevent wasteful constructor creation.
    SuperSprintCam( const SuperSprintCam& supersprintcam );
    SuperSprintCam& operator=( const SuperSprintCam& supersprintcam );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// SuperSprintCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
inline const char* const SuperSprintCam::GetName() const 
{
    return "SuperSprintCam";
}

//=============================================================================
// SuperSprintCam::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      SuperCam
//
//=============================================================================
inline SuperCam::Type SuperSprintCam::GetType()
{
    return SuperCam::SUPER_SPRINT_CAM;
}

#endif //SUPERSPRINTCAM_H
