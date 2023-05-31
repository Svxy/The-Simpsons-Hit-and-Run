//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Slider
//
// Description: Implementation of the Slider class.
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
#include <presentation/gui/utility/slider.h>

#include <raddebug.hpp> // Foundation
#include <polygon.h>
#include <sprite.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

Slider::Slider( eSliderType type )
:   m_type( type ),
    m_pPolygon( NULL ),
    m_value( 1 ),
    m_pImage( NULL ),
    x0( 19820602 ), // some arbitrarily big integer
    y0( 19780627 ), // some arbitrarily big integer
    x1( 0 ),
    y1( 0 )
{
}

void
Slider::SetScroobyPolygon( Scrooby::Polygon* pPolygon, Scrooby::Sprite* pImage )
{
    rAssert( pPolygon );
    m_pPolygon = pPolygon;
    m_pImage = pImage;

    for( int i = 0; i < m_pPolygon->GetNumOfVertexes(); i++ )
    {
        int x = 0;
        int y = 0;
        m_pPolygon->GetVertexLocation( i, x, y );

        if( x < x0 ) x0 = x;
        if( y < y0 ) y0 = y;
        if( x > x1 ) x1 = x;
        if( y > y1 ) y1 = y;
    }

    m_pPolygon->SetVertexLocation( VERTEX_BOTTOM_LEFT, x0, y0 );
    m_pPolygon->SetVertexLocation( VERTEX_TOP_LEFT, x0, y1 );
    m_pPolygon->SetVertexLocation( VERTEX_TOP_RIGHT, x1, y1 );
    m_pPolygon->SetVertexLocation( VERTEX_BOTTOM_RIGHT, x1, y0 );
}

void
Slider::SetValue( float value )
{
    rAssert( value >= 0.0f && value <= 1.0f );
    m_value = value;

    rAssert( m_pPolygon );

    int x = x0;
    int y = y0;

    switch( m_type )
    {
        case HORIZONTAL_SLIDER_LEFT:
        {
            x = x0 + static_cast<int>( (x1 - x0) * value );
            m_pPolygon->SetVertexLocation( VERTEX_BOTTOM_RIGHT, x, y0 );
            m_pPolygon->SetVertexLocation( VERTEX_TOP_RIGHT, x, y1 );

            if( m_pImage != NULL )
            {
                m_pImage->ResetTransformation();
                m_pImage->Translate( x - x1, 0 );
            }

            break;
        }
        case HORIZONTAL_SLIDER_RIGHT:
        {
            x = x1 + static_cast<int>( (x0 - x1) * value );
            m_pPolygon->SetVertexLocation( VERTEX_BOTTOM_LEFT, x, y0 );
            m_pPolygon->SetVertexLocation( VERTEX_TOP_LEFT, x, y1 );

            if( m_pImage != NULL )
            {
                m_pImage->ResetTransformation();
                m_pImage->Translate( x - x0, 0 );
            }

            break;
        }
        case VERTICAL_SLIDER_BOTTOM:
        {
            y = y0 + static_cast<int>( (y1 - y0) * value );
            m_pPolygon->SetVertexLocation( VERTEX_TOP_LEFT, x0, y );
            m_pPolygon->SetVertexLocation( VERTEX_TOP_RIGHT, x1, y );

            if( m_pImage != NULL )
            {
                m_pImage->ResetTransformation();
                m_pImage->Translate( 0, y - y1 );
            }

            break;
        }
        case VERTICAL_SLIDER_TOP:
        {
            y = y1 + static_cast<int>( (y0 - y1) * value );
            m_pPolygon->SetVertexLocation( VERTEX_BOTTOM_LEFT, x0, y );
            m_pPolygon->SetVertexLocation( VERTEX_BOTTOM_RIGHT, x1, y );

            if( m_pImage != NULL )
            {
                m_pImage->ResetTransformation();
                m_pImage->Translate( 0, y - y0 );
            }

            break;
        }
        default:
        {
            rAssertMsg( 0, "ERROR: *** Invalid slider type!" );

            break;
        }
    }
}

//===========================================================================
// Public Member Functions (for ImageSlider)
//===========================================================================

ImageSlider::ImageSlider( eSliderType type )
:   Slider( type )
{
}

void
ImageSlider::SetScroobyImage( Scrooby::Sprite* pImage )
{
    rAssert( pImage );
    m_pImage = pImage;
}

void
ImageSlider::SetValue( float value, bool resetTransformation )
{
    rAssert( value >= 0.0f && value <= 1.0f );
    m_value = value;

    rAssert( m_pImage );

    if( resetTransformation )
    {
        m_pImage->ResetTransformation();
    }

    int posX = 0;
    int posY = 0;
    m_pImage->GetOriginPosition( posX, posY );

    int width = 0;
    int height = 0;
    m_pImage->GetBoundingBoxSize( width, height );

    switch( m_type )
    {
        case HORIZONTAL_SLIDER_LEFT:
        {
            m_pImage->Translate( -posX, 0 );
            m_pImage->Scale( value, 1.0f, 1.0f );
            m_pImage->Translate( posX, 0 );

            break;
        }
        case HORIZONTAL_SLIDER_RIGHT:
        {
            m_pImage->Translate( -posX, 0 );
            m_pImage->Scale( value, 1.0f, 1.0f );
            m_pImage->Translate( posX, 0 );

            // right align image
            //
            m_pImage->Translate( (int)( width * (1.0f - value) ), 0 );

            break;
        }
        case HORIZONTAL_SLIDER_ABOUT_CENTER:
        {
            m_pImage->ScaleAboutCenter( value, 1.0f, 1.0f );

            break;
        }
        case VERTICAL_SLIDER_BOTTOM:
        {
            m_pImage->Translate( 0, -posY );
            m_pImage->Scale( 1.0f, value, 1.0f );
            m_pImage->Translate( 0, posY );

            break;
        }
        case VERTICAL_SLIDER_TOP:
        {
            m_pImage->Translate( 0, -posY );
            m_pImage->Scale( 1.0f, value, 1.0f );
            m_pImage->Translate( 0, posY );

            // top align image
            //
            m_pImage->Translate( 0, (int)( height * (1.0f - value) ) );

            break;
        }
        case VERTICAL_SLIDER_ABOUT_CENTER:
        {
            m_pImage->ScaleAboutCenter( 1.0f, value, 1.0f );

            break;
        }
        default:
        {
            rAssertMsg( 0, "ERROR: *** Invalid slider type!" );

            break;
        }
    }
}

