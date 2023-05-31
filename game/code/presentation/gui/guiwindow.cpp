//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiWindow
//
// Description: Implementation of the CGuiWindow class.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions        Date            Author      Revision
//                  2000/09/21      DChau       Created
//                  2002/05/29      TChu        Modified for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <memory/classsizetracker.h>
#include <presentation/gui/guiwindow.h>
#include <presentation/gui/guimanager.h>
#include <raddebug.hpp> // Foundation

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiWindow::CGuiWindow
//===========================================================================
// Description: Constructor.
//
// Constraints: None.
//
// Parameters:  None.
//
// Return:      N/A.
//
//===========================================================================
CGuiWindow::CGuiWindow
(
    eGuiWindowID id,    
    CGuiEntity* pParent
)
:
    CGuiEntity( pParent ),
    m_state( GUI_WINDOW_STATE_UNITIALIZED ),
    m_ID( id ),
    m_numTransitionsPending( 0 ),
    m_firstTimeEntered( true )
{
    CLASSTRACKER_CREATE( CGuiWindow );
}


//===========================================================================
// CGuiWindow::~CGuiWindow
//===========================================================================
// Description: Destructor.
//
// Constraints: None.
//
// Parameters:  None.
//
// Return:      N/A.
//
//===========================================================================
CGuiWindow::~CGuiWindow()
{
    CLASSTRACKER_CREATE( CGuiWindow );
}


//===========================================================================
// CGuiWindow::HandleMessage
//===========================================================================
// Description: 
//
// Constraints: None.
//
// Parameters:  
//
// Return:      N/A.
//
//===========================================================================
void CGuiWindow::HandleMessage
(
    eGuiMessage message, 
    unsigned int param1,
    unsigned int param2
)
{
    switch( message )
    {
        case GUI_MSG_WINDOW_ENTER:
        {
/*
            // Ignore multiple enter requests.
            //
            if( GUI_WINDOW_STATE_UNITIALIZED != m_state )
            {
                break;
            }
*/
            m_state = GUI_WINDOW_STATE_INTRO;
            this->InitIntro();

            break;
        }

        case GUI_MSG_WINDOW_EXIT:
        {
/*
            if( GUI_WINDOW_STATE_OUTRO == m_state ||
                GUI_WINDOW_STATE_UNITIALIZED == m_state )
            {
                // Ignore multiple exit requests.
                //
                break;
            }
*/
            m_state = GUI_WINDOW_STATE_OUTRO;
            this->InitOutro();

            break;
        }

        case GUI_MSG_WINDOW_PAUSE:
        {
            m_prevState = m_state;
            m_state = GUI_WINDOW_STATE_PAUSED;
            break;
        }

        case GUI_MSG_WINDOW_RESUME:
        {
            m_state = m_prevState;
            break;
        }

        case GUI_MSG_UPDATE:
        {
            if( GUI_WINDOW_STATE_UNITIALIZED == m_state 
                || GUI_WINDOW_STATE_PAUSED == m_state )
            {
                return;
            }

            switch( m_state )
            {
                case GUI_WINDOW_STATE_INTRO:
                {
                    // The intro transition is complete if there are no more
                    // non-persistent sequencers running.
                    //
                    if( m_numTransitionsPending == 0 )
                    {
                        m_state = GUI_WINDOW_STATE_RUNNING;
                        this->InitRunning();
                    }

                    break;
                }

                case GUI_WINDOW_STATE_OUTRO:
                {
                    // The outro transition is complete if there are no more
                    // non-persistent sequencers running.
                    //
                    if( m_numTransitionsPending == 0 )
                    {
                        // wait one more frame for last transition update to get rendered
                        //
                        m_numTransitionsPending--;
                        CleanUp();
                    }
                    else if( m_numTransitionsPending < 0 )
                    {
                        m_state = GUI_WINDOW_STATE_UNITIALIZED;
                        m_firstTimeEntered = false;

                        m_pParent->HandleMessage( GUI_MSG_WINDOW_FINISHED );
                    }

                    break;
                }

                default:
                {
                    break;
                }
            }
           
            break;
        }

        default:
        {
            break;
        }
    }
}

//===========================================================================
// CGuiWindow::CleanUp
//===========================================================================
// Description: Called after the last outro transition is done
//
// Constraints: None.
//
// Parameters:  
//
// Return:      N/A.
//
//===========================================================================
void CGuiWindow::CleanUp()
{
    //do nothing
}

//===========================================================================
// CGuiWindow::ForceClearTransitions
//===========================================================================
// Description: Forces all transitions to think they are done
//
// Constraints: None.
//
// Parameters:  
//
// Return:      N/A.
//
//===========================================================================
void CGuiWindow::ForceClearTransitions()
{
    m_numTransitionsPending = 0;
}