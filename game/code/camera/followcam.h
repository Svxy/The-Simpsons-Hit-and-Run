//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        followcam.h
//
// Description: The follow cam is the main camera mode for in-car gameplay.
//              It works though interpolating the angles of a spherical
//              co-ordinate system localized to the target.  
//
//              The spherical co-ordinate system uses two angles and a 
//              magnitude to describe a "rod" in space.  The two angles are:
//              1.  An angle representing rotation in the XZ plane; and
//              2.  An angle representing rotation in the Y plane.
//
//              The magnitude is simply the length of the "rod" going outward
//              from the target.
//
//              Interpolation of the angles is calculated through knowing what
//              the current angle is and what the desired angle should be.  A
//              "lag" factor is introduced that controls the interval of
//              interpolation.  Essentially, the value represents the 
//              parametric position after interpolation between the current
//              and desired angles.
//
//              The values of the "lag" are between 0 and 1, where 0 results 
//              in no interpolation and 1 is a "snap" to the desired angle ( a
//              value of one is also no interpolation, it is more like an 
//              assignment of position )
//      
//              An internal value of rotation delta or "rate" holds the
//              intermediate value of the interpolation so that further
//              interpolation to a new desired angle is continuous.
//
//              If the current angle is overridden by a camera cut, or other
//              means, the rotation delta must be reset to the same value or
//              offset by the same amount as the current angle was changed.
//              Otherwise, the interpolation will attept to interpolate to 
//              some whacky asmyptote or possibly 0 or infinity...
//
// History:     22/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef FOLLOWCAM_H
#define FOLLOWCAM_H

//========================================
// Nested Includes
//========================================
//These are included in the precompiled headers on XBOX and GC
#include <radmath/radmath.hpp>

#include <camera/supercam.h>
#include <camera/FollowCamData.h>

//========================================
// Forward References
//========================================

class ISuperCamTarget;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class FollowCam : public SuperCam
{
public:
    enum
    {
        MAX_TARGETS = 3
    };

    enum Follow_Flag
    {
        FOLLOW = SUPERCAM_END,  //This is to carry on from the supercam flags
        UNSTABLE,               //When the car is unstable this is true
        STABILIZING,
        QUICK_TURN_ALERT,       //We have been in a quick turn situation
        QUICK_TURN,             //We are out of UNSTABLE, into QUICK_TURN
        LOOKING_BACK,
        LOOKING_SIDEWAYS,
        COLLIDING,
        LOS_CORRECTED           //This is when we've corrected due to line of sight.
    };
    
    enum FollowType
    {
        FOLLOW_NEAR,
        FOLLOW_FAR
    };

    FollowCam( FollowType type = FOLLOW_NEAR );
    virtual ~FollowCam();

   //Update: Called when you want the super cam to update its state.
    virtual void Update( unsigned int milliseconds );
    virtual void UpdateForPhysics( unsigned int milliseconds );


    //Returns the name of the super cam.  
    //This can be used in the FE or debug info
    virtual const char* const GetName() const;

    //This loads the off-line created settings for the camera.  
    //It is passed in as a byte stream of some data of known size.
    virtual void LoadSettings( unsigned char* settings ); 

    // copies the FollowCam's members into the FollowCamData structure
    void CopyToData();

    virtual Type GetType();

    //These are for favourable support of this command
    virtual void SetTarget( ISuperCamTarget* target ); 
    virtual void AddTarget( ISuperCamTarget* target );
    
    unsigned int GetNumTargets() const;

    void SetDirty();

    void EnableShake();
    void DisableShake();

    //Support for colliding with the world.
    void SetCollisionOffset( const rmt::Vector* offset, unsigned int numCollisions, const rmt::Vector& groundOffset );
    float GetCollisionRadius() const { return GetNearPlane() * 2.0f; };

    virtual float GetIntersectionRadius() const { return mData.GetMagnitude(); };


    bool ShouldReverse() const;

protected:
    virtual void OnInit();

private:

    ISuperCamTarget* mTargets[ MAX_TARGETS ];
    unsigned int mNumTargets;
    unsigned int mActiveTarget;

    //Camera brains
    float mRotationAngleXZ;
    float mRotationAngleY;
    float mMagnitude;

    float mRotationAngleXZDelta;
    float mRotationAngleYDelta;
    float mMagnitudeDelta;

    rmt::Vector mTargetPosition;
    rmt::Vector mTargetPositionDelta;

    rmt::Vector mUnstablePosition;
    rmt::Vector mUnstableTarget;
    rmt::Vector mOldTargetPos;

    FollowCamData mData;
    
    //Unstable time counter
    int mUnstableDelayTimeLeft;

    //Quick turn time counter
    int mQuickTurnTimeLeft;

    const rmt::Vector* mCollisionOffset;
    unsigned int mNumCollisions;
    rmt::Vector mGroundOffset;

    FollowType mFollowType;

    float mXAxis;

    //These functions are to allow real-time control of the settings of 
    //the supercam.
    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();

    void DoCameraCut();
    void InitUnstable();
    void UpdateUnstable( unsigned int milliseconds );

    void InitQuickTurn();
    void UpdateQuickTurn( unsigned int milliseconds );

    void GetTargetPosition( rmt::Vector* position, bool withOffset = true );

    void CalculateRod( rmt::Vector* rod, 
                       unsigned int milliseconds, 
                       float timeMod );

    void CalculateTarget( rmt::Vector* desiredTarget, 
                          unsigned int milliseconds, 
                          float timeMod );

    bool GetDesiredRod( rmt::Vector* rod );

    bool IsPushingStick();

    //Prevent wasteful constructor creation.
    FollowCam( const FollowCam& followcam );
    FollowCam& operator=( const FollowCam& followcam );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// FollowCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================

inline const char* const FollowCam::GetName() const
{
    if ( mFollowType == FollowCam::FOLLOW_NEAR )
    {
        return "NEAR_FOLLOW_CAM";
    }
    else
    {
        return "FAR_FOLLOW_CAM";
    }
}

//=============================================================================
// unsigned int FollowCam::GetNumTargets const
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int FollowCam::GetNumTargets() const
{
    return mNumTargets;
}

//=============================================================================
// FollowCam::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
inline SuperCam::Type FollowCam::GetType()
{
    return static_cast<SuperCam::Type>(NEAR_FOLLOW_CAM + mFollowType);
}

//=============================================================================
// FollowCam::SetDirty
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void FollowCam::SetDirty()
{
    mData.SetDirty();
}

//=============================================================================
// FollowCam::SetCollisionOffset
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& offset, unsigned int numCollisions, const rmt::Vector& groundOffset )
//
// Return:      void 
//
//=============================================================================
inline void FollowCam::SetCollisionOffset( const rmt::Vector* offset, unsigned int numCollisions, const rmt::Vector& groundOffset )
{
    mCollisionOffset = offset;
    mNumCollisions = numCollisions;
    mGroundOffset = groundOffset;
}

//*****************************************************************************
//
// Inline Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// FollowCam::OnInit
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void FollowCam::OnInit()
{
    InitMyController();
    mUnstableDelayTimeLeft = 0;
}

#endif //FOLLOWCAM_H





















