//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehicle.h
//
// Description: the car
//
// History:     Nov 16, 2001 + Created, gmayer
//
//=============================================================================


#ifndef _VEHICLE_H
#define _VEHICLE_H


//========================================
// Nested System Includes
//========================================

#include <camera/isupercamtarget.h>
#include <constants/vehicleenum.h>
#include <presentation/gui/utility/hudmap.h>
#include <radmath/radmath.hpp>
#include <render/DSG/InstDynaPhysDSG.h>
#include <render/intersectmanager/intersectmanager.h> // For terrain type enumeration.

//========================================
// Forward References
//========================================

class Character;
class EventLocator;
class GeometryVehicle;
class PhysicsLocomotion;
class RectTriggerVolume;
class RootMatrixDriver;
class StatePropCollectible;
class SuspensionJointDriver;
class TrafficLocomotion;
class TrafficVehicle;
class VehicleEventListener;
class VehicleLocomotion;
class Wheel;

namespace poser
{
    class PoseEngine;
}

namespace sim
{
    class PhysicsJointInertialEffector;
    class PhysicsJointMatrixModifier;
    class SimStateArticulated;
    class ArticulatedPhysicsObject;
    class PhysicsProperties;
}

enum VehicleLocomotionType {VL_PHYSICS, VL_TRAFFIC};
enum VehicleState {VS_NORMAL, VS_SLIP, VS_EBRAKE_SLIP};

enum VehicleType {VT_USER, VT_AI, VT_TRAFFIC, VT_LAST};

//=============================================================================
//
// The Vehicle.
//
//=============================================================================
class Vehicle : public DynaPhysDSG, public ISuperCamTarget, public IHudMapIconLocator
{
public:

    //-----
    // core
    //-----

    Vehicle();
    virtual ~Vehicle();

    /*
    void AddRef();
    void Release();
    */

    bool Init( const char* name, sim::SimEnvironment* se, VehicleLocomotionType loco, VehicleType vt = VT_USER, bool startoutofcar = true);
    void Reset( bool ResetDamage = true, bool clearTraffic = false );
    void ResetOnSpot( bool resetDamage=true, bool moveCarOntoRoad = true );
    
    // make sure, for gui reasons, we don't call repeatedly
    bool mAlreadyCalledAutoResetOnSpot;
    
    void SetPosition(rmt::Vector* newPos);
    
    void TransitToAI();

    // utility
    float FacingIntoRad(rmt::Vector facing);
    
    // reset has come to be the one that uses the initial position & facing
    // make a different reset that will just reset the flags and state - set transform can be called in 
    // conjunction with this
    void ResetFlagsOnly(bool resetDamage);

    VehicleType mVehicleType;

    int mVehicleCentralIndex;

    //
    // Dlong: Proposed new methods (works better!)
    //
    void SetInitialPositionGroundOffsetAutoAdjust( rmt::Vector* newPos );
    void SetInitialPosition( rmt::Vector* newPos );
    void SetResetFacingInRadians( float rotation );
    float GetFacingInRadians();

    void SetTransform( rmt::Matrix &m );
    void TrafficSetTransform(rmt::Matrix &m);

    // for debugging and a cheat
    void JumpOnHorn(float test);

    void TurboOnHorn();
    float mSecondsTillNextTurbo;
    int mNumTurbos;

