//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        fmvplayer.cpp
//
// Description: Implement FMVPlayer
//
// History:     17/04/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

#include <radplatform.hpp>
#include <radsound.hpp>
#include <raddebug.hpp>
#include <radmovie2.hpp>
#include <radcontroller.hpp>
#include <radfile.hpp>
#include <radmemory.hpp>
//========================================
// Project Includes
//========================================
#include <main/game.h>
#include <main/platform.h>
#include <gameflow/gameflow.h>
#include <presentation/fmvplayer/FMVplayer.h>
#include <input/inputmanager.h>
#include <sound/soundmanager.h>
#include <memory/srrmemory.h>
#include <contexts/bootupcontext.h>
#include <presentation/fmvplayer/fmvplayer.h>
#include <presentation/fmvplayer/fmvuserinputhandler.h>
#include <presentation/presevents/fmvevent.h>
#include <presentation/gui/guitextbible.h>
#include <events/eventmanager.h>

#include <render/RenderManager/RenderManager.h>
#include <p3d/utility.hpp>
#include <p3d/matrixstack.hpp>
#include <p3d/context.hpp>
#include <radmemory.hpp>
#include <radmemorymonitor.hpp>
#include <pddi/pddi.hpp>

#include <string.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

#define MOVIE_MAX_WIDTH 640
#ifdef PAL
    #define MOVIE_MAX_HEIGHT 512
#else
    #define MOVIE_MAX_HEIGHT 480
#endif
#define MOVIE_ENCODED_VIDEO_BUFFER_SIZE ( 512 * 1024 )
#define MOVIE_PRIMARY_AUDIO_BUFFER_SIZE 2000
#define MOVIE_SECONDARY_AUDIO_BUFFER_SIZE 1000
#define MOVIE_AUDIO_BUFFER_SIZE_TYPE IRadSoundHalAudioFormat::Milliseconds

#ifdef RAD_XBOX
static const float VOLUME_MULTIPLIER = 0.75f;
#else
static const float VOLUME_MULTIPLIER = 1.0f;
#endif

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// FMVPlayer::FMVPlayer
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
FMVPlayer::FMVPlayer() :
    m_refIRadMoviePlayer( NULL ),
    mElapsedTime( 0.0f ),
	mFadeOut(-1.0f)
{
    m_UserInputHandler = new FMVUserInputHandler;
    m_UserInputHandler->AddRef();
}

//==============================================================================
// FMVPlayer::~FMVPlayer
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
FMVPlayer::~FMVPlayer()
{
    m_UserInputHandler->Release();
    m_UserInputHandler = NULL;

    if( m_refIRadMoviePlayer != NULL )
    {
        m_refIRadMoviePlayer->Unload( );
        m_refIRadMoviePlayer = NULL;
    }
}

//=============================================================================
// FMVPlayer::LoadData
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* fileName, bool bInInventory )
//
// Return:      void 
//
//=============================================================================
void FMVPlayer::LoadData( const char* fileName, bool bInInventory, void* pUserData ) 
{
    if( GetState() == ANIM_IDLE || GetState() == ANIM_LOADING )
    {
        GetSoundManager()->StopForMovie();
        while( !( GetSoundManager()->IsStoppedForMovie() ) )
        {
            ::radMovieService2( );
            ::radFileService( );
            SoundManager::GetInstance()->Update();
            SoundManager::GetInstance()->UpdateOncePerFrame( 0, NUM_CONTEXTS, false );
        }

        GameMemoryAllocator allocator = GMA_LEVEL_MOVIE;
        unsigned int audioIndex = 0;
        FMVEvent::FMVEventData* data = reinterpret_cast<FMVEvent::FMVEventData*>(pUserData);
        if( data )
        {
            allocator = data->Allocator;

            if ( allocator >= GMA_ANYWHERE_IN_LEVEL )
            {
                ::SetupAllocatorSearch( allocator );
                allocator = GMA_ALLOCATOR_SEARCH;
            }

            audioIndex = data->AudioIndex;

            if( data->KillMusic )
            {
                GetEventManager()->TriggerEvent( EVENT_STOP_THE_MUSIC );
            }
        }
        HeapMgr()->PushHeap( allocator );
        Initialize( allocator );
        m_refIRadMoviePlayer->Load( fileName, audioIndex );
		mDriveFinished = false;
        Game::GetInstance()->GetPlatform()->GetHostDrive()->AddCompletionCallback( this, 0 );

        while( !mDriveFinished )
        {
    		::radMovieService2( );
            ::radFileService( );
            SoundManager::GetInstance()->Update();
            SoundManager::GetInstance()->UpdateOncePerFrame( 0, NUM_CONTEXTS, false );
        }
        HeapMgr()->PopHeap(allocator);
        SetState( ANIM_LOADED );
    }
}

