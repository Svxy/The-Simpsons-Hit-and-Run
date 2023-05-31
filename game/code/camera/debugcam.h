//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        debugcam.h
//
// Description: Blahblahblah
//
// History:     24/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef DEBUGCAM_H
#define DEBUGCAM_H

//========================================
// Nested Includes
//========================================
#include <camera/supercam.h>

//========================================
// Forward References
//========================================
class InputManager;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class DebugCam : public SuperCam
{
public:
    DebugCam();
    virtual ~DebugCam();
  
    //Update: Called when you want the super cam to update its state.
    void Update( unsigned int milliseconds );

    //Returns the name of the super cam.  
    //This can be used in the FE or debug info
    const char* const GetName() const;

    Type GetType();

    void EnableShake();
    void DisableShake();

private:

    float mRotationAngleXZ;
    float mRotationAngleY;

    InputManager* mIm;
  
    //These functions are to allow real-time control of the settings of 
    //the supercam.
    void OnRegisterDebugControls();
    void OnUnregisterDebugControls();

    //Prevent wasteful constructor creation.
    DebugCam( const DebugCam& debugcam );
    DebugCam& operator=( const DebugCam& debugcam );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// DebugCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================
inline const char* const DebugCam::GetName() const
{
    return "DEBUG_CAM";
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
inline SuperCam::Type DebugCam::GetType()
{
    return DEBUG_CAM;
}

#endif //DEBUGCAM_H
