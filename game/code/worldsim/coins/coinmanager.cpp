//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File:        CoinManager.cpp
//
// Description: Implementation of class CoinManager
//
// History:     29/1/2003 + Created -- James Harrison
//
//=============================================================================

//========================================
// System Includes
//========================================

#include <radmath/matrix.hpp>
#include <radmath/random.hpp>
#include <p3d/entity.hpp>
#include <p3d/view.hpp>
#include <p3d/camera.hpp>
#include <p3d/matrixstack.hpp>
#include <pddi/pddi.hpp>

//#include <math.h> //need this if we want extra accurate coin paths.

//========================================
// Project Includes
//========================================
#include <gameflow/gameflow.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/coins/sparkle.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/redbrick/vehicle.h>
#include <memory/srrmemory.h>
#include <events/eventmanager.h>
#include <render/intersectmanager/intersectmanager.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/worldrenderlayer.h>
#include <data/persistentworldmanager.h>
#include <mission/gameplaymanager.h>
#include <mission/charactersheet/charactersheetmanager.h>

/* Watcher stuff */
#ifndef RAD_RELEASE
#include <raddebug.hpp>
#include <raddebugwatch.hpp>

static float COIN_HOVER = 0.5f; // How far off the ground will the coin hover.
#if !defined( RAD_PS2 ) && defined( RAD_RELEASE )
static const float FRAME_RATIO = 1.0f / 60.0f;
//static float COIN_DRAG = 0.9848f; // .4 to the power of 1/60 of a second.
#elif defined( RAD_RELEASE )
static const float FRAME_RATIO = 1.0f / 30.0f;
//static float COIN_DRAG = 0.9700f; // .4 to the power of 1/30 of a second.
#else
static const float FRAME_RATIO = 1.0f / 15.0f;
//static float COIN_DRAG = 0.9407f; // .4 to the power of 1/15 of a second.
#endif
static float COIN_SPAWN_VELOCITY = 9.5f;// * FRAME_RATIO;
static float COIN_EXTRA_VERTICAL = 1.1f;// * FRAME_RATIO;
// Use these values if you want accurate coins.
static float GRAVITY = 21.0f; // * FRAME_RATIO;
static float COIN_DRAG = 0.1f;

// Use these if you want faster (but less accurate coins.
//static float GRAVIYT = 0.9f * FRAME_RATIO;

static float LIFE_TIME = 5.0f; // How many seconds the coin lives for.
static float DECAY_TIME = 5.0f; // How long does it take to disappear.
#define I_DECAY_TIME ( 1.0f / DECAY_TIME ) // Instead of doing a divide.
static float DECAY_EXTRA_SPIN = rmt::PI * 8.0f; // How much extra spin per frame do we want.
static float SPIN_MULTIPLIER = 3.0f;
static float RANGE = 2.25f; // Coin collection radius.
#define INNER_RANGE ( RANGE * ( 4.0f / 3.0f ) )
static float COLLECT_TIME = 1.0f; // How long does it take a coin to suck into the player.
#define I_COLLECT_TIME ( 1.0f / COLLECT_TIME )
static float POP_SCALE = 0.4f; // How much pop up does the coin have when it is collected.
static float BOUNCE_DAMPENING = 0.6f; // How much energy does the coin retain when it hits the ground.

static float FLYING_TIME = 0.25f; // How long does it take to fly up to the HUD.
#define I_FLYING_TIME ( 1.0f / FLYING_TIME )
static float MAX_VISIBLITY = 60.0f; // How far away is the coin.
static float COUNTER_X = 0.45f;  // Where is the destination we are flying the coin up to.
static float COUNTER_Y = 0.45f;  // Where is the destination we are flying the coin up to.
#ifdef RAD_XBOX
static float SPARKLE_FLOAT = 0.025f; // How much vertical velocity for the sparkles per frame.
#else
static float SPARKLE_FLOAT = 0.05f;
#endif
static float IN_CAR_RANGE_MULTIPLIER = 2.25f;
static int dbg_CoinsDrawn = 0;
static int dbg_MaxCoins = 0;

#else /* Same as the variables available to the watcher, but const so the compiler can optimize them. */
const static float COIN_HOVER = 0.5f; // How far off the ground will the coin hover.
// Use this for more accurate coins.
//static const float COIN_SPAWN_VELOCITY = 4.0f;
//static const float COIN_EXTRA_VERTICAL = 2.0f
#if !defined( RAD_PS2 ) && defined ( RAD_RELEASE )
static const float FRAME_RATIO = 1.0f / 60.0f;
//static const float COIN_DRAG = 0.9848f; // .4 to the power of 1/60 of a second.
#elif defined( RAD_RELEASE )
static const float FRAME_RATIO = 1.0f / 30.0f;
//static const float COIN_DRAG = 0.9700f; // .4 to the power of 1/30 of a second.
#else
static const float FRAME_RATIO = 1.0f / 15.0f;
//static const float COIN_DRAG = 0.9407f; // .4 to the power of 1/15 of a second.
#endif
static const float COIN_SPAWN_VELOCITY = 9.5f;// * FRAME_RATIO;
static const float COIN_EXTRA_VERTICAL = 1.1f;// * FRAME_RATIO;
static const float GRAVITY = 21.0f;
static const float COIN_DRAG = 0.1f;
//static const float GRAVITY = 0.9f * FRAME_RATIO;
static const float LIFE_TIME = 11.0f; // How many seconds the coin lives for.
static const float DECAY_TIME = 5.0f; // How long does it take to disappear.
static const float I_DECAY_TIME = 1.0f / DECAY_TIME; // Instead of doing a divide.
static const float DECAY_EXTRA_SPIN = rmt::PI * 8.0f; // How much extra spin per frame do we want.
static const float SPIN_MULTIPLIER = 3.0f;
static const float RANGE = 2.25f; // Coin collection radius.
static const float INNER_RANGE = RANGE * ( 4.0f / 3.0f );
static const float COLLECT_TIME = 1.0f; // How long does it take a coin to suck into the player.
static const float I_COLLECT_TIME = 1.0f / COLLECT_TIME;
static const float POP_SCALE = 0.4f; // How much pop up does the coin have when it is collected.
// Use these values if you want accurate coins.
/*
static const float COIN_DRAG = 0.5f;
static const float GRAVITY = 0.9f;
*/
static const float BOUNCE_DAMPENING = 0.6f; // How much energy does the coin retain when it hits the ground.
static const float FLYING_TIME = 0.25f; // How long does it take to fly up to the HUD.
static const float I_FLYING_TIME = 1.0f / FLYING_TIME;
static const float MAX_VISIBLITY = 60.0f; // How far away is the coin.
static const float COUNTER_X = 0.25f;  // Where is the destination we are flying the coin up to.
static const float COUNTER_Y = 0.4f;  // Where is the destination we are flying the coin up to.
#ifdef RAD_XBOX
static const float SPARKLE_FLOAT = 0.025f;
#else
static const float SPARKLE_FLOAT = 0.05f;
#endif
static const float IN_CAR_RANGE_MULTIPLIER = 2.25f;
#endif

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
CoinManager* CoinManager::spCoinManager = 0;

