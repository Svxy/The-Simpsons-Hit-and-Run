//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundloader.h
//
// Description: Declaration for the SoundLoader class, used to manage the loading 
//              and unloading of sound clips and streams.
//
// History:     25/06/2002 + Created -- Darren
//
//=============================================================================

#ifndef SOUNDLOADER_H
#define SOUNDLOADER_H

//========================================
// Nested Includes
//========================================

#include <render/Enums/RenderEnums.h>
#include <sound/soundrenderer/soundsystem.h>
#include <sound/soundclusternameenum.h>
#include <sound/soundcluster.h>
#include <events/eventlistener.h>

//========================================
// Forward References
//========================================

class SoundFileHandler;
class Vehicle;

//=============================================================================
//
// Synopsis:    SoundLoader class declaration
//
//=============================================================================

class SoundLoader : public EventListener
{
    public:
        SoundLoader();
        virtual ~SoundLoader();

        bool LoadClusterByName( const char* clusterName, SoundFileHandler* callbackObj );
        
        void LoadPermanentSounds() { queueLoad( SC_ALWAYS_LOADED ); }

        void LevelLoad( RenderEnums::LevelEnum level );
        void LevelUnload( bool goingToFe );

        void MissionLoad( RenderEnums::MissionEnum mission );
        void MissionUnload( RenderEnums::MissionEnum mission );

        void LoadFrontEnd() { queueLoad( SC_FRONTEND ); }
        void UnloadFrontEnd() { clusterUnload( SC_FRONTEND ); }

        void LoadCarSound( Vehicle* theCar, bool unloadOtherCars );

        bool IsSoundLoaded( Sound::daResourceKey soundKey );

        void SetCurrentCluster( SoundClusterName cluster ) 
            { rAssert( cluster != SC_MAX_CLUSTERS ); m_currentCluster = cluster; }
        bool AddResourceToCurrentCluster( const char* resourceName )
            { return( m_clusterList[m_currentCluster]->AddResource( resourceName ) ); }

        //
        // EventListener functions
        //
        void HandleEvent( EventEnum id, void* pEventData );

    private:
        //Prevent wasteful constructor creation.
        SoundLoader( const SoundLoader& original );
        SoundLoader& operator=( const SoundLoader& rhs );

        //
        // Queue a load with the loading manager
        //
        void queueLoad( SoundClusterName cluster );

        bool clusterLoad( SoundClusterName name, SoundFileHandler* callbackObj = NULL );
        void clusterUnload( SoundClusterName name );

        //
        // List of clusters, each holding list of loadable sounds
        //
        SoundCluster* m_clusterList[SC_MAX_CLUSTERS];

        //
        // Cluster that any created sound resources will be added to
        //
        SoundClusterName m_currentCluster;
};


#endif // SOUNDLOADER_H

