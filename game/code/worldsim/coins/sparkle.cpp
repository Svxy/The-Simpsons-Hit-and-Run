//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File:        Sparkle.cpp
//
// Description: Implementation of class Sparkle
//
// History:     18/2/2003 + Created -- James Harrison
//
//=============================================================================

//========================================
// System Includes
//========================================

#include <radmath/matrix.hpp>
#include <p3d/entity.hpp>
#include <p3d/view.hpp>
#include <p3d/camera.hpp>
#include <p3d/matrixstack.hpp>
#include <p3d/texture.hpp>
#include <pddi/pddi.hpp>
#ifndef RAD_RELEASE
#include <raddebug.hpp>
#include <raddebugwatch.hpp>
#endif

//========================================
// Project Includes
//========================================
#include <contexts/bootupcontext.h>
#include <worldsim/coins/sparkle.h>
#include <memory/srrmemory.h>
#include <main/game.h>
#include <camera/supercam.h>
#include <camera/supercamcentral.h>
#include <camera/supercammanager.h>
#include <gameflow/gameflow.h>
#include <mission/gameplaymanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
#if defined( RAD_XBOX ) || defined( RAD_WIN32 )
    #ifdef RAD_RELEASE
        static const float FRAME_RATE = 60.0f;
    #elif RAD_TUNE
        static const float FRAME_RATE = 30.0f;
    #else
        static const float FRAME_RATE = 15.0f;
    #endif
    static const float FRAME_RATIO = 1.0f / FRAME_RATE;
#endif

static const float DRIFT_DRAG = 0.99f; // How much horizontal inertia is lost per frame.
static const int NUM_SPARK_PARTICLES = 10;
static const float SPARK_RADIUS = 1.8f;
static const float SPARK_SIZE = 0.25f;
static const float SPARK_VERTICAL_SCALE = 0.25f;
static const float SPARK_DURATION = 0.3f;
static const float SPARK_DURATION_RATIO = 1.0f / SPARK_DURATION;
static const float SPARK_VELOCITY_TO_SIZE_RATIO = 1.0f / ( SPARK_RADIUS * SPARK_DURATION_RATIO * FRAME_RATIO );
static const float DASH_SIZE = 0.2f;
static const float DASH_SIZE_INCREASE = 0.2f;
static const float DASH_DURATION_RATIO = 2.0f;
static const float LANDING_SIZE = 0.5f;
static const float LANDING_NUM = 16;
static const float LANDING_DURATION_RATIO = 1.5f;
static const int NUM_PAINT_CHIPS = 10;
static const float PAINT_CHIP_SIZE = 0.05f;
static const float SHOCK_RING_RADIUS = 3.0f;
static const float SHOCK_RING_SPEED = 1.75f;
static const int NUM_STAR_PARTICLES = 5;
static const float STAR_DURATION_RATIO = 1.0f / 0.5f;
static const float STAR_RADIUS = 10.0f;
static const float STAR_SIZE = 0.25f;
static const float STAR_DUST_RADIUS = 2.0f;
static const float STAR_DUST_SIZE = 1.0f;
static const float STAR_DUST_DURATION_RATIO = 1.0f;
static const int NUM_BOTTOMOUT_SPARKS = 30;
static const float BOTTOMOUT_RADIUS = 2.5f;
static const int NUM_SMOKE_PARTICLES = 2; // Maximum number of particles per call for the smoke.
static const float SMOKE_SIZE = 0.3f;
static const int NUM_GAG_SPARKLES = 1;
static const float GAG_SPARKLE_ELEVATION = 1.0f;
static const float GAG_SPARKLE_EMIT_RADIUS = 0.75f;
static const float GAG_SPARKLE_DISTANCE = 0.15f;
static const float GAG_SPARKLE_DURATION_RATIO = 1.0f / 1.6f;
static const float GAG_SPARKLE_SIZE = 0.45f;
static const tColour GAG_SPARKLE_COLOUR(32, 128, 192);
static const tColour STAR_COLOUR( 255, 240, 100, 255 );
static const tColour STAR_DUST_COLOUR( 150, 140, 110, 128 );
static const tColour TRAIL_COLOUR( 0xFF, 0xFF, 0x7F );
static const tColour SPARK_COLOUR( 0xFF, 0xC6, 0x00 );
static const tColour DASH_COLOUR( 220, 210, 180, 32 );
static const tColour LANDING_COLOUR( 220, 210, 180, 128 );
static const tColour RING_COLOUR( 0xFF, 0xFF, 0xFF );
static const tColour LEAF_COLOUR( 120, 200, 100 );
static const tColour LIGHT_SMOKE_COLOUR( 0xC0, 0xD0, 0xE0, 0x10 );
static const tColour DARK_SMOKE_COLOUR( 0x40, 0x40, 0x40, 0xA0 );
#ifndef RAD_RELEASE
static int dbg_MaxSparkles = 0;
#endif

rmt::Randomizer Sparkle::sRandom( 0 ); // Set in the constructor.
bool Sparkle::sRandomSeeded = false;

Sparkle* Sparkle::spInstance = 0;

Sparkle* Sparkle::CreateInstance( unsigned char NumTextures, unsigned short NumSparkles )
{
	rAssertMsg( spInstance == 0, "SparkleManager already created.\n" );
    HeapManager::GetInstance()->PushHeap( GMA_PERSISTENT );
    spInstance = new Sparkle( NumTextures, NumSparkles );
    rAssert( spInstance );
    HeapManager::GetInstance()->PopHeap( GMA_PERSISTENT );
    return Sparkle::GetInstance();
}

Sparkle* Sparkle::GetInstance( void )
{
	rAssertMsg( spInstance != 0, "SparkleManager has not been created yet.\n" );
	return spInstance;
}

void Sparkle::DestroyInstance( void )
{
	rAssertMsg( spInstance != 0, "SparkleManager has not been created.\n" );
	delete spInstance;
    spInstance = 0;
}


