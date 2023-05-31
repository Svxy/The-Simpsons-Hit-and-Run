//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehicleinit.cpp
//
// Description: init stuff for the vehicle, that is only called once!
//
// History:     Aug 2, 2002 + Created -- gmayer
//
//=============================================================================


//========================================
// System Includes
//========================================

#include <simcommon/skeletoninfo.hpp>
#include <simcommon/simstate.hpp>
#include <simcommon/simstatearticulated.hpp>
#include <simcommon/simulatedobject.hpp>
#include <simphysics/articulatedphysicsobject.hpp>
#include <simphysics/physicsjoint.hpp>
#include <simphysics/physicsobject.hpp>
#include <simcommon/simenvironment.hpp>
#include <simcollision/collisionobject.hpp>
#include <simcollision/collisionvolume.hpp>
#include <mission/charactersheet/charactersheetmanager.h>
#include <simcollision/collisiondisplay.hpp>

#include <raddebug.hpp>
#include <raddebugwatch.hpp>

#include <p3d/anim/drawablepose.hpp>
#include <p3d/shadow.hpp>

#include <string.h>

//========================================
// Project Includes
//========================================

#include <worldsim/redbrick/vehicle.h>
#include <worldsim/worldobject.h>

#include <worldsim/worldphysicsmanager.h>
#include <worldsim/redbrick/geometryvehicle.h>
#include <worldsim/redbrick/rootmatrixdriver.h>
#include <worldsim/redbrick/suspensionjointdriver.h>
#include <worldsim/redbrick/vehicleeventlistener.h>
#include <worldsim/redbrick/wheel.h>
#include <worldsim/redbrick/redbrickcollisionsolveragent.h>

#include <worldsim/redbrick/physicslocomotion.h>
#include <worldsim/redbrick/trafficlocomotion.h>

#include <worldsim/character/charactermanager.h>


#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>

#include <meta/carstartlocator.h>

#include <debug/debuginfo.h>


#include <ai/actionbuttonhandler.h>
#include <ai/actionbuttonmanager.h>

#include <meta/eventlocator.h>
#include <meta/spheretriggervolume.h>
#include <meta/recttriggervolume.h>

#include <worldsim/character/character.h>

#include <gameflow/gameflow.h>

using namespace sim;

// The force required to knock a collectible off
const float MIN_FORCE_DETACH_COLLECTIBLES = 30000.0f;


//=============================================================================
// Vehicle::Vehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Vehicle
//
//=============================================================================
Vehicle::Vehicle() :
    mGeometryVehicle( NULL ),
    mName( NULL ),
    mVehicleID( VehicleEnum::INVALID ),
    mTerrainType( TT_Road),
    mInterior( false ),
    mpEventLocator( NULL ),
    mCharacterSheetCarIndex( -1 ),
    m_IsSimpleShadow( true )
{
    mTranslucent = true;
    mDrawVehicle = true;

    mTransform.Identity();
    
    mVehicleCentralIndex = -1;

    // wtf?    
    mInitialPosition.Set(0.0f, 8.0f, 20.0f);
    mResetFacingRadians = 0.0f;
    
    mVehicleFacing.Set(0.0f, 0.0f, 1.0f);
    mVehicleUp.Set(0.0f, 1.0f, 0.0f);
    
#ifdef RAD_GAMECUBE
    HeapMgr()->PushHeap( GMA_GC_VMM );
#else
    GameMemoryAllocator allocator = GetGameplayManager()->GetCurrentMissionHeap();
    HeapMgr()->PushHeap( allocator );
#endif
    mGeometryVehicle = new GeometryVehicle;
#ifdef RAD_GAMECUBE
    HeapMgr()->PopHeap( GMA_GC_VMM );
#else
    HeapMgr()->PopHeap( allocator );
#endif


    //mPhysicsVehicle = new PhysicsVehicle;
    //mPhysicsVehicle = 0;  test this motherfucker

    // should this be here or in Init?
    // shouldn't really be in this class at all
    // TODO - take this shit out
    //mVehicleRender = new VehicleRender(this);


    mSimStateArticulated = 0;
    mPoseEngine = 0;
    mRootMatrixDriver = 0;
    
    //mPoseEngineOutOfCar = 0;
    //mRootMatrixDriverOutOfCar = 0;
    
    
    mSimStateArticulatedOutOfCar = 0;
    mSimStateArticulatedInCar = 0;

    mGas = 0.0f;
    mLastGas = 0.0f;
    mDeltaGas = 0.0f;
    mBrake = 0.0f;
    mWheelTurnAngle = 0.0f;
    mReverse = 0.0f;
    mEBrake = 0.0f;

    mBrakeTimer = 0.0f;
    mBrakeActingAsReverse = false;

    mGasBrakeDisabled = false;

    mUnmodifiedInputWheelTurnAngle = 0.0f;

    mVelocityCM.Set(0.0f, 0.0f, 0.0f);
    mSpeed = 0.0f;
    mSpeedKmh = 0.0f;
    mLastSpeedKmh = 0.0f;
    mAccelMss = 0.0f;
    mPercentOfTopSpeed = 0.0f;

    mRollingFrictionForce = 2.0f;    // need to tune?  I don't think so
    //mRollingFrictionForce = 1.5f;    // need to tune?  I don't think so

    mRPM = 0.0f;
    mBaseRPM = 500.0f;
    mMaxRpm = 6500.0f;
    
    mRPMUpRate = 10.0f;
    mRPMDownRate = 50.0f;
    //mShiftPointHigh = 3500.0f;
    //mShiftPointLow = 1000.0f;

    //mShiftPointHigh = 4000.0f;
    mShiftPointHigh = 4500.0f;
    //mShiftPointLow = 1500.0f;
    mShiftPointLow = 2000.0f;


    mGear = 0;  // neutral?

    mNumGears = -1; // unset
    mGearRatios = 0;
    mFinalDriveRatio = -1.0f;  // unset

    mSkidLevel = 0.0f;
    mBurnoutLevel = 0.0f;
    mSlipGasModifier = 1.0f;

    int i;
    for(i = 0; i < 4; i++)
    {
        mInertialJointDrivers[i] = 0;
        mPhysicsJointMatrixModifiers[i] = 0;
    }
    mJointIndexToInertialJointDriverMapping = 0;

    mCollisionAreaIndex = -1;

    mDoorDJoint = -1;
    mDoorPJoint = -1;
    mHoodJoint = -1;
    mTrunkJoint = -1;

    mDoorDDamageLevel = 0;
    mDoorPDamageLevel = 0;
    mHoodDamageLevel = 0;
    mTrunkDamageLevel = 0;

    mOkToDrawSelf = true;
    mSteeringWheelsOutOfContact = false;
    mAirBorn = false;
    mWasAirborn = false;
    mWasAirbornTimer = 0.0f;
    mWeebleOn = false;
    mInstabilityOffsetOn = false;

    mSpeedBurstTimer = 0.0f;
    mBuildingUpSpeedBurst = false;
    mDoSpeedBurst = false;
    mFOVToRestore = 1.57f;
    mSpeedBurstTimerHalf = 0.0f;

    //mDamageType = 0;    // 0 is unset - normally 1, 2, or 3
    mDamageType = VDT_UNSET;

    mVehicleDestroyed = false;
    mVehicleCanSustainDamage = true;
    //mVehicleCanSustainDamage = false;

    mIsADestroyObjective = false;

    mHitPoints = 2.0f;
    
    mDamageOutResetTimer = 0.0f;

    mSmokeOffset.Set(0.0f, -0.5f, 1.5f);

//    mHitJoint = 0;	

    mTrafficVehicle = NULL;

    mCollisionLateralResistanceDropFactor = 1.0f;

    mUserDrivingCar = false;

    mDesiredDoorPosition[0] = mDesiredDoorPosition[1] = 0.0f;
    mDesiredDoorAction[0] = mDesiredDoorAction[1]  = DOORACTION_NONE;

    mpTriggerVolume = NULL;

    mpDriver = NULL;

    mDoingRockford = false;
    mDoingWheelie = false;


    strcpy(mDriverName,"phantom");
//    mDriverName[0] = 0;
    mPhantomDriver = false;


    // vehicle event stuff
    mVehicleEventListener = 0;
    
    mDoingJumpBoost = false;
    
    mNoSkid = false;    // only true for frink's hovering vehicles (also the ghost ship)
    mNoFrontSkid = false; // only true for the rocket car

    mBrakeLightsOn = false;
    mReverseLightsOn = false;
    mDontShowBrakeLights = false;   
    
    mCMOffsetSetToOriginal = false;

    mOutOfControl = false;
    mCollidedWithVehicle = false;

    mNormalizedMagnitudeOfVehicleHit = 0.0f;
    mWasHitByVehicle = false;
    mWasHitByVehicleType = VT_LAST;

    mDamperShouldNotPullDown[0] = false;
    mDamperShouldNotPullDown[1] = false;
    mDamperShouldNotPullDown[2] = false;
    mDamperShouldNotPullDown[3] = false;
    

    mNumTurbos = 3; // default to 3 for now
    mSecondsTillNextTurbo = 0.0f;

    mUsingInCarPhysics = true;
    
    //mWaitingToSwitchToOutOfCar = false;
    //mOutOfCarSwitchTimer = 0.0f; // brutal fucking hack
    
    mNoDamageTimer = 0.0f;
    
    mAlreadyPlayedExplosion = false;
    
    mEBrakeTimer = 0.0f;
    
    mWheelTurnAngleInputValue = 0.0f;

    mDrawWireFrame = false;
    mLosingTractionDueToAccel = false; // for plum - just debug output

    mHijackedByUser = false;



    mDesignerParams.mDpGamblingOdds= 1.0f; //Chuck: set the Gambling Odds to 1.0 or 1:1 by default.
    mbPlayerCar = false;

    mOurRestSeatingPosition.Set(0.0f, 0.0f, 0.0f);
    mNPCRestSeatingPosition.Set(0.0f, 0.0f, 0.0f);
    mYAccelForSeatingOffset = 0.0f;
    
    mVelocityCMLag.Set(0.0f, 0.0f, 0.0f);
    mPositionCMLag.Set(0.0f, 0.0f, 0.0f);
    
    // value... fresh from my ass:
    //mBounceLimit = 0.5f;
    //mBounceLimit = 0.25f;
    mBounceLimit = 0.11f;
    
    
    //mMaxBounceDisplacementPerSecond = 1.0f;
    mMaxBounceDisplacementPerSecond = 2.0f;
    
    mBounceAccelThreshold = 1.0f;    // below this value just try and move back to rest
    //mBounceAccelThreshold = 2.0f;    // below this value just try and move back to rest
    

    mForceToDetachCollectible = MIN_FORCE_DETACH_COLLECTIBLES;

    mHasDoors = true;
    mVisibleCharacters = true;
    mIrisTransition = false;
    mAllowSlide = true;
    mHighRoof = false;
    mCharacterScale = 1.0f;


    // Lets set the default to inflict half damage on this vehicle when they explode
    s_DamageFromExplosion = 0.5f;
    // And no damage on player vehicles
    s_DamageFromExplosionPlayer = 0;
    
    mStuckOnSideTimer = 0.0f;
    mAlreadyCalledAutoResetOnSpot = false;
    
    
    mOriginalCMOffset.x = 0.0f;
    mOriginalCMOffset.y = 0.0f;
    mOriginalCMOffset.z = 0.0f;
    
    mBottomOutSpeedMaintenance = 0.0f;

    mTriggerActive = false;
    
    mAtRestAsFarAsTriggersAreConcerned = true;
    
    mLocoSwitchedToPhysicsThisFrame = false;
    
    mCreatedByParkedCarManager = false;
}


