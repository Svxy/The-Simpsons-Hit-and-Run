//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        BonusMissionInfo.cpp
//
// Description: Implement BonusMissionInfo
//
// History:     11/19/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include <mission/BonusMissionInfo.h>
#include <mission/animatedicon.h>
#include <mission/gameplaymanager.h>
#include <mission/missionmanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <meta/eventlocator.h>
#include <meta/triggervolumetracker.h>
#include <meta/carstartlocator.h>
#include <events/eventmanager.h>
#include <events/eventenum.h>
#include <events/eventdata.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>
#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/vehiclecentral.h>

#include <memory/srrmemory.h>

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>

#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/ingame/guiscreenmissionload.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// BonusMissionInfo::BonusMissionInfo
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
BonusMissionInfo::BonusMissionInfo() :
    mIcon( NULL ),
    mAlternateIcon( NULL ),
    mMissionNum( -1 ),
    mEventLocator( NULL ),
    mIsOneShot( false ),
    mIsCompleted( false ),
    mChar1Pos( NULL ),
    mChar2Pos( NULL ),
    mCarPos( NULL ),
    mChar1Rotation( 0.0f ),
    mChar2Rotation( 0.0f ),
    mCarRotation( 0.0f ),
    mReset( false ),
    mMoved( false ),
    mHideAnimatedIcon( false ),
    mHidTheCar( false ),
    mHidDefault( false ),
    mHudMapIconID( -1 )
{

    mDialogEventData.char1 = NULL;
    mDialogEventData.char2 = NULL;

    mChar1OldPos.Set( 0.0f, 0.0f, 0.0f );
    mChar2OldPos.Set( 0.0f, 0.0f, 0.0f );
    mCarOldPos.Set( 0.0f, 0.0f, 0.0f );
}

//==============================================================================
// BonusMissionInfo::~BonusMissionInfo
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
BonusMissionInfo::~BonusMissionInfo()
{
    if ( mIcon )
    {
        delete mIcon;
        mIcon = NULL;
    }

    if ( mAlternateIcon )
    {
        delete mAlternateIcon;
        mAlternateIcon = NULL;
    }

    if ( mEventLocator )
    {
        mEventLocator->Release();
        mEventLocator = NULL;
    }

    if( mDialogEventData.char1 != NULL )
    {
        mDialogEventData.char1->Release();
        mDialogEventData.char1 = NULL;
    }
    if( mDialogEventData.char2 != NULL )
    {
        mDialogEventData.char2->Release();
        mDialogEventData.char2 = NULL;
    }

    if ( mChar1Pos )
    {
        mChar1Pos->Release();
        mChar1Pos = NULL;
    }

    if ( mChar2Pos )
    {
        mChar2Pos->Release();
        mChar2Pos = NULL;
    }

    if ( mCarPos )
    {
        mCarPos->Release();
        mCarPos = NULL;
    }
}

//=============================================================================
// BonusMissionInfo::HandleEvent
//=============================================================================
// Description: handles events that this object listens for
//
// Parameters:  id - the event we received
//              pEventData - extra info that we get          
//
// Return:      void 
//
//=============================================================================
void BonusMissionInfo::HandleEvent( EventEnum id, void* pEventData )
{
    rAssert( id == EVENT_CONVERSATION_DONE );
    //
    // Turn on the animated icon
    //
    if ( mAlternateIcon && mIsCompleted )
    {
        mAlternateIcon->ShouldRender( true );
    }
    else if ( !mIsCompleted )
    {
        mIcon->ShouldRender( true );
    }

    mHideAnimatedIcon = false;
    GetEventManager()->RemoveListener( this , EVENT_CONVERSATION_DONE );

    ResetCharacterPositions();    
}

//=============================================================================
// BonusMissionInfo::SetUpIcon
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* iconName, rmt::Vector position )
//
// Return:      void 
//
//=============================================================================
void BonusMissionInfo::SetUpIcon( const char* iconName, rmt::Vector position )
{
MEMTRACK_PUSH_GROUP( "BonusMissionInfo" );
    rAssert( mIcon == NULL );

    mIcon = new(GMA_LEVEL_OTHER) AnimatedIcon();
    mIcon->Init( iconName, position, true );

    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud != NULL )
    {
        // un-register old one first
        //
        if( mHudMapIconID != -1 )
        {
            currentHud->GetHudMap( 0 )->UnregisterIcon( mHudMapIconID );
            mHudMapIconID = -1;
        }

        // determine which icon type to register
        //
        Mission* mission = GetGameplayManager()->GetMission( this->GetMissionNum() );
        rAssert( mission != NULL );

        HudMapIcon::eIconType iconType = HudMapIcon::UNKNOWN_ICON_TYPE;
        if( mission->IsRaceMission() )
        {
            iconType = HudMapIcon::ICON_STREET_RACE;
        }
        else if( mission->IsWagerMission() )
        {
            iconType = HudMapIcon::ICON_WAGER_RACE;
        }
        else if( mission->IsBonusMission() )
        {
            if( !GetCharacterSheetManager()->QueryBonusMissionCompleted( GetGameplayManager()->GetCurrentLevelIndex() ) )
            {
                iconType = HudMapIcon::ICON_BONUS_MISSION;
            }
        }
        else
        {
            rAssertMsg( false, "Then what the hell is this??" );
        }

        // register the hud map icon
        //
        if( iconType != HudMapIcon::UNKNOWN_ICON_TYPE )
        {
            mHudMapIconID = currentHud->GetHudMap( 0 )->RegisterIcon( iconType, position );
        }
    }
