//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenHasTransitions
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/19      Ian Gipson  Created for SRR2
//
//===========================================================================

#ifndef GUISCREENHASTRANSITIONS_H
#define GUISCREENHASTRANSITIONS_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiscreen.h>

//===========================================================================
// Forward References
//===========================================================================
#define MAX_TRANSITIONS 64

namespace GuiSFX
{
    class Transition;
}

//===========================================================================
// Interface Definitions
//===========================================================================

class CGuiScreenHasTransitions
{
public:
    CGuiScreenHasTransitions();
protected:
    void AddTransition( GuiSFX::Transition* transition );
    void AddTransition( GuiSFX::Transition& transition );
    void DoneAddingTransitions();
    void ResetMovableObjects();
    void ResetTransitions();
    void UpdateTransitions( const float deltaT );
private:
    GuiSFX::Transition*  m_Transitions[ MAX_TRANSITIONS ];
    bool mDoneAddingTransitions;
};

#endif // GUISCREENHASTRANSITIONS_H