//=============================================================================
// Vehicle::Init
//=============================================================================
// Description: Comment
//
// Parameters:  (char* name, int num, SimEnvironment* se, VehicleLocomotionType loco)
//
// Return:      bool 
//
//=============================================================================
bool Vehicle::Init( const char* name, SimEnvironment* se, VehicleLocomotionType loco, VehicleType vt, bool startoutofcar)
{
    if(mbPlayerCar ==true)
    {
        mCharacterSheetCarIndex = GetCharacterSheetManager()->GetCarIndex( name );
    }
    else
    {
        mCharacterSheetCarIndex = -1;
    }

    mDrawVehicle = true;   

    mVehicleType = vt;

    rAssert( mName == NULL );
    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();
    mName = new(gma)char[strlen(name)+1];
    strcpy( mName, name );
    
    CollisionEntityDSG::SetName(mName);    

    mLoco = loco;

    AssignEnumBasedOnName();

    //++++++++++++++++++++
    // the designer params
    //++++++++++++++++++++


        //mDesignerParams.mDpGasScale = 8.0f;     // proportional to mass
        //mDesignerParams.mDpGasScale = 6.0f;     // proportional to mass
        //mDesignerParams.mDpGasScale = 3.0f;     // proportional to mass
        mDesignerParams.mDpGasScale = 4.0f;     // proportional to mass

        mDesignerParams.mDpSlipGasScale = 4.0f;

        mDesignerParams.mDpHighSpeedGasScale = 2.0f;
        //mDesignerParams.mDpGasScaleSpeedThreshold = 0.5f;
        mDesignerParams.mDpGasScaleSpeedThreshold = 1.0f;   // make this the default so it is not used for the time being
        

        mDesignerParams.mDpBrakeScale = 3.0f;   // proportional to mass

        //mDesignerParams.mDpTopSpeedKmh = 300.0f;
        //mDesignerParams.mDpTopSpeedKmh = 250.0f;
        mDesignerParams.mDpTopSpeedKmh = 220.0f;

        mDesignerParams.mDpMass = 1000.0f;      // think of it as kg

        mDesignerParams.mDpMaxWheelTurnAngle = 35.0f;     //  in degrees
        //mDesignerParams.mDpMaxWheelTurnAngle = 30.0f;     //  in degrees
        //mDesignerParams.mDpMaxWheelTurnAngle = 24.0f;     //  in degrees
        //mDesignerParams.mDpHighSpeedSteeringDrop = 0.25f;       // 0.0 to 1.0
        mDesignerParams.mDpHighSpeedSteeringDrop = 0.0f;       // 0.0 to 1.0

        // for wheel
        //mDesignerParams.mDpTireLateralStaticGrip = 2.5f;
        //mDesignerParams.mDpTireLateralStaticGrip = 5.0f;
        mDesignerParams.mDpTireLateralStaticGrip = 4.0f;
        
           
        mDesignerParams.mDpTireLateralResistanceNormal = 110.0f;
        mDesignerParams.mDpTireLateralResistanceSlip = 35.0f;

        mDesignerParams.mDpEBrakeEffect = 0.5f;        

        //mDesignerParams.mDpTireLateralResistanceSlipNoEBrake = 20.0f; // this one's for more out of control driving
        mDesignerParams.mDpTireLateralResistanceSlipNoEBrake = 50.0f; // this one's for more out of control driving
        mDesignerParams.mDpSlipEffectNoEBrake = 0.1f;

        mDesignerParams.mDpCMOffset.x = 0.0f;
        mDesignerParams.mDpCMOffset.y = 0.0f;
        mDesignerParams.mDpCMOffset.z = 0.0f;

        // don't think they need to touch these for now
        mDesignerParams.mDpSuspensionLimit = 0.4f;
        mDesignerParams.mDpSpringk = 0.5f; 
        mDesignerParams.mDpDamperc = 0.2f; 

        mDesignerParams.mDpSuspensionYOffset = 0.0f;

        mDesignerParams.mHitPoints = 2.0f;
        //mDesignerParams.mHitPoints = 0.5f;

        //mDesignerParams.mDpBurnoutRange = 0.2f;
        mDesignerParams.mDpBurnoutRange = 0.3f;

        mDesignerParams.mDpWheelieRange = 0.0f;
        mDesignerParams.mDpWheelieYOffset = 0.0f;
        mDesignerParams.mDpWheelieZOffset = 0.0f;

        mDesignerParams.mDpMaxSpeedBurstTime = 1.0f;
        mDesignerParams.mDpDonutTorque = 10.0f;

        mDesignerParams.mDpWeebleOffset = -1.0f;


    mVehicleState = VS_NORMAL;
    mTireLateralResistance = mDesignerParams.mDpTireLateralResistanceNormal;

    mSteeringInputThreshold = 0.7f;
    mSteeringPreSlope = 0.5f;


    bool localizedDamage;
    //localizedDamage = mGeometryVehicle->Init(name, this, num);
    localizedDamage = mGeometryVehicle->Init(name, this, 0);
    
    // change the logic so that localizedDamage is true if there as at 
    // least one localized damage texture


    if(!localizedDamage)
    {
        //mDamageType = 3;    // traffic
        mDamageType = VDT_TRAFFIC;
    }

    InitSimState(se); 

    InitGroundPlane();

    FetchWheelMapping();
    InitWheelsAndLinkSuspensionJointDrivers();

    
    mGeometryVehicle->InitParticles();
    //if(mVehicleType == VT_USER)
    if(1)// mVehicleType == VT_USER || mVehicleType == VT_AI)  change this to init for all, only draw for USER - that way it is totally safe to switch vehicle type on the fly
    {
        mGeometryVehicle->InitSkidMarks();  
    }



    bool flappingJointPresent = InitFlappingJoints();
    if(flappingJointPresent)
    {
        //if(mDamageType == 3)    
        if(mDamageType == VDT_TRAFFIC)    
        {
            //rAssertMsg(0, "Fink, what are you doing?");
           
        }
        
        // else

        //mDamageType = 1;
        mDamageType = VDT_USER;

    }
    else if(mDamageType == VDT_UNSET)   // still unset
    {
        mDamageType = VDT_AI;
    }

    // should be set for sure at this point
    rAssertMsg(mDamageType != VDT_UNSET, "damage type not set?");

    InitGears();

        //CalculateDragCoeffBasedOnTopSpeed();

    CalculateValuesBasedOnDesignerParams();
        // does this:
        // SetupPhysicsProperties();
        // CalculatedDragCoeffBasedOnTopSpeed
        // SetDesignerParams on Wheels


    CreateLocomotions();
    mLocoSwitchedToPhysicsThisFrame = false;



    // need this call here?
    Reset( false );

    SetupRadDebugWatchStuff();

    // moved here from VehicleCentral
    InitEventLocator();
 
  
    // vehicle event stuff
    mVehicleEventListener = new VehicleEventListener(this);




    //mUsingInCarPhysics
    
    // default at this point in creation is to be using the incar physics.
    //
    // perhaps there should be a paramter to say which, passed into
    
    
    if(startoutofcar)
    {
        SetOutOfCarSimState();
    }

    /*
    if( GetGameFlow()->GetCurrentContext() == CONTEXT_DEMO )
    {
        mWaitingToSwitchToOutOfCar = true;
    }
    else
    {
        mWaitingToSwitchToOutOfCar = false;
    }
    */

    // actually any use in this result?
    // might as well just assert.
    //return result;
    return true;
}



