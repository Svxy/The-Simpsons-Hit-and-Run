//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLoadingFE
//
// Description: Implementation of the CGuiScreenLoadingFE class.
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
#include <presentation/gui/backend/guiscreenloadingfe.h>
#include <presentation/gui/backend/guiloadingbar.h>
#include <presentation/gui/utility/specialfx.h>

#include <gameflow/gameflow.h>
#include <memory/createheap.h>
#include <memory/memoryutilities.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/renderlayer.h>
#include <sound/soundmanager.h>

#include <p3d/drawable.hpp>
#include <p3d/camera.hpp>
#include <p3d/anim/multicontroller.hpp>

#include <raddebug.hpp> // Foundation
#include <screen.h>
#include <layer.h>
#include <page.h>
#include <group.h>
#include <text.h>
#include <sprite.h>
#include <polygon.h>
#include <pure3dobject.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

#define ENABLE_DYNA_LOADED_RESOURCES
//#define LOADING_BAR_FE_EXPLOSION

#ifdef ENABLE_DYNA_LOADED_RESOURCES
    const char* LOADING_FE_PURE3D_FILE = "art\\frontend\\scrooby\\resource\\pure3d\\loading.p3d";
    const char* LOADING_FE_INVENTORY = "LoadingFEScreen";

    const char* LOADING_FE_DRAWABLE = "loading_screen";
    const char* LOADING_FE_CAMERA = "loading_screen_cameraShape";
    const char* LOADING_FE_MULTICONTROLLER = "loading_screen_MasterController";
#endif

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenLoadingFE::CGuiScreenLoadingFE
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
CGuiScreenLoadingFE::CGuiScreenLoadingFE
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_LOADING_FE ),
    m_loadingText( NULL ),
    m_itchyAndScratchy( NULL ),
    m_elapsedTime( 0 ),
    m_loadingBarGroup( NULL ),
    m_currentMemoryUsage( 0.0f ),
    m_startingMemoryAvailable( 0 ),
    m_elapsedFireTime( 0 ),
    m_explosionLayer( NULL ),
    m_explosion( NULL ),
    m_elapsedExplosionTime( 0 )
{
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "LoadingFE" );
    rAssert( pPage != NULL );

    Scrooby::Layer* foreground = pPage->GetLayer( "Foreground" );
    Scrooby::Layer* background = pPage->GetLayer( "Background" );

    m_loadingText = foreground->GetText( "Loading" );
    rAssert( m_loadingText != NULL );

    m_itchyAndScratchy = background->GetPure3dObject( "Loading" );
    if( m_itchyAndScratchy != NULL )
    {
        m_itchyAndScratchy->SetDrawable( NULL );
        m_itchyAndScratchy->SetCamera( NULL );
        m_itchyAndScratchy->SetMultiController( NULL );

        m_itchyAndScratchy->SetClearDepthBuffer( true );
    }

    // get loading bar
    //
    m_loadingBarGroup = pPage->GetGroup( "LoadingBar" );
    rAssert( m_loadingBarGroup != NULL );
    m_loadingBar.m_type = Slider::HORIZONTAL_SLIDER_RIGHT;
    m_loadingBar.SetScroobyPolygon( m_loadingBarGroup->GetPolygon( "Wick" ),
                                    m_loadingBarGroup->GetSprite( "Fire" ) );

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

#ifdef ENABLE_DYNA_LOADED_RESOURCES
    p3d::inventory->AddSection( LOADING_FE_INVENTORY );
#endif
}


//===========================================================================
// CGuiScreenLoadingFE::~CGuiScreenLoadingFE
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
CGuiScreenLoadingFE::~CGuiScreenLoadingFE()
{
#ifdef ENABLE_DYNA_LOADED_RESOURCES
    p3d::inventory->DeleteSection( LOADING_FE_INVENTORY );
#endif
}


