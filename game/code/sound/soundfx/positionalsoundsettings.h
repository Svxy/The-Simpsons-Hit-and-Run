//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        positionalsoundsettings.h
//
// Description: Declaration of object encapsulating positional sound settings
//
// History:     12/20/2002 + Created -- Darren
//
//=============================================================================

#ifndef POSITIONALSOUNDSETTINGS_H
#define POSITIONALSOUNDSETTINGS_H

//========================================
// Nested Includes
//========================================
#include <radlinkedclass.hpp>

#include <sound/soundfx/ipositionalsoundsettings.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    positionalSoundSettings
//
//=============================================================================

class positionalSoundSettings: public IPositionalSoundSettings,
                               public radLinkedClass< positionalSoundSettings >,
                               public radRefCount
{
    public:
        IMPLEMENT_REFCOUNTED( "positionalSoundSettings" );
        positionalSoundSettings();
        virtual ~positionalSoundSettings();

        void SetClipName( const char* clipName );
        const char* GetClipName() { return( m_clipName ); }

        void SetMinDistance( float min );
        float GetMinDistance() { return( m_minDist ); }

        void SetMaxDistance( float max );
        float GetMaxDistance() { return( m_maxDist ); }

        void SetPlaybackProbability( float prob );
        float GetPlaybackProbability() { return( m_playProbability ); }

    private:
        //Prevent wasteful constructor creation.
        positionalSoundSettings( const positionalSoundSettings& positionalsoundsettings );
        positionalSoundSettings& operator=( const positionalSoundSettings& positionalsoundsettings );

        //
        // Settings
        //
        char* m_clipName;
        float m_minDist;
        float m_maxDist;
        float m_playProbability;
};

//=============================================================================
// Factory Functions
//=============================================================================

//
// Create a positionalSoundSettings object
//
void PositionalSettingsObjCreate
(
    IPositionalSoundSettings** ppSettings,
    radMemoryAllocator allocator
);

#endif //POSITIONALSOUNDSETTINGS_H
