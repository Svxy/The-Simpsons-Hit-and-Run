//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMissionLoad
//
// Description: Implementation of the CGuiScreenMissionSuccess class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/04/07      ian gipson  Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <camera/animatedcam.h>
#include <camera/supercammanager.h>
#include <events/eventmanager.h>
#include <gameflow/gameflow.h>
#include <memory/classsizetracker.h>
#include <mission/gameplaymanager.h>
#include <mission/objectives/coinobjective.h>
#include <mission/objectives/raceobjective.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/missionmanager.h>
#include <p3d/imagefactory.hpp>
#include <p3d/sprite.hpp>
#include <presentation/gui/ingame/guiscreenmissionbase.h>
#include <presentation/gui/ingame/guiscreenmissionselect.h>
#include <presentation/gui/utility/transitions.h>
#include <sound/soundmanager.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/worldphysicsmanager.h>
#include <worldsim/character/charactermanager.h>

#include <ai/actor/actormanager.h>

#include <group.h>
#include <layer.h>
#include <page.h>
#include <polygon.h>
#include <pure3dobject.h>
#include <screen.h>
#include <sprite.h>
#include <text.h>


//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================
#define CLIP_PAUSE_TIME 300.0f
#define CLIP_MOVE_TIME  300.0f


//Chuck
//Intialize  the special case patty and selma screen pointer
tSprite* CGuiScreenMissionBase::sp_PattyAndSelmaScreenPNG          = NULL;

char     CGuiScreenMissionBase::s_AnimatedBitmapName[ 256 ]     = "";
char     CGuiScreenMissionBase::s_AnimatedBitmapShortName[ 32 ] = "";
tSprite* CGuiScreenMissionBase::s_AnimatedBitmapSprite          = NULL;
bool     CGuiScreenMissionBase::s_BitmapLoadPending             = false;

static   tColour g_BackgroundColorLeft( 255, 255, 255, 255 );
static   tColour g_BackgroundColorRight( 255, 255, 255, 255 );

const    tColour g_BackgroundBlue (  59,  76, 129 );
const    tColour g_BackgroundRed  ( 100,   6,   6 );
const    tColour g_BackgroundGreen(   7,  73,  30 );

const    tColour  g_ColorNormalLeft ( 255, 255, 255, 255 );
const    tColour  g_ColorNormalRight( 255, 255, 255, 255 );
const    tColour  g_ColorBonusLeft  ( 255, 255, 255, 255 );
const    tColour  g_ColorBonusRight ( 255, 255, 255, 255 );
const    tColour  g_ColorRaceLeft   ( 255, 255, 255, 255 );
const    tColour  g_ColorRaceRight  ( 255, 255, 255, 255 );
const    tColour  g_ColorWagerLeft  ( 255, 255, 255, 255 );
const    tColour  g_ColorWagerRight ( 255, 255, 255, 255 );

GuiSFX::Dummy         g_IntroStart(          "IntroStart"           );
GuiSFX::Junction3     g_IntroJunction;
GuiSFX::Show          g_ForegroundShow(      "ForeGroundShow"       );
GuiSFX::Show          g_ClipLeftShow(        "ClipLeftShow"         );
GuiSFX::PauseInFrames g_IntroPause(          "Pause"                );
GuiSFX::Translator    g_ScreenSlideIn(       "ScreenSlideIn"        );
GuiSFX::Pause         g_ClipLeftPause(       "ClipLeftPause"        );
GuiSFX::Translator    g_ClipLeftSlideOut(    "ClipLeftSlideOut"     );
GuiSFX::Hide          g_ClipLeftHide(        "ClipLeftHide"         );
GuiSFX::Show          g_BackgroundShow(      "BackgroundShow"       );
GuiSFX::ColorChange   g_DarkenPolyFade(      "DarkenPolyFade"       );

GuiSFX::Dummy         g_OutroStart(          "OutroStart"           );
GuiSFX::Junction3     g_OutroJunction;
GuiSFX::Translator    g_OutroBottomOut(      "OutroBottomOut"       );
GuiSFX::Translator    g_OutroTopOut(         "OutroTopOut"          );
GuiSFX::Show          g_ClipRightShow(       "ClipRightShow"        );
GuiSFX::Pause         g_ClipRightPause(      "ClipRightPause"       );
GuiSFX::Translator    g_ClipRightSlideIn(    "ClipRightSlideIn"     );
GuiSFX::Translator    g_ScreenSlideOut(      "ScreenSlideOut"       );
GuiSFX::Hide          g_OutroHideEverything( "OutroHideEverything"  );
GuiSFX::Dummy         g_OutroDone(           "OutroDone"            );

GuiSFX::IrisWipeOpen  g_IrisOpen(            "IrisOpen"             );
GuiSFX::IrisWipeOpen  g_IrisClose(           "IrisClose"            );

GuiSFX::PulseScale    g_TitlePulse(          "TitlePulse"           );

const float VEHICLE_ODDS_HARD_THRESHOLD = 3.0f;
const float VEHICLE_ODDS_MEDIUM_THRESHOLD = 2.0f;