//===========================================================================
// CGuiScreenLoadingFE::HandleMessage
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
void CGuiScreenLoadingFE::HandleMessage
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
            // update loading text
            //
            const unsigned int LOADING_TEXT_LOOP_TIME = 500; // in msec
            m_elapsedTime += param1;
            if( m_elapsedTime > LOADING_TEXT_LOOP_TIME )
            {
                rAssert( m_loadingText != NULL );
                int nextIndex = (m_loadingText->GetIndex() + 1) % m_loadingText->GetNumOfStrings();
                m_loadingText->SetIndex( nextIndex );

                m_elapsedTime %= LOADING_TEXT_LOOP_TIME;
            }

            ContextEnum currentContext = GetGameFlow()->GetCurrentContext();
            if( currentContext == CONTEXT_FRONTEND ||
                currentContext == CONTEXT_LOADING_DEMO ||
                currentContext == CONTEXT_LOADING_SUPERSPRINT ||
                currentContext == CONTEXT_LOADING_GAMEPLAY )
            {
                // update loading bar
                //
                rAssert( m_loadingBarGroup != NULL );
                m_loadingBarGroup->SetVisible( true );

                float newMemoryUsage = this->GetCurrentMemoryUsage( currentContext );

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
#ifdef LOADING_BAR_FE_EXPLOSION
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
#endif // LOADING_BAR_FE_EXPLOSION
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
CGuiScreenLoadingFE::LoadResources()
{
#ifdef ENABLE_DYNA_LOADED_RESOURCES
    if( m_itchyAndScratchy != NULL )
    {
        m_itchyAndScratchy->SetDrawable( NULL );
    }

    GameMemoryAllocator heapAllocator = GMA_DEFAULT;

    ContextEnum nextContext = GetGameFlow()->GetNextContext();
    switch( nextContext )
    {
        case CONTEXT_FRONTEND:
        {
            heapAllocator = GMA_LEVEL_MOVIE;

            break;
        }
        case CONTEXT_LOADING_GAMEPLAY:
        case CONTEXT_LOADING_DEMO:
        {
            heapAllocator = GMA_LEVEL_OTHER;

            break;
        }
        case CONTEXT_LOADING_SUPERSPRINT:
        case CONTEXT_SUPERSPRINT_FE:
        {
            heapAllocator = GMA_LEVEL_HUD;

            break;
        }
        default:
        {
            break;
        }
    }

    rAssert( heapAllocator != GMA_DEFAULT );

    GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D,
                                     LOADING_FE_PURE3D_FILE,
                                     heapAllocator,
                                     LOADING_FE_INVENTORY,
                                     LOADING_FE_INVENTORY,
                                     this );
#endif
}

void
CGuiScreenLoadingFE::OnProcessRequestsComplete( void* pUserData )
{
    if( m_itchyAndScratchy != NULL )
    {
        // push and select inventory section for searching
        //
        p3d::inventory->PushSection();
        p3d::inventory->SelectSection( LOADING_FE_INVENTORY );
        bool currentSectionOnly = p3d::inventory->GetCurrentSectionOnly();
        p3d::inventory->SetCurrentSectionOnly( true );

        // search for drawable, camera, and multicontroller, and assign
        // them to the itchy & scratchy pure3d object
        //
        tDrawable* drawable = p3d::find<tDrawable>( LOADING_FE_DRAWABLE );
        m_itchyAndScratchy->SetDrawable( drawable );

        tCamera* camera = p3d::find<tCamera>( LOADING_FE_CAMERA );
        m_itchyAndScratchy->SetCamera( camera );

        tMultiController* multiController = p3d::find<tMultiController>( LOADING_FE_MULTICONTROLLER );
        m_itchyAndScratchy->SetMultiController( multiController );

        // pop inventory section and restore states
        //
        p3d::inventory->SetCurrentSectionOnly( currentSectionOnly );
        p3d::inventory->PopSection();
    }
}