MEMTRACK_POP_GROUP( "BonusMissionInfo" );
}

//=============================================================================
// BonusMissionInfo::SetUpAlternateIcon
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* iconName, rmt::Vector position )
//
// Return:      void 
//
//=============================================================================
void BonusMissionInfo::SetUpAlternateIcon( const char* iconName, rmt::Vector position )
{
    MEMTRACK_PUSH_GROUP( "BonusMissionInfo" );
    rAssert( mAlternateIcon == NULL );

    mAlternateIcon = new(GMA_LEVEL_OTHER) AnimatedIcon();
    mAlternateIcon->Init( iconName, position, true );
    MEMTRACK_POP_GROUP( "BonusMissionInfo" );
}

//=============================================================================
// BonusMissionInfo::SetEventLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventLocator* loc )
//
// Return:      void 
//
//=============================================================================
void BonusMissionInfo::SetEventLocator( EventLocator* loc )
{
    tRefCounted::Assign( mEventLocator, loc );
}

//=============================================================================
// BonusMissionInfo::SetPositions
//=============================================================================
// Description: Comment
//
// Parameters:  ( CarStartLocator* pos1, CarStartLocator* pos2, CarStartLocator* carPos )
//
// Return:      void 
//
//=============================================================================
void BonusMissionInfo::SetPositions( CarStartLocator* pos1, CarStartLocator* pos2, CarStartLocator* carPos )
{
    tRefCounted::Assign( mChar1Pos, pos1 );
    tRefCounted::Assign( mChar2Pos, pos2 );
    tRefCounted::Assign( mCarPos, carPos );
}

//=============================================================================
// BonusMissionInfo::CleanUp
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void BonusMissionInfo::CleanUp()
{
    ResetCharacterPositions();

    // if not already done so, unregister hud map icon
    //
    if( mHudMapIconID != -1 )
    {
        CGuiScreenHud* currentHud = GetCurrentHud();
        if( currentHud != NULL )
        {
            currentHud->GetHudMap( 0 )->UnregisterIcon( mHudMapIconID );
            mHudMapIconID = -1;
        }
    }

    if ( mIcon )
    {
        delete mIcon;
        mIcon = NULL;
    }

    if ( mAlternateIcon )
    {
        delete mAlternateIcon;
        mAlternateIcon = NULL;
    }

    if ( mEventLocator )
    {
        mEventLocator->Release();
        mEventLocator = NULL;
    }

    if( mDialogEventData.char1 != NULL )
    {
        mDialogEventData.char1->Release();
        mDialogEventData.char1 = NULL;
    }
    if( mDialogEventData.char2 != NULL )
    {
        mDialogEventData.char2->Release();
        mDialogEventData.char2 = NULL;
    }

    tRefCounted::Release( mChar1Pos );
    tRefCounted::Release( mChar2Pos );
    tRefCounted::Release( mCarPos   );

    mMissionNum = -1;
    mIsOneShot = false;
    mIsCompleted = false;
}

//=============================================================================
// BonusMissionInfo::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void BonusMissionInfo::Update( unsigned int milliseconds )
{
    //chuck adding a check for invalid mission number since this will cause a array bounds read.
    if (mMissionNum == -1)
    {
        return;
    }
    if ( !mIsCompleted || (mIsCompleted && !mIsOneShot) )
    {
        rmt::Vector charPos;
        if  (mDialogEventData.char2 != NULL)
        {
            mDialogEventData.char2->GetPosition( &charPos );
        }

        //
        // Test if the avatar is near the position
        //
        Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
        rmt::Vector avatarPosition;
        avatar->GetPosition( avatarPosition );
        float distSquared = ( charPos - avatarPosition ).MagnitudeSqr();
        if( distSquared < 5.0f *5.0f )
        {
            int missionNum = GetMissionNum();
            GetEventManager()->TriggerEvent( EVENT_BONUS_MISSION_CHARACTER_APPROACHED, reinterpret_cast< void* >( missionNum ) );
        }

        if ( mIcon )
        {
            if ( mIsCompleted && mAlternateIcon )
            {
                //This will get called too often.
                mIcon->ShouldRender( false );
            }
            else
            {
                if( !mHideAnimatedIcon )
                {
                    mIcon->Move( charPos );
                    mIcon->Update( milliseconds );
                }
            }
        }

        if ( mAlternateIcon )
        {
            if ( mIsCompleted )
            {
                if( !mHideAnimatedIcon )
                {
                    mAlternateIcon->Move( charPos );
                    mAlternateIcon->Update( milliseconds );
                }
            }
            else
            {
                mAlternateIcon->ShouldRender( false );
            }
        }

        if(mEventLocator)
        {
            mEventLocator->GetTriggerVolume( 0 )->SetPosition( charPos );
        }
    }
}