#ifdef RAD_WIN32
const float MISSION_BITMAP_CORRECTION_SCALE = 0.67f;
#endif

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenMissionSuccess::CGuiScreenMissionSuccess
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
CGuiScreenMissionBase::CGuiScreenMissionBase( Scrooby::Screen* pScreen, CGuiEntity* pParent, eGuiWindowID id ):
    CGuiScreen( pScreen, pParent, id ),
    m_missionTitle( NULL ),
    m_loadCompleted( NULL ),
    m_PlayAnimatedCamera( false ),
    m_gamblingInfo( NULL ),
    m_gamblingEntryFee( NULL ),
    m_gamblingTimeToBeat( NULL ),
    m_gamblingBestTime( NULL ),
    m_gamblingVehicleOdds( NULL ),
    m_gamblingPayout( NULL ),
    m_ReadyToExitScreen( false )
{
    CLASSTRACKER_CREATE( CGuiScreenMissionBase );

    unsigned char* leftChar = reinterpret_cast< unsigned char* >( &g_BackgroundColorLeft );
    radDbgWatchAddUnsignedChar( leftChar + 2, "Red",    "gui\\missionloading\\left" );
    radDbgWatchAddUnsignedChar( leftChar + 1, "Green",  "gui\\missionloading\\left" );
    radDbgWatchAddUnsignedChar( leftChar + 0, "Blue",   "gui\\missionloading\\left" );
    //radDbgWatchAddUnsignedChar( leftChar + 3, "Alpha",  "gui\\missionloading\\left" );

    unsigned char* rightChar = reinterpret_cast< unsigned char* >( &g_BackgroundColorRight );
    radDbgWatchAddUnsignedChar( rightChar + 2, "Red",    "gui\\missionloading\\right" );
    radDbgWatchAddUnsignedChar( rightChar + 1, "Green",  "gui\\missionloading\\right" );
    radDbgWatchAddUnsignedChar( rightChar + 0, "Blue",   "gui\\missionloading\\right" );
    //radDbgWatchAddUnsignedChar( rightChar + 3, "Alpha",  "gui\\missionloading\\right" );




    //
    // Add All the transitions to the screen
    //
    AddTransition( g_DarkenPolyFade    );
    AddTransition( g_TitlePulse        );
    AddTransition( g_ScreenSlideIn     );
    AddTransition( g_ScreenSlideOut    );
    AddTransition( g_ClipLeftSlideOut  );
    AddTransition( g_ClipRightSlideIn  );
    AddTransition( g_OutroBottomOut    );
    AddTransition( g_OutroTopOut       );
    AddTransition( g_IntroPause        );
    AddTransition( g_ClipLeftPause     );
    AddTransition( g_ClipRightPause    );
    DoneAddingTransitions();

    //
    // Retrieve the Scrooby drawing elements.
    //

    //
    // Iris wipe stuff
    //
    Scrooby::Page* irisPage;
	irisPage = m_pScroobyScreen->GetPage( "3dIris" );
	rAssert( irisPage != NULL );

    m_Iris = irisPage->GetPure3dObject( "p3d_iris" );
    rAssert( m_Iris != NULL );

    m_MultiController = p3d::find< tMultiController >( "IrisController" );
    rAssert( m_MultiController );
    m_irisWipeNumFrames = m_MultiController->GetNumFrames();

    //
    // Functional page stuff
    //

    Scrooby::Layer* overlay;

    //
    // Get the elements out of scrooby
    //
	m_Page = m_pScroobyScreen->GetPage(   "MissionLoad" );        rAssert( m_Page          != NULL );
    m_LetterboxPage = m_pScroobyScreen->GetPage( "LetterBox" );   rAssert( m_LetterboxPage != NULL );
    m_FgLayer =      m_Page-> GetGroup(   "foreground" );         rAssert( m_FgLayer       != NULL );
    m_loadCompleted = m_Page->GetGroup(   "LoadCompleted" );      rAssert( m_loadCompleted != NULL );
    m_Line0 = m_FgLayer->GetText(         "MissionInfo" );        rAssert( m_Line0         != NULL );
    m_missionTitle  = m_FgLayer->GetText(   "MissionTitle" );     rAssert( m_missionTitle  != NULL );
    m_BgLayer = m_Page->GetLayer(         "Background" );         rAssert( m_BgLayer       != NULL );
    m_missionStartBitmap = m_Page->GetSprite( "MissionStartBitmap" ); rAssert( m_missionStartBitmap != NULL);
    overlay = m_Page->GetLayer(           "Overlay" );            rAssert( overlay         != NULL );
    m_darkenPolys  = overlay->GetGroup(   "dark_polys" );         rAssert( m_darkenPolys   != NULL );
    m_darkenTop = m_LetterboxPage->GetGroup( "TopBar" );          rAssert( m_darkenTop     != NULL );
    m_darkenBottom = m_LetterboxPage->GetGroup( "BottomBar" );    rAssert( m_darkenBottom  != NULL );
    m_backgroundPoly = m_Page->GetPolygon( "background" );        rAssert( m_backgroundPoly!= NULL );
    m_textOverlays = m_Page->GetGroup(     "Text"  );             rAssert( m_textOverlays  != NULL );
    m_Flag =         m_Page->GetSprite(    "flag"  );             rAssert( m_Flag          != NULL );
    m_ClipLeftGroup = m_Page->GetGroup(    "ClipLeftGroup");      rAssert( m_ClipLeftGroup != NULL );
    m_ClipRightGroup= m_Page->GetGroup(    "ClipRightGroup");     rAssert( m_ClipRightGroup!= NULL );
    m_ClipLeft  =    m_Page->GetSprite(    "clipLeft" );          rAssert( m_ClipLeft      != NULL );
    m_ClipRight =    m_Page->GetSprite(    "clipRight1");         rAssert( m_ClipRight     != NULL );
    m_ClipLeftArm  = m_Page->GetSprite(    "clipArmLeft1" );      rAssert( m_ClipLeftArm   != NULL );
    m_ClipRightArm = m_Page->GetSprite(    "clipArmRight1" );     rAssert( m_ClipRightArm  != NULL );
    m_Foreground =   m_Page->GetLayer(     "Background" );        rAssert( m_Foreground    != NULL );
    m_LetterboxLayer=m_LetterboxPage->GetLayer( "Background" );   rAssert( m_LetterboxLayer!= NULL );

    if( IsWideScreenDisplay() )
    {
        m_Page->ResetTransformation();
        ApplyWideScreenCorrectionScale( m_Page );
    }


    m_buttonIcons[ BUTTON_ICON_ACCEPT ] = m_loadCompleted->GetGroup( "Continue" );
    m_buttonIcons[ BUTTON_ICON_BACK ] = m_loadCompleted->GetGroup( "Abort" );

    //
    // add some scrooby elements to the watcher
    //
    const char* screenName = GetWatcherName();

    #ifdef DEBUGWATCH
        m_missionStartBitmap->  WatchAll( screenName );
        m_missionTitle->        WatchAll( screenName );
        m_Line0->               WatchAll( screenName );
        m_LetterboxPage->       WatchAll( screenName );
        m_LetterboxLayer->      WatchAll( screenName );
        m_darkenTop->           WatchAll( screenName );
        m_darkenBottom->        WatchAll( screenName );
        m_Flag->                WatchAll( screenName );
        m_ClipLeft->            WatchAll( screenName );
        m_ClipRight->           WatchAll( screenName );
        m_ClipLeftArm->         WatchAll( screenName );
        m_ClipRightArm->        WatchAll( screenName );
        m_ClipLeftGroup->       WatchAll( screenName );
        m_ClipRightGroup->      WatchAll( screenName );
    #endif

    this->SetFadingEnabled( false );
    m_ClipRight->Scale( -1.0f, 1.0f, 1.0f );

    // Get gambling info group and text
    //
    m_gamblingInfo = m_Page->GetGroup( "GamblingInfo" );
    rAssert( m_gamblingInfo != NULL );

    m_gamblingEntryFee    = m_gamblingInfo->GetText( "EntryFee_Value" );
    m_gamblingTimeToBeat  = m_gamblingInfo->GetText( "TimeToBeat_Value" );
    m_gamblingBestTime    = m_gamblingInfo->GetText( "BestTime_Value" );
    m_gamblingVehicleOdds = m_gamblingInfo->GetText( "VehicleOdds_Value" );
    m_gamblingPayout      = m_gamblingInfo->GetText( "Payout_Value" );

    g_BackgroundColorLeft  = m_backgroundPoly->GetVertexColour( 0 );
    g_BackgroundColorRight = m_backgroundPoly->GetVertexColour( 2 );

    m_Flag->ResizeToBoundingBox();

    //
    // Set Up Transitions
    //

    // Continuous
    g_TitlePulse.SetDrawable( m_textOverlays );
    g_TitlePulse.Activate();
    g_TitlePulse.SetAmplitude( 0.05f );
    g_TitlePulse.SetFrequency( 2.0f );
    WATCH( g_TitlePulse, GetWatcherName() );

    // Intro
    WATCH( g_IntroStart, GetWatcherName() );

    g_ForegroundShow.SetDrawable( m_Foreground );

    g_ClipLeftShow.SetDrawable( m_ClipLeftGroup );

    g_IntroPause.SetNumberOfFrames( 1 );

    g_ScreenSlideIn.SetDrawable( m_Foreground );
    g_ScreenSlideIn.SetStartOffscreenLeft( m_Foreground );
    g_ScreenSlideIn.SetTimeInterval( 400.0f );
    WATCH( g_ScreenSlideIn, GetWatcherName() );

    g_ClipLeftPause.SetTimeInterval( CLIP_PAUSE_TIME );
    WATCH( g_ClipLeftPause, GetWatcherName() );

    g_ClipLeftSlideOut.SetDrawable( m_ClipLeftGroup );
    g_ClipLeftSlideOut.SetEndOffscreenLeft( m_ClipLeftGroup );
    g_ClipLeftSlideOut.SetTimeInterval( CLIP_MOVE_TIME );
    WATCH( g_ClipLeftSlideOut, GetWatcherName() );

    g_ClipLeftHide.SetDrawable( m_ClipLeftGroup );
    
    // Outro
    WATCH( g_OutroStart, GetWatcherName() );

    g_ClipRightShow.SetDrawable( m_ClipRightGroup );

    g_OutroTopOut.SetDrawable( m_darkenTop );
    g_OutroTopOut.SetEndOffscreenTop( m_darkenTop );
    g_OutroTopOut.SetTimeInterval( 1000.0f );
    WATCH( g_OutroTopOut, GetWatcherName() );

    g_OutroBottomOut.SetDrawable( m_darkenBottom );
    g_OutroBottomOut.SetEndOffscreenBottom( m_darkenBottom );
    g_OutroBottomOut.SetTimeInterval( 1000.0f );
    WATCH( g_OutroBottomOut, GetWatcherName() );

    g_ClipRightSlideIn.SetDrawable( m_ClipRightGroup );
    g_ClipRightSlideIn.SetStartOffscreenRight( m_ClipRightGroup );
    g_ClipRightSlideIn.SetTimeInterval( CLIP_MOVE_TIME );
    WATCH( g_ClipRightSlideIn, GetWatcherName() );

    g_ClipRightPause.SetTimeInterval( CLIP_PAUSE_TIME );
    WATCH( g_ClipRightPause, GetWatcherName() );

    g_ScreenSlideOut.SetDrawable( m_Foreground );
    g_ScreenSlideOut.SetEndOffscreenRight( m_Foreground );
    g_ScreenSlideOut.SetTimeInterval( 400.0f );
    WATCH( g_ScreenSlideOut, GetWatcherName() );

    g_BackgroundShow.SetDrawable( m_BgLayer );

    g_DarkenPolyFade.SetDrawable( m_darkenPolys );
    g_DarkenPolyFade.SetStartColour( tColour( 255, 255, 255, 255 ) );
    g_DarkenPolyFade.SetEndColour(   tColour( 255, 255, 255,   0 ) );
    g_DarkenPolyFade.SetTimeInterval( 100.0f );

    g_OutroHideEverything.SetDrawable( m_Foreground );

    m_missionStartBitmap->SetRawSprite( NULL );
#ifdef RAD_WIN32
    m_missionStartBitmap->ResetTransformation();
    m_missionStartBitmap->ScaleAboutCenter( MISSION_BITMAP_CORRECTION_SCALE );
    m_missionStartBitmap->Translate( -71, -32 );  // These are trial & error numbers that hopefully work.
#endif

    // text wrap mission info title and description
    //
    m_missionTitle->SetTextMode( Scrooby::TEXT_WRAP );
    m_Line0->SetTextMode( Scrooby::TEXT_WRAP );

    //
    // Set up transitions for lines of text
    //

    //
    // Transition sequencing
    //
    g_IntroStart.               SetNextTransition( g_IntroJunction            );
    g_IntroJunction.            SetNextTransition( 0, g_IntroPause            );
    g_IntroJunction.            SetNextTransition( 1, g_ClipLeftShow          );
    g_IntroJunction.            SetNextTransition( 2, g_ScreenSlideIn         );
        g_ForegroundShow.           SetNextTransition( NULL                   );
        g_ClipLeftShow.             SetNextTransition( NULL                   );
        g_IntroPause.               SetNextTransition( g_ForegroundShow       );
    g_ScreenSlideIn.            SetNextTransition( g_ClipLeftPause            );
    g_ClipLeftPause.            SetNextTransition( g_ClipLeftSlideOut         );
    g_ClipLeftSlideOut.         SetNextTransition( g_ClipLeftHide             );
    g_ClipLeftHide.             SetNextTransition( g_BackgroundShow           );
    g_BackgroundShow.           SetNextTransition( g_DarkenPolyFade           );
    g_DarkenPolyFade.           SetNextTransition( NULL                       );

    g_OutroStart.               SetNextTransition( g_OutroJunction            );
    g_OutroJunction.            SetNextTransition( 0,  g_ClipRightShow        );
    g_OutroJunction.            SetNextTransition( 1,  g_OutroBottomOut       );
    g_OutroJunction.            SetNextTransition( 2,  g_OutroTopOut          );
        g_OutroBottomOut.       SetNextTransition( NULL                       );
        g_OutroTopOut.          SetNextTransition( NULL                       );
    g_ClipRightShow.            SetNextTransition( g_ClipRightSlideIn         );
    g_ClipRightSlideIn.         SetNextTransition( g_ClipRightPause           );
    g_ClipRightPause.           SetNextTransition( g_ScreenSlideOut           );
    g_ScreenSlideOut.           SetNextTransition( g_OutroHideEverything      );
    g_OutroHideEverything.      SetNextTransition( g_OutroDone                );
    g_OutroDone.                SetNextTransition( NULL                       );
}

