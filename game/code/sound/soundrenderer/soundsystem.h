//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundsystem.hpp
//
// Subsystem:   Dark Angel - Sound System
//
// Description: General declarations relating to the Dark Angel sound system
//
// Notes:       This is the only file that may be loaded by non sound related
//              components of the game.
//     
// Revisions:	October 2, 2001     Creation                BJR
//
//=============================================================================

#ifndef _SOUNDSYSTEM_HPP
#define _SOUNDSYSTEM_HPP

//=============================================================================
// Included Files
//=============================================================================

#include <radobject.hpp>
#include <raddebug.hpp>
#include <radkey.hpp>

//=============================================================================
// Define Owning Namespace
//=============================================================================

namespace Sound {

class daSoundRenderingManager;

//=============================================================================
// Constants, Definitions and Macros
//=============================================================================

// The sound memory allocator
extern radMemoryAllocator DAMEMORY_ALLOC_SOUND;

// One aux send channels will be used for spacial effects
#define DA_SPACIAL_EFFECT_AUX_SEND 0
#define DA_MISC_AUX_SEND 1

//=============================================================================
// Prototypes
//=============================================================================

struct IDaSoundPlayerState;
struct IDaSoundFadeState;
struct IDaSoundSoundState;

//=============================================================================
// Typedefs and Enumerations
//=============================================================================

//
// This is a sound resource name and key
//
typedef const char* daResourceName;
typedef radKey32 daResourceKey;

//
// These are the sound values
//
typedef float daPitchValue;
typedef float daTrimValue;

//=============================================================================
// Interfaces
//=============================================================================

//
// Sound player state
//
struct IDaSoundPlayerState : public IRefCount
{
    virtual void OnSoundReady( void* pData ) = 0;
    virtual void OnSoundDone( void* pData ) = 0;
};

//
// Fade state
//
struct IDaSoundFadeState : public IRefCount
{
    virtual void OnFadeDone( void* pData ) = 0;
};

//
// A sound object
//
struct IDaSoundObject : public IRefCount
{
    // Left intentionally blank
};

//=============================================================================
// Public Functions
//=============================================================================

//
// Set some global sound flags
//
void daSoundSetSoundOn( bool soundOn );

//
// Work indirectly with the sound manager
//
void daSoundRenderingManagerCreate( radMemoryAllocator allocator );
daSoundRenderingManager* daSoundRenderingManagerGet( void );
void daSoundRenderingManagerTerminate( void );

} // Namespace
#endif //_SOUNDSYSTEM_HPP

