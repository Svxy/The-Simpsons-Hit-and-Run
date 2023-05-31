#ifndef CHARACTER_H_
#define CHARACTER_H_

#include <radmath/radmath.hpp>
#include <radtime.hpp>

#include <render/DSG/InstDynaPhysDSG.h>
#include <ai/state.h>
#include <ai/statemanager.h>
//#include <ai/sequencer/action.h>
#include <memory/memorypool.h>
#include <p3d/memory.hpp>

#include <ai/sequencer/task.h>
#include <p3d/p3dtypes.hpp>
#include <events/eventenum.h>
#include <events/eventmanager.h>
#include <radmath/radmath.hpp>
#include <worldsim/redbrick/vehicle.h>

#include <worldsim/character/charactercontroller.h>

#include <choreo/puppet.hpp>
#include <simcollision/proximitydetection.hpp>

class tDrawablePose;
class tCamera;
class CharacterTarget;
class ActionController;

namespace ActionButton
{
    class ButtonHandler;
	class AttachProp;
}
class Vehicle;

namespace CharacterAi
{
    class StateManager;
};

namespace sim
{
    class CollisionVolume;
};

namespace poser
{
    class Joint;
    class Transform;
};

struct CharacterTune
{
    static float sfLocoRotateRate;
    static float sfLocoAcceleration;
    static float sfLocoDecceleration;
    static bool bLocoTest;
    static float sfAirRotateRate;
    static float sfAirAccelScale;
    static float sfAirGravity;  
    static float sfStompGravityScale;  
    static float sfDashBurstMax;
    static float sfDashAcceleration;
    static float sfDashDeceleration;
    static float sfJumpHeight;
    static float sfDoubleJumpHeight;
    static float sfDoubleJumpAllowUp;
    static float sfDoubleJumpAllowDown;
    static float sfHighJumpHeight;
    static float sfMaxSpeed;

    static rmt::Vector sGetInPosition;
    static float sGetInHeightThreshold;
    static float sGetInOpenDelay;
    static float sGetInOpenSpeed;
    static float sGetInCloseDelay;
    static float sGetInCloseSpeed;
    static float sGetOutOpenDelay;
    static float sGetOutOpenSpeed;
    static float sGetOutCloseDelay;
    static float sGetOutCloseSpeed;

    static float sfTurboRotateRate;

    static float sfGetInOutOfCarAnimSpeed;
    static float sfKickingForce;
    static float sfSlamForce;
    static float sfShockTime;
};
class tPose;
class tSkeleton;
class InstDynaPhysDSG;
class WorldScene;
class AmbientDialogueTrigger;
class CharacterRenderable;
class JumpAction;
class WalkerLocomotionAction;