//=============================================================================
// Vehicle::InitEventLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::InitEventLocator()
{
    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();

    mpEventLocator = new(gma)EventLocator();
    mpEventLocator->SetName( mName );
    mpEventLocator->AddRef();
    mpEventLocator->SetEventType( LocatorEvent::CAR_DOOR );
    int id = -1;

    ActionButton::GetInCar* pABHandler = static_cast<ActionButton::GetInCar*>( ActionButton::GetInCar::NewAction( mpEventLocator ) ); 
    rAssert( dynamic_cast<ActionButton::GetInCar*>( pABHandler ) != NULL  );   
    rAssert( pABHandler );
    bool bResult = GetActionButtonManager()->AddAction( pABHandler, id );
    rAssert( bResult );
    
    // this part is done in VehicleCentral::AddVehicleToActiveList
    //pABHandler->SetVehicleId( mNumActiveVehicles );

    mpEventLocator->SetData( id );

    // grab the car bounding box 
    mExtents = GetSimState()->GetCollisionObject()->GetCollisionVolume()->mBoxSize;
    
    // slap down a trigger volume for getting intop car
    const float edge = 1.0f; // how big?  TODO : tunable?
    mpTriggerVolume = new(gma) RectTriggerVolume( mTransform.Row(3), 
                                                                    mTransform.Row(0), 
                                                                    mTransform.Row(1), 
                                                                    mTransform.Row(2), 
                                                                    mExtents.x + edge, 
                                                                    mExtents.y + edge, 
                                                                    mExtents.z + edge);

    // add volume to event trigger
    mpEventLocator->SetNumTriggers( 1, gma ); 
    mpEventLocator->AddTriggerVolume( mpTriggerVolume );
    mpTriggerVolume->SetLocator( mpEventLocator );
    mpTriggerVolume->SetName( "get_in" );

}

