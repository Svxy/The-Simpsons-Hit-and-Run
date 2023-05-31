//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        bonusmissioninfo.h
//
// Description: Blahblahblah
//
// History:     11/19/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef BONUSMISSIONINFO_H
#define BONUSMISSIONINFO_H

//========================================
// Nested Includes
//========================================
#include <events/eventdata.h>
#include <events/eventlistener.h>
#include <mission/haspresentationinfo.h>
#include <radmath/radmath.hpp>

//========================================
// Forward References
//========================================
class AnimatedIcon;
class EventLocator;
class Character;
class CarStartLocator;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class BonusMissionInfo:
    public HasPresentationInfo,
    public EventListener
{
private:
    typedef HasPresentationInfo Parent;
public:
    BonusMissionInfo();
	virtual ~BonusMissionInfo();
    
    void SetUpIcon( const char* iconName, rmt::Vector position );
    void SetUpAlternateIcon( const char* iconName, rmt::Vector position );
    
    void SetMissionNum( int missionNum );
    int GetMissionNum() const;

    void SetEventLocator( EventLocator* loc );
    EventLocator* GetEventLocator();

    void SetOneShot( bool isOneShot );
    
    void SetCompleted( bool completed );
    bool GetCompleted() const;

    void SetNPC( Character* character );
    Character* GetNPC();

    void SetDialogueName( const char* name );

    void Update( unsigned int milliseconds );
    void CleanUp();

    void Enable();
    void Disable();

    void TriggerDialogue();

    void SetPositions( CarStartLocator* pos1, CarStartLocator* pos2, CarStartLocator* carPos );
    void ResetCharacterPositions();
    void HandleEvent( EventEnum id, void* pEventData );
    void ResetMissionBitmap();

private:
    AnimatedIcon* mIcon;
    AnimatedIcon* mAlternateIcon;
    int mMissionNum;
    EventLocator* mEventLocator;
    DialogEventData mDialogEventData;
    bool mIsOneShot;
    bool mIsCompleted;

    CarStartLocator* mChar1Pos;
    CarStartLocator* mChar2Pos;
    CarStartLocator* mCarPos;

    rmt::Vector mChar1OldPos;
    rmt::Vector mChar2OldPos;
    rmt::Vector mCarOldPos;
    float mChar1Rotation;
    float mChar2Rotation;
    float mCarRotation;
    char mPreviousMissionPic[ 256 ];

    bool mReset            : 1;
    bool mMoved            : 1;
    bool mHideAnimatedIcon : 1;
    bool mHidTheCar : 1;
    bool mHidDefault : 1;

    int mHudMapIconID;

    //Prevent wasteful constructor creation.
	BonusMissionInfo( const BonusMissionInfo& bonusmissioninfo );
	BonusMissionInfo& operator=( const BonusMissionInfo& bonusmissioninfo );
};

//******************************************************************************
//
// Inline Public Member Functions
//
//******************************************************************************

//=============================================================================
// BonusMissionInfo::SetMissionNum
//=============================================================================
// Description: Comment
//
// Parameters:  ( int missionNum )
//
// Return:      void 
//
//=============================================================================
inline void BonusMissionInfo::SetMissionNum( int missionNum )
{
    mMissionNum = missionNum;
}

//=============================================================================
// BonusMissionInfo::GetMissionNum
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      int 
//
//=============================================================================
inline int BonusMissionInfo::GetMissionNum() const
{
    return mMissionNum;
}

//=============================================================================
// BonusMissionInfo::GetEventLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      EventLocator
//
//=============================================================================
inline EventLocator* BonusMissionInfo::GetEventLocator()
{
    return mEventLocator;
}


//=============================================================================
// BonusMissionInfo::SetOneShot
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool isOneShot )
//
// Return:      void 
//
//=============================================================================
inline void BonusMissionInfo::SetOneShot( bool isOneShot )
{
    mIsOneShot = isOneShot;
}


//=============================================================================
// BonusMissionInfo::GetCompleted
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool BonusMissionInfo::GetCompleted() const
{
    return mIsCompleted;
}

//=============================================================================
// BonusMissionInfo::GetNPC
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Character
//
//=============================================================================
inline Character* BonusMissionInfo::GetNPC()
{
    return mDialogEventData.char2;
}

//=============================================================================
// BonusMissionInfo::SetDialogueName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void BonusMissionInfo::SetDialogueName( const char* name )
{
    mDialogEventData.dialogName = ::radMakeKey32( name );
}

#endif //BONUSMISSIONINFO_H
