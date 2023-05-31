#ifndef CHARACTERCONTROLLER_H_
#define CHARACTERCONTROLLER_H_

#include <p3d/refcounted.hpp>
#include <radmath/radmath.hpp>

//////// FORWARD DECLARATIONS /////////
class Character;
class MouthFlapper;
///////////////////////////////////////

class CharacterController
:
public tRefCounted
{
public:
    enum eIntention
	{
        NONE,
		LeftStickX,
		LeftStickY,
        DoAction,   // DUSIT [Nov 6,2002]: "Action" conflicts with class name
        Jump,
        Dash,
        Attack,
        DPadUp,
        DPadDown,
        DPadLeft,
        DPadRight,
#ifdef RAD_WIN32
        GetOutCar,
        MouseLookLeft,
        MouseLookRight,
#endif
        NUM_INPUTS, // "real" inputs (stuff with buttons) goes 
                    // before this, things that are only triggerd by AI go after
        Dodge,
        Cringe,
        TurnRight,
        TurnLeft,
        CelebrateSmall,
        CelebrateBig,
        WaveHello,
        WaveGoodbye
	};

    CharacterController( void );
    virtual ~CharacterController( void );

    virtual void Update( float timeins ) {};
	virtual void GetDirection(  rmt::Vector& outDirection ) = 0;
    virtual float GetValue( int buttonId ) const = 0;
    virtual bool IsButtonDown( int buttonId ) const = 0;
    virtual int  TimeSinceChange( int buttonId ) const { return IsButtonDown(buttonId) ? 0 : 5000; };
    
    Character* GetCharacter( ) const;
    virtual void SetCharacter( Character* pCharacter );

    virtual void SetIntention( eIntention intention )
    {
        if( mActive )
        {
            mIntention = intention;
        }
    }
    eIntention GetIntention( void ) const
    {
        return mIntention;
    }

    void PreserveIntention( eIntention intention )
    {
        mPreserveIntention = intention;
    }

    void ClearIntention( void )
    {
        mIntention = mPreserveIntention;
        mPreserveIntention = NONE;
    }

    void SetActive( bool active ) { mActive = active; }

protected:
    Character* mpCharacter;
    eIntention mIntention;
    eIntention mPreserveIntention;
    bool mActive;
};


////////////////////////////////////////////////////////////
// Dusit
////////////////////////////////////////////////////////////

class NPCController
:
public CharacterController
{
public: // METHODS
    NPCController( void );
    virtual ~NPCController( void );

    virtual void Update( float seconds );
    virtual float GetSpeedMps() const;
    virtual void SetSpeedMps( float fSpeedMps );
    virtual void SetDirection( const rmt::Vector& inDirection );
    virtual void GetDirection( rmt::Vector& outDirection );
    virtual float GetValue( int buttonId ) const;
    virtual bool IsButtonDown( int buttonId ) const;

    void SetCharacter( Character *pCharacter );

    enum State
    {   
        // We enumerate here, all the different states specific to 
        // NPC behavior controller... which are essentially substates 
        // of the character's locomotion state (in character's statemanager)

        FOLLOWING_PATH, // following predescribed path
        STOPPED,        // not moving, turns to track player, will resume following path
        DODGING,        // uh... dodging
        CRINGING,       // if we're not dodging, we're cringing...
        TALKING,        // when peds want to talk to one another!
        STANDING,       // just not moving, will resume following path
        PANICKING,      // running in random direction, more or less away from player 
        TALKING_WITH_PLAYER, // just for conversations..
        NONE,           // limbo... do nothing... stand there... don't resume following path
        NUM_STATES
    };
    virtual State GetState() const
    {
        return mState;
    }
    void TransitToState( State state );

    bool AddNPCWaypoint( const rmt::Vector& pt );
    void ClearNPCWaypoints();

    void StartTalking();
    void StopTalking();
    void IncitePanic();
    void QuellPanic();

    void TeleportToPath(void);
    void SetTempWaypont(const rmt::Vector&);
    void ClearTempWaypoint(void);

public: // MEMBERS

    enum { MAX_NPC_WAYPOINTS = 32 };

    bool mOffPath;
    MouthFlapper* mMouthFlapper;
    int mMillisecondsInTalk;
    NPCController* mTalkTarget;
    bool mListening; // if in talking state but mouth not flapping

protected: // METHODS

    virtual void OnOffPath( rmt::Vector lastPos, rmt::Vector currPos );
    virtual void GetAllowedPathOffset( rmt::Vector& offset );
    virtual float GetFollowPathSpeedMps() const;
    virtual void FollowPath( float seconds );
    virtual void OnReachedWaypoint();

    // detect collision with player vehicle and set dodge intention.
    // returns true if need to dodge, false otherwise...
    virtual void DetectAndDodge( float seconds );
    virtual void TraversePath( float seconds );

    bool Detect( float seconds, float& hisVel );
    void PerformDodge();
    void PerformCringe();

protected: // MEMBERS

    // This struct is filled in the Detect call before PerformDodge is called
    // and used from within PerformDodge to determine direction of dodge.
    struct DodgeInfo
    {
        bool wasSetThisFrame;
        rmt::Vector playerRightSide;
        bool myPosIsOnPlayersRightSide;
    };
    DodgeInfo mDodgeInfo; 

    State mState;
    float mSecondsInStopped;
    float mSpeedMps;
    rmt::Vector mNormalizedDodgeDir; // Normalized 

    rmt::Vector mNPCWaypoints[ MAX_NPC_WAYPOINTS ];
    int mNumNPCWaypoints;
    int mCurrNPCWaypoint;

    bool mStartPanicking;
    float mSecondsChangePanicDir;
    rmt::Vector mNormalizedPanicDir;

    float mSecondsSinceLastTurnAnim; // while in STOPPED state, we sometimes play turn animations 

    bool mUseTempWaypoint;
    rmt::Vector mTempWaypoint;

private: // METHODS

private: // MEMBERS
    rmt::Vector mDirection;
};