//=============================================================================
// Vehicle::AssignEnumBasedOnName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::AssignEnumBasedOnName()
{
    /*
        enum VehicleID
        {
            BART_V = 0,                     // bart_v   0
            APU_V,                          // apu_v    1
            SNAKE_V,                        // snake_v  2
            HOMER_V,                        // homer_v  3
            FAMIL_V,                        // famil_v  4
            GRAMP_V,                        // gramp_v  5
            CLETU_V,                        // cletu_v  6
            WIGGU_V,                        // wiggu_v  7
            KRUSTYKAR_NOTYETIN,             //
            MARGE_V,                        // marge_v  9
            OTTOBUS_NOTYETIN,
            MOESDUFFTRUCK_NOTYETIN,
            SMITH_V,                        // smith_v  12
            FLANDERSMOTORHOME_NOTYETIN,
            MCBAINHUMMER_NOTYETIN,
            ALIEN_NOTYETIN,
            ZOMBI_V,                        // zombi_v  16
            MUNSTERS_NOTYETIN,
            HUMMER2_NOTYETIN,

            CVAN,                           // cVan     19
            COMPACTA,                       // compactA 20
            
            COMIC_V,                        // comic_v  21
            SKINN_V,                        // skinn_v  22



            // some more new ai cars
            CCOLA,                          // 23
            CSEDAN,
            CPOLICE,
            CCELLA,
            CCELLB,
            CCELLC,
            CCELLD,

            // some more new traffic cars
            MINIVANA,                       // 30
            PICKUPA,
            TAXIA,
            SPORTSA,
            SPORTSB,
            SUVA,
            WAGONA,       // 36

            // some more new cars: nov 12, 2002
            HBIKE_V,    // 37
            BURNS_V,    // 38
            HONOR_V,    // 39

            CARMOR,     // 40
            CCURATOR,   // 41
            CHEARS,     // 42
            CKLIMO,     // 43
            CLIMO,      // 44
            CNERD,      // 45

            FRINK_V,    // 46
            
            // some more new cars: dec 18, 2002
            CMILK,      // 47
            CDONUT,     // 48
            BBMAN_V,    // 49
            BOOKB_V,    // 50
            CARHOM_V,   // 51
            ELECT_V,    // 52
            FONE_V,     // 53
            GRAMR_V,    // 54
            MOE_V,      // 55
            MRPLO_V,    // 56
            OTTO_V,     // 57
            PLOWK_V,    // 58
            SCORP_V,    // 59
            WILLI_V,    // 60

            // new traffic cars: Jan 11, 2003
            SEDANA,     // 61
            SEDANB,     // 62
            
            // new Chase cars: Jan 11, 2003
            CBLBART,    // 63
            CCUBE,      // 64
            CDUFF,      // 65
            CNONUP,     // 66

            // new Driver cars: Jan 11, 2003
            LISA_V,     // 67
            KRUST_V,    // 68
                
            // new Traffic cars: Jan 13, 2003
            COFFIN,     // 69
            HALLO,      // 70
            SHIP,       // 71
            WITCHCAR,   // 72

            // new Traffic husk: Feb 10, 2003
            HUSKA,      // 73

            // new Driver cars: Feb 11, 2003
            ATV_V,      // 74
            DUNE_V,     // 75
            HYPE_V,     // 76
            KNIGH_V,    // 77
            MONO_V,     // 78
            OBLIT_V,    // 79
            ROCKE_V,    // 80

            // new Traffic cars: Feb 24, 2003
            AMBUL,      // 81
            BURNSARM,   // 82
            FISHTRUC,   // 83
            GARBAGE,    // 84
            ICECREAM,   // 85
            ISTRUCK,    // 86
            NUCTRUCK,   // 87
            PIZZA,      // 88
            SCHOOLBU,   // 89
            VOTETRUC,   // 90
            
            // new traffic cars: April 2, 2003
            GLASTRUC,   // 91
            CFIRE_V,     // 92
            
            CBONE,  
            REDBRICK,   // 94
            
            NUM_VEHICLES,   
            INVALID
            
        };

    */
    
    if(strcmp(mName, "bart_v") == 0)
    {
        mVehicleID = VehicleEnum::BART_V;
    }    

    if(strcmp(mName, "apu_v") == 0)
    {
        mVehicleID = VehicleEnum::APU_V;
    }    

    if(strcmp(mName, "snake_v") == 0)
        {
        mVehicleID = VehicleEnum::SNAKE_V;
    }    

    if(strcmp(mName, "homer_v") == 0)
    {
        mVehicleID = VehicleEnum::HOMER_V;
    }    

    if(strcmp(mName, "famil_v") == 0)
    {
        mVehicleID = VehicleEnum::FAMIL_V;
    }    

    if(strcmp(mName, "gramp_v") == 0)
    {
        mVehicleID = VehicleEnum::GRAMP_V;
    }    
    
    if(strcmp(mName, "cletu_v") == 0)
    {
        mVehicleID = VehicleEnum::CLETU_V;
    }    
    
    if(strcmp(mName, "wiggu_v") == 0)
    {
        mVehicleID = VehicleEnum::WIGGU_V;
    }    
    
    if(strcmp(mName, "marge_v") == 0)
    {
        mVehicleID = VehicleEnum::MARGE_V;
    }

    if(strcmp(mName, "smith_v") == 0)
    {
        mVehicleID = VehicleEnum::SMITH_V;
    }

    if(strcmp(mName, "zombi_v") == 0)
    {
        mVehicleID = VehicleEnum::ZOMBI_V;
    }    
    
    if(strcmp(mName, "cVan") == 0)
    {
        mVehicleID = VehicleEnum::CVAN;
    }    

    if(strcmp(mName, "compactA") == 0)
    {
        mVehicleID = VehicleEnum::COMPACTA;
    }    

    if(strcmp(mName, "comic_v") == 0)
    {
        mVehicleID = VehicleEnum::COMIC_V;
    }    

    if(strcmp(mName, "skinn_v") == 0)
    {
        mVehicleID = VehicleEnum::SKINN_V;
    }    

    if(strcmp(mName, "cCola") == 0)
    {
        mVehicleID = VehicleEnum::CCOLA;
    }    
    
    if(strcmp(mName, "cSedan") == 0)
    {
        mVehicleID = VehicleEnum::CSEDAN;
    }    

    if(strcmp(mName, "cPolice") == 0)
    {
        mVehicleID = VehicleEnum::CPOLICE;
    }    

    if(strcmp(mName, "cCellA") == 0)
    {
        mVehicleID = VehicleEnum::CCELLA;
    }    
    
    if(strcmp(mName, "cCellB") == 0)
    {
        mVehicleID = VehicleEnum::CCELLB;
    }    

    if(strcmp(mName, "cCellC") == 0)
    {
        mVehicleID = VehicleEnum::CCELLC;
    }    

    if(strcmp(mName, "cCellD") == 0)
    {
        mVehicleID = VehicleEnum::CCELLD;
    }    

    if(strcmp(mName, "minivanA") == 0)
    {
        mVehicleID = VehicleEnum::MINIVANA;
    }    

    if(strcmp(mName, "pickupA") == 0)
    {
        mVehicleID = VehicleEnum::PICKUPA;
    }    

    if(strcmp(mName, "taxiA") == 0)
    {
        mVehicleID = VehicleEnum::TAXIA;
    }    
    
    if(strcmp(mName, "sportsA") == 0)
    {
        mVehicleID = VehicleEnum::SPORTSA;
    }    

    if(strcmp(mName, "sportsB") == 0)
    {
        mVehicleID = VehicleEnum::SPORTSB;
    }    

    if(strcmp(mName, "SUVA") == 0)
    {
        mVehicleID = VehicleEnum::SUVA;
    }    

    if(strcmp(mName, "wagonA") == 0)
    {
        mVehicleID = VehicleEnum::WAGONA;
    }    

    if(strcmp(mName, "hbike_v") == 0)
    {
        mVehicleID = VehicleEnum::HBIKE_V;
        mNoSkid = true;
    }    


    if(strcmp(mName, "burns_v") == 0)
    {
        mVehicleID = VehicleEnum::BURNS_V;
    }    
            
    if(strcmp(mName, "honor_v") == 0)
    {
        mVehicleID = VehicleEnum::HONOR_V;
        mNoFrontSkid = true;
    }    

    if(strcmp(mName, "cArmor") == 0)
    {
        mVehicleID = VehicleEnum::CARMOR;
    }    

    if(strcmp(mName, "cCurator") == 0)
    {
        mVehicleID = VehicleEnum::CCURATOR;
    }    

    if(strcmp(mName, "cHears") == 0)
    {
        mVehicleID = VehicleEnum::CHEARS;
    }    

    if(strcmp(mName, "cKlimo") == 0)
    {
        mVehicleID = VehicleEnum::CKLIMO;
    }

    if(strcmp(mName, "cLimo") == 0)
    {
        mVehicleID = VehicleEnum::CLIMO;
    }

    if(strcmp(mName, "cNerd") == 0)
    {
        mVehicleID = VehicleEnum::CNERD;
    }

    if(strcmp(mName, "frink_v") == 0)
    {
        mVehicleID = VehicleEnum::FRINK_V;
        mNoSkid = true;
    }

    if(strcmp(mName, "cMilk") == 0)
    {
        mVehicleID = VehicleEnum::CMILK;
    }

    if(strcmp(mName, "cDonut") == 0)
    {
        mVehicleID = VehicleEnum::CDONUT;
    }

    if(strcmp(mName, "bbman_v") == 0)
    {
        mVehicleID = VehicleEnum::BBMAN_V;
    }

    if(strcmp(mName, "bookb_v") == 0)
    {
        mVehicleID = VehicleEnum::BOOKB_V;
    }
    
    if(strcmp(mName, "carhom_v") == 0)
    {
        mVehicleID = VehicleEnum::CARHOM_V;
    }
    
    if(strcmp(mName, "elect_v") == 0)
    {
        mVehicleID = VehicleEnum::ELECT_V;
    }

    if(strcmp(mName, "fone_v") == 0)
    {
        mVehicleID = VehicleEnum::FONE_V;
    }

    if(strcmp(mName, "gramR_v") == 0)
    {
        mVehicleID = VehicleEnum::GRAMR_V;
    }
    
    if(strcmp(mName, "moe_v") == 0)
    {
        mVehicleID = VehicleEnum::MOE_V;
    }

    if(strcmp(mName, "mrplo_v") == 0)
    {
        mVehicleID = VehicleEnum::MRPLO_V;
    }

    if(strcmp(mName, "otto_v") == 0)
    {
        mVehicleID = VehicleEnum::OTTO_V;
    }
    
    if(strcmp(mName, "plowk_v") == 0)
    {
        mVehicleID = VehicleEnum::PLOWK_V;
    }
    
    if(strcmp(mName, "scorp_v") == 0)
    {
        mVehicleID = VehicleEnum::SCORP_V;
    }
    
    if(strcmp(mName, "willi_v") == 0)
    {
        mVehicleID = VehicleEnum::WILLI_V;
    }

    if(strcmp(mName, "sedanA") == 0)
    {
        mVehicleID = VehicleEnum::SEDANA;
    }

    if(strcmp(mName, "sedanB") == 0)
    {
        mVehicleID = VehicleEnum::SEDANB;
    }

    if(strcmp(mName, "cBlbart") == 0)
    {
        mVehicleID = VehicleEnum::CBLBART;
    }

    if(strcmp(mName, "cCube") == 0)
    {
        mVehicleID = VehicleEnum::CCUBE;
    }

    if(strcmp(mName, "cDuff") == 0)
    {
        mVehicleID = VehicleEnum::CDUFF;
    }

    if(strcmp(mName, "cNonup") == 0)
    {
        mVehicleID = VehicleEnum::CNONUP;
    }

    if(strcmp(mName, "lisa_v") == 0)
    {
        mVehicleID = VehicleEnum::LISA_V;
    }

    if(strcmp(mName, "krust_v") == 0)
    {
        mVehicleID = VehicleEnum::KRUST_V;
    }
                
    if(strcmp(mName, "coffin") == 0)
    {
        mVehicleID = VehicleEnum::COFFIN;
    }

    if(strcmp(mName, "hallo") == 0)
    {
        mVehicleID = VehicleEnum::HALLO;
    }

    if(strcmp(mName, "ship") == 0)
    {
        mVehicleID = VehicleEnum::SHIP;
        m_IsSimpleShadow = false;
        mNoSkid = true;
    }

    if(strcmp(mName, "witchcar") == 0)
    {
        mVehicleID = VehicleEnum::WITCHCAR;
        mNoSkid = true;
    }

    if(strcmp(mName, "huskA") == 0)
    {
        mVehicleID = VehicleEnum::HUSKA;        
    }

    if(strcmp(mName, "atv_v") == 0)
    {
        mVehicleID = VehicleEnum::ATV_V;
    }

    if(strcmp(mName, "dune_v") == 0)
    {
        mVehicleID = VehicleEnum::DUNE_V;
    }

    if(strcmp(mName, "hype_v") == 0)
    {
        mVehicleID = VehicleEnum::HYPE_V;
    }

    if(strcmp(mName, "knigh_v") == 0)
    {
        mVehicleID = VehicleEnum::KNIGH_V;
    }

    if(strcmp(mName, "mono_v") == 0)
    {
        mVehicleID = VehicleEnum::MONO_V;
        mNoSkid = true;
    }

    if(strcmp(mName, "oblit_v") == 0)
    {
        mVehicleID = VehicleEnum::OBLIT_V;
    }

    if(strcmp(mName, "rocke_v") == 0)
    {
        mVehicleID = VehicleEnum::ROCKE_V;
        mNoFrontSkid = true;
    }

    if(strcmp(mName, "ambul") == 0)
    {
        mVehicleID = VehicleEnum::AMBUL;
    }

    if(strcmp(mName, "burnsarm") == 0)
    {
        mVehicleID = VehicleEnum::BURNSARM;
    }

    if(strcmp(mName, "fishtruc") == 0)
    {
        mVehicleID = VehicleEnum::FISHTRUC;
    }

    if(strcmp(mName, "garbage") == 0)
    {
        mVehicleID = VehicleEnum::GARBAGE;
    }

    if(strcmp(mName, "icecream") == 0)
    {
        mVehicleID = VehicleEnum::ICECREAM;
    }

    if(strcmp(mName, "IStruck") == 0)
    {
        mVehicleID = VehicleEnum::ISTRUCK;
    }

    if(strcmp(mName, "nuctruck") == 0)
    {
        mVehicleID = VehicleEnum::NUCTRUCK;
    }

    if(strcmp(mName, "pizza") == 0)
    {
        mVehicleID = VehicleEnum::PIZZA;
    }

    if(strcmp(mName, "schoolbu") == 0)
    {
        mVehicleID = VehicleEnum::SCHOOLBU;
    }

    if(strcmp(mName, "votetruc") == 0)
    {
        mVehicleID = VehicleEnum::VOTETRUC;
    }
    
    if(strcmp(mName, "glastruc") == 0)
    {
        mVehicleID = VehicleEnum::GLASTRUC;
    }
        
    if(strcmp(mName, "cFire_v") == 0)
    {
        mVehicleID = VehicleEnum::CFIRE_V;
    }
    
    if(strcmp(mName, "cBone") == 0)
    {
        mVehicleID = VehicleEnum::CBONE;
    }
    
    
    // the best for last!
    if(strcmp(mName, "redbrick") == 0)
    {
        mVehicleID = VehicleEnum::REDBRICK;
    }
    
    
}