//=============================================================================
// FMVPlayer::Play
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FMVPlayer::Play()
{
    if(( GetState() == ANIM_LOADED ) && ( m_refIRadMoviePlayer != NULL ))
    {
        AnimationPlayer::Play();
		FadeScreen(0.0f);
		p3d::context->SwapBuffers();
		p3d::display->SetForceVSync(true);
#ifdef FINAL
		m_UserInputHandler->SetEnabled(GetSkippable());
#endif
        // register GUI user input handler for all controllers
        for( unsigned i = 0; i < GetInputManager()->GetMaxControllers(); i++ )
        {
            GetInputManager()->RegisterMappable( i, m_UserInputHandler );
        }
        mElapsedTime = 0.0f;
		mFadeOut = -1.0f;

		mMovieVolume = VOLUME_MULTIPLIER;
		if(GetGameFlow()->GetCurrentContext() != CONTEXT_BOOTUP)
		{
			mMovieVolume *= GetSoundManager()->GetDialogueVolume();
		}

		m_refIRadMoviePlayer->SetVolume(mMovieVolume);
        m_refIRadMoviePlayer->Play( ); 
    }
}

/*=============================================================================
Causes the movie to begin fading out. When it's finished fading out it stops.
Fade out time is hard coded right now to half a second.
=============================================================================*/
void FMVPlayer::Abort(void)
{
	if(mFadeOut == -1.0f)
	{
		mFadeOut = 1.0f;
	}
}

//=============================================================================
// FMVPlayer::Stop
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FMVPlayer::Stop()
{ 
	// Force a clear screen.
	FadeScreen(0.0f);
    p3d::context->SwapBuffers();
	p3d::display->SetForceVSync(false);
    if( this->IsPlaying() && m_refIRadMoviePlayer != NULL )
    {
        for( unsigned i = 0; i < GetInputManager()->GetMaxControllers(); i++ )
        {
            GetInputManager()->UnregisterMappable( i, m_UserInputHandler );
        }
		m_UserInputHandler->SetEnabled(true);
        GetSoundManager()->ResumeAfterMovie();
        AnimationPlayer::Stop();

        ClearData();
    }
	mFadeOut = -1.0f;
}

//=============================================================================
// FMVPlayer::ForceStop
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
#ifdef RAD_WIN32
void FMVPlayer::ForceStop()
{ 
    // Force a clear screen.
    if( this->IsPlaying() && m_refIRadMoviePlayer != NULL )
    {
        for( unsigned i = 0; i < GetInputManager()->GetMaxControllers(); i++ )
        {
            GetInputManager()->UnregisterMappable( i, m_UserInputHandler );
        }
        m_UserInputHandler->SetEnabled(true);
        GetSoundManager()->ResumeAfterMovie();
        AnimationPlayer::Stop();

        ClearData();
    }
}
#endif

//=============================================================================
// FMVPlayer::Pause
//=============================================================================
void FMVPlayer::Pause()
{ 
    if( this->IsPlaying() && m_refIRadMoviePlayer != NULL )
    {
        m_refIRadMoviePlayer->Pause( );
    }
}

