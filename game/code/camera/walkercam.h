//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        walkercam.h
//
// Description: Blahblahblah
//
// History:     25/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef WALKERCAM_H
#define WALKERCAM_H

//========================================
// Nested Includes
//========================================
#include <camera/supercam.h>
#include <camera/walkercamdata.h>

//========================================
// Forward References
//========================================
class ISuperCamTarget;
class Character;
class AnimatedIcon;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class WalkerCam : public SuperCam
{
public:
    enum 
    { 
        MAX_TARGETS = 3,
        IN_COLLISION = SUPERCAM_END
    };

    WalkerCam();
    virtual ~WalkerCam();

    //Update: Called when you want the super cam to update its state.
    void Update( unsigned int milliseconds );
    void UpdateForPhysics( unsigned int milliseconds );

    //Returns the name of the super cam.  
    //This can be used in the FE or debug info
    virtual const char* const GetName() const;

    virtual Type GetType();

    //This loads the off-line created settings for the camera.  
    //It is passed in as a byte stream of some data of known size.
    void LoadSettings( unsigned char* settings ); 

    //These are for favourable support of this command
    void SetTarget( ISuperCamTarget* target ); 
    void AddTarget( ISuperCamTarget* target );

    unsigned int GetNumTargets() const;

    void SetCollisionOffset( const rmt::Vector* offset, unsigned int numCollisions, const rmt::Vector& groundOffset );
    float GetCollisionRadius() const { return GetNearPlane(); };

    virtual float GetIntersectionRadius() const { return mMagnitude; };

    void SetCollectibleLocation( AnimatedIcon* icon ) { mCollectible = icon; };

    #ifdef DEBUGWATCH
        virtual const char* GetWatcherName() const;
    #endif

protected:
    void OnDisplay() const;
    virtual void OnInit() { InitMyController(); };

private:
    ISuperCamTarget* mTarget;

    WalkerCamData mData;

    float mFOVDelta;
    float mMagnitude;
    float mMagnitudeDelta;
    float mElevation;
    float mElevationDelta;

    rmt::Vector mTargetPos;
    rmt::Vector mTargetPosDelta;

    rmt::Vector mPosition;
    rmt::Vector mDesiredPositionDelta;

    bool mIsAirborn;
    int mLandingCountdown;

    float mCameraHeight;

    const rmt::Vector* mCollisionOffset;
    unsigned int mNumCollisions;
    rmt::Vector mGroundOffset;

    unsigned int mLastCollisionFrame;

    rmt::Vector mPlaneIntersectPoint;

    tUID mLastCharacter;
    int mPedTimeAccum;

    float mXAxis;

    rmt::Vector mOldPos;
    float mOldMagnitude;

    AnimatedIcon* mCollectible;

    void UpdatePositionNormal( unsigned int milliseconds, float timeMod );
    void UpdatePositionInCollision( unsigned int milliseconds, float timeMod );
    bool UpdatePositionOneCollsion( unsigned int milliseconds, float timeMod, unsigned int collisionIndex = 0 );
    void UpdatePositionMultipleCollision( unsigned int milliseconds, float timeMod );

    //These functions are to allow real-time control of the settings of 
    //the supercam.
    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();

    void GetTargetPosition( rmt::Vector* position, bool withOffset = true ) const;

    float IsTargetNearPed( unsigned int milliseconds );

    bool IsPushingStick();
    bool IsStickStill();
    bool PushingTheWrongWay();
    
    //Prevent wasteful constructor creation.
    WalkerCam( const WalkerCam& walkercam );
    WalkerCam& operator=( const WalkerCam& walkercam );
};

//Subclass, just ignore this.
class ComedyCam : public WalkerCam
{
public:
    ComedyCam(){};
    virtual ~ComedyCam() {};

    virtual const char* const GetName() const { return "COMEDY_CAM"; };
    virtual Type GetType() { return SuperCam::COMEDY_CAM; };
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// WalkerCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================
inline const char* const WalkerCam::GetName() const
{
   return "WALKER_CAM";
}

//=============================================================================
// WalkerCam::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
inline SuperCam::Type WalkerCam::GetType()
{
    return WALKER_CAM;
}

//=============================================================================
// WalkerCam::GetNumTargets
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int WalkerCam::GetNumTargets() const
{
    return (mTarget != NULL) ? 1 : 0;
}

//=============================================================================
// WalkerCam::SetCollisionOffset
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& offset, unsigned int numCollisions, const rmt::Vector& groundOffset )
//
// Return:      void 
//
//=============================================================================
inline void WalkerCam::SetCollisionOffset( const rmt::Vector* offset, unsigned int numCollisions, const rmt::Vector& groundOffset )
{
    mCollisionOffset = offset;
    mNumCollisions = numCollisions;
    mGroundOffset = groundOffset;
}


#endif //WALKERCAM_H
