//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMissionGallery
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/19      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENMISSIONGALLERY_H
#define GUISCREENMISSIONGALLERY_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <loading/loadingmanager.h>

//===========================================================================
// Forward References
//===========================================================================
class CGuiMenu2D;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenMissionGallery : public CGuiScreen,
                                 public LoadingManager::ProcessRequestsCallback
{
public:
    CGuiScreenMissionGallery( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenMissionGallery();

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
    void SetMenuAlpha( float alpha );
    void OnMenuSelectionMade( int selection );
    void OnStartMission();
#ifdef RAD_WIN32
    void SetVisibilityForAllOtherMenuItems( bool bDisable );
#endif

    int LoadMissionImages();
    void UnloadMissionImages();

    enum eMenuItem
    {
        MISSION_1,
        MISSION_2,
        MISSION_3,
        MISSION_4,
        MISSION_5,
        MISSION_6,
        MISSION_7,
        MISSION_8,

        NUM_MISSIONS_PER_LEVEL
    };

    CGuiMenu2D* m_pMenu;
    Scrooby::Group* m_menuGroup;

    bool m_isMissionImagesLoaded : 1;

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

    Scrooby::Layer* m_missionInfo;
    Scrooby::Text* m_missionNum;
    Scrooby::Text* m_missionTitle;

    int m_selectedMission;

};

#endif // GUISCREENMISSIONGALLERY_H
