//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        .h
//
// Description: Blahblahblah
//
// History:     16/04/2002 + Created -- NAME
//
//=============================================================================

#ifndef PRESENTATIONMANAGER_H
#define PRESENTATIONMANAGER_H

//========================================
// Nested Includes
//========================================

#include <radmath/radmath.hpp>
#include <p3d/entity.hpp>
#include <p3d/drawable.hpp>
#include <events/eventlistener.h>

#include <memory/allocpool.h>
#include <memory/stlallocators.h>
//#include <presentation/language.h>
#include <presentation/presevents/fmvevent.h>
#include <presentation/presevents/nisevent.h>
#include <presentation/presevents/presentationevent.h>
#include <presentation/presevents/transevent.h>

#include <render/enums/renderenums.h>
#include <vector>

//========================================
// Forward References
//========================================
class Character;
class NISPlayer;
class FMVPlayer;
class CameraPlayer;
class TransitionPlayer;
class PlayerDrawable;
class SuperCam;
class PresentationAnimator;

//========================================
// typedefs
//========================================

typedef AllocPool< FMVEvent > FMVEventPool;
typedef AllocPool< NISEvent > NISEventPool;
typedef AllocPool< TransitionEvent > TransitionEventPool;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

/*=============================================================================
 This drawable is used to a poly over the camera to fade out the scene, etc.
This was being done by the frontend, but it relies on a scrooby project being
loaded and while playing a movie during game play we don't have that.
 This could also be used for putting a texture over the screen, but that's not
implemented at this time.
=============================================================================*/
const tColour BLACK( 0x00, 0x00, 0x00, 0xFF );
const tColour WHITE( 0xFF, 0xFF, 0xFF, 0xFF );
const tColour BLACK_TRANSPARENT( 0x00, 0x00, 0x00, 0x00 );
const tColour WHITE_TRANSPARENT( 0xFF, 0xFF, 0xFF, 0x00 );
class PresentationOverlay : public tDrawable
{
public:
    PresentationOverlay();

    virtual void Display();
    void Update( unsigned int ElapsedTime );

    void SetStart( tColour Colour ) { mStart = Colour; }
    void SetEnd( tColour Colour ) { mEnd = Colour; }
    // Note setting the duration begins the fading from one colour to another.
    void SetDuration( float Duration ) { mInvDuration = 0.001f / Duration; mAlpha = 1.0f; mFrameCount = -1; }
    void SetFrames( int FrameCount ) { mFrameCount = FrameCount; } // Hold start colour for this many frames.
    float GetAlpha( void ) const { return mAlpha; }
    void SetRemoveOnComplete( bool AutoRemove ) { mIsAutoRemove = AutoRemove; }

protected:
    float mAlpha;
    float mInvDuration;
    tColour mStart;
    tColour mEnd;
    int mFrameCount;
    bool mIsAutoRemove : 1;
};

