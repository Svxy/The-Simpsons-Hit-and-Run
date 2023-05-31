//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        interiormanager.h
//
// Description: InteriorManager class declaration.
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef INTERIORMANAGER_H
#define INTERIORMANAGER_H

//========================================
// System Includes
//========================================
#include <p3d/p3dtypes.hpp>  // tUID
#include <p3d/anim/animate.hpp> // p3dCycleMode
#include <radmath/radmath.hpp>

//========================================
// Project Includes
//========================================
#include <events/eventlistener.h>
#include <events/eventdata.h>
#include <presentation/presevents/presentationevent.h>
#include <mission/objectives/dialogueobjective.h>

//========================================
// Forward References
//========================================
class NISEvent;
class InteriorEntranceLocator;
class Character;
class Sequencer;
class NISPlayer;
class GagDrawable;
class Gag;
class InteriorExit;
class AnimatedIcon;
class EventLocator;
class SphereTriggerVolume;

//==============================================================================
//
// Synopsis: 
//
//==============================================================================
class InteriorManager : public EventListener
{
    public:

        // Static Methods for accessing this singleton.
        static InteriorManager* CreateInstance();
        static InteriorManager* GetInstance();
        static void DestroyInstance();

        void OnBootupStart();
        void OnGameplayStart();
        void OnGameplayEnd();
        void LoadLevelGags(const rmt::Vector& startPos, bool initial = false);

        void UnloadGagSounds();

        // start an entry (callled from EnterInterior button handler)
        void Enter(InteriorEntranceLocator* entry, Character* character, Sequencer* seq);

        bool IsInside(void) { return mIn || mEntryRequested; }
        bool IsEntering(void) { return mState == ENTER; }
        bool IsExiting(void) { return mState == EXIT; }
        tUID GetInterior(void) { return mCurrentInteriorUID;}

        void Update( unsigned int elapsedTime );

        void CollectionEffect(const char* Name, const rmt::Vector& Pos);

        // Implement EventListener interface
        virtual void HandleEvent( EventEnum id, void* pEventData );

        struct GagBinding
        {
            GagBinding();
            void Clear(void);

            // data for selection of gags
            tUID         interiorUID;
            bool         random;
            int          weight;

            char         gagFileName[13]; // force 8.3 compliance!
            p3dCycleMode cycleMode;
            bool         triggered;
            bool         action;
            bool         retrigger;
            bool         useGagLocator;
            tUID         gagLoc;
            rmt::Vector  gagPos;
            bool         useTriggerLocator;
            tUID         triggerLoc;
            rmt::Vector  triggerPos;
            float        triggerRadius;
            unsigned char i_S_Movie;
            char         gagFMVFileName[ 13 ]; // enforce 8.3 file naming.
            radKey32     soundID;
            bool         cameraShake;
            ShakeEventData shake;
            float        shakeDelay;
            float        shakeDuration;
            float        coinDelay;
            unsigned     coins;
            unsigned     loopIntro;
            unsigned     loopOutro;
            char         dialogChar1[DialogueObjective::MAX_CHAR_NAME_LEN];
            char         dialogChar2[DialogueObjective::MAX_CHAR_NAME_LEN];
            radKey32     acceptDialogID;
            radKey32     rejectDialogID;
            radKey32     instructDialogID;
            bool         sparkle;
            bool         animBV;
            unsigned int loadDist;
            unsigned int unloadDist;
            unsigned int soundLoadDist;
            unsigned int soundUnloadDist;
            int persistIndex;
        };
        
        void ExteriorCharPosn( rmt::Vector& orPosn ){ orPosn = mExitPos; }

        const tName& ClassifyPoint(const rmt::Vector& point);

        void SetISMovieDialogPlaying( bool isPlaying ) { m_isPlayingISDialog = isPlaying; }
        bool IsPlayingISMovieDialog() const { return m_isPlayingISDialog; }

private:
        
        // No public access to these, use singleton interface.
        InteriorManager();
        ~InteriorManager();

        // Declared but not defined to prevent copying and assignment.
        InteriorManager( const InteriorManager& );
        InteriorManager& operator=( const InteriorManager& );

        void OnMissionRestart();

        void LoadGagNIS(tUID uid);
        void ClearGags();
        void AttemptEntry();
        void SwitchToInterior();
        void ExitInterior();
        void SwitchToExterior();
        void SetupLightsAndAnims();

        void ClearGagBindings();

        void AddGagBinding( tUID interiorUID, 
                            char* gagFileName, 
                            p3dCycleMode cycleMode, 
                            int weight,
                            char* gagSound );

        void GagBegin(char* gagFileName);
        GagBinding* GetBuildBinding(void);
        void GagEnd(void);

        // Expose to Console 

        static void ConsoleClearGagBindings( int argc, char** argv );
        static void ConsoleAddGagBinding( int argc, char** argv );
        static void ConsoleGagBegin(int argc, char** argv );
        static void ConsoleGagSetInterior(int argc, char** argv );
        static void ConsoleGagSetCycle(int argc, char** argv );
        static void ConsoleGagSetWeight(int argc, char** argv );
        static void ConsoleGagSetSound(int argc, char** argv );
        static void ConsoleGagSetTrigger(int argc, char** argv );
        static void ConsoleGagSetPosition(int argc, char** argv );
        static void ConsoleGagSetRandom(int argc, char** argv );
        static void ConsoleGagEnd(int argc, char** argv );
        static void ConsoleGagPlayFMV( int argc, char** argv );
        static void ConsoleGagSetIntro( int argc, char** argv );
        static void ConsoleGagSetOutro( int argc, char** argv );
        static void ConsoleGagSetCameraShake( int argc, char** argv );
        static void ConsoleGagSetCoins( int argc, char** argv );
        static void ConsoleGagSetSparkle( int argc, char** argv );
        static void ConsoleGagSetAnimCollision( int argc, char** argv );
        static void ConsoleGagSetLoadDistances( int argc, char** argv );
        static void ConsoleGagSetSoundLoadDistances( int argc, char** argv );
        static void ConsoleGagSetPersist( int argc, char** argv );
        static void ConsoleGagCheckCollCards(int argc, char** argv);
        static void ConsoleGagCheckMovie(int argc, char** argv);

        // Pointer to the one and only instance of this singleton.
        static InteriorManager* spInstance;

        enum State
        {
            NONE,
            ENTER,
            EXIT,
            INSIDE,
            NUM_STATES
        };

        State mState;
        bool mEntryRequested;
        bool mInteriorLoaded;
        bool mLoadedGags;
        tUID mLoadedInteriorUID;
        tUID mCurrentInteriorUID;
        tUID mSection;
        AnimatedIcon* mCollectionEffect;
        bool mIn;

        rmt::Vector mExitPos;
        float mExitFacing;

        void GetExitPos();

        static const int MAX_BINDINGS = 64;
        GagBinding mGagBindings[MAX_BINDINGS];
        int mBindingCount;

        GagBinding mBuildGag;

        static const int MAX_GAGS = 32;
        int mGagCount;
        Gag* gags[MAX_GAGS];

        bool mBuildingGag;

        InteriorExit* mExit;

        tFrameController* mInteriorAnimations;

        static rmt::Randomizer sRandom;
        static bool sRandomSeeded;

        static unsigned sPersistGagIndex;

        bool m_isPlayingISDialog : 1;

        friend class GagDrawable;
        friend class Gag;
};


// A little syntactic sugar for getting at this singleton.
inline InteriorManager* GetInteriorManager() { return( InteriorManager::GetInstance() ); }


#endif // INTERIORMANAGER_H

