//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        sounddebugpage.cpp
//
// Description: Implementation of SoundDebugPage class, which is used for 
//              displaying a chunk of debug info in Watcher (or whatever 
//              our display mechanism happens to be)
//
// History:     11/22/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <p3d/utility.hpp>

//========================================
// Project Includes
//========================================
#include <sound/sounddebug/sounddebugpage.h>

#include <sound/sounddebug/sounddebugdisplay.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// SoundDebugPage::SoundDebugPage
//==============================================================================
// Description: Constructor.
//
// Parameters:	None
//
// Return:      N/A.
//
//==============================================================================
SoundDebugPage::SoundDebugPage() :
    m_pageNum( 0 ),
    m_displayMaster( NULL )
{
    //
    // Presumably, we're saving full initialization for later.
    //

#ifndef SOUND_DEBUG_INFO_ENABLED
    //
    // Don't want to build these in release, so make it die horribly.
    //
    rReleaseAssertMsg( false, "Oops, shouldn't be storing sound debug info in release" );
#endif
}

//==============================================================================
// SoundDebugPage::SoundDebugPage
//==============================================================================
// Description: Constructor.
//
// Parameters:	pageNum - page we'll display info on in Watcher's SoundInfo.
//
// Return:      N/A.
//
//==============================================================================
SoundDebugPage::SoundDebugPage( unsigned int pageNum, SoundDebugDisplay* master ) :
m_pageNum( pageNum ),
m_displayMaster( master )
{
#ifndef SOUND_DEBUG_INFO_ENABLED
    //
    // Don't want to build these in release, so make it die horribly.
    //
    rReleaseAssertMsg( false, "Oops, shouldn't be storing sound debug info in release" );
#endif

    //
    // Tell debug display master about our existence
    //
    master->RegisterPage( this );
}

//==============================================================================
// SoundDebugPage::~SoundDebugPage
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundDebugPage::~SoundDebugPage()
{
    m_displayMaster->DeregisterPage( this );
}

//=============================================================================
// SoundDebugPage::LazyInitialization
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int pageNum, SoundDebugDisplay* master )
//
// Return:      void 
//
//=============================================================================
void SoundDebugPage::LazyInitialization( unsigned int pageNum, SoundDebugDisplay* master )
{
    m_pageNum = pageNum;
    m_displayMaster = master;

    m_displayMaster->RegisterPage( this );
}

//=============================================================================
// SoundDebugPage::Render
//=============================================================================
// Description: Draw our debug info.  This relies on subclasses providing the
//              info and telling us how many lines to draw
//
// Parameters:  leftPosn - leftmost horizontal position to write text to
//              topPosn - topmost vertical position to write text to
//              colour - colour of pddi text to use
//              dumpToWindow - also write text as debug string if true
//
// Return:      void 
//
//=============================================================================
void SoundDebugPage::Render( int leftPosn, int topPosn, tColour& colour, bool dumpToWindow )
{
    int i;
    char lineBuffer[255];
    int numLines = getNumLines();

    for( i = 0; i < numLines; i++ )
    {
        fillLineBuffer( i, lineBuffer );

        p3d::pddi->DrawString( lineBuffer, leftPosn, topPosn + (i * 20), colour );

        if ( dumpToWindow )
        {
            rDebugString( lineBuffer );
        }
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
