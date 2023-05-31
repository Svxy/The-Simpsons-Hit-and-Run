//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        AnimatedIcon.cpp
//
// Description: Implement AnimatedIcon
//
// History:     10/3/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <radtime.hpp>
#include <raddebug.hpp>
#include <p3d/anim/multicontroller.hpp>
#include <p3d/effects/particlesystem.hpp>
#include <p3d/view.hpp>

//========================================
// Project Includes
//========================================
#include <mission/AnimatedIcon.h>

#include <memory/srrmemory.h>

#include <mission/gameplaymanager.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/worldrenderlayer.h>
#include <render/culling/worldscene.h>

#include <debug/profiler.h>

#include <main/game.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
AnimatedIcon* AnimatedIcon::sAnimatedIconPool = NULL;
unsigned int AnimatedIcon::sNumAllocated = 0;

// Setup default parameters for our watcher variables
#ifdef RAD_TUNE
float AnimatedIcon::sDbgMinArrowScale = MIN_ARROW_SCALE;
float AnimatedIcon::sDbgMaxArrowScale = MAX_ARROW_SCALE;
float AnimatedIcon::sDbgMinArrowScaleDist = MIN_ARROW_SCALE_DIST;
float AnimatedIcon::sDbgMaxArrowScaleDist = MAX_ARROW_SCALE_DIST;
bool AnimatedIcon::sDbgEnableArrowScaling = false;
#endif

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// AnimatedIcon::AnimatedIcon
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
AnimatedIcon::AnimatedIcon() :
    mDSGEntity( NULL ),
    mRenderLayer( RenderEnums::LevelSlot ),
    mFlags( 0 )
{
#ifdef RAD_TUNE
    AttachWatcherVariables();
#endif
}

//==============================================================================
// AnimatedIcon::~AnimatedIcon
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
AnimatedIcon::~AnimatedIcon()
{
    Deallocate();
}

//=============================================================================
// AnimatedIcon::Init
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* iconName, const rmt::Matrix& mat, bool render = true, bool oneShot = false )
//
// Return:      void 
//
//=============================================================================
void AnimatedIcon::Init( const char* iconName, const rmt::Matrix& mat, bool render, bool oneShot )
{
    MEMTRACK_PUSH_GROUP( "Mission - Animated Icon" );
    rAssert( iconName );
    rAssertMsg( mDSGEntity == NULL, "Never call this twice on the same object!!!! LEAK, LEAK!" );

    //========================= SET UP THE ICON

    HeapMgr()->PushHeap( GetGameplayManager()->GetCurrentMissionHeap() );

    SetUpContents( iconName );

    mDSGEntity = new AnimIconDSG;

    mDSGEntity->mTranslucent = true;

    rAssert( mDSGEntity != NULL );

    mDSGEntity->AddRef();

    //
    // The entity takes overship of m, so spake Devin
    //
    rmt::Matrix* m = new rmt::Matrix;
    rAssert( m );

    *m = mat;

    mDSGEntity->LoadSetUp( m, mAnimIcon.drawable, mAnimIcon.shadowDrawable );

    mRenderLayer = static_cast<RenderEnums::LayerEnum>(GetRenderManager()->rCurWorldRenderLayer());

    if ( render )
    {
        ShouldRender( true );
    }

    SetFlag( (Flag)ONE_SHOT, oneShot );

    SetUpEffects();

    HeapMgr()->PopHeap( GetGameplayManager()->GetCurrentMissionHeap() );

    MEMTRACK_POP_GROUP( "Mission - Animated Icon" );
}

//=============================================================================
// AnimatedIcon::Init
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* iconName, const rmt::Vector& pos, bool render, bool oneShot )
//
// Return:      void 
//
//=============================================================================
void AnimatedIcon::Init( const char* iconName, const rmt::Vector& pos, bool render, bool oneShot )
{
MEMTRACK_PUSH_GROUP( "Mission - Animated Icon" );
    rAssert( iconName );
    rAssertMsg( mDSGEntity == NULL, "Never call this twice on the same object!!!! LEAK, LEAK!" );

    //========================= SET UP THE ICON
    
    HeapMgr()->PushHeap( GetGameplayManager()->GetCurrentMissionHeap() );

    SetUpContents( iconName );

   	mDSGEntity = new AnimIconDSG;

    mDSGEntity->mTranslucent = true;

	rAssert( mDSGEntity != NULL );
    
    mDSGEntity->AddRef();

    mDSGEntity->SetName( iconName );

	//
	// The entity takes overship of m, so spake Devin
	//
	rmt::Matrix* m = new rmt::Matrix;
    rAssert( m );

    m->Identity();
	m->FillTranslate( pos );

    mDSGEntity->LoadSetUp( m, mAnimIcon.drawable, mAnimIcon.shadowDrawable );

    mRenderLayer = static_cast<RenderEnums::LayerEnum>(GetRenderManager()->rCurWorldRenderLayer());

    if ( render )
    {
        ShouldRender( true );
    }

    SetFlag( (Flag)ONE_SHOT, oneShot );

    SetUpEffects();

    HeapMgr()->PopHeap( GetGameplayManager()->GetCurrentMissionHeap() );

MEMTRACK_POP_GROUP( "Mission - Animated Icon" );
}

