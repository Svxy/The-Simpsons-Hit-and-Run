//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundcluster.h
//
// Description: Declaration of SoundCluster class.  Used to allocate a group
//              of related sounds in sound memory.  This is an abstract base
//              class, the subclasses are used to differentiate between clips
//              and streams.
//
// History:     26/06/2002 + Created -- Darren
//
//=============================================================================

#ifndef SOUNDCLUSTER_H
#define SOUNDCLUSTER_H

//========================================
// Nested Includes
//========================================

#include <sound/soundrenderer/soundsystem.h>
#include <sound/soundrenderer/sounddynaload.h>

//========================================
// Forward References
//========================================

class SoundFileHandler;
struct IRadNameSpace;

//=============================================================================
//
// Synopsis:    SoundCluster
//
//=============================================================================

class SoundCluster : public Sound::IDaSoundDynaLoadCompletionCallback,
                     public radRefCount
{
    public:
        IMPLEMENT_REFCOUNTED( "SoundCluster" );

        SoundCluster( int clusterIndex,
                      IRadNameSpace* soundNamespace );
        virtual ~SoundCluster();

        bool AddResource( const char* resourceName )
            { return( AddResource( ::radMakeKey32( resourceName ) ) ); }
        bool AddResource( Sound::daResourceKey resourceKey );

        bool IsLoaded() const { return m_isLoaded; }

        void LoadSounds( SoundFileHandler* callbackObj = NULL );
        void UnloadSounds();
        
        bool ContainsResource( const char* resourceName ) 
            { return ContainsResource( ::radMakeKey32( resourceName ) ); }
        bool ContainsResource( Sound::daResourceKey resourceKey );

        IRadNameSpace* GetMyNamespace() { return( m_namespace ); }

        //
        // Interface for sound renderer load completion callback
        //
        void OnDynaLoadOperationsComplete( void* pUserData );

    private:
        //Prevent wasteful constructor creation.
        SoundCluster();
        SoundCluster( const SoundCluster& original );
        SoundCluster& operator=( const SoundCluster& rhs );

        static const int MAX_RESOURCES = 80;

        //
        // True if the sounds for this cluster are allocated in sound memory
        //
        bool m_isLoaded;

        //
        // List of sound clips
        //
        Sound::daResourceKey m_soundList[MAX_RESOURCES];

        //
        // Namespace containing these resources
        //
        IRadNameSpace* m_namespace;

        //
        // Callback object on load completion
        //
        SoundFileHandler* m_loadCompleteCallbackObj;

#ifdef RAD_DEBUG
        int m_clusterIndex;
#endif
};


#endif // SOUNDCLUSTER_H

