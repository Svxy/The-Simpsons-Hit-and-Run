//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CTeleTypeText
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/19      TChu        Created for SRR2
//
//===========================================================================

#ifndef TELETYPETEXT_H
#define TELETYPETEXT_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <p3d/p3dtypes.hpp>

//===========================================================================
// Forward References
//===========================================================================

namespace Scrooby
{
    class Text;
    class Group;
}

const unsigned int DEFAULT_TELETYPING_RATE = 8; // in characters/sec

//===========================================================================
// Interface Definitions
//===========================================================================
class CTeleTypeText
{
    public:

        CTeleTypeText( Scrooby::Text* pText,
                       Scrooby::Group* pTextBox = NULL,
                       unsigned int numCharsPerSecond = DEFAULT_TELETYPING_RATE,
                       unsigned int numSecondsToDisplay = 0 );
        virtual ~CTeleTypeText();

        // update teletype text
        void Update( unsigned int elapsedTime );

        // display message and start teletyping
        void DisplayMessage();
        void DisplayMessage( int index );

        // hide message and stop teletyping if still in progress
        void HideMessage();

        // pause/resume teletyping
        void Pause();
        void Resume();
        
        // accessor to scrooby text object
        Scrooby::Text* GetText() const
        {
            return m_pText;
        }

	protected:
        
        //---------------------------------------------------------------------
        // Protected Functions
        //---------------------------------------------------------------------

        //---------------------------------------------------------------------
        // Protected Data
        //---------------------------------------------------------------------

	private:

        //---------------------------------------------------------------------
        // Private Functions
        //---------------------------------------------------------------------

        // No copying or asignment. Declare but don't define.
        //
        CTeleTypeText( const CTeleTypeText& );
        CTeleTypeText& operator= ( const CTeleTypeText& );

        void TypeNextCharacter();

        //---------------------------------------------------------------------
        // Private Data
        //---------------------------------------------------------------------

        enum eTeletypeState
        {
            STATE_TELETYPING,
            STATE_DISPLAYING,
            STATE_PAUSED,

            STATE_INACTIVE
        };

        eTeletypeState m_state;

        unsigned int m_elapsedTime;
        unsigned int m_teletypePeriod;
        unsigned int m_displayDuration;

        Scrooby::Text* m_pText;
        Scrooby::Group* m_pTextBox;
        P3D_UNICODE* m_charBuffer;
        P3D_UNICODE m_nextChar;
        int m_currentCharIndex;

};

#endif // TELETYPETEXT_H
