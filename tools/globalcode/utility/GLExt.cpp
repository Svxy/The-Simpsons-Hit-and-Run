#include "precompiled/PCH.h"
//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// GLExt.cpp
//
// Description: Functions that do drawing using the OpenGL API. 
//
// Modification History:
//  + Created Aug 21, 2001 -- bkusy 
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
#include "GLExt.h"

//----------------------------------------
// Forward References
//----------------------------------------

//----------------------------------------
// Constants, Typedefs and Statics
//----------------------------------------

//=============================================================================
// GLExt::drawCamera3D
//=============================================================================
// Description: Comment
//
// Parameters:  ( double scale, double tx = 0.0f, double ty = 0.0f, double tz = 0.0f, float width = 1.0f )
//
// Return:      void 
//
//=============================================================================
void GLExt::drawCamera3D( double scale, double tx, double ty, double tz, float width )
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );

    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPushMatrix();
    glScaled(scale, scale, scale);
    glTranslated(tx, ty, tz);

    //Set line width
    glLineWidth( width );
    
    glBegin( GL_LINES );
        //X-Z
        glVertex3d(-0.5, 0.0, 0.5 );
        glVertex3d( 0.5, 0.0, 0.5 );

        glVertex3d( 0.5, 0.0,  0.5 );
        glVertex3d( 0.5, 0.0, -0.5 );

        glVertex3d(  0.5, 0.0, -0.5 );
        glVertex3d( -0.5, 0.0, -0.5 );

        glVertex3d( -0.5, 0.0, -0.5 );
        glVertex3d( -0.5, 0.0,  0.5 );

        glVertex3d( 0.125, 0.0, -0.5 );
        glVertex3d( 0.325, 0.0, -0.75 );

        glVertex3d(  0.325, 0.0, -0.75 );
        glVertex3d( -0.325, 0.0, -0.75 );

        glVertex3d( -0.325, 0.0, -0.75 );
        glVertex3d( -0.125, 0.0, -0.5 );

        //Y-Z
        glVertex3d( 0.0, -0.5, 0.5 );
        glVertex3d( 0.0,  0.5, 0.5 );

        glVertex3d( 0.0, 0.5,  0.5 );
        glVertex3d( 0.0, 0.5, -0.5 );

        glVertex3d( 0.0,  0.5, -0.5 );
        glVertex3d( 0.0, -0.5, -0.5 );

        glVertex3d( 0.0, -0.5, -0.5 );
        glVertex3d( 0.0, -0.5,  0.5 );

        glVertex3d( 0.0, 0.125, -0.5 );
        glVertex3d( 0.0, 0.325, -0.75 );

        glVertex3d( 0.0,  0.325, -0.75 );
        glVertex3d( 0.0, -0.325, -0.75 );

        glVertex3d( 0.0, -0.325, -0.75 ); 
        glVertex3d( 0.0, -0.125, -0.5 );        
    glEnd();
    glPopMatrix();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glLineWidth( oldWidth );

    glPopAttrib();
}

//-----------------------------------------------------------------------------
// d r a w C r o s s H a i r 3 D 
//
// Synopsis:    Draws a camera symbol
//
// Parameters:  scale   - the scale to draw at.
//              x       - the x coordinate of the symbol.
//              y       - the y coordinate of the symbol.
//              z       - the z coordinate of the symbol.
//              width   - line width
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void GLExt::drawCrossHair3D( double scale, double tx, double ty, double tz, float width )
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );

    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPushMatrix();
    glScaled(scale, scale, scale);
    glTranslated(tx, ty, tz);

    //Set line width
    glLineWidth( width );
    
    glBegin( GL_LINES );
        glVertex3d(-1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 0.0 );

        glVertex3d( 0.0, -1.0, 0.0 );
        glVertex3d( 0.0,  1.0, 0.0 );

        glVertex3d( 0.0, 0.0, -1.0 );
        glVertex3d( 0.0, 0.0,  1.0 );
    glEnd();
    glPopMatrix();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glLineWidth( oldWidth );

    glPopAttrib();
}

