//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPlayMovie
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/04      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENPLAYMOVIE_H
#define GUISCREENPLAYMOVIE_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <presentation/presevents/presentationevent.h>
#include <constants/movienames.h>

//===========================================================================
// Forward References
//===========================================================================
class CGuiMenu;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenPlayMovie : public CGuiScreen,
                            public PresentationEvent::PresentationEventCallBack
{
public:
    CGuiScreenPlayMovie( Scrooby::Screen* pScreen, CGuiEntity* pParent,
                         eGuiWindowID windowID = GUI_SCREEN_ID_PLAY_MOVIE );
    virtual ~CGuiScreenPlayMovie();

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    void SetMovieToPlay( const char* movieName, bool IsSkippable = true,
                                                bool KillMusic = false,
                                                bool IsLocalized = true );

    // Implements PresentationEvent::PresentationEventCallBack
    //
    virtual void OnPresentationEventBegin( PresentationEvent* pEvent );
    virtual void OnPresentationEventLoadComplete( PresentationEvent* pEvent );
    virtual void OnPresentationEventEnd( PresentationEvent* pEvent );

	void SetSkippable(bool IsSkippable) {m_IsSkippable = IsSkippable;}
	bool GetSkippable(void) const {return m_IsSkippable;}

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();

private:
    Scrooby::Text* m_demoText;
    unsigned int m_elapsedTime;
	bool m_IsSkippable : 1;
};

#endif // GUISCREENPLAYMOVIE_H