//===========================================================================
// CGuiScreenMissionBase::~CGuiScreenMissionBase
//===========================================================================
// Description: destructor
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
CGuiScreenMissionBase::~CGuiScreenMissionBase()
{
    CLASSTRACKER_DESTROY( CGuiScreenMissionBase );
    if( s_AnimatedBitmapSprite)
    {
        tRefCounted::Release(s_AnimatedBitmapSprite);
    }

    //Chuck release the special patty and selma screen if we are still holding on to it.
    if(sp_PattyAndSelmaScreenPNG)
    {
        tRefCounted::Release(sp_PattyAndSelmaScreenPNG);
        sp_PattyAndSelmaScreenPNG = NULL;
    }
    s_BitmapLoadPending = false;
    p3d::inventory->RemoveSectionElements( tEntity::MakeUID( "DynamicHud" ) );
}

//===========================================================================
// CGuiScreenMissionBase::ClearBitmap
//===========================================================================
// Description: clears the currently loaded animated bitmap
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      pointer to the abort bitmap group
//
//===========================================================================
void CGuiScreenMissionBase::ClearBitmap()
{
    s_AnimatedBitmapSprite = NULL;
}

//===========================================================================
// CGuiScreenMissionBase::GetAbortBitmap
//===========================================================================
// Description: Allows access to the stored group pointer
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      pointer to the abort bitmap group
//
//===========================================================================
Scrooby::Group* CGuiScreenMissionBase::GetAbortBitmap()
{
    return m_buttonIcons[ BUTTON_ICON_BACK ];
}

