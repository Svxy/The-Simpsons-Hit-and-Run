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

#ifndef PlayerDRAWABLE_H
#define PlayerDRAWABLE_H

//========================================
// Nested Includes
//========================================
#include <p3d/drawable.hpp>
#include <render/enums/renderenums.h>

//========================================
// Forward References
//========================================

class AnimationPlayer;

//=============================================================================
//
// Synopsis:    The PlayerDrawable is just a temporary thing for the DSG to
//              call and get the current presentation player to draw.
//
//=============================================================================

class PlayerDrawable : public tDrawable
{
    public:
        PlayerDrawable();
        virtual ~PlayerDrawable();

        void SetPlayer( AnimationPlayer* pPlayer ) { mpPlayer = pPlayer; }
        virtual void Display();

        void SetRenderLayer( RenderEnums::LayerEnum layer ) { mRenderLayer = layer; }
        RenderEnums::LayerEnum GetRenderLayer() { return mRenderLayer; }

    private:

        //Prevent wasteful constructor creation.
        PlayerDrawable( const PlayerDrawable& pPlayerDrawable );
        PlayerDrawable& operator=( const PlayerDrawable& pPlayerDrawable );

        AnimationPlayer* mpPlayer;
        RenderEnums::LayerEnum mRenderLayer;
};


#endif // PlayerDRAWABLE_H

