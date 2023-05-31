//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        .h
//
// Description: Blahblahblah
//
// History:     22/04/2002 + Created -- NAME
//
//=============================================================================

#ifndef ANIMPLAYER_H
#define ANIMPLAYER_H

//========================================
// Nested Includes
//========================================
#include <p3d/p3dtypes.hpp>
#include <loading/loadingmanager.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Base class for all presentation players.  A player is something
//              that keeps an non-interactive animation going.
//
//=============================================================================

class AnimationPlayer : public LoadingManager::ProcessRequestsCallback
{
    public:
        AnimationPlayer();
        virtual ~AnimationPlayer();


        // Loads the data for this animation and sits on it
        // Set bInInventory to true if the data is already loaded and
        // in the p3d inventory.
        // Clients must implement the callback if they want notification when
        // the load is complete.
        struct LoadDataCallBack
        {
            virtual void OnLoadDataComplete() = 0;
        };
        virtual void LoadData( const char* fileName,
                               LoadDataCallBack* pCallback,
                               bool bInInventory,
                               void* pUserData );

        virtual void LoadData( const char* fileName,
                               bool bInInventory,
                               void* pUserData );

        // Tells it to start playing right away once it's done loading
        void SetPlayAfterLoad( bool bPlay ) { mbPlayAfterLoad = bPlay; }

        void SetShowAlways(bool b) { mbShowAlways = b; }

        // Whee!
	    virtual void Play();
        virtual void Stop();

        // TC: This just calls Stop(), so why do we need another function??
        //
//        void StopAndCleanUp();

        // Tells the player to enter "Exclusive Mode", which sets the
        // Render Level to Presentation and should stop everything else
        // in the game.
        void SetExclusive( bool bIsExclusive ) { mbExclusive = bIsExclusive; }

        bool IsPlaying() { return((( mState != ANIM_IDLE ) && (mState != ANIM_STOPPED ))); }
        bool IsFinished() { return mState == ANIM_STOPPED; }
        virtual void Update( unsigned int elapsedTime ) = 0;
        void Render();

        virtual void ClearData();
        void Reset() { mState = ANIM_IDLE; }

        void OnProcessRequestsComplete( void* pUserData );
        bool GetKeepLayersFrozen( void ) const { return mbKeepLayersFrozen; }
        void SetKeepLayersFrozen( bool IsKeepFrozen ) { mbKeepLayersFrozen = IsKeepFrozen; }
		bool GetSkippable(void) const {return mbIsSkippable;}
		void SetSkippable(bool IsSkippable) {mbIsSkippable = IsSkippable;}

    protected:
        
        enum AnimState
        {
            ANIM_IDLE,
            ANIM_LOADING,
            ANIM_LOADED,
            ANIM_PLAYING,
            ANIM_STOPPED,
            NUM_STATES
        };

        AnimState GetState() { return( mState ); }
        void SetState( AnimState state ) { mState = state; }

        // A sub-class must implement these
        // DoLoaded is called once the data is ready (in the case on p3d
        // stuff, this means it is in the inventory)
        virtual void DoLoaded() = 0;

        // A sub-class should draw stuff here
        virtual void DoRender() = 0;

        void EnterExclusive();
        void LeaveExclusive();
    private:

        //Prevent wasteful constructor creation.
        AnimationPlayer( const AnimationPlayer& animPlayer );
        AnimationPlayer& operator=( const AnimationPlayer& animPlayer );

        AnimState mState;

		bool mbPlayAfterLoad : 1;
		bool mbExclusive : 1;
		bool mbShowAlways : 1;
		bool mbKeepLayersFrozen : 1;
		bool mbIsSkippable : 1;

        tUID mSection;

        LoadDataCallBack* mpLoadDataCallback;
};


#endif //ANIMPLAYER_H_H

