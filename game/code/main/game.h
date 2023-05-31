//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        game.h
//
// Description: The game loop
//
// History:     + Stolen and cleaned up from Svxy -- Darwin Chau
//
//=============================================================================

#ifndef GAME_H
#define GAME_H

//========================================
// Forward References
//========================================
class Platform;
struct IRadTimerList;
class GameFlow;
class RenderFlow;

#include <radtime.hpp>

class DemoProfiler
{
public:
    DemoProfiler(unsigned maxFrames);

    void AddChannel(unsigned c, const char* name);

    void NextFrame();

    void Start(unsigned c);  // start timing channel c
    void Stop(unsigned c);   // stop timing channel c
    void Set(unsigned c, unsigned val);  // set sample value for channel c
    unsigned GetSample(unsigned c);
    unsigned GetCurrentFrame();
    void Accumulate(unsigned c, unsigned val);  // add value to sample for channel c
    void Dump();  // print out all samples

    void StartRecording();
    bool IsRecording();
    enum AlertStatus { PROFILER_ALERT_GREEN, PROFILER_ALERT_YELLOW, PROFILER_ALERT_RED };
    AlertStatus GetAlertStatus();

private:    
    enum { MAX_CHANNEL = 64 };

    struct Channel
    {
        char name[255];
        unsigned* samples;
        radTime64 t0;
    };

    bool recording;
    unsigned maxFrames;

    Channel* channel[MAX_CHANNEL];
    unsigned nChannel;
    unsigned currentFrame;

    AlertStatus alertStatus;
    unsigned numFramesBelow_20;
    unsigned numFramesBetween_20_30;
    unsigned numFramesBetween_30_40;
    unsigned numFramesAbove_40;
};

extern DemoProfiler g_DemoProfiler;

//=============================================================================
//
// Synopsis:    The game loop
//
//=============================================================================
class Game
{
    public:

        // Static Methods (for creating and getting an instance of the game)
        static Game* CreateInstance( Platform* platform );
        static void  DestroyInstance();
        static Game* GetInstance();
        
        Platform* GetPlatform();


        // Game Flow Public Methods
        void Initialize();
        void Terminate();

        void Run();
        void Stop();
        
        IRadTimerList* GetTimerList() { return mpTimerList; }

        unsigned int GetFrameCount() const { return mFrameCount; };

        unsigned int GetDemoCount() const { return mDemoCount; };
        void IncrementDemoCount() { ++mDemoCount; };
        void SetTime( unsigned int timeMS ) { mTimeMS = timeMS; };
        unsigned int GetTime() { return mTimeMS; };

        static unsigned GetRandomSeed ();

    private:

        // Constructors, Destructors, and Operators
        Game( Platform* platform );
        virtual ~Game();

        // Unused Constructors, Destructors, and Operators
        Game();
        Game( const Game& aGame );
        Game& operator=( const Game& aGame );

        // Static Singleton Attribute
        static Game*    spInstance;

        // Private Attributes
        Platform*       mpPlatform;
        IRadTimerList*  mpTimerList;
        GameFlow*       mpGameFlow;
        RenderFlow*     mpRenderFlow;
        
        unsigned int mFrameCount;

        bool        mExitNow;

        unsigned int mDemoCount;
        unsigned int mTimeMS;
};

inline Game* GetGame() { return( Game::GetInstance() ); }

#endif // GAME_H

