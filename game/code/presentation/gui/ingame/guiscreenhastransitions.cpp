//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLetterBox
//
// Description: Implementation of the CGuiScreenLetterBox class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/21      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreenhastransitions.h>
#include <presentation/gui/utility/transitions.h>
#include <drawable.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenHasTransitions::CGuiScreenHasTransitions
//===========================================================================
// Description: Constructor
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
CGuiScreenHasTransitions::CGuiScreenHasTransitions():
    mDoneAddingTransitions( false )
{
    // zero out the array of transitions (use a memset instead)?
    int i;
    for( i = 0; i < MAX_TRANSITIONS; ++i )
    {
        m_Transitions[ i ] = NULL;
    }
}

//===========================================================================
// CGuiScreenHasTransitions::AddTransition
//===========================================================================
// Description: Adds the transition to the array of transitions
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenHasTransitions::AddTransition( GuiSFX::Transition* transition )
{
    int i;
    for( i = 0; i < MAX_TRANSITIONS; ++i )
    {
        if( m_Transitions[ i ] == NULL )
        {
            m_Transitions[ i ] = transition;
            return;
        }
    }
    rAssertMsg( false, "Need to bump up the size of the array of transitions" );
}

//===========================================================================
// CGuiScreenMissionLoad::AddTransition
//===========================================================================
// Description: Adds the transition to the array of transitions
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenHasTransitions::AddTransition( GuiSFX::Transition& transition )
{
    int i;
    for( i = 0; i < MAX_TRANSITIONS; ++i )
    {
        if( m_Transitions[ i ] == NULL )
        {
            m_Transitions[ i ] = &transition;
            return;
        }
    }
    rAssertMsg( false, "Need to bump up the size of the array of transitions" );
}

//===========================================================================
// CGuiScreenMissionLoad::DoneAddingTransitions
//===========================================================================
// Description: tells the class that we're done adding transitions to it.  Used
//              for debugging the size of the array
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenHasTransitions::DoneAddingTransitions()
{
    mDoneAddingTransitions = true;
}

//===========================================================================
// CGuiScreenHasTransitions::ResetMovableObjects
//===========================================================================
// Description: resets transformations for all movable objects
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenHasTransitions::ResetMovableObjects()
{
    int i;
    for( i = 0; i < MAX_TRANSITIONS; ++i )
    {
        GuiSFX::Transition* t = m_Transitions[ i ];
        if( t != NULL )
        {
            bool active = t->IsActive();
            if( active )
            {
                bool movable = t->MovesDrawable();
                if( movable )
                {
                    Scrooby::Drawable* drawable = t->GetDrawable();
                    if( drawable != NULL )
                    {
                        drawable->ResetTransformation();
                    }
                }
            }
        }
    }
}

//===========================================================================
// CGuiScreenHasTransitions::ResetTransitions
//===========================================================================
// Description: resets all of the transitions on the screen
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenHasTransitions::ResetTransitions()
{
    rAssert( mDoneAddingTransitions );
    int i;
    for( i = 0; i < MAX_TRANSITIONS; ++i )
    {
        if( m_Transitions[ i ] == NULL )
        {
            return;
        }
        m_Transitions[ i ]->Reset();
    }
}

//===========================================================================
// CGuiScreenMissionLoad::UpdateTransitions
//===========================================================================
// Description: updates all the transitions
//
// Constraints:	None.
//
// Parameters:	deltaT - how much time has elapsed
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenHasTransitions::UpdateTransitions( const float deltaT )
{
    rAssert( mDoneAddingTransitions );
    int i;
    for( i = 0; i < MAX_TRANSITIONS; ++i )
    {
        if( m_Transitions[ i ] == NULL )
        {
            return;
        }
        if( m_Transitions[ i ]->IsActive() )
        {
            m_Transitions[ i ]->Update( deltaT );
        }
    }
}