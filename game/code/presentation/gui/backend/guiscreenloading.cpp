//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLoading
//
// Description: Implementation of the CGuiScreenLoading class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/23      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <camera/animatedcam.h>
#include <camera/supercammanager.h>
#include <presentation/gui/backend/guiscreenloading.h>
#include <presentation/gui/backend/guiloadingbar.h>
#include <presentation/gui/utility/specialfx.h>
#include <presentation/gui/guitextbible.h>

#include <gameflow/gameflow.h>
#include <memory/createheap.h>
#include <memory/memoryutilities.h>
#include <mission/gameplaymanager.h>
#include <mission/missionmanager.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/renderlayer.h>
#include <sound/soundmanager.h>

#include <raddebug.hpp> // Foundation
#include <p3d/sprite.hpp>
#include <p3d/utility.hpp>
#include <screen.h>
#include <page.h>
#include <layer.h>
#include <group.h>
#include <sprite.h>
#include <polygon.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

#define ENABLE_DYNA_LOADED_IMAGES
//#define LOADING_BAR_EXPLOSION

#ifdef ENABLE_DYNA_LOADED_IMAGES
    const char* DYNAMIC_RESOURCES_DIR = "art\\frontend\\dynaload\\images\\loading\\";
    const char* DYNA_LOAD_INVENTORY_SECTION = "LoadingScreenImages";
#endif

// this is to correct the original reduced scale in the source image
//
#ifdef RAD_WIN32
    const float LOADING_BGD0_CORRECTION_SCALE = 1.05f;
#else
    const float LOADING_BGD0_CORRECTION_SCALE = 4.2f;
#endif

#ifdef RAD_WIN32
    const float LOADING_BGD1_CORRECTION_SCALE = 1.75f;
#else
    const float LOADING_BGD1_CORRECTION_SCALE = 8.4f;
#endif

#ifdef RAD_PS2
    const float LOADING_IMAGE_CORRECTION_SCALE = 1.95f;
#elif defined( RAD_WIN32 )
    const float LOADING_IMAGE_CORRECTION_SCALE = 0.925f;
#else
    const float LOADING_IMAGE_CORRECTION_SCALE = 1.85f;
#endif

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenLoading::CGuiScreenLoading
//===========================================================================
// Description: Constructor.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
CGuiScreenLoading::CGuiScreenLoading
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_LOADING ),
    m_elapsedTime( 0 ),
    m_elapsedSpiralTime( 0 ),
    m_loadingBarGroup( NULL ),
    m_currentMemoryUsage( 0.0f ),
    m_startingMemoryAvailable( 0 ),
    m_elapsedFireTime( 0 ),
    m_loadingImage( NULL ),
    m_isSpirallingDone( false ),
    m_loadingImageSprite( NULL ),
    m_explosionLayer( NULL ),
    m_explosion( NULL ),
    m_elapsedExplosionTime( 0 )
{
    memset( m_loadingImageName, 0, sizeof( m_loadingImageName ) );

    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage;
	pPage = m_pScroobyScreen->GetPage( "Loading" );
	rAssert( pPage );

    Scrooby::Layer* newspaperLayer = pPage->GetLayer( "Newspaper" );
    rAssert( newspaperLayer != NULL );
    m_loadingImage = newspaperLayer->GetSprite( "Loading" );
    m_loadingImage->SetVisible( false );

    if( this->IsWideScreenDisplay() )
    {
        newspaperLayer->ResetTransformation();
        this->ApplyWideScreenCorrectionScale( newspaperLayer );
    }

    Scrooby::Layer* foregroundLayer = pPage->GetLayer( "Foreground" );

    for( int i = 0; i < NUM_LOADING_OVERLAYS; i++ )
    {
        char overlayName[ 32 ];
        sprintf( overlayName, "Overlay%d", i );
        m_loadingOverlays[ i ] = foregroundLayer->GetSprite( overlayName );
        rAssert( m_loadingOverlays[ i ] != NULL );
    }

    // get loading bar
    //
    m_loadingBarGroup = pPage->GetGroup( "LoadingBar" );
    rAssert( m_loadingBarGroup != NULL );
    m_loadingBar.m_type = Slider::HORIZONTAL_SLIDER_RIGHT;
    m_loadingBar.SetScroobyPolygon( m_loadingBarGroup->GetPolygon( "Wick" ),
                                    m_loadingBarGroup->GetSprite( "Fire" ) );

    Scrooby::Sprite* loadingBgd = pPage->GetSprite( "Background" );
    if( loadingBgd != NULL )
    {
        loadingBgd->ScaleAboutCenter( LOADING_BGD0_CORRECTION_SCALE );
    }

/*
    // XBOX ONLY: show loading text to satisfy Xbox TCR requirement (C01-07)
    //
#ifndef RAD_XBOX
    // otherwise, hide it for all other platforms
    //
    Scrooby::Text* loadingText = pPage->GetText( "Loading" );
    if( loadingText != NULL )
    {
        loadingText->SetVisible( false );
    }
#endif // !RAD_XBOX
*/

    // get explosion overlay (from Explosion.pag)
    //
    pPage = m_pScroobyScreen->GetPage( "Explosion" );
    if( pPage != NULL )
    {
        m_explosionLayer = pPage->GetLayer( "Explosion" );
        rAssert( m_explosionLayer != NULL );

        m_explosion = m_explosionLayer->GetPolygon( "Explosion0" );
        rAssert( m_explosion != NULL );
    }

#ifdef ENABLE_DYNA_LOADED_IMAGES
    // add inventory section for dynamically loaded resources
    //
    p3d::inventory->AddSection( DYNA_LOAD_INVENTORY_SECTION );
#endif
}


