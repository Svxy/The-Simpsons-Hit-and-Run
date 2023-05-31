//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        .h
//
// Description: Blahblahblah
//
// History:     02/05/2002 + Created -- NAME
//
//=============================================================================

#ifndef TRANSITIONPLAYER_H
#define TRANSITIONPLAYER_H

//========================================
// Nested Includes
//========================================

#include <presentation/animplayer.h>

#include <render/enums/renderenums.h>

//========================================
// Forward References
//========================================

class RenderLayer;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class TransitionPlayer : public AnimationPlayer
{
    public:
        TransitionPlayer();
        virtual ~TransitionPlayer();

        enum TransitionType
        {
            TRANS_CROSSFADE,
            TRANS_WIPE_RIGHT,
            TRANS_TO_BLACK,
            TRANS_FROM_BLACK,
            NUM_TRANS
        };

        struct TransitionInfo
        {
            RenderEnums::LayerEnum layer1;
            RenderEnums::LayerEnum layer2;
            unsigned int length;
            TransitionType type;
        };

        void SetTransition( TransitionInfo* info );

        virtual void Update( unsigned int elapsedTime );
    protected:
        virtual void DoUpdate( unsigned int elapsedTime );

        virtual void DoLoaded() {};
        virtual void DoRender();
        
    private:

        //Prevent wasteful constructor creation.
        TransitionPlayer( const TransitionPlayer& transitionPlayer );
        TransitionPlayer& operator=( const TransitionPlayer& transitionPlayer );

        RenderLayer* mpLayer1;
        RenderLayer* mpLayer2;

        TransitionInfo msInfo;
        unsigned int miIndex;
};


#endif //TRANSITIONPLAYER_H