//=============================================================================
// AnimatedIcon::ScaleByCameraDistance
//=============================================================================
// Description: Converts all billboardquadgroups in the icon drawable to quads that are 
//              scaled by distance to camera
//
// Parameters:  (float minScale, float maxScale, float minDist, float maxDist)
//
// Return:      void 
//
//=============================================================================
void AnimatedIcon::ScaleByCameraDistance( float minScale, float maxScale, float minDist, float maxDist )
{
    // Default to no scaling
    mDSGEntity->SetScaleParameters( minScale, maxScale, minDist, maxDist ); 
}

//=============================================================================
// AnimatedIcon::Move
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& newPos )
//
// Return:      void 
//
//=============================================================================
void AnimatedIcon::Move( const rmt::Vector& newPos )
{
    if ( mRenderLayer == static_cast<RenderEnums::LayerEnum>(GetRenderManager()->rCurWorldRenderLayer()) )
    {
        rmt::Box3D oldBox;
        mDSGEntity->GetBoundingBox( &oldBox );

        mDSGEntity->pMatrix()->FillTranslate( newPos );

        if ( !GetFlag( (Flag)RENDERING ) )
        {
            ShouldRender( true );
        }

        GetRenderManager()->pWorldScene()->Move( oldBox, mDSGEntity );
        mDSGEntity->RecomputeShadowPosition();
    }
}

//=============================================================================
// AnimatedIcon::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedMilliseconds )
//
// Return:      void 
//
//=============================================================================
void AnimatedIcon::Update( unsigned int elapsedMilliseconds )
{
    rmt::Sphere iconBoundingSphere;
    if ( mDSGEntity )
    {
        mDSGEntity->GetBoundingSphere( &iconBoundingSphere );
        mDSGEntity->m_Visible = false;
    }
    else
    {
        iconBoundingSphere.centre = rmt::Vector(0,0,0);
        iconBoundingSphere.radius = 1.0f;
    }

    if ( GetFlag( (Flag)RENDERING ) && GetGameplayManager()->TestPosInFrustrumOfPlayer(iconBoundingSphere.centre, 0, iconBoundingSphere.radius))
    {    
        if ( mDSGEntity )
        {
            mDSGEntity->m_Visible = true;
        }

        if ( mAnimIcon.multiController )
        {
            BEGIN_PROFILE( mAnimIcon.multiController->GetName() );
            //This allows me to ignore the extra updates caused by being part of 
            //a substep.
            unsigned int frame = GetGame()->GetFrameCount();
            if ( (mAnimIcon.multiController)->GetLastFrameUpdated() != frame )
            {
                (mAnimIcon.multiController)->Advance( (float)elapsedMilliseconds );
                (mAnimIcon.multiController)->SetLastFrameUpdated( frame );
                // Handle particle systems
                if ( mAnimIcon.effectIndex != -1 )
                {
                    tCompositeDrawable* compDraw = static_cast< tCompositeDrawable* >( mAnimIcon.drawable );
                    tCompositeDrawable::DrawableEffectElement* effectElement = static_cast< tCompositeDrawable::DrawableEffectElement* > ( compDraw->GetDrawableElement( mAnimIcon.effectIndex ) );
                    tParticleSystem* system = static_cast< tParticleSystem* >( effectElement->GetDrawable() );
                    if ( system )
                    {
                        rmt::Matrix mat;
                        mat.Identity();
                        system->Update( &mat );
                    }   
                }

                if ( GetFlag( (Flag)ONE_SHOT ) && (mAnimIcon.multiController)->LastFrameReached() > 0 )
                {
                    //This is a way to remove non-cyclic animations.  Beware,
                    //if the animation is intended to be cyclic it will disappear...
                    ShouldRender( false );
                }
            }
            END_PROFILE( mAnimIcon.multiController->GetName() );
        }
        
        if ( mAnimIcon.shadowController != NULL )
        {
            mAnimIcon.shadowController->Advance( (float)elapsedMilliseconds );
        }

#ifdef RAD_TUNE
        if ( sDbgEnableArrowScaling )
        {
            // Jeff P. wants to have watcher variables to tune the AI scaling.
            // If we are in tune mode, lets override the input parameters and set them to be
            // the values of some static watcher variables
            ScaleByCameraDistance( sDbgMinArrowScale, sDbgMaxArrowScale, sDbgMinArrowScaleDist, sDbgMaxArrowScaleDist );
        }   
#endif
    }
}

