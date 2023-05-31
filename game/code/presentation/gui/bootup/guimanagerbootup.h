//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiManagerBootUp
//
// Description: Interface for the CGuiManagerBootUp class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/15      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUIMANAGERBOOTUP_H
#define GUIMANAGERBOOTUP_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guimanager.h>
#include <memory\srrmemory.h> // Needed for my STL allocations to go on the right heap

#include <queue>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiManagerBootUp : public CGuiManager
{
public:
    CGuiManagerBootUp( Scrooby::Project* pProject, CGuiEntity* pParent );
    virtual ~CGuiManagerBootUp();

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
    CGuiManagerBootUp( const CGuiManagerBootUp& );
    CGuiManagerBootUp& operator= ( const CGuiManagerBootUp& );

    CGuiWindow::eGuiWindowID PopNextScreenInQueue();

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------
    typedef std::deque< CGuiWindow::eGuiWindowID, s2alloc<CGuiWindow::eGuiWindowID> > WindowIDVector;
    std::queue<CGuiWindow::eGuiWindowID, WindowIDVector> m_bootupScreenQueue;

};

#endif // GUIMANAGERBOOTUP_H