//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
Sparkle::Sparkle( unsigned char NumTextures, unsigned short NumSparkles ) :
    mActiveSparkles( 0 ),
    mpTextures( 0 ),
    mNumSparkles( NumSparkles ),
    mNextInactiveSparkle( 0 ),
    mNumActiveSparkles( 0 ),
    mNumHUDSparkles( 0 ),
    mNumTextures( NumTextures )
{
    rAssert( mNumSparkles <= 65535 ); // We're using an unsigned short for our counters!
    rAssert( mNumTextures <= ST_NUM_TEXTURES );

    if (!sRandomSeeded)
    {
        sRandom.Seed (Game::GetRandomSeed ());
        sRandomSeeded = true;
    }
}

//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
Sparkle::~Sparkle()
{
    Destroy();
#ifndef RAD_RELEASE
    radDbgWatchDelete( &dbg_MaxSparkles );
#endif
}

void Sparkle::Init( void )
{
    #ifdef RAD_GAMECUBE
        HeapManager::GetInstance()->PushHeap( GMA_GC_VMM );
    #else
        HeapManager::GetInstance()->PushHeap( GMA_LEVEL_ZONE );
    #endif
    mActiveSparkles = new ActiveSparkle[ mNumSparkles ];
    rAssert( mActiveSparkles );
    mpTextures = new tTexture*[ mNumTextures ];
    rAssert( mpTextures );
    mNumActiveSparkles = new unsigned short[ mNumTextures << 1 ];
    rAssert( mNumActiveSparkles );
    mNumHUDSparkles = new unsigned short[ mNumTextures ];
    rAssert( mNumHUDSparkles );
    #ifdef RAD_GAMECUBE
        HeapManager::GetInstance()->PopHeap( GMA_GC_VMM );
    #else
        HeapManager::GetInstance()->PopHeap( GMA_LEVEL_ZONE );
    #endif

    for( int i = 0; i < mNumTextures; ++i )
    {
        mpTextures[ i ] = 0;
        mNumActiveSparkles[ i ] = 0;
        mNumActiveSparkles[ i + ST_NUM_TEXTURES ] = 0;
        mNumHUDSparkles[ i ] = 0;
    }
}

void Sparkle::Destroy( void )
{
    if( mpTextures )
    {
        for( int i = 0; i < mNumTextures; ++i )
        {
            tRefCounted::Release( mpTextures[ i ] );
        }
        delete [] mpTextures;
        mpTextures = 0;
    }
    delete [] mNumActiveSparkles;
    mNumActiveSparkles = 0;
    delete [] mNumHUDSparkles;
    mNumHUDSparkles = 0;
    delete [] mActiveSparkles;
    mActiveSparkles = 0;
}

/*=============================================================================
Set the texture the sparkles will be drawn with. Perhaps in the future we'll
have some method of doing multiple textures.
=============================================================================*/
void Sparkle::SetTexture( unsigned char Index, tTexture* Texture )
{
    rAssert( Index < mNumTextures );
    tRefCounted::Assign( mpTextures[ Index ], Texture );
}

/*=============================================================================
Find an unused sparkle in our pre-allocated array of sparkles.
=============================================================================*/
Sparkle::ActiveSparkle* Sparkle::GetInactiveSparkle( void )
{
    for( int i = 0; i < mNumSparkles; ++i )
    {
        ActiveSparkle* s = &(mActiveSparkles[ mNextInactiveSparkle ]);
        mNextInactiveSparkle = ( mNextInactiveSparkle + 1 ) % mNumSparkles;
        if( !s->Active )
        {
            s->Active = 1;
            s->HUD = 0;
            return s;
        }
    }
#ifndef RAD_RELEASE
    // rTunePrintf( "WARNING: Ran out of sparkles!" );
#endif
    return 0;
}

/*=============================================================================
Add a sparkle to the world. This is used for collection/spawn trails.
=============================================================================*/
void Sparkle::AddSparkle( const rmt::Vector& Position, float Size, float Duration, const rmt::Vector& Velocity, eSparkleEffect Effect )
{
    rAssert( Duration > 0.0f ); // No infinite druation sparkles.
    if( Duration <= 0.0f )
    {
        return;
    }
    ActiveSparkle* s = GetInactiveSparkle();
    if( !s )
    {
        return;
    }
    s->Texture = ST_Sparkle;
    if( Effect == SE_Trail )
    {
        s->Align = SA_None;
        s->Scale = SS_LinearDown;
        s->ColourAnim = SC_LinearRedDownAtHalf;
        s->CornerFade = 0;
        s->Motion = SM_Linear;
        s->Colour = TRAIL_COLOUR;
    }
    else if( Effect == SE_Glint )
    {
        s->Align = SA_None;
        s->Scale = SS_LinearDown;
        s->ColourAnim = SC_LinearDown;
        s->CornerFade = 0;
        s->Motion = SM_Linear;
        s->Colour = TRAIL_COLOUR;
    }
    else if( Effect == SE_Vanish )
    {
        s->Align = SA_None;
        s->Scale = SS_BulgeDown;
        s->ColourAnim = SC_LinearRedDownAtHalf;
        s->CornerFade = 0;
        s->Motion = SM_Linear;
        s->Colour = TRAIL_COLOUR;
    }
    else
    {
        s->HUD = 1;
        s->Align = SA_None;
        s->Scale = SS_LinearDown;
        s->ColourAnim = SC_LinearDown;
        s->CornerFade = 0;
        s->Motion = SM_Linear;
        s->Colour = TRAIL_COLOUR;
        ++( mNumHUDSparkles[ s->Texture ] );
    }
    s->Size = Size;
    s->DurationRatio = 1.0f / Duration;
    s->Life = 1.0f;
    s->Position = Position;
    s->Velocity = Velocity;
    ++( mNumActiveSparkles[ s->Texture ] );
#ifndef RAD_RELEASE
//    dbg_MaxSparkles = rmt::Max( mNumActiveSparkles, dbg_MaxSparkles );
#endif
}

