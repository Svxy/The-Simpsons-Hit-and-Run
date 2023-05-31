//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenHudMap
//
// Description: Implementation of the CGuiScreenHudMap class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/04      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/ingame/guiscreenhudmap.h>
#include <presentation/gui/utility/hudmap.h>

#include <memory/srrmemory.h>

#include <p3d/pointcamera.hpp>

#include <raddebug.hpp>     // Foundation
#include <raddebugwatch.hpp>
#include <page.h>
#include <polygon.h>
#include <pure3dobject.h>
#include <screen.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

#ifdef DEBUGWATCH
    static const char* HUD_MAP_WATCHER_NAMESPACE = "GUI System - HUD Map Screen";
    float g_wCamPosY = 700.0f;
    float g_wCamTargetX = 0.0f;
    float g_wCamTargetZ = 0.0f;
#endif

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenHudMap::CGuiScreenHudMap
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
CGuiScreenHudMap::CGuiScreenHudMap
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:
	CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_HUD_MAP ),
    m_largeHudMap( NULL ),
    m_posX( 0 ),
    m_posY( 0 ),
    m_width( 0 ),
    m_height( 0 ),
    m_camera( NULL )
{
MEMTRACK_PUSH_GROUP( "CGUIScreenHudMap" );
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage;
	pPage = m_pScroobyScreen->GetPage( "ViewMap" );
	rAssert( pPage );

    Scrooby::Polygon* mapBgd = pPage->GetPolygon( "MapBgd" );
    rAssert( mapBgd );

    // Get map position and size (based on background polygon)
    //
    mapBgd->GetVertexLocation( 0, m_posX, m_posY ); // assuming first vertex
                                                    // is bottom-left co-ord
    mapBgd->GetBoundingBoxSize( m_width, m_height );

    // Create an overhead camera
    //
    m_camera = new(GMA_LEVEL_HUD) tPointCamera;
    rAssert( m_camera );
    m_camera->AddRef();
    m_camera->SetVUp( rmt::Vector( 1, 0, 0 ) );
    m_camera->SetPosition( rmt::Vector( 0, 700, 0 ) );
    m_camera->SetTarget( rmt::Vector( 0, 0, 0 ) );

#ifdef DEBUGWATCH
    radDbgWatchAddFloat( &g_wCamPosY,
                         "Camera Height (Y)",
                         HUD_MAP_WATCHER_NAMESPACE,
                         NULL,
                         NULL,
                         100.0f,
                         1000.0f );

    radDbgWatchAddFloat( &g_wCamTargetX,
                         "Camera Target (X)",
                         HUD_MAP_WATCHER_NAMESPACE,
                         NULL,
                         NULL,
                         -1000.0f,
                         1000.0f );

    radDbgWatchAddFloat( &g_wCamTargetZ,
                         "Camera Target (Z)",
                         HUD_MAP_WATCHER_NAMESPACE,
                         NULL,
                         NULL,
                         -1000.0f,
                         1000.0f );
#endif
MEMTRACK_POP_GROUP();
}


//===========================================================================
// CGuiScreenHudMap::~CGuiScreenHudMap
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
CGuiScreenHudMap::~CGuiScreenHudMap()
{
#ifdef DEBUGWATCH
    radDbgWatchDelete( &g_wCamPosY );
    radDbgWatchDelete( &g_wCamTargetX );
    radDbgWatchDelete( &g_wCamTargetZ );
#endif

    if( m_camera != NULL )
    {
        m_camera->Release();
        m_camera = NULL;
    }
}


//===========================================================================
// CGuiScreenHudMap::HandleMessage
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
void CGuiScreenHudMap::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
#ifdef DEBUGWATCH
                rAssert( m_camera );
                m_camera->SetPosition( rmt::Vector( g_wCamTargetX, g_wCamPosY, g_wCamTargetZ ) );
                m_camera->SetTarget( rmt::Vector( g_wCamTargetX, 0, g_wCamTargetZ ) );
#endif
                rAssert( m_largeHudMap );
                m_largeHudMap->Update( param1 );

                break;
            }
            case GUI_MSG_CONTROLLER_START:
            {
                // resume game
                m_pParent->HandleMessage( GUI_MSG_UNPAUSE_INGAME );

                break;
            }

            default:
            {
                break;
            }
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenHudMap::InitIntro
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
void CGuiScreenHudMap::InitIntro()
{
    m_largeHudMap = GetCurrentHud()->GetHudMap( 0 );
    rAssert( m_largeHudMap );

    Scrooby::Pure3dObject* p3dMap = m_largeHudMap->GetPure3dObject();
    rAssert( p3dMap );

    p3dMap->SetPosition( m_posX, m_posY );
    p3dMap->SetBoundingBoxSize( m_width, m_height );

    p3dMap->SetCamera( m_camera );

    m_largeHudMap->Update( 0 );
}


//===========================================================================
// CGuiScreenHudMap::InitRunning
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
void CGuiScreenHudMap::InitRunning()
{
}


//===========================================================================
// CGuiScreenHudMap::InitOutro
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
void CGuiScreenHudMap::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

