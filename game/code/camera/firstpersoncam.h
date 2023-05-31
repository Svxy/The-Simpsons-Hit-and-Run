//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        firstpersoncam.h
//
// Description: Blahblahblah
//
// History:     2/21/2003 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef FIRSTPERSONCAM_H
#define FIRSTPERSONCAM_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>

#include <camera/supercam.h>
#include <input/controller.h>


//========================================
// Forward References
//========================================
class ISuperCamTarget;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class FirstPersonCam : public SuperCam
{
public:
    FirstPersonCam();
    virtual ~FirstPersonCam();

    //Update: Called when you want the super cam to update its state.
    virtual void Update( unsigned int milliseconds );
    virtual void UpdateForPhysics( unsigned int milliseconds );


    //Returns the name of the super cam.  
    //This can be used in the FE or debug info
    virtual const char* const GetName() const;

    virtual Type GetType();

    virtual void SetTarget( ISuperCamTarget* target ); 

    virtual unsigned int GetNumTargets() const;

    //Hack
    //This is so if we change views from firstperson in the pause menu we don't screw the controller
    void OverrideOldState( Input::ActiveState state ); 

    //Support for colliding with the world.
    void SetCollisionOffset( const rmt::Vector* offset, unsigned int numCollisions, const rmt::Vector& groundOffset );
    float GetCollisionRadius() const { return GetNearPlane() / 2.0f; };

protected:
    //Override this if you want to get initted.
    virtual void OnInit();
    virtual void OnShutdown();

    //You'll need to overload these if you want debug watcher or other debug controls.
    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();

private:
    ISuperCamTarget* mTarget;
    bool mTargetDirty : 1;
    rmt::Vector mTargetPositionOffset;

    float mRotation;
    float mElevation;
    float mRotationDelta;
    float mElevationDelta;

    Input::ActiveState mOldState;

    float mFOVDelta;

    const rmt::Vector* mCollisionOffset;
    unsigned int mNumCollisions;

    //Prevent wasteful constructor creation.
    FirstPersonCam( const FirstPersonCam& firstpersoncam );
    FirstPersonCam& operator=( const FirstPersonCam& firstpersoncam );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************
//=============================================================================
// FirstPersonCam::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
inline SuperCam::Type FirstPersonCam::GetType()
{
    return SuperCam::FIRST_PERSON_CAM;
}

//=============================================================================
// FirstPersonCam::GetNumTargets
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int FirstPersonCam::GetNumTargets() const
{
    return 1;
}

//=============================================================================
// FirstPersonCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
inline const char* const FirstPersonCam::GetName() const
{
    return "FIRST_PERSON_CAM";
}

//=============================================================================
// FirstPersonCam::SetCollisionOffset
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& offset, unsigned int numCollisions, const rmt::Vector& groundOffset )
//
// Return:      void 
//
//=============================================================================
inline void FirstPersonCam::SetCollisionOffset( const rmt::Vector* offset, unsigned int numCollisions, const rmt::Vector& groundOffset )
{
    mCollisionOffset = offset;
    mNumCollisions = numCollisions;
}
#endif //FIRSTPERSONCAM_H