class Character
:
public DynaPhysDSG
{
public:
    enum Role {
        ROLE_UNKNOWN,
        ROLE_DRIVER,
        ROLE_REWARD,
        ROLE_ACTIVE_BONUS,
        ROLE_COMPLETED_BONUS,
        ROLE_PEDESTRIAN,
        ROLE_MISSION
    };

public: // MEMBERS
	Character( void );
	virtual ~Character( void );
	void Init( void );
    
    virtual void OnTransitToAICtrl();
    virtual int GetAIRef() { return PhysicsAIRef::PlayerCharacter;}
    
	float GetFacingDir( void ) const;
    void SetFacingDir( float fFacingDir );

    float GetDesiredDir( void ) const;
    void GetDesiredFacing( rmt::Vector& facing ) const; 
    //ICameraTarget.
    //
	void GetFacing( rmt::Vector& facing ) const;
    void SetFacing( rmt::Vector& facing );

    // Call this to place character at a particular place in the world
    // and perhaps reset its states.
    void RelocateAndReset( const rmt::Vector& position, float facing, bool resetMe=true, bool snapToGround = true );

	void SetPosition( const rmt::Vector& position );
	void GetPosition( rmt::Vector& position ) const;

	void SetVelocity( rmt::Vector& velocity );
	void GetVelocity( rmt::Vector& velocity ) const;
	const rmt::Vector& GetLocoVelocity() const;

	float GetSpeed( void ) const;
    void SetSpeed( float fSpeed );
    void ResetSpeed( void );
	float GetDesiredSpeed( void ) const;


    void StickReleased( void );
    void StickPressed( void );

    void SetPuppet( choreo::Puppet* pPuppet );
    choreo::Puppet* GetPuppet( void ) const;

    void SetDrawable( CharacterRenderable* pDrawablePose );
    
    virtual void PreSimUpdate(float timeins);
    virtual void UpdateRoot( float timeins );
    virtual void ResolveCollisions(void);
    virtual void PostSimUpdate(float timeins);

    // Just use this to set a flag.
    virtual void Update( float timeins ) {};
    
    void UpdateSimState( float timeins );
    void UpdateBBox( rmt::Box3D& oldBox );
    
    bool IsInCar( void ) const;
    void SetInCar( bool bInCar );
    bool IsInCarOrGettingInOut( void );

    CharacterTarget* GetTarget( void ) const;
    
    void SetController( CharacterController* pController );
    CharacterController* GetController( void ) const;
    
    void SetDesiredDir( float fDesiredDir );
    void SetDesiredSpeed( float fDesiredSpeed );

    ActionButton::ButtonHandler* GetActionButtonHandler( void ) const;
    void AddActionButtonHandler( ActionButton::ButtonHandler* pActionButtonHandler );
    void RemoveActionButtonHandler( ActionButton::ButtonHandler* pActionButtonHandler );
    void ClearAllActionButtonHandlers();
    
    void SetTargetVehicle( Vehicle* pVehicle );
    Vehicle* GetTargetVehicle( ) const;
    float GetMaxSpeed( void ) const;

    CharacterAi::StateManager* GetStateManager( void ) const;

    ActionController* GetActionController( void ) const;

    static float GetJumpHeight( void );
    void GetTerrainIntersect( rmt::Vector& pos, rmt::Vector& normal ) const;
    void GetTerrainType( eTerrainType& TerrainType, bool& Interior ) const;

    virtual void SubmitStatics( void );
    virtual void SubmitAnimCollisions( void );
    virtual void SubmitDynamics( void );
    
    // Implements CollisionEntityDSG
    //
    virtual void Display(void);
    virtual rmt::Vector*       pPosition() ;
    virtual const rmt::Vector& rPosition() ;
    virtual void GetPosition( rmt::Vector* ipPosn ) ;
 
    void DisplayShadow();
	void DisplaySimpleShadow( void );
    int  CastsShadow();
	bool IsSimpleShadow( void );
	void SetSimpleShadow( bool IsSimpleShadow );

    virtual sim::Solving_Answer PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision );
    virtual sim::Solving_Answer PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision);

   
    bool IsInCollision( void ) const;
    void ResetCollisions( void );
    bool CanStandOnCollisionVolume( void ) const;
    bool CanStaggerCollision( void ) const;

    enum eCollisionType
    {
        NoCollision = 0,
        HitWall = 1 << 0,
        HitHead = 1 << 1,
        DangerousMultiple = 1 << 3
    };

    eCollisionType SolveCollisionWithStatic( const rmt::Vector& desiredPos, rmt::Vector& outPos );
    
    bool IsStanding( void ) const;
    void SetGroundPoint( const rmt::Vector& groundPoint );

    WalkerLocomotionAction* GetWalkerLocomotionAction( void ) const;
    JumpAction* GetJumpLocomotionAction( void ) const;
    
    void UpdateParentTransform( float timeins );
    void GetRootTransform( poser::Transform& out ) const;
    void SetStandingJoint( const poser::Joint* pJoint );
    const rmt::Matrix& GetParentTransform( void ) const;
    const rmt::Matrix& GetInverseParentTransform( void ) const;
    float GetGroundVerticalVelocity( void ) const;

    void AddToWorldScene( void );
    void RemoveFromWorldScene( void );
    void MoveInWorldScene( void );
    virtual void InitGroundPlane( void );
    virtual void AddToPhysics( void );
    virtual void RemoveFromPhysics( void );

    int GetCollisionAreaIndex( void ) const;

    rmt::Vector WorldToLocal( const rmt::Vector& world ) const;
    rmt::Vector LocalToWorld( const rmt::Vector& local ) const;
    void UpdateTransformToLoco( void );
    void UpdateTransformToInCar( void );

    bool IsJumping( void ) const;
    void SetJumping( bool bIsJumping );

    void SetTurbo( bool bTurbo ); 
    bool IsTurbo( void ) const;

    bool IsVisible( void ) const { return mVisible;}

    void SetSolveCollisions( bool bCollide )
    {
        mbSolveCollisions = bCollide;
    }

    bool GetSolveCollisions( void ) const
    {
        return mbSolveCollisions;
    }

	struct Prop
	{
		Prop( void );
		~Prop( void );
		InstDynaPhysDSG* mpProp;
		tPose* mpPose;
		static tSkeleton* spSkeleton;
        static int sSkelRefs;
		static int mPropCount;
	};
	void TouchProp( InstDynaPhysDSG* pProp );
	void AttachProp( InstDynaPhysDSG* pProp );
	void RemoveProp( InstDynaPhysDSG* pProp );
	void UpdateProps( float timeins );
    
    virtual void UpdatePhysicsObjects( float timeins, int area );
    bool CanPlayAnimation( const tName& name ) const;
    #ifdef RAD_DEBUG
        void PrintAnimations() const;
    #else
        void PrintAnimations() const{};
    #endif

    void SetSwatch( int swatchNum );
    bool PosInFrustrumOfPlayer( const rmt::Vector& pos, int playerID );

    void SetYAdjust( float yOffset );
    float GetYAdjust();

    void Kick();
    void Slam();

    void SetFadeAlpha( int fadeAlpha );

    bool IsBusy(void) {return mbBusy;}
    void SetBusy(bool b) {mbBusy = b;}

    bool IsSimpleLoco(void) {return mbSimpleLoco;}
    void SetSimpleLoco(bool b) {mbSimpleLoco = b;}

    void SetShadowColour( tColour shadowColour );
    tColour GetShadowColour();

    void Shock( float timeInSeconds );

    void DoKickwave(void);

    // Don't apply any velocity change to the player character
    // There appear to be bugs when this happens ( character getting stuck in the ground )
    virtual void ApplyForce( const rmt::Vector& direction, float force ){}

    void SetAmbient(const char* location, float radius);
    bool IsAmbient(void) { return mAmbient;}
    void EnableAmbientDialogue(bool);
    void ResetAmbientPosition(void);

    bool IsNPC();

    enum { MAX_ACTION_BUTTON_HANDLERS = 5 };

    void DoGroundIntersect(bool b) { mbDoGroundIntersect = b;}

    bool GetRockinIdle(void) { return mAllowRockin; }
    void SetRockinIdle(bool b) { mAllowRockin = b; }

    bool HasBeenHit() { return mHasBeenHit; }
    void SetHasBeenHit( bool tf ) { mHasBeenHit = tf; }

    void SetRole(Role r) { mRole = r;}
    Role GetRole(void)   { return mRole;}

    void SetScale(float f) { mScale = f;}

    bool CollidedThisFrame(void) { return mCollidedThisFrame; }

    bool IsInSubstep() { return mIsInSubstep; }
    void SetInSubstep( bool in ) { mIsInSubstep = in; }

    bool TestInAnyonesFrustrum();

    const rmt::Vector& GetLean(void) { return mLean; }

    bool IsLisa(void) { return mIsLisa; }
    bool IsMarge(void) { return mIsMarge; }
    void SetIsLisa(bool b) { mIsLisa = b; }
    void SetIsMarge(bool b) { mIsMarge = b; }

    void SnapToGround(void);

    unsigned GetActiveFrame(void) { return mIntersectFrame; }

    void SetManaged(bool b) { mManaged = b;}
    bool IsManaged(void) { return mManaged; }

