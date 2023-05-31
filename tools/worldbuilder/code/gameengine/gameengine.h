//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        gameengine.h
//
// Description: Blahblahblah
//
// History:     19/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

//========================================
// Nested Includes
//========================================
#include "main/toolhack.h"

#include "precompiled/PCH.h"

#include "mayacamera.h"

//========================================
// Forward References
//========================================
class RailCam;
class SuperCam;
class WBCamTarget;
class StaticCam;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class GameEngine
{
public:
    static void CreateInstance();
    static void DestroyInstance();

    static GameEngine* GetInstance();

    void UpdateRailCam( MObject& railCam );
    void UpdateStaticCam( MObject& staticCam );

    MayaCamera& GetMayaCam();

private:
    void UpdateRailSettings( RailCam* railCam, MObject obj );
    void UpdateStaticCamSettings( StaticCam* staticCam, MObject obj );

    static GameEngine* mInstance;
    MayaCamera* mMayaCam;

    unsigned int mTimeStart;

    GameEngine();
    virtual ~GameEngine();
    void Init();
    void UpdateFOV( SuperCam* cam, const rmt::Vector position );

    //Prevent wasteful constructor creation.
    GameEngine( const GameEngine& gameengine );
    GameEngine& operator=( const GameEngine& gameengine );
};

//=============================================================================
// GameEngine::GetMayaCam
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline MayaCamera& GameEngine::GetMayaCam()
{
    return *mMayaCam;
}

#endif //GAMEENGINE_H
