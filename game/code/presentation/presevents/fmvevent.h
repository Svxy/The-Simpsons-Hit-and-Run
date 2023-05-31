//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        .h
//
// Description: Blahblahblah
//
// History:     23/05/2002 + Created -- NAME
//
//=============================================================================

#ifndef FMVEVENT_H
#define FMVEVENT_H

//========================================
// Nested Includes
//========================================

#include <presentation/presevents/presentationevent.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class FMVEvent : public PresentationEvent
{
    public:
        FMVEvent ();
        virtual ~FMVEvent ();

        AnimationPlayer* GetPlayer();
        void SetAudioIndex(unsigned int idx) { mData.AudioIndex = idx; }
        void SetAllocator( GameMemoryAllocator Alloc ) { mData.Allocator = Alloc; }
        void KillMusic() { mData.KillMusic = true; }

        struct FMVEventData
        {
            FMVEventData();
            unsigned int AudioIndex;
            GameMemoryAllocator Allocator;
            bool KillMusic;
        };

        static const unsigned int AUDIO_INDEX_ENGLISH = 0;
        static const unsigned int AUDIO_INDEX_FRENCH = 1;
        static const unsigned int AUDIO_INDEX_GERMAN = 2;
        static const unsigned int AUDIO_INDEX_SPANISH = 3;

    protected:
        void* GetUserData () { return reinterpret_cast<void*>(&mData); }

        FMVEventData mData;
};

#endif //FMVEVENT_H

