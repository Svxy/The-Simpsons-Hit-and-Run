//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundfxgameplaylogic.h
//
// Description: Declaration for the SoundFXGameplayLogic class, which handles
//              the translation of events into sound effects for the game
//
// History:     31/07/2002 + Created -- Darren
//
//=============================================================================

#ifndef SOUNDFXGAMEPLAYLOGIC_H
#define SOUNDFXGAMEPLAYLOGIC_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>

#include <sound/soundfx/soundfxlogic.h>
#include <sound/positionalsoundplayer.h>
#include <events/eventenum.h>

//========================================
// Forward References
//========================================

class SoundCollisionData;
class Character;
class ScriptLocator;
class CollisionEntityDSG;
class globalSettings;

//=============================================================================
//
// Synopsis:    PositionalSFXPlayer
//
// Associated stuff required to play a positional collision sound
//
//=============================================================================
struct PositionalSFXPlayer
{
    PositionalSoundPlayer soundPlayer;
    void* collObjA;
    void* collObjB;
};

//=============================================================================
//
// Synopsis:    SoundFXGameplayLogic
//
//=============================================================================

class SoundFXGameplayLogic : public SoundFXLogic
{
    public:
        SoundFXGameplayLogic();
        virtual ~SoundFXGameplayLogic();

        void RegisterEventListeners();

        virtual void HandleEvent( EventEnum id, void* pEventData );

        //
        // Override callback to trigger tutorial events
        //
        void OnPlaybackComplete();

        //
        // Virtual function to clean up positional sounds
        //
        void Cleanup();

    private:
        //Prevent wasteful constructor creation.
        SoundFXGameplayLogic( const SoundFXGameplayLogic& original );
        SoundFXGameplayLogic& operator=( const SoundFXGameplayLogic& rhs );

        globalSettings* getGlobalSettings();

        void handleCollisionEvent( SoundCollisionData* collisionData );
        void handleFootstepEvent( Character* walkingCharacter );
        void handleSwitchEvent();
        void handleCollection();
        void handleObjectKick( CollisionEntityDSG* collObject );
        void playPositionalSound( ScriptLocator* locator );
        void playCarDoorSound( EventEnum eventType, Character* playerCharacter );
        void playCoinCollectSound();
        void startCollisionPlayer( const char* soundName, 
                                   CollisionEntityDSG* objA,
                                   CollisionEntityDSG* objB,
                                   rmt::Vector* positionPtr );

        bool collisionPairMatches( int index, void* firstObj, void* secondObj );

        // Number of simultaneous positional sounds
        static const int s_numPositionalSounds = 3;

        //
        // Positional sound objects
        //
        PositionalSoundPlayer m_positionalSounds[s_numPositionalSounds];

        // Number of simultaneous collision sounds
        static const int s_numCollisionSounds = 6;

        //
        // Positional collision sounds
        //
        PositionalSFXPlayer m_collisionSounds[s_numCollisionSounds];

        positionalSoundSettings* m_collisionMinMax;
        
        //
        // Counter for cycling through ka-ching sounds
        //
        unsigned int m_coinCounter;

        //
        // Timing for suspension ronks
        //
        unsigned int m_lastRonkTime;

        globalSettings* m_globalSettings;
};


#endif // SOUNDFXGAMEPLAYLOGIC_H

