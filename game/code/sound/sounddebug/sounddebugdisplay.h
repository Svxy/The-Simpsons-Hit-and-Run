//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        sounddebugdisplay.h
//
// Description: Declaration for the SoundDebugDisplay class, used for
//              printing debugging/tuning information on screen
//
// History:     10/26/2002 + Created -- Darren
//
//=============================================================================

#ifndef SOUNDDEBUGDISPLAY_H
#define SOUNDDEBUGDISPLAY_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>

//========================================
// Forward References
//========================================
class SoundDebugPage;

namespace Sound
{
    class daSoundRenderingManager;
}


//=============================================================================
//
// Synopsis:    SoundDebugDisplay
//
//=============================================================================

class SoundDebugDisplay
{
    public:
        SoundDebugDisplay( Sound::daSoundRenderingManager* renderMgr );
        virtual ~SoundDebugDisplay();

        void Render();

        void RegisterPage( SoundDebugPage* page );
        void DeregisterPage( SoundDebugPage* page );

        static const int MAX_DEBUG_PAGES = 5;

        //
        // Watcher tunables
        //
        static bool s_isVisible;
        static int s_red;
        static int s_green;
        static int s_blue;
        // Text position
        static int s_leftOffset; 
        static int s_topOffset;
        // Displayed page
        static unsigned int s_page;
        static bool s_dumpToWindow;
        // Name display radius
        static float s_radius;
        // Type info
        static bool s_dumpTypeInfoToWindow;

    private:
        //Prevent wasteful constructor creation.
        SoundDebugDisplay( const SoundDebugDisplay& original );
        SoundDebugDisplay& operator=( const SoundDebugDisplay& rhs );

        void renderPositionAndHeapInfo( rmt::Vector* position );
        void renderNearbyObjectNames( rmt::Vector* position );

        void renderTextLine( const char* text, int leftPosn, int topPosn, tColour& colour );

        SoundDebugPage* m_debugPages[MAX_DEBUG_PAGES];

        Sound::daSoundRenderingManager* m_renderMgr;
};


#endif // SOUNDDEBUGDISPLAY_H