//===========================================================================
// CGuiScreenLoadingFE::InitIntro
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
void CGuiScreenLoadingFE::InitIntro()
{
    RenderLayer* levelLayer = GetRenderManager()->mpLayer( RenderEnums::LevelSlot );
    if( levelLayer != NULL )
    {
        levelLayer->Freeze();
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
}

//===========================================================================
// CGuiScreenLoadingFE::InitRunning
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
void CGuiScreenLoadingFE::InitRunning()
{
}


//===========================================================================
// CGuiScreenLoadingFE::InitOutro
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
void CGuiScreenLoadingFE::InitOutro()
{
    RenderLayer* levelLayer = GetRenderManager()->mpLayer( RenderEnums::LevelSlot );
    if( levelLayer != NULL && levelLayer->IsFrozen() )
    {
        levelLayer->Thaw();
    }

#ifdef ENABLE_DYNA_LOADED_RESOURCES
    p3d::pddi->DrawSync();

    if( m_itchyAndScratchy != NULL )
    {
        m_itchyAndScratchy->SetDrawable( NULL );
        m_itchyAndScratchy->SetCamera( NULL );
        m_itchyAndScratchy->SetMultiController( NULL );
    }

    p3d::inventory->RemoveSectionElements( LOADING_FE_INVENTORY );
#endif

    rReleasePrintf( "Final memory usage value for loading bar = %.3f\n", m_currentMemoryUsage );
    m_loadingBar.SetValue( 0.0f );

    // reset loading text animation
    //
    m_elapsedTime = 0;
    rAssert( m_loadingText != NULL );
    m_loadingText->SetIndex( 0 );

    m_elapsedFireTime = 0;

    GetSoundManager()->ResetDucking();
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

float
CGuiScreenLoadingFE::GetCurrentMemoryUsage( ContextEnum currentContext ) const
{
    float currentMemoryUsage = 0.0f;

#ifdef RAD_XBOX
    int totalMemoryFree = Memory::GetTotalMemoryFree();
#else
    int totalMemoryFree = GetTotalMemoryFreeInAllHeaps();
#endif

    if( totalMemoryFree > 0 )
    {
        switch( currentContext )
        {
            case CONTEXT_LOADING_GAMEPLAY:
            {
                currentMemoryUsage = (m_startingMemoryAvailable - totalMemoryFree) / TOTAL_INGAME_MEMORY_USAGE;

                break;
            }
            case CONTEXT_LOADING_DEMO:
            {
                currentMemoryUsage = (m_startingMemoryAvailable - totalMemoryFree) / TOTAL_DEMO_MEMORY_USAGE;

                break;
            }
            case CONTEXT_LOADING_SUPERSPRINT:
            {
#ifndef RAD_WIN32
                currentMemoryUsage = (m_startingMemoryAvailable - totalMemoryFree) / TOTAL_SUPERSPRINT_MEMORY_USAGE;
#else
                // this sucks but i just want to finish it.
                float memUsage = float( GetLoadingManager()->GetNumRequestsProcessed() ) / TOTAL_SUPERSPRINT_FILES;
                currentMemoryUsage = m_currentMemoryUsage;
                if( memUsage - currentMemoryUsage < LOAD_MIN_SPEED )
                {
                    currentMemoryUsage = memUsage;
                }
                else
                {
                    float delta = (memUsage - currentMemoryUsage) / LOAD_BLEND_FACTOR;
                    currentMemoryUsage += delta > LOAD_MIN_SPEED ? delta : LOAD_MIN_SPEED;
                }
#endif
                break;
            }
            case CONTEXT_FRONTEND:
            {
                currentMemoryUsage = (m_startingMemoryAvailable - totalMemoryFree) / TOTAL_FE_MEMORY_USAGE;

                break;
            }
            default:
            {
                rWarningMsg( false, "Invalid context!" );

                break;
            }
        }
    }
    else
    {
        rWarningMsg( false, "Why is total memory free negative??" );
    }

    return currentMemoryUsage;
}

