//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiManagerBackEnd
//
// Description: Interface for the CGuiManagerBackEnd class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/15      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUIMANAGERBACKEND_H
#define GUIMANAGERBACKEND_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guimanager.h>

//===========================================================================
// Forward References
//===========================================================================

const unsigned int IS_LOADING_GAMEPLAY = 1;

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiManagerBackEnd : public CGuiManager
{
    public:

        CGuiManagerBackEnd( Scrooby::Project* pProject,
                            CGuiEntity* pParent );

        virtual ~CGuiManagerBackEnd();

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
        CGuiManagerBackEnd( const CGuiManagerBackEnd& );
        CGuiManagerBackEnd& operator= ( const CGuiManagerBackEnd& );

        void GotoLoadingScreen( unsigned int param1 );

        //---------------------------------------------------------------------
        // Private Data
        //---------------------------------------------------------------------

        bool m_isQuittingDemo;
        bool m_isBackendPreRun;

};

#endif // GUIMANAGERBACKEND_H