// call back for debug watcher:
//typedef void (*RADDEBUGWATCH_CALLBACK)( void* userData );
void DebugWatchVehicleTuningCallback(void* userData)
{
    ((Vehicle*)userData)->CalculateValuesBasedOnDesignerParams();
}   


void InflictDamageHoodCallback(void* userData)
{
    ((Vehicle*)userData)->DebugInflictDamageHood();
}

void InflictDamageBackCallback(void* userData)
{
    ((Vehicle*)userData)->DebugInflictDamageBack();
}

void InflictDamageDriverSideCallback(void* userData)
{
    ((Vehicle*)userData)->DebugInflictDamageDriverSide();
}

void InflictDamagePassengerSideCallback(void* userData)
{
    ((Vehicle*)userData)->DebugInflictDamagePassengerSide();
}


//=============================================================================
// Vehicle::SetupRadDebugWatchStuff
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetupRadDebugWatchStuff()
{
    radDbgWatchAddFloat(&mSpeedKmh, "speed kmh", mName, NULL, (void*)this, 0.0f, 1000.0f);   // just want to observe this
    radDbgWatchAddFloat(&mForceToDetachCollectible, "Detach Collectible Force", mName, DebugWatchVehicleTuningCallback, (void*)this, 5000.0f, 50000.0f );

    // Add the static explosion damage variables to the watcher. Make sure to only add them once
    // under group "Vehicle"
    static bool staticVariablesInitialized = false;
    if ( !staticVariablesInitialized)
    {
        radDbgWatchAddFloat(&Vehicle::s_DamageFromExplosion, "Damage from Explosion (other)", "Vehicle", NULL, NULL, 0, 10.0f );
        radDbgWatchAddFloat(&Vehicle::s_DamageFromExplosionPlayer, "Damage from Explosion (player)", "Vehicle", NULL, NULL, 0, 10.0f );
        staticVariablesInitialized = true;
    }

    radDbgWatchAddFloat(&(mDesignerParams.mDpMass), "mass", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 10000.0f );   
    radDbgWatchAddFloat(&(mDesignerParams.mDpGasScale), "gas scale", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 30.0f ); 
    radDbgWatchAddFloat(&(mDesignerParams.mDpSlipGasScale), "slip gas scale", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 30.0f ); 
    
    radDbgWatchAddFloat(&(mDesignerParams.mDpHighSpeedGasScale), "high speed gas scale", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 30.0f ); 
    radDbgWatchAddFloat(&(mDesignerParams.mDpGasScaleSpeedThreshold), "gas scale threshold", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 1.0f ); 
        
    radDbgWatchAddFloat(&(mDesignerParams.mDpBrakeScale), "brake scale", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 30.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpTopSpeedKmh), "top speed kmh", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 300.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpMaxWheelTurnAngle), "max wheel turn angle", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 55.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpHighSpeedSteeringDrop), "high speed steering drop", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 1.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpTireLateralStaticGrip), "tire grip", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 15.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpTireLateralResistanceNormal), "normal steering", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 500.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpTireLateralResistanceSlip), "slip steering", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 500.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpEBrakeEffect), "ebrake effect", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 1.0f );

    radDbgWatchAddFloat(&(mDesignerParams.mDpTireLateralResistanceSlipNoEBrake), "slip steering without ebrake", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 500.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpSlipEffectNoEBrake), "slip effect without ebrake", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 1.0f );

    radDbgWatchAddFloat(&(mDesignerParams.mDpCMOffset.x), "cmoffset x", mName, DebugWatchVehicleTuningCallback, (void*)this, -1.0f, 1.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpCMOffset.y), "cmoffset y", mName, DebugWatchVehicleTuningCallback, (void*)this, -1.0f, 1.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpCMOffset.z), "cmoffset z", mName, DebugWatchVehicleTuningCallback, (void*)this, -1.0f, 1.0f );

    radDbgWatchAddFloat(&(mDesignerParams.mDpSuspensionLimit), "suspension limit", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 1.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpSpringk), "spring k", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 1.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpDamperc), "damper c", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 1.0f );
    
    radDbgWatchAddFloat(&(mDesignerParams.mDpSuspensionYOffset), "suspension Y Offset", mName, DebugWatchVehicleTuningCallback, (void*)this, -1.0f, 1.0f );

    radDbgWatchAddFloat(&(mDesignerParams.mDpBurnoutRange), "burnout range", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 1.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpMaxSpeedBurstTime), "max speed burst time", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 10.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpDonutTorque), "donut torque", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 20.0f );

    radDbgWatchAddFloat(&(mDesignerParams.mDpWeebleOffset), "weeble offset", mName, DebugWatchVehicleTuningCallback, (void*)this, -3.0f, 3.0f );

    radDbgWatchAddFloat(&(mDesignerParams.mDpWheelieRange), "wheelie range", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 1.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpWheelieYOffset), "wheelie Y offset", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, 2.0f );
    radDbgWatchAddFloat(&(mDesignerParams.mDpWheelieZOffset), "wheelie Z offset", mName, DebugWatchVehicleTuningCallback, (void*)this, 0.0f, -2.0f );

    radDbgWatchAddFunction( "Inflict Damage Hood", &InflictDamageHoodCallback, (void*)this, mName );
    radDbgWatchAddFunction( "Inflict Damage Back", &InflictDamageBackCallback, (void*)this, mName );
    radDbgWatchAddFunction( "Inflict Damage Driver Side", &InflictDamageDriverSideCallback, (void*)this, mName );
    radDbgWatchAddFunction( "Inflict Damage Passenger Side", &InflictDamagePassengerSideCallback, (void*)this, mName );

}

//=============================================================================
// Vehicle::~Vehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Vehicle
//
//=============================================================================
Vehicle::~Vehicle()
{
    if(GetGameplayManager()->GetCurrentVehicle() == this)
    {
        GetGameplayManager()->UnregisterVehicleHUDIcon();
    }

    delete mGeometryVehicle;
    
    delete mPhysicsLocomotion;
    delete mTrafficLocomotion;

    int i;
    for(i = 0; i < 4; i++)
    {
        delete mWheels[i];
        mSuspensionJointDrivers[i]->Release();

        // TODO - need to wrap these somehow in case they don't exist?
        if(mInertialJointDrivers[i])
        {
            mInertialJointDrivers[i]->Release();
        }
        if(mPhysicsJointMatrixModifiers[i])
        {
            mPhysicsJointMatrixModifiers[i]->Release();
        }

    }

    delete[] mGearRatios;
    delete[] mJointIndexToWheelMapping;
    if(mJointIndexToInertialJointDriverMapping)
    {
        delete[] mJointIndexToInertialJointDriverMapping;        
    }     
      
    // these moved to a method that can be called from VehicleCentral::RemoveVehicleFromActiveList    
    //RemoveSelfFromCollisionManager();
    //GetWorldPhysicsManager()->FreeCollisionAreaIndex(mCollisionAreaIndex);
    //mCollisionAreaIndex = -1;

    mGroundPlaneWallVolume->Release();
    mGroundPlanePhysicsProperties->Release();
    mPhysicsProperties->Release();
    
    if(mRootMatrixDriver)
    {
        mRootMatrixDriver->Release();
    }
    
    mPoseEngine->Release();
    
    mGroundPlaneSimState->Release();

    //p3d::inventory->SelectSection("Level");
    //p3d::inventory->Remove(mSimStateArticulated->GetCollisionObject());
    //p3d::inventory->Remove(mSimStateArticulated->GetSimulatedObject());


    tRefCounted::Release(mSimStateArticulatedInCar);
    tRefCounted::Release(mSimStateArticulatedOutOfCar);


    int id = mpEventLocator->GetData();    
    GetActionButtonManager()->RemoveActionByIndex( id );
    

    mpEventLocator->Release();

    if(mpDriver)
    {
        // DO NOT remove a pedestrian manually! 
        // We need them for recycling within the level. 
        if( mpDriver->GetRole() != Character::ROLE_PEDESTRIAN )
        {
           GetCharacterManager()->RemoveCharacter(mpDriver);
        }
        tRefCounted::Release(mpDriver);
    }

    GetCharacterManager()->ClearTargetVehicle(this);

    rAssert( mName != NULL );
    delete[] mName;

    if(mVehicleEventListener)
    {
        delete mVehicleEventListener;
    }
}



