//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        tunerdebugpage.h
//
// Description: Displays debug info for the sound tuner
//
// History:     6/11/2003 + Created -- Esan
//
//=============================================================================

#ifndef TUNERDEBUGPAGE_H
#define TUNERDEBUGPAGE_H

//========================================
// Nested Includes
//========================================
#include <sound/sounddebug/sounddebugpage.h>

#include <sound/soundrenderer/dasoundgroup.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    TunerDebugPage
//
//=============================================================================

class TunerDebugPage : public SoundDebugPage
{
    public:
        TunerDebugPage();
        virtual ~TunerDebugPage();

        void SetDuckLevel( Sound::DuckSituations situation, Sound::DuckVolumes volumeType, float volume );
        void SetFinalDuckLevel( Sound::DuckVolumes volumeType, float volume );
        void SetUserVolume( Sound::DuckVolumes volumeType, float volume );

    protected:
        //
        // Pure virtual functions from SoundDebugPage
        //
        void fillLineBuffer( int lineNum, char* buffer );
        int getNumLines();

    private:
        //Prevent wasteful constructor creation.
        TunerDebugPage( const TunerDebugPage& tunerdebugpage );
        TunerDebugPage& operator=( const TunerDebugPage& tunerdebugpage );

        //
        // Ducking info
        //
        Sound::DuckVolumeSet m_duckLevels[Sound::NUM_DUCK_SITUATIONS];
        Sound::DuckVolumeSet m_finalDuckLevel;
        Sound::DuckVolumeSet m_userVolumes;
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //TUNERDEBUGPAGE_H