//-----------------------------------------------------------------------------
// d r a w C r o s s H a i r 3 D
//
// Synopsis:    Draws a camera symbol
//
// Parameters:  scale - the scale to draw the symbol at.
//              p     - where to draw the symbol.
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void GLExt::drawCrossHair3D( double scale, const MPoint& p, float width )
{
    drawCrossHair3D( scale, p.x, p.y, p.z, width );
}


//-----------------------------------------------------------------------------
// d r a w P y r a m i d 
//
// Synopsis:    Draws a pyramid using OpenGL.
//
// Parameters:  scale  - factor of scaling for the pyramid.
//              tx     - x position translation.
//              ty     - y position translation.
//              tz     - z position translation.
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void GLExt::drawPyramid( double scale, double tx, double ty, double tz, float width )
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );

    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPushMatrix();
    glScaled(scale, scale, scale);
    glTranslated(tx, ty, tz);
    glBegin( GL_QUAD_STRIP );
        glVertex3d(0.5, 0 ,0.5);
        glVertex3d(0, 1, 0);
        glVertex3d(0.5, 0, -0.5);
        glVertex3d(0, 1, 0);
        glVertex3d(-0.5, 0, -0.5);
        glVertex3d(0, 1, 0);
        glVertex3d(-0.5, 0 ,0.5);
        glVertex3d(0, 1, 0);
        glVertex3d(0.5, 0 ,0.5);
        glVertex3d(0, 1, 0);
    glEnd();
    glPopMatrix();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glLineWidth( oldWidth );

    glPopAttrib();
}

//-----------------------------------------------------------------------------
// d r a w P y r a m i d 
//
// Synopsis:    Draws a pyramid using OpenGL.
//
// Parameters:  scale  - factor of scaling for the pyramid.
//              p0     - the point at which to draw the pyramid.
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void GLExt::drawPyramid( double scale, const MPoint& p0, float width )
{
    drawPyramid( scale, p0.x, p0.y, p0.z, width );
}

//-----------------------------------------------------------------------------
// d r a w S p h e r e 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//------------------------------------------------------radius-----------------------
void GLExt::drawSphere( double scale, double tx, double ty, double tz, float width )
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    glPushMatrix();
    glScaled(scale, scale, scale);
    glTranslated(tx, ty, tz);

    GLUquadricObj* quadric = gluNewQuadric();
    gluQuadricDrawStyle( quadric, GLU_LINE );
    gluSphere( quadric, 1.0, 12, 6 );

    glPopMatrix();

    glLineWidth( oldWidth );
    glPopAttrib();
}

//-----------------------------------------------------------------------------
// d r a w S p h e r e 
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
void GLExt::drawSphere( double scale, const MPoint& p, float width )
{
    drawSphere( scale, p.x, p.y, p.z, width );
}

//-----------------------------------------------------------------------------
// d r a w L i n e 
//
// Synopsis:    Draws a line between the two points.
//
// Parameters:  The coordinates of the two points.
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void GLExt::drawLine( double x0, double y0, double z0,
                      double x1, double y1, double z1,
                      float width )
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );
    
    glBegin( GL_LINES );
        glVertex3f( (float)x0, (float)y0, (float)z0 );
        glVertex3f( (float)x1, (float)y1, (float)z1 );
    glEnd();
    
    //Reset line width
    glLineWidth( oldWidth );

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPopAttrib();
}

//-----------------------------------------------------------------------------
// d r a w L i n e 
//
// Synopsis:    Draws a line between the two points.
//
// Parameters:  The points to draw the line between.
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void GLExt::drawLine( const MPoint& p0, const MPoint& p1, float width )
{
    drawLine( p0.x, p0.y, p0.z, p1.x, p1.y, p1.z, width );
}

