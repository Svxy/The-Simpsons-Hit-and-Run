//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLicense
//
// Description: Implementation of the CGuiScreenLicense class.
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
#include <presentation/gui/bootup/guiscreenlicense.h>

#include <contexts/bootupcontext.h>

#include <screen.h>
#include <page.h>
#include <group.h>

#include <p3d/utility.hpp>
#include <p3d/sprite.hpp>
#include <raddebug.hpp> // Foundation

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const float LICENSE_SCREEN_FADE_TIME = 500.0f;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenLicense::CGuiScreenLicense
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
CGuiScreenLicense::CGuiScreenLicense
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_LICENSE )
{
    // retrieve Scrooby drawing elements
    //
    rAssert( m_pScroobyScreen != NULL );
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "License" );
    rAssert( pPage != NULL );

    // set platform-specific license screen image
    //
#ifdef RAD_GAMECUBE
    tSprite* pSprite = p3d::find<tSprite>( "licenseG.png" );
#endif
#ifdef RAD_PS2
    tSprite* pSprite = p3d::find<tSprite>( "licenseP.png" );
#endif
#ifdef RAD_XBOX
    tSprite* pSprite = p3d::find<tSprite>( "licenseX.png" );
#endif
#ifdef RAD_WIN32
    tSprite* pSprite = p3d::find<tSprite>( "licensePC.png" );
#endif
    rAssert( pSprite != NULL );

    Scrooby::Sprite* licenseImage = pPage->GetSprite( "License" );
    rAssert( licenseImage != NULL );
    licenseImage->SetRawSprite( pSprite );

    // set fade time for license screen (in milliseconds)
    //
    this->SetFadeTime( LICENSE_SCREEN_FADE_TIME );

#ifdef RAD_GAMECUBE
    // skip screen fade in (on Gamecube only)
    //
    this->SetFadeTime( 0.0f );
#endif
}


//===========================================================================
// CGuiScreenLicense::~CGuiScreenLicense
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
CGuiScreenLicense::~CGuiScreenLicense()
{
}


//===========================================================================
// CGuiScreenLicense::HandleMessage
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
void CGuiScreenLicense::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenLicense::InitIntro
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
void CGuiScreenLicense::InitIntro()
{
    // start loading sound files
    //
    GetBootupContext()->StartLoadingSound();
}


//===========================================================================
// CGuiScreenLicense::InitRunning
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
void CGuiScreenLicense::InitRunning()
{
#ifdef RAD_GAMECUBE
    // allow screen fade out (on Gamecube only)
    this->SetFadeTime( LICENSE_SCREEN_FADE_TIME );
#endif

    GetBootupContext()->ResetLicenseScreenDisplayTime();
}


//===========================================================================
// CGuiScreenLicense::InitOutro
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
void CGuiScreenLicense::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

