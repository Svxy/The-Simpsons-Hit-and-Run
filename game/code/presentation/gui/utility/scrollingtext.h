//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ScrollingText
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/11/21      TChu        Created for SRR2
//
//===========================================================================

#ifndef SCROLLINGTEXT_H
#define SCROLLINGTEXT_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <strings/unicodestring.h>


//===========================================================================
// Forward References
//===========================================================================

namespace Scrooby
{
    class Text;
}

//===========================================================================
// Interface Definitions
//===========================================================================
class ScrollingText
{
public:
    ScrollingText( Scrooby::Text* pText );
    virtual ~ScrollingText();

    void RestoreText();
    void SetTextIndex( int index );

    void Start();
    void Stop();
    void Pause();
    void Resume();

    void Update( unsigned int elapsedTime );

    enum eScrollState
    {
        STATE_IDLE,
        STATE_SCROLLING,
        STATE_PAUSED,

        NUM_SCROLL_STATES
    };

    eScrollState GetCurrentState() const { return m_state; }

    void SetSpeed( int numPixelsPerSecond ) { m_numPixelsPerSecond = numPixelsPerSecond; }

    void SetCyclic( bool isCyclic ) { m_isCyclic = isCyclic; }
    bool IsCyclic() const { return m_isCyclic; }

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
    ScrollingText( const ScrollingText& );
    ScrollingText& operator= ( const ScrollingText& );

    void ClipText( int x, int y );

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

    Scrooby::Text* m_pText;
    UnicodeChar* m_originalString;

    int m_x0;
    int m_y0;
    int m_x1;
    int m_y1;
    float m_currentPos;

    eScrollState m_state;

    int m_numPixelsPerSecond;
    bool m_isCyclic;

};

#endif // SCROLLINGTEXT_H
