//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        profiler.h
//
// Description: In-game profiler.
//
// History:     5/8/2002 + Migrated from SRR -- Darwin Chau
//
//=============================================================================

#ifndef PROFILER_H
#define PROFILER_H

//========================================
// Nested Includes
//========================================

//========================================
// Forward References
//========================================

#if !defined( RAD_RELEASE ) && !defined( WORLD_BUILDER ) && !defined( RAD_MW ) && !defined( RAD_WIN32 )
    #define PROFILER_ENABLED
#endif // RAD_RELEASE


#include "main/commandlineoptions.h"

#include <p3d/entity.hpp>
#include <radload/utility/hashtable.hpp>

//===========================================================================
// *** USE THESE MACROS, DO NOT INVOKE PROFILER DIRECTLY ***
//===========================================================================


#ifndef PROFILER_ENABLED

    #define CREATE_PROFILER()     
    #define DESTROY_PROFILER()    

    #define BEGIN_PROFILE(string)
    #define END_PROFILE(string)

    #define BEGIN_PROFILER_FRAME()
    #define END_PROFILER_FRAME()

	#define RENDER_PROFILER()

    #define SNSTART(id, txt)
    #define SNSTOP(id)

#else

#ifdef SNTUNER
#include <SNTuner.h>
    #define SNSTART(id, txt)  snStartMarker(id, txt)
    #define SNSTOP(id)   snStopMarker(id);
#endif

    #define CREATE_PROFILER()       if(!(CommandLineOptions::Get(CLO_DESIGNER) || CommandLineOptions::Get(CLO_FIREWIRE))) { Profiler::CreateInstance(); }
    #define DESTROY_PROFILER()      if(!(CommandLineOptions::Get(CLO_DESIGNER) || CommandLineOptions::Get(CLO_FIREWIRE))) { Profiler::DestroyInstance(); }

    #define BEGIN_PROFILE(string)   if(!(CommandLineOptions::Get(CLO_DESIGNER) || CommandLineOptions::Get(CLO_FIREWIRE))) { GetProfiler()->BeginProfile(string); }
    #define END_PROFILE(string)     if(!(CommandLineOptions::Get(CLO_DESIGNER) || CommandLineOptions::Get(CLO_FIREWIRE))) { GetProfiler()->EndProfile(string); }

    #define BEGIN_PROFILER_FRAME()  if(!(CommandLineOptions::Get(CLO_DESIGNER) || CommandLineOptions::Get(CLO_FIREWIRE))) { GetProfiler()->BeginFrame(); }
    #define END_PROFILER_FRAME()    if(!(CommandLineOptions::Get(CLO_DESIGNER) || CommandLineOptions::Get(CLO_FIREWIRE))) { GetProfiler()->EndFrame(); }

	#define RENDER_PROFILER()		if(!(CommandLineOptions::Get(CLO_DESIGNER) || CommandLineOptions::Get(CLO_FIREWIRE))) { GetProfiler()->Render(); }


#define MAX_PROFILER_DEPTH  32

//===========================================================================
//
// Description: Generally, describe what behaviour this class possesses that
//              clients can rely on, and the actions that this service
//              guarantees to clients.
//
// Constraints: Describe here what you require of the client which uses or
//              has this class - essentially, the converse of the description
//              above. A constraint is an expression of some semantic
//              condition that must be preserved, an invariant of a class or
//              relationship that must be preserved while the system is in a
//              steady state.
//
//===========================================================================
class Profiler
{
    public:
        
        static Profiler* CreateInstance();
        static Profiler* GetInstance();
        static void DestroyInstance();

        void Init();

        void BeginProfile( const char* name );
        void EndProfile( const char* name );
        
        void BeginFrame();
        void EndFrame();

        void ToggleDisplay() { sDisplay = !sDisplay; }
        void NextPage();

        void Render();

        //
        // Watcher tunable
        //
        static int sRed;
        static int sGreen;
        static int sBlue;
        static int sPage;
        static int sLeftOffset;
        static int sTopOffset;
        static bool sDisplay;
        static bool sDumpToOutput;

    private:


        Profiler();
        ~Profiler();


        void StoreProfileHistory( tUID nameUID,
                                  float percent,
                                  float elapsedTime, 
                                  float sampleTime, 
                                  float totalTime );
        
        void GetProfileHistory( tUID nameUID, 
                                float* ave, 
                                float* min, 
                                float* max , 
                                float* sample, 
                                float* total );
        struct ProfileSample 
        {
           bool bValid;                         // Whether the data is valid
           tUID uid;                            // Hashed name for comparisons.
           unsigned int iProfileInstances;      // # of times ProfileBegin called
           float        fStartTime;            // The current open profile start time
           float        fAccumulator;          // All samples this frame added together
           float        fChildrenSampleTime;   // Time taken by all children
           float        fSampleTime;           // Time for a single pass  
           float        fTotalTime;            // Time for all passess 
           unsigned int iNumParents;            // Number of profile parents
           bool         isOpen;                 // Is the profile section open?
           char         szName[256];            // Name of sample
        };

        struct ProfileSampleHistory 
        {
           tUID uid;           //Hased name for comparisons
           float fAve;         //Average time per frame (percentage)
           float fMin;         //Minimum time per frame (percentage)
           float fMax;         //Maximum time per frame (percentage)
           float fSampleAve;
           float fSampleTotal;
        };

        static Profiler* spInstance;

        ProfileSample        *AllocSample(void);
        ProfileSampleHistory *AllocHistory(void);

        enum
        {
            NUM_SAMPLES = 256, // Must be power of two for hash table
            NUM_VISIBLE_LINES = 15,
            NUM_VISIBLE_PANES = 1 + ((NUM_SAMPLES-1) / NUM_VISIBLE_LINES )
        };

        HashTable<ProfileSample>        *mOpenSampleStore;
        HashTable<ProfileSampleHistory> *mOpenHistoryStore;

        ProfileSample mSamples[NUM_SAMPLES];
        ProfileSampleHistory mHistory[NUM_SAMPLES];

        unsigned int mOpenStack[MAX_PROFILER_DEPTH];
        int          mOpenStackTop;

        float mStartProfile;
        float mEndProfile;
        
        float mFrameRate;
        float mFrameRateAdjusted;
        
        float mDisplayTime;
        unsigned int mNextSampleAllocIndex;
        unsigned int mNextHistoryAllocIndex;

        static bool sEnableCollection;
};

// A little syntactic sugar for getting at this singleton.
inline Profiler* GetProfiler() { return( Profiler::GetInstance() ); }

#endif // PROFILER_ENABLED

#endif // PROFILER_H