//===========================================================================
// CGuiScreenLoading::~CGuiScreenLoading
//===========================================================================
// Description: Destructor.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
CGuiScreenLoading::~CGuiScreenLoading()
{
#ifdef ENABLE_DYNA_LOADED_IMAGES
    // delete inventory section for dynamically loaded resources
    //
    p3d::inventory->DeleteSection( DYNA_LOAD_INVENTORY_SECTION );
#endif
}


//===========================================================================
// CGuiScreenLoading::HandleMessage
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLoading::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    switch( message )
    {
        case GUI_MSG_UPDATE:
        {
            // update elapsed time
            m_elapsedTime += param1;

            if( !m_isSpirallingDone )
            {
                // update loading image
                //
                static float SPIRAL_DURATION_TIME = 1800.0f; // in msec
                static float SPIRAL_ROTATION_TIME = 60.0f; // in msec

                m_elapsedSpiralTime += param1;
                m_isSpirallingDone = GuiSFX::Spiral( m_loadingImage,
                                                     (float)m_elapsedSpiralTime,
                                                     SPIRAL_DURATION_TIME,
                                                     SPIRAL_ROTATION_TIME,
                                                     0.25f, // LOADING_IMAGE_CORRECTION_SCALE * 2.0f,
                                                     LOADING_IMAGE_CORRECTION_SCALE );
            }

            static float ROTATION_PERIOD = 20000; // in msec
            float currentAngle = ((float)m_elapsedTime / ROTATION_PERIOD) * 360.0f;

            for( int i = 0; i < NUM_LOADING_OVERLAYS; i++ )
            {
                rAssert( m_loadingOverlays[ i ] );
                m_loadingOverlays[ i ]->ResetTransformation();
                m_loadingOverlays[ i ]->ScaleAboutCenter( LOADING_BGD1_CORRECTION_SCALE );
                m_loadingOverlays[ i ]->RotateAboutCenter( currentAngle );

                currentAngle *= -1.0f; // reverse rotation direction
            }

            if( GetGameFlow()->GetCurrentContext() == CONTEXT_LOADING_GAMEPLAY )
            {
                // update loading bar
                //
                rAssert( m_loadingBarGroup != NULL );
                m_loadingBarGroup->SetVisible( true );

#if defined( RAD_XBOX )
                float newMemoryUsage = (m_startingMemoryAvailable - Memory::GetTotalMemoryFree()) / TOTAL_INGAME_MEMORY_USAGE;
#elif defined( RAD_WIN32 )
                // this sucks but i just want to finish it.
                float memUsage = float( GetLoadingManager()->GetNumRequestsProcessed() ) / TOTAL_INGAME_FILES;
                float newMemoryUsage = m_currentMemoryUsage;
                if( memUsage - newMemoryUsage < LOAD_MIN_SPEED )
                {
                    newMemoryUsage = memUsage;
                }
                else
                {
                    float delta = (memUsage - newMemoryUsage) / LOAD_BLEND_FACTOR;
                    newMemoryUsage += delta > LOAD_MIN_SPEED ? delta : LOAD_MIN_SPEED;
                }
#else
                float newMemoryUsage = (m_startingMemoryAvailable - GetTotalMemoryFreeInAllHeaps()) / TOTAL_INGAME_MEMORY_USAGE;
#endif
                // don't allow loading bar to go backwards
                //
                if( newMemoryUsage > m_currentMemoryUsage )
                {
                    m_currentMemoryUsage = newMemoryUsage;

                    if( m_currentMemoryUsage < 1.0f )
                    {
                        m_loadingBar.SetValue( 1.0f - m_currentMemoryUsage );
                    }
                    else
                    {
#ifdef LOADING_BAR_EXPLOSION
                        if( m_explosionLayer != NULL && m_explosion != NULL )
                        {
                            m_explosionLayer->SetVisible( true );

                            m_elapsedExplosionTime += param1;
/*
                            float explosionScale = EXPLOSION_SCALE_START + m_elapsedExplosionTime / 1000.0f * EXPLOSION_SCALE_RATE;
                            m_explosion->ResetTransformation();
                            m_explosion->ScaleAboutCenter( explosionScale );
*/
                            const unsigned int EXPLOSION_FLICKER_PERIOD = 50;
                            bool isBlinked = GuiSFX::Blink( m_explosion,
                                                            (float)m_elapsedExplosionTime,
                                                            (float)EXPLOSION_FLICKER_PERIOD );

                            if( isBlinked )
                            {
                                m_elapsedExplosionTime %= EXPLOSION_FLICKER_PERIOD;
                            }
/*
                            tColour currentColour;
                            GuiSFX::ModulateColour( &currentColour,
                                                    (float)m_elapsedExplosionTime,
                                                    100.0f,
                                                    tColour( 255, 255, 0 ),
                                                    tColour( 255, 0, 0 ) );

                            for( int i = 0; i < m_explosion->GetNumOfVertexes(); i++ )
                            {
                                m_explosion->SetVertexColour( i, currentColour );
                            }
*/
                        }
#else
    #ifndef RAD_DEBUG
                        rReleaseWarningMsg( false, "Current memory usage for loading bar exceeds 100%!" );
    #endif
#endif // LOADING_BAR_EXPLOSION
                    }
                }

                // flicker loading bar flame
                //
                const unsigned int LOADING_BAR_FIRE_TIME = 50; // in msec
                m_elapsedFireTime += param1;
                if( m_elapsedFireTime > LOADING_BAR_FIRE_TIME )
                {
                    rAssert( m_loadingBar.m_pImage != NULL );
                    m_loadingBar.m_pImage->SetIndex( 1 - m_loadingBar.m_pImage->GetIndex() );

                    m_elapsedFireTime %= LOADING_BAR_FIRE_TIME;
                }
            }

            break;
        }
        case GUI_MSG_LOAD_RESOURCES:
        {
            this->LoadResources();

#ifdef RAD_XBOX
            m_startingMemoryAvailable = Memory::GetTotalMemoryFree();
#else
            m_startingMemoryAvailable = GetTotalMemoryFreeInAllHeaps();
#endif
            rReleasePrintf( "Starting Memory Available = %.2f MB\n", (float)m_startingMemoryAvailable / MB );

#ifdef RAD_WIN32
            GetLoadingManager()->ResetRequestsProcessed();
#endif
            break;
        }
        default:
        {
            break;
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}

void
CGuiScreenLoading::LoadResources()
{
    int currentLevel = GetGameplayManager()->GetCurrentLevelIndex();

#ifdef ENABLE_DYNA_LOADED_IMAGES
    char languageDir[ 16 ];
    languageDir[ 0 ] = '\0';

#ifdef PAL
    switch( CGuiTextBible::GetCurrentLanguage() )
    {
        case Scrooby::XL_FRENCH:
        {
            strcpy( languageDir, "french\\" );

            break;
        }
        case Scrooby::XL_GERMAN:
        {
            strcpy( languageDir, "german\\" );

            break;
        }
        case Scrooby::XL_SPANISH:
        {
            strcpy( languageDir, "spanish\\" );

            break;
        }
        default:
        {
            rAssert( CGuiTextBible::GetCurrentLanguage() == Scrooby::XL_ENGLISH );

            break;
        }
    }
#endif // PAL

    sprintf( m_loadingImageName, "loading%d.png", currentLevel + 1 );

    char loadingImageFile[ 256 ];
    sprintf( loadingImageFile, "%s%sloading%d.p3d",
             DYNAMIC_RESOURCES_DIR,
             languageDir,
             currentLevel + 1 );

    // add request to loading manager to load image file
    //
    GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D,
                                     loadingImageFile,
                                     GMA_LEVEL_OTHER,
                                     DYNA_LOAD_INVENTORY_SECTION,
                                     DYNA_LOAD_INVENTORY_SECTION,
                                     this );
#else
    // set the current level loading image
    //
    rAssert( m_loadingImage );
    m_loadingImage->SetIndex( currentLevel );
#endif

    // reset elapsed spiralling time
    //
    m_elapsedSpiralTime = 0;
}