//=============================================================================
// Vehicle::CreateLocomotions
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::CreateLocomotions()
{    
MEMTRACK_PUSH_GROUP( "Vehicle" );
    // is this vehicle being setup for A or B?  
    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();
    mPhysicsLocomotion = new(gma)PhysicsLocomotion(this);
    mTrafficLocomotion = new(gma)TrafficLocomotion(this);

    //?
    SetLocomotion( mLoco );
MEMTRACK_POP_GROUP( "Vehicle" );
}




//=============================================================================
// Vehicle::InitWheelsAndLinkSuspensionJointDrivers
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::InitWheelsAndLinkSuspensionJointDrivers()
{
MEMTRACK_PUSH_GROUP( "Vehicle" );
    CollisionObject* collObj = mSimStateArticulated->GetCollisionObject();        
    CollisionVolume* collVol = collObj->GetCollisionVolume();
    rAssert(collVol);

    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();

    int i;
    for(i = 0; i < 4; i++)
    {
        CollisionVolume* sub = collVol->GetSubCollisionVolume(mWheelToJointIndexMapping[i], true);  // true because we only care about the local sub volume

        // sub should be of type SphereVolumeType
        rAssert(sub->Type() == SphereVolumeType);
    
        float radius = ((SphereVolume*)sub)->GetRadius();

        mWheels[i] = new(gma)Wheel;

        // now we can init the goddamn wheel
        if(i < 2)   // todo - way to NOT hardcode this for 4 wheels????
        {
            mWheels[i]->Init(this, i, radius, false, true);
        }
        else
        {            
            mWheels[i]->Init(this, i, radius, true, false);
            // try all wheel drive
            //mWheels[i]->Init(this, i, radius, true, true);
        }

        mSuspensionJointDrivers[i] = new(gma)SuspensionJointDriver(mWheels[i], mWheelToJointIndexMapping[i]);

        mSuspensionJointDrivers[i]->AddRef();
        mPoseEngine->AddPoseDriver(1, mSuspensionJointDrivers[i]);
    }
MEMTRACK_POP_GROUP("Vehicle");
}


//=============================================================================
// Vehicle::InitFlappingJoints
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
bool Vehicle::InitFlappingJoints()
{
  
    tPose* p3dPose = mGeometryVehicle->GetP3DPose();
    rAssert(p3dPose);    

    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap(); 
    mJointIndexToInertialJointDriverMapping = new(gma) int[p3dPose->GetNumJoint()];

    // TODO ?
    // ok to use memset?
    memset(mJointIndexToInertialJointDriverMapping, -1, (sizeof(int) * p3dPose->GetNumJoint()) );


    int count = 0;
    
    rmt::Vector axis;
    rmt::Vector rotAxis;

    bool atLeastOneFlappingJointPresent = false;


    //-----------------
    // driver-side door
    //-----------------
    axis.Set(0.0f, 0.0f, -1.0f);
    rotAxis.Set(0.0f, 1.0f, 0.0f);
    if(AddFlappingJoint("DoorDTrans", "DoorDRot", axis, rotAxis, count, &mDoorDJoint))
    {         
        // override default settings

        mInertialJointDrivers[count]->SetSpeedRate(15.0f);
        mInertialJointDrivers[count]->SetAccelRate(180.0f);
        mInertialJointDrivers[count]->SetGravityRate(0.5f);
        mInertialJointDrivers[count]->SetCentrifugalRate(2.0f);

        atLeastOneFlappingJointPresent = true;
    }

    //--------------------
    // passenger-side door
    //--------------------
    count++;
    rotAxis.Set(0.0f, -1.0f, 0.0f);
    if(AddFlappingJoint("DoorPTrans", "DoorPRot", axis, rotAxis, count, &mDoorPJoint))
    {       
        mInertialJointDrivers[count]->SetSpeedRate(15.0f);
        mInertialJointDrivers[count]->SetAccelRate(180.0f);
        mInertialJointDrivers[count]->SetGravityRate(0.5f);
        mInertialJointDrivers[count]->SetCentrifugalRate(2.0f);

        atLeastOneFlappingJointPresent = true;
    }

    //-----
    // hood
    //-----
    count++;
    axis.Set(0.0f, 0.0f, 1.0f);
    rotAxis.Set(-1.0f, 0.0f, 0.0f);
    if(AddFlappingJoint("HoodTrans", "HoodRot", axis, rotAxis, count, &mHoodJoint))
    {        
        mInertialJointDrivers[count]->SetSpeedRate(3.0f);
        mInertialJointDrivers[count]->SetAccelRate(300.0f);
        mInertialJointDrivers[count]->SetGravityRate(1.0f);
        mInertialJointDrivers[count]->SetCentrifugalRate(1.0f);

        atLeastOneFlappingJointPresent = true;
    }

    //------
    // trunk
    //------
    count++;
    axis.Set(0.0f, 0.0f, -1.0f);
    rotAxis.Set(1.0f, 0.0f, 0.0f);
    if(AddFlappingJoint("TrunkTrans", "TrunkRot", axis, rotAxis, count, &mTrunkJoint))
    {
        mInertialJointDrivers[count]->SetSpeedRate(12.0f);
        mInertialJointDrivers[count]->SetAccelRate(180.0f);
        mInertialJointDrivers[count]->SetGravityRate(1.0f);
        mInertialJointDrivers[count]->SetCentrifugalRate(1.0f);

        atLeastOneFlappingJointPresent = true;
    }

    return atLeastOneFlappingJointPresent;

}


//=============================================================================
// Vehicle::AddFlappingJoint
//=============================================================================
// Description: Comment
//
// Parameters:  (const char* transJointName, const char* rotJointName, rmt::Vector& axis, rmt::Vector& rotAxis, int count)
//
// Return:      void 
//
//=============================================================================
bool Vehicle::AddFlappingJoint(const char* transJointName, const char* rotJointName, rmt::Vector& axis, rmt::Vector& rotAxis, int count, int* collisionJointIndex)
{
MEMTRACK_PUSH_GROUP( "Vehicle" );

    //SkeletonInfo* skelInfo = p3d::find<SkeletonInfo>(mName);
    SkeletonInfo* skelInfo = mPhObj->GetSkeletonInfo ();
    rAssert(skelInfo);

    // release debugging:
    if(skelInfo == 0)
    {
        char buffy[64];
        sprintf(buffy, "can't find skelInfo for %s\n", mName);
        rReleaseString(buffy);
    }

    
    tPose* p3dPose = mGeometryVehicle->GetP3DPose();
    rAssert(p3dPose);

    
    int indexTrans;
    int indexRot;

    indexTrans = p3dPose->FindJointIndex(transJointName);
    indexRot = p3dPose->FindJointIndex(rotJointName);

    *collisionJointIndex = indexRot;

    if(indexTrans == -1 || indexRot == -1)
    {
        MEMTRACK_POP_GROUP( "Vehicle" );
        return false;
    }

    #ifdef RAD_DEBUG    // wrap in define?
    tPose::Joint* transJoint = p3dPose->GetJoint(indexTrans);
    tPose::Joint* rotJoint = p3dPose->GetJoint(indexRot);
    rAssert(rotJoint->parent == transJoint);
    #endif

    skelInfo->SetJointAxis(indexTrans, axis, 1.0f);
    skelInfo->SetJointAxis(indexRot, axis, 1.0f);

    skelInfo->SetJointRotAxis(indexRot, rotAxis);

    // add new driver...
    mJointIndexToInertialJointDriverMapping[indexRot] = count;
    
    PhysicsJoint* phizJoint = mPhObj->GetJoint(indexRot);
    rAssert(phizJoint);

    // test
    //phizJoint->SetInvStiffness(0.5f);



    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap(); 
    mInertialJointDrivers[count] = new(gma) PhysicsJointInertialEffector(phizJoint);
    
    mInertialJointDrivers[count]->AddRef();

    // defaults
    // override shortly after this...
    
    mInertialJointDrivers[count]->SetSpeedRate(15.0f);
    //mInertialJointDrivers[count]->SetAccelRate(20.0f);
    mInertialJointDrivers[count]->SetAccelRate(180.0f);
    mInertialJointDrivers[count]->SetGravityRate(1.0f);
    mInertialJointDrivers[count]->SetCentrifugalRate(2.0f);
    
    // new
    // start out disabled
    
    mPhObj->GetJoint(indexRot)->SetInvStiffness(0.0f);
    mPhObj->GetJoint(indexRot)->ResetDeformation();

    mInertialJointDrivers[count]->SetIsEnabled(false); 
    
    // debug
    //mInertialJointDrivers[count]->SetIsEnabled(true); 


    /*
    mInertialJointDrivers[count]->SetSpeedRate(0.0f);
    //mInertialJointDrivers[count]->SetAccelRate(20.0f);
    mInertialJointDrivers[count]->SetAccelRate(0.0f);
    mInertialJointDrivers[count]->SetGravityRate(0.0f);
    mInertialJointDrivers[count]->SetCentrifugalRate(2.0f);
    */


    // TODO - which pose engine pass?
    mPoseEngine->AddPoseDriver(2, mInertialJointDrivers[count]);

    // also need this other jointmatrixmodifier thing...
    mPhysicsJointMatrixModifiers[count] = new(gma) PhysicsJointMatrixModifier(phizJoint);
    
    mPhysicsJointMatrixModifiers[count]->AddRef();

    mPoseEngine->AddPoseDriver(2, mPhysicsJointMatrixModifiers[count]);

    MEMTRACK_POP_GROUP("Vehicle");
    return true;
}