public: // MEMBERS
    bool mbCollidedWithVehicle;
    bool mbInAnyonesFrustrum; //if we have array of MAX_PLAYERS, we can distinguish betw each player's frustrum
    bool mbSurfing;
    bool mbAllowUnload;
    bool mbIsPlayingIdleAnim; // shuffling feet, scratching bum, etc.
#ifdef RAD_WIN32
    int mPCCamFacing; // 0 = cam direction, 1 = cam's right, 2 = facing cam, 3 = cam's left
#endif


    rmt::Matrix mPrevSimTransform;

protected: // METHODS
    void TestInFrustrumOfPlayer( int playerID );

    void UpdateGroundHeight(void);

    void SetParentTransform( const rmt::Matrix& mat, float timeins = 0.0f );
    sim::CollisionVolume* FindStandingVolume( const rmt::Vector& inPos, sim::CollisionVolume* inVolume, rmt::Vector& outNormal, float& outDist );
    bool GetCollisionHeight( const rmt::Vector& prevPosition, const rmt::Vector& position, rmt::Vector& outPosition, rmt::Vector& collisionNormal ) ;
    bool CanStandOnCollisionNormal( const rmt::Vector& normal ) const;
    bool CanStaggerCollisionNormal( const rmt::Vector& normal ) const;
    
    void UpdateController( rmt::Vector& direction, float timeins );
    virtual void UpdateDesiredDirAndSpeed( const rmt::Vector& dir );
    virtual void UpdateFootPlant( void );
    void UpdatePuppet( float timeins );
    void UpdateGroundPlane( float timeins );
    // Is the character allowed to move?
    bool IsMovementLocked()                 { return m_IsBeingShocked; }


    virtual void AssignCollisionAreaIndex( void );
    virtual float GetInputScale( void )
    {   
        // Number arrived at via experimentation.
        //
        return 0.69f;
    }