/*=============================================================================
Add the blue gag sparkle to the world. Size is used to scale the particles down
for indoors. A 1 is full sized, .5 is half, etc. The Strength value is used to
fade the sparkle out for distance culling.
Okay, so the problem is we don't want sparkles shooting out every frame for all
those gags. It's too many particles and it looks bad. We'd either have to store
a state with each gag, some sort of counter, or store a state here to only
activate every few calls. The problem with storing a state with each gag is an
extra byte for anything using this.
Currently only gags uses the sparkles, but it could be others (doorbells, NPCs
you talk to). The problem with only activating every few calls, is if there is
exactly the number of gags as the number we wait, then only one gag gets a
sparkle. I perfer to hold the state here anyway, since it's less memory. So we
have two solutions, randomize the amount we skip before we spit out another
sparkle, or hold the state of the objects here. If we held the state here we
could create a hash table and then group the calling objects by their hash
value (we'd have to pass in a this pointer so we know who called us). So if we
took bits 4 to 7 (i.e. the high order bits of the first byte of the address
(remember since the address is aligned we don't want to take the first bits of
the first bytes) we'd end up with a number between 0 and 15 which we could use
as an index into some bits we are using to hold a counter controling when to
spit out a sparkle. I think I'll try something along those lines.
=============================================================================*/
void Sparkle::AddGagSparkle(const rmt::Vector& Position, float Size, float Strength, unsigned int Caller)
{
    static unsigned int actionFlag = 0;
    if(GetGameFlow()->GetCurrentContext() == CONTEXT_PAUSE)
    {
        return;
    }

    if((Size <= 0.0f) || (Strength <= 0.0f) || (int(255 * Strength) <= 0))
    {
        return;
    }
    int bitMask = 1 << ((Caller >> 4) & 31);
    actionFlag ^= bitMask;
    if(actionFlag & bitMask)
    {
        return;
    }
    ActiveSparkle* s = GetInactiveSparkle();
    if(!s)
    {
        return;
    }
    s->Align = SA_None;
    s->Scale = SS_Constant;
    s->ColourAnim = SC_LinearDown;
    s->CornerFade = 0;
    s->Motion = SM_Linear;
    s->Texture = ST_Sparkle;
    s->Position = Position;
    s->Position.y += GAG_SPARKLE_ELEVATION * Size;
    s->Velocity.x = sRandom.FloatSigned();
    s->Velocity.y = sRandom.FloatSigned();
    s->Velocity.z = sRandom.FloatSigned();
    s->Position.ScaleAdd(GAG_SPARKLE_EMIT_RADIUS * Size, s->Velocity);
    s->Velocity.Normalize();
    s->Velocity.Scale( GAG_SPARKLE_DISTANCE * GAG_SPARKLE_DURATION_RATIO * FRAME_RATIO * Size);
    s->Size = GAG_SPARKLE_SIZE * Size;
    s->Life = 1.0f + ( sRandom.FloatSigned() * 0.1f );
    s->DurationRatio = GAG_SPARKLE_DURATION_RATIO;
    int red = rmt::Clamp(int(GAG_SPARKLE_COLOUR.Red() * Strength), 0, 255);
    int green = rmt::Clamp(int(GAG_SPARKLE_COLOUR.Green() * Strength), 0, 255);
    int blue = rmt::Clamp(int(GAG_SPARKLE_COLOUR.Blue() * Strength), 0, 255);
    s->Colour.Set(red, green, blue);
    ++(mNumActiveSparkles[ s->Texture ]);
}

/*=============================================================================
Add spark to the world. A spark is like a spherical explosion.
=============================================================================*/
void Sparkle::AddSparks( const rmt::Vector& Position, const rmt::Vector& Velocity, float Strength )
{
    int numParticles = int( sRandom.IntRanged( NUM_SPARK_PARTICLES, NUM_SPARK_PARTICLES * 2 ) * Strength ) + 1;
    for( int i = 0; i < numParticles; ++i )
    {
        ActiveSparkle* s = GetInactiveSparkle();
        if( !s )
        {
            return;
        }
        s->Align = SA_MotionStreak;
        s->Scale = SS_Constant;
        s->ColourAnim = SC_LinearDownAtHalf;
        s->CornerFade = 0;
        s->Motion = SM_Decelerate;
        s->Texture = ST_Spark;
        s->Position = Position;
        s->Velocity.x = sRandom.FloatSigned();
        s->Velocity.y = sRandom.FloatSigned();
        s->Velocity.z = sRandom.FloatSigned();
        s->Velocity.Normalize();
        s->Velocity.Scale( SPARK_RADIUS * SPARK_DURATION_RATIO );
        s->Velocity.ScaleAdd(SPARK_DURATION_RATIO, Velocity );
        s->Velocity.Scale( FRAME_RATIO );
        s->Size = s->Velocity.Magnitude() * SPARK_VELOCITY_TO_SIZE_RATIO * SPARK_SIZE;
        s->Life = 1.0f + ( sRandom.FloatSigned() * 0.1f );
        s->DurationRatio = SPARK_DURATION_RATIO;
        s->Colour = SPARK_COLOUR;
        ++( mNumActiveSparkles[ s->Texture ] );
    }
}

/*=============================================================================
This is basically the same as the AddSparks, except it's radial sparks at the
position and more pronounced.
=============================================================================*/
void Sparkle::AddBottomOut( const rmt::Vector& Position )
{
    for( int i = 0; i < NUM_BOTTOMOUT_SPARKS; ++i )
    {
        ActiveSparkle* s = GetInactiveSparkle();
        if( !s )
        {
            return;
        }
        s->Align = SA_MotionStreak;
        s->Scale = SS_Constant;
        s->ColourAnim = SC_LinearDownAtHalf;
        s->CornerFade = 0;
        s->Motion = SM_Decelerate;
        s->Texture = ST_Spark;
        s->Position = Position;
        s->Velocity.x = sRandom.FloatSigned();
        s->Velocity.y = sRandom.Float() * 0.1f;
        s->Velocity.z = sRandom.FloatSigned();
        s->Velocity.Normalize();
        s->Velocity.Scale( BOTTOMOUT_RADIUS * SPARK_DURATION_RATIO );
        s->Velocity.Scale( FRAME_RATIO );
        s->Size = SPARK_SIZE;
        s->Life = 1.0f + ( sRandom.FloatSigned() * 0.1f );
        s->DurationRatio = SPARK_DURATION_RATIO;
        s->Colour = SPARK_COLOUR;
        ++( mNumActiveSparkles[ s->Texture ] );
    }
}