//=============================================================================
// GLExt::drawArrow
//=============================================================================
// Description: Comment
//
// Parameters:  ( const MPoint& p0, const MPoint& p1, float width, float scale )
//
// Return:      void 
//
//=============================================================================
void GLExt::drawArrow( const MPoint& p0, const MPoint& p1, float width, float scale )
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    MVector scaledVector;
    scaledVector = p1 - p0;
    scaledVector /= scale;

    //Figure out a cross product...
    MVector normal;

    normal = scaledVector^( MVector(0, 1.0, 0 ) );
    normal.normalize();
    normal *= scaledVector.length();

    MPoint head0, head1;

    head0 = (p1 - scaledVector + normal );
    head1 = (p1 - scaledVector - normal );

    //Set line width
    glLineWidth( width );
    
    glBegin( GL_LINES );
        glVertex3f( (float)p0.x, (float)p0.y, (float)p0.z );
        glVertex3f( (float)p1.x, (float)p1.y, (float)p1.z );

        glVertex3f( (float)p1.x, (float)p1.y, (float)p1.z );
        glVertex3f( (float)head0.x, (float)head0.y, (float)head0.z );

        glVertex3f( (float)p1.x, (float)p1.y, (float)p1.z );
        glVertex3f( (float)head1.x, (float)head1.y, (float)head1.z );
    glEnd();
    
    //Reset line width
    glLineWidth( oldWidth );

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPopAttrib();
}

//-----------------------------------------------------------------------------
// d r a w B o x 
//
// Synopsis:    Draws a box using OpenGL.
//
// Parameters:  The coordinates of the minimal and maximal corners of the box.
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void GLExt::drawBox( double x0, double y0, double z0, 
                     double x1, double y1, double z1,
                     float width  )
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin( GL_QUADS );

        glVertex3f( (float)x0, (float)y0, (float)z0 );
        glVertex3f( (float)x0, (float)y0, (float)z1 );
        glVertex3f( (float)x1, (float)y0, (float)z1 );
        glVertex3f( (float)x1, (float)y0, (float)z0 );

        glVertex3f( (float)x0, (float)y1, (float)z0 );
        glVertex3f( (float)x0, (float)y1, (float)z1 );
        glVertex3f( (float)x1, (float)y1, (float)z1 );
        glVertex3f( (float)x1, (float)y1, (float)z0 );

    glEnd();

    glBegin( GL_LINES );

        glVertex3f( (float)x0, (float)y0, (float)z0 );
        glVertex3f( (float)x0, (float)y1, (float)z0 );
        
        glVertex3f( (float)x0, (float)y0, (float)z1 );
        glVertex3f( (float)x0, (float)y1, (float)z1 );

        glVertex3f( (float)x1, (float)y0, (float)z1 );
        glVertex3f( (float)x1, (float)y1, (float)z1 );

        glVertex3f( (float)x1, (float)y0, (float)z0 );
        glVertex3f( (float)x1, (float)y1, (float)z0 );
    glEnd();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glLineWidth( oldWidth );
    glPopAttrib();
}

//-----------------------------------------------------------------------------
// d r a w B o x 
//
// Synopsis:    Draws a box using OpenGL.
//
// Parameters:  p0 - the minimal corner of the box.
//              p1 - the maximal corner of the box.
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void GLExt::drawBox( const MPoint& p0, const MPoint& p1, float width )
{
    drawBox( p0.x, p0.y, p0.z, p1.x, p1.y, p1.z, width );
}

