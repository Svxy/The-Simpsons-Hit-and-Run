//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLetterBox
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/21      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUISCREENLETTERBOX_H
#define GUISCREENLETTERBOX_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>
#include <presentation/gui/ingame/guiscreenhastransitions.h>
#include <presentation/gui/utility/transitions.h>

#include <events/eventlistener.h>

//===========================================================================
// Forward References
//===========================================================================
namespace Scrooby
{
    class Group;
}
//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreenLetterBox : 
    public CGuiScreen,
    public CGuiScreenHasTransitions,
    public EventListener
{
public:
    CGuiScreenLetterBox( Scrooby::Screen* pScreen, CGuiEntity* pParent );
    virtual ~CGuiScreenLetterBox();
    static void  ForceOpen();
	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

    virtual void HandleEvent( EventEnum id, void* pEventData );

    static void  SurpressSkipButton();
    static void  UnSurpressSkipButton();

    static void SuppressAcceptCancelButtons( bool suppress = true );
    void        CheckIfScreenShouldBeBlank();

protected:
    void InitIntro();
	void InitRunning();
	void InitOutro();
    void SetIntroFromClosed();
    void SetIntroFromOpen();
    #ifdef DEBUGWATCH
        virtual const char* GetWatcherName() const;
    #endif


private:
    void OnAccept();
    void OnCancel();
    void OnSkip();

    Scrooby::Page*  m_Page;
    Scrooby::Page*  m_IrisPage;
    Scrooby::Group* m_topBar;
    Scrooby::Group* m_bottomBar;
    Scrooby::Group* m_skipButton;
    Scrooby::Group* m_skipLabel;
    Scrooby::Pure3dObject* m_Iris;

    unsigned int m_elapsedTime;
    static bool  m_enableReopen;
    static bool  m_forceOpen;
    bool         m_OutroPending : 1;
    GuiSFX::Pause        s_OutroPauseBetweenCloseAndIris;
    GuiSFX::Show         s_OutroIrisShow;
    GuiSFX::Hide         s_OutroLetterBoxHide;
    GuiSFX::IrisWipeOpen s_OutroIrisOpen;

    static bool s_suppressAcceptCancelButtons;

};

inline void CGuiScreenLetterBox::SuppressAcceptCancelButtons( bool suppress )
{
    s_suppressAcceptCancelButtons = suppress;
}

#endif // GUISCREENLETTERBOX_H
