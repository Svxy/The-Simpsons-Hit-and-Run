//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        supercamcentral.h
//
// Description: Blahblahblah
//
// History:     03/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef SUPERCAMCENTRAL_H
#define SUPERCAMCENTRAL_H

//========================================
// Nested Includes
//========================================
#include <camera/supercam.h>
#include <data/gamedata.h>
#include <events/eventlistener.h>

#include <ai/actor/intersectionlist.h>

#include <camera/followcamdatachunk.h>

//========================================
// Forward References
//========================================
class tCamera;
class tPointCamera;
class ISuperCamTarget;
class FrustrumDrawable;
class FOVLocator;
class SuperCamController;

namespace sim
{
    class CollisionObject;
    //class CollisionVolume;
    class SphereVolume;
    class ManualSimState;
};

extern SuperCam::Type CAMERAS_FOR_DRIVING[];
extern const int NUM_CAMERAS_FOR_DRIVING;
extern const int NUM_CAMERAS_FOR_DRIVING_WITHOUT_CHEAT;

extern SuperCam::Type CAMERAS_FOR_WALKING[];
extern const int NUM_CAMERAS_FOR_WALKING;
extern const int NUM_CAMERAS_FOR_WALKING_WITHOUT_CHEAT;

extern SuperCam::Type SUPER_SPRINT_CAMERAS[];
extern const int NUM_SUPERSPRINT_CAMS;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class SuperCamCentral : public EventListener,
                        public GameDataHandler
{
public:
    enum 
    { 
        MAX_CAMERAS = 32, 
#ifdef RAD_WIN32
        MAX_COLLISIONS = 15,
#else
        MAX_COLLISIONS = 10, 
#endif
        MAX_DATA_CHUNKS = 180 
    };
    static const char* CAMERA_INVENTORY_SECTION;

    SuperCamCentral();
    virtual ~SuperCamCentral();

    void Init( bool shutdown = false );
    bool IsInit() const;

    //Update the active super camera
    void Update( unsigned int milliseconds, bool isFirstSubstep = true );

    //Physics stuff
    void PreCollisionPrep();
    void AddCameraCollisionOffset(rmt::Vector& fixOffset);
    
    // submission to collision detection
    void SubmitStatics();

    void UpdateForPhysics( unsigned int milliseconds );

    //Adding and removing available cameras
    unsigned int    RegisterSuperCam( SuperCam* cam );
    void            UnregisterSuperCam( SuperCam* cam );
    void            UnregisterSuperCam( unsigned int which );
    
    //Accessing cameras
    unsigned int    GetNumRegisteredSuperCams() const;
    SuperCam*       GetSuperCam( unsigned int which ) const;
    SuperCam*       GetSuperCam( SuperCam::Type type ) const; // returns NULL if not found
    unsigned int    GetActiveSuperCamIndex() const;

    //Selecting which cam(s) to make active
    enum SelectFlag { CUT = 0x00000001, QUICK = 0x00000010, FORCE = 0x00000100, NO_TRANS = 0x10000000};
    bool            ToggleSuperCam( bool forwards, bool quickTransition = true );
    void            SelectSuperCam( unsigned int which, int flags = CUT | QUICK, unsigned int timems = 7000 );
    void            SelectSuperCam( SuperCam::Type type, int flags = CUT | QUICK, unsigned int timems = 7000 );
    void            SelectSuperCam( SuperCam* cam, int flags = CUT | QUICK, unsigned int timems = 7000 );
    SuperCam*       GetActiveSuperCam();

    //This defines what tPointCamera class is used with all the SuperCams
    void            SetCamera( tPointCamera* cam );
    tPointCamera* const  GetCamera();

    void                            SetTarget( ISuperCamTarget* target );
    void                            AddTarget( ISuperCamTarget* target );
    const ISuperCamTarget* const    GetTarget() const;

    //Debugging stuff
    void ToggleDebugView( bool on );
    void ToggleDebugView();

    bool AllowCameraToggle();
    bool AllowAutoCameraChange();

    void RegisterFOVLocator( FOVLocator* loc );
    void UnregisterFOVLocator();

    void DoCameraCut();
    void NoTransition();

    int mCollisionAreaIndex; // index into the collision area this instance will use.

    //FROM EventListener
    virtual void HandleEvent( EventEnum id, void* pEventData );

    void ToggleFirstPerson( int controllerID );

    bool IsCutCam();
    
    void SetIsInitialCamera(bool b) { mInitialCamera = b; }
    bool IsInitialCamera(void) { return mInitialCamera; }

    void EnableInvertedCamera( bool isEnabled );
    bool IsInvertedCameraEnabled() const;

    // Implements GameDataHandler
    //
    virtual void LoadData( const GameDataByte* dataBuffer, unsigned int numBytes );
    virtual void SaveData( GameDataByte* dataBuffer, unsigned int numBytes );
    virtual void ResetData();

    void EnableJumpCams( bool enable );
    bool JumpCamsEnabled() const;

    bool IsLegalType( SuperCam::Type type );

    IntersectionList& GetIntersectionList() { return mIntersectionList; };

    SuperCam::Type GetPreferredFollowCam(void) { return mPreferredFollowCam; }

    static FollowCamDataChunk& GetNewFollowCamDataChunk();
    static void ReleaseFollowCamDataChunk( FollowCamDataChunk& chunk );
    static FollowCamDataChunk* FindFCD( unsigned int id );
    static void CleanupDataChunks() { mNumUsedFDC = 0; };

    void NastyHypeCamHackEnable( bool enable ) { mNastyHypeCamHackEnabled = enable; };

private:
    void SetActiveSuperCam( unsigned int which = 0, int flags = CUT | QUICK, unsigned int timems = 7000 );

#ifdef DEBUGWATCH
    void DrawFrustrum();
#endif

    SuperCam* mSuperCameras[ MAX_CAMERAS ];
    SuperCam* mActiveSuperCam;
    unsigned int mActiveSuperCamIndex;
    unsigned int mNumRegisteredSuperCams;

    struct NextSuperCam
    {
        NextSuperCam() : nextSuperCam( 0 ), nextCamDelay( 0 ), flags( 0 ), timems( 0 ), incoming( false ) {};
        unsigned int nextSuperCam;  //This is for delaying transitions.
        int nextCamDelay;
        int flags;
        unsigned int timems;
        bool incoming;
    };

    NextSuperCam mNextSuperCam;

    //Things to share between SuperCams
    tPointCamera*       mCamera;
    ISuperCamTarget*    mTarget;

    static unsigned char    mTotalSuperCamCentrals;
    unsigned char           mMyNumber;

#ifdef SUPERCAM_DEBUG
    //Debugging stuff.
    bool            mDebugViewOn;
    tPointCamera*   mDebugCamera;    
    float mDebugXZAngle;
    float mDebugYAngle;
    float mDebugMagnitude;

    FrustrumDrawable* mFrustrumDrawable;
#endif

    FOVLocator* mCurrentFOVLocator;

    bool mDoCameraCut;

    unsigned int mChanceToBurnout;

    //Physics stuff
    sim::CollisionObject* mCameraCollisionObject;
    //sim::CollisionVolume* mCameraCollisionVolume;
    sim::SphereVolume* mCameraCollisionVolume;

    sim::ManualSimState* mCameraSimState;

    float mCameraCollisionFudge;
    rmt::Vector mCameraCollisionOffsetFix[ MAX_COLLISIONS ];
    unsigned int mCameraCollisionCount;
    
    rmt::Vector mCameraTerrainCollisionOffsetFix;
    
    void UpdateCameraCollisionSpherePosition(rmt::Vector& p);
    void UpdateCameraCollisionSphereRadius(float radius);

    SuperCamController* mController;
    int mControllerHandle;

    unsigned int mWrecklessCount;

    SuperCam::Type mPreferredFollowCam;

    IntersectionList mIntersectionList;

    bool mInitialCamera : 1;
    bool mIsInvertedCameraEnabled : 1;
    bool mJumpCamsEnabled : 1;
    bool mCameraToggling : 1;
    bool mNastyHypeCamHackEnabled : 1;

    static FollowCamDataChunk mFollowCamDataChunks[ MAX_DATA_CHUNKS ];
    static unsigned int mNumUsedFDC;

    //Prevent wasteful constructor creation.
    SuperCamCentral( const SuperCamCentral& supercamcentral );
    SuperCamCentral& operator=( const SuperCamCentral& supercamcentral );
};