//=============================================================================
// Vehicle::InitGears
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::InitGears()
{
    mNumGears = 6;  // TODO - not sure we actually want designers to worry about this?
                    // maybe, they can just choose number of gears and there will be pre-defined ratios??
                    //
                    // recall: all this gear shit is just for sound and does not affect performance

    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();
    mGearRatios = new(gma) float[mNumGears];
    
    mFinalDriveRatio = 3.42f;

    mGearRatios[0] = 2.97f; // 1st
    //mGearRatios[1] = 2.07f; // 2nd
    mGearRatios[1] = 1.8f; // 2nd
    mGearRatios[2] = 1.43f; // 3rd
    mGearRatios[3] = 1.00f; // 4th
    mGearRatios[4] = 0.84f; // 5th
    mGearRatios[5] = 0.56f; // 6th



/* some stuff fromt trav for a corvette

    Transmission..........6-speed manual
    Final-drive ratio..........3.42:1, limited slip
    Gear..........Ratio..........Mph/1000 rpm..........Max. test speed
    I..........2.97..........7.4..........48 mph (6500 rpm)
    II..........2.07..........10.6..........69 mph (6500 rpm)
    III..........1.43..........15.3..........100 mph (6500 rpm)
    IV..........1.00..........21.9..........143 mph (6500 rpm)
    V..........0.84..........26.1..........168 mph (6450 rpm)
    VI..........0.56..........39.2..........155 mph (4000 rpm)

*/
    

}


//=============================================================================
// Vehicle::SetupPhysicsProperties
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetupPhysicsProperties()
{
    // called when we first create the sim state, as well as when 
    // we set new designer paramters.

    //float volume = mPhObj->GetVolume();
    
    float volume = ((ArticulatedPhysicsObject*)(mSimStateArticulatedInCar->GetSimulatedObject(-1)))->GetVolume();
    
    // TODO - verify this is coming back in the correct units
    
    // can't set mass directly
    float density = mDesignerParams.mDpMass / volume;

    // TODO - which of these do we actually want to allow designers to modify?
    //mPhysicsProperties->SetFrictCoeffCGS(0.8f);
    //mPhysicsProperties->SetRestCoeffCGS(1.05f);
    //mPhysicsProperties->SetTangRestCoeffCGS(0.0f);
    
    // these values slide you along walls a little nicer
    mPhysicsProperties->SetFrictCoeffCGS(0.3f);
    mPhysicsProperties->SetRestCoeffCGS(1.15f);
    mPhysicsProperties->SetTangRestCoeffCGS(-0.6f);
    
    
    //mPhysicsProperties->SetDensityCGS(density / 1000000.0f); // our density is in g / m^3
    mPhysicsProperties->SetDensityCGS(density); // our density is in g / m^3

    //mSimStateArticulated->SetPhysicsProperties(mPhysicsProperties);
    
    mSimStateArticulatedInCar->SetPhysicsProperties(mPhysicsProperties);
    if(mSimStateArticulatedOutOfCar)
    {
        mSimStateArticulatedOutOfCar->SetPhysicsProperties(mPhysicsProperties);
    }

    mPhObj->SetInvTWDissip(0);    
    mPhObj->SetDissipationInternalRate(0);
    mPhObj->SetDissipationDeformationRate(0); //no good Martin

    
    // TODO - where to put this!
    // should i even use it?
    
    // TODO
    // temp hack to fix lack of phizsim

    const rmt::Vector& gravity = GetWorldPhysicsManager()->mSimEnvironment->Gravity();// this or CGS ?

    //float gravity_y = 9.81f;
    float gravity_y = -1.0f * gravity.y;

    //rmt::Vector gravity = SG::phizSim.mSimEnvironment->Gravity();
    //mSuspensionRestValue = mDesignerParams.mDpMass * rmt::Fabs(gravity.y) * 0.25f;
    mSuspensionRestValue = mDesignerParams.mDpMass * gravity_y * 0.25f;

    // new thing aimed at minimizing strage whipping effects on the car when the wheels bottom out
    // and you get insane values calculated for suspension force
    mSuspensionMaxValue = mDesignerParams.mDpMass * gravity_y * 2.5f;   // TODO - find the right value

    
    mCMOffset = mOriginalCMOffset;

    mCMOffset.Add(mDesignerParams.mDpCMOffset);
   
    ((ArticulatedPhysicsObject*)(mSimStateArticulatedInCar->GetSimulatedObject(-1)))->SetExternalCMOffset(mCMOffset);
    
    //mPhObj->SetExternalCMOffset(mCMOffset);
    

}


//=============================================================================
// Vehicle::InitGroundPlane
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::InitGroundPlane()
{
MEMTRACK_PUSH_GROUP( "Vehicle" );

    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();
    
    rmt::Vector p(0.0f, 0.0f, 0.0f);
    rmt::Vector n(0.0f, 1.0f, 0.0f);

    HeapMgr()->PushHeap (gma);

    mGroundPlaneWallVolume = new WallVolume(p, n);
    mGroundPlaneWallVolume->AddRef();

    mGroundPlaneSimState = (sim::ManualSimState*)(SimState::CreateManualSimState(mGroundPlaneWallVolume));
    mGroundPlaneSimState->AddRef(); 

    mGroundPlaneSimState->GetCollisionObject()->SetManualUpdate(true);
    mGroundPlaneSimState->GetCollisionObject()->SetAutoPair(false);
    mGroundPlaneSimState->GetCollisionObject()->SetIsStatic(true);
    
    char buffy[128];
    sprintf(buffy, "%s_groundplane", mName);

    mGroundPlaneSimState->GetCollisionObject()->SetName(buffy);

    mGroundPlaneSimState->mAIRefIndex = this->GetGroundPlaneAIRef();
    mGroundPlaneSimState->mAIRefPointer = (void*)this;
   
    mGroundPlanePhysicsProperties = new PhysicsProperties;
    mGroundPlanePhysicsProperties->AddRef();

    mGroundPlanePhysicsProperties->SetFrictCoeffCGS(0.8f);
    mGroundPlanePhysicsProperties->SetRestCoeffCGS(1.15f);
    mGroundPlanePhysicsProperties->SetTangRestCoeffCGS(0.0f);
   
    mGroundPlaneSimState->SetPhysicsProperties(mGroundPlanePhysicsProperties);
    
    HeapMgr()->PopHeap (gma);

/*


    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();

    rmt::Vector p, n;
    p.Set(0.0f, 0.0f, 0.0f);
    //n.Set(0.0f, 1.0f, 0.0f);
    n.Set(0.0f, 0.0f, 1.0f);

    mGroundPlaneWallVolume = new(gma) WallVolume(p, n);

    mGroundPlaneWallVolume->AddRef();
    
    mGroundPlaneSimState = SimState::CreateSimState(mGroundPlaneWallVolume);
    mGroundPlaneSimState->AddRef(); 
    //mGroundPlaneSimState->GetCollisionObject()->SetIsStatic(true);
    mGroundPlaneSimState->GetCollisionObject()->SetAutoPair(false); // TODO - does not work yet
    // TODO - name?
    
    char buffy[128];
    sprintf(buffy, "%s_groundplane", mName);

    mGroundPlaneSimState->GetCollisionObject()->SetName(buffy);

    mGroundPlaneSimState->mAIRefIndex = this->GetGroundPlaneAIRef();
    mGroundPlaneSimState->mAIRefPointer = (void*)this;
   
    mGroundPlanePhysicsProperties = new(gma) PhysicsProperties;
    mGroundPlanePhysicsProperties->AddRef();

    mGroundPlanePhysicsProperties->SetFrictCoeffCGS(0.8f);
    mGroundPlanePhysicsProperties->SetRestCoeffCGS(1.05f);
    mGroundPlanePhysicsProperties->SetTangRestCoeffCGS(0.0f);
   
    mGroundPlaneSimState->SetPhysicsProperties(mGroundPlanePhysicsProperties);
    
    
*/  
    
    
    
MEMTRACK_POP_GROUP("Vehicle");
}




