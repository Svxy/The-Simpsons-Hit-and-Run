//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMissionSuccess
//
// Description: 
//              
//
// Authors:     Ian Gipson
//
// Revisions		Date			Author	    Revision
//                  2003/04/07      igipson     Created for SRR2
//
//===========================================================================

#ifndef GUISCREENMISSIONBASE_H
#define GUISCREENMISSIONBASE_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <events/eventlistener.h>
#include <presentation/gui/guiscreen.h>
#include <presentation/gui/ingame/guiscreenhastransitions.h>

//===========================================================================
// Forward References
//===========================================================================
class tSprite;
namespace Scrooby
{
    class BoundedDrawable;
    class Drawable;
}


//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenMissionBase:
    public CGuiScreen,
    public CGuiScreenHasTransitions
{
public:
    CGuiScreenMissionBase( Scrooby::Screen* pScreen, CGuiEntity* pParent, eGuiWindowID id );
    ~CGuiScreenMissionBase();
	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );
    virtual void InitIntro();
	virtual void InitRunning();
	virtual void InitOutro();
    static  void ClearBitmap();
    static  void GetBitmapName( char* buffer );
    static  bool IsCurrentBitmap( const char* name );
    static  void ReplaceBitmap();
    static  void SetBitmapName( const char* name );

protected:
    Scrooby::Group*    GetAbortBitmap();
    Scrooby::Drawable* GetFlag();
    Scrooby::Group*    GetLoadCompletedGroup();
    Scrooby::Text*     GetMissionInfoText();
    Scrooby::BoundedDrawable* GetMissionStartBitmap();
    Scrooby::Page*     GetPage();
    Scrooby::Text*     GetTitleText();
    void InitIntroWagerMission();
    virtual void OutroDone();
    static void RemoveAnimatedBitmap();
    void StartIrisWipeClose();
    void StartIrisWipeOpen();
    void UpdateAnimatedBitmap( const float deltaT );
    void UnloadBitmap();
    void UpdateGamblingInfo();
    void SetPlayAnimatedCamera( bool play );
    const char* GetWatcherName() const;

private:
    Scrooby::Text*     m_missionTitle;
    Scrooby::Page*     m_Page;
    Scrooby::Page*     m_LetterboxPage;
    Scrooby::Sprite*   m_missionStartBitmap;
    Scrooby::Sprite*   m_background;
    Scrooby::Sprite*   m_Flag;
    Scrooby::Drawable* m_ClipLeftGroup;
    Scrooby::Drawable* m_ClipRightGroup;
    Scrooby::Drawable* m_ClipLeft;
    Scrooby::Drawable* m_ClipRight;
    Scrooby::Sprite*   m_ClipLeftArm;
    Scrooby::Sprite*   m_ClipRightArm;
    Scrooby::Drawable* m_Foreground;
    Scrooby::Group*    m_darkenPolys;
    Scrooby::Group*    m_loadCompleted;
    Scrooby::Group*    m_textOverlays;
    Scrooby::Group*    m_darkenTop;
    Scrooby::Group*    m_darkenBottom;
    Scrooby::Polygon*  m_backgroundPoly;
    Scrooby::Layer*    m_LetterboxLayer; 
    Scrooby::Layer*    m_BgLayer; 
    Scrooby::Group*    m_FgLayer;
    Scrooby::Pure3dObject* m_Iris;
    tMultiController*      m_MultiController;
    float m_irisWipeNumFrames;
    Scrooby::Text*    m_Line0;
    static tSprite*   s_AnimatedBitmapSprite;
    static char       s_AnimatedBitmapName[ 256 ];
    static char       s_AnimatedBitmapShortName[ 32 ];
    static bool       s_BitmapLoadPending;
    static tSprite* sp_PattyAndSelmaScreenPNG;
    bool              m_PlayAnimatedCamera;

    enum eGamblingVehicleOdds
    {
        VEHICLE_ODDS_EASY,
        VEHICLE_ODDS_MEDIUM,
        VEHICLE_ODDS_HARD,

        NUM_VEHICLE_ODDS
    };

    Scrooby::Group* m_gamblingInfo;
    Scrooby::Text* m_gamblingEntryFee;
    Scrooby::Text* m_gamblingTimeToBeat;
    Scrooby::Text* m_gamblingBestTime;
    Scrooby::Text* m_gamblingVehicleOdds;
    Scrooby::Text* m_gamblingPayout;

    bool m_ReadyToExitScreen : 1;

};

#endif // GUISCREENMISSIONBASE_H