    // Implements CollisionEntityDSG
    //
    virtual sim::Solving_Answer PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision );
    virtual sim::Solving_Answer PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision);

    // drawable vehicle encapsulation:
    GeometryVehicle* mGeometryVehicle;

    rmt::Matrix mTransform;
        
    // TODO - this is temporary?
    const rmt::Vector& GetPosition() { return *((rmt::Vector*)mTransform.m[3]); }
    const rmt::Vector& GetFacing() { return mVehicleFacing; }
    const rmt::Matrix& GetTransform();
    const rmt::Vector& GetExtents() { return mExtents; }
        
    char* mName;
    rmt::Vector mInitialPosition;
    float mResetFacingRadians;

    //This is which type of vehicle it is.
    VehicleEnum::VehicleID mVehicleID;
    void AssignEnumBasedOnName();


    // vehicle event stuff
    VehicleEventListener* mVehicleEventListener;
    
    void EnteringJumpBoostVolume();
    void ExitingJumpBoostVolume();
    
    bool mDoingJumpBoost;
    

    void ActivateTriggers( bool activate );
    /*

         in pure3d lhc, top-down view of vehicle:



                      ^
                      |
                      |
                      |
                      +Z 
                      |
                      |
                    front
                _____________  
       wheel 2  |           |  wheel 3 
                |           |   
                |           |   
                |           |   
                |           |   
                |           |   
                |           | ---+X----->   
                |           |   
                |           |   
                |           |   
                |           |   
                |           |   
       wheel 1  -------------  wheel 0
                     rear



    */

    rmt::Vector mVehicleFacing;
    rmt::Vector mVehicleUp;
    rmt::Vector mVehicleTransverse;


    rmt::Vector mVelocityCM;
    float mSpeed;
    float mPercentOfTopSpeed;
    float mSpeedKmh;
    float mLastSpeedKmh;
    float mAccelMss;

    rmt::Vector mOriginalCMOffset;
    rmt::Vector mCMOffset;  // this one modified by designer input

    //----------------
    // Shadow stuff
    //----------------

    virtual int CastsShadow();
	bool IsSimpleShadow( void ) { return m_IsSimpleShadow; }
	void SetSimpleShadow( bool IsSimpleShadow ) { m_IsSimpleShadow = IsSimpleShadow; }
    void SetShadow( tShadowMesh* pShadowMesh ){};
    void DisplayShadow();
	void DisplaySimpleShadow( void );

    //----------------
    // Terrain type stuff
    //----------------

    // This is a 'rollup' from the four wheels.
    eTerrainType mTerrainType;
    bool mInterior;
    
    
    float GetGroundY();


    //----------------
    // locomotion shit
    //----------------    
    
    VehicleState mVehicleState; //{VS_NORMAL, VS_SLIP};

    VehicleLocomotionType GetLocomotionType() {return mLoco;}

    VehicleLocomotion* mVehicleLocomotion;
    VehicleLocomotionType mLoco;
    

    PhysicsLocomotion* mPhysicsLocomotion;


    // *** For Traffic *** //
    TrafficVehicle* mTrafficVehicle;

    TrafficLocomotion* mTrafficLocomotion;

    void CreateLocomotions();
    void SetLocomotion( VehicleLocomotionType loco );
           
    
    friend class PhysicsLocomotion;
    friend class TrafficLocomotion;
   
    bool mLocoSwitchedToPhysicsThisFrame;
    
    //---------------------------
    // world simulation interface
    //---------------------------

    void PreSubstepUpdate(float dt);    // only thing this does right now is reset a flag
    void PostSubstepUpdate(float dt); 
    void PreCollisionPrep(float dt, bool firstSubstep);  
    void Update(float dt);

    void SetVehicleSimEnvironment(sim::SimEnvironment* se); // don't actually need this anymore

    void GetCollisionAreaIndexAndAddSelf();
    void RemoveSelfAndFreeCollisionAreaIndex();

    void AddSelfToCollisionManager();
    void AddToOtherCollisionArea(int index);
    void RemoveSelfFromCollisionManager();

    int mCollisionAreaIndex; // index into the collision area this instance will use.
    RenderEnums::LayerEnum mRenderLayerEnum;
    void SetRenderLayerEnum(RenderEnums::LayerEnum renderLayerEnum) {mRenderLayerEnum = renderLayerEnum;}
    RenderEnums::LayerEnum GetRenderLayerEnum() {return mRenderLayerEnum;}

    void DebugDisplay();
    void CarDisplay(bool doit);
    bool mOkToDrawSelf;

    void DrawVehicle( bool draw ) { mDrawVehicle = draw; };
    bool mDrawVehicle;  //This is the same thing as above, but Greg uses the above for debugging.

    //---------------------
    // controller interface
    //---------------------

    void SetGas(float gas);
    void SetBrake(float brake);
    void SetWheelTurnAngle(float wheelTurnAngle, bool doNotModifyInputValue, float dt); 
    void SetReverse(float reverse);
    void SetEBrake(float ebrake, float dt); // new timing thing for plum
    
    void SetWheelTurnAngleDirectlyInRadiansForDusitOnly(float rad);

    // Vehicle's cached values
    float mGas;
    float mLastGas;
    float mDeltaGas;
    float mBrake;
    float mWheelTurnAngle;
    float mWheelTurnAngleInputValue;    // for Plum's low speed lag
    float mReverse;
    float mEBrake;
    float mEBrakeTimer;
    bool mBrakeLightsOn;
    bool mReverseLightsOn;
    
    float mBrakeTimer;
    bool mBrakeActingAsReverse;


    float mSteeringInputThreshold;  // my own values to modify the h/w input value
    float mSteeringPreSlope;
    
    // the designers will have their own value to max the dropoff    

    float mUnmodifiedInputWheelTurnAngle; // just for ease of looking at the pure input
    

    bool mDoingRockford;
    
    float mSpeedBurstTimer;
    bool mBuildingUpSpeedBurst;
    bool mDoSpeedBurst;
    float mFOVToRestore;
    float mSpeedBurstTimerHalf;

    //to disable player control of vehicle
    void SetDisableGasAndBrake(bool tf) { mGasBrakeDisabled = tf; }
    bool mGasBrakeDisabled;

    //----------------------------------
    // RenderManager/EntityDSG Interface
    //----------------------------------
    void Display();
    rmt::Vector*        pPosition();
    const rmt::Vector&  rPosition();

    sim::SimState* GetSimState() const {return (sim::SimState*)mSimStateArticulated;}
    sim::SimState* mpSimState() const { return GetSimState(); }

    void DSGUpdateAndMove();

    // just in case
    virtual void OnSetSimState( sim::SimState* ipSimState ) {}; 

    // stub out?
    //virtual int FetchGroundPlane();    
    //virtual void FreeGroundPlane();    

    //virtual bool IsAtRest();    

    //-----------------
    // camera interface
    //-----------------

    virtual void GetPosition( rmt::Vector* position );
    virtual void GetHeading( rmt::Vector* heading );
    virtual void GetVUP( rmt::Vector* vup );
    virtual void GetVelocity( rmt::Vector* velocity );
    virtual unsigned int GetID();
    virtual bool IsCar() const;
    virtual bool IsAirborn();
    virtual bool IsUnstable(); 
    virtual bool IsQuickTurn();
    virtual bool IsInReverse();
    virtual void GetFirstPersonPosition( rmt::Vector* position ) {};
    virtual void GetTerrainIntersect( rmt::Vector& pos, rmt::Vector& normal ) const;


    void CameraShakeTest(float impulseMagnitude, sim::Collision& inCollision);
    
    bool IsMovingBackward();    // for Darren - don't have to have 'brake' input held down

    bool mSteeringWheelsOutOfContact;    // note:
                        // use for adjusting locomotive force appliction points when cresting hills, so turn this on
                        // when wheels 2,3 leave ground

    bool mAirBorn;

    bool mWeebleOn;
    bool mCMOffsetSetToOriginal;
    
    bool mInstabilityOffsetOn;
            
    // ------------
    // ai interface
    // ------------
    void SetDriverName(const char*);
    Character* GetDriver()const{ return mpDriver; }
    const char* GetDriverName(void);

    bool HasDriver( void ) const    // note! -  this only determines if the car has a character model in the driver seat
    {                               //          nothing to do with whether or not there is a 'user' driving the car
        return (mpDriver != NULL) || mPhantomDriver;
    }

    void SetDriver(Character*);

    void SetPhantomDriver(bool b) { mPhantomDriver = b;}

    char mDriverName[32];
    Character* mpDriver;
    bool mPhantomDriver;

    void BounceCharacters(float dt);
    void RecordRestSeatingPositionsOnEntry();
    rmt::Vector mOurRestSeatingPosition;
    rmt::Vector mNPCRestSeatingPosition;
    static bool sDoBounce;

    float mYAccelForSeatingOffset;
    
    float mMaxBounceDisplacementPerSecond;
    float mBounceAccelThreshold;    // below this value just try and move back to rest
                                    // if accel is above this value then we move opposite accel direction
    
    rmt::Vector mVelocityCMLag;
    rmt::Vector mPositionCMLag;
    
    float mBounceLimit;
    void ApplyDisplacementToCharacters(float displacement);
    void MoveCharactersTowardsRestPosition(float dt);

    virtual const char* const GetName();


    bool mUserDrivingCar;
    bool IsUserDrivingCar()const { return mUserDrivingCar; } // Sorry, I just really like to use accessor functions.
    void SetUserDrivingCar(bool b);


    virtual int GetAIRef() {return PhysicsAIRef::redBrickVehicle;}
    /*
    static int GetAIRef( void )
    {
        return PhysicsAIRef::redBrickVehicle;
    }
    */

    virtual int GetGroundPlaneAIRef() {return PhysicsAIRef::redBrickPhizVehicleGroundPlane;}

    /*
    static int GetGroundPlaneAIRef( void )
    {
        return PhysicsAIRef::redBrickPhizVehicleGroundPlane;
    }
    */

    const rmt::Vector& GetPassengerLocation( void ) const;
    const rmt::Vector& GetDriverLocation( void ) const;

    //--------------
    // sfx interface
    //--------------

    float GetSpeedKmh();
    float GetAccelMss();
    float GetRPM();
    float GetGas()const         { return mGas; }
    float GetDeltaGas()const    { return mDeltaGas; }
    float GetBrake()const       { return mBrake; }
    float GetSkidLevel();   // overall amount amongst the (currently) 4 wheels
                            // both the skidding - tire locked
                            // and slip - tire rotating really fast.
    int GetGear();  // -1 is reverse, 0 neutral
    // TODO - how to do the gearshift interface?

    bool IsSafeToUpShift();

      // gearbox stuff
    float mRPM;
    float mRPMUpRate;
    float mRPMDownRate;

    float mBaseRPM;
    float mMaxRpm;
    float mShiftPointHigh;
    float mShiftPointLow;
    int mGear;    // -1 for reverse, 0 neutral

    float mSkidLevel;
    float mBurnoutLevel;    // use for sound and smoke level?
    bool mDoingBurnout;
    void SetGeometryVehicleWheelSmokeLevel();

    bool mNoSkid;   // just to flag frinks hovering vehilces...
    bool mNoFrontSkid; // For the rocket car and other vehicles that can only skid using 
    //  the back two wheels

    bool mDoingWheelie; //need this?

    int mNumGears;
    float* mGearRatios;  
    float mFinalDriveRatio;
    void InitGears();

    void UpdateGearAndRPM();

    void SparksTest(float impulseMagnitude, sim::Collision& inCollision);  // should this also be wrapped in mUserDrivingCar??

    //--------------------
    // the designer params
    //--------------------

    struct DesignerParams
    {
        // from physicsvehicle

        float mDpGasScale;      // proportional to mass
        float mDpSlipGasScale;
        float mDpHighSpeedGasScale;
        float mDpGasScaleSpeedThreshold;
        float mDpBrakeScale;    // proportional to mass
        float mDpTopSpeedKmh;

        float mDpMass;          // think of it as kg

        float mDpMaxWheelTurnAngle;     //  in degrees
        float mDpHighSpeedSteeringDrop;       // 0.0 to 1.0

        float mDpTireLateralStaticGrip;
        float mDpTireLateralResistanceNormal;
        float mDpTireLateralResistanceSlip;

        float mDpTireLateralResistanceSlipNoEBrake; // this one's for more out of control driving
        float mDpSlipEffectNoEBrake;

        float mDpEBrakeEffect;

        float mDpSuspensionLimit;
        float mDpSpringk; 
        float mDpDamperc; 

        float mDpSuspensionYOffset;

        float mHitPoints;

        float mDpBurnoutRange;


        float mDpWheelieRange;
        float mDpWheelieYOffset;
        float mDpWheelieZOffset;

        float mDpMaxSpeedBurstTime;

        float mDpDonutTorque;

        // new tunable value for plum
        float mDpWeebleOffset;

        float mDpGamblingOdds;

        rmt::Vector mDpCMOffset;

    };

    DesignerParams mDesignerParams;

    // simple accessors that script functions call
    //
    // note - you have to call CalculateValuesBasedOnDesignerParams after using these.
    //
    // make sure to name script methods based on watcher names
    // these names can match the data.

    //Chuck: Added these so cars now have odds for gambling races.
    void SetGamblingOdds(float odds) {mDesignerParams.mDpGamblingOdds = odds;}
    float GetGamblingOdds() {return mDesignerParams.mDpGamblingOdds;}

    //Chuck:using this flag for forced car mission, since forced cars dont belong to the player,but the NPC
    //Forced cars should respawn and do not check the check to or update the charactersheet. If Flag is true then this
	//car is owned by the player, if false then its owned by the NPC  
    bool mbPlayerCar;
    
    void SetGasScale(float gs) {mDesignerParams.mDpGasScale = gs;}
    void SetSlipGasScale(float value) {mDesignerParams.mDpSlipGasScale = value;}
    
    void SetHighSpeedGasScale(float gs) {mDesignerParams.mDpHighSpeedGasScale = gs;}
    void SetGasScaleSpeedThreshold(float t) {mDesignerParams.mDpGasScaleSpeedThreshold = t;}
        
    void SetBrakeScale(float bs) {mDesignerParams.mDpBrakeScale = bs;}
    void SetTopSpeedKmh(float ts) {mDesignerParams.mDpTopSpeedKmh = ts;}
    float GetTopSpeed() const;
    void SetMass(float m) {mDesignerParams.mDpMass = m;}
    void SetMaxWheelTurnAngle(float mwta) {mDesignerParams.mDpMaxWheelTurnAngle = mwta;}
    void SetHighSpeedSteeringDrop(float value) {mDesignerParams.mDpHighSpeedSteeringDrop = value;}
    void SetTireLateralStaticGrip(float g) {mDesignerParams.mDpTireLateralStaticGrip = g;}
    void SetTireLateralResistanceNormal(float n) {mDesignerParams.mDpTireLateralResistanceNormal = n;}
    void SetTireLateralResistanceSlip(float s) {mDesignerParams.mDpTireLateralResistanceSlip = s;}
    void SetEBrakeEffect(float s) {mDesignerParams.mDpEBrakeEffect = s;}

    void SetTireLateralResistanceSlipWithoutEBrake(float s) {mDesignerParams.mDpTireLateralResistanceSlipNoEBrake = s;}
    void SetSlipEffectWithoutEBrake(float s) {mDesignerParams.mDpSlipEffectNoEBrake = s;}

    void SetCMOffsetX(float s) {mDesignerParams.mDpCMOffset.x = s;}
    void SetCMOffsetY(float s) {mDesignerParams.mDpCMOffset.y = s;}
    void SetCMOffsetZ(float s) {mDesignerParams.mDpCMOffset.z = s;}

    void SetSuspensionLimit(float s) {mDesignerParams.mDpSuspensionLimit = s;}
    void SetSuspensionSpringK(float s) {mDesignerParams.mDpSpringk = s;}
    void SetSuspensionDamperC(float s) {mDesignerParams.mDpDamperc = s;}
    
    void SetSuspensionYOffset(float s) {mDesignerParams.mDpSuspensionYOffset = s;}

    void SetHitPoints(float h) {mDesignerParams.mHitPoints = h;}
    void SetBurnoutRange(float h) {mDesignerParams.mDpBurnoutRange = h;}
    void SetMaxSpeedBurstTime(float h) {mDesignerParams.mDpMaxSpeedBurstTime = h;}
    void SetDonutTorque(float h) {mDesignerParams.mDpDonutTorque = h;}

    void SetWeebleOffset(float w) {mDesignerParams.mDpWeebleOffset = w;}

    void SetWheelieRange(float w) {mDesignerParams.mDpWheelieRange = w;}
    void SetWheelieYOffset(float y) {mDesignerParams.mDpWheelieYOffset = y;}
    void SetWheelieZOffset(float z) {mDesignerParams.mDpWheelieZOffset = z;}

    void SetShadowAdjustments( float Adjustments[ 4 ][ 2 ] );
    void SetShininess( unsigned char EnvRef );

    void CalculateValuesBasedOnDesignerParams();


    //------------
    // wheel stuff
    //------------

    void UpdateWheelRenderingInfo(float dt);
    bool IsJointAWheel(int jointIndex);
    //bool SetWheelCorrectionOffset(int jointNum, float objectSpaceYOffsetFromCurrentPosition);
    bool SetWheelCorrectionOffset(int jointNum, float objectSpaceYOffsetFromCurrentPosition, rmt::Vector& normalPointingAtCar, rmt::Vector& groundContactPoint);
    
    void SetNoDamperDownFlagOnWheel(int jointIndex);    // joint corresponds to a wheel
    bool mDamperShouldNotPullDown[4];

    SuspensionJointDriver* mSuspensionJointDrivers[4];
   
    int mWheelToJointIndexMapping[4];
    int* mJointIndexToWheelMapping;

    rmt::Vector mSuspensionRestPointsFromFile[4];
    rmt::Vector mSuspensionRestPoints[4];
    float mSuspensionRestValue;
    float mSuspensionMaxValue;

    void CalculateSuspensionLocationAndVelocity();
    rmt::Vector mSuspensionWorldSpacePoints[4];
    rmt::Vector mSuspensionPointVelocities[4];

    float mWheelBase;  
    float GetWheelBase() {return mWheelBase;}

    float GetMass()const { return mDesignerParams.mDpMass; }

    float GetRestHeightAboveGround();

    void FetchWheelMapping();

    Wheel* mWheels[4];
    void InitWheelsAndLinkSuspensionJointDrivers();

    friend class Wheel;


    //--------------
    // damage states
    //--------------

