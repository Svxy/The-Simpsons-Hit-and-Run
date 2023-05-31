//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        sounddebugpage.h
//
// Description: Declaration of SoundDebugPage class, which is used for displaying
//              a chunk of debug info in Watcher (or whatever our display
//              mechanism happens to be)
//
// History:     11/22/2002 + Created -- Darren
//
//=============================================================================

#ifndef SOUNDDEBUGPAGE_H
#define SOUNDDEBUGPAGE_H

//========================================
// Nested Includes
//========================================
#include <p3d/p3dtypes.hpp>

//========================================
// Forward References
//========================================
class SoundDebugDisplay;

#ifndef RAD_RELEASE
#define SOUND_DEBUG_INFO_ENABLED
#endif

//=============================================================================
//
// Synopsis:    SoundDebugPage
//
//=============================================================================

class SoundDebugPage
{
    public:
        SoundDebugPage();
        SoundDebugPage( unsigned int pageNum, SoundDebugDisplay* master );
        virtual ~SoundDebugPage();

        void LazyInitialization( unsigned int pageNum, SoundDebugDisplay* master );

        unsigned int GetPage() { return( m_pageNum ); }

        void Render( int leftPosn, int topPosn, tColour& colour, bool dumpToWindow );

    protected:

        virtual void fillLineBuffer( int lineNum, char* buffer ) = 0;
        virtual int getNumLines() = 0;

    private:
        //Prevent wasteful constructor creation.
        SoundDebugPage( const SoundDebugPage& original );
        SoundDebugPage& operator=( const SoundDebugPage& rhs );

        unsigned int m_pageNum;

        SoundDebugDisplay* m_displayMaster;
};


#endif // SOUNDDEBUGPAGE_H