static const int NUM_COINS = 200;
static const int NUM_SPARKLES = 120; // Although we seldom use all these sparkles we do when the vehicle explodes.
#ifdef RAD_XBOX
static int Sparkle_Rate = 6;
static const int Glint_Rate = 4;
static const int SPARKLE_RATE_TOGGLE = Sparkle_Rate ^ ( Sparkle_Rate + 2 );
static const int HUD_SPARKLE_RATE = 60;
#else
static int Sparkle_Rate = 3;
static const int Glint_Rate = 2;
static const int SPARKLE_RATE_TOGGLE = Sparkle_Rate ^ ( Sparkle_Rate + 1 );
static const int HUD_SPARKLE_RATE = 30;
#endif
static int DoSparkle = 0;
static int DoGlint = 0;
static int GlintDelay = 0;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

CoinManager* CoinManager::CreateInstance( void )
{
	rAssertMsg( spCoinManager == 0, "CoinManager already created.\n" );
    HeapManager::GetInstance()->PushHeap( GMA_PERSISTENT );
	spCoinManager = new CoinManager;
    rAssert( spCoinManager );
    HeapManager::GetInstance()->PopHeap( GMA_PERSISTENT );
    return CoinManager::GetInstance();
}

CoinManager* CoinManager::GetInstance( void )
{
	rAssertMsg( spCoinManager != 0, "CoinManager has not been created yet.\n" );
	return spCoinManager;
}

void CoinManager::DestroyInstance( void )
{
	rAssertMsg( spCoinManager != 0, "CoinManager has not been created.\n" );
	delete spCoinManager;
    spCoinManager = 0;
}

//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
CoinManager::CoinManager() :
    m_pCoinDrawable( 0 ),
    mActiveCoins( NULL),
    mNumActiveCoins( 0 ),
    mNextInactiveCoin( 0 ),
    mNumHUDFlying( 0 ),
    mHUDSparkle( 0 ),
    mHUDCoinX( -10.0f ),
    mHUDCoinY( -10.0f ),
    mHUDCoinAngle( 0.0f ),
    mDrawAfterGui(false)
{   
#ifndef RAD_RELEASE
    radDbgWatchAddFloat( &COIN_SPAWN_VELOCITY, "Spawn - Initial velocity", "Coins", 0, 0, 0.01f, 5.0f );
    radDbgWatchAddFloat( &COIN_EXTRA_VERTICAL, "Spawn - Initial extra vertical", "Coins", 0, 0, 0.0f, 2.5f );
    radDbgWatchAddFloat( &COIN_DRAG, "Spawn - Movement drag", "Coins", 0, 0, 0.1f, 1.0f );
    radDbgWatchAddFloat( &GRAVITY, "Spawn - Gravity", "Coins", 0, 0, 0.1f, 2.0f );
    radDbgWatchAddFloat( &BOUNCE_DAMPENING, "Spawn - Bounce energy dampening", "Coins", 0, 0, 0.0f, 2.0f );
    radDbgWatchAddFloat( &COIN_HOVER, "Spawn - Ground hover", "Coins", 0, 0, 0.0f, 2.0f );
    radDbgWatchAddFloat( &LIFE_TIME, "Duration - Life", "Coins", 0, 0, 1.0f, 30.0f );
    radDbgWatchAddFloat( &DECAY_TIME, "Duration - Decay", "Coins", 0, 0, 1.0f, 10.0f );
    radDbgWatchAddFloat( &RANGE, "Collection - Radius", "Coins", 0, 0, 0.5f, 10.0f );
    radDbgWatchAddFloat( &IN_CAR_RANGE_MULTIPLIER, "Collection - In car multiplier", "Coins", 0, 0, 0.0f, 5.0f );
    radDbgWatchAddFloat( &DECAY_EXTRA_SPIN, "Animation - Extra Decay spin", "Coins", 0, 0, 0.0f, 50.0f );
    radDbgWatchAddFloat( &SPIN_MULTIPLIER, "Animation - Spin multiplier", "Coins", 0, 0, 0.01f, 5.0f );
    radDbgWatchAddFloat( &COLLECT_TIME, "Animation - Collection duration", "Coins", 0, 0, 0.1f, 5.0f );
    radDbgWatchAddFloat( &POP_SCALE, "Animation - Collection pop up", "Coins", 0, 0, 0.1f, 1.0f );
    radDbgWatchAddFloat( &FLYING_TIME, "Animation - Fly to HUD duration", "Coins", 0, 0, 0.1f, 1.0f );
    radDbgWatchAddFloat( &COUNTER_X, "Animation - HUD X position", "Coins", 0, 0, -0.7f, 0.7f );
    radDbgWatchAddFloat( &COUNTER_Y, "Animation - HUD Y position", "Coins", 0, 0, -0.7f, 0.7f );
    radDbgWatchAddFloat( &SPARKLE_FLOAT, "Animation - Sparkle float", "Coins", 0, 0, 0.0f, 0.5f );
    radDbgWatchAddFloat( &MAX_VISIBLITY, "Visibility culling distance", "Coins", 0, 0, 10.0f, 100.0f );
    radDbgWatchAddShort( &mNumActiveCoins, "Active coin count", "Coins", 0, 0, 0, NUM_COINS, true );
    radDbgWatchAddInt( &dbg_CoinsDrawn, "Drawn coin count", "Coins", 0, 0, 0, NUM_COINS, true );
    radDbgWatchAddInt( &dbg_MaxCoins, "Max active coins", "Coins", 0, 0, 0, NUM_COINS, true );
#endif
}

