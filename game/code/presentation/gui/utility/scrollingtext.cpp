//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ScrollingText
//
// Description: Implementation of the ScrollingText class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/11/21      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <p3d/font.hpp>
#include <presentation/gui/utility/scrollingtext.h>

#include <raddebug.hpp> // Foundation
#include <app.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// ScrollingText::ScrollingText
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
ScrollingText::ScrollingText( Scrooby::Text* pText )
:   m_pText( pText ),
    m_originalString( NULL ),
    m_x0( 0 ),
    m_y0( 0 ),
    m_x1( 0 ),
    m_y1( 0 ),
    m_currentPos( 0.0f ),
    m_state( STATE_IDLE ),
    m_numPixelsPerSecond( 90 ),
    m_isCyclic( false )
{
    rAssert( m_pText != NULL );

    // set text mode to clip on right boundary
    //
    m_pText->SetTextMode( Scrooby::TEXT_CLIP );

    // get scroll box boundary
    //
    m_pText->GetBoundingBox( m_x0, m_y0, m_x1, m_y1 );

    // hide text until scrolling is started
    //
    m_pText->SetVisible( false );

    // save reference to original string buffer
    //
    m_originalString = m_pText->GetStringBuffer();

/*
    // save copy of original string
    //
    P3D_UNICODE* originalString = static_cast<P3D_UNICODE*>( m_pText->GetStringBuffer() );
    rAssert( originalString != NULL );
    p3d::UnicodeStrCpy( originalString,
                        m_originalString,
                        p3d::UnicodeStrLen( originalString ) + 1 );
*/
}

//===========================================================================
// ScrollingText::~ScrollingText
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
ScrollingText::~ScrollingText()
{
}

void
ScrollingText::RestoreText()
{
    // restore position and size
    //
    m_pText->SetPosition( m_x0, m_y0 );
    m_pText->SetBoundingBoxSize( m_x1 - m_x0, m_y1 - m_y0 );

    // restore original string buffer
    //
    m_pText->SetStringBuffer( m_originalString );

    // restore visibility
    //
    m_pText->SetVisible( true );
}

void
ScrollingText::SetTextIndex( int index )
{
    rAssert( m_state == STATE_IDLE );

    m_pText->SetIndex( index );

    // update reference to original string buffer
    //
    m_originalString = m_pText->GetStringBuffer();
}

void
ScrollingText::Start()
{
    m_state = STATE_SCROLLING;

    m_currentPos = (float)m_x1;
    this->ClipText( (int)m_currentPos, m_y0 );

    // show text
    //
    m_pText->SetVisible( true );
}

void
ScrollingText::Stop()
{
    m_state = STATE_IDLE;

    m_pText->SetPosition( m_x0, m_y0 );

    // restore original string buffer
    //
    m_pText->SetStringBuffer( m_originalString );

    // hide text
    //
    m_pText->SetVisible( false );

/*
    P3D_UNICODE* originalString = static_cast<P3D_UNICODE*>( m_pText->GetStringBuffer() );
    rAssert( originalString != NULL );
    p3d::UnicodeStrCpy( m_originalString,
                        originalString,
                        p3d::UnicodeStrLen( m_originalString ) + 1 );
*/
}

void
ScrollingText::Update( unsigned int elapsedTime )
{
    if( m_state == STATE_SCROLLING )
    {
        // advance text towards the left
        //
        m_currentPos -= (m_numPixelsPerSecond * elapsedTime) / 1000.0f;

        // apply text clipping for current position
        //
        this->ClipText( (int)m_currentPos, m_y0 );
    }
}

//===========================================================================
// Private Member Functions
//===========================================================================

void
ScrollingText::ClipText( int x, int y )
{
    if( x >= m_x1 )         // *** origin beyond right boundary
    {
        m_pText->SetPosition( x, y );
        m_pText->SetBoundingBoxSize( 0, m_y1 - y );
    }
    else if( x >= m_x0 )    // *** origin within both boundaries
    {
        m_pText->SetPosition( x, y );
        m_pText->SetBoundingBoxSize( m_x1 - x, m_y1 - y );

#ifdef RAD_WIN32
        // TC: for PC sku, the source fonts are actually twice as big (for higher resolution display),
        //     so we need to stretch the bounding box accordingly for proper text clipping
        //
        m_pText->StretchBoundingBox( 2.0f, 1.0f );
#endif
    }
    else                    // *** origin beyond left boundary
    {
        m_pText->SetBoundingBoxSize( m_x1 - m_x0, m_y1 - y );

#ifdef RAD_WIN32
        // TC: for PC sku, the source fonts are actually twice as big (for higher resolution display),
        //     so we need to stretch the bounding box accordingly for proper text clipping
        //
        m_pText->StretchBoundingBox( 2.0f, 1.0f );
#endif

        tFont* textFont = m_pText->GetFont();
        if( textFont != NULL )
        {
            bool doneClipping = false;

            for( UnicodeChar* currentString = m_pText->GetStringBuffer();
                !doneClipping;
                currentString++ )
            {
                UnicodeChar tempChar = currentString[ 0 ];
                currentString[ 0 ] = '\0';

                float clippedTextWidth = textFont->GetTextWidth( static_cast<P3D_UNICODE*>( m_originalString ) ) *
                                        Scrooby::App::GetInstance()->GetScreenWidth() /
                                        Scrooby::App::GetInstance()->GetScreenHeight();

#ifdef RAD_WIN32
                // TC: for PC sku, the source fonts are actually twice as big (for higher resolution display),
                //     so the width of the current text string is only half as wide
                //
                clippedTextWidth /= 2.0f;
#endif

                currentString[ 0 ] = tempChar;

                int amountClipped = static_cast<int>( clippedTextWidth ) - (m_x0 - x);
                if( amountClipped >= 0 )
                {
                    m_pText->SetPosition( m_x0 + amountClipped, y );
                    m_pText->SetStringBuffer( currentString );
                    doneClipping = true;
                }

                if( currentString[ 0 ] == '\0' )
                {
                    // done scrolling
                    //
                    this->Stop();

                    if( m_isCyclic )
                    {
                        // start over again
                        //
                        this->Start();
                    }

                    // make sure we break out of the loop; otherwise, there's
                    // a small chance that 'amountClipped' may be slightly less
                    // than 0 and so we might not be done clipping
                    //
                    break;
                }
            }
        }
        else
        {
            rWarningMsg( false, "No font for clipping text!" );

            this->Stop();
        }
    }
}