//=============================================================================
// BonusMissionInfo::Enable
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void BonusMissionInfo::Enable()
{
    if ( !mIsCompleted || (mIsCompleted && !mIsOneShot) )
    {
        if ( mIcon && ( !mIsCompleted || !mAlternateIcon ) )
        {
            mIcon->ShouldRender( true );
        }

        if ( mAlternateIcon && mIsCompleted )
        {
            mAlternateIcon->ShouldRender( true );
        }

        if ( mEventLocator )
        {
            TriggerVolumeTracker::GetInstance()->AddTrigger( mEventLocator->GetTriggerVolume(0) );
        }
    }
}

//=============================================================================
// BonusMissionInfo::Disable
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void BonusMissionInfo::Disable()
{
    if ( mIcon )
    {
        mIcon->ShouldRender( false );
    }

    if ( mAlternateIcon )
    {
        mAlternateIcon->ShouldRender( false );
    }

    if ( mEventLocator )
    {
        TriggerVolumeTracker::GetInstance()->RemoveTrigger( mEventLocator->GetTriggerVolume(0) );
    }
}

//=============================================================================
// BonusMissionInfo::SetCompleted
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool completed )
//
// Return:      void 
//
//=============================================================================
void BonusMissionInfo::SetCompleted( bool completed )
{
    mIsCompleted = completed;

    if(mIsCompleted && mIsOneShot)
    {
        mDialogEventData.char2->SetRole(Character::ROLE_COMPLETED_BONUS);
        GetCharacterManager()->SetGarbage(mDialogEventData.char2, true);
        tRefCounted::Assign( mDialogEventData.char2, (Character*)NULL);

        if( mHudMapIconID != -1 )
        {
            CGuiScreenHud* currentHud = GetCurrentHud();
            if( currentHud != NULL )
            {
                currentHud->GetHudMap( 0 )->UnregisterIcon( mHudMapIconID );
                mHudMapIconID = -1;
            }
        }
    }
}

