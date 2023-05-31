//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CTeleTypeText
//
// Description: Implementation of the CTeleTypeText class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/19      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/utility/teletypetext.h>

#include <raddebug.hpp> // Foundation
#include <group.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CTeleTypeText::CTeleTypeText
//===========================================================================
// Description: Constructor.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
CTeleTypeText::CTeleTypeText( Scrooby::Text* pText,
                              Scrooby::Group* pTextBox,
                              unsigned int numCharsPerSecond,
                              unsigned int numSecondsToDisplay )
:   m_state( STATE_INACTIVE ),
    m_elapsedTime( 0 ),
    m_teletypePeriod( 1000 / numCharsPerSecond ),
    m_displayDuration( numSecondsToDisplay * 1000 ),
    m_pText( pText ),
    m_pTextBox( pTextBox ),
    m_charBuffer( NULL ),
    m_nextChar( 0 ),
    m_currentCharIndex( 0 )
{
    rAssert( m_pText != NULL );

    // hide by default
    if( m_pTextBox != NULL )
    {
        m_pTextBox->SetVisible( false );
    }
    else
    {
        m_pText->SetVisible( false );
    }

    // wrap text by default
    m_pText->SetTextMode( Scrooby::TEXT_WRAP );
}

//===========================================================================
// CTeleTypeText::~CTeleTypeText
//===========================================================================
// Description: Destructor.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
CTeleTypeText::~CTeleTypeText()
{
}

//===========================================================================
// CTeleTypeText::Update
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CTeleTypeText::Update( unsigned int elapsedTime )
{
    switch( m_state )
    {
        case STATE_TELETYPING:
        {
            if( m_elapsedTime >= m_teletypePeriod )
            {
                this->TypeNextCharacter();

                m_elapsedTime = m_elapsedTime % m_teletypePeriod;
            }

            // update elapsed time
            m_elapsedTime += elapsedTime;

            break;
        }
        case STATE_DISPLAYING:
        {
            // 0 means display forever, until client hides it
            //
            if( m_displayDuration > 0 )
            {
                if( m_elapsedTime >= m_displayDuration )
                {
                    this->HideMessage();
                }

                // update elapsed time
                m_elapsedTime += elapsedTime;
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
// CTeleTypeText::DisplayMessage
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CTeleTypeText::DisplayMessage()
{
//    rAssertMsg( m_state == STATE_INACTIVE, "ERROR: Teletype message in progress!\n" );
    if( m_state != STATE_INACTIVE )
    {
        // ignore request if there's already a message in progress
        return;
    }

    rAssert( m_pText );

    // show scrooby text
    if( m_pTextBox != NULL )
    {
        m_pTextBox->SetVisible( true );
    }
    else
    {
        m_pText->SetVisible( true );
    }

    // get string buffer from text
    m_charBuffer = m_pText->GetStringBuffer();

    rAssert( m_charBuffer );

    // reset current character index
    m_currentCharIndex = 0;

    // save next character and terminate buffer
    m_nextChar = m_charBuffer[ m_currentCharIndex + 1 ];
    m_charBuffer[ m_currentCharIndex + 1 ] = '\0';

    m_state = STATE_TELETYPING;
}

//===========================================================================
// CTeleTypeText::DisplayMessage
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CTeleTypeText::DisplayMessage( int index )
{
    rAssert( m_pText );

    if( index >= 0 && index < m_pText->GetNumOfStrings() )
    {
        m_pText->SetIndex( index );

        this->DisplayMessage();
    }
    else
    {
        rAssertMsg( 0, "WARNING: Invalid index for teletype message!\n" );
    }
}

//===========================================================================
// CTeleTypeText::HideMessage
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CTeleTypeText::HideMessage()
{
    // if stopped during middle of teletyping
    if( m_state == STATE_TELETYPING )
    {
        // restore character in buffer
        m_charBuffer[ m_currentCharIndex + 1 ] = m_nextChar;
    }

    // hide scrooby text
    if( m_pTextBox != NULL )
    {
        m_pTextBox->SetVisible( false );
    }
    else
    {
        m_pText->SetVisible( false );
    }

    // reset elapsed time
    m_elapsedTime = 0;

    m_state = STATE_INACTIVE;
}

void CTeleTypeText::Pause()
{
    if( m_state == STATE_TELETYPING )
    {
        // restore character in buffer
        m_charBuffer[ m_currentCharIndex + 1 ] = m_nextChar;

        // hide scrooby text
        m_pText->SetVisible( false );

        m_state = STATE_PAUSED;
    }
}

void CTeleTypeText::Resume()
{
    if( m_state == STATE_PAUSED )
    {
        // restore character in buffer
        m_charBuffer[ m_currentCharIndex + 1 ] = '\0';

        // show scrooby text
        m_pText->SetVisible( true );

        m_state = STATE_TELETYPING;
    }
}

//===========================================================================
// CTeleTypeText::TypeNextCharacter
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CTeleTypeText::TypeNextCharacter()
{
    // increment current character index
    m_currentCharIndex++;

    // restore character in buffer
    m_charBuffer[ m_currentCharIndex ] = m_nextChar;

    // save next character and terminate buffer
    m_nextChar = m_charBuffer[ m_currentCharIndex + 1 ];
    m_charBuffer[ m_currentCharIndex + 1 ] = '\0';

    // if end of buffer reached
    if( m_nextChar == '\0' )
    {
        m_state = STATE_DISPLAYING;
    }
}

//===========================================================================
// Protected Member Functions
//===========================================================================