/*=============================================================================
Add the dash cloud. Note that the velocity is the travel of the character,
not the direction you want the dash cloud to travel.
=============================================================================*/
void Sparkle::AddDash( const rmt::Vector& Position, const rmt::Vector& Velocity, float Strength )
{
    int alpha = int(DASH_COLOUR.Alpha() * (Strength*Strength+Strength)*0.5f);
    if(alpha <= 0)
    {
        return;
    }
    int numParticles = sRandom.IntRanged( NUM_SPARK_PARTICLES >> 2, NUM_SPARK_PARTICLES ) + 1;
    for( int i = 0; i < numParticles; ++i )
    {
        ActiveSparkle* s = GetInactiveSparkle();
        if( !s )
        {
            return;
        }
        s->Align = SA_Rotation;
        s->Scale = SS_LinearGrow;
        s->ColourAnim = SC_LinearAlphaDown;
        s->CornerFade = 0;
        s->Motion = SM_Linear;
        s->Texture = ST_Puff;
        s->Position = Position;
        s->Position.y += 0.05f;
        s->Position.ScaleAdd(-0.05f, Velocity);
        s->Velocity.x = -Velocity.x * sRandom.FloatSigned() * 2.0f;
        s->Velocity.y = ( sRandom.Float() * 0.5f ) + 0.5f;
        s->Velocity.z = -Velocity.z * sRandom.FloatSigned() * 2.0f;
        s->Velocity.Normalize();
        s->Velocity.Scale( 0.2f + ( 0.2f * sRandom.Float() ) );
        s->Velocity.Scale( FRAME_RATIO );
        s->Size = DASH_SIZE;
        s->Life = 1.0f + ( sRandom.FloatSigned() * 0.5f );
        s->DurationRatio = DASH_DURATION_RATIO;
        s->Colour = DASH_COLOUR;
        s->Colour.SetAlpha(int(DASH_COLOUR.Alpha() * (Strength*Strength+Strength)*0.5f));
        ++( mNumActiveSparkles[ s->Texture ] );
    }
}

/*=============================================================================
Add the landing puff. It's a radial explosion of puffs used in the dash.
The strength is used to control the look over a jump landing (~0.5), a jump-
jump landing (~0.7), and a stomp (1.0).
=============================================================================*/
void Sparkle::AddLanding( const rmt::Vector& Position, float Strength )
{
    if( Strength <= 0.0f )
    {
        return;
    }
    int numParticles = int( LANDING_NUM + ( LANDING_NUM * Strength ) );
    for( int i = 0; i < numParticles; ++i )
    {
        ActiveSparkle* s = GetInactiveSparkle();
        if( !s )
        {
            return;
        }
        s->Align = SA_Rotation;
        s->Scale = SS_LinearUp;
        s->ColourAnim = SC_LinearAlphaDown;
        s->CornerFade = 0;
        s->Motion = SM_Linear;
        s->Texture = ST_Puff;
        s->Position = Position;
        s->Position.y += 0.25f * Strength;

        s->Velocity.x = sRandom.FloatSigned();
        s->Velocity.y = sRandom.Float() * Strength * 0.25f;
        s->Velocity.z = sRandom.FloatSigned();
        s->Velocity.Normalize();
        s->Velocity.Scale( 1.0f + Strength );
        s->Velocity.Scale( FRAME_RATIO );

        s->Size = LANDING_SIZE * Strength;
        s->Life = ( sRandom.Float() * 0.2f ) + Strength;
        s->DurationRatio = LANDING_DURATION_RATIO;
        s->Colour = DASH_COLOUR;
        ++( mNumActiveSparkles[ s->Texture ] );
    }
}
/*=============================================================================
Add a shockwave ring, which is basically a billboard with a texture which
expands from the center and then disappears. Really easy. Handy for things like
the jump-jump-stomp.
=============================================================================*/
void Sparkle::AddShockRing( const rmt::Vector& Position, float Strength )
{
    ActiveSparkle* s = GetInactiveSparkle();
    if( !s )
    {
        return;
    }
    s->Align = SA_Flat;
    s->Scale = SS_LinearUp;
    s->ColourAnim = SC_LinearDown;
    s->CornerFade = 0;
    s->Motion = SM_Linear;
    s->Texture = ST_Ring;
    s->Position = Position;
    s->Position.y += 0.25f;
    s->Velocity.Set( 0.0f, 0.0f, 0.0f );
    s->Size = SHOCK_RING_RADIUS * Strength;
    s->Life = 1.0f;
    s->DurationRatio = SHOCK_RING_SPEED;
    s->Colour = RING_COLOUR;
    ++( mNumActiveSparkles[ s->Texture ] );
}

/*=============================================================================
Add the leaves spray when the car drives over a hedge. The animation is like a
coins spawning.
=============================================================================*/
void Sparkle::AddLeaves( const rmt::Vector& Position, const rmt::Vector& Velocity, float Strength )
{
    int numParticles = int( sRandom.IntRanged( NUM_SPARK_PARTICLES, NUM_SPARK_PARTICLES * 2 ) * Strength ) + 1;
    for( int i = 0; i < numParticles; ++i )
    {
        ActiveSparkle* s = GetInactiveSparkle();
        if( !s )
        {
            return;
        }
        s->Align = SA_Rotation;
        s->Scale = SS_Constant;
        s->ColourAnim = SC_LinearAlphaDown;
        s->CornerFade = 0;
        s->Motion = SM_Gravity;
        s->Texture = ST_Leaf;
        s->Position = Position;
        s->Velocity.x = sRandom.FloatSigned();
        s->Velocity.y = sRandom.FloatSigned();
        s->Velocity.z = sRandom.FloatSigned();
        s->Velocity.Normalize();
        s->Velocity.Scale( SPARK_RADIUS * SPARK_DURATION_RATIO );
        s->Velocity.ScaleAdd(SPARK_DURATION_RATIO, Velocity );
        s->Velocity.Scale( FRAME_RATIO );
        s->Size = s->Velocity.Magnitude() * SPARK_VELOCITY_TO_SIZE_RATIO;
        s->Life = 1.0f + ( sRandom.FloatSigned() * 0.1f );
        s->DurationRatio = SPARK_DURATION_RATIO;
        s->Colour = LEAF_COLOUR;
        ++( mNumActiveSparkles[ s->Texture ] );
    }
}