protected: // MEMBERS
    bool mIsNPC;

    // ground plane stuff for player character only
    sim::ManualSimState* mGroundPlaneSimState; 
    sim::WallVolume* mGroundPlaneWallVolume;

    int mCollisionAreaIndex;

    static sim::TArray<sim::RayIntersectionInfo> msIntersectInfo;

    radTime64 mLastInteriorLoadCheck;

private: // METHODS
    virtual void OnUpdateRoot( float timeins );
    virtual void OnPostSimUpdate(float timeins);
    void UpdateShock( float timeins );
    ActionButton::ButtonHandler* TestPriority( ActionButton::ButtonHandler* bA, ActionButton::ButtonHandler* bB );


private: // MEMBERS
	// The abstract controller object.
	//
	CharacterController* mpController;

    // A pointer to the renderable object.
    //
    CharacterRenderable* mpCharacterRenderable;

    // A pointer to the choreo::Puppet
    //
    choreo::Puppet* mpPuppet;

    // The facing angle, in radians. 0 = ( 0, 0, -1 )
    //
	float mfFacingDir;
	
    // The desired facing angle.  The character will attempt to converge on this angle.
    //
    float mfDesiredDir;
	
    // Scalar velocity along the facing angle.
    //
    float mfSpeed;

    rmt::Vector mVelocity;
	
    // The desired speed.  The character will accel or decel towards this speed.
    //
    float mfDesiredSpeed;

    // Is in car.
    //
    bool mbInCar;

    // Whether the characters feet were planted last update
    //
    std::vector< bool, s2alloc<bool> > mbWasFootPlanted;

    // For camera tracking.
    //
    CharacterTarget* mpCharacterTarget;

    // For ai and animation.
    //
    ActionController* mpActionController;


    //To control action button priority, we do the following.
    ActionButton::ButtonHandler* mpActionButtonHandlers[ MAX_ACTION_BUTTON_HANDLERS ];
    ActionButton::ButtonHandler* mpCurrentActionButtonHandler;

    Vehicle* mpTargetVehicle;
    
    static float sfMaxSpeed;

    // Terrain positioning.
    //
    float mGroundY;
    rmt::Vector mGroundNormal;
    eTerrainType mTerrainType; // What type of terrain is the character on.
    bool mInteriorTerrain; // The terrain is covered/inside. Such as a tunnel or building interior.

    rmt::Vector mRealGroundPos; // ground, excluding statics we may be on.
    rmt::Vector mRealGroundNormal; // ground, excluding statics we may be on.

    CharacterAi::StateManager* mpStateManager;

    float mfRadius;