/*

    here is what damage states have evolved too:

    there will be 3 types of damage states on vehicles:

    1.
    On "user" level vehicles there will be:

    normal
    texture damage (localized front, back, driver side, passenger side)
    flapping joints
    [flapping joints breaking off] <--- this might not be done for this milestone (or ever); I see it as very low priority right now	
    smoke level 1
    smoke level 2
    smoke level 3
    disabled


    2.
    On "ai" level vehicles there will be

    normal
    texture damage (localized front, back, driver side, passenger side)
    smoke level 1
    smoke level 2
    smoke level 3
    disabled


    3.
    On "traffic" there will be 

    normal
    simultaneous BRIEF paricle system Poof! and overall damage texture - disabled.






*/

    enum VehicleDamageType { VDT_UNSET, VDT_USER, VDT_AI, VDT_TRAFFIC };
    //int mDamageType;    // 1, 2, or 3 - see above.
    VehicleDamageType mDamageType;


    bool IsAFlappingJoint(int index);
    bool InitFlappingJoints();    // returns true if least 1 flapping joint
    bool AddFlappingJoint(const char* transJointName, const char* rotJointName, rmt::Vector& axis, rmt::Vector& rotAxis, int count, int* collisionJointIndex);
    
    sim::PhysicsJointInertialEffector* mInertialJointDrivers[4];
    int* mJointIndexToInertialJointDriverMapping;
    // just for easier delete
    sim::PhysicsJointMatrixModifier* mPhysicsJointMatrixModifiers[4];

    int mDoorDJoint;    // recall set to -1 if not present
    int mDoorPJoint;
    int mHoodJoint;
    int mTrunkJoint;

    // logical state of pieces:
    // 0 undamaged
    // 1 textured
    // 2 flapping
    // 3 gone

    int mDoorDDamageLevel;
    int mDoorPDamageLevel;
    int mHoodDamageLevel;
    int mTrunkDamageLevel;



    enum DamageLocation{ dl_hood, dl_trunk, dl_driverside, dl_passengerside };

    DamageLocation TranslateCollisionIntoLocation(sim::Collision& inCollision);

    //void DamageJoint(int joint, int damlevel);
    //void DamageAllJoints();

    //void BreakOffFlappingPiece(int index);

    bool CarOnCarDamageLogic(bool thisIsA, sim::SimState* simStateA,  sim::SimState* simStateB);
    void SwitchOnDamageTypeAndApply(float normalizedMagnitude, sim::Collision& inCollision);

    // =========================================================
    bool mCollidedWithVehicle; // when we are involved in collision with a vehicle 
    void DusitsStunTest( float normalizedMagnitude );

    // store data about the last thing that hit me
    bool mOutOfControl;
    float mNormalizedMagnitudeOfVehicleHit;
    bool mWasHitByVehicle; // when we try to distinguish who hit who, if somebody hit us, this is true
    VehicleType mWasHitByVehicleType;
    rmt::Vector mSwerveNormal; // if somebody hit us, this tells us direction of swerve
    void TestWhoHitWhom( sim::SimState* simA, sim::SimState* simB, float normalizedMagnitude, const sim::Collision & inCollision );
    // =========================================================


    void VisualDamageType1(float percentageDamage, DamageLocation dl);
    void VisualDamageType2(float percentageDamage, DamageLocation dl);
    void VisualDamageType3(float percentageDamage);
    void SyncVisualDamage( float Health ); // Note that this is the health of the car 0.0f -husk, 1.0f -nice and new.

    rmt::Vector mSmokeOffset;
    rmt::Vector& GetSmokeOffset() {return mSmokeOffset;}
    rmt::Vector GetWheel0Offset();
    rmt::Vector GetWheel1Offset();

    void DebugInflictDamageHood();
    void DebugInflictDamageBack();
    void DebugInflictDamageDriverSide();
    void DebugInflictDamagePassengerSide();


    //void TriggerDamage(float amount, int hitJoint);
    float TriggerDamage(float amount, bool clamp = true);  // returns new percentage
    bool IsVehicleDestroyed()const { return mVehicleDestroyed; }

    bool mVehicleDestroyed;
    bool mDontShowBrakeLights;
    
    bool mAlreadyPlayedExplosion;
    
    float mDamageOutResetTimer;
    
    float mNoDamageTimer; // to make a clean distinction between the second last and the last hit
    
        
    float mHitPoints;
    bool mVehicleCanSustainDamage;  // defaults to false
    void SetVehicleCanSustainDamage(bool trueOrFalse) {mVehicleCanSustainDamage = trueOrFalse;}
    float GetVehicleLifePercentage(float testvalue);

    bool mIsADestroyObjective;
    void VehicleIsADestroyObjective(bool b) {mIsADestroyObjective = b;} 


    void ResetDamageState();
 
    void BeefUpHitPointsOnTrafficCarsWhenUserDriving();
    
    //----------
    // sim model
    //----------

    sim::SimStateArticulated* mSimStateArticulated;
    sim::ArticulatedPhysicsObject* mPhObj;
    sim::PhysicsProperties* mPhysicsProperties;

    // new - swap in model for character to jump on top of
    sim::SimStateArticulated* mSimStateArticulatedOutOfCar;

    sim::SimStateArticulated* mSimStateArticulatedInCar;
        
    bool mUsingInCarPhysics;
    void SetInCarSimState();
    void SetOutOfCarSimState();   
    
    //bool mWaitingToSwitchToOutOfCar;
    //float mOutOfCarSwitchTimer; // brutal fucking hack
    
    void InitSimState(sim::SimEnvironment* se);
    void SetupPhysicsProperties();
    
    void InitGroundPlane();
    sim::ManualSimState* mGroundPlaneSimState; 
    sim::WallVolume* mGroundPlaneWallVolume;
    sim::PhysicsProperties* mGroundPlanePhysicsProperties;

    
    void RestTest(void);
    bool SelfRestTest(void);
    bool mAtRestAsFarAsTriggersAreConcerned;
    void ZeroOutXZVelocity();

    bool mCreatedByParkedCarManager;

    void CreatePoseEngine();
    poser::PoseEngine* mPoseEngine;
    RootMatrixDriver* mRootMatrixDriver;

    //void CreatePoseEngineOutOfCar();
    //poser::PoseEngine* mPoseEngineOutOfCar;
    //RootMatrixDriver* mRootMatrixDriverOutOfCar;

  
    float mDragCoeff;   // TODO - do we want regular, quadratic drag?
    void CalculateDragCoeffBasedOnTopSpeed();    

    float mRollingFrictionForce;
    float mTireLateralResistance;
    float mSlipGasModifier;

    float mCollisionLateralResistanceDropFactor;

    //bool mOkToCrashLand;
    //-----------
    // other crap
    //-----------

    bool mBottomedOutThisFrame; // for debug rendering
    bool mWasAirborn;   // used to help decide when to fire a bottomed out event for esan
    float mWasAirbornTimer;
 
    float mBottomOutSpeedMaintenance;
 
    float mStuckOnSideTimer;
    
    bool mDrawWireFrame;    // also debug rendering
    bool mLosingTractionDueToAccel; // for plum

    void SetupRadDebugWatchStuff();
    
    // unfortunately vehicle will now have a dependency on this shyte, but
    // I still think this is the cleanest way to do it.
    RectTriggerVolume* mpTriggerVolume;
    EventLocator* mpEventLocator;
    bool mTriggerActive;
    void InitEventLocator();

    int mDriverInit;

    bool mHijackedByUser;

    // Attach a tDrawable collectible to the vehicle
    // returns true if attached or false if not ( false happens
    // when a collectible is already attached to the vehicle)
    bool AttachCollectible( StatePropCollectible* );
    StatePropCollectible* GetAttachedCollectible();
    // The collectible is detached and free to move around the world under physics control
    void DetachCollectible( const rmt::Vector& velocity, bool explode = true );
    float mForceToDetachCollectible;

    int mCharacterSheetCarIndex;

    //----------------------
    // door opening/closing
    //----------------------