/*=============================================================================
Add paint chips when upon vehicle-vehicle collision. The colour depends
upon the colour of the vehicle getting hit.
=============================================================================*/
void Sparkle::AddPaintChips( const rmt::Vector& Position, const rmt::Vector& Velocity, pddiColour Colour, float Strength )
{
    return;

    int numParticles = int( sRandom.IntRanged( NUM_PAINT_CHIPS, NUM_PAINT_CHIPS * 2 ) * Strength ) + 1;
    for( int i = 0; i < numParticles; ++i )
    {
        ActiveSparkle* s = GetInactiveSparkle();
        if( !s )
        {
            return;
        }

        const float VELOCITY_DAMPENING = 0.2f;

        s->Align = SA_Rotation;
        s->Scale = SS_Constant;
        s->ColourAnim = SC_LinearAlphaDown;
        s->CornerFade = 0;
        s->Motion = SM_Gravity;
        s->Texture = ST_Paint;
        s->Position = Position;
        s->Velocity.x = sRandom.FloatSigned();
        s->Velocity.y = sRandom.FloatSigned();
        s->Velocity.z = sRandom.FloatSigned();
        s->Velocity.Normalize();
        s->Velocity.Scale( VELOCITY_DAMPENING );

        s->Size = PAINT_CHIP_SIZE;
        s->Life = 1.0f + ( sRandom.FloatSigned() * 0.1f );
        s->DurationRatio = SPARK_DURATION_RATIO;
        const float COLOUR_DAMPENING = 0.5f;
        int red = static_cast< int >( static_cast< float >(Colour.Red()) * COLOUR_DAMPENING );
        int green = static_cast< int >( static_cast< float >(Colour.Green()) * COLOUR_DAMPENING );
        int blue = static_cast< int >( static_cast< float >(Colour.Blue()) * COLOUR_DAMPENING );
        s->Colour.Set( red, green, blue );
        ++( mNumActiveSparkles[ s->Texture ] );
    }
}
/*=============================================================================
Explosion of stars and smoke. Useful when the car hits a static.
=============================================================================*/
void Sparkle::AddStars( const rmt::Vector& Position, float Strength )
{
    int numParticles = NUM_STAR_PARTICLES + int( NUM_STAR_PARTICLES * Strength );
    for( int i = 0; i < numParticles; ++i )
    {
        ActiveSparkle* s = GetInactiveSparkle();
        if( !s )
        {
            return;
        }
        s->Align = SA_MotionStreak;
        s->Scale = SS_LinearDown;
        s->ColourAnim = SC_LinearAlphaDown;
        s->CornerFade = 0;
        s->Motion = SM_Linear;
        s->Position = Position;
        s->Velocity.x = sRandom.FloatSigned();
        s->Velocity.y = sRandom.FloatSigned();
        s->Velocity.z = sRandom.FloatSigned();
        s->Velocity.Normalize();
        s->Velocity.Scale( FRAME_RATIO * STAR_RADIUS );
        s->DurationRatio = STAR_DURATION_RATIO;
        s->Size = s->Velocity.Magnitude() * SPARK_VELOCITY_TO_SIZE_RATIO * SPARK_SIZE;
        s->Life = 1.0f;
        s->Texture = ST_Spark;
        s->Colour = STAR_COLOUR;
        ++( mNumActiveSparkles[ s->Texture ] );
        s = GetInactiveSparkle();
        if( !s )
        {
            return;
        }
        s->Align = SA_Rotation;
        s->Scale = SS_LinearGrow;
        s->ColourAnim = SC_LinearAlphaDown;
        s->CornerFade = 1;
        s->Motion = SM_Linear;
        s->Position = Position;
        s->Velocity.x = sRandom.FloatSigned();
        s->Velocity.y = sRandom.FloatSigned();
        s->Velocity.z = sRandom.FloatSigned();
        s->Velocity.Normalize();
        s->Velocity.Scale( FRAME_RATIO * STAR_DUST_RADIUS );
        s->DurationRatio = STAR_DUST_DURATION_RATIO;
        s->Size = STAR_DUST_SIZE;
        s->Life = 1.0f;
        s->Texture = ST_Puff;
        s->Colour = STAR_DUST_COLOUR;
        ++( mNumActiveSparkles[ s->Texture ] );
    }
}

/*=============================================================================
Upward spray of smoke. Used for car damage smoke.
Transform is the position of the vehicle and the offset is the position of the
emission of the smoke _relative_ to the vehicle. The velocity is the vehicle's
movement per frame.
=============================================================================*/
void Sparkle::AddSmoke( const rmt::Matrix& Transform, const rmt::Vector& Offset, const rmt::Vector& Velocity, float Strength )
{
    if( Strength <= 0.0f )
    {
        return;
    }
    rmt::Vector pos;
    Transform.Transform( Offset, &pos );
    // Do some distance type LODing. The further from the camera, the fewer particles.
    rmt::Vector camPos;
    GetSuperCamManager()->GetSCC(0)->GetActiveSuperCam()->GetPosition( &camPos );
    camPos.Sub( pos );
    int d = 0;
    if( GetGameplayManager()->GetGameType() != GameplayManager::GT_SUPERSPRINT )
    {
        d = int( rmt::Max( rmt::Abs( camPos.x ), rmt::Abs( camPos.z ) ) * 0.02f );
    }
    int numParticles = int( NUM_SMOKE_PARTICLES * Strength ) + 1 - d;
    if( numParticles <= 0 )
    {
        return;
    }
    for( int i = 0; i < numParticles; ++i )
    {
        ActiveSparkle* s = GetInactiveSparkle();
        if( !s )
        {
            return;
        }
        s->Align = SA_None;
        s->Scale = SS_LinearTriple;
        s->ColourAnim = SC_LinearAlphaDown;
        s->CornerFade = 0;
        s->Motion = SM_Drift;
        s->Position = pos;
        s->Position.x += sRandom.FloatSigned() * 0.15f;
        s->Position.z += sRandom.FloatSigned() * 0.15f;
        s->Position.y += sRandom.FloatSigned() * 0.15f;
        s->Velocity.x = sRandom.FloatSigned() * 2.0f;
        s->Velocity.z = sRandom.FloatSigned() * 2.0f;
        s->Velocity.y = 2.0f + ( sRandom.Float() * 2.0f );
        s->Velocity.Normalize();
        s->Velocity.Scale( FRAME_RATIO * ( 2.0f + ( 1.0f * Strength ) ) );
        //s->Velocity.ScaleAdd( s->Velocity, FRAME_RATIO, Velocity );
        s->Velocity.Add(Velocity);
        s->DurationRatio = 0.2f / 1.0f + Strength + sRandom.Float();
        s->Size = SMOKE_SIZE; // + ( ( Strength + sRandom.Float() ) * 0.1f );
        s->Life = 1.0f + ( Strength * 0.5f );
        int red = rmt::Clamp( int( ( LIGHT_SMOKE_COLOUR.Red() * ( 1.0f - Strength ) ) + ( DARK_SMOKE_COLOUR.Red() * Strength ) ), 0, 255 );
        int green = rmt::Clamp( int( ( LIGHT_SMOKE_COLOUR.Green() * ( 1.0f - Strength ) ) + ( DARK_SMOKE_COLOUR.Green() * Strength ) ), 0, 255 );
        int blue = rmt::Clamp( int( ( LIGHT_SMOKE_COLOUR.Blue() * ( 1.0f - Strength ) ) + ( DARK_SMOKE_COLOUR.Blue() * Strength ) ), 0, 255 );
        int alpha = rmt::Clamp( int( ( LIGHT_SMOKE_COLOUR.Alpha() * ( 1.0f - Strength ) ) + ( DARK_SMOKE_COLOUR.Alpha() * Strength ) ), 0, 255 );
        s->Colour.Set( red, green, blue, alpha );
        s->Texture = ST_SortedPuff;
        ++( mNumActiveSparkles[ s->Texture ] );
    }
}

