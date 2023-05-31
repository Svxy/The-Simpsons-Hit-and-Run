//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        musicsoundplayer.hpp
//
// Subsystem:   Dark Angel - Sound players
//
// Description: Used to hack in volume control for RadMusic, even though
//              it doesn't go through the DA system
//
// Revisions:
//  + Hacked 5 Mar 03 -- Esan
//
//=============================================================================

#ifndef _MUSICSOUNDPLAYER_HPP
#define _MUSICSOUNDPLAYER_HPP

//=============================================================================
// Included Files
//=============================================================================

#include <sound/soundrenderer/soundplayer.h>

//=============================================================================
// Define Owning Namespace
//=============================================================================

namespace Sound {

//=============================================================================
// Prototypes
//=============================================================================

//=============================================================================
// Forward declarations
//=============================================================================

//=============================================================================
// Typdefs and enumerations
//=============================================================================

//=============================================================================
// Class Declarations
//=============================================================================

//
// This contains a DA player instance.
//
class MusicSoundPlayer : public daSoundPlayerBase
{
public:
    //
    // Constructor and destructor
    //
    MusicSoundPlayer ( );
    virtual ~MusicSoundPlayer ( );

    void Initialize ( bool isMusic );  

    // Suspend the player if it should be playing, but no one can hear it
    void Suspend ( void );

    void SetExternalTrim( float newTrim );
    void SetGroupTrim( float newTrim );
    void SetFaderGroupTrim( float newTrim );
    void SetMasterTrim( float newTrim );

    daSoundGroup GetSoundGroup ( void );

    unsigned int GetPlaybackTimeInSamples ( void );

    // daSoundPlayerBase
    
    virtual void ServiceOncePerFrame( void ) { }
    virtual bool IsCaptured ( void ) { return true; }
    virtual void Pause ( void ) { }
    virtual bool IsPaused( void ) { return false; }
    virtual void Continue ( void ) { }
    virtual void UberContinue( void ) { }
    virtual void Stop ( void ) { }
    virtual void SetPitch( float pitch ) { }
    
    virtual void ChangeTrim( daSoundGroup groupName, float newTrim ); 
    virtual void ChangeFaderTrim( daSoundGroup groupName, float newTrim );


private:
    void ResetMusicTrim( );

    bool m_isMusic;
    
    //
    // Trim values
    //
    float m_PlayerTrim;
    float m_ResourceTrim;
    float m_ExternalTrim;
    float m_GroupTrim;
    float m_FaderGroupTrim;
    float m_MasterTrim;  
      
};


//=============================================================================
// Public Functions
//=============================================================================

} // Sound Namespace
#endif //_MUSICSOUNDPLAYER_HPP

