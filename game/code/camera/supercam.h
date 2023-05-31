//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        supercam.h
//
// Description: This is the base class of all SuperCams
//
// History:     03/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef SUPERCAM_H
#define SUPERCAM_H

//Uncomment this for supercam debugging.
#ifndef RAD_RELEASE
#define SUPERCAM_DEBUG
#endif

//========================================
// Nested Includes
//========================================
//These are included in the precompiled headers on XBOX and GC
#include <raddebug.hpp>

#ifndef WORLD_BUILDER
//This is the non tool includes.
#include <p3d/refcounted.hpp>
#include <radmath/radmath.hpp>
#include <camera/SineCosShaker.h>

#include <debug/profiler.h>

#include <events/eventdata.h>
#else
//The tools need a different include style.
#include <p3d/refcounted.hpp>
#include <radmath/radmath.hpp>
#include "sinecosshaker.h"
#include "events/eventdata.h"
#endif



//========================================
// Forward References
//========================================

class tPointCamera;
class tCamera;
class SuperCamCentral;
class ISuperCamTarget;

class ICameraShaker;

class SuperCamController;

//========================================
// Constants
//========================================

#define CLAMP_TO_ONE(x) if ((x) > 1.0f) { (x)=1.0f; } else if ((x)< 0.0f) { (x)=0.0f; }

//=============================================================================
//
// Synopsis:    This is the base class of all SuperCams
//
//=============================================================================

class SuperCam : public tRefCounted
{
public:

    //These are the types of SuperCamera that are available.  When a new one is
    //created it is registered in this enum.  If this gets to be larger than 
    //32 types, we'll have to change some things.
    typedef enum Type
    {
        DEFAULT_CAM,
        FOLLOW_CAM,
        NEAR_FOLLOW_CAM,
        FAR_FOLLOW_CAM,
        BUMPER_CAM,
        CHASE_CAM,
        DEBUG_CAM,
        WRECKLESS_CAM,
        WALKER_CAM,
        COMEDY_CAM,
        KULL_CAM,
        TRACKER_CAM,
        SPLINE_CAM,     //Deprecated
        RAIL_CAM,
        CONVERSATION_CAM,
        STATIC_CAM,
        BURNOUT_CAM,
        REVERSE_CAM,
        SNAPSHOT_CAM,
        SURVEILLANCE_CAM,
        ANIMATED_CAM,
        RELATIVE_ANIMATED_CAM,
        SUPER_SPRINT_CAM,
        FIRST_PERSON_CAM,
#ifdef RAD_WIN32
        ON_FOOT_CAM,
        PC_CAM,
#endif

        //Add cameras before here!
        NUM_TYPES,
        INVALID = NUM_TYPES
    };
    
    SuperCam();
    virtual ~SuperCam();

    //Init...  This gets called when the camera is registered
    void Init();

    //Update: Called when you want the super cam to update its state.
    virtual void Update( unsigned int milliseconds ) = 0;
    virtual void UpdateForPhysics( unsigned int milliseconds ) {};

    //Returns the name of the super cam.  
    //This can be used in the FE or debug info
    virtual const char* const GetName() const = 0;

    virtual Type GetType() = 0;

    //This loads the off-line created settings for the camera.  
    //It is passed in as a byte stream of some data of known size.
    virtual void LoadSettings( unsigned char* settings ) {}; 

    //These are for favourable support of this command
    virtual void SetTarget( ISuperCamTarget* target ) {}; 
    virtual void AddTarget( ISuperCamTarget* target ) {};

    virtual unsigned int GetNumTargets() const { return 0; };

    //Override this if you want physics updates.
    virtual void SetCollisionOffset( const rmt::Vector* offset, unsigned int numCollisions, const rmt::Vector& groundOffset ) {};
    virtual float GetCollisionRadius() const { return 2.0f; };

    virtual float GetIntersectionRadius() const { return 1.0f; };

    //These functions are to allow real-time control of the settings of 
    //the supercam.
    void RegisterDebugControls();
    void UnregisterDebugControls();