//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// SuperCamCentral::GetActiveSuperCam
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      SuperCam
//
//=============================================================================
inline SuperCam* SuperCamCentral::GetActiveSuperCam()
{
    return mActiveSuperCam;
}

//=============================================================================
// SuperCamCentral::GetCamera
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      tCamera
//
//=============================================================================
inline tPointCamera* const SuperCamCentral::GetCamera()
{
    return mCamera;
}

//=============================================================================
// SuperCamCentral::GetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
inline const ISuperCamTarget* const SuperCamCentral::GetTarget() const
{
    return mTarget;
}

//=============================================================================
// SuperCamCentral::DoCameraCut
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void SuperCamCentral::DoCameraCut()
{
    mDoCameraCut = true;
}

//=============================================================================
// SuperCamCentral::EnableInvertedCamera
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool isEnabled )
//
// Return:      void 
//
//=============================================================================
inline void SuperCamCentral::EnableInvertedCamera( bool isEnabled )
{
    mIsInvertedCameraEnabled = isEnabled;
}

//=============================================================================
// SuperCamCentral::IsInvertedCameraEnabled
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool SuperCamCentral::IsInvertedCameraEnabled() const
{
    return mIsInvertedCameraEnabled;
}

//=============================================================================
// SuperCamCentral::EnableJumpCams
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool enable )
//
// Return:      void 
//
//=============================================================================
inline void SuperCamCentral::EnableJumpCams( bool enable )
{
    mJumpCamsEnabled = enable;
}

//=============================================================================
// SuperCamCentral::JumpCamsEnabled
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool SuperCamCentral::JumpCamsEnabled() const
{
    return mJumpCamsEnabled;
}



#endif //SUPERCAMCENTRAL_H