void
CGuiScreenLoading::OnProcessRequestsComplete( void* pUserData )
{
#ifdef ENABLE_DYNA_LOADED_IMAGES
    // push and select inventory section for searching
    //
    p3d::inventory->PushSection();
    p3d::inventory->SelectSection( DYNA_LOAD_INVENTORY_SECTION );
    bool currentSectionOnly = p3d::inventory->GetCurrentSectionOnly();
    p3d::inventory->SetCurrentSectionOnly( true );

    // search for the loading image sprite
    //
    rAssert( m_loadingImageSprite == NULL );
    m_loadingImageSprite = p3d::find<tSprite>( m_loadingImageName );
    rAssert( m_loadingImageSprite );
    m_loadingImageSprite->AddRef();

    // pop inventory section and restore states
    //
    p3d::inventory->SetCurrentSectionOnly( currentSectionOnly );
    p3d::inventory->PopSection();

    // set the raw sprite to the Scrooby loading image
    //
    rAssert( m_loadingImage );
    m_loadingImage->SetRawSprite( m_loadingImageSprite, true );
    m_loadingImage->SetVisible( true );
    m_loadingImage->ScaleAboutCenter( 0.0f );

    m_isSpirallingDone = false;
#endif
}


//===========================================================================
// CGuiScreenLoading::InitIntro
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLoading::InitIntro()
{
    RenderLayer* levelLayer = GetRenderManager()->mpLayer( RenderEnums::LevelSlot );
    if( levelLayer != NULL )
    {
        levelLayer->Freeze();
    }

    // reset loading overlay scale
    //
    for( int i = 0; i < NUM_LOADING_OVERLAYS; i++ )
    {
        rAssert( m_loadingOverlays[ i ] );
        m_loadingOverlays[ i ]->ResetTransformation();
        m_loadingOverlays[ i ]->ScaleAboutCenter( LOADING_BGD1_CORRECTION_SCALE );
    }

    m_loadingBar.SetValue( 1.0f );

    // reset current memory usage
    //
    m_currentMemoryUsage = 0.0f;

    // hide loading bar by default
    //
    rAssert( m_loadingBarGroup != NULL );
    m_loadingBarGroup->SetVisible( false );

    // hide explosion overlay
    //
    if( m_explosionLayer != NULL && m_explosion != NULL )
    {
        m_explosionLayer->SetVisible( false );
        m_explosion->SetVisible( true );

        m_elapsedExplosionTime = 0;
    }

    //
    // Sound ducking, but only for intra-mission stuff.  If we're coming
    // from the FE, we want to play the newspaper spin music.
    //
    if( GetGameFlow()->GetCurrentContext() != CONTEXT_FRONTEND )
    {
        GetSoundManager()->OnPauseStart();
    }
}


