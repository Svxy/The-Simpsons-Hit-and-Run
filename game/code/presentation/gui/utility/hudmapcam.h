//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   HudMapCam
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/10      TChu        Created for SRR2
//
//===========================================================================

#ifndef HUDMAPCAM_H
#define HUDMAPCAM_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <camera/kullcam.h>

//===========================================================================
// Interface Definitions
//===========================================================================
struct HudMapCam : public KullCam
{
    HudMapCam( int playerID );
    virtual ~HudMapCam();

    void Update( unsigned int milliseconds );
    void SetHeight( float height );

    tPointCamera* m_camera;
    rmt::Vector m_originalHeading;

};

#endif // HUDMAPCAM_H