    //Questions other objects can ask of the SuperCam;
    //Note: Due to the camera shaking, this is how all children of
    //SuperCam must access the mCamera values.
    void                GetPosition( rmt::Vector* position ) const;
    void                GetTarget( rmt::Vector* target ) const;
    void                GetHeading( rmt::Vector* heading ) const;
    void                GetHeadingNormalized( rmt::Vector* heading );
    void                GetVelocity( rmt::Vector* velocity ) const;
    void                GetCameraUp( rmt::Vector* up ) const;
    tPointCamera* const GetCamera() const;

    //These will override any chnages to the camera if they are set directly 
    //on the tCamera.  They are applied to the tCamera when the SuperCam is 
    //updated.
    void            SetFOV( float FOVinRadians );
    float           GetFOV() const; //Returns value in degrees
    void            SetAspect( float aspect );
    float           GetAspect() const;
    void            SetNearPlane( float nearPlane );
    float           GetNearPlane() const;
    void            SetFarPlane( float farPlane );
    float           GetFarPlane() const;

    virtual void EnableShake();
    virtual void DisableShake();

    virtual void LookBack( bool lookBack );

    virtual void DoFirstTime();
    virtual void DoCameraCut();
    virtual void DoCameraTransition( bool quick, unsigned int timems = 7000 );

    bool IsRegistered() const;

    void SetTransitionPositionRate( float rate );
    void SetTransitionTargetRate( float rate );

    //This is for debug rendering.
    void Display() const;

    static void SetSecondaryControllerID( unsigned int controllerID );

    void SetTwist( float twist );

    //For Wreckless Effect
    void EnableWrecklessZoom( bool enable );

    inline void AllowShake(void);

    #ifdef DEBUGWATCH
        virtual const char* GetWatcherName() const;
        void PrintClassName() const;
    #endif

protected:
    
    //Go ahead and make flags specific to the subclass, just make sure there's 
    //no overlap.
    enum Flag
    {
        FIRST_TIME,         //First time into this supercam (cleared after use)
        CUT,                //Do a camera cut to the default settings of this cam (cleared after use)
        START_TRANSITION,   //
        TRANSITION,         //This is a blended transition for target and position.
        END_TRANSITION,
        QUICK_TRANSITION,   //This does the cubic interpolation on both pos and targ.
        LOOK_BACK,          //Lookback requested (cleared after use)
        MULTIPLE_TARGET,    //There are multiple targets available
        START_SHAKE,        //This says we want to start shaking, (cleared after use)
        SHAKE,              //The camera is shaking!
        END_SHAKE,          //The shake is coming to an end. (cleared after use)
        OVERRIDE_FOV,       //This allows temporary overriding of FOV over time.
        IS_REGISTERED,
        DIRTY_HEADING,      //This is for the normalized heading calculations...
        EASE_IN,
        WRECKLESS_ZOOM,     //This is the wreckless-style zoom effect
        SUPERCAM_END
    };
    
    //Override this if you want to get initted.
    virtual void OnInit() {};
    virtual void OnShutdown() {};

    //You'll need to overload these if you want debug watcher or other debug controls.
    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();

    //Call this when you want to update the camera.  Sets VUP and test for 
    //bad camera settings.
    void SetCameraValues( unsigned int milliseconds,
                          rmt::Vector pos, 
                          rmt::Vector targ, 
                          const rmt::Vector* vup = 0 );

    void SetFOVOverride( float newFOV );
    void OverrideFOV( bool enable, float time = 0.0f, float rate = 0.0f );
    void DisableOverride();

    void SetRegistered( bool isRegistered );

    virtual bool CanSwitch();
    virtual float GetTargetSpeedModifier() { return 1.0f; };

    friend class SuperCamCentral;

#ifdef WORLD_BUILDER
    friend class GameEngine;
    friend class RailCamLocatorNode;
    friend class StaticCameraLocatorNode;
#endif
    
    void SetCamera( tPointCamera* cam );  
    inline void SetFlag( Flag flag, bool value );
    bool GetFlag( Flag flag ) const;
    void SetPlayerID( int id );

    //Motion Cubic
    void MotionCubic( float* Position, 
                      float* Rate, 
                      float DesiredPosition, 
                      float Interval ) const;

    void SphericalMotion( const rmt::Vector& target, 
                          rmt::Vector& currentPos, 
                          const rmt::Vector& desiredPos, 
                          rmt::Vector& desiredPosDelta, 
                          const float rate ) const;

