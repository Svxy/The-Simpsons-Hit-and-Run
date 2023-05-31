#ifndef _GLEXT_H
#define _GLEXT_H
//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// GLExt.h
//
// Description: Functions that do drawing using the OpenGL API. 
//
// Modification History:
//  + Created Aug 21, 2001 -- bkusy 
//  + grossly modified by Cary Brisebois
//-----------------------------------------------------------------------------

//----------------------------------------
// System Includes
//----------------------------------------

//----------------------------------------
// Project Includes
//----------------------------------------

//----------------------------------------
// Forward References
//----------------------------------------
class MPoint;

//----------------------------------------
// Constants, Typedefs and Statics
//----------------------------------------

class GLExt
{
public:
    static void drawCamera3D( double scale, double tx = 0.0f,
                                            double ty = 0.0f,
                                            double tz = 0.0f,
                                            float width = 1.0f );

    static void drawCrossHair3D( double scale, double tx = 0.0f,
                                               double ty = 0.0f,
                                               double tz = 0.0f,
                                               float width = 1.0f );

    static void drawCrossHair3D( double scale, 
                                 const MPoint& p,
                                 float width = 1.0f );


    static void drawPyramid( double scale, double tx = 0.0f, 
                                           double ty = 0.0f,
                                           double tz = 0.0f,
                                           float width = 1.0f );

    static void drawPyramid( double scale, 
                             const MPoint& p0,
                             float width = 1.0f );

    static void drawLine( double x0, double y0, double z0, 
                          double x1, double y1, double z1,
                          float width = 1.0f );

    static void drawLine( const MPoint& p0, 
                          const MPoint& p1, 
                          float width = 1.0f );

    static void drawArrow( const MPoint& p0, 
                           const MPoint& p1,
                           float width = 1.0f,
                           float scale = 6.0f );

    static void drawBox( double x0, double y0, double z0, 
                         double x1, double y1, double z1,
                         float width = 1.0f );       

    static void drawBox( const MPoint& p0, 
                         const MPoint& p1,
                         float width = 1.0f );

    static void drawSphere( double scale, double tx = 0, 
                            double ty = 0, double tz = 0,
                            float width = 1.0f );

    static void drawSphere( double scale, 
                            const MPoint& p,
                            float width = 1.0f );
    
    static void drawXZPlane( double xscale,
                             double yscale,
                             double zscale, 
                             double x0, 
                             double z0,
                             double x1, 
                             double z1, 
                             double y,
                             float width = 1.0f
                           );

    static void drawXZPlane( double xscale,
                             double yscale,
                             double zscale, 
                             const MPoint& v0, 
                             const MPoint& v1,
                             float width = 1.0f 
                           );
    static void drawP( double scale, 
                       double tx = 0, 
                       double ty = 0, 
                       double tz = 0, 
                       float width = 1.0f );

    static void drawStickMan( double scale, 
                              double tx = 0, 
                              double ty = 0, 
                              double tz = 0, 
                              float width = 1.0f );

    static void drawCar( double scale, 
                         double tx = 0, 
                         double ty = 0, 
                         double tz = 0, 
                         float width = 1.0f );

    static void drawA( double scale, 
                       double tx = 0, 
                       double ty = 0, 
                       double tz = 0, 
                       float width = 1.0f );

    static void drawD( double scale, 
                       double tx = 0, 
                       double ty = 0, 
                       double tz = 0, 
                       float width = 1.0f );

    static void drawE( double scale, 
                       double tx = 0, 
                       double ty = 0, 
                       double tz = 0, 
                       float width = 1.0f );

    static void drawS( double scale, 
                       double tx = 0, 
                       double ty = 0, 
                       double tz = 0, 
                       float width = 1.0f );

    static void drawCross( double scale, 
                           double tx = 0, 
                           double ty = 0, 
                           double tz = 0, 
                           float width = 1.0f );

    static void drawLBolt( double scale, 
                           double tx = 0, 
                           double ty = 0, 
                           double tz = 0, 
                           float width = 1.0f );

    static void drawI( double scale, 
                       double tx = 0, 
                       double ty = 0, 
                       double tz = 0, 
                       float width = 1.0f );

    static void drawW( double scale, 
                       double tx = 0, 
                       double ty = 0, 
                       double tz = 0, 
                       float width = 1.0f );

    static void drawM( double scale, 
                       double tx = 0, 
                       double ty = 0, 
                       double tz = 0, 
                       float width = 1.0f );

    static void drawC( double scale, 
                       double tx = 0, 
                       double ty = 0, 
                       double tz = 0, 
                       float width  = 1.0f );

    static void drawO( double scale, 
                       double tx = 0, 
                       double ty = 0, 
                       double tz = 0, 
                       float width  = 1.0f );
};

#endif
