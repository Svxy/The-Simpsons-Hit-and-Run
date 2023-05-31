//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMultiHud
//
// Description: 
//              
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2000/11/20		DChau		Created
//                  2002/06/06      TChu        Modified for SRR2
//
//===========================================================================

#ifndef GUISCREENMULTIHUD_H
#define GUISCREENMULTIHUD_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <presentation/gui/utility/numerictext.h>
#include <presentation/gui/utility/slider.h>
#include <presentation/gui/utility/transitions.h>
#include <constants/maxplayers.h>

//===========================================================================
// Forward References
//===========================================================================

class CHudMap;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenMultiHud : public CGuiScreen
{
public:
    CGuiScreenMultiHud( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenMultiHud();

    CGuiScreenMultiHud( Scrooby::Screen* pScreen, CGuiEntity* pParent,
                        eGuiWindowID id, int numActivePlayers );

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CHudMap* GetHudMap( unsigned int playerID ) const;

    void TutorialBitmapShow();
    void TutorialBitmapHide();
    void TutorialBitmapInitOutro();
    void SetTutorialMessage( int index );
    void ShowLetterBox();

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

    void RetrieveElements( Scrooby::Page* pPage );
    void UpdateElements( unsigned int elapsedTime );
    void UpdateTutorialMode( const float deltaT );

    int m_numActivePlayers;

    NumericText<Scrooby::Text> m_speedDigital[ MAX_PLAYERS ];

    static const int MAX_TUTORIAL_MESSAGE_LENGTH = 256;

    ImageSlider m_gasMeter[ MAX_PLAYERS ];
    CHudMap* m_hudMap[ MAX_PLAYERS ];
    Scrooby::Group*  m_tutorialWalkie;
    Scrooby::Sprite* m_tutorialMessage;
    bool m_TutorialBitmapVisible : 1;
    bool m_isStartButtonPressed : 1;
    float m_TutorialBitmapTimeShown;
    GuiSFX::UnderdampedTranslator m_TutorialBitmapTransitionIn;
    GuiSFX::Translator m_TutorialBitmapSteadyState;
    GuiSFX::Translator m_TutorialBitmapTransitionOut;
    GuiSFX::Translator m_TurorialBitmapStayOut;
    Scrooby::Page*     m_LetterboxPage;

};


#endif // GUISCREENMULTIHUD_H