//-----------------------------------------------------------------------------
// d r a w X Z P l a n e
//
// Synopsis:    Draws a finite plane in the XZ plane using OpenGL.
//
// Parameters:  x0, z0 - the minimal vertex of the finite plane.
//              x1, z1 - the maximal vertex of the finite plane.
//              y      - the height of the plane.
//
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void GLExt::drawXZPlane( double xscale, 
                         double yscale, 
                         double zscale,
                         double x0, 
                         double z0, 
                         double x1, 
                         double z1, 
                         double y,
                         float width
                       )
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    glPushMatrix();
    glScaled(xscale, yscale, zscale);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
    glBegin( GL_QUADS );

        glVertex3f( (float)x0, (float)y, (float)z0 );
        glVertex3f( (float)x0, (float)y, (float)z1 );
        glVertex3f( (float)x1, (float)y, (float)z1 );
        glVertex3f( (float)x1, (float)y, (float)z0 );

    glEnd();

    glPopMatrix();

    glLineWidth( oldWidth );
    glPopAttrib();
}

//-----------------------------------------------------------------------------
// d r a w X Z P l a n e
//
// Synopsis:    Draws a finite plane in the XZ plane using OpenGL.
//
// Parameters:  p0 - the minimal vertex of the finite plane.
//              p1 - the maximal vertex of the finite plane.
//
// Returns:     NOTHING
//
// Constraints: The y component of p1 is ignored and the plane is drawn at 
//              the height of the first point.
//
//-----------------------------------------------------------------------------
void GLExt::drawXZPlane( double xscale, 
                         double yscale,
                         double zscale,
                         const MPoint& v0, 
                         const MPoint& v1,
                         float width
                       )
{
    drawXZPlane( xscale, yscale, zscale, v0.x, v0.z, v1.x, v1.z, v0.y, width );
}

//=============================================================================
// GLExt::drawP
//=============================================================================
// Description: Comment
//
// Parameters:  (double scale, double tx, double ty, double tz, float width)
//
// Return:      void 
//
//=============================================================================
void GLExt::drawP(double scale, double tx, double ty, double tz, float width)
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    glPushMatrix();
    glScaled(scale, scale, scale);
    glTranslated(tx, ty, tz);
    glBegin( GL_LINE_LOOP );
        //Draw a "P" for pickup.
        glVertex3d( 0,0,0);
        glVertex3d( 0, 2.0, 0);
        glVertex3d( 0.7, 2.0, 0);
        glVertex3d( 1.2, 1.7, 0);
        glVertex3d( 1.2, 1.3, 0);
        glVertex3d( 0.7, 1, 0);
        glVertex3d( 0, 1, 0);
    glEnd();
    glPopMatrix();

    glLineWidth( oldWidth );
    glPopAttrib();
}

//=============================================================================
// GLExt::drawStickMan
//=============================================================================
// Description: Comment
//
// Parameters:  (double scale, double tx, double ty, double tz, float width)
//
// Return:      void 
//
//=============================================================================
void GLExt::drawStickMan(double scale, double tx, double ty, double tz, float width)
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    glPushMatrix();
    glScaled(scale, scale, scale);
    glTranslated(tx, ty, tz);
    glBegin( GL_LINES );
        glVertex3d( 0,0.6,0);
        glVertex3d( 0, 1.5, 0);
        glVertex3d( -0.5, 1.2, 0.0);
        glVertex3d( 0.5, 1.2, 0);
    glEnd();
    glBegin( GL_LINE_STRIP );
        glVertex3d( -0.5, 0, 0 );
        glVertex3d( 0, 0.6, 0 );
        glVertex3d( 0.5, 0, 0 );
    glEnd();
    glBegin( GL_QUADS );
        glVertex3d( -0.125, 1.5, 0 );
        glVertex3d( -0.125, 1.75, 0 );
        glVertex3d( 0.125, 1.75, 0 );
        glVertex3d( 0.125, 1.5, 0 );
    glEnd();
    glPopMatrix();

    glLineWidth( oldWidth );
    glPopAttrib();
}