//=============================================================================
// AnimatedIcon::Reset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void AnimatedIcon::Reset()
{
    if ( mAnimIcon.multiController )
    {
        (mAnimIcon.multiController)->Reset();
    }
}

//=============================================================================
// AnimatedIcon::ShouldRender
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool shouldRender )
//
// Return:      void 
//
//=============================================================================
void AnimatedIcon::ShouldRender( bool shouldRender )
{
    if ( shouldRender && !GetFlag( (Flag)RENDERING ) )
    {
        (reinterpret_cast<WorldRenderLayer*>(GetRenderManager()->mpLayer( mRenderLayer )))->pWorldScene()->Add( mDSGEntity );
        SetFlag( (Flag)RENDERING, true );
    }
    else if ( !shouldRender && GetFlag( (Flag)RENDERING ) )
    {
        (reinterpret_cast<WorldRenderLayer*>(GetRenderManager()->mpLayer( mRenderLayer )))->pWorldScene()->RemoveQuietFail( mDSGEntity );
        SetFlag( (Flag)RENDERING, false );
    }
}

//=============================================================================
// AnimatedIcon::GetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector& pos )
//
// Return:      void 
//
//=============================================================================
void AnimatedIcon::GetPosition( rmt::Vector& pos )
{
    mDSGEntity->GetPosition( &pos );
}

//=============================================================================
// ::operator new
//=============================================================================
// Description: Comment
//
// Parameters:  ( size_t size )
//
// Return:      void
//
//=============================================================================
void* AnimatedIcon::operator new( size_t size )
{
    rAssert( sAnimatedIconPool != NULL );
    rAssert( sNumAllocated < MAX_ICONS );

    void* data = NULL;
    if ( sNumAllocated < MAX_ICONS )
    {
        unsigned int i;
        for ( i = 0; i < MAX_ICONS; ++i )
        {
            if ( !sAnimatedIconPool[ i ].mAllocated )
            {
                sAnimatedIconPool[ i ].mAllocated = true;
                data = static_cast<void*>(&sAnimatedIconPool[ i ]);
                ++sNumAllocated;
                break;
            }
        }
    }

    return data;
}

//=============================================================================
// ::operator new
//=============================================================================
// Description: Comment
//
// Parameters:  ( size_t size, GameMemoryAllocator allocator )
//
// Return:      void
//
//=============================================================================
void* AnimatedIcon::operator new( size_t size, GameMemoryAllocator allocator )
{
    return new AnimatedIcon();
}

//=============================================================================
// ::operator delete
//=============================================================================
// Description: Comment
//
// Parameters:  ( void* mem )
//
// Return:      void 
//
//=============================================================================
void AnimatedIcon::operator delete( void* mem )
{
    AnimatedIcon* icon = static_cast<AnimatedIcon*>( mem );

    unsigned int i;
    for ( i = 0; i < MAX_ICONS; ++i )
    {
        if ( icon == &sAnimatedIconPool[ i ] )
        {
            icon->Deallocate();
            sNumAllocated--;
            break;
        }
    }

    rAssert( i < MAX_ICONS );
}

//=============================================================================
// void operator delete
//=============================================================================
// Description: Comment
//
// Parameters:  ( void* mem, GameMemoryAllocator allocator )
//
// Return:      void 
//
//=============================================================================
void AnimatedIcon::operator delete( void* mem, GameMemoryAllocator allocator )
{
    AnimatedIcon::operator delete( mem );
}

//=============================================================================
// AnimatedIcon::InitAnimatedIcons
//=============================================================================
// Description: Comment
//
// Parameters:  ( GameMemoryAllocator allocator )
//
// Return:      void 
//
//=============================================================================
void AnimatedIcon::InitAnimatedIcons( GameMemoryAllocator allocator )
{
    rAssert( sAnimatedIconPool == NULL );
    
    if ( sAnimatedIconPool == NULL )
    {
        #ifdef RAD_GAMECUBE
                HeapMgr()->PushHeap( GMA_GC_VMM );
        #else
                HeapMgr()->PushHeap( allocator );
        #endif
        sAnimatedIconPool = new AnimatedIcon[ MAX_ICONS ];

        unsigned int i;
        for ( i = 0; i < MAX_ICONS; ++i )
        {
            sAnimatedIconPool[ i ].mAllocated = false;
        }

        #ifdef RAD_GAMECUBE
                HeapMgr()->PopHeap( GMA_GC_VMM );
        #else
                HeapMgr()->PopHeap( allocator );
        #endif
    }
}