//===========================================================================
// CGuiScreenMissionBase::GetBitmapName
//===========================================================================
// Description: Allows access to the name of the mission briefing pic
//
// Constraints:	None.
//
// Parameters:	buffer - filled in with the name
//
// Return:      pointer to the flag
//
//===========================================================================
void CGuiScreenMissionBase::GetBitmapName( char* buffer )
{
    if( buffer != NULL )
    {
        ::strcpy( buffer, s_AnimatedBitmapName );
    }
}

//===========================================================================
// CGuiScreenMissionBase::GetFlag
//===========================================================================
// Description: Allows access to the flag bitmap
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      pointer to the flag
//
//===========================================================================
Scrooby::Drawable* CGuiScreenMissionBase::GetFlag()
{
    return m_Flag;
}

//===========================================================================
// CGuiScreenMissionBase::GetLoadCompletedGroup
//===========================================================================
// Description: Allows access to the stored group pointer
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      pointer to the pace
//
//===========================================================================
Scrooby::Group* CGuiScreenMissionBase::GetLoadCompletedGroup()
{
    return m_loadCompleted;
}

//===========================================================================
// CGuiScreenMissionBase::GetMissionInfoText
//===========================================================================
// Description: Allows access to the stored text pointer
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      pointer to the text
//
//===========================================================================
Scrooby::Text* CGuiScreenMissionBase::GetMissionInfoText()
{
    return m_Line0;
}

//===========================================================================
// CGuiScreenMissionBase::GetMissionStartBitmap
//===========================================================================
// Description: Allows access to the stored mission start bitmap
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      pointer to the drawable
//
//===========================================================================
Scrooby::BoundedDrawable* CGuiScreenMissionBase::GetMissionStartBitmap()
{
    return m_missionStartBitmap;
}

//===========================================================================
// CGuiScreenMissionBase::GetPage
//===========================================================================
// Description: Allows access to the stored page pointer
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      pointer to the pace
//
//===========================================================================
Scrooby::Page* CGuiScreenMissionBase::GetPage()
{
    return m_Page;
}

//===========================================================================
// CGuiScreenMissionBase::GetPage
//===========================================================================
// Description: Allows access to the stored page pointer
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      pointer to the pace
//
//===========================================================================
Scrooby::Text*  CGuiScreenMissionBase::GetTitleText()
{
    return m_missionTitle;
}

