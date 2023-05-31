//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        animated.h
//
// Description: camers used for animated camera transitions
//
// History:     21/01/2003 + Created -- Ian Gipson
//
//=============================================================================

#ifndef ANIMATEDCAMERA_H
#define ANIMATEDCAMERA_H

//========================================
// Nested Includes
//========================================
#include <camera/supercam.h>
#include <events/eventlistener.h>
#include <input/mappable.h>

//========================================
// Forward References
//========================================
class ISuperCamTarget;
class tMultiController;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class AnimatedCam : 
    public SuperCam
{
public:
    AnimatedCam();
    virtual ~AnimatedCam();

    //Update: Called when you want the super cam to update its state.
    void Update( unsigned int milliseconds );
    void              Abort();
    static void       AllowSkipping( const bool skippingAllowed );
    static void       CheckPendingCameraSwitch();
    static void       ClearCamera();
    const char* const GetName() const;
    Type              GetType();
    virtual void      OnInit();
    virtual void      OnShutdown();
    static void       Reset();
    static void       SetCamera( tName name );
    static void       SetCameraTransitionFlags( int flags );
    static void       SetMulticontroller( tName name );
    static void       SetMissionStartCamera( tName name );
    static void       SetMissionStartMulticontroller( tName name );
    void              SetNextCameraType( const SuperCam::Type type );
    void              SetTarget( ISuperCamTarget* target );
    static void       Skip();
    static void       SupressNextLetterbox();
    static void       TriggerMissionStartCamera();

    #ifdef DEBUGWATCH
        virtual const char* GetWatcherName() const;
    #endif


protected:
    static bool       CameraSwitchPending();
    static void       LookupCamera();
    static void       LookupMulticontroller();
    virtual void      LetterBoxStart();
    virtual void      LetterBoxStop();
    static void       SetCameraSwitchPending( const bool pending );

    SuperCam::Type      m_NextCameraType;

    //Prevent wasteful constructor creation.
    AnimatedCam( const AnimatedCam& AnimatedCam );
    AnimatedCam& operator=( const AnimatedCam& AnimatedCam );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

#endif //ANIMATEDCAMERA_H
