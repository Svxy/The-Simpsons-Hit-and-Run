//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        idasoundresource.hpp
//
// Subsystem:   Dark Angel - Sound resources
//
// Description: Defines the interface for a sound resource
//
// WARNING:     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//              IF THIS FILE IS MODIFIED THE TYPE INFO FILE MUST BE REGENERATED
//              !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// Revisions:
//  + Created October 3, 2001 -- aking
//
//=============================================================================

#ifndef _IDASOUNDRESOURCE_HPP
#define _IDASOUNDRESOURCE_HPP

//=============================================================================
// Included Files
//=============================================================================

#include <radobject.hpp>

#include <sound/soundrenderer/dasoundgroup.h>

//=============================================================================
// Definitions and macros
//=============================================================================

//
// This is the maximum number of file variations that can exist in a resource.
//
#define DASound_MaxNumSoundResourceFiles 7

//=============================================================================
// Prototypes
//=============================================================================

struct IDaSoundResource;

//=============================================================================
// Typedefs and enumerations
//=============================================================================

//
// A procedure used to modify a sound resource
//
typedef void daSoundResourceProc( IDaSoundResource* pRes, void* pUserData );

//=============================================================================
// Interface Definitions
//=============================================================================

//
// IDaSoundResourceData contains sound resource data.
//
struct IDaSoundResourceData :  public IRefCount
{
    // SCRIPTED FEATURES THAT CAN BE TUNED IN REAL TIME

    //
    // Add files to the resource
    //
    virtual void AddFilename
    (
        const char* newFileName,
        float trim
    ) = 0;

    //
    // Set the pitch variation
    //
    virtual void SetPitchRange
    (
        float minPitch,
        float maxPitch
    ) = 0;
    
    //
    // Set the trim variation
    //
    virtual void SetTrimRange
    (
        float minTrim,
        float maxTrim
    ) = 0;

    //
    // Set the trim to one value
    //
    virtual void SetTrim( float trim ) = 0;
    
    // SCRIPTED FEATURES THAT CAN NOT BE TUNED IN REAL TIME

    // Is this a streaming sound resource?
    virtual void SetStreaming( bool streaming ) = 0;

    // Is this a looping sound resource?
    virtual void SetLooping( bool looping ) = 0;

    // SCRIPTED FEATURES AVAILABLE ONLY FOR TUNERS

    // Play the resource
    virtual void Play( void ) = 0;
};

//
// IDaSoundResource is based on an IDaSoundResourceData.  It adds functionality
// to get parameters, and to capture/release resources.
//
struct IDaSoundResource : public IDaSoundResourceData
{
    // A resource file description
    struct daSoundResourceFileDesc
    {
        char*   m_Filename;
        float   m_Trim;
        int     m_TableIndex;
    };

    //
    // Monitor files in the resource
    //
    virtual unsigned int GetNumFiles( void ) = 0;

    virtual void GetFileNameAt( unsigned int index, char* buffer, unsigned int max ) = 0;
    virtual void GetFileKeyAt( unsigned int index, char* buffer, unsigned int max ) = 0;
    //
    // Get parameters set in IDaSoundResourceData
    //
    virtual void GetPitchRange
    (
        float* pMinPitch,
        float* pMaxPitch
    ) = 0;
    virtual void GetTrimRange
    (
        float* pMinTrim,
        float* pMaxTrim
    ) = 0;
    virtual bool GetLooping( void ) = 0;

    //
    // Find out what kind of resource this is
    //
    enum Type {
        UNKNOWN,
        CLIP,
        STREAM
    };
    virtual Type GetType( void ) = 0;

    //
    // Modify the sound group that this resource belongs to
    //
    virtual void SetSoundGroup( Sound::daSoundGroup soundGroup ) = 0;
    virtual Sound::daSoundGroup GetSoundGroup( void ) = 0;

    //
    // Capture and release the resource
    //
    virtual void CaptureResource( void ) = 0;
    virtual bool IsCaptured( void ) = 0;
    virtual void ReleaseResource( void ) = 0;
};

//}; //Sound Namespace
#endif // _IDASOUNDRESOURCE_HPP