//===========================================================================
// CGuiScreenMissionBase::GetWatcherName
//===========================================================================
// Description: Allows access to the stored page pointer
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      pointer to the pace
//
//===========================================================================
const char* CGuiScreenMissionBase::GetWatcherName() const
{
    return "GuiScreenMissionBase";
}

//===========================================================================
// CGuiScreenMissionBase::HandleMessage
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
void CGuiScreenMissionBase::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if( message == GUI_MSG_UPDATE )
    {
        float deltaT = static_cast< float >( param1 );
        ResetMovableObjects();
        m_ClipLeftArm->ResizeToBoundingBox();
        m_ClipRightArm->ResizeToBoundingBox();
        m_ClipLeftGroup->ResetTransformation();
        UpdateTransitions( deltaT );
        m_FgLayer->SetColour( tColour( 255, 255, 255, 255 ) );
        m_missionStartBitmap->SetColour( tColour( 255, 255, 255, 255 ) );
        m_backgroundPoly->SetVertexColour( 0, g_BackgroundColorLeft  );
        m_backgroundPoly->SetVertexColour( 1, g_BackgroundColorLeft  );
        m_backgroundPoly->SetVertexColour( 2, g_BackgroundColorRight );
        m_backgroundPoly->SetVertexColour( 3, g_BackgroundColorRight );
    }

    if( message == GUI_MSG_WINDOW_EXIT )
    {
    }

    if( m_state == GUI_WINDOW_STATE_INTRO )
    {
        if( message == GUI_MSG_UPDATE )
        {
//            --m_numTransitionsPending; //get us out of the intro state
            float deltaT = static_cast< float >( param1 );
            UpdateAnimatedBitmap( deltaT );
        }
    }

    if( m_state == GUI_WINDOW_STATE_OUTRO )
    {
        if( message == GUI_MSG_UPDATE )
        {
            if( g_OutroDone.IsDone() )
            {
                --m_numTransitionsPending;
                OutroDone();
            }
        }
    }

    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                UpdateAnimatedBitmap( static_cast< float >( param1 ) );
                break;
            }
            default:
            {
                break;
            }
        };
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}

//===========================================================================
// CGuiScreenMissionBase::InitIntro
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
void CGuiScreenMissionBase::InitIntro()
{
    m_ReadyToExitScreen = false;
    if( ( !s_BitmapLoadPending ) && ( s_AnimatedBitmapSprite == NULL) )
    {
        ReplaceBitmap();
    }

    //
    // Need to reset all the transitions
    //
    this->SetButtonVisible( BUTTON_ICON_BACK, true );
    m_LetterboxPage->SetVisible( true  );
    m_Page->         SetVisible( true  );
    m_Line0->        SetVisible( false );
    m_BgLayer->      SetVisible( true  );
    m_FgLayer->      SetVisible( true  );
    m_Iris->         SetVisible( false );
    m_missionTitle-> SetVisible( false );
    m_Flag->         SetVisible( false );
    m_ClipLeftGroup->SetVisible( true  );
    m_ClipRightGroup->SetVisible( false );
    m_Foreground->   SetVisible( false );
    m_darkenPolys->  SetColour( tColour( 255, 255, 255, 255 ) );
    m_darkenBottom-> ResetTransformation();
    m_darkenTop->    ResetTransformation();
    m_ClipLeft->     ResetTransformation();


    m_MultiController->Reset();
    m_MultiController->SetRelativeSpeed( 1.0f );
    m_MultiController->SetFrameRange( 0.0f, m_irisWipeNumFrames );
    m_MultiController->SetFrame( 0.0f );
    unsigned int size = m_textOverlays->Size();
    unsigned int i;
    for( i = 0; i < size; ++i )
    {
        Scrooby::Drawable* drawable = m_textOverlays->GetChildDrawable( i );
        drawable->SetVisible( false );
    }

    ResetTransitions();

    g_IntroStart.Activate();
    g_OutroStart.DeactivateChain();

    const int currentLevelIndex = GetGameplayManager()->GetCurrentLevelIndex();
    int currentMissionIndex = GetGameplayManager()->GetCurrentMissionIndex();

    int currentMissionNum = GetGameplayManager()->GetCurrentMissionNum();
    if( currentMissionNum >= GameplayManager::MAX_MISSIONS )
    {
        // current mission must be either a street race or a bonus mission
        //
        currentMissionIndex = currentMissionNum - GameplayManager::MAX_MISSIONS +
                              MAX_NUM_REGULAR_MISSIONS;
    }
    else
    {
        // special case for level 1 due to tutorial mission
        //
        if( currentLevelIndex == RenderEnums::L1 )
        {
            currentMissionIndex--;
        }

#ifdef RAD_E3
        // TC: *** quick and dirty E3 hack!
        //
        if( currentLevelIndex == RenderEnums::L2 )
        {
            currentMissionIndex = RenderEnums::M5;
        }
#endif
    }

    int textIndex = currentLevelIndex * MAX_NUM_MISSIONS_PER_LEVEL + currentMissionIndex;
    int missionNumber = textIndex % MAX_NUM_MISSIONS_PER_LEVEL ;

    // special case for level 1 due to tutorial mission
    //
    if( currentLevelIndex == 0 )
    {
        if( textIndex < 0 ) // meaning it must be the tutorial mission
        {
            const int NUM_LEVELS = 7;
            textIndex = MAX_NUM_MISSIONS_PER_LEVEL * NUM_LEVELS;
            missionNumber = MAX_NUM_MISSIONS_PER_LEVEL;
        }
    }

    m_missionTitle->SetIndex( textIndex );
    m_Line0->SetIndex( textIndex );

    //
    // Set the Background Color of the screen
    //
    bool bonusMission = GetGameplayManager()->GetCurrentMission()->IsBonusMission();
    bool raceMission  = GetGameplayManager()->GetCurrentMission()->IsRaceMission();
    bool wagerMission = GetGameplayManager()->GetCurrentMission()->IsWagerMission();

    if( wagerMission )
    {
        m_backgroundPoly->SetColour( g_BackgroundGreen );
        g_BackgroundColorLeft  = g_ColorWagerLeft;
        g_BackgroundColorRight = g_ColorWagerRight;        
    }
    else if( bonusMission )
    {
        m_backgroundPoly->SetColour( g_BackgroundRed );
        g_BackgroundColorLeft  = g_ColorBonusLeft;
        g_BackgroundColorRight = g_ColorBonusRight;        
    }
    else if( raceMission )
    {
        m_backgroundPoly->SetColour( g_BackgroundBlue );
        g_BackgroundColorLeft  = g_ColorRaceLeft;
        g_BackgroundColorRight = g_ColorRaceRight;        
    }
    else
    {
        m_backgroundPoly->SetColour( g_BackgroundBlue );
        g_BackgroundColorLeft  = g_ColorNormalLeft;
        g_BackgroundColorRight = g_ColorNormalRight;        
    }

    if( wagerMission || bonusMission || raceMission )
    {
        s_AnimatedBitmapSprite = NULL;
        //SetBitmapName( NULL );
    }

    rAssert( m_gamblingInfo != NULL );
    m_gamblingInfo->SetVisible( false ); // hide by default

    this->SetButtonVisible( BUTTON_ICON_ACCEPT, true ); // show by default

    //
    // Inform the sound manager that it's time to turn the sound down a bit
    //
    GetSoundManager()->OnMissionBriefingStart();
}

