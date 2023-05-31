//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiManagerFrontEnd
//
// Description: Interface for the CGuiManagerFrontEnd class.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2000/09/20		DChau		Created
//                  2002/05/29      TChu        Modified for SRR2
//
//===========================================================================

#ifndef GUIMANAGERFRONTEND_H
#define GUIMANAGERFRONTEND_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guimanager.h>

//===========================================================================
// Forward References
//===========================================================================

const unsigned int FOR_THE_FIRST_TIME = 1;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiManagerFrontEnd : public CGuiManager
{
public:
    CGuiManagerFrontEnd( Scrooby::Project* pProject,
                            CGuiEntity* pParent );

    virtual ~CGuiManagerFrontEnd();

	virtual void Populate();
    virtual void Start( CGuiWindow::eGuiWindowID initialWindow = CGuiWindow::GUI_WINDOW_ID_UNDEFINED );

	virtual void HandleMessage( eGuiMessage message, 
			                    unsigned int param1 = 0,
								unsigned int param2 = 0 );

private:
    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No copying or assignment. Declare but don't define.
    //
    CGuiManagerFrontEnd( const CGuiManagerFrontEnd& );
    CGuiManagerFrontEnd& operator= ( const CGuiManagerFrontEnd& );

    void OnControllerDisconnected( int controllerID );
    void OnControllerConnected( int controllerID );

    void StartIntroMovie();

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

    int m_levelNum;
    int m_disconnectedController;
    bool m_quittingToDemo : 1;
    bool m_controllerPromptShown : 1;
    bool m_quittingToMiniGame : 1;
    bool m_isControllerReconnected : 1;
    bool m_wasFMVInputHandlerEnabled : 1;

#ifdef RAD_WIN32
    bool m_quittingGame : 1;
#endif

};

#endif // GUIMANAGERFRONTEND_H