//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
CoinManager::~CoinManager()
{
#ifndef RAD_RELEASE
    radDbgWatchDelete( &COIN_SPAWN_VELOCITY );
    radDbgWatchDelete( &COIN_EXTRA_VERTICAL );
    radDbgWatchDelete( &COIN_DRAG );
    radDbgWatchDelete( &GRAVITY );
    radDbgWatchDelete( &BOUNCE_DAMPENING );
    radDbgWatchDelete( &COIN_HOVER );
    radDbgWatchDelete( &LIFE_TIME );
    radDbgWatchDelete( &DECAY_TIME );
    radDbgWatchDelete( &RANGE );
    radDbgWatchDelete( &DECAY_EXTRA_SPIN );
    radDbgWatchDelete( &SPIN_MULTIPLIER );
    radDbgWatchDelete( &COLLECT_TIME );
    radDbgWatchDelete( &POP_SCALE );
    radDbgWatchDelete( &FLYING_TIME );
    radDbgWatchDelete( &COUNTER_X );
    radDbgWatchDelete( &COUNTER_Y );
    radDbgWatchDelete( &SPARKLE_FLOAT );
    radDbgWatchDelete( &MAX_VISIBLITY );
    radDbgWatchDelete( &mNumActiveCoins );
    radDbgWatchDelete( &dbg_CoinsDrawn );
    radDbgWatchDelete( &dbg_MaxCoins );
#endif
    tRefCounted::Release( m_pCoinDrawable );
    Destroy( );
}

//==============================================================================
// Description: Destruction method for all transient data.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
void CoinManager::Destroy( void )
{
    SetCoinDrawable( NULL );

    if( mActiveCoins != NULL )
    {
        GetCheatInputSystem()->UnregisterCallback( this );

        GetEventManager()->RemoveAll( this );

        delete [] mActiveCoins;
        mActiveCoins = NULL;
    }
	mNumHUDFlying = 0;
	mNumActiveCoins = 0;
}

/*==============================================================================
Description:    This will be called set up as the game session begins.

Parameters:     ( void )

Return:         void 

=============================================================================*/
void CoinManager::Init( void )
{
    #ifdef RAD_GAMECUBE
        HeapManager::GetInstance()->PushHeap( GMA_GC_VMM );
    #else
        HeapManager::GetInstance()->PushHeap( GMA_LEVEL_ZONE );
    #endif
    mActiveCoins = new ActiveCoin[ NUM_COINS ];
    rAssert( mActiveCoins );
    #ifdef RAD_GAMECUBE
        HeapManager::GetInstance()->PopHeap( GMA_GC_VMM );
    #else
        HeapManager::GetInstance()->PopHeap( GMA_LEVEL_ZONE );
    #endif
    mNextInactiveCoin = 0;
    mNumActiveCoins = 0;
    mNumHUDFlying = 0;
    DoSparkle = 0;
    DoGlint = 0;
#ifndef RAD_RELEASE
    dbg_MaxCoins = 0;
#endif

    GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED );
    GetEventManager()->AddListener( this, EVENT_DUMP_DYNA_SECTION );

    GetCheatInputSystem()->RegisterCallback( this );
}

void CoinManager::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
    case EVENT_VEHICLE_DESTROYED:
        {
            Vehicle* vehicle = reinterpret_cast<Vehicle*>( pEventData );
            OnVehicleDestroyed( vehicle );
        }
        break;
    case EVENT_DUMP_DYNA_SECTION:
        {
            RemoveWorldCoins( reinterpret_cast<tName*>( pEventData )->GetUID() );
        }
        break;
    default:
        {
            break;
        }
    }
}

/*=============================================================================
Go nosing through he active coin array looking for an inactive coin. Update
the next inactive coin index while we are at it to speed up the search.
=============================================================================*/
CoinManager::ActiveCoin* CoinManager::GetInactiveCoin( void )
{
    if( mNumActiveCoins >= NUM_COINS )
    {
        return 0;
    }
    for( int i = 0; i < NUM_COINS; ++i )
    {
        ActiveCoin* c = &(mActiveCoins[ mNextInactiveCoin ]);
        mNextInactiveCoin = ( mNextInactiveCoin + 1 ) % NUM_COINS;
        if( c->State == CS_Inactive )
        {
			c->HeadingCos = 0.0f;
			c->HeadingSin = 1.0f;
            return c;
        }
    }
    return 0;
}