// Hack.
    friend class CharacterRenderable;
    // Collision detection and response.
    //
    bool mbCollided;
    int mCurrentCollision;
    struct CollisionData 
    {
        CollisionData( void )
            :
        mCollisionDistance( 0.0f ),
        mpCollisionVolume( 0 )
        {
        }
        static const int MAX_COLLISIONS = 8;
        rmt::Vector mCollisionPosition; 
        rmt::Vector mCollisionNormal;
        float mCollisionDistance;
        sim::CollisionVolume* mpCollisionVolume;
    };
    CollisionData mCollisionData[ CollisionData::MAX_COLLISIONS ];

    bool mbIsStanding;
    WalkerLocomotionAction* mpWalkerLocomotion;
    JumpAction* mpJumpLocomotion;


    sim::CollisionVolume* mpStandingCollisionVolume;
    const poser::Joint* mpStandingJoint;
    rmt::Matrix mParentTransform;
    rmt::Matrix mInvParentTransform;
    float mfGroundVerticalVelocity;
    float mfGroundVerticalPosition;

    bool mbTurbo;
    bool mbIsJump;
    bool mbSolveCollisions;

	static const int MAX_PROPS = 1;
	
	Prop mPropList[ MAX_PROPS ];

	ActionButton::AttachProp* mpPropHandler;
	int mPropJoint;
    
    bool mVisible;
    
    WorldScene* mpWorldScene;
	bool m_IsSimpleShadow;

    // when we apply choreo puppet skeleton over top of differently 
    // scaled character model, this value is to adjust the difference between
    // the character model root and the npd skeleton root. Can be +ve or -ve
    float mYAdjust;

    bool mbBusy;

    bool mbSimpleLoco;
    bool mbNeedChoreoUpdate;

    tColour mShadowColour;

    float m_TimeLeftToShock;
    bool m_IsBeingShocked;

    bool mDoKickwave;
    tDrawable* mKickwave;
    tFrameController* mKickwaveController;

    bool mAmbient;
    tUID mAmbientLocator;
    AmbientDialogueTrigger* mAmbientTrigger;

    rmt::Vector mLastFramePos;

    bool mbDoGroundIntersect;

    unsigned mIntersectFrame;

    bool mAllowRockin;

    bool mHasBeenHit;

    bool mbSnapToGround;

    float mSecondsSinceActionControllerUpdate; 

    bool mTooFarToUpdate;

    float mSecondsSinceOnPostSimUpdate;

    Role mRole;

    float mScale;

    bool mCollidedThisFrame;

    bool mIsInSubstep;

    rmt::Vector mLean;

    bool mIsLisa;
    bool mIsMarge;

    rmt::Vector mLastGoodPosOverStatic;
    
    rmt::Vector lameAssPosition;
    
    bool mManaged;
};

/*
==============================================================================
Character::GetFacing
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector& facingVector )

Return:         void

=============================================================================
*/
inline void Character::GetFacing( rmt::Vector& facingVector ) const
{
    if ( mpPuppet )
    {
        // Transform from object to world space.
        //
        facingVector = mpPuppet->GetFacingVector( );
        // Transform from object to world space.
        //
        //mInvParentTransform.RotateVector( facingVector, &facingVector );
        return;
    }
    facingVector.Set( 0.0f, 0.0f, -1.0f );
}
/*
==============================================================================
Character::GetFacingDir
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         float

=============================================================================
*/
inline float Character::GetFacingDir( void ) const
{
    rmt::Vector facing;
    GetFacing( facing );
    return choreo::GetWorldAngle( facing.x, facing.z );
}