    float EaseMotion( float t, float a, float b );

    //Camera shaking stuff
    inline void SetShaker( ICameraShaker* shaker );
    void SetCameraShakerData( const ShakeEventData* data );

    int GetPlayerID() const;

    int ClampAngle( float* angle ) const;
    void AdjustAngles( float* desiredAngle, 
                       float* currentAngle, 
                       float* currentAngleDelta ) const;

    rmt::Vector UpdateVUP( rmt::Vector position, rmt::Vector target );

    void Shutdown();
    void CorrectDist( const rmt::Vector& pos, rmt::Vector& targ );

    //For display in the children.
    virtual void OnDisplay() const {};

    void InitMyController( int controllerID = -1 );
    void ShutDownMyController();

    tPointCamera* GetCameraNonConst();

    void FilterFov( float zoom, 
                    float min, 
                    float max, 
                    float& curFOV, 
                    float& delta, 
                    float lag, 
                    float timeMod, 
                    float offset = 0.0f );

    void ResetTwistDelta() { mTwistDelta = 0.0f; };

    void DoWrecklessZoom( float distance, float minDist, float maxDist, 
                          float minFOV, float maxFOV, float& fov, 
                          float& fovDelta, float lag, float time );

    void EndTransition( bool abort = false ); //Use caution.


    //Tell me when we're first running.
    unsigned int mFlags;

    //These are shared, default shakers.
    SineCosShaker mSineCosShaker;

    SuperCamController* mController;
    int mControllerHandle;

    static unsigned int s_secondaryControllerID;

    tPointCamera* mCamera;
private:
    float mSCFOV;     //This is in RADIANS
    float mSCAspect;
    float mSCNearPlane;
    float mSCFarPlane;

    //We do some fakery to the camera to hide the camera shaking it.
    //tPointCamera* mCamera;
    rmt::Vector mVelocity;

    //This is the position that the camera thinks it's in, while shaking.
    rmt::Vector mCameraVirtualPosition;
    rmt::Vector mCameraVirtualTarget;
    float mCameraVirtualFOV;

    //These are used with the virtual position to do transitions.
    rmt::Vector mCameraVirtualPositionDelta;
    rmt::Vector mCameraVirtualTargetDelta;
    float mCameraVirtualFOVDelta;
    unsigned int mTransitionTime;
    unsigned int mTransitionTimeLimit;
    float mTransitionPositionRate;
    float mTransitionTargetRate;
    float mFOVTransitionRate;

    ICameraShaker* mShaker;

    int mPlayerID;

    float mSCBackupFOV;

    rmt::Vector mNormalizedHeading;

    float mSCTwist;
    float mOldTwist;
    float mTwistDelta;
    float mTwistLag;

    void TestCameraMatrix();
    void SetupShake();
    void EndShake();
    void SetupTransition( bool swap = false );
    void ShakeCamera( rmt::Vector* pos, rmt::Vector* targ, unsigned int milliseconds );
    void TransitionCamera( float timeLeftPct, rmt::Vector* pos, rmt::Vector* targ, float* fov, unsigned int milliseconds, bool quick = false );
    void TransitionFOV( float timeLeftPct, float* fov, unsigned int milliseconds );
    void EaseIn( float timeLeftPct, rmt::Vector* pos, rmt::Vector* targ, unsigned int milliseconds );

    //Prevent wasteful constructor creation.
    SuperCam( const SuperCam& supercam ); 
    SuperCam& operator=( const SuperCam& supercam );

};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// SuperCam::Init
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::Init()
{
    const int NO_CLEAR_FLAGS = ( 1 << IS_REGISTERED );
    mFlags &= NO_CLEAR_FLAGS;

    OnInit();
}

//=============================================================================
// SuperCam::SetTwist
//=============================================================================
// Description: Comment
//
// Parameters:  ( float twist )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::SetTwist( float twist )
{
    mSCTwist = twist;
}

//*****************************************************************************
//
// Inline Protected Member Functions
//
//*****************************************************************************
//=============================================================================
// SuperCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::OnRegisterDebugControls()
{
}