/*=============================================================================
Spawn count number of coins at position. You can optionally override the
groundY position (handy for exploding wasps), or it takes the Y value form
the position.
=============================================================================*/
void CoinManager::SpawnCoins( int Count, const rmt::Vector& Position, const rmt::Vector* Direction, bool Force )
{
    SpawnCoins( Count, Position, Position.y, Direction, Force );
}

void CoinManager::SpawnCoins( int Count, const rmt::Vector& Position, float GroundY, const rmt::Vector* Direction, bool Force )
{
    rAssert( Count >= 0 );
    if( Count <= 0  )
    {
        return;
    }


    // Check for a valid avatar pointer
    rTuneAssert( GetAvatarManager() != NULL );
    rTuneAssert( GetAvatarManager()->GetAvatarForPlayer(0) != NULL );
    bool instaCollect = !Force && GetAvatarManager()->GetAvatarForPlayer( 0 )->IsInCar();

    for( int i = 0; i < Count; ++i )
    {
        ActiveCoin* c = GetInactiveCoin();
        if( !c )
        {
            CollectCoins( Count - i );
            break;
        }
        SpawnCoin( *c, Position, GroundY + COIN_HOVER, Direction, instaCollect );
    }
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_LEVEL_ZONE );
    #endif
    GetEventManager()->TriggerEvent( EVENT_SPAWNED_COINS, reinterpret_cast<void*>( Count ) );
    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_LEVEL_ZONE );
    #endif

}

/*=============================================================================
This spawns a coin which is immediately collected by the player. Used for giving
the player a coin reward and you want to make sure they don't miss it.
=============================================================================*/
void CoinManager::SpawnInstantCoins(int Count, const rmt::Vector& Position)
{
    rAssert( Count >= 0 );
    if( Count <= 0  )
    {
        return;
    }

    // Check for a valid avatar pointer
    rTuneAssert( GetAvatarManager() != NULL );
    rTuneAssert( GetAvatarManager()->GetAvatarForPlayer(0) != NULL );

	for( int i = 0; i < Count; ++i )
    {
        ActiveCoin* c = GetInactiveCoin();
        if( !c )
        {
            CollectCoins( Count - i );
            break;
        }
        SpawnCoin(*c, Position, Position.y + COIN_HOVER, 0, true);
    }
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_LEVEL_ZONE );
    #endif
    GetEventManager()->TriggerEvent( EVENT_SPAWNED_COINS, reinterpret_cast<void*>( Count ) );
    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_LEVEL_ZONE );
    #endif
}

/*=============================================================================
Spawn a single coin.
=============================================================================*/
void CoinManager::SpawnCoin( ActiveCoin& Coin, const rmt::Vector& Start, float Ground, const rmt::Vector* Direction, bool InstaCollect )
{
    Coin.Position = Start;
    Coin.Position.y = Ground + COIN_HOVER;
    Coin.State = InstaCollect ? CS_SpawnToCollect : CS_InitialSpawning;
    Coin.Age = Sparkle::sRandom.Float() * rmt::PI;
    Coin.Ground = Ground;
    Coin.Velocity.x = Sparkle::sRandom.FloatSigned();
    Coin.Velocity.y = Sparkle::sRandom.Float();
    Coin.Velocity.z = Sparkle::sRandom.FloatSigned();
    Coin.Velocity.Normalize();
    Coin.Velocity.y += COIN_EXTRA_VERTICAL;
    if( Direction )
    {
        if( Direction->Dot( Coin.Velocity ) < 0 )
        {
			Coin.Velocity.x *= -1.0f;
			Coin.Velocity.z *= -1.0f;
        }
    }
    Coin.Velocity.Scale(COIN_SPAWN_VELOCITY);
    ++mNumActiveCoins;
#ifndef RAD_RELEASE
    dbg_MaxCoins = rmt::Max( (int)mNumActiveCoins, dbg_MaxCoins );
#endif
}

/*=============================================================================
Add a HUD coin at the coin counter position and have it fly down to the middle
of the screen. Used when the player's coin amount decrements so that they can
see that it's going down a little easier.
=============================================================================*/
void CoinManager::AddFlyDownCoin(void)
{
	return;
	// They didn't like it. Method is now a nop...until they decide they want it again.
/*    ActiveCoin* c = GetInactiveCoin();
    if( !c )
    {
        return;
    }
    c->Position.x = 0.0f;
    c->Position.y = 0.0f;
    c->Position.z = 0.0f;
    c->State = CS_FlyingFromHUD;
    c->Age = 0.0f;
	c->Velocity.Set(-0.5f, -0.5f, 0.0f);
    c->Velocity.Scale(FLYING_TIME * 2.0f);
    ++mNumActiveCoins;
    ++mNumHUDFlying;
#ifndef RAD_RELEASE
    dbg_MaxCoins = rmt::Max( (int)mNumActiveCoins, dbg_MaxCoins );
#endif
*/
}