/*
==============================================================================
Character::SetFacing
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector& facingVector )

Return:         inline 

=============================================================================
*/
inline void Character::SetFacing( rmt::Vector& facingVector )
{
    if ( mpPuppet )
    {
        mpPuppet->SetFacingVector( facingVector );
    }
}
/*
==============================================================================
Character::SetFacingDir
==============================================================================
Description:    Comment

Parameters:     ( float fDir )

Return:         void

=============================================================================
*/
inline void Character::SetFacingDir( float fDir )
{
    rmt::Vector facing;
    facing = choreo::DEFAULT_FACING_VECTOR;
    choreo::RotateYVector( fDir, facing );
    SetFacing( facing );
}
/*
==============================================================================
Character::GetDesiredFacing
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector& facingVector )

Return:         void

=============================================================================
*/
inline void Character::GetDesiredFacing( rmt::Vector& facingVector ) const
{
    facingVector = choreo::DEFAULT_FACING_VECTOR;
	choreo::RotateYVector( mfDesiredDir, facingVector );
    // Transform from object to world space.
    //
    //mInvParentTransform.RotateVector( facingVector, &facingVector );
}
/*
==============================================================================
Character::GetDesiredDir
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         float

=============================================================================
*/
inline float Character::GetDesiredDir( void ) const
{
    rmt::Vector facing;
    GetDesiredFacing( facing );
    return choreo::GetWorldAngle( facing.x, facing.z );
}

/*
==============================================================================
Character::SetPosition
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector& position )

Return:         void

=============================================================================
*/
inline void Character::SetPosition( const rmt::Vector& position )
{
	if ( mpPuppet )
    {
        // Transform from world to object space.
        //
        rmt::Vector transformedPos = position;
        transformedPos.Transform( mInvParentTransform );

        mpPuppet->SetPosition( transformedPos );
    }

    sim::SimState* simState = mpSimStateObj; //GetSimState();
    if( simState != NULL && simState->GetControl() == sim::simAICtrl )
    {
        UpdateSimState( 0.0f );
    }
}

/*
==============================================================================
Character::GetPosition
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector& position )

Return:         void

=============================================================================
*/
inline void Character::GetPosition( rmt::Vector& position ) const
{
    if ( mpPuppet )
    {
        position = mpPuppet->GetPosition( );
        position.Transform( mParentTransform );
    }
    else
    {
        position.Set(0.0f, 0.0f, 0.0f);
    }
}
/*
==============================================================================
Character::GetVelocity
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector& velocity )

Return:         void

=============================================================================
*/
inline void Character::GetVelocity( rmt::Vector& velocity ) const
{
    velocity = mVelocity;
}
/*
==============================================================================
Character::GetLocoVelocity
==============================================================================
Description:	How fast is the character running/walking (locomoting)

Parameters:		()

Return:			inline 

=============================================================================
*/
inline const rmt::Vector& Character::GetLocoVelocity() const
{
	return mpPuppet->GetVelocity( );
}
/*
==============================================================================
Character::GetSpeed
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         float

=============================================================================
*/
inline float Character::GetSpeed( void ) const
{
	return mfSpeed;
}
/*
==============================================================================
Character::SetSpeed
==============================================================================
Description:    Comment

Parameters:     ( float fSpeed )

Return:         void

=============================================================================
*/
inline void Character::SetSpeed( float fSpeed )
{
    rmt::Vector facing;
    GetFacing( facing );
    facing.Scale( fSpeed );
    mParentTransform.RotateVector( facing, &facing );
    mpSimStateObj->VelocityState( ).mLinear = facing;
}
/*
==============================================================================
Character::GetDesiredSpeed
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         float

=============================================================================
*/
inline float Character::GetDesiredSpeed( void ) const
{
	return mfDesiredSpeed;
}
/*

/*
==============================================================================
Character::SetDesiredDir
==============================================================================
Description:    Comment

Parameters:     ( float fDesiredDir )

Return:         void

=============================================================================
*/
inline void Character::SetDesiredDir( float fDesiredDir )
{
    mfDesiredDir = fDesiredDir;
}
/*
==============================================================================
Character::SetDesiredSpeed
==============================================================================
Description:    Comment

Parameters:     ( float fDesiredSpeed )

Return:         void

=============================================================================
*/
inline void Character::SetDesiredSpeed( float fDesiredSpeed )
{
    if ( fDesiredSpeed > GetMaxSpeed( ) )
    {
        fDesiredSpeed = GetMaxSpeed( );
    }
    mfDesiredSpeed = fDesiredSpeed;
}

