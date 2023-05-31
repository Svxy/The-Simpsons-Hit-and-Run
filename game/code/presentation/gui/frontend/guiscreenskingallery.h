//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenSkinGallery
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/21      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENSKINGALLERY_H
#define GUISCREENSKINGALLERY_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <loading/loadingmanager.h>

//===========================================================================
// Forward References
//===========================================================================

class CGuiMenu2D;
class Reward;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenSkinGallery : public CGuiScreen,
                              public LoadingManager::ProcessRequestsCallback
{
public:
    CGuiScreenSkinGallery( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenSkinGallery();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }

    // Implements LoadingManager::ProcessRequestsCallback
    //
    virtual void OnProcessRequestsComplete( void* pUserData );

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    void OnUpdate( unsigned int elapsedTime );
    void OnMenuSelectionChange( int selection );
    void OnMenuSelectionMade( int selection );
    void SetMenuAlpha( float alpha );
#ifdef RAD_WIN32
    void SetVisibilityForAllOtherMenuItems( bool bDisable );
#endif

    void Load2DImages();
    void Unload2DImages();

    static const int MAX_NUM_SKINS_PER_LEVEL = 3;

    CGuiMenu2D* m_pMenu;

    Reward* m_rewardSelections[ MAX_NUM_SKINS_PER_LEVEL ];
    int m_numSelections;

    bool m_isSkinsLoaded : 1;

    enum eScreenState
    {
        SCREEN_STATE_NORMAL,
        SCREEN_STATE_GOTO_VIEW,
        SCREEN_STATE_VIEWING,
        SCREEN_STATE_BACK_VIEW,

        NUM_SCREEN_STATES
    };

    eScreenState m_screenState;
    unsigned int m_elapsedTime;
    rmt::Vector m_projectileVelocity;

    Scrooby::Layer* m_skinInfo;
    Scrooby::Text* m_skinName;

#ifdef RAD_WIN32
    int m_selectedSkin;
#endif
};

#endif // GUISCREENSKINGALLERY_H