/*=============================================================================
Update the position/animation of all the coins.
=============================================================================*/
void CoinManager::Update( int ElapsedMS )
{
    float deltaSeconds = (float)ElapsedMS * 0.001f;
    mHUDCoinAngle += deltaSeconds;
#ifndef RAD_RELEASE
    dbg_CoinsDrawn = 0;
#endif
    if(GetGameFlow()->GetCurrentContext() == CONTEXT_PAUSE)
    {
        return;
    }
    rmt::Vector avatarPos;
    GetAvatarManager()->GetAvatarForPlayer( 0 )->GetPosition( avatarPos );
    bool isInCar = GetAvatarManager()->GetAvatarForPlayer( 0 )->IsInCar();
    for( int i = 0; i < NUM_COINS; ++i )
    {
        ActiveCoin& c = mActiveCoins[ i ];
        if( c.State != CS_Inactive )
        {
            float decaySpin = 0.0f;
            if( c.State != CS_InitialSpawning && c.State != CS_SpawnToCollect && c.State != CS_Collecting && c.State != CS_Collected )
            {
                CheckCollection( c, avatarPos, isInCar ? IN_CAR_RANGE_MULTIPLIER : 1.0f );
            }
            if( c.State == CS_Collecting )
            {
                UpdateCollecting( c, avatarPos );
            }
            else if((c.State == CS_FlyingToHUD) || (c.State == CS_FlyingFromHUD))
            {
                UpdateHUDFlying( c );
            }
            else if( c.State != CS_RestingIndefinitely && c.State != CS_Collecting && c.State != CS_Collected )
            {
                if( c.Age > LIFE_TIME + DECAY_TIME )
                {
                    // Coin has expired.
                    c.State = CS_Inactive;
                    --mNumActiveCoins;
                    GetSparkleManager()->AddSparkle( rmt::Vector( c.Position.x, c.Position.y + mCoinBounding.radius, c.Position.z ), 0.25f, 0.5f, rmt::Vector( 0.0f, 0.01f, 0.0f ), Sparkle::SE_Vanish );
                }
                else if( c.Age > LIFE_TIME  )
                {
                    c.State = CS_Decaying;
                    decaySpin = ( c.Age - LIFE_TIME ) * I_DECAY_TIME;
                    decaySpin *= decaySpin;
                }
                if( c.State == CS_Spawning || c.State == CS_InitialSpawning || c.State == CS_SpawnToCollect )
                {
                    UpdateSpawning(c, avatarPos, deltaSeconds);
                }
            }
            c.Age += deltaSeconds;
            // Rotate the coin based on age.
            float coinSin, coinCos;
            rmt::SinCos( ( c.Age * SPIN_MULTIPLIER ) + ( decaySpin * DECAY_EXTRA_SPIN ), &coinSin, &coinCos );
            c.HeadingCos = coinCos;
            c.HeadingSin = coinSin;
        }
    }
}

/*=============================================================================
Damaging out the player's vehicle causes their bank to go down.
=============================================================================*/
void CoinManager::OnVehicleDestroyed( Vehicle* DestroyedVehicle )
{
	if( DestroyedVehicle->mVehicleType == VT_USER )
	{
        int lose = rmt::Min( 20, GetBankValue() );
	    LoseCoins( lose, &( DestroyedVehicle->GetPosition() ) );
	}
}

/*=============================================================================
Whatever drawable is set here will be used for all the coin drawables. Note
that there isn't any support for shadows or animation. It's just geometry.
=============================================================================*/
void CoinManager::SetCoinDrawable( tDrawable* Drawable )
{
    tRefCounted::Assign( m_pCoinDrawable, Drawable );
    if( m_pCoinDrawable )
    {
        m_pCoinDrawable->GetBoundingSphere( &mCoinBounding );
    }
    else
    {
        mCoinBounding.centre.Set( 0.0f, 0.0f, 0.0f );
        mCoinBounding.radius = 0.0f;
    }
}

/*=============================================================================
Check if a coin is within range to do the collection animation to the player.
If it is we set the state to Collecting.
=============================================================================*/
void CoinManager::CheckCollection( ActiveCoin& Coin, const rmt::Vector& AvatarPos, float RangeMultiplier )
{
    float x = rmt::Abs( Coin.Position.x - AvatarPos.x );
    float z = rmt::Abs( Coin.Position.z - AvatarPos.z );
    float y = rmt::Abs( Coin.Position.y - AvatarPos.y );
    // Note, no multipler for Y.
    if( ( x < ( RANGE * RangeMultiplier ) ) && ( z < ( RANGE * RangeMultiplier ) ) && ( y < RANGE ) && ( x + z < ( INNER_RANGE * RangeMultiplier ) ) )
    {
        if( Coin.State == CS_RestingIndefinitely )
        {
            GetPersistentWorldManager()->OnObjectBreak( Coin.PersistentObjectID );
        }
        Coin.State = CS_Collecting;
        Coin.Age = 0.0f;
    }
}

/*=============================================================================
Player has picked up a coin. This will increase the player's bank value, fire
a coin collected event so that we can play a sound
=============================================================================*/
void CoinManager::CollectCoins( int Count )
{
	AdjustBankValue( Count );
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_LEVEL_ZONE );
    #endif
    GetEventManager()->TriggerEvent( EVENT_COLLECTED_COINS, reinterpret_cast<void*>( Count ) );
    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_LEVEL_ZONE );
    #endif
}

/*=============================================================================
Player loses coins for whatever reason. We play a sound and HUD animation.
If Position isn't null then we respawn the coins at the specified location.
=============================================================================*/
void CoinManager::LoseCoins( int Count, const rmt::Vector* Position )
{
    int count = rmt::Min( Count, GetBankValue() );
    rAssert( count >= 0 );
    AdjustBankValue( -count );
    if( Position )
    {
        // Because we are losing the coins when we are shot (or car explodes) the position tends to be lower
        //then for a crate. So we raise the position a tiny bit so that the coin's travel is the same as
        //the designers tune for when kicking the crates around.
        SpawnCoins( count, rmt::Vector(Position->x, Position->y + 0.25f, Position->z), Position->y, 0, true );
    }
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_LEVEL_ZONE );
    #endif
    GetEventManager()->TriggerEvent( EVENT_LOST_COINS, reinterpret_cast<void*>( count ) );
    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_LEVEL_ZONE );
    #endif
}