//===========================================================================
// CGuiScreenMissionLoad::InitIntroWagerMission
//===========================================================================
// Description: sets up stuff for the wager mission screen
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionBase::InitIntroWagerMission()
{
    int textIndex = m_Line0->GetNumOfStrings() - 2;
    int entryFee = 0; // in coins
    Mission* currentMission = GetGameplayManager()->GetCurrentMission();
    rAssert( currentMission != NULL );
    MissionStage* ms = currentMission->GetStage( 0 );
    rAssert( ms != NULL );
    CoinObjective* coinObjective = dynamic_cast<CoinObjective*>( ms->GetObjective() );
    rAssertMsg( coinObjective != NULL, "No coin objective in first stage!" );
    entryFee = coinObjective->GetCoinAmount();

    if( entryFee > GetCoinManager()->GetBankValue() )
    {
        //
        // Disable accept button
        //
        this->SetButtonVisible( BUTTON_ICON_ACCEPT, false );

        //
        // Change the info text to the "You need (x) coins to continue" message
        //
        m_Line0->SetIndex( textIndex );
        UnicodeString us = m_Line0->GetString( textIndex );
        char number[ 256 ] = "";
        sprintf( number, "%d", entryFee );
        us.Replace( "%D", number );

        m_Line0->SetString( textIndex + 1, us );
        m_Line0->SetIndex ( textIndex + 1 );
    }
}

//===========================================================================
// CGuiScreenMissionLoad::InitOutro
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
void CGuiScreenMissionBase::InitOutro()
{
    //This is a hack to get the stage setup before we start showing the world after
    //the mission briefing screen.  This simulates a one frame step that sets up and
    //"starts" the mission before we leave the pause context.  NOTE:  There may be
    //seom other managers that need to be run here.
    GetMissionManager()->Update( 16 );
    GetSuperCamManager()->GetSCC( 0 )->NoTransition();
    GetSuperCamManager()->Update( 16, true );
    GetCharacterManager()->PreSimUpdate( 0.0001f );
    GetCharacterManager()->PreSubstepUpdate( 0.0001f );
    GetCharacterManager()->Update( 0.0001f );
    GetCharacterManager()->PostSubstepUpdate( 0.0001f );
    GetCharacterManager()->PostSimUpdate( 0.0001f );

    g_OutroDone.Reset();
    g_OutroStart.Activate();
    ++m_numTransitionsPending;

    //
    // Turn the sound back up
    //
    GetSoundManager()->OnMissionBriefingEnd();
}

//===========================================================================
// CGuiScreenMissionBase::InitRunning
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
void CGuiScreenMissionBase::InitRunning()
{
}

//===========================================================================
// CGuiScreenMissionBase::IsCurrentBitmap
//===========================================================================
// Description: deetermines if the current bitmap loaded corresponds to this
//              name
//
// Constraints:	None.
//
// Parameters:	name - the string name to check.
//
// Return:      N/A.
//
//===========================================================================
bool CGuiScreenMissionBase::IsCurrentBitmap( const char* name )
{
    bool returnMe = ( strcmp( name, s_AnimatedBitmapName ) == 0 );
    return returnMe;
}

//===========================================================================
// CGuiScreenMissionBase::OutroDone
//===========================================================================
// Description: called when the outro is done and we're ready to leave this 
//              screen
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionBase::OutroDone()
{
    UnloadBitmap();

    //
    // If this is a race, we need to play a camera
    //
    if( m_PlayAnimatedCamera )
    {
        //
        // If it's a race mission, don't allow skipping of the animated cam
        //
        bool raceMission  = GetGameplayManager()->GetCurrentMission()->IsRaceMission();
        if( raceMission )
        {
            AnimatedCam::AllowSkipping( false );
            AnimatedCam::CheckPendingCameraSwitch();

            //
            // we need to go to the gameplay context. This is so not cool it almost makes me sick - IAN
            //
            GetGameFlow()->SetContext( CONTEXT_GAMEPLAY );
        }
        m_PlayAnimatedCamera = false;
    }
    else
    {
        AnimatedCam::SetCamera( "" );
        AnimatedCam::SetMulticontroller( "" );
    }
}

//===========================================================================
// CGuiScreenMissionBase::RemoveAnimatedBitmap
//===========================================================================
// Description: gets rid of the animated bitmap
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionBase::RemoveAnimatedBitmap()
{
    if( s_AnimatedBitmapSprite != NULL )
    {
        s_AnimatedBitmapSprite->Release();
        s_AnimatedBitmapSprite = NULL;
    }
}