//=============================================================================
// Vehicle::FetchWheelMapping
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::FetchWheelMapping()
{
    // should have already done these asserts, but just in case things
    // get moved around a bit...

    rAssert(mGeometryVehicle);

    tPose* p3dPose = mGeometryVehicle->GetP3DPose();
    rAssert(p3dPose);

    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();  
    mJointIndexToWheelMapping = new(gma) int[p3dPose->GetNumJoint()];
   
    memset(mJointIndexToWheelMapping, -1, (sizeof(int) * p3dPose->GetNumJoint()) );
    
    //
    // naming convention so far:
    //
    // should have joints named:
    //
    // w0, w1, w2, w3

    char buffy[8];
    memset(buffy, 0, sizeof(buffy));


    int i;
    for(i = 0; i < 4; i++)
    {
        // TODO!
        // safe to use sprintf ??
        sprintf(buffy, "w%d", i);
        mWheelToJointIndexMapping[i] = p3dPose->FindJointIndex(buffy);

        // for convenience we can look up either way....
        mJointIndexToWheelMapping[mWheelToJointIndexMapping[i]] = i;

        tPose::Joint* joint = p3dPose->GetJoint(mWheelToJointIndexMapping[i]);

        // fill mSuspensionRestPoints[4] with the transform row of the joint object space
        // matrix

        mSuspensionRestPointsFromFile[i] = joint->objectMatrix.Row(3);

        mSuspensionRestPoints[i] = mSuspensionRestPointsFromFile[i];

        mSuspensionRestPoints[i].y += mDesignerParams.mDpSuspensionYOffset;

        // used to be other stuff in here for radius, and  Wheels, and suspensionjointdrivers

    }

    // passenger/driver locations

    int passengerIndex = p3dPose->FindJointIndex( "pl" ); 
    int driverIndex = p3dPose->FindJointIndex( "dl" ); 

    if(passengerIndex != -1)
    {
        tPose::Joint* joint = p3dPose->GetJoint( passengerIndex );
        mPassengerLocation = joint->objectMatrix.Row(3);
    }
    else
    {
        mPassengerLocation.Set( 0.5f, -0.6f, 0.01f );
    }

    if(driverIndex != -1)
    {
        tPose::Joint* joint = p3dPose->GetJoint( driverIndex );
        mDriverLocation = joint->objectMatrix.Row(3);
    }
    else
    {
        mDriverLocation.Set( -0.5f, -0.6f, 0.01f );
    }
    
      
    mWheelBase = mSuspensionRestPoints[3].z - mSuspensionRestPoints[1].z;
        
    // smoke location

    //int smokeIndex = p3dPose->FindJointIndex("smoke");
    int smokeIndex = p3dPose->FindJointIndex("sl");
    tPose::Joint* joint = p3dPose->GetJoint(smokeIndex);
    if (joint)
    {
        mSmokeOffset = joint->objectMatrix.Row(3);
    }

}


//=============================================================================
// Vehicle::GetWheel0Offset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      rmt
//
//=============================================================================
rmt::Vector Vehicle::GetWheel0Offset()
{
    // this is not accurate as wheel is lurching around but should be close enough for smoke effects?
    rmt::Vector temp = mSuspensionRestPoints[0];
    temp.y -= mWheels[0]->mRadius;
    return temp;   
}


//=============================================================================
// Vehicle::GetWheel1Offset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      rmt
//
//=============================================================================
rmt::Vector Vehicle::GetWheel1Offset()
{
    rmt::Vector temp = mSuspensionRestPoints[1];
    temp.y -= mWheels[1]->mRadius;
    return temp;   
}

//=============================================================================
// Vehicle::InitSimState
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::InitSimState(SimEnvironment* se)
{
MEMTRACK_PUSH_GROUP( "Vehicle" );
    CreatePoseEngine();
    rAssert(mPoseEngine);

    // The section stuff here is a hack.
    // Tracked to ATG as bug 1259.
    //
    p3d::inventory->PushSection ();
    p3d::inventory->AddSection (SKELCACHE);
    p3d::inventory->SelectSection (SKELCACHE);
    //mSimStateArticulated  = SimStateArticulated::CreateSimStateArticulated(mName, mPoseEngine->GetPose(), SimStateAttribute_Default, NULL);
    
    SimStateArticulated* newSimState = SimStateArticulated::CreateSimStateArticulated(mName, mPoseEngine->GetPose(), SimStateAttribute_Default, NULL);
    rAssert( mSimStateArticulatedInCar == NULL );
    tRefCounted::Assign( mSimStateArticulatedInCar, newSimState );
    rAssert( mSimStateArticulatedInCar != NULL );


    mSimStateArticulatedInCar->mAIRefIndex = PhysicsAIRef::redBrickVehicle;

    mSimStateArticulatedInCar->mAIRefPointer = (void*)this;


    ((ArticulatedPhysicsObject*)(mSimStateArticulatedInCar->GetSimulatedObject(-1)))->SetSimEnvironment(se);    




    // new test:    
    char buffy[128];
    sprintf(buffy, "%sBV", mName);
    rAssert( mSimStateArticulatedOutOfCar == NULL );
    mSimStateArticulatedOutOfCar = SimStateArticulated::CreateSimStateArticulated(buffy, mPoseEngine->GetPose(), SimStateAttribute_Default, NULL);

    if(mSimStateArticulatedOutOfCar)
    {

        mSimStateArticulatedOutOfCar->AddRef();
        mSimStateArticulatedOutOfCar->mAIRefIndex = PhysicsAIRef::redBrickVehicle;

        mSimStateArticulatedOutOfCar->mAIRefPointer = (void*)this;
        ((ArticulatedPhysicsObject*)(mSimStateArticulatedOutOfCar->GetSimulatedObject(-1)))->SetSimEnvironment(se);    
        
    }
    
    
    p3d::inventory->PopSection ();

    // finish initialization with this... then switch back
    mSimStateArticulated = mSimStateArticulatedInCar;






    mPhObj = (ArticulatedPhysicsObject*)(mSimStateArticulated->GetSimulatedObject(-1));
    
    // fetch once only ever    
    // seven fucking days
    //mOriginalCMOffset = mPhObj->GetExternalCMOffset();
    
    mOriginalCMOffset.x = 0.0f;
    mOriginalCMOffset.y = 0.0f;
    mOriginalCMOffset.z = 0.0f;
      


    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();
    mPhysicsProperties = new(gma) PhysicsProperties; // TODO - who owns - ie. who is responsible to delete

    mPhysicsProperties->AddRef();

/*
    for (int i=0; i<mPhObj->NumSimJoints(); i++)
    {
        PhysicsJoint* joint = mPhObj->GetSimJoint(i);
        P3DASSERT(joint);
        mPoseEngine->AddPoseDriver(2, new PhysicsJointMatrixModifier(joint));
    }

    sim::PhysicsJointMatrixModifier mPhysicsJointMatrixModifiers[4];
*/
MEMTRACK_POP_GROUP( "Vehicle" );
}


//=============================================================================
// Vehicle::CreatePoseEngineOutOfCar
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
/*
void Vehicle::CreatePoseEngineOutOfCar()
{
MEMTRACK_PUSH_GROUP( "Vehicle" );
    // TODO - wtf?
    //const int PoseEngineSimPass = 1;
    const int PoseEngineSimPass = 2;

    rAssert(mGeometryVehicle);

    

    tPose* p3dPose = mGeometryVehicle->GetP3DPose();
    rAssert(p3dPose);

    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();
    mPoseEngine = new(gma) poser::PoseEngine(p3dPose, PoseEngineSimPass+1, p3dPose->GetNumJoint());
    mRootMatrixDriver = new(gma) RootMatrixDriver(&mTransform);

    mPoseEngine->AddRef();    
    mRootMatrixDriver->AddRef();
    mPoseEngine->AddPoseDriver(1, mRootMatrixDriver);   // 0 is the pass

MEMTRACK_POP_GROUP();

}
*/
//=============================================================================
// Vehicle::CreatePoseEngine
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::CreatePoseEngine()
{
MEMTRACK_PUSH_GROUP( "Vehicle" );
    // TODO - wtf?
    //const int PoseEngineSimPass = 1;
    const int PoseEngineSimPass = 2;

    rAssert(mGeometryVehicle);

    

    tPose* p3dPose = mGeometryVehicle->GetP3DPose();
    rAssert(p3dPose);

    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();
    mPoseEngine = new(gma) poser::PoseEngine(p3dPose, PoseEngineSimPass+1, p3dPose->GetNumJoint());
    mRootMatrixDriver = new(gma) RootMatrixDriver(&mTransform);

    mPoseEngine->AddRef();    
    mRootMatrixDriver->AddRef();
    //mPoseEngine->AddPoseDriver(1, mRootMatrixDriver);   // 0 is the pass
    mPoseEngine->AddPoseDriver(0, mRootMatrixDriver);   // 0 is the pass

MEMTRACK_POP_GROUP("Vehicle");
}
