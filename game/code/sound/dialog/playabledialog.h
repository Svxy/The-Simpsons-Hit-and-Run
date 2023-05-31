//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        playabledialog.h
//
// Description: Abstract base class for the basic unit of dialog playback.
//              A PlayableDialog object represents a piece of dialog which 
//              is to be played without interruption (unless it gets booted 
//              by a higher-priority PlayableDialog).
//
// History:     02/09/2002 + Created -- Darren
//
//=============================================================================

#ifndef PLAYABLEDIALOG_H
#define PLAYABLEDIALOG_H

//========================================
// Nested Includes
//========================================
#include <sound/dialog/selectabledialog.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    PlayableDialog
//
//=============================================================================

class PlayableDialog : public SelectableDialog
{
    public:
        PlayableDialog();
        PlayableDialog( unsigned int level, unsigned int mission, EventEnum event );
        virtual ~PlayableDialog();

    private:
        //Prevent wasteful constructor creation.
        PlayableDialog( const PlayableDialog& original );
        PlayableDialog& operator=( const PlayableDialog& rhs );
};


#endif // PLAYABLEDIALOG_H

