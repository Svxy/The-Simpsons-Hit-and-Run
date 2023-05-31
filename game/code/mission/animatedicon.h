//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        animatedicon.h
//
// Description: Blahblahblah
//
// History:     10/3/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef ANIMATEDICON_H
#define ANIMATEDICON_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>
#include <render/enums/renderenums.h>
#include <memory/srrmemory.h>
#include <render/DSG/inststatentitydsg.h>

//========================================
// Forward References
//========================================
class tCompositeDrawable;
class tDrawable;
class tMultiController;
class InstStatEntityDSG;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

static char* EXCLAMATION = "exclamation";
static const char* GIFT  = "gift";
static const char* ARROW = "arrow";
static const char* ARROW_CHASE = "arrow_evade";
static const char* ARROW_EVADE = "arrow_chase";
static const char* ARROW_RACE = "arrow_race";
static const char* ARROW_DESTROY = "arrow_destroy";

// Constants controlling the way the arrows scale as they get go farther away
const float MIN_ARROW_SCALE = 1.8f;
const float MAX_ARROW_SCALE = 5.0f;
const float MIN_ARROW_SCALE_DIST = 10.0f;
const float MAX_ARROW_SCALE_DIST = 80.0f;



class AnimatedIcon
{
public:
    enum { MAX_ICONS = 100 };

    AnimatedIcon();
	virtual ~AnimatedIcon();

    void Init( const char* iconName, const rmt::Vector& pos, bool render = true, bool oneShot = false );
    void Init( const char* iconName, const rmt::Matrix& mat, bool render = true, bool oneShot = false );

    // Convert the icon's billboards to be scaled by the distance to the camera
    // Scaling is linear between minDist and maxDist with a scaling between minScale and maxScale
    void ScaleByCameraDistance( float minScale, float maxScale, float minDist, float maxDist );

    void Move( const rmt::Vector& newPos );
    void Update( unsigned int elapsedMilliseconds );
    void Reset();
    void ShouldRender( bool shouldRender );
    bool IsRendering() const;

    void GetPosition( rmt::Vector& pos );

    static void* operator new( size_t size );
    static void* operator new( size_t size, GameMemoryAllocator allocator );
    static void operator delete( void* mem );
    static void operator delete( void* mem, GameMemoryAllocator allocator );

    static void InitAnimatedIcons( GameMemoryAllocator allocator );
    static void ShutdownAnimatedIcons();

    // A class that has useful functions for scaling an object after local
    // transformations but before local->world is applied
    class AnimIconDSG : public InstStatEntityDSG
    {
    public:
        AnimIconDSG();
        virtual ~AnimIconDSG();
        virtual void Display();
        // minSize - icon will never be smaller than this
        // maxSize - icon will never be bigger than this
        // nearDist - the distance at which size will be minSize
        // farDist - distance where size will be maxSize
        // In between, it is scaled linearly
        void SetScaleParameters( float minSize, float maxSize, float nearDist, float farDist );
        bool m_Visible;

    private:

        // Calculates the distance to camera and returns a scalar scale value
        // indicating how much scale to apply to the object
        float CalcScale();
        float m_Slope; // Slope of the scaling equation
        // m_Slope = ( m_MaxSize - m_MinSize ) / ( far - m_NearDist )
        float m_MaxSize;  // scale factor maximum
        float m_MinSize;  // scale factor minimum
        float m_NearDist; // distance where the scaling is at minSize
        bool m_ScalingEnabled;
    };

private:
    static AnimatedIcon* sAnimatedIconPool;
    static unsigned int sNumAllocated;

    struct Icon
    {
        Icon() : drawable( NULL ), multiController( NULL ), effectIndex( -1 ), shadowDrawable( NULL ), shadowController( NULL ) {};
        tDrawable* drawable;
        tMultiController* multiController;
        int effectIndex;

        tDrawable* shadowDrawable;
        tMultiController* shadowController;
    };

    AnimIconDSG* mDSGEntity;
    Icon mAnimIcon;
    RenderEnums::LayerEnum mRenderLayer;
    
    enum Flag
    {
        ONE_SHOT,
        RENDERING
    };

    unsigned int mFlags;

    char mAllocated; //Slighty wasteful due to padding.

    void SetFlag( Flag flag, bool value );
    bool GetFlag( Flag flag ) const;

    void Deallocate();

    void SetUpContents( const char* iconName );
    void SetUpEffects();

    // Jeff. P's tune mode watcher variables for determining the desired 
    // arrow scaling parameters
#ifdef RAD_TUNE
    static float sDbgMinArrowScale;
    static float sDbgMaxArrowScale;
    static float sDbgMinArrowScaleDist;
    static float sDbgMaxArrowScaleDist;
    static bool sDbgEnableArrowScaling;
    static void AttachWatcherVariables();
#endif

    //Prevent wasteful constructor creation.
	AnimatedIcon( const AnimatedIcon& animatedicon );
	AnimatedIcon& operator=( const AnimatedIcon& animatedicon );
};

//=============================================================================
// AnimatedIcon::IsRendering
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool AnimatedIcon::IsRendering() const
{ 
    return GetFlag( RENDERING );
};

#endif //ANIMATEDICON_H