//=============================================================================
// AnimatedIcon::ShutdownAnimatedIcons
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void AnimatedIcon::ShutdownAnimatedIcons()
{
    rAssert( sNumAllocated == 0 );

    //Paranoia Test
#ifdef RAD_DEBUG
    unsigned int i;
    for ( i = 0; i < MAX_ICONS; ++i )
    {
        rAssert( sAnimatedIconPool[ i ].mDSGEntity == NULL );
    }
#endif

    delete[] sAnimatedIconPool;
    sAnimatedIconPool = NULL;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// AnimatedIcon::SetFlag
//=============================================================================
// Description: Comment
//
// Parameters:  ( Flag flag, bool value )
//
// Return:      void 
//
//=============================================================================
void AnimatedIcon::SetFlag( Flag flag, bool value )
{
    if ( value )
    {   
        mFlags |= ( 1 << flag );
    }
    else
    {
        mFlags &= ~( 1 << flag );
    }    
}

//=============================================================================
// AnimatedIcon::GetFlag
//=============================================================================
// Description: Comment
//
// Parameters:  ( Flag flag )
//
// Return:      bool 
//
//=============================================================================
bool AnimatedIcon::GetFlag( Flag flag ) const
{
    return ( (mFlags & ( 1 << flag )) > 0 );
}

//=============================================================================
// AnimatedIcon::Deallocate
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void AnimatedIcon::Deallocate()
{
    if ( mDSGEntity )
    {
        //Scary, scary!
        if ( GetFlag( (Flag)RENDERING ) )
        {
            ShouldRender(false);
        }

        mDSGEntity->Release();
    }

    mFlags = 0;
    mAllocated = false;

    mDSGEntity = NULL;

    tRefCounted::Release(mAnimIcon.drawable);
    tRefCounted::Release(mAnimIcon.multiController);
    tRefCounted::Release(mAnimIcon.shadowController);
    tRefCounted::Release(mAnimIcon.shadowDrawable);
}

//=============================================================================
// AnimatedIcon::SetUpContents
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* iconName )
//
// Return:      void
//
//=============================================================================
void AnimatedIcon::SetUpContents( const char* iconName )
{
    tRefCounted::Assign(mAnimIcon.drawable, p3d::find<tDrawable>( iconName ));
    if ( mAnimIcon.drawable == NULL )
    {
        rReleasePrintf( "ERORR!!!!!  Can not init Animicon with missing (%s) drawable!!!!\n", iconName );
        tRefCounted::Assign(mAnimIcon.drawable, p3d::find<tDrawable>( "triggersphere_000" ));
    }

    rAssert( mAnimIcon.drawable != NULL );

    char controllerName[256];
    sprintf( controllerName, "%s_controller", iconName );

    tRefCounted::Assign(mAnimIcon.multiController, p3d::find<tMultiController>( controllerName ) );

    // Try and find the shadows (Collector cards have them, others are optional)
    // Append _shadow to the icon name to get the shadow name
    char shadowDrawableName[256];
    sprintf( shadowDrawableName, "%s_shadow", iconName );
    tRefCounted::Assign(mAnimIcon.shadowDrawable, p3d::find<tDrawable>( shadowDrawableName ));
    // Now try and find the shadow multicontroller
    char shadowControllerName[256];
    sprintf( shadowControllerName, "%s_controller", shadowDrawableName );
    tRefCounted::Assign(mAnimIcon.shadowController, p3d::find<tMultiController>( shadowControllerName ) );
}

//=============================================================================
// AnimatedIcon::SetUpEffects
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void AnimatedIcon::SetUpEffects()
{
    //Only do this once.
    rAssert( mAnimIcon.effectIndex == -1 );

    // Try and find particle systems
    tCompositeDrawable* compDraw = dynamic_cast< tCompositeDrawable* >( mAnimIcon.drawable );
    if ( compDraw )
    {
        for ( int i = 0 ; i < compDraw->GetNumDrawableElement() ; i++ )
        {
            tCompositeDrawable::DrawableEffectElement* effectElement = dynamic_cast< tCompositeDrawable::DrawableEffectElement* > ( compDraw->GetDrawableElement( i ) );
            if (effectElement)
            {             
                tEffect* effect = static_cast< tEffect* >( effectElement->GetDrawable() );
                if ( effect != NULL )
                {
                    tParticleSystem* ps = static_cast< tParticleSystem* >( effect );
                    tParticleSystemFactory* factory = static_cast< tParticleSystemFactory* >( ps->GetFactory() );
                    factory->SetAlwaysUpdateAfterDisplay( false );
                    ps->ConvertEmittersToLocal();
                    mAnimIcon.effectIndex = i;
                    break;
                }
            }
        }
    }
}

