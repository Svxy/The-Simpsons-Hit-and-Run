//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File: hitnrunmanager.h
//
// Description: Hit & Run manager looks after everything to do with the hit & run system
//
// History:     26/03/2003 + Created -- Jesse Cluff
//
//=============================================================================

#ifndef HITNRUNMANAGER_H
#define HITNRUNMANAGER_H

//========================================
// Nested Includes
//========================================
#include <events/eventlistener.h>
#include <constants/breakablesenum.h>
#include <constants/vehicleenum.h>

//========================================
// Forward References
//========================================
class Vehicle;
class CollisionEntityDSG;

//=============================================================================
//
// Synopsis:    Handling events that cause the hit & run value to change and spawning 
//              chase vehicles when value exceeds a given threshold
//
//=============================================================================
class HitnRunManager : public EventListener
{
public:
    static HitnRunManager* GetInstance( void );
    static HitnRunManager* CreateInstance( void );
    static void DestroyInstance( void );

    void Init( void );
    void Destroy( void );
    virtual void HandleEvent( EventEnum id, void* pEventData );

    void Update( int elapsedMS ); // spawns chase vehicles if hit & run value is too high.
    void HUDRender( void ); // Renders HUD data.

    float GetHitnRunValue( void ) const; // What is the current Hit & Run value.
    void AdjustHitnRunValue( float delta ); // Adjust the Hit & Run value.
    void SetHitnRunValue( float value );
    void MaxHitnRunValue();  //sets hitnrun meter to maximum value

    void DisableMeterDecay() { mDecayDisabled = true; }
    void EnableMeterDecay() { mDecayDisabled = false ; }

    bool IsHitnRunActive(){ return mChaseOn; }
    bool IsHitnRunDisabled(){ return mHitnRunDisabled; }

    void ResetState();

    void EnableHitnRun() { ResetState(); mHitnRunDisabled = false; }
    void DisableHitnRun() { ResetState(); mHitnRunDisabled = true; }

    void SetNumChaseCars( int num ) { mNumChaseCars = num; }
    void SetDecayRate( float rate ) { mDecayRatePerSecond = rate; }
    float GetDecayRate() {return mDecayRatePerSecond;}
    void SetDecayRateInside( float rate ) { mDecayRateInsidePerSecond = rate; }
    float GetDecayRateInside() {return mDecayRateInsidePerSecond;}

    bool IsWaitingForReset() { return !mVehicleReset; }

    bool HasntBeenHit( void* ptr );
    void RegisterHit( void* ptr );

    void RegisterVehicleImmunity( Vehicle* v );

    float GetWarningThreshold() { return mWarningThreshold; }

    bool BustingPlayer() { return ( mVehicleResetTimer > 0.0f || mFadeTimer > 0.0f ); }

protected:
    //Prevent wasteful constructor creation.
    HitnRunManager( const HitnRunManager& That );
    HitnRunManager& operator=( const HitnRunManager& That );

    static HitnRunManager* smpHitnRunManager;

private:
    HitnRunManager();
    ~HitnRunManager();

    float mCurrHitnRun;
    float mDecayRatePerSecond;
    float mDecayRateWhileSpawning;
    float mDecayRateInsidePerSecond;
    float mVehicleDestroyedDelta;
    int   mVehicleDestroyedCoins;
    float mVehicleHitDelta;
    float mHitBreakableDelta;
    int   mHitBreakableCoins;
    float mHitMoveableDelta;
    int   mHitMoveableCoins;
    float mHitKrustyGlassDelta;
    int   mHitKrustyGlassCoins;
    float mColaPropDestroyedDelta;
    int   mColaPropDestroyedCoins;
    float mKickNPCDelta;
    int   mKickNPCCoins;
    float mPlayerCarHitNPCDelta;
    int   mPlayerCarHitNPCCoins;
    int   mBustedCoins;
    float mSwitchSkinDelta;
    float mChangeVehicleDelta;
    VehicleEnum::VehicleID mPrevVehicleID;
    //Vehicle* mPrevVehicleHit;
    //CollisionEntityDSG* mPrevMoveableHit;
    void* mPrevHits[16];
    int mLeastRecentlyHit;
    bool mChaseOn;
    bool mSpawnOn;
    float mDecayDelayMS;
    float mDecayDelay;
    float mDecayDelayWhileSpawning;
    int mNumChaseCars;
    bool mHitnRunDisabled;
    bool mDecayDisabled;
    float mCopTicketDistance;
    float mCopTicketDistanceOnFoot;
    float mCopTicketSpeedThreshold;
    float mCopTicketTimeThreshold;
    float mTicketTimer;
    float mLastUpdateValue;
    float mLastHitnRunValue;
    bool mVehicleReset;
    float mVehicleResetTimer;
    float mUnresponsiveTime;
    bool mFadeDone;
    float mFadeTimer;

    tUID* mObjectsThatNeverGiveCoins;
    bool DoesObjectGiveCoins( CollisionEntityDSG* );  //const

    Vehicle* mImmunityVehicle;
    float mWarningThreshold;
    bool mAllowThrob;
};

inline HitnRunManager* GetHitnRunManager() { return( HitnRunManager::GetInstance() ); }

#endif //HITNRUNMANAGER_H
