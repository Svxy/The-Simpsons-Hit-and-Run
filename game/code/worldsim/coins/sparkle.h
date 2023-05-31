//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File: coinmanager.h
//
// Description: Sparkle effect looks after drawing sparkle effects. It bundles
//              them together under one set up call to PDDI.
//
// History:     29/01/2003 + Created -- James Harrison
//
//=============================================================================

#ifndef SPARKLE_H
#define SPARKLE_H

//========================================
// Nested Includes
//========================================
#include <radmath/vector.hpp>
#include <radmath/matrix.hpp>
#include <radmath/random.hpp>
#include <render/enums/renderenums.h>

//========================================
// Forward References
//========================================
class tDrawable;
class tTexture;

//=============================================================================
//
// Synopsis:    Creating, place, animate sparkles in the world. Handy for coin
//              collection trails, etc.
//
//=============================================================================
static const int DEFAULT_NUM_SPARKLES = 150;

class Sparkle
{
public:
    enum eSparkleTextures
    {
        ST_Sparkle, // 0
        ST_Spark,   // 1
        ST_Puff,    // 2
        ST_Leaf,    // 3
        ST_BlueSparkle, // 4 - used for gags.
        ST_Paint,   // 5
        ST_Ring,    // 6
        ST_NUM_TEXTURES,
        ST_SortedSparkle,
        ST_SortedSpark,
        ST_SortedPuff,
        ST_SortedLeaf,
        ST_SortedBlueSparkle,
        ST_SortedPaint,
        ST_SortedRing
    };

    Sparkle( unsigned char NumTextures = 1, unsigned short NumSparkles = DEFAULT_NUM_SPARKLES );
	~Sparkle();

  	static Sparkle* GetInstance( void );
	static Sparkle* CreateInstance( unsigned char NumTextures = ST_NUM_TEXTURES, unsigned short NumSparkles  = DEFAULT_NUM_SPARKLES );
    static void DestroyInstance( void );

    void Init( void );
    void Destroy( void );

    enum eSparkleEffect
    {
        SE_Trail,
        SE_Glint,
        SE_HUDGlint,
        SE_Vanish
    };

    void AddSparkle( const rmt::Vector& Position, float Size, float Duration, const rmt::Vector& Velocity, eSparkleEffect Effect  );
    void AddGagSparkle(const rmt::Vector& Position, float Size, float Strength, unsigned int Caller); // See notes for information on the Caller.
    void AddSparks( const rmt::Vector& Position, const rmt::Vector& Velocity, float Strength );
    void AddDash( const rmt::Vector& Position, const rmt::Vector& Velocity, float Strength ); // NOTE: Velocity is direction of character travel.
    void AddLanding( const rmt::Vector& Position, float Strength );
    void AddLeaves( const rmt::Vector& Position, const rmt::Vector& Velocity, float Strength );
    void AddPaintChips( const rmt::Vector& Position, const rmt::Vector& Velocity, pddiColour colour, float Strength );
    void AddShockRing( const rmt::Vector& Position, float Strength );
    void AddStars( const rmt::Vector& Position, float Strength );
    void AddBottomOut( const rmt::Vector& Position );
    void AddSmoke( const rmt::Matrix& Transform, const rmt::Vector& Offset, const rmt::Vector& Velocity, float Strength );

    void Update( unsigned int Elapsedms ); // Elapsed milliseconds.
    enum eSparkleRenderMode
    {
        SRM_IncludeSorted,
        SRM_ExcludeSorted,
        SRM_SortedOnly
    };
    // Note that the RenderSorted Draws all the particles tagged as being sorted. This is a pretty crude implementation and designed just the fix the car smoke.
    void Render( eSparkleRenderMode Mode = SRM_IncludeSorted );
    void HUDRender( void ); // Draw any HUD sparkles.

    void SetTexture( unsigned char Index, tTexture* Texture );

    static rmt::Randomizer sRandom;
    static bool sRandomSeeded;

protected:
    //Prevent wasteful constructor creation.
	Sparkle( const Sparkle& That );
	Sparkle& operator=( const Sparkle& That );

    struct ActiveSparkle
    {
        ActiveSparkle();
        rmt::Vector Position;
        rmt::Vector Velocity;
        float Size;
        float DurationRatio;
        float Life;
        pddiColour Colour;
        unsigned int Active : 1;
        unsigned int HUD : 1;
        unsigned int Texture : 4;
        unsigned int Align : 2;
        unsigned int Scale : 3;
        unsigned int ColourAnim : 3;
        unsigned int CornerFade : 1;
        unsigned int Motion : 2;
    };
    enum eSparkleMotion
    {
        SM_Linear,
        SM_Decelerate,
        SM_Gravity,
        SM_Drift
    };
    enum eSparkleColour
    {
        SC_LinearDown,
        SC_LinearDownAtHalf,
        SC_LinearAlphaDown,
        SC_LinearRedDownAtHalf,
        SC_BulgeDown
    };
    enum eSparkleScale
    {
        SS_Constant,
        SS_LinearDown,
        SS_BulgeDown,
        SS_LinearUp,
        SS_LinearGrow,
        SS_LinearTriple
    };
    enum eSparkleAlign
    {
        SA_None,
        SA_MotionStreak,
        SA_Flat,
        SA_Rotation
    };
    ActiveSparkle* GetInactiveSparkle( void );
    static Sparkle* spInstance;
    ActiveSparkle* mActiveSparkles;
    tTexture** mpTextures;
    unsigned short mNumSparkles;
    unsigned short mNextInactiveSparkle;
    unsigned short* mNumActiveSparkles;
    unsigned short* mNumHUDSparkles;
    unsigned char mNumTextures;
};

// A little syntactic sugar for getting at this singleton.
inline Sparkle* GetSparkleManager() { return( Sparkle::GetInstance() ); }

#endif // #ifndef SPARKLE_H