//===========================================================================
// CGuiScreenLoading::InitRunning
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLoading::InitRunning()
{
}


//===========================================================================
// CGuiScreenLoading::InitOutro
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLoading::InitOutro()
{
    RenderLayer* levelLayer = GetRenderManager()->mpLayer( RenderEnums::LevelSlot );
    if( levelLayer != NULL && levelLayer->IsFrozen() )
    {
        levelLayer->Thaw();
    }

#ifdef ENABLE_DYNA_LOADED_IMAGES
    p3d::pddi->DrawSync();

    if( m_loadingImageSprite != NULL )
    {
        rAssert( m_loadingImage );
        m_loadingImage->SetRawSprite( NULL, true );
        m_loadingImage->SetVisible( false );

        // remove and release the loading image sprite
        //
        p3d::inventory->RemoveSectionElements( DYNA_LOAD_INVENTORY_SECTION );
        m_loadingImageSprite->ReleaseVerified();
        m_loadingImageSprite = NULL;
    }
#endif

    rReleasePrintf( "Final memory usage value for loading bar = %.3f\n", m_currentMemoryUsage );
    m_loadingBar.SetValue( 0.0f );

    m_isSpirallingDone = true;

    // reset elapsed time for any loading animations
    //
    m_elapsedTime = 0;
    m_elapsedFireTime = 0;

    GetSoundManager()->ResetDucking();
    AnimatedCam::CheckPendingCameraSwitch();
    GetSuperCamManager()->GetSCC( 0 )->NoTransition();
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

