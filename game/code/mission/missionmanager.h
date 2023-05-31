//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        missionmanager.h
//
// Description: Blahblahblah
//
// History:     15/04/2002 + Created -- NAME
//
//=============================================================================

#ifndef MISSIONMANAGER_H
#define MISSIONMANAGER_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>

#include <mission/gameplaymanager.h>

//========================================
// Forward References
//========================================
class AnimatedIcon;

//=============================================================================
//
// Synopsis:    Loads two missions and keeps one active while the other is
//              held in reserve.  Takes the player through all the missions
//              in a level until the level is complete.
//
//=============================================================================

class MissionManager : public GameplayManager,
                       public LoadingManager::ProcessRequestsCallback
{
    public:

        enum LoadingStateEnum {
            STATE_INVALID,
            STATE_LEVEL,
            STATE_MISSION_LOAD,
            STATE_MISSION_LOADING,
            STATE_MISSION_INIT,
            STATE_MISSION_INITING,
            STATE_MISSION_DYNALOAD,
            STATE_WAIT_FOR_DYNALOAD,
            STATE_MISSION_START,
            NUM_STATES
        };



        static MissionManager* GetInstance();
        static MissionManager* CreateInstance();
        static void DestroyInstance();

        virtual void Initialize();
        virtual void Finalize();
        virtual void Reset();

        virtual void LoadLevelData();
        virtual void InitLevelData();
        virtual void CleanMissionData();

        virtual void OnProcessRequestsComplete( void* pUserData );

        virtual void Update( int elapsedTime );

        virtual void HandleEvent( EventEnum id, void* pEventData );

        virtual void PerformLoading();

        virtual bool IsSundayDrive() { return GetCurrentMission() ? GetCurrentMission()->IsSundayDrive() : true; };

        virtual void RestartCurrentMission();
        virtual void RestartToMission( RenderEnums::MissionEnum mission );
        virtual void AbortCurrentMission();

        void PutEffectHere( const rmt::Vector& pos );
        bool InResetState(){ return mResetting; };

        LoadingStateEnum GetLoadingState () {   return mLoadingState; };


        

protected:
        MissionManager();
        ~MissionManager();

        virtual void LoadMission();
    private:    
        //Prevent wasteful constructor creation.
        MissionManager( const MissionManager& missionManager  );
        MissionManager& operator=( const MissionManager& missionManager );

        // Pointer to the one and only instance of this singleton.
        static MissionManager* spInstance;        


        LoadingStateEnum mLoadingState;

        enum MissionStateEnum {
            MISSION_INVALID,
            MISSION_LOADING,
            MISSION_INIT,
            MISSION_SUSPEND,
            MISSION_RUNNING,
            MISSION_NUM_STATES
        };

        MissionStateEnum mMissionState;

        char mLastFileName[256];

        bool mIsSundayDrive;

        bool mResetting;

        //HACK
        bool mHAHACK;

        AnimatedIcon* mCollectionEffect;
};

inline MissionManager* GetMissionManager() { return( MissionManager::GetInstance() ); }

#endif //MISSIONMANAGER_H
