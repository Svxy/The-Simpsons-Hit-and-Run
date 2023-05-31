//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        tutorialmanager.h
//
// Description: this system controls the operation of tutorial mode
//
// History:     19/12/2002 + Created -- Ian Gipson
//
//=============================================================================

#ifndef TUTORIALMANGER_H
#define TUTORIALMANGER_H

//========================================
// Nested Includes
//========================================
#include "events/eventlistener.h"
#include "presentation/tutorialmode.h"
#include <vector>

#include <data/gamedata.h>
#include <memory/stlallocators.h>

//========================================
// Forward References
//========================================

//========================================
// typedefs
//========================================


//=============================================================================
//
// Synopsis:    TutorialManager
//
//=============================================================================

class TutorialManager : public EventListener,
                        public GameDataHandler
{
    public:
        // Static Methods for accessing this singleton.
        static TutorialManager* CreateInstance();
        static TutorialManager* GetInstance();
        static void             DestroyInstance();

        void EnableTutorialMode( const bool enabled );
        bool IsTutorialModeEnabled() const;

        void EnableTutorialEvents( const bool enabled );
        bool IsTutorialEventsEnabled() const;

        virtual void HandleEvent( EventEnum id, void* pEventData );
        void Initialize();
        void MarkDialogFinished();
        void Update( const float deltaT );

        TutorialMode GetCurrentEventID() const;
        bool IsDialogPlaying() const;

        // Implements GameDataHandler
        //
        virtual void LoadData( const GameDataByte* dataBuffer, unsigned int numBytes );
        virtual void SaveData( GameDataByte* dataBuffer, unsigned int numBytes );
        virtual void ResetData();

    protected:
        void AddToQueue( TutorialMode event );
        void ProcessQueue();

        bool QueryTutorialSeen( const TutorialMode tutorial );
        void SetTutorialSeen( const TutorialMode tutorial, const bool seen );

        static TutorialManager* spInstance;
    private:
        TutorialManager();
        virtual ~TutorialManager();

        std::vector< TutorialMode, s2alloc<TutorialMode> > m_Queue;

        bool  m_EnableTutorialMode     : 1; // controlled by scripts
        bool  m_EnableTutorialEvents   : 1; // controlled by user (in pause menu settings)
        bool  m_DialogCurrentlyPlaying : 1;
        bool  m_RaceMissionPlayed      : 1;
        bool  m_BonusMissionPlayed     : 1;
        float m_TimeSinceDialogStart;

        int m_tutorialsSeen; // bit field

};

inline TutorialManager* GetTutorialManager() { return( TutorialManager::GetInstance() ); }

inline void TutorialManager::EnableTutorialMode( const bool enabled )
{
    m_EnableTutorialMode = enabled;
}

inline bool TutorialManager::IsTutorialModeEnabled() const
{
    return m_EnableTutorialMode;
}

inline void TutorialManager::EnableTutorialEvents( const bool enabled )
{
    m_EnableTutorialEvents = enabled;
}

inline bool TutorialManager::IsTutorialEventsEnabled() const
{
    return m_EnableTutorialEvents;
}

inline TutorialMode TutorialManager::GetCurrentEventID() const
{
    return m_Queue.front();
}

inline bool TutorialManager::IsDialogPlaying() const
{
    return m_DialogCurrentlyPlaying;
}

#endif //PRESENTATIONMANAGER_H

