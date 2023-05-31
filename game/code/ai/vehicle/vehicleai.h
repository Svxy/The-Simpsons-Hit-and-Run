//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehicleai.h
//
// Description: Blahblahblah
//
// History:     27/06/2002 + Created -- NAME
//
//=============================================================================

#ifndef VEHICLEAI_H
#define VEHICLEAI_H

//========================================
// Nested Includes
//========================================

#include <roads/roadmanager.h>
#include <roads/roadsegment.h>
#include <radmath/radmath.hpp>
#include <ai/vehicle/potentialfield.h>
#include <presentation/gui/utility/hudmap.h>
#include <worldsim/redbrick/vehiclecontroller/aivehiclecontroller.h>

//========================================
// Forward References
//========================================

class Intersection;
class Road;


//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class VehicleAI : public AiVehicleController,
                  public IHudMapIconLocator
{
public:
    enum VehicleAITypeEnum
    {
        AI_WAYPOINT,
        AI_CHASE,
        NUM_AI_TYPES
    };
    enum VehicleAIState
    {
        STATE_WAITING,
        STATE_WAITING_FOR_PLAYER,
        STATE_ACCEL,
        STATE_BRAKE,
        STATE_CORNER_PREPARE,
        STATE_REVERSE,
        STATE_STOP,
        STATE_EVADE,
        STATE_LIMBO,
        STATE_STUNNED,
        STATE_OUT_OF_CONTROL,
        NUM_STATES
    };

    static const int DEFAULT_MIN_SHORTCUT_SKILL;
    static const int DEFAULT_MAX_SHORTCUT_SKILL;

    // TUNABLE catch-up values 
    //static const float CATCHUP_MAX_SPEED_MOD;
    static const float CATCHUP_NORMAL_DRIVING_PERCENTAGE_OF_TOPSPEED;
    static const int CATCHUP_MAX_SHORTCUTSKILL_MOD;


    VehicleAI( 
        Vehicle* pVehicle, 
        VehicleAITypeEnum type, 
        bool enableSegmentOptimization=true,
        int minShortcutSkill=DEFAULT_MIN_SHORTCUT_SKILL,
        int maxShortcutSkill=DEFAULT_MAX_SHORTCUT_SKILL,
        bool useMultiplier=true );

    virtual ~VehicleAI();

    //
    // Call Init once at the beginning of the mission
    // and Finalize once at the end. Reset when you need to just reset states
    //
    virtual void Initialize();
    virtual void Finalize();
    virtual void Reset();

    void ResetControllerValues();

    void SetMinShortcutSkill( int skill );
    int GetMinShortcutSkill();
    void SetMaxShortcutSkill( int skill );
    int GetMaxShortcutSkill();

    int GetShortcutSkillMod();

    void SetActive( bool bIsActive );

    //
    // Call this bad mofo every frame
    //
    virtual void Update( float timeins );

    VehicleAITypeEnum GetType();

    virtual void GetPosition( rmt::Vector* currentLoc );
    virtual void GetHeading( rmt::Vector* heading );
    virtual void EnterLimbo ();
    virtual void ExitLimbo ();
    VehicleAIState GetState();

    int GetHUDIndex() const { return mHudIndex; };

    void GetDestination( rmt::Vector& pos );
    void GetNextDestination( rmt::Vector& pos );

    static bool FindClosestPathElement( 
        rmt::Vector& pos, 
        RoadManager::PathElement& elem, 
        RoadSegment*& seg, 
        float& segT,
        float& roadT,
        bool considerShortcuts );

    void GetLastPathInfo( 
        RoadManager::PathElement& elem,
        RoadSegment*& seg, 
        float& segT,
        float& roadT );

    void GetRacePathInfo( 
        RoadManager::PathElement& elem,
        RoadSegment*& seg, 
        float& segT,
        float& roadT );

    float GetDesiredSpeedKmh();

    void SetUseMultiplier( bool use );


    struct RaceCatchupParams
    {
        float DistMaxCatchup; // dist at which catchup is 1
        float FractionPlayerSpeedMinCatchup; // fraction of player vehicle's speed to use as target speed when catchup is -1
        float FractionPlayerSpeedMidCatchup; // fraction of player vehicle's speed to use as target speed when catchup is 0
        float FractionPlayerSpeedMaxCatchup; // fraction of player vehicle's speed to use as target speed when catchup is 1
    };
    struct EvadeCatchupParams
    {
        float DistPlayerTooNear;  // dist at which catchup is 1
        float DistPlayerFarEnough; // dist at which catchup is 0
    };
    struct TargetCatchupParams
    {
        float DistPlayerNearEnough; // dist at which catchup is 0
        float DistPlayerTooFar; // dist at which catchup is -1
    };
    struct CatchupParams
    {
        RaceCatchupParams Race;
        EvadeCatchupParams Evade;
        TargetCatchupParams Target;
    };

    void SetRaceCatchupParams( const RaceCatchupParams& raceParams );
    void SetEvadeCatchupParams( const EvadeCatchupParams& evadeParams );
    void SetTargetCatchupParams( const TargetCatchupParams& targetParams );



public:
    class Segment
    {
    public: // MEMBERS
        rmt::Vector mStart;
        rmt::Vector mEnd;
        float mLength;
        RoadSegment* mpSegment;
        int mType;  // 0 = normal roadsegment, 
                    // 1 = first tweening segment, 
                    // 2 = second tweening segment
    public: // METHODS

        void InitZero()
        {
            mStart.Set( 0.0f, 0.0f, 0.0f );
            mEnd.Set( 0.0f, 0.0f, 0.0f );
            mLength = 0.0f;
            mpSegment = NULL;
            mType = 0;
        }

        Segment() 
        {
            InitZero();
        }

        Segment& operator=( const Segment& src )
        {
            mStart = src.mStart;
            mEnd = src.mEnd;
            mLength = src.mLength;
            mpSegment = src.mpSegment;
            mType = src.mType;
            return (*this);
        }

        void SelfVerify()
        {
        #ifdef RAD_DEBUG
            rAssert( !rmt::IsNan(mStart.x) && !rmt::IsNan(mStart.y) && !rmt::IsNan(mStart.y) );
            rAssert( !rmt::IsNan(mEnd.x) && !rmt::IsNan(mEnd.y) && !rmt::IsNan(mEnd.y) );
            rAssert( !rmt::IsNan(mLength) );
            rAssert( !rmt::Epsilon( mLength, 0.0f, 0.001f ) );
            rAssert( mpSegment != NULL );
            rAssert( mType == 0 || mType == 1 || mType == 2 );
        #endif
        }

    };

    // NOTE: 
    // This array needs to be large enough to account for at least 2 roads
    // for optimization purposes..
    static const int MAX_SEGMENTS = 20;
    int mNumSegments;
    Segment mSegments[ MAX_SEGMENTS ];


protected:


    void SetState( VehicleAIState state );

    void DriveTowards( rmt::Vector* dest, float &distToTarget, 
                       float& steering  );

    void SetDestination( rmt::Vector& pos );
    void SetNextDestination( rmt::Vector& pos );

    void CheckState( float timeins );
    virtual void DoCatchUp( float timeins );
    void DoSteering();
    void FollowRoad();
    void EvadeTraffic( Vehicle* exceptThisOne );

    // TODO:
    // Remove this after the new pathfinding stuff goes in (no need for it anymore)
    //virtual bool DetermineNextRoad( const Road** pRoad ) = 0;
    //bool FindNextRoad( const Road** pRoad, rmt::Vector& nextDestination );

    virtual bool MustRepopulateSegments();
    virtual bool TestReachedTarget( const rmt::Vector& start, const rmt::Vector& end ) = 0;

    void UpdateSegments();
    void FindClosestSegment( const rmt::Vector& pos, int& closestIndex, float& closestDistSqr, rmt::Vector& closestPt );
    void ResetSegments();
    void ShiftSegments( int numShifts, int first=0 );
    void FillSegments();
    void GetPosAheadAlongRoad( float t, float lookAheadDist, int i, rmt::Vector* lookAheadPos );

    int DetermineShortcutSkill();

    virtual void UpdateSelf();
    virtual void GetClosestPathElementToTarget( 
        rmt::Vector& targetPos,
        RoadManager::PathElement& elem,
        RoadSegment*& seg,
        float& segT,
        float& roadT ) = 0;

    void FillPathElements();

    void ResetCatchUpParams();


protected:
    float mSecondsStunned;
    float mSecondsOutOfControl;
    rmt::Vector mOutOfControlNormal;

    SwapArray<RoadManager::PathElement> mPathElements;
    int mCurrPathElement;

    // path info stuff
    RoadManager::PathElement mLastPathElement;
    RoadSegment* mLastRoadSegment; // the last (or current) road we're on
    float mLastRoadSegmentT;
    float mLastRoadT;

    // race position stuff
    RoadManager::PathElement mRacePathElement;
    RoadSegment* mRaceRoadSegment;
    float mRaceRoadSegmentT;
    float mRaceRoadT;


    // catch-up logic stuff
    int mShortcutSkillMod;
    float mDesiredSpeedKmh;
    float mSecondsSinceLastDoCatchUp;

    CatchupParams mCatchupParams; // persistent catch-up tunables

private:

    //Prevent wasteful constructor creation.
    VehicleAI( const VehicleAI& vehicleai );
    VehicleAI& operator=( const VehicleAI& vehicleai );

    virtual int RegisterHudMapIcon() = 0;

    rmt::Vector mDestination;
    rmt::Vector mNextDestination;

    unsigned int mStartStuckTime;
    unsigned int mNextStuckTime;

    float mSteeringRatio;

    VehicleAIState mState;
    VehicleAIState mLimboPushedState;

    PotentialField mPotentialField;

    VehicleAITypeEnum mType;

    int mHudIndex;

    float mSecondsBeforeCorner;

    int mRenderHandle;

    int mMinShortcutSkill;
    int mMaxShortcutSkill;

    float mSecondsLeftToGetBackOnPath;
    float mReverseTime;

    bool mEvadeVehicles             : 1;
    bool mEvadeStatics              : 1;
    bool mEvading                   : 1;
    bool mEnableSegmentOptimization : 1;

    // to use AGAINST_TRAFFIC_COST_MULTIPLIER in pathfinding or to not use it...
    // if we use it, we pretty much guarantee we won't drive on the wrong side
    // of the road (good for avoiding traffic). There are some cases where we 
    // may not want to use it, such as for ChaseAI, or for when it's a street race.
    // We should expose the ability to turn it on/off at will...
    bool mUseMultiplier             : 1; 
};

inline float VehicleAI::GetDesiredSpeedKmh()
{
    return mDesiredSpeedKmh;
}

inline VehicleAI::VehicleAIState VehicleAI::GetState() 
{ 
    return mState; 
}

inline void VehicleAI::SetState( VehicleAIState state ) 
{ 
    mState = state; 
}

inline void VehicleAI::GetDestination( rmt::Vector& pos )
{
    pos = mDestination;
}
inline void VehicleAI::GetNextDestination( rmt::Vector& pos )
{
    pos = mNextDestination;
}

inline VehicleAI::VehicleAITypeEnum VehicleAI::GetType()
{
    return mType;
}
inline void VehicleAI::SetUseMultiplier( bool use )
{
    mUseMultiplier = use;
}

#endif //VEHICLEAI_H
