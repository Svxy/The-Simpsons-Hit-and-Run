//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        .h
//
// Description: Blahblahblah
//
// History:     29/05/2002 + Created -- NAME
//
//=============================================================================

#ifndef SIMPLEANIMATIONPLAYER_H
#define SIMPLEANIMATIONPLAYER_H

//========================================
// Nested Includes
//========================================
#include <p3d/anim/animate.hpp> // p3dCycleMode
#include <presentation/animplayer.h>

//========================================
// Forward References
//========================================

class tMultiController;
class tCamera;

//=============================================================================
//
// Synopsis:    Wraps up the P3D related crap involved with playing an 
//              animation.
//
//=============================================================================

class SimpleAnimationPlayer : public AnimationPlayer
{
    public:
        SimpleAnimationPlayer();
        virtual ~SimpleAnimationPlayer();

        virtual void Update( unsigned int elapsedTime );

        virtual void ClearData();

        void SetNameData( char* controller, char* camera, char* animation );

        void SetCycleMode( p3dCycleMode cycleMode ) { mCycleMode = cycleMode; }
        void SetIntroLoop(unsigned nFrames);
        void SetOutroLoop(unsigned nFrames);

        void Play(void);
        void DoneIntro(void);

        void Rewind();

    protected:

        // These set all the objects needs to play an animation
        void SetController(tMultiController* pController ) { mpMasterController = pController; }
        void SetCamera( tCamera* pCamera ) { mpCamera = pCamera; }

        virtual void DoLoaded();
        virtual void DoRender();

        const char* GetAnimationName() { return( &msAnimation[0] ); }

    private:

        //Prevent wasteful constructor creation.
        SimpleAnimationPlayer( const SimpleAnimationPlayer&  );
        SimpleAnimationPlayer& operator=( const SimpleAnimationPlayer& );

        char msController[32];
        char msCamera[32];
        char msAnimation[32];

        tMultiController* mpMasterController;
        p3dCycleMode mCycleMode;
        tCamera* mpCamera;
        tCamera* mpViewCamera;
        bool mbSetCamera;
        unsigned mIntroFrames;
        unsigned mOutroFrames;
        bool mInIntro;
        unsigned mNumFrames;
};


#endif // SIMPLEANIMATIONPLAYER_H