//=============================================================================
// BonusMissionInfo::TriggerDialogue
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void BonusMissionInfo::TriggerDialogue()
{
    Parent::Reset();

    //Set the primary character
    Character* c1 = GetCharacterManager()->GetCharacter( 0 );
    tRefCounted::Assign( mDialogEventData.char1, c1 );

    Character* c2 = mDialogEventData.char2;

    //POSITIONS, PLEASE!
    c1->GetPosition( &mChar1OldPos );
    mChar1Rotation = c1->GetFacingDir();

    rmt::Vector location;
    if ( mChar1Pos )
    {
        mChar1Pos->GetLocation( &location );
        c1->RelocateAndReset( location, mChar1Pos->GetRotation() );
    }

    c2->GetPosition( &mChar2OldPos );
    mChar2Rotation = c2->GetFacingDir();

    if ( mChar2Pos )
    {
        mChar2Pos->GetLocation( &location );
        c2->RelocateAndReset( location, mChar2Pos->GetRotation() );
    }

    Vehicle* v = GetGameplayManager()->GetCurrentVehicle();
    v->GetPosition( &mCarOldPos );
    mCarRotation = v->GetFacingInRadians();

    bool needToHide = false;

    float dist = 0.0f;
    
    if ( mChar1Pos )
    {
        mChar1Pos->GetLocation( &location );
        rmt::Vector c1ToCar;
        c1ToCar.Sub( mCarOldPos, location );
        if ( c1ToCar.MagnitudeSqr() < 49.0f )
        {
            needToHide = true;
        }
    }

    if ( !needToHide && mChar2Pos )
    {
        mChar2Pos->GetLocation( &location );
        rmt::Vector c2ToCar;
        c2ToCar.Sub( mCarOldPos, location );
        if ( c2ToCar.MagnitudeSqr() < 49.0f )
        {
            needToHide = true;
        }
    }

    if ( needToHide )
    {
        //hide this car
        v->CarDisplay( false );
        GetVehicleCentral()->RemoveVehicleFromActiveList( v );
        mHidTheCar = true;
    }

    //Should also hide the default car if it is not your current vehicle.
    Vehicle* defaultCar = GetGameplayManager()->GetVehicleInSlot( GameplayManager::eDefaultCar );
    if ( defaultCar && defaultCar != v )
    {
        bool needToHideDefault = false;

        rmt::Vector defaultPos;
        defaultCar->GetPosition( &defaultPos );

        if ( mChar1Pos )
        {
            mChar1Pos->GetLocation( &location );
            rmt::Vector c1ToCar;
            c1ToCar.Sub( defaultPos, location );
            if ( c1ToCar.MagnitudeSqr() < 49.0f )
            {
                needToHideDefault = true;
            }
        }

        if ( !needToHideDefault && mChar2Pos )
        {
            mChar2Pos->GetLocation( &location );
            rmt::Vector c2ToCar;
            c2ToCar.Sub( defaultPos, location );
            if ( c2ToCar.MagnitudeSqr() < 49.0f )
            {
                needToHideDefault = true;
            }
        }

        if ( needToHideDefault )
        {
            //hide default
            defaultCar->CarDisplay( false );
            GetVehicleCentral()->RemoveVehicleFromActiveList( defaultCar );

            mHidDefault = true;
        }
    }

    if ( mChar1Pos || mChar2Pos )
    {
        GetSuperCamManager()->GetSCC( 0 )->DoCameraCut();

        mChar1Pos->GetLocation( &location );
        TrafficManager::GetInstance()->ClearTrafficInSphere( rmt::Sphere( location, 100.0f ) );

        mMoved = true;
    }

    mReset = false;

    CGuiScreenMissionBase::GetBitmapName( mPreviousMissionPic );
    GetEventManager()->TriggerEvent( EVENT_CONVERSATION_INIT, (void*)(&mDialogEventData) );
    GetEventManager()->TriggerEvent( EVENT_CONVERSATION_INIT_DIALOG, (void*)(&mDialogEventData) );
    GetEventManager()->TriggerEvent( EVENT_CONVERSATION_START, (void*)(NULL) );

    //
    // Turn off the animated icon
    //
    if ( mAlternateIcon )
    {
        mAlternateIcon->ShouldRender( false );
    }

    if ( mIcon )
    {
        mIcon->ShouldRender( false );
    }
 
    mHideAnimatedIcon = true;
    GetEventManager()->AddListener( this , EVENT_CONVERSATION_DONE );
}


//=============================================================================
// BonusMissionInfo::SetNPC
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* character )
//
// Return:      void 
//
//=============================================================================
void BonusMissionInfo::SetNPC( Character* character )
{
    tRefCounted::Assign( mDialogEventData.char2, character );
}

//=============================================================================
// BonusMissionInfo::ResetCharacterPositions
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void BonusMissionInfo::ResetCharacterPositions()
{
    if(mIcon != NULL)
    {
        mIcon->ShouldRender( true );
    }
    mHideAnimatedIcon = false;

    if ( mReset || !mMoved )
    {
        return;
    }

    if ( mChar1Pos && mDialogEventData.char1 )
    {
        mDialogEventData.char1->RelocateAndReset( mChar1OldPos, mChar1Rotation );
    }

    if ( mChar2Pos && mDialogEventData.char2 )
    {
        mDialogEventData.char2->RelocateAndReset( mChar2OldPos, mChar2Rotation );
    }

    if ( mHidTheCar )
    {
        Vehicle* v = GetGameplayManager()->GetCurrentVehicle();
        
        v->CarDisplay( true );
        GetVehicleCentral()->AddVehicleToActiveList( v );
        mHidTheCar = false;
    }

    if ( mHidDefault )
    {
        Vehicle* defaultCar = GetGameplayManager()->GetVehicleInSlot( GameplayManager::eDefaultCar );
        defaultCar->CarDisplay( true );
        GetVehicleCentral()->AddVehicleToActiveList( defaultCar );

        mHidDefault = false;
    }

    if ( mChar1Pos || mChar2Pos )
    {
        GetSuperCamManager()->GetSCC( 0 )->DoCameraCut();
        GetSuperCamManager()->GetSCC( 0 )->Update( 16 );
    }

    mMoved = false;
    mReset = true;
}

//=============================================================================
// BonusMissionInfo::ResetMissionBitmap
//=============================================================================
// Description: Puts the mission briefing bitmap back to how it was before
//              this bonus mission got triggered
//
// Parameters:  none
//
// Return:      void 
//
//=============================================================================
void BonusMissionInfo::ResetMissionBitmap()
{
    CGuiScreenMissionBase::SetBitmapName( mPreviousMissionPic );
    CGuiScreenMissionBase::ReplaceBitmap();
    strcpy( mPreviousMissionPic, "" );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
