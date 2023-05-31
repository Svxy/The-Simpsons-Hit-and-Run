//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenCardGallery
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

#ifndef GUISCREENCARDGALLERY_H
#define GUISCREENCARDGALLERY_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <cards/cardgallery.h>
#include <cards/card.h>

//===========================================================================
// Forward References
//===========================================================================

class CGuiMenu2D;
class ScrollingText;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenCardGallery : public CGuiScreen
{
public:
    CGuiScreenCardGallery( Scrooby::Screen* pScreen, CGuiEntity* pParent,
                           eGuiWindowID windowID = GUI_SCREEN_ID_CARD_GALLERY );
    virtual ~CGuiScreenCardGallery();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual CGuiMenu* HasMenu() { return m_pMenu; }

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

    void UpdateCards( unsigned int currentLevel );

    enum eScreenState
    {
        STATE_BROWSING_CARDS,
        STATE_GOTO_VIEW_CARD,
        STATE_VIEWING_CARD,
        STATE_BACK_VIEW_CARD,

        NUM_SCREEN_STATES
    };

    eScreenState m_cardGalleryState;

    float m_cardScaleSmall;
    float m_cardScaleLarge;

    CGuiMenu2D* m_pMenu;

private:
//    void MoveCursor( unsigned int from, unsigned int to );
    void UpdateCardTransition( unsigned int elapsedTime, bool toViewCard );
    void UpdateViewCard( unsigned int elapsedTime );
    void SetCurrentViewCard( unsigned int cardID );

    Scrooby::Sprite* m_viewCard;
    int m_viewCardDistX;
    int m_viewCardDistY;
    rmt::Vector m_cardVelocity;

    Scrooby::Layer* m_cardBrowseLayer;
    Scrooby::Layer* m_cardViewLayer;
    Scrooby::Layer* m_cardSFXLayer;

    unsigned int m_elapsedTime;

    // for viewing card
    //
    Card* m_currentCard;

    Scrooby::Text* m_cardTitle;
    Scrooby::Text* m_cardEpisode;
    Scrooby::Text* m_cardDescription;

    Scrooby::Sprite* m_quoteIcon[ MAX_NUM_QUOTES ];
    ScrollingText* m_quote[ MAX_NUM_QUOTES ];
    int m_currentQuote;

};

#endif // GUISCREENCARDGALLERY_H