//=============================================================================
// SuperCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::OnUnregisterDebugControls()
{
}
//=============================================================================
// SuperCam::SetFlag
//=============================================================================
// Description: Comment
//
// Parameters:  ( Flag flag, bool value )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::SetFlag( Flag flag, bool value )
{
    if ( value )
    {   
        mFlags |= ( 1 << flag );
    }
    else
    {
        mFlags &= ~( 1 << flag );
    }    
}
//=============================================================================
// SuperCam::SetRegistered
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool isRegistered )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::SetRegistered( bool isRegistered )
{
    SetFlag( (Flag)IS_REGISTERED, isRegistered );
}

//=============================================================================
// SuperCam::CanSwitch
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool SuperCam::CanSwitch()
{
    return true;
}
//=============================================================================
// SuperCam::GetFlag
//=============================================================================
// Description: Comment
//
// Parameters:  ( Flag flag )
//
// Return:      bool 
//
//=============================================================================
inline bool SuperCam::GetFlag( Flag flag ) const
{
    return ( (mFlags & ( 1 << flag )) > 0 );
}

//=============================================================================
// SuperCam::MotionCubic
//=============================================================================
// Description: Comment
//
// Parameters:  ( float* Position, 
//                float* Rate, 
//                float DesiredPosition, 
//                float Interval)
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::MotionCubic( float* Position, 
                                   float* Rate, 
                                   float DesiredPosition, 
                                   float Interval ) const
{
    //Note: If you are using this function and the camera shake, make
    //sure to reset the rate to the position after each camera shake.
    //Otherwise you'll get funny interpolation.
#ifdef PROFILER_ENABLED
        char sCname[256];
        sprintf( sCname, "SC: %d MotionCubic", mPlayerID );
        BEGIN_PROFILE( sCname )
#endif

    float  a, b, c, d, Interval_Cube, Interval_Square;

    Interval_Square = Interval * Interval;
    Interval_Cube   = Interval_Square * Interval;

    a = *Rate + ((*Position - DesiredPosition) * 2.0f );
    b = ((DesiredPosition - *Position) * 3.0f) - (*Rate * 2.0f);
    c = *Rate;
    d = *Position;

    *Position = (a * Interval_Cube) + (b * Interval_Square) + (c * Interval) + d;
    *Rate = (a * Interval_Square * 3.0f) + (b * Interval * 2.0f) + c;    

#ifdef PROFILER_ENABLED
        END_PROFILE( sCname )
#endif

}

//=============================================================================
// SuperCam::SetShaker
//=============================================================================
// Description: Comment
//
// Parameters:  ( ICameraShaker* shaker )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::SetShaker( ICameraShaker* shaker )
{
    mShaker = shaker;
}

inline void SuperCam::AllowShake(void) { SetShaker(&mSineCosShaker); }

//=============================================================================
// SuperCam::FilterFov
//=============================================================================
// Description: Comment
//
// Parameters:  ( float zoom, float min, float max, float& curFOV, float& delta, float lag, float timeMod, float offset )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::FilterFov( float zoom, float min, float max, float& curFOV, float& delta, float lag, float timeMod, float offset )
{
    float diffFOV = max - min;

    //The closer we get to the max speed, the wider the FOV.
    float desiredFOV = max - (diffFOV * zoom);
    desiredFOV -= offset;
    if ( desiredFOV < min )
    {
        desiredFOV = min;
    }

    float fovLag = lag * timeMod;
    CLAMP_TO_ONE(fovLag);

    MotionCubic( &curFOV, &delta, desiredFOV, fovLag );
}

//=============================================================================
// SuperCam::DoWrecklessZoom
//=============================================================================
// Description: Comment
//
// Parameters:  ( float distance, float minDist, float maxDist, float minFOV, 
//                float maxFOV, float& fov, float& fovDelta, float lag, 
//                float time )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::DoWrecklessZoom( float distance, float minDist, float maxDist, 
                                       float minFOV, float maxFOV, float& fov, 
                                       float& fovDelta, float lag, float time )
{
    float percent = (distance - minDist) / (maxDist - minDist);
    CLAMP_TO_ONE( percent );

    float desiredFOV = maxFOV - ((maxFOV - minFOV) * percent);

    float fovlag = lag * time;
    CLAMP_TO_ONE(fovlag);

    MotionCubic( &fov, &fovDelta, desiredFOV, fovlag );
}

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// SuperCam::GetVelocity
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* velocity )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::GetVelocity( rmt::Vector* velocity ) const
{
    *velocity = mVelocity;
}