/*=============================================================================
Update the position/animation of all the coins.
=============================================================================*/
void Sparkle::Update( unsigned int Elapsedms )
{
    float deltaSeconds = (float)Elapsedms * 0.001f;
    bool HUDOnly = GetGameFlow()->GetCurrentContext() == CONTEXT_PAUSE;

    for( int i = 0; i < mNumSparkles; ++i )
    {
        ActiveSparkle& s = mActiveSparkles[ i ];
        if(s.Active)
        {
            if(HUDOnly && s.HUD != 1)
            {
                continue;
            }
            if( s.Life < 0.0f )
            {
                if(s.HUD)
                {
                    s.HUD = 0;
                    --( mNumHUDSparkles[ s.Texture ] );
                }
                s.Active = 0;
                --(mNumActiveSparkles[ s.Texture ]);
            }
            if( s.Motion == SM_Decelerate )
            {
                s.Position.ScaleAdd( 2.0f - s.Life, s.Velocity );
            }
            else if ( s.Motion == SM_Gravity )
            {
                 s.Position.Add( s.Velocity );
                 const float GRAVITY = 0.02f;
                 s.Velocity.y -= static_cast<float>(Elapsedms) * GRAVITY;
            }
            else if( s.Motion == SM_Drift )
            {
                s.Position.Add( s.Velocity );
                s.Velocity.x *= DRIFT_DRAG;
                s.Velocity.z *= DRIFT_DRAG;
            }
            else
            {
                s.Position.Add( s.Velocity );
            }
            s.Life -= deltaSeconds * s.DurationRatio;
        }
    }
}

Sparkle::ActiveSparkle::ActiveSparkle() : Active( 0 ), HUD( 0 )
{}