//=============================================================================
// GLExt::drawCar
//=============================================================================
// Description: Comment
//
// Parameters:  (double scale, double tx, double ty, double tz, float width)
//
// Return:      void 
//
//=============================================================================
void GLExt::drawCar(double scale, double tx, double ty, double tz, float width)
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    glPushMatrix();
    glScaled(scale, scale, scale);
    glTranslated(tx, ty, tz);
    glBegin( GL_LINE_LOOP );
        glVertex3d( 0, 0, 0);
        glVertex3d( 0, 0, -0.25);
        glVertex3d( 0, -0.2, -0.5 );
        glVertex3d( 0, 0, -0.75 );
        glVertex3d( 0, 0, -1.0 );
        glVertex3d( 0, 0.4, -1.0 );
        glVertex3d( 0, 0.4, -0.5 );
        glVertex3d( 0, 0.7, -0.45 );
        glVertex3d( 0, 0.7, 0.25 );
        glVertex3d( 0, 0.4, 0.4 );
        glVertex3d( 0, 0.3, 1.0 );
        glVertex3d( 0, 0, 1.0 );
        glVertex3d( 0, 0, 0.75 );
        glVertex3d( 0, -0.2, 0.5 );
        glVertex3d( 0, 0, 0.25 );
    glEnd();
    glPopMatrix();

    glLineWidth( oldWidth );
    glPopAttrib();
}

//=============================================================================
// GLExt::drawA
//=============================================================================
// Description: Comment
//
// Parameters:  (double scale, double tx, double ty, double tz, float width)
//
// Return:      void 
//
//=============================================================================
void GLExt::drawA(double scale, double tx, double ty, double tz, float width)
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    //Draw an "A" on top of the pyramid
    glPushMatrix();
    glScaled(scale, scale, scale);
    glTranslated(tx, ty, tz);
    glBegin( GL_LINES );
        glVertex3d( 0, 2.0, 0);
        glVertex3d( -0.7, 0, 0);
        glVertex3d( 0, 2.0, 0);
        glVertex3d( 0.7, 0, 0);
        glVertex3d( -0.7, 1.2, 0);
        glVertex3d( 0.7, 1.2, 0);
    glEnd();
    glPopMatrix();

    glLineWidth( oldWidth );
    glPopAttrib();
}

//=============================================================================
// GLExt::drawD
//=============================================================================
// Description: Comment
//
// Parameters:  (double scale, double tx, double ty, double tz, float width)
//
// Return:      void 
//
//=============================================================================
void GLExt::drawD(double scale, double tx, double ty, double tz, float width)
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    //Draw a "D" on top of the pyramid
    glPushMatrix();
    glScaled(scale, scale, scale);
    glTranslated(tx, ty, tz);
    glTranslated( -0.6, 0, 0);
    glBegin( GL_LINE_LOOP );
        //Draw a "D" for dropoff.
        glVertex3d( 0, 0, 0);
        glVertex3d( 0, 2.0, 0);
        glVertex3d( 0.6, 2.0, 0);
        glVertex3d( 1.1, 1.7, 0);
        glVertex3d( 1.2, 1, 0);
        glVertex3d( 1.1, 0.3, 0);
        glVertex3d( 0.6, 0, 0);
    glEnd();
    glPopMatrix();
    glLineWidth( oldWidth );
    glPopAttrib();
}

//=============================================================================
// GLExt::drawE
//=============================================================================
// Description: Comment
//
// Parameters:  (double scale, double tx, double ty, double tz, float width)
//
// Return:      void 
//
//=============================================================================
void GLExt::drawE(double scale, double tx, double ty, double tz, float width)
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    //Draw the Letter "E" on top of the pyramid
    glPushMatrix();
    glScaled(scale, scale, scale);
    glTranslated(tx, ty, tz);
    glBegin( GL_LINES );
        glVertex3d( -0.4, 2,  0); 
	    glVertex3d( -0.4, 0,  0);
        glVertex3d( -0.4, 1, 0);
        glVertex3d( 0.3, 1, 0);
        glVertex3d( -0.4, 0, 0);
        glVertex3d( 0.4, 0, 0);
        glVertex3d( -0.4, 2, 0);
        glVertex3d( 0.4, 2, 0);
    glEnd();
    glPopMatrix();

    glLineWidth( oldWidth );
    glPopAttrib();
}