int
CoinManager::GetBankValue() const
{
    RenderEnums::LevelEnum currentLevel = GetGameplayManager()->GetCurrentLevelIndex();
    int bankValue = GetCharacterSheetManager()->GetNumberOfTokens( currentLevel );

    return bankValue;
}

/*=============================================================================
Modify the player's bank value without any special effects.
=============================================================================*/
void CoinManager::AdjustBankValue( int DeltaCoins )
{
/*
    mBankValue += DeltaCoins;
    mBankValue = rmt::Max( mBankValue, 0 );
*/
    // tell the character sheet manager
    //
    GetCharacterSheetManager()->AddTokens( GetGameplayManager()->GetCurrentLevelIndex(),
                                           DeltaCoins );

    if( DeltaCoins > 0 )
    {
        GetEventManager()->TriggerEvent( EVENT_COLLECTED_COINS );
    }
    else if( DeltaCoins < 0 )
    {
        GetEventManager()->TriggerEvent( EVENT_LOST_COINS );
    }
}

/*
/*=============================================================================
Force bank to a specific value. No special effects.
=============================================================================/
void CoinManager::SetBankValue( int Coins )
{
    rAssert( Coins >= 0 );
    mBankValue = Coins;

    // update character sheet
    //
    GetCharacterSheetManager()->SetNumberOfTokens( GetGameplayManager()->GetCurrentLevelIndex(),
                                                   mBankValue );
}
*/

/*=============================================================================
World coins just sit around until their zone unloads. They take up space in the
active coin array so don't use them lightly.
=============================================================================*/
void CoinManager::AddWorldCoin( const rmt::Vector& Position, tUID Sector )
{
    if( !mActiveCoins )
    {
        return;
    }

    short persistentID = GetPersistentWorldManager()->GetPersistentObjectID( Sector );
    if( persistentID < -1 )
    {
        return;
    }

    ActiveCoin* c = GetInactiveCoin();
    if( !c )
    {
        return;
    }

    c->Position = Position;
    c->State = CS_RestingIndefinitely;
    c->Age = Sparkle::sRandom.Float() * rmt::PI;
    c->Sector = Sector;
    c->PersistentObjectID = persistentID;
    ++mNumActiveCoins;
#ifndef RAD_RELEASE
    dbg_MaxCoins = rmt::Max( (int)mNumActiveCoins, dbg_MaxCoins );
#endif
}

/*=============================================================================
Remove any coins resting in the world which are in the sector which was just
unloaded. Only remove the coins which are in CS_RestingIndefinite state since
others could be spawning or collecting, etc (that's kind of unlikely however).
=============================================================================*/
void CoinManager::RemoveWorldCoins( tUID Sector )
{
    for( int i = 0; i < NUM_COINS; ++i )
    {
        ActiveCoin& c = mActiveCoins[ i ];
        if( ( c.State == CS_RestingIndefinitely ) && ( c.Sector == Sector ) )
        {
            c.State = CS_Inactive;
            --mNumActiveCoins;
        }
    }
}

/*=============================================================================
Update the animation for the coins when they are collected.
=============================================================================*/
void CoinManager::UpdateCollecting( ActiveCoin& Coin, const rmt::Vector& AvatarPos )
{
    if( Coin.Age > ( COLLECT_TIME * 0.4f ) )
    {
        Coin.State = CS_Collected;
        Coin.Age = 0.0f;
		Coin.HeadingCos = 1;
        CollectCoins( 1 );
    }
    else
    {
        rmt::Vector diff;
        diff.Sub( AvatarPos, Coin.Position );
        diff.Scale( Coin.Age * I_COLLECT_TIME );
        float pop = ( COLLECT_TIME - Coin.Age ) * I_COLLECT_TIME;
        diff.y += pop * POP_SCALE;
        Coin.Position.Add( diff );
        --DoSparkle;
        if( DoSparkle < 0 )
        {
            DoSparkle = Sparkle_Rate;
            Sparkle_Rate ^= SPARKLE_RATE_TOGGLE;
			rmt::Vector vel;
			vel.Scale(FRAME_RATIO * 0.1f, Coin.Velocity);
			vel.y += SPARKLE_FLOAT;
            GetSparkleManager()->AddSparkle( Coin.Position, 0.5f, 1.0f, vel, Sparkle::SE_Trail );
        }
    }
}

/*=============================================================================
Update the spawning animation for the coins.
=============================================================================*/
void CoinManager::UpdateSpawning(ActiveCoin& Coin, const rmt::Vector& AvatarPos, float DeltaSeconds)
{
	rmt::Vector vel;
	vel.Scale(DeltaSeconds, Coin.Velocity);
    Coin.Velocity.Scale( (float)pow(COIN_DRAG, DeltaSeconds) );
    Coin.Velocity.y -= GRAVITY * DeltaSeconds;
    Coin.Position.Add( vel );
    // Use this if we want faster (but less accurate) coins.
    /*
    Coin.Velocity.Scale( COIN_DRAG );
    Coin.Velocity.y -= GRAVITY;
    Coin.Position.Add( Coin.Velocity );
    */
    if( Coin.State == CS_InitialSpawning )
    {
        --DoSparkle;
        if( DoSparkle < 0 )
        {
            DoSparkle = Sparkle_Rate;
            Sparkle_Rate ^= SPARKLE_RATE_TOGGLE;
            GetSparkleManager()->AddSparkle( Coin.Position, 0.5f, 1.0f, rmt::Vector( Coin.Velocity.x * 0.1f, ( Coin.Velocity.y * 0.1f ) + SPARKLE_FLOAT, Coin.Velocity.z * 0.1f ), Sparkle::SE_Trail );
        }
    }
    if( Coin.Position.y < Coin.Ground )
    {
        // Bounce.
        Coin.Velocity.y = rmt::Abs( Coin.Velocity.y * BOUNCE_DAMPENING );
        if( Coin.State == CS_SpawnToCollect )
        {
            Coin.State = CS_Collecting;
            Coin.Age = 0.0f;
        }
        else
        {
            Coin.State = CS_Spawning;
            if( Coin.Velocity.y < 0.01f )
            {
                Coin.State = CS_Resting;
            }
        }
        Coin.Position.y = Coin.Ground;
    }
}