/*=============================================================================
Render pass for the world sparkles. They are aligned to face the camera.
=============================================================================*/
void Sparkle::Render( eSparkleRenderMode Mode )
{
    unsigned short numWorldSparkles[ ST_NUM_TEXTURES ];
    int totalSparkles = 0;
    int ti;
    for( ti = 0; ti < mNumTextures; ++ti )
    {
        if( mpTextures[ ti ] )
        {
            if( Mode == SRM_IncludeSorted )
            {
                numWorldSparkles[ ti ] = mNumActiveSparkles[ ti ] + mNumActiveSparkles[ ti + ST_NUM_TEXTURES ] - mNumHUDSparkles[ ti ];
            }
            else if( Mode == SRM_ExcludeSorted )
            {
                numWorldSparkles[ ti ] = mNumActiveSparkles[ ti ] - mNumHUDSparkles[ ti ];
            }
            else
            {
                numWorldSparkles[ ti ] = mNumActiveSparkles[ ti + ST_NUM_TEXTURES ];
            }
        }
        else
        {
            numWorldSparkles[ ti ] = 0;
        }
        totalSparkles += numWorldSparkles[ ti ];
    }
    if( totalSparkles <= 0 )
    {
        return;
    }

    tCamera* camera = p3d::context->GetView()->GetCamera();
    rAssert( camera );
    const rmt::Matrix& camTransform = camera->GetCameraToWorldMatrix();
    rmt::Vector cameraDiagR = camTransform.Row( 1 );
    rmt::Vector cameraDiagL = cameraDiagR;
    cameraDiagR.Add( camTransform.Row( 0 ) );
    cameraDiagL.ScaleAdd( -1.0f, camTransform.Row( 0 ) );
    rmt::Vector offset;
    offset.Scale( -0.1f, camTransform.Row( 2 ) );

    bool oldZWrite = p3d::pddi->GetZWrite();
    pddiCompareMode oldZComp = p3d::pddi->GetZCompare();
    if( oldZWrite )
    {
        p3d::pddi->SetZWrite( false );
    }

    pddiPrimStream* sprite = 0;
 
    pddiShader* spriteShader = BootupContext::GetInstance()->GetSharedShader();
    rAssert( spriteShader );

    spriteShader->SetInt( PDDI_SP_ISLIT, 0 );
    spriteShader->SetInt( PDDI_SP_ALPHATEST, 1 );
    spriteShader->SetInt( PDDI_SP_SHADEMODE, PDDI_SHADE_FLAT );
    spriteShader->SetInt( PDDI_SP_FILTER, PDDI_FILTER_BILINEAR );
    for( ti = 0; ti < mNumTextures; ++ti )
    {
        if( numWorldSparkles[ ti ] <= 0 )
        {
            continue;
        }
        spriteShader->SetTexture( PDDI_SP_BASETEX, mpTextures[ ti ]->GetTexture() );
        if((ti == ST_Puff) || (ti == ST_Leaf))
        {
            spriteShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ALPHA );
            spriteShader->SetInt( PDDI_SP_ALPHATEST, 0 );
        }
        else
        {
            spriteShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ADD );
        }


        sprite = p3d::pddi->BeginPrims( spriteShader, PDDI_PRIM_TRIANGLES, PDDI_V_CT , numWorldSparkles[ ti ] * 6 );
        for( int i = 0; i < mNumSparkles; ++i )
        {
            ActiveSparkle& s = mActiveSparkles[ i ];
            if( ( s.Active == 0 ) || ( s.HUD == 1 ) )
            {
                continue;
            }
            unsigned short matchTexture = s.Texture;
            if( Mode == SRM_IncludeSorted )
            {
                matchTexture %= ST_NUM_TEXTURES;
            }
            else if( Mode == SRM_SortedOnly )
            {
                matchTexture -= ST_NUM_TEXTURES;
            }
            if( ( matchTexture != ti ) )
            {
                continue;
            }
            float life = rmt::Max( s.Life, 0.0f );
            float scale;
            // Triangle points:
            // 2 - 3
            // | \ |
            // 0 - 1
            tColour colour[ 4 ];
            rmt::Vector corners[ 4 ];

            // Calculate the scale first.
            if( s.Scale == SS_LinearDown )
            {
                scale = s.Size * life;
            }
            else if( s.Scale == SS_BulgeDown )
            {
                scale = s.Size * ( ( 1.0f - ( rmt::Abs( ( life * life ) - 0.5f ) * 2.0f ) ) + life );
            }
            else if( s.Scale == SS_LinearGrow )
            {
                scale = s.Size + ( s.Size * ( 1.0f - life ) * DASH_SIZE_INCREASE );
            }
            else if( s.Scale == SS_LinearTriple )
            {
                scale = s.Size + ( s.Size * 3.0f * ( 1.0f - rmt::Min( life, 1.0f ) ) );
            }
            else if( s.Scale == SS_LinearUp )
            {
                scale = s.Size * ( 1.0f - life );
            }
            else
            {
                scale = s.Size;
            }
            // Now do colour.
            if( s.ColourAnim == SC_LinearRedDownAtHalf )
            {
                float l = rmt::Clamp( life * 2.0f, 0.0f, 1.0f );
                float l_half = rmt::Clamp( life - 0.5f, 0.0f, 1.0f );
                colour[ 0 ].Set( rmt::FtoL( s.Colour.Red() * l ),
                                 rmt::FtoL( s.Colour.Green() * life ),
                                 rmt::FtoL( s.Colour.Blue() * l_half ) );
            }
            else if( s.ColourAnim == SC_LinearDownAtHalf )
            {
                float l = rmt::Clamp( life * 2.0f, 0.0f, 1.0f );
                colour[ 0 ].Set( rmt::FtoL( s.Colour.Red() * l ),
                                 rmt::FtoL( s.Colour.Green() * l),
                                 rmt::FtoL( s.Colour.Blue() * l ) );
            }
            else if( s.ColourAnim == SC_LinearAlphaDown )
            {
                float l = rmt::Clamp( life, 0.0f, 1.0f );
                colour[ 0 ].Set( s.Colour.Red(),
                                 s.Colour.Green(),
                                 s.Colour.Blue(),
                                 rmt::FtoL( s.Colour.Alpha() * l ) );
            }
            else
            {
                float l = rmt::Clamp( life, 0.0f, 1.0f );
                colour[ 0 ].Set( rmt::FtoL( s.Colour.Red() * l ),
                                 rmt::FtoL( s.Colour.Green() * l ),
                                 rmt::FtoL( s.Colour.Blue() * l ) );
            }
            if( s.CornerFade == 1 )
            {
                colour[ 1 ] = colour[ 0 ];
                colour[ 1 ].SetAlpha( int( colour[ 0 ].Alpha() * 0.75f ) );
                colour[ 2 ] = colour[ 1 ];
                colour[ 3 ] = colour[ 0 ];
                colour[ 3 ].SetAlpha( colour[ 0 ].Alpha() >> 1 );
            }
            else
            {
                for( int i = 1; i < 4; ++i )
                {
                    colour[ i ] = colour[ 0 ];
                }
            }

            // Now do corners two corners.
            if( s.Align == SA_MotionStreak )
            {
                // Align to motion.
                // Project travel onto plane of the camera.
                rmt::Vector travel = s.Velocity;
                travel.Normalize();
                float dot = travel.DotProduct( camTransform.Row( 2 ) );
                travel.ScaleAdd( -dot, camTransform.Row( 2 ) );
                dot = 1.0f - rmt::Abs( dot );
                corners[ 2 ].Set( -scale * dot, scale * SPARK_VERTICAL_SCALE, 0.0f );
                corners[ 3 ].Set( scale * dot, scale * SPARK_VERTICAL_SCALE, 0.0f );

                travel.Normalize();
                // Because textures like to be wider then they are taller
                //on the PS2, the spark textures is aligned down the X axis instead of Y.
                //so travel becomes the right in the matrix, the camera direction becomes facing,
                //and the up is a cross of those two.
                rmt::Vector cross;
                cross.CrossProduct( camTransform.Row( 2 ), travel );
                rmt::Matrix r( travel.x, travel.y, travel.z, 0.0f,
                               cross.x, cross.y, cross.z, 0.0f,
                               camTransform.Row( 2 ).x, camTransform.Row( 2 ).y, camTransform.Row( 2 ).z, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f );
                r.Transform( 2, &( corners[ 2 ] ), &( corners[ 2 ] ) );
            }
            else if( s.Align == SA_Rotation )
            {
                rmt::Vector diagR;
                diagR.Scale( scale, cameraDiagR );
                rmt::Vector diagL;
                diagL.Scale( scale, cameraDiagL );
                corners[ 2 ] = diagL;
                corners[ 3 ] = diagR;
                rmt::Matrix r;
                r.Identity();
                r.FillRotation( camTransform.Row( 2 ), rmt::PI_BY2 * life );
                r.Transform( 2, &( corners[ 2 ] ), &( corners[ 2 ] ) );
            }
            else if( s.Align == SA_Flat )
            {
                corners[ 2 ].Set( scale, 0.0f, -scale );
                corners[ 3 ].Set( scale, 0.0f,  scale );
            }
            else
            {
                rmt::Vector diagR;
                diagR.Scale( scale, cameraDiagR );
                rmt::Vector diagL;
                diagL.Scale( scale, cameraDiagL );
                corners[ 2 ] = diagL;
                corners[ 3 ] = diagR;
            }

            // Now mirror those two corners.
            corners[ 0 ].Scale( -1.0f, corners[ 3 ] );
            corners[ 1 ].Scale( -1.0f, corners[ 2 ] );

            // Now move corners into world position.
            rmt::Vector pos;
            pos.Add( s.Position, offset );
            for( int i = 0; i < 4; ++i )
            {
                corners[ i ].Add( pos );
            }

            // Now draw the triangle.
            // bottom left tri.
            sprite->Colour( colour[ 0 ] );
            sprite->UV( 0.0f, 0.0f );
            sprite->Coord( corners[ 0 ].x, corners[ 0 ].y, corners[ 0 ].z );
            sprite->Colour( colour[ 1 ] );
            sprite->UV( 1.0f, 0.0f );
            sprite->Coord( corners[ 1 ].x, corners[ 1 ].y, corners[ 1 ].z );
            sprite->Colour( colour[ 2 ] );
            sprite->UV( 0.0f, 1.0f );
            sprite->Coord( corners[ 2 ].x, corners[ 2 ].y, corners[ 2 ].z );

            // Top right tri.
            sprite->Colour( colour[ 3 ] );
            sprite->UV( 1.0f, 1.0f );
            sprite->Coord( corners[ 3 ].x, corners[ 3 ].y, corners[ 3 ].z );
            sprite->Colour( colour[ 2 ] );
            sprite->UV( 0.0f, 1.0f );
            sprite->Coord( corners[ 2 ].x, corners[ 2 ].y, corners[ 2 ].z );
            sprite->Colour( colour[ 1 ] );
            sprite->UV( 1.0f, 0.0f );
            sprite->Coord( corners[ 1 ].x, corners[ 1 ].y, corners[ 1 ].z );
        }
        p3d::pddi->EndPrims( sprite );
    }
    spriteShader->SetTexture( PDDI_SP_BASETEX, 0 );
    if( oldZWrite )
    {
        p3d::pddi->SetZWrite( true );
    }
}

