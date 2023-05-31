//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Slider
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

#ifndef SLIDER_H
#define SLIDER_H

#ifndef NULL
    #define NULL 0
#endif

//===========================================================================
// Nested Includes
//===========================================================================

//===========================================================================
// Forward References
//===========================================================================

namespace Scrooby
{
    class Polygon;
    class Sprite;
}

//===========================================================================
// Interface Definitions
//===========================================================================
struct Slider
{
    enum eSliderType
    {
        HORIZONTAL_SLIDER_LEFT,
        HORIZONTAL_SLIDER_ABOUT_CENTER,
        HORIZONTAL_SLIDER_RIGHT,

        VERTICAL_SLIDER_BOTTOM,
        VERTICAL_SLIDER_ABOUT_CENTER,
        VERTICAL_SLIDER_TOP,

        NUM_SLIDER_TYPES
    };

    eSliderType m_type;

    enum eVertex
    {
        VERTEX_BOTTOM_LEFT,
        VERTEX_TOP_LEFT,
        VERTEX_TOP_RIGHT,
        VERTEX_BOTTOM_RIGHT,

        NUM_VERTICES
    };

    Scrooby::Polygon* m_pPolygon;
    float m_value;

    // optional image to tag onto the end of the slider
    Scrooby::Sprite* m_pImage;

    // bottom-left and top-right vertex co-ordinates
    int x0;
    int y0;
    int x1;
    int y1;

    Slider( eSliderType type = HORIZONTAL_SLIDER_LEFT );
    void SetScroobyPolygon( Scrooby::Polygon* pPolygon, Scrooby::Sprite* pImage = NULL );
    virtual void SetValue( float value );

};

struct ImageSlider : public Slider
{
    ImageSlider( eSliderType type = HORIZONTAL_SLIDER_LEFT );

    void SetScroobyImage( Scrooby::Sprite* pImage );
    virtual void SetValue( float value, bool resetTransformation = true );

};

#endif // SLIDER_H