#ifdef RAD_TUNE
void AnimatedIcon::AttachWatcherVariables()
{
    static bool sDbgArrowsAttachedToWatcher = false; // has the watcher variables been 
    // attached already ? dont attach them more than once
    if ( sDbgArrowsAttachedToWatcher == false )
    {
        const char* ANIMATED_ICON_DBG_NAMESPACE = "Animated Icons";

        radDbgWatchAddBoolean( &sDbgEnableArrowScaling, "Enable Arrow Scaling", ANIMATED_ICON_DBG_NAMESPACE );
        radDbgWatchAddFloat( &sDbgMinArrowScale, "Min Arrow Scale", ANIMATED_ICON_DBG_NAMESPACE,  NULL, NULL, 0, 20.0f );     
        radDbgWatchAddFloat( &sDbgMaxArrowScale, "Max Arrow Scale", ANIMATED_ICON_DBG_NAMESPACE,  NULL, NULL, 0, 20.0f );     
        radDbgWatchAddFloat( &sDbgMinArrowScaleDist, "Min Arrow Scale Distance", ANIMATED_ICON_DBG_NAMESPACE,  NULL, NULL, 0, 250.0f );     
        radDbgWatchAddFloat( &sDbgMaxArrowScaleDist, "Max Arrow Scale Distance", ANIMATED_ICON_DBG_NAMESPACE,  NULL, NULL, 0, 250.0f );     
    }
    sDbgArrowsAttachedToWatcher = true;
}

#endif

AnimatedIcon::AnimIconDSG::AnimIconDSG():
m_ScalingEnabled( false )
{
    
    // Default to no scaling
    SetScaleParameters( 1.0f, 1.0f, 1.0f, 1.0f );
}

AnimatedIcon::AnimIconDSG::~AnimIconDSG(){

}

float AnimatedIcon::AnimIconDSG::CalcScale()
{
    rmt::Vector objPosition;
    GetPosition( &objPosition );
    // Determine the distance to the camera
    rmt::Vector camPosition;
    p3d::context->GetView()->GetCamera()->GetWorldPosition( &camPosition );
    float distToBillboard = (camPosition - objPosition ).Magnitude();
    float billboardScale = m_Slope * ( distToBillboard - m_NearDist ) + m_MinSize;
    billboardScale = rmt::Clamp( billboardScale, m_MinSize, m_MaxSize );

    return billboardScale;
}

void AnimatedIcon::AnimIconDSG::SetScaleParameters( float minSize, float maxSize, float nearDist, float farDist )
{
    // Compute m
    // The slope of the equation determining icon scaling

    if ( ( farDist - nearDist ) > 0.01f )
    {
        // m = ( maxSize - 1.0f ) / ( far - near )
        m_MaxSize = maxSize;
        m_MinSize = minSize;
        m_NearDist = nearDist;
        m_Slope = ( maxSize - minSize ) / ( farDist - nearDist );  
        m_ScalingEnabled = true;
    }
    else
    {
        m_ScalingEnabled = false;
    }

}

// Display with scaling. 
void AnimatedIcon::AnimIconDSG::Display()
{
    if( m_Visible )
    {
        extern bool g_ParticleLOD;
        g_ParticleLOD = true;
        bool wasBBQManagerEnabled = BillboardQuadManager::sEnabled;
        BillboardQuadManager::Enable();
        if ( m_ScalingEnabled )
        {
            float scale = CalcScale();
            rmt::Matrix scaleMat;
            scaleMat.Identity();
            scaleMat.FillScale( scale );

            // Apply rotation/transform THEN scale to the local object
            rmt::Matrix origMatrix = *mpMatrix;
            mpMatrix->Mult( scaleMat, origMatrix );

            InstStatEntityDSG::Display();

            *mpMatrix = origMatrix;
        }
        else
        {
            InstStatEntityDSG::Display();
        }
        if ( wasBBQManagerEnabled == false )
        {
            BillboardQuadManager::Disable();
        }
        g_ParticleLOD = false;
    }
}
