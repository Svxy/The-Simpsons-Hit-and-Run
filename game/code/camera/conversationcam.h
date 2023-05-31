//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ConversationCam.h
//
// Description: Camera used for outdoor conversation. Based of  Cary's Bumper Cam
//
// History:     24/04/2002 + Created -- Cary Brisebois, Chuck Chow
//
//=============================================================================

#ifndef ConversationCam_H
#define ConversationCam_H

//========================================
// Nested Includes
//========================================
#include <camera/supercam.h>
#include <camera/ConversationCamdata.h>

//========================================
// Forward References
//========================================
class Character;
class ISuperCamTarget;

#define CONV_CAM_MAX_CHARACTERS 2

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class ConversationCam : public SuperCam
{
public:

    enum ConversationFlag
    {
        READY = SUPERCAM_END   //This is to carry on from the supercam flags
    };
    
    ConversationCam();
    virtual ~ConversationCam();

    //Update: Called when you want the super cam to update its state.
    virtual void Update( unsigned int milliseconds);

    //Returns the name of the super cam.  
    //This can be used in the FE or debug info
    virtual const char* const GetName() const;

    //This loads the off-line created settings for the camera.  
    //It is passed in as a byte stream of some data of known size.
    virtual void LoadSettings( unsigned char* settings ); 
 
    virtual Type GetType();

    //These are for favourable support of this command
    virtual void SetTarget( ISuperCamTarget* target ); 
    virtual void AddTarget( ISuperCamTarget* target );
    void         LockCharacterPositions();
    void         SetCharacter( const int index, Character* character );
    virtual void OnInit();
    virtual void OnShutdown();
    
    unsigned int GetNumTargets() const;
    static void  SetCameraByName( const tName& name );
    static void  SetCameraDistanceByName( const tName& name, const float distance );
    static void  SetPcCameraByName( const tName& name );
    static void  SetNpcCameraByName( const tName& name );
    static void  UsePcCam();
    static void  UseNpcCam();
    static void  SetNpcIsChild( const bool isChild );
    static void  SetPcIsChild( const bool isChild );
    static void  SetCamBestSide( const tName& name );

private:
    static void  SetCameraByIndex( const unsigned int index );
    static void  SetCameraDistanceByIndex( const unsigned int index, const float distance );

    //These functions are to allow real-time control of the settings of 
    //the supercam.
    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();
    void CopyDefaultsToCameraPerLineOfDialog();

    ConversationCamData mData;
    ISuperCamTarget* mTargets[2];

    rmt::Vector mPositionOriginal;
    rmt::Vector mTargetOriginal;
    rmt::Vector mPosition;
    rmt::Vector mTarget;

    float mTargetHeight;
    float mPositionHeight;

    Character* mCharacters[ CONV_CAM_MAX_CHARACTERS ];

    static int sInstCount;
    
    //Prevent wasteful constructor creation.
    ConversationCam( const ConversationCam& ConversationCam );
    ConversationCam& operator=( const ConversationCam& ConversationCam );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// ConversationCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================
inline const char* const ConversationCam::GetName() const
{
    return "CONVERSATION_CAM";
}

//=============================================================================
// ConversationCam::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
inline SuperCam::Type ConversationCam::GetType()
{
    return CONVERSATION_CAM;
}

#endif //ConversationCam_H
