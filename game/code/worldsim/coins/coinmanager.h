//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File: coinmanager.h
//
// Description: Coin manager looks after everything to do with the collectable coins
//
// History:     29/01/2003 + Created -- James Harrison
//
//=============================================================================

#ifndef COINMANAGER_H
#define COINMANAGER_H

//========================================
// Nested Includes
//========================================
#include <radmath/vector.hpp>
#include <radmath/geometry.hpp>
#include <events/eventlistener.h>
#include <constants/breakablesenum.h>
#include <cheats/cheatinputsystem.h>

//========================================
// Forward References
//========================================
class Vehicle;
class tDrawable;
class tTexture;

//=============================================================================
//
// Synopsis:    Creating and placing coins in the world, picking up coins,
//              losing coins,
//
//=============================================================================
class CoinManager : public EventListener,
                    public ICheatEnteredCallback
{
public:
    CoinManager();
	~CoinManager();

	static CoinManager* GetInstance( void );
	static CoinManager* CreateInstance( void );
    static void DestroyInstance( void );

    void Init( void );
    void Destroy( void );
    void SetCoinDrawable( tDrawable* Drawable );
    virtual void HandleEvent( EventEnum id, void* pEventData );

    void Update( int ElapsedMS ); // Elapsed milliseconds.
    void Render( void ); // Draw coins. Do simple frustum and distance check on them.
    void HUDRender( void ); // Draw any HUD animations.

    int GetBankValue( void ) const; // How much is in the bank.
    void AdjustBankValue( int DeltaCoins ); // Adjust the bank value without any visual SFX.
    void CollectCoins( int Count ); // Collect coins with sound SFX.
    void LoseCoins( int Count, const rmt::Vector* Position = 0 ); // decrease bank value with visual SFX.
    // spawn coins from this position. If in car, the coin actually just go to the player, unless the Force
    //parameter is true.
    void SpawnCoins( int Count, const rmt::Vector& Position, const rmt::Vector* Direction = 0, bool Force = false );
    void SpawnCoins( int Count, const rmt::Vector& Position, float GroundY, const rmt::Vector* Direction = 0, bool Force = false ); // spawn coins and force specific ground value.
	void SpawnInstantCoins(int Count, const rmt::Vector& Position);

    void AddWorldCoin( const rmt::Vector& Position, tUID Sector ); // Place a coin to sit happily in a zone until the zone unloads.

    void SetHUDCoin( int X, int Y, bool IsShowing = true );
    void AddFlyDownCoin(void); // Create an animation of a single coin zipping down from the HUD counter.
	void ClearHUDCoins(void); // Remove all HUD coins.

    bool DrawAfterGui() const { return mDrawAfterGui;}
    void SetDrawAfterGui(bool d) { mDrawAfterGui = d;}

    enum eCoinState
    {
        CS_Inactive,
        CS_InitialSpawning,     // Coins aren't collectable during this time.
        CS_SpawnToCollect,      // Coins spawn and then are collected on first bounce.
        CS_Spawning,            // Still moving, but can be collected.
        CS_Resting,             // Sitting happily.
        CS_RestingIndefinitely, // Sitting happily in the world. I doesn't decay.
        CS_Decaying,            // Spinning away into nothing.
        CS_Collecting,          // Reverse spawning. Attracted to the player.
        CS_Collected,           // Special state held for one frame before the coin heads up into the HUD.
        CS_FlyingToHUD,         // Coin is doing the little fly up animation.
        CS_FlyingFromHUD,       // Coin is doing the little fly down animation.
        CS_NUM_STATES
    };

    tDrawable* GetCoinDrawable( void ) const { return m_pCoinDrawable; }
    const rmt::Sphere& GetCoinBounding( void ) const { return mCoinBounding; }

    virtual void OnCheatEntered( eCheatID cheatID, bool isEnabled );

protected:
    //Prevent wasteful constructor creation.
	CoinManager( const CoinManager& That );
	CoinManager& operator=( const CoinManager& That );

    void OnVehicleDestroyed( Vehicle* DestroyedVehicle );
    struct ActiveCoin;
    void SpawnCoin( ActiveCoin& Coin, const rmt::Vector& Start, float Ground, const rmt::Vector* Direction = 0, bool InstaCollect = false );
    void CheckCollection( ActiveCoin& Coin, const rmt::Vector& AvatarPos, float RangeMultiplier = 1.0f );
    void RemoveWorldCoins( tUID Sector );
    void AddGlint( ActiveCoin& Coin, const rmt::Vector& ToCamera, const rmt::Vector& CoinAxis, const rmt::Vector& CameraRight );

    void UpdateCollecting( ActiveCoin& Coin, const rmt::Vector& AvatarPos );
    void UpdateSpawning(ActiveCoin& Coin, const rmt::Vector& AvatarPos, float DeltaSeconds);
    void UpdateHUDFlying( ActiveCoin& Coin );

    static CoinManager* spCoinManager;

    tDrawable* m_pCoinDrawable;
    rmt::Sphere mCoinBounding;

    struct ActiveCoin
    {
        ActiveCoin() : State( CS_Inactive ) {};
        union
        {
#ifdef RAD_GAMECUBE
            rmt::Vector Velocity;
            tUID Sector;
#else
            struct
            {
                rmt::Vector Velocity;
            };

            struct
            {
                tUID Sector;
                short PersistentObjectID;
            };
#endif
        };
#ifdef RAD_GAMECUBE
        short PersistentObjectID;
#endif
        rmt::Vector Position;
        float HeadingCos;
        float HeadingSin;
        float Age;
        float Ground;
        eCoinState State;
    };

    ActiveCoin* GetInactiveCoin( void );

    ActiveCoin* mActiveCoins;
    short mNumActiveCoins;
    short mNextInactiveCoin;
    short mNumHUDFlying; // So we can early out of the HUD render.
    short mHUDSparkle;

    float mHUDCoinX;
    float mHUDCoinY;
    float mHUDCoinAngle;

    bool mDrawAfterGui;
};

// A little syntactic sugar for getting at this singleton.
inline CoinManager* GetCoinManager() { return( CoinManager::GetInstance() ); }

#endif //COINMANAGER_H