//===========================================================================
// CGuiScreenMissionBase::ReplaceBitmap
//===========================================================================
// Description: replaces the bitmap with a sprite from a file
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionBase::ReplaceBitmap()
{
    p3d::pddi->DrawSync();
    p3d::inventory->RemoveSectionElements( tEntity::MakeUID( "DynamicHud" ) );
    RemoveAnimatedBitmap();

    rWarning( s_BitmapLoadPending != true );
    s_BitmapLoadPending = true;
    rAssert( strlen( s_AnimatedBitmapName ) != 0 );
    if( strlen( s_AnimatedBitmapName ) != 0 )
    {
        tRefCounted::Release( s_AnimatedBitmapSprite );
        tFileHandler* fileHandler = p3d::loadManager->GetHandler( "png" );
        rAssert( fileHandler != NULL );
        tImageHandler* pngHandler = dynamic_cast< tImageHandler* >( fileHandler );
        rAssert( pngHandler != NULL );
        pngHandler->SetLoadType( tImageHandler::SPRITE );
        LoadingManager::GetInstance()->AddRequest
        (
            FILEHANDLER_PURE3D, 
            s_AnimatedBitmapName,
            GMA_LEVEL_OTHER,
            "DynamicHud"
        );
    }
}

//===========================================================================
// CGuiScreenMissionBase::SetBitmapName
//===========================================================================
// Description: sets up the name of the animated bitmap that will be loaded 
//              for this mission
//
// Constraints:	None.
//
// Parameters:	name - string representing the name of this bitmap on disk
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionBase::SetBitmapName( const char* name )
{
    //
    // If we're erasing the bitmap
    //
    if( name == NULL )
    {
        strcpy( s_AnimatedBitmapName, "" );
        strcpy( s_AnimatedBitmapShortName, "" );
        return;
    }

    //
    // If we're not really changing anything, early out
    //
    if( strcmp( name, s_AnimatedBitmapName ) == 0 )
    {
        return;
    }

    //save the whole filename

    //chuck: we need to somehow release the sprite or 
    //not set the s_AnimatedBitmapSprite = NULL because it 
    //will leak the the patty and selma screen. 
    //tRefCounted::Release(s_AnimatedBitmapSprite);
    
    //check if our static pointer is to the patty and selma screen
    if (s_AnimatedBitmapSprite != NULL)
    {
//    s_AnimatedBitmapSprite->Release();
        
        if  (
                    s_AnimatedBitmapSprite->GetUID() == tName::MakeUID("misXX_PS.png")
                ||
                     s_AnimatedBitmapSprite->GetUID() == tName::MakeUID("misXX_HW.png")                   
            )
        {
            //if it is then we make a switch
            //tRefCounted::Assign(sp_PattyAndSelmaScreenPNG,s_AnimatedBitmapSprite);
            rAssert(sp_PattyAndSelmaScreenPNG == NULL);
            sp_PattyAndSelmaScreenPNG = s_AnimatedBitmapSprite;
        }
    }

    s_AnimatedBitmapSprite = NULL;
    size_t length = strlen( name );
    rAssert( length < sizeof( s_AnimatedBitmapName ) );

    //
    // If there's no text just return
    //
    if( length == 0 )
    {
        return;
    }

    //
    // if we're not actually changing anything, return
    //
    if( strcmp( s_AnimatedBitmapName, name ) == 0 )
    {
        return;
    }
    strcpy( s_AnimatedBitmapName, name );
    
    //figure out and save the name of the sprite from the filename
    if( length >= 12 )
    {
        const char* spriteName = name + length - 12;
        strcpy( s_AnimatedBitmapShortName, spriteName );
        //s_AnimatedBitmapShortName[ 09 ] = 'p';
        s_AnimatedBitmapShortName[ 10 ] = 'n';
        s_AnimatedBitmapShortName[ 11 ] = 'g';
    }
    else
    {
        strcpy( s_AnimatedBitmapShortName, "" );
    }
}

//===========================================================================
// CGuiScreenMissionBase::SetPlayAnimatedCamera
//===========================================================================
// Description: sets a flag to tell us whether or not to play the animated
//              camera after we exit this screen
//
// Constraints:	None.
//
// Parameters:	None
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionBase::SetPlayAnimatedCamera( bool play )
{
    m_PlayAnimatedCamera = play;
}

//===========================================================================
// CGuiScreenMissionBase::StartIrisWipeClose
//===========================================================================
// Description: starts the transition to close the iris
//
// Constraints:	None.
//
// Parameters:	None
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionBase::StartIrisWipeClose()
{
//    ++m_numTransitionsPending;
    m_MultiController->Reset();
    float frames = m_MultiController->GetNumFrames();
    m_MultiController->SetFrameRange( 0.0f, frames * 0.5f );
    m_MultiController->SetFrame( 0.0f );
}
//===========================================================================
// CGuiScreenMissionBase::StartIrisWipeOpen
//===========================================================================
// Description: starts the transition to open the iris
//
// Constraints:	None.
//
// Parameters:	None
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionBase::StartIrisWipeOpen()
{
//    ++m_numTransitionsPending;
    m_MultiController->Reset();
    float frames = m_MultiController->GetNumFrames();
    m_MultiController->SetFrameRange( frames * 0.5f, frames );
    m_MultiController->SetFrame( frames * 0.5f );
}

//===========================================================================
// CGuiScreenMissionBase::UnloadBitmap
//===========================================================================
// Description: unloads the bitmap, and reclaims all lost memory
//
// Constraints:	None.
//
// Parameters:	None
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionBase::UnloadBitmap()
{
    p3d::pddi->DrawSync();
    //::radThreadSleep (500);  // Hmmmm, this shouldn't be required
    tRefCounted::Release( s_AnimatedBitmapSprite );
    m_missionStartBitmap->SetRawSprite( NULL );
    m_missionStartBitmap->SetVisible( false );
}

