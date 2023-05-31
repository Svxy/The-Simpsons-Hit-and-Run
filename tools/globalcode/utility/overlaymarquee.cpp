//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// marquee.cpp
//
// Description: 
//
// Modification History:
//  + Created Jun 14, 2001 -- bkusy 
//-----------------------------------------------------------------------------

//----------------------------------------
// System Includes
//----------------------------------------
#include <windows.h>
#include <assert.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>

//----------------------------------------
// Project Includes
//----------------------------------------
#include "overlaymarquee.h"

//----------------------------------------
// Forward References
//----------------------------------------

//----------------------------------------
// Define Owning Namespace
//----------------------------------------
namespace GLObj {

//----------------------------------------
// Constants, Typedefs and Statics
//----------------------------------------

//-----------------------------------------------------------------------------
// M a r q u e e 
//
// Synopsis:    Constructor
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
OverlayMarquee::OverlayMarquee() :
    m_xStart( 0 ),
    m_yStart( 0 ),
    m_xEnd( 0 ),
    m_yEnd( 0 )
{
}

//-----------------------------------------------------------------------------
// ~ M a r q u e e 
//
// Synopsis:    Destructor
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
OverlayMarquee::~OverlayMarquee()
{
}

//-----------------------------------------------------------------------------
// B e g i n 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void OverlayMarquee::Begin( M3dView& view, short xStart, short yStart )
{
    m_View = view;
    m_xStart = xStart;
    m_yStart = yStart;
    m_View.beginGL();
    m_View.beginOverlayDrawing();
}

//-----------------------------------------------------------------------------
// D r a w 
//
// Synopsis:    Draw a marquee with the given coordinates.
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void OverlayMarquee::Draw( short xEnd, short yEnd )
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    m_xEnd = xEnd;
    m_yEnd = yEnd;
    
    //
    // Clearing the overlay plane doesn't seem to work, but swapping buffers does
    // so we'll use that instead.  You've gotta luv Maya bugs -- embrace them 
    // don't squash them.
    //
    m_View.clearOverlayPlane();
    SwapBuffers( m_View.deviceContext() );

    // 
    // Setup the orthographic projection matrix.
    //
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0.0, (GLdouble)m_View.portWidth(), 0.0, (GLdouble)m_View.portHeight() );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef( 0.375, 0.375, 0.0 );

    //
    // Retrieve GL State.
    //
    float         lineWidth;
    unsigned char lineSmoothedFlag;
    glGetFloatv( GL_LINE_WIDTH, &lineWidth );
    glGetBooleanv( GL_LINE_SMOOTH, &lineSmoothedFlag );

    //
    // Set line style.
    //
    glLineStipple( 1, 0x5555 );
    glLineWidth( 1.0 );
    glEnable( GL_LINE_STIPPLE );
    glEnable( GL_LINE_SMOOTH );
    glIndexi( 2 );

    //
    // Draw marquee.
    //
    glBegin( GL_LINES );

        glVertex2i( m_xStart, m_yEnd );  // Left Side.
        glVertex2i( m_xStart, m_yStart );

        glVertex2i( m_xStart, m_yStart );// Top 
        glVertex2i( m_xEnd,   m_yStart );

        glVertex2i( m_xEnd, m_yStart );  // Right Side.
        glVertex2i( m_xEnd, m_yEnd );

        glVertex2i( m_xEnd,    m_yEnd ); // Bottom.
        glVertex2i( m_xStart, m_yEnd );

    glEnd();

    //
    // Don't swap the display buffers.  A bug in Maya causes the display to be 
    // cleared -- making the flicker worse.
    //
    //SwapBuffers( m_View.deviceContext() );

    // 
    // Instead just flush the GL buffers.
    //
    glFlush();
    

    // 
    // Restore GL state.
    glLineWidth( lineWidth );
    if ( !lineSmoothedFlag )  glDisable( GL_LINE_SMOOTH );
    glDisable( GL_LINE_STIPPLE );

    glPopAttrib();
}

//-----------------------------------------------------------------------------
// R e d r a w 
//
// Synopsis:    Draw a marquee with the last given coordinates.
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void OverlayMarquee::Redraw()
{
    Draw( m_xEnd, m_yEnd );
}

void OverlayMarquee::End()
{
    m_View.endOverlayDrawing();
    m_View.endGL();
    
    //
    // Use the Maya bug to get rid of any drawing we have done.
    //
    SwapBuffers( m_View.deviceContext() );
}

} // namespace GLObj
    