class PresentationManager : public EventListener,
                            public PresentationEvent::PresentationEventCallBack,
                            public LoadingManager::ProcessRequestsCallback
{
    public:
        // Static Methods for accessing this singleton.
        static PresentationManager* CreateInstance();
        static PresentationManager* GetInstance();
        static void DestroyInstance();

        void Initialize();
        void Finalize();
        void OnGameplayStart();
        void OnGameplayStop();

        void InitializePlayerDrawable();
        void FinalizePlayerDrawable();

		// Free frontend, play movie, reload frontend.
        void PlayFMV( const char* FileName, PresentationEvent::PresentationEventCallBack* pCallback = 0,
                      bool IsSkippable = true,
                      bool StopMusic = false,
                      bool IsLocalized = true );

        // Creates an event and passes it back.  Adds this event to the queue.
        void QueueFMV( FMVEvent** pFMVEvent, 
                       PresentationEvent::PresentationEventCallBack* pCallback );

        // Creates an event and passes it back.  Adds this event to the queue.
	    void QueueNIS( NISEvent** pNISEvent, 
                       PresentationEvent::PresentationEventCallBack* pCallback );

        // Creates an event and passes it back.  Adds this event to the queue.
	    void QueueTransition( TransitionEvent** pTransitionEvent, 
                              PresentationEvent::PresentationEventCallBack* pCallback );

        // Scales the p3d stack by 11.9
        void ClearQueue();
        bool IsQueueEmpty() { return( mEventFIFO[ mFIFOBegin ] == NULL ); }
		bool IsBusy(void) const;

	    void Update( unsigned int elapsedTime );

        FMVPlayer* GetFMVPlayer() { return( mpFMVPlayer ); }
        NISPlayer* GetNISPlayer() { return( mpNISPlayer ); }
        TransitionPlayer* GetTransPlayer() { return( mpTransitionPlayer ); }
        CameraPlayer* GetCameraPlayer() {return( mpCameraPlayer ); }

        virtual void HandleEvent( EventEnum id, void* pEventData );
        PresentationAnimator* GetAnimatorNpc();
        PresentationAnimator* GetAnimatorPc();
        typedef std::vector< tName, s2alloc<tName> > TNAMEVECTOR;
        void SetCamerasForLineOfDialog( const TNAMEVECTOR& names );
        bool InConversation() const;
        void StopAll();
        void CheckRaceMissionBitmaps();
        void ReplaceMissionBriefingBitmap( const tName& conversationCharacterName );
        void MakeCharactersFaceEachOther( Character* c0, Character* c1 );

    protected:
        void AddToQueue( PresentationEvent* pEvent );
        const tName GetCameraTargetForLineOfDialog( const unsigned int lineOfDialog ) const;
        void ReturnToPool( PresentationEvent* presevent );

        virtual void OnPresentationEventBegin( PresentationEvent* pEvent );
        virtual void OnPresentationEventLoadComplete( PresentationEvent* pEvent );
        virtual void OnPresentationEventEnd( PresentationEvent* pEvent );
        virtual void OnProcessRequestsComplete( void* pUserData );

        // Gets the first event in the queue
        PresentationEvent* GetFirst();
    private:
        PresentationManager();
        virtual ~PresentationManager();

        //Prevent wasteful constructor creation.
        PresentationManager( const PresentationManager& presentationManager );
        PresentationManager& operator=( const PresentationManager& presentationManager );

        // Pointer to the one and only instance of this singleton.
        static PresentationManager* spInstance;

        TransitionEventPool* mTransitionPool;
        NISEventPool* mNISPool;
        FMVEventPool* mFMVPool;

        SuperCam* mp_oldcam;
        unsigned int mOldCamIndexNum;


        // Very simple implementation of a queue.  I don't actually
        // even know if it works.
        static const unsigned int MAX_EVENT_SIZE = 10;
        PresentationEvent* mEventFIFO[ MAX_EVENT_SIZE ];
        unsigned int mFIFOBegin;
        unsigned int mFIFOEnd;

        PresentationEvent* mpCurrent;

        FMVPlayer* mpFMVPlayer;
        NISPlayer* mpNISPlayer;
        CameraPlayer* mpCameraPlayer;
        TransitionPlayer* mpTransitionPlayer;

        PlayerDrawable* mpPlayerDrawable;
        PresentationAnimator* mp_PCAnimator;
        PresentationAnimator* mp_NPCAnimator;
//        Language::Language mLanguage;
        int mDialogLineNumber;
        TNAMEVECTOR mCameraForLineOfDialog;
        PresentationEvent::PresentationEventCallBack* mpPlayCallback;
        bool mInConversation : 1;
        bool mWaitingOnFade  : 1;
        PresentationOverlay* mOverlay;
};

// A little syntactic sugar for getting at this singleton.
inline PresentationManager* GetPresentationManager() { return( PresentationManager::GetInstance() ); }

#endif //PRESENTATIONMANAGER_H

