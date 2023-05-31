#include "precompiled/PCH.h"

#ifndef _GLOBJ_OVERLAYMARQUEE_H
#define _GLOBJ_OVERLAYMARQUEE_H
//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// overlaymarquee.h
//
// Description: Draw a marquee using openGL.
//
// Modification History:
//  + Created Jun 14, 2001 -- bkusy 
//-----------------------------------------------------------------------------

//----------------------------------------
// System Includes
//----------------------------------------

//----------------------------------------
// Project Includes
//----------------------------------------
//#include <maya/M3dView.h>

//----------------------------------------
// Forward References
//----------------------------------------
//----------------------------------------
// Define Owning Namespace
//----------------------------------------
namespace GLObj {

class OverlayMarquee
{
    public:

        OverlayMarquee();
        ~OverlayMarquee();

        void Begin( M3dView& view, short xStart, short yStart ); 
        void Draw( short xEnd, short yEnd );
        void Redraw();
        void End();

    private:
        OverlayMarquee(OverlayMarquee &);
        OverlayMarquee &operator=(OverlayMarquee &);

        M3dView m_View;
        short m_xStart, m_yStart, m_xEnd, m_yEnd;
};

} // GLObj namespace.

#endif