/*
==============================================================================
Character::GetPuppet
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         choreo

=============================================================================
*/
inline choreo::Puppet* Character::GetPuppet( void ) const
{
    return mpPuppet;
}

/*
==============================================================================
Character::IsInCar
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         bool 

=============================================================================
*/
inline bool Character::IsInCar( void ) const
{
    return mbInCar;
}


/*
==============================================================================
Character::SetController
==============================================================================
Description:    Comment

Parameters:     ( CharacterController* pController )

Return:         void

=============================================================================
*/
inline void Character::SetController( CharacterController* pController )
{
    tRefCounted::Assign( mpController, pController );
}
/*
==============================================================================
Character::GetController
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         CharacterController

=============================================================================
*/
inline CharacterController* Character::GetController( void ) const
{
    return mpController;
}

/*
==============================================================================
Character::GetTarget
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         CharacterTarget

=============================================================================
*/
inline CharacterTarget* Character::GetTarget( void ) const
{
    rAssert( mpCharacterTarget );
    return mpCharacterTarget;
}

/*
==============================================================================
Character::SetTargetVehicle
==============================================================================
Description:    Comment

Parameters:     ( Vehicle* pVehicle )

Return:         void 

=============================================================================
*/
inline void Character::SetTargetVehicle( Vehicle* pVehicle )
{
    mpTargetVehicle = pVehicle;
}
/*
==============================================================================
Character::GetTargetVehicle
==============================================================================
Description:    Comment

Parameters:     ( )

Return:         Vehicle

=============================================================================
*/
inline Vehicle* Character::GetTargetVehicle( ) const
{
    return mpTargetVehicle;
}
/*
==============================================================================
Character::GetStateManager
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         CharacterAi

=============================================================================
*/
inline CharacterAi::StateManager* Character::GetStateManager( void ) const
{
    return mpStateManager;
}

/*
==============================================================================
Character::GetActionController
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ActionController

=============================================================================
*/
inline ActionController* Character::GetActionController( void ) const
{
    return mpActionController;
}
/*
==============================================================================
Character::GetJumpHeight
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         float 

=============================================================================
*/
inline float Character::GetJumpHeight( void )
{
    return CharacterTune::sfJumpHeight;
}
/*
==============================================================================
Character::GetRootTransform
==============================================================================
Description:    Comment

Parameters:     ( poser::Transform& out )

Return:         void 

=============================================================================
*/
inline void Character::GetRootTransform( poser::Transform& out ) const
{
    if ( mpPuppet )
    {  
        out = mpPuppet->GetRootTransform( );
        poser::Transform parentTransform;
        parentTransform.SetMatrix( mParentTransform );
        out.Mult( parentTransform );
    }
    else
    {
        out.Identity( );
    }
}
/*
==============================================================================
Character::WorldToLocal
==============================================================================
Description:    Comment

Parameters:     ( const rmt::Vector& world )

Return:         rmt

=============================================================================
*/
inline rmt::Vector Character::WorldToLocal( const rmt::Vector& world ) const
{
    rmt::Vector local = world;
    local.Transform( mInvParentTransform );
    return local;
}
/*
==============================================================================
Character::LocalToWorld
==============================================================================
Description:    Comment

Parameters:     ( const rmt::Vector& local )

Return:         rmt

=============================================================================
*/
inline rmt::Vector Character::LocalToWorld( const rmt::Vector& local ) const
{  
    rmt::Vector world = local;
    world.Transform( mParentTransform );
    return world;
}
/*
==============================================================================
Character::GetCollisionAreaIndex
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         int 

=============================================================================
*/
inline int Character::GetCollisionAreaIndex( void ) const
{
    return mCollisionAreaIndex;
}
/*
==============================================================================
Character::IsInCollision
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         bool 

=============================================================================
*/
inline bool Character::IsInCollision( void ) const
{
    return mbCollided;
}
/*
==============================================================================
Character::IsStanding
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         bool 

=============================================================================
*/
inline bool Character::IsStanding( void ) const
{
    return mbIsStanding;
}
/*
==============================================================================
Character::GetWalkerLocomotionAction
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         WalkerLocomotionAction

=============================================================================
*/
inline WalkerLocomotionAction* Character::GetWalkerLocomotionAction( void ) const
{
    return mpWalkerLocomotion;
}
/*
==============================================================================
Character::GetJumpLocomotionAction
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         JumpAction

=============================================================================
*/
inline JumpAction* Character::GetJumpLocomotionAction( void ) const
{
    return mpJumpLocomotion;
}
/*
==============================================================================
Character::GetParentTransform
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         const 

=============================================================================
*/
inline const rmt::Matrix& Character::GetParentTransform( void ) const
{
    return mParentTransform;
}
/*
==============================================================================
Character::GetInverseParentTransform
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         const 

=============================================================================
*/
inline const rmt::Matrix& Character::GetInverseParentTransform( void ) const
{
    return mInvParentTransform;
}
/*
==============================================================================
Character::GetGroundVerticalVelocity
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         float 

=============================================================================
*/
inline float Character::GetGroundVerticalVelocity( void ) const
{
    return mfGroundVerticalVelocity;
}

