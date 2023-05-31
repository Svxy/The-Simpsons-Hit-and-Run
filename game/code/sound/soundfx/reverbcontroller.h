//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        reverbcontroller.h
//
// Description: Declaration for the ReverbController class, which turns reverb
//              on and off and applies the required control values.
//
// History:     10/28/2002 + Created -- Darren
//
//=============================================================================

#ifndef REVERBCONTROLLER_H
#define REVERBCONTROLLER_H

//========================================
// Nested Includes
//========================================
#include <events/eventlistener.h>

//========================================
// Forward References
//========================================

struct IRadSoundHalEffect;
class reverbSettings;

//=============================================================================
//
// Synopsis:    ReverbController
//
//=============================================================================

class ReverbController : public EventListener
{
    public:
        ReverbController();
        virtual ~ReverbController();

        void SetReverbGain( float gain );

        virtual void SetReverbOn( reverbSettings* settings ) = 0;
        virtual void SetReverbOff() = 0;

        virtual void PauseReverb();
        virtual void UnpauseReverb();

        //
        // EventListener interface
        //
        void HandleEvent( EventEnum id, void* pEventData );

        void ServiceOncePerFrame( unsigned int elapsedTime );

    protected:
        static const int REVERB_AUX_EFFECT_NUMBER = 0;

        void registerReverbEffect( IRadSoundHalEffect* reverbEffect );
        void prepareFadeSettings( float targetGain, float fadeInTime, float fadeOutTime );
        void startFadeOut();

    private:
        //Prevent wasteful constructor creation.
        ReverbController( const ReverbController& original );
        ReverbController& operator=( const ReverbController& rhs );

        reverbSettings* getReverbSettings( const char* objName );

        //
        // Used for gradual changes in reverb
        //
        float m_targetGain;
        float m_currentGain;
        float m_fadeInMultiplier;
        float m_fadeOutMultiplier;

        reverbSettings* m_lastReverb;
        reverbSettings* m_queuedReverb;
};


#endif // REVERBCONTROLLER_H