public:
    enum Door
    {
        DOOR_DRIVER,
        DOOR_PASSENGER
    };

    enum DoorAction
    {
        DOORACTION_NONE,
        DOORACTION_OPEN,
        DOORACTION_CLOSE
    };

    // move the door to the given "position'
    // positions are in 0 to 1, DoorAction should be open or close, not none
    void  MoveDoor(Door, DoorAction, float position);

    // do we actually need to open or close the door (test's physics state, current position, etc)
    bool NeedToOpenDoor(Door); // returns false if door is already open
    bool NeedToCloseDoor(Door); // returns false if door is already closed
    bool HasActiveDoor(Door); // is there and active door (i.e. exists, and isn't flapping)

    void UpdateDoorState(void);
    void ReleaseDoors(void);

    void PlayExplosionEffect();

    virtual void AddToSimulation();
    virtual void ApplyForce( const rmt::Vector& direction, float force );

    bool mHasDoors : 1;
    bool mVisibleCharacters : 1;
    bool mIrisTransition : 1;
    bool mAllowSlide : 1;
    bool mHighRoof : 1;
    float mCharacterScale;

    // Calculates the s_ForceToDamage variable when given
    // the number of hitpoints that should be removed when a vehicle explodes
    static void SetPercentDamageFromExplosion( float percent );
    // Calculates the s_ForceToDamagePlayer variable when given
    // the number of hitpoints that should be removed when a vehicle explodes
    static void SetPercentDamageFromExplosionPlayer( float percent );

private:

    float mDesiredDoorPosition[2];
    DoorAction mDesiredDoorAction[2];

    void CalcDoor(unsigned index, unsigned joint, float scale);
    void CalcDoors(void);

    //----------------------

private:
    rmt::Vector mPassengerLocation;
    rmt::Vector mDriverLocation;
    rmt::Vector mExtents;
	bool m_IsSimpleShadow;


    // The number of hitpoints that are removed (via TriggerDamage)
    // 
    static float s_DamageFromExplosion;
    static float s_DamageFromExplosionPlayer;
};


#endif // _VEHICLE_H