/*
==============================================================================
Character::IsJumping
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         bool 

=============================================================================
*/
inline bool Character::IsJumping( void ) const
{
    return mbIsJump;
}
/*
==============================================================================
Character::SetJumping
==============================================================================
Description:    Comment

Parameters:     ( bool bIsJumping )

Return:         void 

=============================================================================
*/
inline void Character::SetJumping( bool bIsJumping )
{
    mbIsJump = bIsJumping;
}

/*
==============================================================================
Character::SetTurbo
==============================================================================
Description:    Comment

Parameters:     ( bool bTurbo )

Return:         void 

=============================================================================
*/
inline void Character::SetTurbo( bool bTurbo )
{
    if( bTurbo != mbTurbo )
    {
        if( bTurbo )
        {
            GetEventManager()->TriggerEvent( EVENT_TURBO_START, this );
        }
        mbTurbo = bTurbo;
    }
}
/*
==============================================================================
Character::IsTurbo
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         bool 

=============================================================================
*/
inline bool Character::IsTurbo( void ) const
{
    return mbTurbo;
}

/*
==============================================================================
Character::IsSimpleShadow
==============================================================================
Description:    Is the shadow a blobby shadow or a volume shadow?

Parameters:     ( void )

Return:         bool

=============================================================================
*/
inline bool Character::IsSimpleShadow( void )
{
	return m_IsSimpleShadow;
}
/*
==============================================================================
Character::SetSimpleShadow
==============================================================================
Description:    Set if the character's shadow is a blobby, in apposed to the
				volumetric.

Parameters:     ( bool )

Return:         void

=============================================================================
*/
inline void Character::SetSimpleShadow( bool IsSimpleShadow )
{
	m_IsSimpleShadow =  IsSimpleShadow;
}

inline bool Character::IsNPC()
{
    return mIsNPC;
}

class NPCharacter
:
public Character
{
public:
    NPCharacter( void );
    ~NPCharacter( void );

    virtual int GetAIRef() { return PhysicsAIRef::NPCharacter;}

    virtual void AddToPhysics( void );
    virtual void RemoveFromPhysics( void );

    virtual void SubmitStatics( void );
    virtual void SubmitAnimCollisions( void ) {};
    virtual void SubmitDynamics( void );

    virtual void UpdatePhysicsObjects( float timeins, int area );
    virtual void OnTransitToAICtrl();

    virtual void ApplyKickForce( const rmt::Vector& direction, float force );
    virtual void ApplyForce( const rmt::Vector& direction, float force );
protected:

    virtual void AssignCollisionAreaIndex( void );
    virtual float GetInputScale( void )
    {   
        // Scale 1.0f does nothing.
        return 1.0f;
    }
private:
    virtual void OnUpdateRoot( float timeins );
    virtual void OnPostSimUpdate(float timeins);
    virtual void UpdateFootPlant( void )
    {
    }
    int mMappableHandle;
};



#endif // CHARACTER_H_