void CoinManager::AddGlint( ActiveCoin& Coin, const rmt::Vector& ToCamera, const rmt::Vector& CoinAxis, const rmt::Vector& CameraRight )
{
    rmt::Vector pos;
    pos.Add( Coin.Position, mCoinBounding.centre );
    float dot = ToCamera.DotProduct( CoinAxis );
    float radius = ( dot < 0.0f ) ? -mCoinBounding.radius : mCoinBounding.radius;
    pos.ScaleAdd( radius, CoinAxis );
    rmt::Vector left;
    left.Scale( -0.0025f, CameraRight );
    GetSparkleManager()->AddSparkle( pos, 1.0f, 0.25f, left, Sparkle::SE_Glint );
}

/*=============================================================================
Animation of the coin zipping up to the HUD counter.
=============================================================================*/
void CoinManager::UpdateHUDFlying( ActiveCoin& Coin )
{
    // Do fly to HUD animation.
    if(((Coin.State == CS_FlyingToHUD) && (Coin.Age > FLYING_TIME )) ||
       ((Coin.State == CS_FlyingFromHUD) && (Coin.Age > 0.5f)))
    {
        Coin.State = CS_Inactive;
        --mNumActiveCoins;
        --mNumHUDFlying;
    }
}

void CoinManager::ClearHUDCoins(void)
{
	if(mNumHUDFlying == 0)
	{
		return;
	}
	for(int i = 0; i < NUM_COINS; ++i)
	{
		ActiveCoin& c = mActiveCoins[i];
		if((c.State == CS_FlyingToHUD) || (c.State == CS_FlyingFromHUD))
		{
			c.State = CS_Inactive;
			--mNumActiveCoins;
			--mNumHUDFlying;
		}
	}
}

/*=============================================================================
Render pass called form within the opaque rendering stage of world render. It
does a simple distance and camera culling check to determine which coins to
draw. We don't add the coins to the DSG since the overhead of moving/adding/
removing them is too much. Also, the DSG nodes are hard coded to only allow a
certain number of extra entities over the amount preallocated in the pipeline.
It's very possible that we'll end up with more then then that value.
=============================================================================*/
void CoinManager::Render( void )
{
    if( ( mNumActiveCoins - mNumHUDFlying ) > 0 )
    {
        tCamera* camera = p3d::context->GetView()->GetCamera();
        rAssert( camera );
        const rmt::Matrix& camTrans = camera->GetCameraToWorldMatrix();

        for( int i = 0; i < NUM_COINS; ++i )
        {
            ActiveCoin& c = mActiveCoins[ i ];
            if( c.State != CS_Inactive )
            {
                if( c.State == CS_Collected )
                {
                    camera->WorldToView( c.Position, &c.Position );
                    c.Velocity.Sub( rmt::Vector( COUNTER_X, COUNTER_Y, 0.0f ), c.Position );
                    c.State = CS_FlyingToHUD;
                    ++mNumHUDFlying;
                    continue;
                }
                if( !m_pCoinDrawable )
                {
                    continue;
                }
                // Visiblity test.
                rmt::Vector diff;
                diff.Sub( camTrans.Row( 3 ), c.Position );
                float camDirDot = diff.DotProduct( camTrans.Row( 2 ) );
                // Note that the diff vector is towards the camera so the camDirDot will be negative when the camera is facing
                //the object. So if it's positive then we can assume it's behind the camera.
                if( camDirDot > 0.0f )
                {
                    continue;
                }
                // Distance check.
                float distance = diff.Magnitude();
                if( distance > MAX_VISIBLITY )
                {
                    continue;
                }
#ifndef RAD_RELEASE
                ++dbg_CoinsDrawn;
#endif
                if( ( GlintDelay <= 0 ) && ( DoGlint == i ) )
                {
                    if( c.State != CS_InitialSpawning && c.State != CS_Collecting && c.State != CS_Collected )
                    {
						if(GetGameFlow()->GetCurrentContext() != CONTEXT_PAUSE)
						{
	                        AddGlint( c, diff, rmt::Vector( c.HeadingCos, 0.0f, -c.HeadingSin ), camTrans.Row( 0 ) );
						}
                    }
                }
                // Down the columns, across the rows.
                rmt::Matrix transform( c.HeadingCos, 0.0f, -c.HeadingSin, 0.0f,
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    c.HeadingSin, 0.0f, c.HeadingCos, 0.0f,
                                    c.Position.x, c.Position.y, c.Position.z, 1.0f );

                p3d::pddi->PushMultMatrix( PDDI_MATRIX_MODELVIEW, &transform );
                m_pCoinDrawable->Display();
                p3d::pddi->PopMatrix( PDDI_MATRIX_MODELVIEW );
            }
        }
    }
    --GlintDelay;
    if( GlintDelay < 0 )
    {
        GlintDelay = Glint_Rate;
        DoGlint = Sparkle::sRandom.IntRanged( 0, NUM_COINS );
    }
}