//=============================================================================
// FMVPlayer::UnPause
//=============================================================================
void FMVPlayer::UnPause()
{ 
    if( this->IsPlaying() && m_refIRadMoviePlayer != NULL )
    {
        m_refIRadMoviePlayer->Play( );
    }
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// FMVPlayer::Initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FMVPlayer::Initialize( radMemoryAllocator Allocator )
{
    //
    // Initialize the movie player.  
    //
    // This is where all the memory will be allocated.
    //
    ref< IRadMovieRenderLoop > refIRadMovieRenderLoop = this;
    ref< IRadMovieRenderStrategy > refIRadMovieRenderStrategy = 
        ::radMovieSimpleFullScreenRenderStrategyCreate( Allocator );

    // Note that there is a problem if we start creating multiple movie players
    //(why would we ever do this???). The allocator is stored as a single global
    //value in the movieplayer (binkmovieplayer.cpp for instance) and it is set
    //when you call ::radMoviePlayerCreate2(). So if you make multiple calls
    //the most recent allocator you pass is the one used.
    m_refIRadMoviePlayer = ::radMoviePlayerCreate2( Allocator );
    rAssert( m_refIRadMoviePlayer != NULL );

#if defined(RAD_XBOX) || defined(RAD_GAMECUBE) || defined(RAD_WIN32)
    m_refIRadMoviePlayer->Initialize(
        refIRadMovieRenderLoop,
        refIRadMovieRenderStrategy );
#else // PS2
        m_refIRadMoviePlayer->Initialize( 
        refIRadMovieRenderLoop,
        refIRadMovieRenderStrategy,
        MOVIE_MAX_WIDTH, MOVIE_MAX_HEIGHT, 
        MOVIE_ENCODED_VIDEO_BUFFER_SIZE, 
        MOVIE_PRIMARY_AUDIO_BUFFER_SIZE, 
        MOVIE_SECONDARY_AUDIO_BUFFER_SIZE, 
        MOVIE_AUDIO_BUFFER_SIZE_TYPE );
#endif
}

//=============================================================================
// FMVPlayer::DoRender
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FMVPlayer::DoRender()
{
    IRadMoviePlayer2::State state = m_refIRadMoviePlayer->GetState( );

    mFrameReady = false;

    while( !mFrameReady && (state != IRadMoviePlayer2::NoData))
    {
		::radMovieService2( );
        ::radSoundHalSystemGet( )->Service( );
//        ::radSoundHalSystemGet( )->ServiceOncePerFrame( );      
        ::radFileService( );

        state = m_refIRadMoviePlayer->GetState( );
        if ( state == IRadMoviePlayer2::ReadyToPlay )
        {
            // [ps] Here we are paused, so fall out to the regular game.
            break;
        }
    }

	if((mFadeOut <= 0.0f) && (mFadeOut != -1.0f))
	{
		Stop();
	}
    if( (state == IRadMoviePlayer2::NoData) || ( m_refIRadMoviePlayer == NULL) )
    {
        Stop();
    }
}

void FMVPlayer::IterateLoop( IRadMoviePlayer2* pIRadMoviePlayer )
{
    rAssert( pIRadMoviePlayer != NULL );

    pIRadMoviePlayer->Render();
	if(mFadeOut > 0.0f )
	{
		pIRadMoviePlayer->SetVolume(mMovieVolume * mFadeOut);
		FadeScreen(mFadeOut);
	}
	float deltaTime = mElapsedTime;
    mElapsedTime = pIRadMoviePlayer->GetCurrentFrameNumber() / pIRadMoviePlayer->GetFrameRate();
	if(mFadeOut > 0.0f )
	{
		deltaTime = mElapsedTime - deltaTime;
		mFadeOut -= deltaTime * 2.0f; // Half second fade.
	}
    mFrameReady = true;
}

//=============================================================================
// FMVPlayer::Finalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FMVPlayer::ClearData()
{
    //
    // Free up the radmovie movie player stuff
    //
    if( m_refIRadMoviePlayer != NULL )
    {
        // I think there is a case where the movie player doesn't shut down properly because the
        //decode buffer is still in use as a texture in the draw pipeline, preventing it from being
        //freed.
        p3d::pddi->DrawSync();
        m_refIRadMoviePlayer->Unload( );
        m_refIRadMoviePlayer = NULL;

		mDriveFinished = false;
        Game::GetInstance()->GetPlatform()->GetHostDrive()->AddCompletionCallback( this, 0 );

        while( !mDriveFinished )
        {
    		::radMovieService2( );
            ::radFileService( );
            SoundManager::GetInstance()->Update();
            SoundManager::GetInstance()->UpdateOncePerFrame( 0, NUM_CONTEXTS, false );
        }
    }

    AnimationPlayer::ClearData();
}

void FMVPlayer::OnDriveOperationsComplete( void* pUserData )
{
    mDriveFinished = true;
}

void FMVPlayer::FadeScreen(float Alpha)
{
    tColour c;
	c.Set( 0, 0, 0, int(0xFF * (1.0f - rmt::Clamp(Alpha, 0.0f, 1.0f))) );
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
    p3d::pddi->SetProjectionMode( PDDI_PROJECTION_ORTHOGRAPHIC );
    pddiColour oldAmbient = p3d::pddi->GetAmbientLight();
    p3d::pddi->SetAmbientLight( pddiColour( 255, 255, 255 ) );

    pddiPrimStream* overlay = 0;

    pddiShader* overlayShader = BootupContext::GetInstance()->GetSharedShader();
    rAssert( overlayShader );

    overlayShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ALPHA );
    overlayShader->SetInt( PDDI_SP_ISLIT, 0 );
    overlayShader->SetInt( PDDI_SP_SHADEMODE, PDDI_SHADE_FLAT );

    overlay = p3d::pddi->BeginPrims( overlayShader, PDDI_PRIM_TRISTRIP, PDDI_V_C, 4 );

    overlay->Colour( c );
    overlay->Coord( 0.5f, -0.5f, 1.0f );
    overlay->Colour( c );
    overlay->Coord(  -0.5f, -0.5f, 1.0f );
    overlay->Colour( c );
    overlay->Coord( 0.5f,  0.5f, 1.0f );
    overlay->Colour( c );
    overlay->Coord( -0.5f,  0.5f, 1.0f );
    
    p3d::pddi->EndPrims( overlay );
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