//=============================================================================
// GLExt::drawS
//=============================================================================
// Description: Comment
//
// Parameters:  (double scale, double tx, double ty, double tz, float width)
//
// Return:      void 
//
//=============================================================================
void GLExt::drawS(double scale, double tx, double ty, double tz, float width)
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    glPushMatrix();
    glScaled(scale,scale,scale);
    glTranslated(tx, ty, tz);
    glTranslated( -0.75, 0, 0);
    glBegin( GL_LINE_STRIP );
        //Draw an "S" for sound
        glVertex3d( 0, 0.3, 0);
        glVertex3d( 0.4, 0, 0);
        glVertex3d( 1.2, 0, 0);
        glVertex3d( 1.5, 0.4, 0);
        glVertex3d( 1.2, 0.8, 0);
        glVertex3d( 0.3, 1.25, 0);
        glVertex3d( 0.05, 1.5, 0);
        glVertex3d( 0.25, 1.9, 0);
        glVertex3d( 0.9, 2.0, 0);
        glVertex3d( 1.4, 1.75, 0);
    glEnd();
    glPopMatrix();

    glLineWidth( oldWidth );
    glPopAttrib();
}

//=============================================================================
// GLExt::drawCross
//=============================================================================
// Description: Comment
//
// Parameters:  (double scale, double tx, double ty, double tz, float width)
//
// Return:      void 
//
//=============================================================================
void GLExt::drawCross(double scale, double tx, double ty, double tz, float width)
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    glPushMatrix();
    glScaled( scale, scale, scale );
    glTranslated( tx, ty, tz );
	glBegin( GL_LINES );
			glVertex3d( -1, 0, 0 ); 
			glVertex3d( 1, 0, 0 ); 
			
			glVertex3d( 0, 1, 0 ); 
			glVertex3d( 0, -1, 0 ); 
			
			glVertex3d( 0, 0, 1 ); 
			glVertex3d( 0, 0, -1 );
	glEnd();
    glPopMatrix();

    glLineWidth( oldWidth );
    glPopAttrib();
}

//=============================================================================
// GLExt::drawLBolt
//=============================================================================
// Description: Comment
//
// Parameters:  ( double scale, double tx, double ty, double tz, float width )
//
// Return:      void 
//
//=============================================================================
void GLExt::drawLBolt( double scale, double tx, double ty, double tz, float width )
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    glPushMatrix();
    glScaled(scale,scale,scale);
    glTranslated(tx, ty, tz);
    glBegin( GL_LINE_STRIP );
        glVertex3d( 0, 0, 0); 
	    glVertex3d( 0.5, 0.8,  0);
        glVertex3d( -0.2, 1.4, 0);
        glVertex3d( 0, 2.2, 0);
        glVertex3d( -0.5, 1.4, 0);
        glVertex3d( 0.2, 0.8, 0);
        glVertex3d( 0, 0, 0 );
    glEnd();
    glPopMatrix();

    glLineWidth( oldWidth );
    glPopAttrib();
}

//=============================================================================
// GLExt::drawI
//=============================================================================
// Description: Comment
//
// Parameters:  ( double scale, double tx, double ty, double tz, float width )
//
// Return:      void 
//
//=============================================================================
void GLExt::drawI( double scale, double tx, double ty, double tz, float width )
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    glPushMatrix();
    glScaled(scale,scale,scale);
    glTranslated(tx, ty, tz);
    glBegin( GL_LINE_STRIP );
        glVertex3d( -0.6, 2,  0); 
	    glVertex3d( 0.6, 2,  0);
        glVertex3d( 0, 2, 0);
        glVertex3d( 0, 0, 0);
        glVertex3d( -0.6, 0, 0);
        glVertex3d( 0.6, 0, 0);
    glEnd();
    glPopMatrix();

    glLineWidth( oldWidth );
    glPopAttrib();
}