/*=============================================================================
Rendering pass for the HUD. Used for when the coin flies up to the HUD counter.
We set the state once and do all the rendering.
=============================================================================*/
void Sparkle::HUDRender( void )
{
    if( !mpTextures )
    {
        return;
    }
    unsigned char totalSparkles = 0;
    unsigned int ti;
    for( ti = 0; ti < mNumTextures; ++ti )
    {
        totalSparkles += mNumHUDSparkles[ ti ];
    }
    if( totalSparkles <= 0 )
    {
        return;
    }
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
	p3d::stack->LoadIdentity();
	p3d::stack->Scale( 0.1f, 0.1f, 1.0f);
	p3d::pddi->SetProjectionMode( PDDI_PROJECTION_ORTHOGRAPHIC );
    pddiColour oldAmbient = p3d::pddi->GetAmbientLight();
    p3d::pddi->SetAmbientLight( pddiColour( 255, 255, 255 ) );

    pddiPrimStream* sprite = 0;
 
    pddiShader* spriteShader = BootupContext::GetInstance()->GetSharedShader();
    rAssert( spriteShader );

    spriteShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ADD );
    spriteShader->SetInt( PDDI_SP_ISLIT, 0 );
    spriteShader->SetInt( PDDI_SP_ALPHATEST, 1 );
    spriteShader->SetInt( PDDI_SP_SHADEMODE, PDDI_SHADE_FLAT );

    for( ti = 0; ti < mNumTextures; ++ti )
    {
        if( mNumHUDSparkles[ ti ] <= 0 )
        {
            continue;
        }
        spriteShader->SetTexture( PDDI_SP_BASETEX, mpTextures[ ti ]->GetTexture() );

        sprite = p3d::pddi->BeginPrims( spriteShader, PDDI_PRIM_TRIANGLES, PDDI_V_CT, mNumHUDSparkles[ ti ] * 6 );

        for( int i = 0; i < mNumSparkles; ++i )
        {
            ActiveSparkle& s = mActiveSparkles[ i ];
            if( s.Active == 0 || s.HUD == 0 || ( s.Texture != ti ) )
            {
                continue;
            }
            float life = s.Life;
            tColour colour;
            colour.Set( rmt::Clamp( int( 255 * life ), 0, 255 ),
                        rmt::Clamp( int( 255 * life ), 0, 255 ),
                        rmt::Clamp( int( ( 255 * life ) - 128 ), 0, 255 ) );
            float scale;
            scale = s.Size * life;
            rmt::Vector pos = s.Position;
            pos.Scale( 5.0f );

            // bottom left tri.
            sprite->Colour( colour );
            sprite->UV( 0.0f, 0.0f );
            sprite->Coord( pos.x - scale, pos.y - scale, pos.z );
            sprite->Colour( colour );
            sprite->UV( 1.0f, 0.0f );
            sprite->Coord( pos.x + scale, pos.y - scale, pos.z );
            sprite->Colour( colour );
            sprite->UV( 0.0f, 1.0f );
            sprite->Coord( pos.x - scale, pos.y + scale, pos.z );

            // Top right tri.
            sprite->Colour( colour );
            sprite->UV( 1.0f, 1.0f );
            sprite->Coord( pos.x + scale, pos.y + scale, pos.z );
            sprite->Colour( colour );
            sprite->UV( 0.0f, 1.0f );
            sprite->Coord( pos.x - scale, pos.y + scale, pos.z );
            sprite->Colour( colour );
            sprite->UV( 1.0f, 0.0f );
            sprite->Coord( pos.x + scale, pos.y - scale, pos.z );
        }
        p3d::pddi->EndPrims( sprite );
    }
    spriteShader->SetTexture( PDDI_SP_BASETEX, 0 );

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