//=============================================================================
// SuperCam::GetCamera
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      tPointCamera*
//
//=============================================================================
inline tPointCamera* const SuperCam::GetCamera() const
{
    return mCamera;
}

//=============================================================================
// SuperCam::SetAspect
//=============================================================================
// Description: Comment
//
// Parameters:  ( float aspect )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::SetAspect( float aspect )
{
    mSCAspect = aspect;
}

//=============================================================================
// SuperCam::GetAspect
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float SuperCam::GetAspect() const
{
    return mSCAspect;
}

//=============================================================================
// SuperCam::SetNearPlane
//=============================================================================
// Description: Comment
//
// Parameters:  ( float nearPlane )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::SetNearPlane( float nearPlane )
{
    mSCNearPlane = nearPlane;
}

//=============================================================================
// SuperCam::GetNearPlane
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float SuperCam::GetNearPlane() const
{
    return mSCNearPlane;
}

//=============================================================================
// SuperCam::SetFarPlane
//=============================================================================
// Description: Comment
//
// Parameters:  ( float farPlane )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::SetFarPlane( float farPlane )
{
    mSCFarPlane = farPlane;
}

//=============================================================================
// SuperCam::GetFarPlane
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
inline float SuperCam::GetFarPlane() const
{
    return mSCFarPlane;
}

//=============================================================================
// SuperCam::EnableShake
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::EnableShake()
{
    if ( !GetFlag( (Flag)SHAKE ) && 
         !GetFlag( (Flag)TRANSITION ) &&
         !GetFlag( (Flag)START_TRANSITION ) &&
         !GetFlag( (Flag)END_TRANSITION ) )
    {
        SetFlag( (Flag)START_SHAKE, true );
    }
}

//=============================================================================
// SuperCam::EnableShake
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool enable )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::DisableShake()
{
    if ( GetFlag( (Flag)SHAKE ) )
    {
        SetFlag( (Flag)END_SHAKE, true );
    }
}

//=============================================================================
// SuperCam::IsRegistered
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool SuperCam::IsRegistered() const
{
    return GetFlag( (Flag)IS_REGISTERED );
}

//=============================================================================
// SuperCam::SetTransitionPositionRate
//=============================================================================
// Description: Comment
//
// Parameters:  ( float rate )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::SetTransitionPositionRate( float rate )
{
    mTransitionPositionRate = rate;
}

//=============================================================================
// SuperCam::SetTransitionTargetRate
//=============================================================================
// Description: Comment
//
// Parameters:  ( float rate )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::SetTransitionTargetRate( float rate )
{
    mTransitionTargetRate = rate;
}

//=============================================================================
// SuperCam::SetPlayerID
//=============================================================================
// Description: Comment
//
// Parameters:  ( int id )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::SetPlayerID( int id )
{
    //rAssertMsg( mPlayerID < 0, "This should only be set once!" );
    mPlayerID = id;
}

//=============================================================================
// SuperCam::GetPlayerID
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline int SuperCam::GetPlayerID() const
{
    rAssertMsg( mPlayerID >= 0, "This should be set once!" );
    return mPlayerID;
}

//=============================================================================
// SuperCam::LookBack
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool lookBack )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::LookBack( bool lookBack )
{
    SetFlag( (Flag)LOOK_BACK, lookBack );
}

//=============================================================================
// SuperCam::DoFirstTime
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::DoFirstTime()
{
    SetFlag( (Flag)FIRST_TIME, true );
}

//=============================================================================
// SuperCam::DoCameraCut
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::DoCameraCut()
{
    SetFlag( (Flag)CUT, true );
}

//=============================================================================
// SuperCam::SetSecondaryControllerID
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::SetSecondaryControllerID( unsigned int controllerID )
{
    s_secondaryControllerID = controllerID;
}

//=============================================================================
// SuperCam::EnableWrecklessZoom
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool enable )
//
// Return:      void 
//
//=============================================================================
inline void SuperCam::EnableWrecklessZoom( bool enable )
{
    SetFlag( WRECKLESS_ZOOM, enable );    
}

#endif //SUPERCAM_H