//=============================================================================
// GLExt::drawW
//=============================================================================
// Description: Comment
//
// Parameters:  ( double scale, double tx, double ty, double tz, float width )
//
// Return:      void 
//
//=============================================================================
void GLExt::drawW( double scale, double tx, double ty, double tz, float width )
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    glPushMatrix();
    glScaled(scale,scale,scale);
    glTranslated(tx, ty, tz);
    glBegin( GL_LINE_STRIP );
        glVertex3d( -1.0, 2.0, 0);
        glVertex3d( -0.5, 0, 0);
        glVertex3d( 0, 1.5, 0);
        glVertex3d( 0.5, 0, 0);
        glVertex3d( 1.0, 2.0, 0);
    glEnd();
    glPopMatrix();

    glLineWidth( oldWidth );
    glPopAttrib();
}

//=============================================================================
// GLExt::drawM
//=============================================================================
// Description: Comment
//
// Parameters:  ( double scale, double tx, double ty, double tz, float width )
//
// Return:      void 
//
//=============================================================================
void GLExt::drawM( double scale, double tx, double ty, double tz, float width )
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    glPushMatrix();
    glScaled(scale,scale,scale);
    glTranslated(tx, ty, tz);
    glBegin( GL_LINE_STRIP );
        glVertex3d( -1.0, 0, 0);
        glVertex3d( -0.5, 2.0, 0);
        glVertex3d( 0, 0.5, 0);
        glVertex3d( 0.5, 2.0, 0);
        glVertex3d( 1.0, 0, 0);
    glEnd();
    glPopMatrix();

    glLineWidth( oldWidth );
    glPopAttrib();
}

//=============================================================================
// GLExt::drawC
//=============================================================================
// Description: Comment
//
// Parameters:  ( double scale, double tx, double ty, double tz, float width )
//
// Return:      void 
//
//=============================================================================
void GLExt::drawC( double scale, double tx, double ty, double tz, float width )
{
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    glPushMatrix();
    glScaled(scale,scale,scale);
    glTranslated(tx, ty, tz);
    glBegin( GL_LINE_STRIP );
        //Draw an "C" for carStart
        glVertex3d( 0.7, 0.5, 0);
        glVertex3d( 0.35, 0, 0);
        glVertex3d( -0.35, 0, 0);
        glVertex3d( -0.7, 0.5, 0);
        glVertex3d( -0.7, 1.5, 0);
        glVertex3d( -0.35, 2.0, 0);
        glVertex3d( 0.35, 2.0, 0);
        glVertex3d( 0.7, 1.5, 0);
    glEnd();
    glPopMatrix();

    glLineWidth( oldWidth );
    glPopAttrib();
}

//=============================================================================
// GLExt::drawO
//=============================================================================
// Description: Comment
//
// Parameters:  ( double scale, double tx, double ty, double tz, float width )
//
// Return:      void 
//
//=============================================================================
void GLExt::drawO( double scale, double tx, double ty, double tz, float width )
{
    scale = 100.0f;

    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );
    //Store old line width
    GLfloat oldWidth;
    glGetFloatv( GL_LINE_WIDTH, &oldWidth );

    //Set line width
    glLineWidth( width );

    glPushMatrix();
    glTranslated(tx, ty, tz);
    glScaled(scale,scale,scale);
    glBegin( GL_LINE_STRIP );
        //Draw an "O"
        glVertex3d( 0.7, 0.5, 0);
        glVertex3d( 0, 0, 0);
        glVertex3d( -0.7, 0.5, 0);
        glVertex3d( -0.7, 1.5, 0);
        glVertex3d( 0, 2.0, 0);
        glVertex3d( 0.7, 1.5, 0);
        glVertex3d( 0.7, 0.5, 0);
    glEnd();
    glPopMatrix();

    glLineWidth( oldWidth );
    glPopAttrib();
}