void CoinManager::SetHUDCoin( int X, int Y, bool IsShowing )
{
    const static float ScreenWidthRatio  = 1.0f / 640.0f;
    const static float ScreenHeightRatio = 1.0f / 480.0f;
    const static float ScreenAspect = 4.0f / 3.0f;
    if( IsShowing )
    {
        mHUDCoinX = ( ( ( (float)X * ScreenWidthRatio ) ) - 0.5f ) * ScreenAspect * ScreenAspect;
        mHUDCoinY = ( ( ( (float)Y * ScreenHeightRatio ) ) - 0.5f ) * ScreenAspect;
    }
    else
    {
        mHUDCoinX = -10.f;
    }
}

void CoinManager::OnCheatEntered( eCheatID cheatID, bool isEnabled )
{
    if( cheatID == CHEAT_ID_EXTRA_COINS )
    {
        this->AdjustBankValue( +100 ); // add 100 coins to bank value
    }
}

/*=============================================================================
Rendering pass for the HUD. Used for when the coin flies up to the HUD counter.
We set the state once and do all the rendering.
=============================================================================*/
void CoinManager::HUDRender( void )
{
    if( !m_pCoinDrawable )
    {
        return;
    }
    bool renderHUDCoin = false;
    if( mHUDCoinX > -1.0f && mHUDCoinX < 1.0f && mHUDCoinY > -1.0f && mHUDCoinY < 1.0f )
    {
        renderHUDCoin = true;
    }
    if( ( mNumHUDFlying > 0 ) || renderHUDCoin )
    {
	    p3d::stack->Push();
        bool oldZWrite = p3d::pddi->GetZWrite();
        pddiCompareMode oldZComp = p3d::pddi->GetZCompare();
        if( oldZWrite )
        {
            p3d::pddi->SetZWrite( false );
        }
        if( oldZComp != PDDI_COMPARE_ALWAYS )
        {
            p3d::pddi->SetZCompare( PDDI_COMPARE_ALWAYS );
        }
	    p3d::pddi->SetProjectionMode( PDDI_PROJECTION_ORTHOGRAPHIC );
        pddiColour oldAmbient = p3d::pddi->GetAmbientLight();
        p3d::pddi->SetAmbientLight( pddiColour( 255, 255, 255 ) );

        rmt::Vector pos;
        if( renderHUDCoin )
        {
    	    p3d::stack->LoadIdentity();
            p3d::stack->Scale( 0.1f, 0.1f, 1.0f );
            float coinSin, coinCos;
            rmt::SinCos( ( mHUDCoinAngle * SPIN_MULTIPLIER ), &coinSin, &coinCos );
            rmt::Matrix transform( coinCos, 0.0f, -coinSin, 0.0f,
                                   0.0f, 1.0f, 0.0f, 0.0f,
                                   coinSin, 0.0f, coinCos, 0.0f,
                                   mHUDCoinX * 5.0f, mHUDCoinY * 5.0f, 5.0f, 1.0f );
#ifndef RAD_RELEASE
            ++dbg_CoinsDrawn;
#endif
            if( mHUDSparkle <= 0 )
            {
                rmt::Vector pos;
                pos.ScaleAdd( rmt::Vector( mHUDCoinX, mHUDCoinY, 5.0f ), 0.2f, mCoinBounding.centre );
                float radius = 0.2f * ( ( coinCos < 0.0f ) ? mCoinBounding.radius : -mCoinBounding.radius );
                pos.Add( rmt::Vector( coinCos * radius, 0.0f, coinSin * radius ) );
                GetSparkleManager()->AddSparkle( pos, 1.0f, 0.25f, rmt::Vector( 0.0f, 0.0f, 0.0f ), Sparkle::SE_HUDGlint );
                mHUDSparkle = Sparkle::sRandom.IntRanged( HUD_SPARKLE_RATE, HUD_SPARKLE_RATE * 3 );
            }
            --mHUDSparkle;
            p3d::pddi->PushMultMatrix( PDDI_MATRIX_MODELVIEW, &transform );
            m_pCoinDrawable->Display();
            p3d::pddi->PopMatrix( PDDI_MATRIX_MODELVIEW );
        }
	    p3d::stack->LoadIdentity();
        p3d::stack->Scale( 0.05f, 0.05f, 1.0f);

        for( int i = 0; i < NUM_COINS; ++i )
        {
            ActiveCoin& c = mActiveCoins[ i ];
            if((c.State == CS_FlyingToHUD) || (c.State == CS_FlyingFromHUD))
            {
                if(c.State == CS_FlyingToHUD)
                {
                    float a = c.Age * I_FLYING_TIME;
                    pos.ScaleAdd( c.Position, a * a, c.Velocity );
                }
                else
                {
                    pos.ScaleAdd(c.Position, c.Age * I_FLYING_TIME, c.Velocity);
                }
                // Down the columns, across the rows.
                rmt::Matrix transform( 1.0f, 0.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f,
                                    pos.x * 10.0f, pos.y * 10.0f, 5.0f, 1.0f );
    #ifndef RAD_RELEASE
                ++dbg_CoinsDrawn;
    #endif
                p3d::pddi->PushMultMatrix( PDDI_MATRIX_MODELVIEW, &transform );
                m_pCoinDrawable->Display();
                p3d::pddi->PopMatrix( PDDI_MATRIX_MODELVIEW );
            }
        }
	    p3d::pddi->SetProjectionMode(PDDI_PROJECTION_PERSPECTIVE);
        p3d::pddi->SetAmbientLight( oldAmbient );
        if( oldZWrite )
        {
            p3d::pddi->SetZWrite( true );
        }
        if( oldZComp != PDDI_COMPARE_ALWAYS )
        {
       	    p3d::pddi->SetZCompare( oldZComp );
        }
	    p3d::stack->Pop();
    }
}