inline void NPCController::ClearNPCWaypoints()
{
    mNumNPCWaypoints = 0;
    mCurrNPCWaypoint = -1;
}

inline float NPCController::GetSpeedMps() const
{
    return mSpeedMps;
}
inline void NPCController::SetSpeedMps( float fSpeedMps )
{
    mSpeedMps = fSpeedMps;
}
inline void NPCController::SetDirection( const rmt::Vector& inDirection )
{
    mDirection = inDirection;
}
inline float NPCController::GetValue( int buttonId ) const
{
    return 0.0f;
}
inline bool NPCController::IsButtonDown( int buttonId ) const
{
    return false;
}

/////////////////////////////////////////////////////////////




class CharacterMappable;
class tCamera;

class PhysicalController
:
public CharacterController
{
public:
    PhysicalController( void );
    virtual ~PhysicalController( void );

    virtual void GetDirection( rmt::Vector& outDirection );
    virtual float GetValue( int buttonId ) const;
    virtual bool IsButtonDown( int buttonId ) const;
    virtual int  TimeSinceChange( int buttonId ) const;
    virtual void SetIntention( eIntention intention ) {};

    CharacterMappable* GetCharacterMappable( void ) const;
    void SetCharacterMappable( CharacterMappable* pMappable );
private:
    CharacterMappable* mpCharacterMappable;
};

//////////////////////////////////////////////////////////////////////////
//
//
//
//
// TBJ [8/9/2002] 
//
//////////////////////////////////////////////////////////////////////////

class CameraRelativeCharacterControllerEventHandler;

class CameraRelativeCharacterController
:
public PhysicalController
{
public:

	CameraRelativeCharacterController( void );
    virtual ~CameraRelativeCharacterController( void );

	void Create( Character* pCharacter, CharacterMappable* pCharacterMappable );
	void GetDirection( rmt::Vector& outDirection );
    virtual void SetIntention( eIntention intention );
    
    void SetCamera( tCamera* pCamera );


protected:
    friend class CameraRelativeCharacterControllerEventHandler; 
    void HandleEvent( int id, void* pEventData );
private: 

	// Don't much like this pointer here.
	//
	tCamera* mpCamera;
    CameraRelativeCharacterControllerEventHandler* mpEventHandler;
    bool mbCameraChange;
    rmt::Vector mLastDirection;
    rmt::Matrix mLastCameraMatrix;
};

#endif // CHARACTERCONTROLLER_H_
