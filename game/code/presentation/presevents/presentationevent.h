//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        .h
//
// Description: Blahblahblah
//
// History:     22/05/2002 + Created -- NAME
//
//=============================================================================

#ifndef PRESENTATIONEVENT_H
#define PRESENTATIONEVENT_H

//========================================
// Nested Includes
//========================================

#include <events/eventenum.h>

#include <presentation/animplayer.h>

#include <render/enums/renderenums.h>

//========================================
// Forward References
//========================================

class AnimationPlayer;

//=============================================================================
//
// Synopsis:    These classes make it easier for the PresentationManager to 
//              queue lots of animations for use on the different players.
//              Keep in mind that the PresentationManager is keeping a bunch
//              of these in a pool so you MUST call Init to reset all the
//              members to their default values.
//
//=============================================================================

class PresentationEvent : public AnimationPlayer::LoadDataCallBack
{
    public:
        PresentationEvent();
        virtual ~PresentationEvent();

        // Implement AnimationPlayer::LoadDataCallBack
        // 
        virtual void OnLoadDataComplete();

        struct PresentationEventCallBack
        {
            virtual void OnPresentationEventBegin( PresentationEvent* pEvent ) = 0;
            virtual void OnPresentationEventLoadComplete( PresentationEvent* pEvent ) = 0;
            virtual void OnPresentationEventEnd( PresentationEvent* pEvent ) = 0;
        };

        PresentationEventCallBack* pCallback;
        char fileName[64];
        bool bInInventory;

        // set ClearWhenDone to true to clear all data when this event finishes
        // or false to leave stuff (like the MoviePlayer) allocated. Default = true
        void SetClearWhenDone( bool bClear ) { mbClearWhenDone = bClear; }

        // set AutoPlay to true to play animation immediately after loading finsihes
        // Default = true
        void SetAutoPlay( bool bAutoPlay ) { mbAutoPlay = bAutoPlay; }
        bool GetAutoPlay() { return mbAutoPlay; }
        void SetKeepLayersFrozen( bool IsKeep ) { mbKeepLayersFrozen = IsKeep; }
        bool GetKeepLayersFrozen( void ) const { return mbKeepLayersFrozen; }
		void SetSkippable(bool IsSkippable) {mbIsSkippable = IsSkippable;}
		bool IsSkippable(void) const {return mbIsSkippable;}

        void SetRenderLayer( RenderEnums::LayerEnum layer ) { mRenderLayer = layer; }
        RenderEnums::LayerEnum GetRenderLayer() { return mRenderLayer; }

        virtual AnimationPlayer* GetPlayer() = 0;

        bool Update( unsigned int elapsedTime );

        virtual void Start();
        virtual void Stop();
        
        virtual void Init();

    protected:
        void SetLoaded( bool bLoaded ) { mbLoaded = bLoaded; }

        virtual void* GetUserData () { return 0; }

    private:

        //Prevent wasteful constructor creation.
        PresentationEvent( const PresentationEvent& presentationEvent );
        PresentationEvent& operator=( const PresentationEvent& presentationEvent );

		bool mbAutoPlay : 1;
		bool mbClearWhenDone : 1;
		bool mbLoaded : 1;
		bool mbKeepLayersFrozen : 1;
		bool mbIsSkippable : 1;
        RenderEnums::LayerEnum mRenderLayer;
};

#endif // PRESENTATIONEVENT_H

