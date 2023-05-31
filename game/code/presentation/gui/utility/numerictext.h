//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   NumericText
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/07      TChu        Created for SRR2
//
//===========================================================================

#ifndef NUMERICTEXT_H
#define NUMERICTEXT_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <raddebug.hpp>     // Foundation
#include <group.h>
#include <text.h>

//===========================================================================
// Forward References
//===========================================================================
namespace Scrooby
{
    class Group;
    class Page;
    class Text;
}

//===========================================================================
// Interface Definitions
//===========================================================================
template <class Digit>
struct NumericText
{
    enum eDigits
    {
        DIGIT_ONES,
        DIGIT_TENS,
        DIGIT_HUNDREDS,

        MAX_NUM_DIGITS
    };
    
    enum eAlignment
    {
        ALIGN_RIGHT,
        ALIGN_LEFT,

        NUM_ALIGNMENTS
    };

    Digit* m_digits[ MAX_NUM_DIGITS ];
    Scrooby::Group* m_group;
    int m_numDigits;
    bool m_showLeadingZeros;
    eAlignment m_alignment;

    NumericText()
    :   m_group( NULL ),
        m_numDigits( 0 ),
        m_showLeadingZeros( false ),
        m_alignment( ALIGN_RIGHT )
    {
        memset( m_digits, 0, sizeof( m_digits ) );
    }

    void SetScroobyText( Scrooby::Group* pGroup, const char* name )
    {
        m_group = pGroup;
        m_numDigits = 0;

        char objectName[ 32 ];
        for( int i = 0; i < MAX_NUM_DIGITS; i++ )
        {
            sprintf( objectName, "%s_%d", name, i );

            Digit* pDrawable = dynamic_cast<Digit*>( pGroup->GetText( objectName ) );
            if( pDrawable == NULL )
            {
                pDrawable = dynamic_cast<Digit*>( pGroup->GetSprite( objectName ) );
            }

            if( pDrawable != NULL )
            {
                m_digits[ i ] = pDrawable;

                // increment number of digits
                //
                m_numDigits++;
            }
            else
            {
                // break, assuming no more digits to follow
                //
                break;
            }
        }

        // set value to zero by default
        //
        this->SetValue( 0 );
    }

    void SetValue( unsigned int value, unsigned int offset = 0 )
    {
        for( int i = 0; i < m_numDigits; i++ )
        {
            // set current digit value
            rAssert( m_digits[ i ] );

            m_digits[ i ]->SetIndex( value % 10 + offset );

            // show leading zeros, if enabled
            if( m_showLeadingZeros )
            {
                m_digits[ i ]->SetVisible( true );
            }
            else
            {
                m_digits[ i ]->SetVisible( value > 0 || i == 0 );
            }

            // shift value one digit to the right
            value = value / 10;
        }

        if( m_alignment == ALIGN_LEFT )
        {
            // align numeric text to the left (TC: Can this be improved??)
            for( int j = m_numDigits - 1; j >= 0; j-- )
            {
                if( m_digits[ j ]->IsVisible() )
                {
                    // found leading digit = j, now shift digits to the left
                    //
                    for( int k = m_numDigits - 1; k >= 0; k-- )
                    {
                        if( j >= 0 )
                        {
                            m_digits[ k ]->SetIndex( m_digits[ j ]->GetIndex() );
                            m_digits[ k ]->SetVisible( true );

                            j--;
                        }
                        else
                        {
                            m_digits[ k ]->SetVisible( false );
                        }
                    }

                    // done, break out of loop
                    break;
                }
            }
        }
    }

    void SetVisible( bool isVisible )
    {
        if( m_group != NULL )
        {
            m_group->SetVisible( isVisible );
        }
        else
        {
            for( int i = 0; i < MAX_NUM_DIGITS; i++ )
            {
                if( m_digits[ i ] != NULL )
                {
                    m_digits[ i ]->SetVisible( isVisible );
                }
            }
        }
    }

    void SetColour( tColour colour )
    {
        for( int i = 0; i < MAX_NUM_DIGITS; i++ )
        {
            if( m_digits[ i ] != NULL )
            {
                m_digits[ i ]->SetColour( colour );
            }
        }
    }

};

#endif // NUMERICTEXT_H