//===========================================================================
// CGuiScreenMissionBase::UpdateAnimatedBitmap
//===========================================================================
// Description: Updates animation of the bitmap
//
// Constraints:	None.
//
// Parameters:	deltaT - how much time has elapsed
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionBase::UpdateAnimatedBitmap( const float deltaT )
{
    if( s_AnimatedBitmapSprite == NULL ) 
    {
        p3d::inventory->PushSection();

        p3d::pddi->DrawSync();
        p3d::inventory->SelectSection( "DynamicHud" );
        p3d::inventory->SetCurrentSectionOnly( false );
        tEntityTable* currentSection = p3d::inventory->GetSection( "DynamicHud" );
        
        HeapMgr()->PushHeap( GMA_TEMP );
        tInventory::Iterator< tEntity > it( p3d::inventory );
        HeapMgr()->PopHeap( GMA_TEMP );

        tEntity* entity = it.First();
        while( entity != NULL )
        {
            if( entity->GetNameObject() == s_AnimatedBitmapShortName )
            {
                tSprite* sprite = dynamic_cast< tSprite* >( entity );
                tRefCounted::Assign( s_AnimatedBitmapSprite, sprite );
            }
            entity = it.Next();
        }

        if( s_AnimatedBitmapSprite == NULL )
        {
            tRefCounted::Assign( s_AnimatedBitmapSprite, p3d::find< tSprite >( s_AnimatedBitmapShortName ) );
        }
        if( s_AnimatedBitmapSprite == NULL )
        {
            //loading is not yet complete
            m_missionStartBitmap->SetVisible( false );
        }
        else
        {
            p3d::inventory->RemoveSectionElements( tEntity::MakeUID( "DynamicHud" ) );
            #ifdef RAD_DEBUG
                tSprite* stillThere = p3d::find< tSprite >( s_AnimatedBitmapShortName );
                rWarningMsg( stillThere == NULL, "We're leaking memory if we can't remove this" );
            #endif
            m_missionStartBitmap->SetVisible( true );
            s_BitmapLoadPending = false;
        }

        p3d::inventory->PopSection();

    }
    m_missionStartBitmap->SetRawSprite( s_AnimatedBitmapSprite );
    
    //Chuck I hope this a safe place to release the patty and selma screen
    if (sp_PattyAndSelmaScreenPNG != NULL)
    {
        tRefCounted::Release(sp_PattyAndSelmaScreenPNG);
        sp_PattyAndSelmaScreenPNG= NULL;
    }
}

void
CGuiScreenMissionBase::UpdateGamblingInfo()
{
    rAssert( m_gamblingInfo != NULL );
    m_gamblingInfo->SetVisible( true );

    HeapMgr()->PushHeap( GMA_LEVEL_HUD );

    char buffer[ 32 ];

    Mission* currentMission = GetGameplayManager()->GetCurrentMission();
    rAssert( currentMission != NULL );

    // set entry fee
    //
    int entryFee = 0; // in coins

    CoinObjective* coinObjective = dynamic_cast<CoinObjective*>( currentMission->GetStage( 0 )->GetObjective() );
    if( coinObjective != NULL )
    {
        entryFee = coinObjective->GetCoinAmount();
    }
    else
    {
        rAssertMsg( false, "No coin objective in first stage!" );
    }

    sprintf( buffer, "%d", entryFee );
    rAssert( m_gamblingEntryFee != NULL );
    m_gamblingEntryFee->SetString( 0, buffer );

    // if not enough coins to gamble, disable 'accept button'\
    //
    InitIntroWagerMission();

    // set time to beat
    //
    int timeToBeat = 0; // in seconds

    RaceObjective* raceObjective = dynamic_cast<RaceObjective*>( currentMission->GetStage( 1 )->GetObjective() );
    if( raceObjective != NULL )
    {
        timeToBeat = raceObjective->GetParTime();
    }
    else
    {
        rAssertMsg( false, "No race objective in second stage!" );
    }

    sprintf( buffer, "%d:%02d", timeToBeat / 60, timeToBeat % 60 );
    rAssert( m_gamblingTimeToBeat != NULL );
    m_gamblingTimeToBeat->SetString( 0, buffer );

    // set best time
    //
    RenderEnums::LevelEnum currentLevel = GetGameplayManager()->GetCurrentLevelIndex();
    int bestTime = GetCharacterSheetManager()->GetGambleRaceBestTime( currentLevel );
    if( bestTime < 0 )
    {
        // if no best time set yet, just set it to the time to beat
        //
        bestTime = timeToBeat;
    }

    sprintf( buffer, "%d:%02d", bestTime / 60, bestTime % 60 );
    rAssert( m_gamblingBestTime != NULL );
    m_gamblingBestTime->SetString( 0, buffer );

    // set vehicle odds
    //
    float oddsRatio = 1.0f;

    Vehicle* currentVehicle = GetGameplayManager()->GetCurrentVehicle();
    if( currentVehicle != NULL )
    {
        oddsRatio = currentVehicle->GetGamblingOdds();
    }
    else
    {
        rAssertMsg( false, "What? How do you expect to race w/out a vehicle??" );
    }

    rAssert( m_gamblingVehicleOdds != NULL );
    if( oddsRatio >= VEHICLE_ODDS_HARD_THRESHOLD ) // hard
    {
        m_gamblingVehicleOdds->SetIndex( VEHICLE_ODDS_HARD );
    }
    else if( oddsRatio >= VEHICLE_ODDS_MEDIUM_THRESHOLD ) // medium
    {
        m_gamblingVehicleOdds->SetIndex( VEHICLE_ODDS_MEDIUM );
    }
    else // easy
    {
        m_gamblingVehicleOdds->SetIndex( VEHICLE_ODDS_EASY );
    }
/*
    sprintf( buffer, "%.1f : 1", oddsRatio );
    rAssert( m_gamblingVehicleOdds != NULL );
    m_gamblingVehicleOdds->SetString( 0, buffer );
*/

    // set payout amount
    //
    int payoutAmount = entryFee + (int)( entryFee * oddsRatio );

    sprintf( buffer, "%d", payoutAmount );
    rAssert( m_gamblingPayout != NULL );
    m_gamblingPayout->SetString( 0, buffer );

    HeapMgr()->PopHeap( GMA_LEVEL_HUD );
}

