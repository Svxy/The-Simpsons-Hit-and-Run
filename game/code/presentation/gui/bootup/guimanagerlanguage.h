//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiManagerLanguage
//
// Description: Interface for the CGuiManagerLanguage class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/07/08      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUIMANAGERLANGUAGE_H
#define GUIMANAGERLANGUAGE_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guimanager.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiManagerLanguage : public CGuiManager
{
public:
    CGuiManagerLanguage( Scrooby::Project* pProject, CGuiEntity* pParent );
    virtual ~CGuiManagerLanguage();

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
    CGuiManagerLanguage( const CGuiManagerLanguage& );
    CGuiManagerLanguage& operator= ( const CGuiManagerLanguage& );

    bool CheckLanguage();

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

};

#endif // GUIMANAGERLANGUAGE_H
