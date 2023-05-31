//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implementation of class ActionButtonHandler
//
// History:     08/07/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

#include <poser/joint.hpp>
#include <poser/poseengine.hpp>
#include <poser/pose.hpp>

#include <p3d/anim/pose.hpp>
#include <p3d/anim/animate.hpp>
#include <p3d/anim/poseanimation.hpp>
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/anim/multicontroller.hpp>
#include <p3d/matrixstack.hpp>

//========================================
// Project Includes
//========================================
#include <ai/actionbuttonhandler.h>
#include <ai/actionbuttonmanager.h>
#include <presentation/presentation.h>
#include <worldsim/character/character.h>
#include <worldsim/character/charactercontroller.h>
#include <ai/statemanager.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/character/charactermanager.h>
#include <events/eventmanager.h>
#include <events/eventenum.h>
#include <sound/soundmanager.h>

#include <memory/srrmemory.h>
#include <meta/eventlocator.h>
#include <meta/actioneventlocator.h>
#include <meta/spheretriggervolume.h>
#include <meta/triggervolumetracker.h>
#include <meta/interiorentrancelocator.h>
#include <meta/carstartlocator.h>
#include <worldsim/parkedcars/parkedcarmanager.h>
#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/hitnrunmanager.h>

#include <ai/sequencer/action.h>
#include <ai/sequencer/actioncontroller.h>
#include <ai/sequencer/sequencer.h>
#include <ai/actionbuttonhandler.h>
#ifdef ACTIONEVENTHANDLER_DEBUG
#include <ai/actionnames.h>
#endif // ACTIONEVENTHANDLER_DEBUG
#include <cards/cardgallery.h>
#include <cards/cardsdb.h>
#include <cards/card.h>
#include <camera/supercam.h>

#include <render/DSG/animcollisionentitydsg.h>
#include <render/DSG/InstDynaPhysDSG.h>
#include <render/RenderManager/RenderManager.h>
#include <render/RenderManager/RenderLayer.h>
#include <render/particles/particlemanager.h>

#include <presentation/gui/guisystem.h>
#include <presentation/gui/guiwindow.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreeniriswipe.h>
#include <presentation/gui/ingame/guiscreenletterbox.h>

#include <gameflow/gameflow.h>
#include <mission/gameplaymanager.h>

#include <mission/animatedicon.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/gameplaymanager.h>
#include <mission/rewards/rewardsmanager.h>

#include <render/loaders/allwrappers.h>
#include <render/loaders/BillboardWrappedLoader.h>

#include <interiors/interiormanager.h>
#include <render/DSG/InstAnimDynaPhysDSG.h>

#include <input/inputmanager.h>
#ifdef RAD_WIN32
#include <input/usercontrollerWin32.h>
#else
#include <input/usercontroller.h>
#endif

#include <contexts/contextenum.h>

#include <camera/relativeanimatedcam.h>
#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>
#include <camera/supercam.h>

#include <contexts/context.h>
#include <render/DSG/StatePropDSG.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
static const tUID GIL = tEntity::MakeUID( "gil" );

//
// Oh yeah.  Big pre-final hack.
//
struct CarPurchaseDialogueStuff
{
    radKey32 convName;
    radInt64 sellerName;
};
static CarPurchaseDialogueStuff s_carPurchaseConvNames[] =
{
    { ::radMakeKey32( "plowking" ),  tEntity::MakeUID( "reward_barney" ) },
    { ::radMakeKey32( "son" ),       tEntity::MakeUID( "reward_homer" ) },
    { ::radMakeKey32( "bus" ),       tEntity::MakeUID( "reward_otto" ) },
    { ::radMakeKey32( "tractor" ),   tEntity::MakeUID( "reward_willie" ) },
    { ::radMakeKey32( "borrowing" ), tEntity::MakeUID( "reward_homer" ) },
    { ::radMakeKey32( "swine" ),     tEntity::MakeUID( "reward_kearney" ) },
    { 0, tEntity::MakeUID( "zombie" ) }   // No conversation for zombies
};

struct AnimSwitchSoundData
{
    radKey32 objKey;
    const char* soundName;
    const char* positionalSettings;
    bool isMovingSound;
};

static const AnimSwitchSoundData ANIM_SWITCH_SOUND_TABLE[] =
{
    { ::radMakeKey32( "TD" ), "trapdoor", "platform_settings", false },
    { ::radMakeKey32( "Splatform1Trans" ), "platform_02", "platform_settings", true },
    { ::radMakeKey32( "Splatform4Trans" ), "platform_01", "platform_settings", true },
    { ::radMakeKey32( "Splatform5Trans" ), "platform_01", "platform_settings", true },
    { ::radMakeKey32( "Splatform6Trans" ), "platform_01", "platform_settings", true },
    { ::radMakeKey32( "Aplatform6Trans" ), "aplatform6", "platform_settings", true },
    { ::radMakeKey32( "Splatfrom7Trans" ), "platform_01", "platform_settings", true },  // (sic)
    { ::radMakeKey32( "truck1" ), "duff_truck", "platform_settings", false },
    { ::radMakeKey32( "hdoortrans1" ), "plantdoor", "platform_settings", false },
    { ::radMakeKey32( "elevator" ), "elevator", "loud_elevator_settings", true },
    { ::radMakeKey32( "library" ), "gag_library", "platform_settings", false }
};

static unsigned int ANIM_SWITCH_SOUND_TABLE_SIZE = 
    sizeof( ANIM_SWITCH_SOUND_TABLE ) / sizeof( AnimSwitchSoundData );

namespace ActionButton
{
//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************
/*
==============================================================================
AnimCollisionEntityDSGWrapper::AnimCollisionEntityDSGWrapper
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         AnimCollisionEntityDSGWrapper

=============================================================================
*/
AnimCollisionEntityDSGWrapper::AnimCollisionEntityDSGWrapper( void )
:
mpGameObject( 0 )
{
}
/*
==============================================================================
AnimCollisionEntityDSGWrapper::~AnimCollisionEntityDSGWrapper
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         AnimCollisionEntityDSGWrapper

=============================================================================
*/
AnimCollisionEntityDSGWrapper::~AnimCollisionEntityDSGWrapper( void )
{
    // Do not release.  See comment in AnimCollisionEntityDSGWrapper::SetGameObject().
    //
    // TBJ [8/14/2002] 
    //
    //tRefCounted::Release( mpGameObject );
    mpGameObject = 0;
}
/*
==============================================================================
AnimCollisionEntityDSGWrapper::UpdateVisibility
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void AnimCollisionEntityDSGWrapper::UpdateVisibility( void )
{
    mpGameObject->UpdateVisibility();
}
/*
==============================================================================
AnimCollisionEntityDSGWrapper::SetGameObject
==============================================================================
Description:    Comment

Parameters:     ( AnimCollisionEntityDSG* pGameObject )

Return:         void 

=============================================================================
*/
void AnimCollisionEntityDSGWrapper::SetGameObject( AnimCollisionEntityDSG* pGameObject )
{
    // Do NOT addref.  The "AnimCollisionEntityDSG" is dynamically loaded.
    // If the dynaload system wants to release this object, we must let it go.
    // We will assume that the 'AnimCollisionEntityDSG' will notify the AnimSwitch 
    // when it is deleted.
    //
    // TBJ [8/14/2002] 
    //
    //tRefCounted::Assign( mpGameObject, pGameObject );
    mpGameObject = pGameObject;
}

/*
==============================================================================
AnimCollisionEntityDSGWrapper::GetAnimationDirection
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         float

=============================================================================
*/
float& AnimCollisionEntityDSGWrapper::GetAnimationDirection( void )
{
    return mpGameObject->GetAnimationDirection();
}
/*
==============================================================================
AnimCollisionEntityDSGWrapper::SetAnimationDirection
==============================================================================
Description:    Comment

Parameters:     ( float fDirection )

Return:         void 

=============================================================================
*/
void AnimCollisionEntityDSGWrapper::SetAnimationDirection( float fDirection )
{
    mpGameObject->SetAnimationDirection( fDirection );
}
/*
==============================================================================
AnimCollisionEntityDSGWrapper::GetAnimController
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         tMultiController

=============================================================================
*/
tMultiController* AnimCollisionEntityDSGWrapper::GetAnimController( void ) const
{
    return mpGameObject->GetAnimController();
}

/*
==============================================================================
AnimCollisionEntityDSGWrapper::GetDrawable
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         tCompositeDrawable

=============================================================================
*/
tCompositeDrawable* AnimCollisionEntityDSGWrapper::GetDrawable( void ) const
{
    return mpGameObject->GetDrawable();
}
/*
==============================================================================
AnimCollisionEntityDSGWrapper::Display
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void AnimCollisionEntityDSGWrapper::Display( void )
{
    rAssertMsg( false, "This object should not be drawn.\n" );
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/*
==============================================================================
AnimEntityDSGWrapper::AnimEntityDSGWrapper
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         AnimEntityDSGWrapper

=============================================================================
*/
AnimEntityDSGWrapper::AnimEntityDSGWrapper( void )
:
mpDrawable( 0 ),
mpPose( 0 ),
mpAnimController( 0 ),
mbVisible( true )
{
}
/*
==============================================================================
AnimEntityDSGWrapper::~AnimEntityDSGWrapper
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         AnimEntityDSGWrapper

=============================================================================
*/
AnimEntityDSGWrapper::~AnimEntityDSGWrapper( void )
{
    tRefCounted::Release( mpDrawable );
    tRefCounted::Release( mpPose );
    tRefCounted::Release( mpAnimController );
}
/*
==============================================================================
AnimEntityDSGWrapper::UpdateVisibility
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void AnimEntityDSGWrapper::UpdateVisibility( void )
{
    // No physics here, so do nothing.
    //
}
/*
==============================================================================
AnimEntityDSGWrapper::SetDrawable
==============================================================================
Description:    Comment

Parameters:     ( tCompositeDrawable* pDrawable )

Return:         void 

=============================================================================
*/
void AnimEntityDSGWrapper::SetDrawable( tCompositeDrawable* pDrawable )
{
    tRefCounted::Assign( mpDrawable, pDrawable );
}
/*
==============================================================================
AnimEntityDSGWrapper::SetPose
==============================================================================
Description:    Comment

Parameters:     ( tPose* pPose )

Return:         void 

=============================================================================
*/
void AnimEntityDSGWrapper::SetPose( tPose* pPose )
{
    tRefCounted::Assign( mpPose, pPose );
}
/*
==============================================================================
AnimEntityDSGWrapper::SetAnimController
==============================================================================
Description:    Comment

Parameters:     ( tMultiController* pAnimController )

Return:         void 

=============================================================================
*/
void AnimEntityDSGWrapper::SetAnimController( tMultiController* pAnimController )
{
    tRefCounted::Assign( mpAnimController, pAnimController );
}
/*
==============================================================================
AnimEntityDSGWrapper::SetTransform
==============================================================================
Description:    Comment

Parameters:     ( rmt::Matrix& transform )

Return:         void 

=============================================================================
*/
void AnimEntityDSGWrapper::SetTransform( rmt::Matrix& transform )
{
        mTransform = transform;
}
/*
==============================================================================
AnimEntityDSGWrapper::GetAnimationDirection
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         float

=============================================================================
*/
float& AnimEntityDSGWrapper::GetAnimationDirection( void )
{
    return mfDirection;
}
/*
==============================================================================
AnimEntityDSGWrapper::SetAnimationDirection
==============================================================================
Description:    Comment

Parameters:     ( float fDirection )

Return:         void 

=============================================================================
*/
void AnimEntityDSGWrapper::SetAnimationDirection( float fDirection )
{
    mfDirection = fDirection;
}
/*
==============================================================================
AnimEntityDSGWrapper::GetAnimController
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         tMultiController

=============================================================================
*/
tMultiController* AnimEntityDSGWrapper::GetAnimController( void ) const
{
    // Why did I need to do this?
    //
    //mpAnimController->SetPose( mpPose );
    return mpAnimController;
}

/*
==============================================================================
AnimEntityDSGWrapper::GetDrawable
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         tCompositeDrawable

=============================================================================
*/
tCompositeDrawable* AnimEntityDSGWrapper::GetDrawable( void ) const
{
    return mpDrawable;
}
/*
==============================================================================
AnimEntityDSGWrapper::Display
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void AnimEntityDSGWrapper::Display( void )
{
   if ( IsVisible() )
    {
        p3d::stack->PushMultiply( mTransform );
        {
            mpDrawable->SetPose( mpPose );
            mpDrawable->Display();
        }
        p3d::stack->Pop( );
    }
}

//==============================================================================
// ActionButtonHandler::ActionButtonHandler
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
ButtonHandler::ButtonHandler( void )
:
mActionButton( CharacterController::DoAction )
{   
}

//==============================================================================
// ActionButtonHandler::~ActionButtonHandler
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
ButtonHandler::~ButtonHandler()
{
}

/*
==============================================================================
ButtonHandler::ButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         bool 

=============================================================================
*/
bool ButtonHandler::ButtonPressed( Character* pCharacter )
{
    if ( IsActionButtonPressed( pCharacter ) )
    {
        pCharacter->GetActionController()->Clear();
        Sequencer* pSeq = pCharacter->GetActionController()->GetNextSequencer();
        if ( OnButtonPressed( pCharacter, pSeq ) )
        {
            if ( UsesActionButton() )
            {
//                GetGuiSystem()->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_ACTION_BUTTON );
            }
            return true;
        }
    }
    return false;
}
/*
==============================================================================
ButtonHandler::Enter
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void ButtonHandler::Enter( Character* pCharacter )
{
    OnEnter( pCharacter );
}
/*
==============================================================================
ButtonHandler::Exit
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void ButtonHandler::Exit( Character* pCharacter )
{
    OnExit( pCharacter );
}
//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************
/*
==============================================================================
ButtonHandler::IsActionButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         bool 

=============================================================================
*/
bool ButtonHandler::IsActionButtonPressed( Character* pCharacter )
{
    CharacterController::eIntention theIntention = pCharacter->GetController()->GetIntention();
    return ( theIntention == mActionButton );
}

/*
==============================================================================
PropHandler::PropHandler
==============================================================================
Description:	Comment

Parameters:		( void )

Return:			PropHandler

=============================================================================
*/
PropHandler::PropHandler( void )
:
mpProp( 0 )
{
}
/*
==============================================================================
PropHandler::~PropHandler
==============================================================================
Description:	Comment

Parameters:		( void )

Return:			PropHandler

=============================================================================
*/
PropHandler::~PropHandler( void )
{
	tRefCounted::Release( mpProp );
}
/*
==============================================================================
PropHandler::SetProp
==============================================================================
Description:	Comment

Parameters:		( InstDynaPhysDSG* pProp )

Return:			void 

=============================================================================
*/
void PropHandler::SetProp( InstDynaPhysDSG* pProp )
{
	tRefCounted::Assign( mpProp, pProp );
}
/*
==============================================================================
PropHandler::GetProp
==============================================================================
Description:	Comment

Parameters:		( void )

Return:			InstDynaPhysDSG

=============================================================================
*/
InstDynaPhysDSG* PropHandler::GetProp( void ) const
{
	return mpProp;
}
/*
==============================================================================
AttachProp::OnButtonPressed
==============================================================================
Description:	Comment

Parameters:		( Character* pCharacter, Sequencer* pSeq )

Return:			bool 

=============================================================================
*/
bool AttachProp::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{
	rAssert( GetProp( ) != 0 );
	pCharacter->AttachProp( GetProp( ) );
	return true;
}
/*
==============================================================================
EnterInterior::EnterInterior
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         EnterInterior

=============================================================================
*/
EnterInterior::EnterInterior( InteriorEntranceLocator* pLocator )
:
mpLocator( 0 )
{
    SetLocator (pLocator);
}
/*
==============================================================================
EnterInterior::~EnterInterior
==============================================================================
Description:    Comment

Parameters:     ()

Return:         EnterInterior

=============================================================================
*/
EnterInterior::~EnterInterior()
{
}


void EnterInterior::SetLocator ( InteriorEntranceLocator* pLocator )
{
    mpLocator = pLocator;
}

/*
==============================================================================
EnterInterior::OnButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
bool EnterInterior::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{               
    if(GetGameFlow()->GetNextContext() != CONTEXT_PAUSE)
    {
        GetInteriorManager()->Enter(mpLocator, pCharacter, pSeq);
    }
    return true;
}
/*
==============================================================================
GetInCar::GetInCar
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         GetInCar

=============================================================================
*/
GetInCar::GetInCar( EventLocator* pEventLocator )
:
mpEventLocator( 0 ),
mCharacter( NULL )
{
    SetEventLocator( pEventLocator );
}
/*
==============================================================================
GetInCar::~GetInCar
==============================================================================
Description:    Comment

Parameters:     ()

Return:         GetInCar

=============================================================================
*/
GetInCar::~GetInCar()
{
    if ( mpEventLocator )
    {
        mpEventLocator->Release( );
        mpEventLocator = 0;
    }

    if ( mCharacter )
    {
        mCharacter->Release();
        mCharacter = NULL;
    }
}
/*
==============================================================================
GetInCar::SetEventLocator
==============================================================================
Description:    Comment

Parameters:     ( EventLocator* pEventLocator )

Return:         void 

=============================================================================
*/
void GetInCar::SetEventLocator( EventLocator* pEventLocator )
{
    tRefCounted::Assign( mpEventLocator, pEventLocator );
}
/*
==============================================================================
GetInCar::OnButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
bool GetInCar::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{
    CharacterController::eIntention theIntention = pCharacter->GetController()->GetIntention();

    Vehicle* pVehicle = GetVehicleCentral()->GetVehicle( mVehicleId );
    rAssert( pVehicle ); 

    tRefCounted::Assign( mCharacter, pCharacter );

    //
    // Trigger an event for tutorial mode
    //
    pCharacter->SetTargetVehicle( pVehicle );  

    rAssertMsg( pVehicle->mVehicleType != VT_AI, 
        "Trying to get into AI car?? Don't! We shouldn't have even allowed "
        "reaching this point!" );

    if( pVehicle->mVehicleType == VT_TRAFFIC)
    {
        GetEventManager()->TriggerEvent( EVENT_ENTERING_TRAFFIC_CAR, (void*)pVehicle );
    }
    else
    {
        GetEventManager()->TriggerEvent( EVENT_ENTERING_PLAYER_CAR, (void*)pVehicle );
    }

    pCharacter->GetStateManager()->SetState<CharacterAi::GetIn>();    

    return true;
}

/*
==============================================================================
ActionEventHandler::ActionEventHandler
==============================================================================
Description:    Comment

Parameters:     ( ActionEventLocator* pActionEventLocator )

Return:         ActionEventHandler

=============================================================================
*/
ActionEventHandler::ActionEventHandler( ActionEventLocator* pActionEventLocator )
:
mpActionEventLocator( 0 ),
mIsEnabled( true )
{
    SetActionEventLocator( pActionEventLocator );
}
/*
==============================================================================
ActionEventHandler::~ActionEventHandler
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ActionEventHandler

=============================================================================
*/
ActionEventHandler::~ActionEventHandler( void )
{
    tRefCounted::Release( mpActionEventLocator );
    mpActionEventLocator = 0;
}
#ifdef ACTIONEVENTHANDLER_DEBUG
void ActionEventHandler::Enter( Character* pCharacter )
{    
    if ( mpActionEventLocator )
    {
        rReleasePrintf( "Object:\t%s\nJoint:\t%s\nAction:\t%s\nButton:\t%s\nShouldTransform:\t%d\n",
            mpActionEventLocator->GetObjName(),
            mpActionEventLocator->GetJointName(),
            mpActionEventLocator->GetActionName(),
            ButtonName[ mpActionEventLocator->GetButtonInput() ],
            mpActionEventLocator->GetShouldTransform() ? 1 : 0 );
    }
    ButtonHandler::Enter( pCharacter );
}
#endif //ACTIONEVENTHANDLER_DEBUG
/*
==============================================================================
ActionEventHandler::SetActionEventLocator
==============================================================================
Description:    Comment

Parameters:     ( ActionEventLocator* pActionEventLocator )

Return:         void 

=============================================================================
*/
void ActionEventHandler::SetActionEventLocator( ActionEventLocator* pActionEventLocator )
{
    tRefCounted::Assign( mpActionEventLocator, pActionEventLocator );
}
/*
==============================================================================
AnimSwitch::AnimSwitch
==============================================================================
Description:    Comment

Parameters:     ( ActionEventLocator* pActionEventLocator )

Return:         AnimSwitch

=============================================================================
*/
AnimSwitch::AnimSwitch( ActionEventLocator* pActionEventLocator )
:
ActionEventHandler( pActionEventLocator ),
mpJoint( 0 ),
mpGameObject( 0 ),
mbAttachToJoint( false )
{
    unsigned int i;
    const char* objName = pActionEventLocator->GetObjName();
    radKey32 objNameKey;

    mSoundName = NULL;

    if( objName != NULL )
    {
        //
        // Trapdoor hack!  This is ugly, but not quite as ugly as listing
        // each trapdoor separately in the table, I think.
        //
        if( ( objName[0] == 'T' ) && ( objName[1] == 'D' ) )
        {
            mSoundName = ANIM_SWITCH_SOUND_TABLE[0].soundName;
            mIsMovingSound = ANIM_SWITCH_SOUND_TABLE[0].isMovingSound;
            mSettingsName = ANIM_SWITCH_SOUND_TABLE[0].positionalSettings;
        }
        else
        {
            objNameKey = ::radMakeKey32( objName );

            //
            // Set the sound key 
            //
            for( i = 0; i < ANIM_SWITCH_SOUND_TABLE_SIZE; i++ )
            {
                if( objNameKey == ANIM_SWITCH_SOUND_TABLE[i].objKey )
                {
                    mSoundName = ANIM_SWITCH_SOUND_TABLE[i].soundName;
                    mIsMovingSound = ANIM_SWITCH_SOUND_TABLE[i].isMovingSound;
                    mSettingsName = ANIM_SWITCH_SOUND_TABLE[i].positionalSettings;
                    break;
                }
            }
        }
    }
}
/*
==============================================================================
AnimSwitch::~AnimSwitch
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         AnimSwitch

=============================================================================
*/
AnimSwitch::~AnimSwitch( void )
{
    mpJoint = 0;
    tRefCounted::Release( mpGameObject );
}

/*
==============================================================================
AnimSwitch::Create
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         bool 

=============================================================================
*/
bool AnimSwitch::Create( tEntityStore* inStore )
{
    AnimCollisionEntityDSG* pAnimCollisionEntityDSG = p3d::find<AnimCollisionEntityDSG>( inStore, mpActionEventLocator->GetObjName() );
    bool bCreated = false;
    if ( pAnimCollisionEntityDSG )
    {
        tPose* p3dPose = pAnimCollisionEntityDSG->GetPoseEngine()->GetP3DPose( );
        rAssert( p3dPose );

        // Evaluate the pose to get the proper worldmatrix for the joints.
        //
        p3dPose->Evaluate();
        int jointIndex = p3dPose->FindJointIndex( mpActionEventLocator->GetJointName() ); 

        tPose::Joint* pJoint = pAnimCollisionEntityDSG->GetPoseJoint( jointIndex, mpActionEventLocator->GetShouldTransform() );

        ActionButton::AnimCollisionEntityDSGWrapper* pGameObject = new(GMA_LEVEL_OTHER) ActionButton::AnimCollisionEntityDSGWrapper;
        pGameObject->SetGameObject( pAnimCollisionEntityDSG );
        // Set the start state of all animations requiring a trigger to off.
        //
        // This will stop the animation until Action executes.
        //
        pGameObject->SetAnimationDirection( 0.0f );
        // Reset the animation to the initial frame.
        //
        pGameObject->GetAnimController( )->SetFrame( 0.0f );

        // The AnimCollisionEntityDSG must store a pointer to the action handler
        // so it can callback when it is dumped by the dynaloader.
        //
        // TBJ [8/14/2002] 
        //
        pAnimCollisionEntityDSG->SetAction( this );
        Init( pGameObject, pJoint, mpActionEventLocator->GetShouldTransform() );
        bCreated = true;
    }    
    return bCreated;
}
//=============================================================================
// AnimSwitch::OnReset
//=============================================================================
// Description: Comment
//
// Parameters:  ( void )
//
// Return:      void 
//
//=============================================================================
void AnimSwitch::OnReset( void )
{
    // Set the start state of all animations requiring a trigger to off.
    //
    // This will stop the animation until Action executes.
    //
    mpGameObject->SetAnimationDirection( 0.0f );
    // Reset the animation to the initial frame.
    //
    mpGameObject->GetAnimController( )->SetFrame( 0.0f );
    mpGameObject->GetAnimController( )->Advance( 0.0f );
    mpGameObject->UpdateVisibility( );
}
/*
==============================================================================
AnimSwitch::Init
==============================================================================
Description:    Comment

Parameters:     ( AnimCollisionEntityDSG* pAnimCollisionEntityDSG, tPose::Joint* pJoint, bool bAttachToJoint )

Return:         void 

=============================================================================
*/
void AnimSwitch::Init( IGameObjectWrapper* pGameObject, tPose::Joint* pJoint, bool bAttachToJoint )
{
    mbAttachToJoint = bAttachToJoint;
    mpJoint = pJoint; 
    rAssert( mpJoint );
    
    tRefCounted::Assign( mpGameObject, pGameObject );

    ActionEventLocator* pActionEventLocator = GetActionEventLocator();
    rAssert( pActionEventLocator );
    pActionEventLocator->GetLocation( &mStandPosition );
    if ( mbAttachToJoint )
    {
        // If attach to joint, then transform the eventlocator into 
        // the object local space.  Store it in mStandPosition.
        // Then we will transform mStandPosition back to 
        // world space when we need the position.
        //
        rmt::Matrix invMat = mpJoint->worldMatrix;
        invMat.InvertOrtho( );
        mStandPosition.Transform( invMat );
    }
    OnInit( );
}
/*
==============================================================================
AnimSwitch::GetAnimationDirection
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         float

=============================================================================
*/
float& AnimSwitch::GetAnimationDirection( void )
{
    return mpGameObject->GetAnimationDirection();
}
/*
==============================================================================
AnimSwitch::SetAnimationDirection
==============================================================================
Description:    Comment

Parameters:     ( float fDirection )

Return:         void 

=============================================================================
*/
void AnimSwitch::SetAnimationDirection( float fDirection )
{
    mpGameObject->SetAnimationDirection( fDirection );
}
/*
==============================================================================
AnimSwitch::GetAnimController
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         tMultiController

=============================================================================
*/
tMultiController* AnimSwitch::GetAnimController( void ) const
{
    return mpGameObject->GetAnimController();
}
/*
==============================================================================
AnimSwitch::Update
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void AnimSwitch::Update( float timeins )
{
    if ( mbAttachToJoint )
    {
        rAssert( mpJoint );
        ActionEventLocator* pActionEventLocator = GetActionEventLocator( );
        rAssert( pActionEventLocator );
        
        rmt::Vector pos = mpJoint->worldMatrix.Row( 3 );
    
        pActionEventLocator->GetTriggerVolume( 0 )->SetPosition( pos );
    }
    OnUpdate( timeins );
}
/*
==============================================================================
AnimSwitch::ReleaseGameObject
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void AnimSwitch::Destroy( void ) 
{
}
/*
==============================================================================
AnimSwitch::ButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
bool AnimSwitch::ButtonPressed( Character* pCharacter )
{
    CharacterController::eIntention theIntention = pCharacter->GetController()->GetIntention();
    
    if ( IsActionButtonPressed( pCharacter ) )
    {
        pCharacter->GetActionController()->Clear();
        Sequencer* pSeq = pCharacter->GetActionController()->GetNextSequencer();
        if ( OnButtonPressed( pCharacter, pSeq ) )
        {
            PositionCharacter( pCharacter, pSeq );
            SetAnimation( pCharacter, pSeq );
            
            if ( UsesActionButton() )
            {
//                GetGuiSystem()->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_ACTION_BUTTON );
            }

            return true;
        }
        
    }
    return false;
}

/*
==============================================================================
AnimSwitch::PositionCharacter
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void AnimSwitch::PositionCharacter( Character* pCharacter, Sequencer* pSeq )
{

    rmt::Vector standPosition = mStandPosition;
    if ( mbAttachToJoint )
    {
        // If attach to joint, then the locator has been transformed into
        // object local space.  Here we need to transform the point back
        // into world space, but maintain the object local space of the locator,
        // so we transform a copy of the position.
        //
        rmt::Matrix mat = mpJoint->worldMatrix;
        standPosition.Transform( mat );
    }  
       
    Action* pAction = 0;
        
    // Orient.
    //
    rAssert( mpJoint );

    rmt::Vector direction = GetActionEventLocator()->GetMatrix().Row(2);

    // If direction != 0, 0, 0, then rotate us to align with direction.
    //
    if ( direction.NormalizeSafe( ) )
    {
        pSeq->BeginSequence();
        pAction = new Orient( pCharacter, direction );
        pSeq->AddAction( pAction );
        // SlaveLoco
        //
        pAction = 0;
    
        pAction = pCharacter->GetWalkerLocomotionAction();
	    
	    pSeq->AddAction( pAction );
        pSeq->EndSequence( );
    }
           
    // Play run_into_object.
    //
    pSeq->BeginSequence();
    pAction = new Position( pCharacter, standPosition, 0.1f );
    pSeq->AddAction( pAction );
    pSeq->EndSequence( );
}
/*
==============================================================================
AnimSwitch::SequenceActions
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, Sequencer* pSeq )

Return:         void 

=============================================================================
*/
void AnimSwitch::SequenceActions( Character* pCharacter, Sequencer* pSeq )
{
}

/*
==============================================================================
AnimSwitch::SetAnimation
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, Sequencer* pSeq )

Return:         void 

=============================================================================
*/
void AnimSwitch::SetAnimation( Character* pCharacter, Sequencer* pSeq )
{
    Action* pAction = 0;
    pAction = new PlayAnimationAction( pCharacter, "hit_switch_quick" );
    pSeq->AddActionToSequence( pAction );
    Action* eventAction = new TriggerEventAction( EVENT_BIG_RED_SWITCH_PRESSED, (void*)0 );
    pSeq->AddActionToSequence( 0.3f, -1.0f, eventAction );
}
/*
==============================================================================
ToggleAnim::ToggleAnim
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
ToggleAnim::ToggleAnim( ActionEventLocator* pActionEventLocator )
:
AnimSwitch( pActionEventLocator )
{  
}
/*
==============================================================================
ToggleAnim::~ToggleAnim
==============================================================================
Description:    Comment

Parameters:     ()

Return:         ToggleAnim

=============================================================================
*/
ToggleAnim::~ToggleAnim()
{
    //
    // Make good and sure this thing isn't going to play sound anymore
    //
    GetEventManager()->TriggerEvent( EVENT_STOP_ANIMATION_SOUND, this );
}

/*
==============================================================================
ToggleAnim::OnButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, Sequencer* pSeq )

Return:         void 

=============================================================================
*/
bool ToggleAnim::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{
    Action* pAction = 0;
    float fDirection = 0.0f;
    if ( GetAnimationDirection( ) != 0.0f )
    {
        fDirection = 0.0f;
    }
    else
    {
        fDirection = 1.0f;
    }
    tMultiController* pAnimController = GetAnimController( );
    pAnimController->SetCycleMode( FORCE_CYCLIC );

    pSeq->BeginSequence();
    pAction = new AssignValueToFloat( GetAnimationDirection(), fDirection );
    pSeq->AddAction( pAction );
    pSeq->EndSequence( );

    if( mSoundName != NULL )
    {
        if( fDirection == 0.0f )
        {
            GetEventManager()->TriggerEvent( EVENT_STOP_ANIMATION_SOUND, this );
        }
        else
        {
            AnimSoundData data( mSoundName, mSettingsName );

            if( mIsMovingSound )
            {
                tPoseAnimationController* controller;
                tPose::Joint theJoint;

                //
                // Platform.  Yank the joint out of the animation stuff so that the
                // moving sound code can calculate a position for it.
                //
                rAssert( dynamic_cast<tPoseAnimationController*>( pAnimController->GetTrack( 0 ) ) );
                controller = static_cast<tPoseAnimationController*>( pAnimController->GetTrack( 0 ) );

                rAssert( controller->GetPose()->GetNumJoint() >= 2 );
                data.animJoint = controller->GetPose()->GetJoint( 1 );

                data.soundObject = this;
            }

            GetEventManager()->TriggerEvent( EVENT_START_ANIMATION_SOUND, &data );
        }
    }

    return true;
}

/*
==============================================================================
ReverseAnim::OnButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, Sequencer* pSeq )

Return:         void 

=============================================================================
*/
bool ReverseAnim::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{
    Action* pAction = 0;
    float fDirection = 0.0f;
    if ( GetAnimationDirection( ) == 1.0f )
    {
        fDirection = -1.0f;
    }
    else
    {
        fDirection = 1.0f;
    }
    tMultiController* pAnimController = GetAnimController( );
    pAnimController->SetCycleMode( FORCE_CYCLIC );

    pSeq->BeginSequence();
    pAction = new AssignValueToFloat( GetAnimationDirection( ), fDirection );
    pSeq->AddAction( pAction );
    pSeq->EndSequence( );
    return true;
}

/*
==============================================================================
PlayAnim::PlayAnim
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
PlayAnim::PlayAnim( ActionEventLocator* pActionEventLocator )
:
AnimSwitch( pActionEventLocator )
{  
}
/*
==============================================================================
PlayAnim::~PlayAnim
==============================================================================
Description:    Comment

Parameters:     ()

Return:         PlayAnim

=============================================================================
*/
PlayAnim::~PlayAnim()
{
    //
    // Make good and sure this thing isn't going to play sound anymore
    //
    GetEventManager()->TriggerEvent( EVENT_STOP_ANIMATION_SOUND, this );
}


/*
==============================================================================
PlayAnim::OnButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, Sequencer* pSeq )

Return:         void 

=============================================================================
*/
bool PlayAnim::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{
    tMultiController* pAnimController = GetAnimController( );
    bool bRestart = true;
    // If the animation has already been triggered.
    //
    if ( GetAnimationDirection( ) == 1.0f )
    {
        // Test to see if the animation has finished.
        //
        if ( !pAnimController->LastFrameReached() )
        {
            // Don't perform the action.
            //
            bRestart = false;
        }
    }
    if ( bRestart )
    {
        // This will stop the animation until Action executes.
        //
        SetAnimationDirection( 0.0f );
        // Reset the animation to the initial frame.
        // This assumes that the animation first frame is equal to last frame.
        //
        pAnimController->SetFrame( 0.0f );

        // Force the animation to be NOT cyclic.
        //
        pAnimController->SetCycleMode( FORCE_NON_CYCLIC );

        
        // Action will assign this value which will start the animation.
        //
        Action* pAction = 0;
        float fDirection = 1.0f;
        pSeq->BeginSequence();
        pAction = new AssignValueToFloat( GetAnimationDirection( ), fDirection );
        pSeq->AddAction( pAction );
        pSeq->EndSequence( );

        if( mSoundName != NULL )
        {
            AnimSoundData data( mSoundName, mSettingsName );

            if( mIsMovingSound )
            {
                tPoseAnimationController* controller;
                tPose::Joint theJoint;

                //
                // Platform.  Yank the joint out of the animation stuff so that the
                // moving sound code can calculate a position for it.
                //
                rAssert( dynamic_cast<tPoseAnimationController*>( pAnimController->GetTrack( 0 ) ) );
                controller = static_cast<tPoseAnimationController*>( pAnimController->GetTrack( 0 ) );

                data.animJoint = controller->GetPose()->GetJoint( 4 );

                data.soundObject = this;
            }

            GetEventManager()->TriggerEvent( EVENT_START_ANIMATION_SOUND, &data );
        }
    }
    return bRestart;
}
/*
==============================================================================
PlayAnimLoop::PlayAnimLoop
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
PlayAnimLoop::PlayAnimLoop( ActionEventLocator* pActionEventLocator )
:
AnimSwitch( pActionEventLocator )
{  
}
/*
==============================================================================
PlayAnimLoop::~PlayAnimLoop
==============================================================================
Description:    Comment

Parameters:     ()

Return:         PlayAnimLoop

=============================================================================
*/
PlayAnimLoop::~PlayAnimLoop()
{
}


/*
==============================================================================
PlayAnimLoop::OnButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, Sequencer* pSeq )

Return:         void 

=============================================================================
*/
bool PlayAnimLoop::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{
    // Only do this once.
    //
    if ( GetAnimationDirection( ) == 0.0f )
    {

        // Force the animation to be cyclic.
        //
        tMultiController* pAnimController = GetAnimController( );
        pAnimController->SetCycleMode( FORCE_CYCLIC );

        // Action will assign this value which will start the animation.
        //
        Action* pAction = 0;
        float fDirection = 1.0f;
        pSeq->BeginSequence();
        pAction = new AssignValueToFloat( GetAnimationDirection( ), fDirection );
        pSeq->AddAction( pAction );
        pSeq->EndSequence( );
        return true;
    }
    return false;
}

/*
==============================================================================
AutoPlayAnim::AutoPlayAnim
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
AutoPlayAnim::AutoPlayAnim( ActionEventLocator* pActionEventLocator )
:
AnimSwitch( pActionEventLocator ),
mCharacterCount( 0 ),
mbJustEmpty( false )
{  
    SetActionButton( CharacterController::NONE );
}
/*
==============================================================================
AutoPlayAnim::~AutoPlayAnim
==============================================================================
Description:    Comment

Parameters:     ()

Return:         AutoPlayAnim

=============================================================================
*/
AutoPlayAnim::~AutoPlayAnim()
{
    //
    // Make good and sure this thing isn't going to play sound anymore
    //
    GetEventManager()->TriggerEvent( EVENT_STOP_ANIMATION_SOUND, this );
}

/*
==============================================================================
AutoPlayAnim::IsActionButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         bool 

=============================================================================
*/
bool AutoPlayAnim::IsActionButtonPressed( Character* pCharacter )
{
    // These are auto volumes.  No buttons.
    //
    return false;
}

/*
==============================================================================
AutoPlayAnim::IsActionButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         bool 

=============================================================================
*/
bool AutoPlayAnim::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{   
    return false;
}
/*
==============================================================================
AutoPlayAnim::OnEnter
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void AutoPlayAnim::OnEnter( Character* pCharacter )
{
    if ( mCharacterCount == 0 )
    {
        tMultiController* pAnimController = GetAnimController( );
        // First person in the volume.
        //
        // Reset the animation to the initial frame.
        // This assumes that the animation first frame is equal to last frame.
        //
        pAnimController->SetFrame( 0.0f );

        // This will start the animation.
        //
        SetAnimationDirection( 1.0f );

        // Force the animation to be cyclic dependent on the class type.
        // GetIsCyclic is virtual.
        //
        pAnimController->SetCycleMode( GetIsCyclic() ? FORCE_CYCLIC : FORCE_NON_CYCLIC );

        if( mSoundName != NULL )
        {
            AnimSoundData data( mSoundName, mSettingsName );

            if( mIsMovingSound )
            {
                tPoseAnimationController* controller;
                tPose::Joint theJoint;

                //
                // Platform.  Yank the joint out of the animation stuff so that the
                // moving sound code can calculate a position for it.
                //
                rAssert( dynamic_cast<tPoseAnimationController*>( pAnimController->GetTrack( 0 ) ) );
                controller = static_cast<tPoseAnimationController*>( pAnimController->GetTrack( 0 ) );

                rAssert( controller->GetPose()->GetNumJoint() >= 2 );
                data.animJoint = controller->GetPose()->GetJoint( 1 );

                data.soundObject = this;
            }

            GetEventManager()->TriggerEvent( EVENT_START_ANIMATION_SOUND, &data );
        }
    }
    mCharacterCount++;
}
/*
==============================================================================
AutoPlayAnim::OnUpdate
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void AutoPlayAnim::OnUpdate( float timeins )
{
    if ( mbJustEmpty )
    {
        // Test to see if the animation has finished.
        //
        tMultiController* pAnimController = GetAnimController( );
        if ( pAnimController->LastFrameReached() )
        {        
            mbJustEmpty = false;
            // The animation is finished.  Reactivate the trigger volume.
            //
            mpActionEventLocator->SetFlag( Locator::ACTIVE, true );   

            GetEventManager()->TriggerEvent( EVENT_STOP_ANIMATION_SOUND, this );
        }
    }
}
/*
==============================================================================
AutoPlayAnim::OnExit
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void AutoPlayAnim::OnExit( Character* pCharacter )
{
    mCharacterCount--;
    if ( mCharacterCount == 0 )
    {
        tMultiController* pAnimController = GetAnimController( );

        // Force the animation to be NOT cyclic.
        //
        pAnimController->SetCycleMode( FORCE_NON_CYCLIC );

        mbJustEmpty = true;

        // Deactivate the trigger volume.  Give the animation a chance to finish.
        //
        mpActionEventLocator->SetFlag( Locator::ACTIVE, false );  
    }
    rAssert( mCharacterCount >= 0 );
}

/*
==============================================================================
AutoPlayAnim::PositionCharacter
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void AutoPlayAnim::PositionCharacter( Character* pCharacter, Sequencer* pSeq )
{
}
/*
==============================================================================
AutoPlayAnim::SetAnimation
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void AutoPlayAnim::SetAnimation( Character* pCharacter, Sequencer* pSeq )
{
}
/*
==============================================================================
AutoPlayAnimLoop::AutoPlayAnimLoop
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
AutoPlayAnimLoop::AutoPlayAnimLoop( ActionEventLocator* pActionEventLocator )
:
AutoPlayAnim( pActionEventLocator )
{  
}
/*
==============================================================================
AutoPlayAnimLoop::~AutoPlayAnimLoop
==============================================================================
Description:    Comment

Parameters:     ()

Return:         AutoPlayAnimLoop

=============================================================================
*/
AutoPlayAnimLoop::~AutoPlayAnimLoop()
{
}

/*
==============================================================================
AutoPlayAnimInOut::AutoPlayAnimInOut
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
AutoPlayAnimInOut::AutoPlayAnimInOut( ActionEventLocator* pActionEventLocator )
:
AutoPlayAnim( pActionEventLocator )
{  
}
/*
==============================================================================
AutoPlayAnimInOut::~AutoPlayAnimInOut
==============================================================================
Description:    Comment

Parameters:     ()

Return:         AutoPlayAnimInOut

=============================================================================
*/
AutoPlayAnimInOut::~AutoPlayAnimInOut()
{
}

/*
==============================================================================
AutoPlayAnimInOut::IsActionButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         bool 

=============================================================================
*/
bool AutoPlayAnimInOut::IsActionButtonPressed( Character* pCharacter )
{
    return false;
}

/*
==============================================================================
AutoPlayAnimInOut::IsActionButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         bool 

=============================================================================
*/
bool AutoPlayAnimInOut::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{   
    return false;
}

/*
==============================================================================
AutoPlayAnimInOut::OnUpdate
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void AutoPlayAnimInOut::OnUpdate( float timeins )
{
    if ( mbJustEmpty )
    {
        // Test to see if the animation has finished.
        //
        tMultiController* pAnimController = GetAnimController( );
        if ( pAnimController->LastFrameReached() )
        {
            // This will reverse the animation.
            //
            SetAnimationDirection( -1.0f );
          
 
        }
        else if ( pAnimController->GetFrame() == 0.0f )
        {
            // We have played back to the start.
            //

            mbJustEmpty = false;
            // The animation is finished.  Reactivate the trigger volume.
            //
            mpActionEventLocator->SetFlag( Locator::ACTIVE, true );  
        }
    }
}

/*
==============================================================================
AutoPlayAnimInOut::OnExit
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void AutoPlayAnimInOut::OnExit( Character* pCharacter )
{
    mCharacterCount--;
    if ( mCharacterCount == 0 )
    {
        mbJustEmpty = true;
        // Deactivate the trigger volume.  Give the animation a chance to finish.
        //
        mpActionEventLocator->SetFlag( Locator::ACTIVE, false );  
    }
    rAssert( mCharacterCount >= 0 );
}
/*
==============================================================================
DestroyObject::DestroyObject
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
DestroyObject::DestroyObject( ActionEventLocator* pActionEventLocator )
:
AnimSwitch( pActionEventLocator ),
mbDestroyed( false ),
mpStatePropDSG( NULL )
{  
}
/*
==============================================================================
DestroyObject::~DestroyObject
==============================================================================
Description:    Comment

Parameters:     ()

Return:         DestroyObject

=============================================================================
*/
DestroyObject::~DestroyObject()
{
    if ( mpStatePropDSG != NULL )
    {
        mpStatePropDSG->RemoveStatePropListener( this );
    }
}

bool
DestroyObject::Create( tEntityStore* inStore )
{
    bool success;
    // Try and find the corresponding InstAnimDynaPhysDSG object in the inventory.
  //  InstAnimDynaPhysDSG* pDSG = p3d::find< InstAnimDynaPhysDSG > ( inStore, mpActionEventLocator->GetObjName() );
    StatePropDSG* pDSG = p3d::find< StatePropDSG >( inStore, mpActionEventLocator->GetObjName() );

    if ( pDSG != NULL )
    {
        mpStatePropDSG = pDSG;
        mpStatePropDSG->AddStatePropListener( this );
        mpStatePropDSG->SetState(0);

        SetInstanceEnabled( false );

        success = true;
    }
    else
    {
        rTuneWarning("ERROR : DestroyObject::Create() could not find DSG object in inventory");
        success = false;
    }

	return success;
}
bool 
DestroyObject::NeedsUpdate( void ) const
{
    // Plain destroy objects need no per-frame updating
    return false;
}

void DestroyObject::Enter( Character* pCharacter )
{
    // don't let the action handler get registered of object is destroyed
    // TODO : should really save processing by removing trigger from trigger volume tracker
    if(!mbDestroyed)
    {
        AnimSwitch::Enter(pCharacter);
    }
}

//=============================================================================
// DestroyObject::OnReset
//=============================================================================
// Description: Comment
//
// Parameters:  ( void )
//
// Return:      void 
//
//=============================================================================
void DestroyObject::OnReset( void )
{
    // We want to respawn the stateprops and reset their state.
    // So even if the user stomped and kicked them out of existance before the
    // mission started, they would have magically respawned.
    
    if ( mpStatePropDSG != NULL )
    {
        mpStatePropDSG->SetState( 0 );
        mpStatePropDSG->EnableCollisionVolume( true );
    }
    mbDestroyed = false;
}
/*
==============================================================================
DestroyObject::OnButtonPressed==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, Sequencer* pSeq )

Return:         void 

=============================================================================
*/
bool DestroyObject::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{
    // Button press
    // Destroy the object immediately
    // Play a particle effect
    // Then remove it from the DSG 
    if ( mpStatePropDSG != NULL )
    {
        pCharacter->Kick();
    }
    return true;
}

/*
==============================================================================
DestroyObject::PositionCharacter
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void DestroyObject::PositionCharacter( Character* pCharacter, Sequencer* pSeq )
{

    if ( mbAttachToJoint )
    {
        ActionEventLocator* pActionEventLocator = GetActionEventLocator();
        if ( pActionEventLocator )
        {
            static float sfX = 0.2f;
            static float sfZ = -0.7f;
            rmt::Vector loc( sfX, 0.0f, sfZ );
            rmt::Matrix mat = mpJoint->worldMatrix;
            rmt::Vector pos = mpJoint->worldMatrix.Row( 3 );
            loc.Transform( mat );
            pActionEventLocator->SetLocation( loc );
        }
    }
    ActionEventLocator* pActionEventLocator = GetActionEventLocator( );
    rAssert( pActionEventLocator );
    rmt::Vector standPosition;
    pCharacter->GetPosition( standPosition );
    rmt::Vector direction;
    pActionEventLocator->GetLocation( &direction );  
    
    Action* pAction = 0;
    // Orient.
    //
    direction.Sub( standPosition );
    // If direction != 0, 0, 0, then rotate us to align with direction.
    //
    if ( direction.NormalizeSafe( ) )
    {
        pSeq->BeginSequence();
        pAction = new Orient( pCharacter, direction );
        pSeq->AddAction( pAction );
        // SlaveLoco
        //
        pAction = 0;
    
        pAction = pCharacter->GetWalkerLocomotionAction();
	    
	    pSeq->AddAction( pAction );
        pSeq->EndSequence( );
    }
}
/*
==============================================================================
DestroyObject::SetAnimation
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void DestroyObject::SetAnimation( Character* pCharacter, Sequencer* pSeq )
{
    Action* pAction = 0;
    pAction = new PlayAnimationAction( pCharacter, "break_quick" );
    pSeq->AddActionToSequence( pAction );
}

/*
==============================================================================
DestroyObject::OnUpdate
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void DestroyObject::OnUpdate( float timeins )
{
 
}

void DestroyObject::RecieveEvent( int callback , CStateProp* stateProp )
{
    const int REMOVE_FROM_WORLD_CALLBACK = 0;
    const int OBJECT_DESTROYED_CALLBACK = 10;
    if ( callback == REMOVE_FROM_WORLD_CALLBACK || callback == OBJECT_DESTROYED_CALLBACK )
    {
        rAssert( mpStatePropDSG != NULL );
        mbDestroyed = true;
        GetEventManager()->TriggerEvent( EVENT_OBJECT_DESTROYED, mpActionEventLocator );
        GetEventManager()->TriggerEvent( EVENT_BREAK_CAMERA_OR_BOX );
    }
}


/*
==============================================================================
UseVendingMachine::UseVendingMachine
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
UseVendingMachine::UseVendingMachine( ActionEventLocator* pActionEventLocator )
:
PlayAnim( pActionEventLocator )
{  
}
/*
==============================================================================
UseVendingMachine::~UseVendingMachine
==============================================================================
Description:    Comment

Parameters:     ()

Return:         UseVendingMachine

=============================================================================
*/
UseVendingMachine::~UseVendingMachine()
{
}


/*
==============================================================================
UseVendingMachine::OnButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, Sequencer* pSeq )

Return:         void 

=============================================================================
*/
bool UseVendingMachine::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{
    if ( PlayAnim::OnButtonPressed( pCharacter, pSeq ) )
    {
        // used to set turbo meter here, is this class even neccesary any more
        return true;
    }
    return false;
}
/*
==============================================================================
UseVendingMachine::SetAnimation
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void UseVendingMachine::SetAnimation( Character* pCharacter, Sequencer* pSeq )
{
    Action* pAction = 0;
    pAction = new PlayAnimationAction( pCharacter, "hit_switch_quick" );
    pSeq->AddActionToSequence( pAction );
}
/*
==============================================================================
PrankPhone::PrankPhone
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
PrankPhone::PrankPhone( ActionEventLocator* pActionEventLocator )
:
PlayAnim( pActionEventLocator )
{  
}
/*
==============================================================================
PrankPhone::~PrankPhone
==============================================================================
Description:    Comment

Parameters:     ()

Return:         PrankPhone

=============================================================================
*/
PrankPhone::~PrankPhone()
{
}


/*
==============================================================================
PrankPhone::OnButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, Sequencer* pSeq )

Return:         void 

=============================================================================
*/
bool PrankPhone::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{
    if ( PlayAnim::OnButtonPressed( pCharacter, pSeq ) )
    {
        // Setup the prank phone call.
        //
        return true;
    }
    return false;
}
/*
==============================================================================
PrankPhone::SetAnimation
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void PrankPhone::SetAnimation( Character* pCharacter, Sequencer* pSeq )
{
    Action* pAction = 0;
    pAction = new PlayAnimationAction( pCharacter, "break_quick" );
    pSeq->AddActionToSequence( pAction );
}

//=============================================================================
// Doorbell::Create
//=============================================================================
// Description: Comment
//
// Parameters:  ( tEntityStore* inStore )
//
// Return:      bool 
//
//=============================================================================
bool Doorbell::Create( tEntityStore* inStore )
{
    AnimCollisionEntityDSG* pAnimCollisionEntityDSG = p3d::find<AnimCollisionEntityDSG>( inStore, mpActionEventLocator->GetJointName() );
    bool bCreated = false;
    if ( pAnimCollisionEntityDSG )
    {
        tPose* p3dPose = pAnimCollisionEntityDSG->GetPoseEngine()->GetP3DPose( );
        rAssert( p3dPose );

        // Evaluate the pose to get the proper worldmatrix for the joints.
        //
        p3dPose->Evaluate();
        int jointIndex = p3dPose->FindJointIndex( mpActionEventLocator->GetJointName() ); 

        tPose::Joint* pJoint = pAnimCollisionEntityDSG->GetPoseJoint( jointIndex, mpActionEventLocator->GetShouldTransform() );

        ActionButton::AnimCollisionEntityDSGWrapper* pGameObject = new(GMA_LEVEL_OTHER) ActionButton::AnimCollisionEntityDSGWrapper;
        pGameObject->SetGameObject( pAnimCollisionEntityDSG );
        // Set the start state of all animations requiring a trigger to off.
        //
        // This will stop the animation until Action executes.
        //

        pAnimCollisionEntityDSG->GetAnimController()->SetCycleMode( FORCE_CYCLIC );
        pGameObject->SetAnimationDirection( 1.0f );

        Init( pGameObject, pJoint, false );
        bCreated = true;
    }    
    return bCreated;
}

//=============================================================================
// Doorbell::OnButtonPressed
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* pCharacter, Sequencer* pSeq )
//
// Return:      bool 
//
//=============================================================================
bool Doorbell::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{
    //if ( PlayAnim::OnButtonPressed( pCharacter, pSeq ) )
    {
        //Use the object name as the name for the sound as well.
        const char* charName = this->mpActionEventLocator->GetObjName();

        //Send this to the sound system.
        if( charName != NULL )
        {
            GetEventManager()->TriggerEvent( EVENT_DING_DONG, const_cast<char*>(&charName[3]) );
        }

        return true;
    }
    return false;   
}

bool SummonVehiclePhone::sbEnabled = true;
int SummonVehiclePhone::CarSelectionInfo::sNumUsedSlots = 0;
//int SummonVehiclePhone::sSelectedVehicle = INVALID_VEHICLE;
char SummonVehiclePhone::sSelectedVehicleName[ SummonVehiclePhone::MAX_VEHICLE_NAME_LENGTH ];
SummonVehiclePhone::CarSelectionInfo SummonVehiclePhone::sCarSelectInfo[ NUM_CAR_SELECTION_SLOTS ];
SummonVehiclePhoneStaticCallback SummonVehiclePhone::sCallback;
/*
==============================================================================
SummonVehiclePhone::SummonVehiclePhone
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
SummonVehiclePhone::SummonVehiclePhone( ActionEventLocator* pActionEventLocator )
:   ActionEventHandler( pActionEventLocator ),
    mHudMapIconID( -1 )
{
    rmt::Vector pos;
    pActionEventLocator->GetLocation( &pos );
    mPhoneIcon.Init( "phone_icon", pos );

    // register phone booth icon on HUD map
    //
    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud != NULL )
    {
        mHudMapIconID = currentHud->GetHudMap( 0 )->RegisterIcon( HudMapIcon::ICON_PHONE_BOOTH, pos );
    }

}
/*
==============================================================================
SummonVehiclePhone::~SummonVehiclePhone
==============================================================================
Description:    Comment

Parameters:     ()

Return:         SummonVehiclePhone

=============================================================================
*/
SummonVehiclePhone::~SummonVehiclePhone()
{
    // un-register phone booth icon
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

    // release event locator
    //
    if( sCallback.mpActionEventLocator != NULL )
    {
        sCallback.mpActionEventLocator->Release();
        sCallback.mpActionEventLocator = NULL;
    }
}

//=============================================================================
// SummonVehiclePhone::Create
//=============================================================================
// Description: Comment
//
// Parameters:  ( tEntityStore* inStore /* = 0  */ )
//
// Return:      bool 
//
//=============================================================================
bool SummonVehiclePhone::Create( tEntityStore* inStore /* = 0  */ )
{
    tUID nameUID = tEntity::MakeUID(mpActionEventLocator->GetObjName());
    unsigned int i;
    for ( i = 0; i < ActionButtonManager::MAX_ACTIONS; ++i )
    {
        if ( GetActionButtonManager()->GetActionByIndex( i ) )
        {
            ActionButton::ButtonHandler* bh = GetActionButtonManager()->GetActionByIndex( i );

            if ( bh && bh->GetType() == ActionButton::ButtonHandler::SUMMON_PHONE )
            {
                rAssert( dynamic_cast<ActionButton::SummonVehiclePhone*>(bh) != NULL );
                ActionButton::SummonVehiclePhone* svp = static_cast<ActionButton::SummonVehiclePhone*>(bh);
                rAssert( svp );
                if ( tEntity::MakeUID(svp->GetActionEventLocator()->GetObjName()) == nameUID )
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool
SummonVehiclePhone::UsesActionButton() const
{
    bool isPhoneEnabled = GetGameplayManager()->QueryPhoneBoothsEnabled();
    return isPhoneEnabled;
}

/*
==============================================================================
SummonVehiclePhone::DumpVehicle
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void SummonVehiclePhone::DumpVehicle( void )
{
    rAssertMsg( false, "*** DEPRECATED FUNCTION! ***" );
/*
    if ( INVALID_VEHICLE != GetSelectedVehicleIndex( ) )
    {
        // A vehicle is already loaded.  We need to dump a vehicle.
        //
        CarSelectionInfo* pInfo = GetCarSelectInfo( GetSelectedVehicleIndex( ) );
        rAssert( pInfo );
        if ( pInfo )
        {
            // Dump the vehicle.
            //
            Vehicle* pVehicle = GetVehicleCentral()->GetVehicleByName( pInfo->GetVehicleName() );
            if ( pVehicle )
            {
                GetVehicleCentral()->RemoveVehicleFromActiveList( pVehicle );
            }

            pVehicle->ReleaseVerified();
            // Dump the inventory associated with this vehicle.

            
            p3d::inventory->RemoveSectionElements( pInfo->GetFileName() );
            p3d::inventory->DeleteSection( pInfo->GetFileName() );
        }
    }
*/
}
/*
==============================================================================
SummonVehiclePhone::OnButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, Sequencer* pSeq )

Return:         void 

=============================================================================
*/
bool SummonVehiclePhone::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{
    //chuck
    //we cant enter the phonebooth if we are in the paused state due to mission fail 
    //since this will generate a stack overflow and game death
    if  ( 
                GetGameFlow()->GetNextContext()  == CONTEXT_PAUSE
            ||
                GetGameplayManager()->GetCurrentMission()->GetState() ==Mission::STATE_FAILED
        )
    {
        return false;
    }

    if( GetGuiSystem()->GetInGameManager()->IsEnteringPauseMenu() )
    {
        // if about to enter pause menu, ignore button press
        //
        return false;
    }

    //check if we can use the phone booth.
    if ( sbEnabled && GetGameplayManager()->QueryPhoneBoothsEnabled() )
    {
        // clear out any unneeded characters here (mainly to remove characters 
        // from completed bonus missions in case we need their car)
        GetCharacterManager()->GarbageCollect(true);

        // We do some of the work here, then we will trigger an event to do the menu.
        // The menu will trigger the loading.
        //

        //Chuck: We update the users vehicle's Hit points in the Charactersheet
        int CarIndex = -1;
        char car_name [16];
        car_name[0] = '\0';
        strcpy(car_name,GetGameplayManager()->GetVehicleSlotVehicleName(GameplayManager::eDefaultCar));
        CarIndex = GetCharacterSheetManager()->GetCarIndex(car_name);
        
        //check if the players default car exists

        if ( GetGameplayManager()->GetVehicleInSlot(GameplayManager::eDefaultCar) != NULL)
        {
            //update 
            float currentCarHitpoints = GetGameplayManager()->GetVehicleInSlot(GameplayManager::eDefaultCar)->mHitPoints;
            GetCharacterSheetManager()->UpdateCarHealth(CarIndex,GetGameplayManager()->GetVehicleInSlot(GameplayManager::eDefaultCar)->GetVehicleLifePercentage(currentCarHitpoints));
        }
        else
        {
            if (GetGameplayManager()->GetVehicleInSlot(GameplayManager::eOtherCar) != NULL)
            {
                //we are in a forced car mission do nothing since player may not repair cars in a forced car mission.
            }
        }



        tRefCounted::Assign( sCallback.mpActionEventLocator, mpActionEventLocator );
        
        // From this point we should just trigger an event to throw up a 
        // menu of cars to choose from.  Selecting from the menu will trigger the load.
        //
        GetGuiSystem()->HandleMessage( GUI_MSG_GOTO_SCREEN,
                                       CGuiWindow::GUI_SCREEN_ID_PHONE_BOOTH );

        const rmt::Matrix& matrix = mpActionEventLocator->GetMatrix();
        SuperCam* sc = GetSuperCamManager()->GetSCC( 0 )->GetSuperCam( SuperCam::RELATIVE_ANIMATED_CAM );
        RelativeAnimatedCam* rac = dynamic_cast< RelativeAnimatedCam* >( sc );
        rmt::Matrix m2 = matrix;
        rmt::Matrix rotate;
        rotate.Identity();
        rotate.FillRotateY( rmt::PI / 2 );
        m2.Mult( rotate );
        const rmt::Vector& translation = matrix.Row( 3 );
        m2.FillTranslate( translation );
        rac->SetOffsetMatrix( m2 );

        // switch to pause context
        //
        GetGameFlow()->SetContext( CONTEXT_PAUSE );

        return true;
    }
    else
    {
        // TC: [TODO] trigger 'busy signal' event
        //
        GetEventManager()->TriggerEvent( EVENT_PHONE_BOOTH_BUSY );

        return false;
    }
}

/*
==============================================================================
SummonVehiclePhone::SelectLoadedVehicle
==============================================================================
Description:    Comment

Parameters:     ( int selectedVehicleIndex )

Return:         void 

=============================================================================
*/
/*
void SummonVehiclePhone::SelectLoadedVehicle( int selectedVehicleIndex )
{
    //HACK: this a perpetuation of Trav's original Billboardwrappedloader hack.
    //dyna loading duriong a car load will now be BAD. -dm
    ((BillboardWrappedLoader*)GetAllWrappers()->mpLoader(AllWrappers::msBillboard))->OverrideLoader(true);
    //user has confirmed vehicle switch, so we must dump cars
    //DumpVehicle();
    GetGameplayManager()->DumpCurrentCar();

//    sSelectedVehicle = selectedVehicleIndex;
    CarSelectionInfo* pInfo = GetCarSelectInfo( GetSelectedVehicleIndex( ) );
    rAssert( pInfo );
    if ( pInfo )
    {
        //update the Gameplaymanager about new car getting loaded into the Other slot.
        strcpy(GetGameplayManager()->mVehicleSlots[GameplayManager::eOtherCar].name,pInfo->GetVehicleName());
        strcpy(GetGameplayManager()->mVehicleSlots[GameplayManager::eOtherCar].filename,pInfo->GetFileName());

        sCallback.OnProcessRequestsComplete( NULL );
    }
}
*/

/*
==============================================================================
SummonVehiclePhone::LoadVehicle
==============================================================================
Description:    Comment

Parameters:     ( const char* filename )

Return:         void 

=============================================================================
*/
void SummonVehiclePhone::LoadVehicle( const char* name, const char* filename, VehicleCentral::DriverInit driver)
{
    //HACK: this a perpetuation of Trav's original Billboardwrappedloader hack.
    //dyna loading duriong a car load will now be BAD. -dm
    ((BillboardWrappedLoader*)GetAllWrappers()->mpLoader(AllWrappers::msBillboard))->OverrideLoader(true);

    //user has confirmed vehicle switch, so we must dump cars
    //DumpVehicle();
    GetGameplayManager()->DumpCurrentCar();
    rmt::Vector position;
    GetAvatarManager()->GetAvatarForPlayer( 0 )->GetPosition( position );
    GetPCM().RemoveFreeCarIfClose( position );
    rmt::Sphere s;
    s.centre = position;
    s.radius = 10.0f;
    TrafficManager::GetInstance()->ClearTrafficInSphere( s );

    // new 
    // greg
    // jan 4, 2003
    
    // phone booth cars should overwrite the default slot

    rAssert( name != NULL && filename != NULL );

    strncpy( sSelectedVehicleName, name, sizeof( sSelectedVehicleName ) );

    //update the Gameplaymanager about new car getting loaded into the Other slot.
    strcpy(GetGameplayManager()->mVehicleSlots[GameplayManager::eDefaultCar].name, name);
    strcpy(GetGameplayManager()->mVehicleSlots[GameplayManager::eDefaultCar].filename, filename);

    sCallback.mDriver = driver;
    GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D, filename, GMA_LEVEL_MISSION,  filename, NULL, &sCallback );
}

/*
==============================================================================
SummonVehiclePhone::LoadVehicle
==============================================================================
Description:    Comment

Parameters:     ( int selectedVehicleIndex )

Return:         void 

=============================================================================
*/
void SummonVehiclePhone::LoadVehicle( int selectedVehicleIndex )
{
//    sSelectedVehicle = selectedVehicleIndex;

    CarSelectionInfo* pInfo = GetCarSelectInfo( selectedVehicleIndex );
    rAssert( pInfo );
    SummonVehiclePhone::LoadVehicle( pInfo->GetVehicleName(), pInfo->GetFileName(), VehicleCentral::ALLOW_DRIVER );
}

/*
==============================================================================
SummonVehiclePhone::LoadDebugVehicle
==============================================================================
Description:    Comment

Parameters:     ( const char* filename )

Return:         void 

=============================================================================
*/
void SummonVehiclePhone::LoadDebugVehicle( void )
{
    LoadVehicle( NUM_CAR_SELECTION_SLOTS - 1 );
}

//=============================================================================
// SummonVehiclePhone::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( float timeins )
//
// Return:      void 
//
//=============================================================================
void SummonVehiclePhone::OnUpdate( float timeins )
{
    if ( GetGameplayManager()->QueryPhoneBoothsEnabled() )
    {
        mPhoneIcon.ShouldRender( true );
        mPhoneIcon.Update( rmt::FtoL(timeins * 1000.0f) );
    }
    else
    {
        mPhoneIcon.ShouldRender( false );
    }
}

/*
==============================================================================
SummonVehiclePhoneStaticCallback::~SummonVehiclePhoneStaticCallback
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         SummonVehiclePhoneStaticCallback

=============================================================================
*/
SummonVehiclePhoneStaticCallback::~SummonVehiclePhoneStaticCallback( void )
{
    tRefCounted::Release( mpActionEventLocator );
}
/*
==============================================================================
SummonVehiclePhoneStaticCallback::OnProcessRequestsComplete
==============================================================================
Description:    Comment

Parameters:     ( void* pUserData )

Return:         void 

=============================================================================
*/
void SummonVehiclePhoneStaticCallback::OnProcessRequestsComplete( void* pUserData )
{
    ((BillboardWrappedLoader*)GetAllWrappers()->mpLoader(AllWrappers::msBillboard))->OverrideLoader(false);
    // So now the vehicle is loaded.
    //
//    SummonVehiclePhone::CarSelectionInfo* pInfo = SummonVehiclePhone::GetCarSelectInfo( SummonVehiclePhone::GetSelectedVehicleIndex( ) );
//    rAssert( pInfo );
    // You suck, non const mf.

    //check to see if player car is being requested
    bool bplayercar = true;

    if (GetCharacterSheetManager()->GetCarIndex(SummonVehiclePhone::GetSelectedVehicleName() ) == -1 )
    {
        bool bplayercar = false;
    }

    
    Vehicle* pVehicle = GetVehicleCentral()->InitVehicle( const_cast<char*>( SummonVehiclePhone::GetSelectedVehicleName() ), true, NULL, VT_USER, mDriver,bplayercar );
    rAssert( pVehicle );
    if ( pVehicle )
    {
        if ( mpActionEventLocator )
        {
            CarStartLocator* pLocator = p3d::find<CarStartLocator>( mpActionEventLocator->GetObjName() );
            if ( pLocator )
            {
    
                rmt::Vector position;

                pLocator->GetLocation( &position );
                float facing = pLocator->GetRotation();
            
                // Set the vehicle.
                //
                //pVehicle->SetInitialPosition( &position );
                pVehicle->SetInitialPositionGroundOffsetAutoAdjust( &position );                
                pVehicle->SetResetFacingInRadians( facing );

                // This will activate the above settings.
                //
                pVehicle->Reset( false );
                //applying the correct damage to the called vehicle
/*
                //check if car is in charactersheet a -1 from car index indicates cars is not owned by player                
                int carindex = -1;
                carindex = GetCharacterSheetManager()->GetCarIndex(pVehicle->GetName());
                
                //so if the car is owned by the player and not a husk then apply the recorded damage to it.
                if ( (carindex  != -1) &&  (GetCharacterSheetManager()->GetCarHealth(carindex) != 0.0f) )
                {
                    float damage = 1.0f  -  GetCharacterSheetManager()->GetCarHealth(carindex);
                    damage *= pVehicle->mDesignerParams.mHitPoints;
                    pVehicle->TriggerDamage(damage);

                    
                }                  
*/

                //Chuck: Adding support of "current" vehicle
                GetGameplayManager()->SetCurrentVehicle(pVehicle);

                //Also adding support for continuity errors.
                //If this car is the same as a car currently loaded according to the table, then
                //we have a problem.
                if ( GetGameplayManager()->TestForContinuityErrorWithCar( pVehicle, true ) )
                {
                    Vehicle* errorCar = GetGameplayManager()->GetVehicleInSlot( GameplayManager::eAICar );
                    if ( errorCar )
                    {
                        //This is kinda hacky.
                        GetVehicleCentral()->RemoveVehicleFromActiveList( errorCar );
                        errorCar->DrawVehicle( false );
                    }
                }
    
                // new 
                // greg
                // jan 4, 2003
                
                // phone booth cars should overwrite the default slot                
                
                GetGameplayManager()->mVehicleSlots[GameplayManager::eDefaultCar].mp_vehicle = pVehicle;
                GetGameplayManager()->mVehicleSlots[GameplayManager::eDefaultCar].mp_vehicle->AddRef();
            }
            else
            {
                rAssertMsg(0, "what now! - vehicle init'd and added to active list but not addref'd - see greg \n");
            }
            tRefCounted::Release( mpActionEventLocator );
        }
        else
        {
            rAssertMsg(0, "what now! - vehicle init'd and added to active list but not addref'd - see greg \n");
        }

        // tell GUI system to resume in-game
        //
        GetGuiSystem()->HandleMessage( GUI_MSG_RESUME_INGAME );
    }
}

/*
==============================================================================
SummonVehiclePhone::SetAnimation
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void SummonVehiclePhone::SetAnimation( Character* pCharacter, Sequencer* pSeq )
{
    Action* pAction = 0;
    pAction = new PlayAnimationAction( pCharacter, "phone_hold" );
    pSeq->AddActionToSequence( pAction );
}

/*
==============================================================================
Bounce::Bounce
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
Bounce::Bounce( ActionEventLocator* pActionEventLocator )
:
ActionEventHandler( pActionEventLocator )
{ 
}
/*
==============================================================================
Bounce::~Bounce
==============================================================================
Description:    Comment

Parameters:     ()

Return:         Bounce

=============================================================================
*/
Bounce::~Bounce()
{
}
/*
==============================================================================
Bounce::Create
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         bool 

=============================================================================
*/
bool Bounce::Create( tEntityStore* inStore )
{
    EventLocator* pEventLocator = new (GMA_LEVEL_OTHER) EventLocator();
    pEventLocator->SetName( mpActionEventLocator->GetObjName() );
    pEventLocator->AddRef();
    pEventLocator->SetEventType( LocatorEvent::BOUNCEPAD );
    pEventLocator->SetNumTriggers( 1, GMA_LEVEL_OTHER ); 
    rmt::Vector location;
    mpActionEventLocator->GetLocation( &location );
    pEventLocator->SetLocation( location );

    TriggerVolume* pTriggerVolume = mpActionEventLocator->GetTriggerVolume( 0 );
    pEventLocator->AddTriggerVolume( pTriggerVolume );
    pTriggerVolume->SetLocator( pEventLocator );
    pTriggerVolume->SetName( mpActionEventLocator->GetObjName() );
    
    // Nothing can go wrong here.
    //
    return true;
}
/*
==============================================================================
Bounce::OnButtonPressed
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, Sequencer* pSeq )

Return:         void 

=============================================================================
*/
void Bounce::OnEnter( Character* pCharacter )
{
}

/*
==============================================================================
Bounce::OnEnter
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, Locator* pLocator )

Return:         void 

=============================================================================
*/
void Bounce::OnEnter( Character* pCharacter, Locator* pLocator )
{
    if(pCharacter->GetStateManager()->GetState() != CharacterAi::LOCO)
    {
        return;
    }

    // Do the bounce logic.
    //
    rmt::Vector mJumpTarget;
    pLocator->GetLocation( &mJumpTarget );
    rmt::Vector position;
    pCharacter->GetPosition( position );
    mJumpTarget.Sub( position );

    pCharacter->GetActionController()->Clear();
    pCharacter->GetJumpLocomotionAction()->Reset( mJumpTarget, false );
    Sequencer* seq = pCharacter->GetActionController()->GetNextSequencer();
    seq->BeginSequence();
    seq->AddAction(pCharacter->GetJumpLocomotionAction());
    seq->EndSequence();
}

static float sfCollectibleRespawnTime = 10.0f;
/*
==============================================================================
Collectible::Collectible
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
Collectible::Collectible( ActionEventLocator* pActionEventLocator )
:
ActionEventHandler( pActionEventLocator ),
mAnimatedIcon( NULL ),
mbCollected( false )
//mpGameObject( 0 )
{  
}
/*
==============================================================================
Collectible::~Collectible
==============================================================================
Description:    Comment

Parameters:     ()

Return:         Collectible

=============================================================================
*/
Collectible::~Collectible()
{
    //tRefCounted::Release( mpGameObject );
    if ( mAnimatedIcon )
    {
        delete mAnimatedIcon;
    }
}
/*
==============================================================================
Collectible::Create
==============================================================================
Description:    Comment

Parameters:     ( tEntityStore* inStore )

Return:         bool 

=============================================================================
*/
bool Collectible::Create( tEntityStore* inStore )
{
    bool bCreated = false;

    return bCreated;
/*
    // We can use this same tCompositeDrawable pointer, because what is really unique
    // is the tPose, not the tCompositeDrawable.
    //
    tCompositeDrawable* pDrawable = p3d::find<tCompositeDrawable>( mpActionEventLocator->GetObjName() );
    if ( pDrawable )
    {
        tMultiController* pAnimController = p3d::find<tMultiController>( pDrawable->GetUID() );
        if ( pAnimController )
        {
            ActionButton::AnimEntityDSGWrapper* pGameObject = new ActionButton::AnimEntityDSGWrapper;

            rmt::Matrix transform;
            rmt::Vector position;
            mpActionEventLocator->GetLocation( &position );
            transform.Identity( );
            transform.FillTranslate( position );
            pGameObject->SetDrawable( pDrawable );
            pGameObject->SetPose( pDrawable->GetSkeleton()->NewPose() );
            pGameObject->SetAnimController( pAnimController );
            pGameObject->SetTransform( transform );
    
            // Set the start state of all animations requiring a trigger to off.
            //
            // This will stop the animation until Action executes.
            //
            pGameObject->SetAnimationDirection( 0.0f );
            // Reset the animation to the initial frame.
            //
            pGameObject->GetAnimController( )->SetFrame( 0.0f );

            // Play the Collectible animation.
            //
            pGameObject->SetAnimationDirection( 1.0f );
    
            tRefCounted::Assign( mpGameObject, pGameObject );
            // Temporary.
            //
            GetRenderManager()->mpLayer(RenderEnums::LevelSlot)->AddGuts((tDrawable*)(pGameObject) );  
            bCreated = true;
        }
        else
        {
#ifdef RAD_DEBUG
            char error[256];
            sprintf( error, "Shouldn't %s have a properly named multicontroller?\n", mpActionEventLocator->GetObjName());
            rAssert(false);
#endif
        }
    }
    return bCreated;
*/
}

//=============================================================================
// Collectible::OnReset
//=============================================================================
// Description: Comment
//
// Parameters:  ( void )
//
// Return:      void 
//
//=============================================================================
void Collectible::OnReset( void )
{
/*
    // Set the start state of all animations requiring a trigger to off.
    //
    // This will stop the animation until Action executes.
    //
    mpGameObject->SetAnimationDirection( 0.0f );
    // Reset the animation to the initial frame.
    //
    mpGameObject->GetAnimController( )->SetFrame( 0.0f );

    // Play the Collectible animation.
    //
    mpGameObject->SetAnimationDirection( 1.0f );

    mbCollected = false;
    mpGameObject->SetVisible( true );
    mpGameObject->UpdateVisibility( );
*/
    //Kinda slow, but guarantees that the object will only be in once.
    mAnimatedIcon->ShouldRender( false );
    mAnimatedIcon->ShouldRender( true );
}


//Chuck: added this to make wrenches respawn since I didnt see a method to set mbCollected to false;

void Collectible::ResetCollectible()
{
    mbCollected = false;
}


/*




==============================================================================
Collectible::OnUpdate
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Collectible::OnUpdate( float timeins )
{
    if ( IsCollected() )
    {
        if ( ShouldRespawn() )
        {
            if ( IsRespawnTimeExpired( ) )
            {
                mbCollected = false;
                SetRespawnTime( sfCollectibleRespawnTime );

                // Reactivate the trigger volumes.
                //
                mpActionEventLocator->SetFlag( Locator::ACTIVE, true );

                // Allow display.
                //
                //mpGameObject->SetVisible( true );
                mAnimatedIcon->ShouldRender( true );
            }
            else
            {
                UpdateRespawnTime( timeins );     
            }
        }
    }
    else
    {
        //mpGameObject->GetAnimController()->Advance( timeins * 1000.0f * mpGameObject->GetAnimationDirection() );
        mAnimatedIcon->Update( rmt::FtoL(timeins * 1000.0f) );
    }
}
/*
==============================================================================
Collectible::OnEnter
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void Collectible::OnEnter( Character* pCharacter )
{
    if ( !IsCollected() )
    {
        // Make it disappear.
        //
        //mpGameObject->SetVisible( false );
        mAnimatedIcon->ShouldRender( false );

        // Deactivate the trigger volumes.
        //
        mpActionEventLocator->SetFlag( Locator::ACTIVE, false );
        // Maybe play an animation?
        //

        // Maybe play a sound effect.
        //

        // We collected it.
        //
        mbCollected = true;

        GetEventManager()->TriggerEvent( EVENT_COLLECT_OBJECT );
    }
}

void Collectible::OnExit( Character* pCharacter )
{
    // Do nothing.
    //
}

/*
==============================================================================
RespawnCollectible::RespawnCollectible
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
RespawnCollectible::RespawnCollectible( ActionEventLocator* pActionEventLocator )
:
Collectible( pActionEventLocator ),
mfRespawnTime( sfCollectibleRespawnTime )
{  
}
/*
==============================================================================
RespawnCollectible::~RespawnCollectible
==============================================================================
Description:    Comment

Parameters:     ()

Return:         RespawnCollectible

=============================================================================
*/
RespawnCollectible::~RespawnCollectible()
{
}
/*
==============================================================================
CollectibleFood::CollectibleFood
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
float CollectibleFood::sfSmallTurboGain = 0.1f;
float CollectibleFood::sfLargeTurboGain = 0.3f;
CollectibleFood::CollectibleFood( ActionEventLocator* pActionEventLocator, float fTurboGain )
:
RespawnCollectible( pActionEventLocator ),
mfTurboGain( fTurboGain )
{  
}
/*
==============================================================================
CollectibleFood::~CollectibleFood
==============================================================================
Description:    Comment

Parameters:     ()

Return:         CollectibleFood

=============================================================================
*/
CollectibleFood::~CollectibleFood()
{
}
/*
==============================================================================
CollectibleFood::OnEnter
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void CollectibleFood::OnEnter( Character* pCharacter )
{
    if ( !IsCollected() )
    {
        // used to add turbo here, is this class even neccesary any more

        // Maybe play an animation?
        //

        // Maybe play a sound effect.
        //
    }
    Collectible::OnEnter( pCharacter );
}

AnimatedIcon* CollectibleCard::mAnimatedCollectionThing = NULL;
unsigned int CollectibleCard::mCollectibleCardCount = 0;


/*
==============================================================================
CollectibleCard::CollectibleCard
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
CollectibleCard::CollectibleCard( ActionEventLocator* pActionEventLocator )
:
Collectible( pActionEventLocator )
{  
    mCollectibleCardCount++;
}
/*
==============================================================================
CollectibleCard::~CollectibleCard
==============================================================================
Description:    Comment

Parameters:     ()

Return:         CollectibleCard

=============================================================================
*/
CollectibleCard::~CollectibleCard()
{
    mCollectibleCardCount--;

    if ( mCollectibleCardCount == 0 )
    {
        delete mAnimatedCollectionThing;
        mAnimatedCollectionThing = NULL;
    }
}

//=============================================================================
// CollectibleCard::Create
//=============================================================================
// Description: Comment
//
// Parameters:  ( tEntityStore* inStore )
//
// Return:      bool 
//
//=============================================================================
bool CollectibleCard::Create( tEntityStore* inStore )
{
MEMTRACK_PUSH_GROUP( "Collectible - Cards" );
    bool bCreated = false;

    // We can use this same tCompositeDrawable pointer, because what is really unique
    // is the tPose, not the tCompositeDrawable.
    //
    bool found = false;
    tUID nameUID = tEntity::MakeUID(mpActionEventLocator->GetObjName());
    unsigned int i;
    for ( i = 0; i < ActionButtonManager::MAX_ACTIONS; ++i )
    {
        if ( GetActionButtonManager()->GetActionByIndex( i ) )
        {
            ActionButton::ButtonHandler* bh = GetActionButtonManager()->GetActionByIndex( i );

            if ( bh && bh->GetType() == ActionButton::ButtonHandler::COLLECTOR_CARD )
            {
                rAssert( dynamic_cast<ActionButton::CollectibleCard*>(bh) != NULL );
                ActionButton::CollectibleCard* cc = static_cast<ActionButton::CollectibleCard*>(bh);
                rAssert( cc );
                if ( tEntity::MakeUID(cc->GetActionEventLocator()->GetObjName()) == nameUID )
                {
                    found = true;
                    break;
                }
            }
        }
    }

    //Test to make sure the player doesn't already have this one.
    if ( !found &&
         !GetCardGallery()->IsCardCollected( nameUID ) )
    {
        // make sure this card belongs in the current level
        //
        CardsDB* cardsDB = GetCardGallery()->GetCardsDB();
        rAssert( cardsDB != NULL );
        Card* card = cardsDB->GetCardByName( nameUID );
        if( card != NULL &&
            static_cast<int>( card->GetLevel() ) != (GetGameplayManager()->GetCurrentLevelIndex() + 1) )
        {
            rTunePrintf( "*** Invalid card found: %d-%d\n", card->GetLevel(), card->GetLevelID() );
            rTuneAssertMsg( false, "This card does not belong in this level! Please go tell Sheik!" );
        }

        mAnimatedIcon = new(GMA_LEVEL_OTHER) AnimatedIcon();

        rmt::Vector pos;
        mpActionEventLocator->GetLocation( &pos );
        mAnimatedIcon->Init( "card_idle", pos, true );

        if ( mAnimatedCollectionThing == NULL )
        {
            mAnimatedCollectionThing = new(GMA_LEVEL_OTHER) AnimatedIcon();
            mAnimatedCollectionThing->Init( "card_collect", rmt::Vector(0.0f, 0.0f, 0.0f), false, true );
        }

        bCreated = true;
    }
MEMTRACK_POP_GROUP( "Collectible - Cards" );

    return bCreated;
}

/*
==============================================================================
CollectibleCard::OnEnter
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void CollectibleCard::OnEnter( Character* pCharacter )
{
    if ( !IsCollected() )
    {
        // Talk to the collector card DB.
        //
        Card* collectedCard = GetCardGallery()->AddCollectedCardByName( tEntity::MakeUID(mpActionEventLocator->GetObjName()) );
        if( collectedCard != NULL )
        {
            // Update collected cards in character sheet
            //
            GetCharacterSheetManager()->AddCard( static_cast<RenderEnums::LevelEnum>( collectedCard->GetLevel() - 1 ),
                                                 collectedCard->GetLevelID() - 1 );
        }

        // Maybe play an animation?
        //
        mAnimatedCollectionThing->Reset();
        rmt::Vector pos;
        mpActionEventLocator->GetLocation( &pos );
        mAnimatedCollectionThing->ShouldRender( true );
        mAnimatedCollectionThing->Move( pos );

        // Maybe play a sound effect.
        //
        GetEventManager()->TriggerEvent( EVENT_CARD_COLLECTED, reinterpret_cast<void*>( collectedCard ) );

        //Rumble the controller.
        int playerID = mpActionEventLocator->GetPlayerID();
        int controllerID = GetInputManager()->GetControllerIDforPlayer( playerID );
        if ( controllerID != -1 )
        {
            GetInputManager()->GetController( controllerID )->ApplyEffect( RumbleEffect::MEDIUM, 250 );
            GetInputManager()->GetController( controllerID )->ApplyEffect( RumbleEffect::HARD2, 250 );
        }
    }
    Collectible::OnEnter( pCharacter );
}

//=============================================================================
// CollectibleCard::UpdateThing
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void CollectibleCard::UpdateThing( unsigned int milliseconds )
{
    if ( mAnimatedCollectionThing )
    {
        mAnimatedCollectionThing->Update( milliseconds );
    }
}




//Intializing static stuff for WrenchIcon

unsigned int ActionButton::WrenchIcon::mWrenchCount = 0;
AnimatedIcon* ActionButton::WrenchIcon::mAnimatedCollectionThing = NULL;



/*
==============================================================================
WrenchIcon::WrenchIcon
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ToggleAnim

=============================================================================
*/
WrenchIcon::WrenchIcon ( ActionEventLocator* pActionEventLocator ) :
Collectible( pActionEventLocator ),
RespawnEntity(RespawnEntity::eWrench)
{  
   mWrenchCount++;
}
/*
==============================================================================
WrenchIcon::~WrenchIcon
==============================================================================
Description:    Comment

Parameters:     ()

Return:         WrenchIcon

=============================================================================
*/
WrenchIcon::~WrenchIcon()
{
    mWrenchCount--;
   
    if (mWrenchCount ==0)
    {
        delete mAnimatedCollectionThing;
        mAnimatedCollectionThing = NULL;
    }
}

//=============================================================================
// WrenchIcon::Create
//=============================================================================
// Description: Comment
//
// Parameters:  ( tEntityStore* inStore )
//
// Return:      bool 
//
//=============================================================================
bool WrenchIcon::Create( tEntityStore* inStore )
{
MEMTRACK_PUSH_GROUP( "Wrench - Icon" );
    bool bCreated = false;
    // We can use this same tCompositeDrawable pointer, because what is really unique
    // is the tPose, not the tCompositeDrawable.
    //
    bool found = false;
    tUID nameUID = tEntity::MakeUID(mpActionEventLocator->GetObjName());
    unsigned int i;
    for ( i = 0; i < ActionButtonManager::MAX_ACTIONS; ++i )
    {
        if ( GetActionButtonManager()->GetActionByIndex( i ) )
        {
            ActionButton::ButtonHandler* bh = GetActionButtonManager()->GetActionByIndex( i );

            if ( bh && bh->GetType() == ActionButton::ButtonHandler::WRENCH_ICON )
            {
                rAssert( dynamic_cast<ActionButton::WrenchIcon*>(bh) != NULL );
                ActionButton::WrenchIcon* pWrenchIcon = static_cast<ActionButton::WrenchIcon*>(bh);
                rAssert( pWrenchIcon );
                if ( tEntity::MakeUID(pWrenchIcon->GetActionEventLocator()->GetObjName()) == nameUID )
                {
                    found = true;
                    break;
                }
            }
        }
    }

    
        mAnimatedIcon = new(GMA_LEVEL_OTHER) AnimatedIcon();

        rmt::Vector pos;
        mpActionEventLocator->GetLocation( &pos );
        mAnimatedIcon->Init( "wrench", pos, true );
        mAnimatedIcon->ShouldRender(true);

        if ( mAnimatedCollectionThing == NULL )
        {
            mAnimatedCollectionThing = new(GMA_LEVEL_OTHER) AnimatedIcon();
            mAnimatedCollectionThing->Init( "wrench_collect", rmt::Vector(0.0f, 0.0f, 0.0f), false, true );
        }

        bCreated = true;
    
MEMTRACK_POP_GROUP("Wrench - Icon");

    return bCreated;
}

/*
==============================================================================
WrenchIcon::OnEnter
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void WrenchIcon::OnEnter( Character* pCharacter )
{
    if ( !IsCollected() )
    {
       

        // Maybe play an animation?     
        mAnimatedCollectionThing->Reset();
        rmt::Vector pos;
        mpActionEventLocator->GetLocation( &pos );
        mAnimatedCollectionThing->ShouldRender( true );
        mAnimatedCollectionThing->Move( pos );

        // Maybe play a sound effect.
        //

        //check which context we're in. If we are in regular gameplay then trigger Repair Car Event. 
        if ( GetGameFlow()->GetCurrentContext() == CONTEXT_GAMEPLAY)
        {
            GetEventManager()->TriggerEvent(EVENT_REPAIR_CAR);
        }
        //we must be in super sprint mode then just repair the car since we arent going to summon the helper bee thingy
        else
        {
            int playerid = mpActionEventLocator->GetPlayerID();

            GetAvatarManager()->GetAvatarForPlayer(playerid)->GetVehicle()->ResetDamageState();
        }
        //Fire off a event for the sound manager
        GetEventManager()->TriggerEvent(EVENT_COLLECTED_WRENCH);


    }
    Collectible::OnEnter( pCharacter );
    //update the respawnentity
    RespawnEntity::EntityCollected();
}

//=============================================================================
// WrenchIcon::UpdateThing
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void WrenchIcon::UpdateThing( unsigned int milliseconds )
{
    if ( mAnimatedCollectionThing != NULL)
    {
        mAnimatedCollectionThing->Update( milliseconds );
    }    
   
}


void WrenchIcon::Update(float timeins)
{
    unsigned int timeinms=0;
    timeinms = rmt::FtoL(timeins * 1000.0f);
    mAnimatedIcon->Update(timeinms);
    RespawnEntity::Update(timeinms);
    //check if entity should respawn.
    if (RespawnEntity::ShouldEntityRespawn ())
    {
        //reset the collectible class created by trav and cary .
        Collectible::Reset();
        Collectible::ResetCollectible();
        Collectible::mpActionEventLocator->SetFlag(Locator::ACTIVE,true);
        Collectible::mAnimatedIcon->ShouldRender(true);

    }
        
}




//Intializing static stuff for NitroIcon
unsigned int ActionButton::NitroIcon::mNitroCount = 0;
AnimatedIcon* ActionButton::NitroIcon::mAnimatedCollectionThing = NULL;



/*
==============================================================================
NitroIcon::NitroIcon
==============================================================================
Description:    Create the Nitro 

Parameters:    

Return:         

=============================================================================
*/
NitroIcon::NitroIcon ( ActionEventLocator* pActionEventLocator ) :
Collectible( pActionEventLocator ),
RespawnEntity(RespawnEntity::eNitro)
{  
   mNitroCount++;
}
/*
==============================================================================
NitroIcon::~NitroIcon
==============================================================================
Description:   Destructor

Parameters:     ()

Return:        

=============================================================================
*/
NitroIcon::~NitroIcon()
{
    mNitroCount--;
   
    if (mNitroCount ==0) //if no more instances left deleted the static animated class
    {
        delete mAnimatedCollectionThing;
        mAnimatedCollectionThing = NULL;
    }
}

//=============================================================================
// NitroIcon::Create
//=============================================================================
// Description: Comment
//
// Parameters:  ( tEntityStore* inStore )
//
// Return:      bool 
//
//=============================================================================
bool NitroIcon::Create( tEntityStore* inStore )
{
MEMTRACK_PUSH_GROUP( "Nitro - Icon" );
    bool bCreated = false;
    // We can use this same tCompositeDrawable pointer, because what is really unique
    // is the tPose, not the tCompositeDrawable.
    //
    bool found = false;
    tUID nameUID = tEntity::MakeUID(mpActionEventLocator->GetObjName());
    unsigned int i;
    for ( i = 0; i < ActionButtonManager::MAX_ACTIONS; ++i )
    {
        if ( GetActionButtonManager()->GetActionByIndex( i ) )
        {
            ActionButton::ButtonHandler* bh = GetActionButtonManager()->GetActionByIndex( i );

            if ( bh && bh->GetType() == ActionButton::ButtonHandler::NITRO_ICON )
            {
                rAssert( dynamic_cast<ActionButton::NitroIcon*>(bh) != NULL );
                ActionButton::NitroIcon* pNitroIcon = static_cast<ActionButton::NitroIcon*>(bh);
                rAssert( pNitroIcon );
                if ( tEntity::MakeUID(pNitroIcon->GetActionEventLocator()->GetObjName()) == nameUID )
                {
                    found = true;
                    break;
                }
            }
        }
    }

    
        mAnimatedIcon = new(GMA_LEVEL_OTHER) AnimatedIcon();

        rmt::Vector pos;
        mpActionEventLocator->GetLocation( &pos );
        mAnimatedIcon->Init( "nitro", pos, true ); //Hack!

        if ( mAnimatedCollectionThing == NULL )
        {
            mAnimatedCollectionThing = new(GMA_LEVEL_OTHER) AnimatedIcon();
            mAnimatedCollectionThing->Init( "wrench_collect", rmt::Vector(0.0f, 0.0f, 0.0f), false, true );
        }

        bCreated = true;
    
MEMTRACK_POP_GROUP("Nitro - Icon");

    return bCreated;
}

/*
==============================================================================
NitroIcon::OnEnter
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void NitroIcon::OnEnter( Character* pCharacter )
{
    if ( !IsCollected() )
    {
        // Maybe play an animation?     
        mAnimatedCollectionThing->Reset();
        rmt::Vector pos;
        mpActionEventLocator->GetLocation( &pos );
        mAnimatedCollectionThing->ShouldRender( true );
        mAnimatedCollectionThing->Move( pos );

        int playerid = mpActionEventLocator->GetPlayerID();
        Avatar* player = GetAvatarManager()->GetAvatarForPlayer( playerid );

        /*
        // TODO:
        // Maybe play a sound effect? Pass in the avatar pointer? Ask Esan
        ::GetEventManager()->TriggerEvent( EVENT_COLLECTED_NITRO, player );
        */
        Vehicle* playerVehicle = player->GetVehicle();
        if( playerVehicle )
        {
            // increase the nitro count...
            playerVehicle->mNumTurbos++;
        }
    }
    Collectible::OnEnter( pCharacter );
    //update the respawnentity
    RespawnEntity::EntityCollected();
}

//=============================================================================
// NitroIcon::UpdateThing
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void NitroIcon::UpdateThing( unsigned int milliseconds )
{
    if ( mAnimatedCollectionThing != NULL)
    {
        mAnimatedCollectionThing->Update( milliseconds );
    }    
   
}


void NitroIcon::Update(float timeins)
{
    unsigned int timeinms=0;
    timeinms = rmt::FtoL(timeins * 1000.0f);
    mAnimatedIcon->Update(timeinms);
    RespawnEntity::Update(timeinms);
    //check if entity should respawn.
    if (RespawnEntity::ShouldEntityRespawn ())
    {
        //reset the collectible class created by trav and cary .
        Collectible::Reset();
        Collectible::ResetCollectible();
        Collectible::mpActionEventLocator->SetFlag(Locator::ACTIVE,true);
        Collectible::mAnimatedIcon->ShouldRender(true);

    }
        
}

//=============================================================================
// GenericEventButtonHandler::GenericEventButtonHandler
//=============================================================================
// Description: Constructor
//
// Parameters:  ( EventLocator* pEventLocator, EventEnum event  )
//
// Return:      
//
//=============================================================================
GenericEventButtonHandler::GenericEventButtonHandler( EventLocator* pEventLocator, EventEnum event ) :
    mpEventLocator( NULL ),
    mEvent( event ),
    mEventData( NULL )
{
    SetEventLocator( pEventLocator );
}

//=============================================================================
// ::~GenericEventButtonHandler
//=============================================================================
// Description: Destructor
//
// Parameters:  ()
//
// Return:      
//
//=============================================================================
GenericEventButtonHandler::~GenericEventButtonHandler()
{
    if ( mpEventLocator )
    {
        mpEventLocator->Release();
        mpEventLocator = NULL;
    }
};

//=============================================================================
// GenericEventButtonHandler::NewAction
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventLocator* pEventLocator, EventEnum event, GameMemoryAllocator alloc  )
//
// Return:      ButtonHandler
//
//=============================================================================
ButtonHandler* GenericEventButtonHandler::NewAction( EventLocator* pEventLocator, EventEnum event, GameMemoryAllocator alloc  )
{
    return new (alloc) GenericEventButtonHandler( pEventLocator, event );  
}  

//=============================================================================
// GenericEventButtonHandler::SetEventLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventLocator* pEventLocator  )
//
// Return:      void 
//
//=============================================================================
void GenericEventButtonHandler::SetEventLocator( EventLocator* pEventLocator  )
{
    tRefCounted::Assign( mpEventLocator, pEventLocator );
}

//=============================================================================
// GenericEventButtonHandler::OnButtonPressed
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* pCharacter, Sequencer* pSeq )
//
// Return:      bool 
//
//=============================================================================
bool GenericEventButtonHandler::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{
    CGuiScreenHud* currentHUD = GetCurrentHud();
    if( (currentHUD != NULL && !currentHUD->IsActive()) || GetHitnRunManager()->BustingPlayer())
    {
        // if HUD is not active, ignore button press
        //
        return false;
    }

    if ( pCharacter->GetController()->GetIntention() == CharacterController::DoAction )
    {
        GetEventManager()->TriggerEvent( mEvent, mEventData );
        return true;
    }
    else
    {
        return false;
    }
}

//=============================================================================
// TeleportAction::TeleportAction
//=============================================================================
// Description: Comment
//
// Parameters:  ( ActionEventLocator* pActionEventLocator )
//
// Return:      TeleportAction
//
//=============================================================================
TeleportAction::TeleportAction( ActionEventLocator* pActionEventLocator ) :
    ActionEventHandler( pActionEventLocator )
{
}

//=============================================================================
// ::~TeleportAction 
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      TeleportAction
//
//=============================================================================
TeleportAction::~TeleportAction()
{
    GetEventManager()->RemoveListener( this, EVENT_GUI_IRIS_WIPE_CLOSED );
}

//=============================================================================
// TeleportAction::Create
//=============================================================================
// Description: Comment
//
// Parameters:  ( tEntityStore* inStore = 0 )
//
// Return:      bool 
//
//=============================================================================
bool TeleportAction::Create( tEntityStore* inStore )
{
    return true;
}

//=============================================================================
// TeleportAction::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void TeleportAction::HandleEvent( EventEnum id, void* pEventData )
{
    if ( id == EVENT_GUI_IRIS_WIPE_CLOSED )
    {
        GetEventManager()->RemoveListener( this, EVENT_GUI_IRIS_WIPE_CLOSED );

        //Send the teleporting event
        GetEventManager()->TriggerEvent( EVENT_TAKING_TELEPORT, mpActionEventLocator );

        //Teleport the player to the locator
        rmt::Vector pos;
        mpActionEventLocator->GetLocation( &pos );

        rmt::Matrix mat = mpActionEventLocator->GetMatrix();
        mat.IdentityTranslation();

        rmt::Vector z( 1.0f, 0.0f, 0.0f );
        z.Transform( mat );
        float facing, magWaste;

        rmt::CartesianToPolar( z.x, z.z, &magWaste, &facing );        

        GetAvatarManager()->GetAvatarForPlayer( 0 )->GetCharacter()->RelocateAndReset( pos, facing, true );
#ifdef RAD_WIN32
        GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( SuperCam::ON_FOOT_CAM, SuperCamCentral::CUT, 0 );
#else
        GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( SuperCam::WALKER_CAM, SuperCamCentral::CUT, 0 );
#endif
        GetSuperCamManager()->GetSCC( 0 )->DoCameraCut();
        
        //Wipe out when we are finished moving.
        GetGuiSystem()->HandleMessage( GUI_MSG_START_IRIS_WIPE_OPEN );

        GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Resume();
    }
}

//=============================================================================
// TeleportAction::OnButtonPressed
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* pCharacter, Sequencer* pSeq )
//
// Return:      bool 
//
//=============================================================================
bool TeleportAction::OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
{
    //Do an iris wipe in
    GetGuiSystem()->HandleMessage( GUI_MSG_START_IRIS_WIPE_CLOSE );
    
    //add listener  
    GetEventManager()->AddListener( this, EVENT_GUI_IRIS_WIPE_CLOSED );

    //Pause gameplay
    GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Suspend();
    return true;
}

//=============================================================================
// TeleportAction::OnEnter
//=============================================================================
// Description: Comment
// 
// Parameters:  ( Character* pCharacter )
//
// Return:      void 
//
//=============================================================================
void TeleportAction::OnEnter( Character* pCharacter )
{
    GetEventManager()->TriggerEvent( EVENT_ENTERED_TELEPORT_PAD, mpActionEventLocator );
}

//=============================================================================
// TeleportAction::OnExit
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* pCharacter )
//
// Return:      void 
//
//=============================================================================
void TeleportAction::OnExit( Character* pCharacter )
{
    GetEventManager()->TriggerEvent( EVENT_EXITED_TELEPORT_PAD, mpActionEventLocator );
}

//===============================================================================

bool PurchaseReward::sbEnabled = true;

//=============================================================================
// PurchaseReward::PurchaseReward
//=============================================================================
// Description: Comment
//
// Parameters:  ( ActionEventLocator* pActionEventLocator )
//
// Return:      PurchaseReward
//
//=============================================================================
PurchaseReward::PurchaseReward( ActionEventLocator* pActionEventLocator ) :
    ActionEventHandler( pActionEventLocator ),
    mAllPurchased( false ),
    mIsActive( true ),
    mHudMapIconID( -1 )
{
}

//=============================================================================
// ::~PurchaseReward
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      PurchaseReward
//
//=============================================================================
PurchaseReward::~PurchaseReward()
{
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

//=============================================================================
// PurchaseReward::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( float timeins )
//
// Return:      void
//
//=============================================================================
void PurchaseReward::OnUpdate( float timeins )
{
    if ( sbEnabled && mIsActive && !mAllPurchased && GetGameplayManager()->IsSundayDrive() )
    {
        mIcon.Update( rmt::FtoL(timeins * 1000.0f) );
    }

    if ( mIsActive && ( mAllPurchased || !sbEnabled || !GetGameplayManager()->IsSundayDrive() ) )
    {
        //Time to shut this one down.
        mIcon.ShouldRender( false );
        mIsActive = false;

        mpActionEventLocator->SetFlag( Locator::ACTIVE, false );

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
    else if ( !mIsActive && !mAllPurchased && sbEnabled && GetGameplayManager()->IsSundayDrive() )
    {
        mIcon.ShouldRender( true );
        mIsActive = true;

        mpActionEventLocator->SetFlag( Locator::ACTIVE, true );

        rAssert( mHudMapIconID == -1 );
        if( mHudMapIconID == -1 )
        {
            CGuiScreenHud* currentHud = GetCurrentHud();
            if( currentHud != NULL )
            {
                mHudMapIconID = currentHud->GetHudMap( 0 )->RegisterIcon( HudMapIcon::ICON_PURCHASE_CENTRE,
                                                                          rmt::Vector( 0.0f, 0.0f, 0.0f ),
                                                                          mpActionEventLocator );
                rAssert( mHudMapIconID != -1 );
            }
        }
    }
}

//=============================================================================
// PurchaseReward::CanEnable
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool PurchaseReward::CanEnable() const
{
    return (!mAllPurchased && sbEnabled && GetGameplayManager()->IsSundayDrive() );
}


//=============================================================================
// PurchaseCar::PurchaseCar
//=============================================================================
// Description: Comment
//
// Parameters:  ( ActionEventLocator* pActionEventLocator )
//
// Return:      PurchaseCar
//
//=============================================================================
PurchaseCar::PurchaseCar( ActionEventLocator* pActionEventLocator ) :
    PurchaseReward( pActionEventLocator )
{
    rmt::Vector pos;
    pActionEventLocator->GetLocation( &pos );
    mIcon.Init( "dollar", pos );  //TODO: get the real icon in.

    //
    // Set up transitions
    //
    m_DisableInput.SetState( Input::ACTIVE_FRONTEND );
    m_GotoLetterbox.SetScreen( CGuiWindow::GUI_SCREEN_ID_LETTER_BOX );
    m_GotoLetterbox.SetWindowOptions( CLEAR_WINDOW_HISTORY );
    m_ContextSwitch.SetContext( CONTEXT_PAUSE );
    m_WaitForConversationDone.SetEvent( EVENT_CONVERSATION_DONE );
    m_GotoHud.SetScreen( CGuiWindow::GUI_SCREEN_ID_HUD );
    m_GotoHud.SetWindowOptions( FORCE_WINDOW_CHANGE_IMMEDIATE );
    m_GotoScreen.SetScreen( CGuiWindow::GUI_SCREEN_ID_PURCHASE_REWARDS );
    m_GotoScreen.SetWindowOptions( CLEAR_WINDOW_HISTORY );

    m_GotoLetterbox.SetNextTransition( m_DisableInput );
    m_DisableInput.SetNextTransition( m_WaitForConversationDone );
    m_WaitForConversationDone.SetNextTransition( m_ContextSwitch );
//    m_GotoHud.SetNextTransition( m_ContextSwitch ); // TC: no need to go to the HUD
    m_ContextSwitch.SetNextTransition( m_GotoScreen );
    m_GotoScreen.SetNextTransition( NULL );
}

//=============================================================================
// ::~PurchaseCar
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      PurchaseCar
//
//=============================================================================
PurchaseCar::~PurchaseCar()
{
}

//=============================================================================
// PurchaseCar::Create
//=============================================================================
// Description: Comment
//
// Parameters:  ( tEntityStore* inStore )
//
// Return:      bool 
//
//=============================================================================
bool PurchaseCar::Create( tEntityStore* inStore )
{
    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud != NULL )
    {
        mHudMapIconID = currentHud->GetHudMap( 0 )->RegisterIcon( HudMapIcon::ICON_PURCHASE_CENTRE,
                                                                  rmt::Vector( 0.0f, 0.0f, 0.0f ),
                                                                  mpActionEventLocator );
    }

    //Should findout and fill in all the Car selection data here.
    //Tony?
    return true;
}

//=============================================================================
// PurchaseCar::OnButtonPressed
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* pCharacter )
//
// Return:      bool 
//
//=============================================================================
bool PurchaseCar::OnButtonPressed( Character* pCharacter )
{
    if( GetGuiSystem()->GetInGameManager()->IsEnteringPauseMenu() )
    {
        // if about to enter pause menu, ignore button press
        //
        return false;
    }

    int level;
    GameplayManager* gameplayMgr = NULL;

    //Purchase and load the car.
    //Tony?
    // switch to pause context
    //

    if ( mpActionEventLocator->GetUID() == GIL )
    {
        m_ContextSwitch.Activate();
        m_GotoScreen.SetParam1( Merchandise::SELLER_GIL );
        m_GotoScreen.Activate();

        //
        // Make Gil say something
        //
        GetEventManager()->TriggerEvent( EVENT_HAGGLING_WITH_GIL, GetCharacterManager()->GetCharacterByName( "reward_gil" ) );
    }
    else
    {
        m_GotoScreen.SetParam1( Merchandise::SELLER_SIMPSON );
        //
        // Start a conversation
        //
        gameplayMgr = GetGameplayManager();
        rAssert( gameplayMgr != NULL );

        level = gameplayMgr->GetCurrentLevelIndex() - RenderEnums::L1;
        rAssert( ( level >= 0 ) && ( level <= 7 ) );

        if( s_carPurchaseConvNames[level].convName != 0 )
        {
            DialogEventData data;

            data.dialogName = s_carPurchaseConvNames[level].convName;

            rAssert( pCharacter != NULL );

            data.charUID1 = s_carPurchaseConvNames[level].sellerName;
            data.charUID2 = pCharacter->GetUID();

            GetEventManager()->TriggerEvent( EVENT_CONVERSATION_INIT_DIALOG, &data );
            GetEventManager()->TriggerEvent( EVENT_CONVERSATION_START );

            //
            // make the two characters face one another
            //
            Character* c0 = GetCharacterManager()->GetCharacterByName( data.charUID1 );
            Character* c1 = GetCharacterManager()->GetCharacterByName( data.charUID2 );
            rAssertMsg( c0 != NULL, "Character's name is incorrect" );
            rAssertMsg( c1 != NULL, "Character's name is incorrect" );
            GetPresentationManager()->MakeCharactersFaceEachOther( c0, c1 );

            CGuiScreenLetterBox::SuppressAcceptCancelButtons();
            CGuiScreenLetterBox::ForceOpen();
            m_GotoLetterbox.Activate();
        }
        else
        {
            m_ContextSwitch.Activate();
            m_GotoScreen.Activate();
        }
    }

    //Trickyness to get the carstart locator.
    // We do some of the work here, then we will trigger an event to do the menu.
    // The menu will trigger the loading.
    //
    tRefCounted::Assign( SummonVehiclePhone::sCallback.mpActionEventLocator, mpActionEventLocator );


    return true;
}

//=============================================================================
// PurchaseCar::OnEnter
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* pCharacter )
//
// Return:      void 
//
//=============================================================================
void PurchaseCar::OnEnter( Character* pCharacter )
{
}

//=============================================================================
// PurchaseCar::OnExit
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* pCharacter )
//
// Return:      void 
//
//=============================================================================
void PurchaseCar::OnExit( Character* pCharacter )
{
}

//=============================================================================
// PurchaseCar::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( float timeins )
//
// Return:      void 
//
//=============================================================================
void PurchaseCar::OnUpdate( float timeins )
{
    RenderEnums::LevelEnum currLev = GetGameplayManager()->GetCurrentLevelIndex();
    RewardsManager* rm = GetRewardsManager();

    Merchandise::eSellerType st = Merchandise::SELLER_GIL;
    if ( mpActionEventLocator->GetUID() != GIL )
    {
        st = Merchandise::SELLER_SIMPSON;
    }

    //Test to see if the cars are all sold.

    Merchandise* m = rm->FindFirstMerchandise( currLev, st );

    mAllPurchased = true;
    while ( m != NULL )
    {
        if ( !m->RewardStatus() )
        {
            //If one isn't purchased, we're not all purchased. 
            mAllPurchased = false;
            break;
        }

        m = rm->FindNextMerchandise( currLev, st );
    }

    if ( mIsActive )
    {
        //Follow the character around.
        const char* charName = mpActionEventLocator->GetJointName();
        Character* character = GetCharacterManager()->GetCharacterByName( tEntity::MakeUID( charName ) );
        rAssert( character );

        rmt::Vector pos;
        character->GetPosition( &pos );
        mIcon.Move( pos );

        mpActionEventLocator->SetLocation( pos );
        mpActionEventLocator->GetTriggerVolume( 0 )->SetPosition( pos );
    }

    PurchaseReward::OnUpdate( timeins );
}

//=============================================================================
// PurchaseSkin::PurchaseSkin
//=============================================================================
// Description: Comment
//
// Parameters:  ( ActionEventLocator* pActionEventLocator )
//
// Return:      PurchaseSkin
//
//=============================================================================
PurchaseSkin::PurchaseSkin( ActionEventLocator* pActionEventLocator ) :
    PurchaseReward( pActionEventLocator )
{
    rmt::Vector pos;
    pActionEventLocator->GetLocation( &pos );
    mIcon.Init( "shirtdollar", pos );  //TODO: get the real icon in.
}

//=============================================================================
// ::~PurchaseSkin
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      PurchaseSkin
//
//=============================================================================
PurchaseSkin::~PurchaseSkin()
{
}

//=============================================================================
// PurchaseSkin::Create
//=============================================================================
// Description: Comment
//
// Parameters:  ( tEntityStore* inStore )
//
// Return:      bool 
//
//=============================================================================
bool PurchaseSkin::Create( tEntityStore* inStore )
{
    rmt::Vector pos;
    rAssert( mpActionEventLocator != NULL );
    mpActionEventLocator->GetPosition( &pos );

    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud != NULL )
    {
        mHudMapIconID = currentHud->GetHudMap( 0 )->RegisterIcon( HudMapIcon::ICON_PURCHASE_CENTRE,
                                                                  pos );
    }

    GetActionButtonManager()->LoadingIntoInterior();  //HACK

    return true;
}

//=============================================================================
// PurchaseSkin::OnButtonPressed
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* pCharacter )
//
// Return:      bool 
//
//=============================================================================
bool PurchaseSkin::OnButtonPressed( Character* pCharacter )
{
    if( GetGuiSystem()->GetInGameManager()->IsEnteringPauseMenu() )
    {
        // if about to enter pause menu, ignore button press
        //
        return false;
    }

    // switch to pause context
    //
    GetGameFlow()->SetContext( CONTEXT_PAUSE );

    GetGuiSystem()->GotoScreen( CGuiWindow::GUI_SCREEN_ID_PURCHASE_REWARDS, Merchandise::SELLER_INTERIOR );

    return true;
}

//=============================================================================
// PurchaseSkin::OnEnter
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* pCharacter )
//
// Return:      void 
//
//=============================================================================
void PurchaseSkin::OnEnter( Character* pCharacter )
{
}

//=============================================================================
// PurchaseSkin::OnExit
//=============================================================================
// Description: Comment
//
// Parameters:  ( Character* pCharacter )
//
// Return:      void 
//
//=============================================================================
void PurchaseSkin::OnExit( Character* pCharacter )
{
}

//=============================================================================
// PurchaseSkin::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( float timeins )
//
// Return:      void 
//
//=============================================================================
void PurchaseSkin::OnUpdate( float timeins )
{
    RenderEnums::LevelEnum currLev = GetGameplayManager()->GetCurrentLevelIndex();
    RewardsManager* rm = GetRewardsManager();

    //Test to see if the skins are all sold.
/*
    Merchandise* m = rm->FindFirstMerchandise( currLev, Merchandise::SELLER_INTERIOR );

/*    mAllPurchased = true;

    while ( m != NULL )
    {
        if ( !m->RewardStatus() )
        {
            //If one isn't purchased, we're not all purchased. 
            mAllPurchased = false;
            break;
        }

        m = rm->FindNextMerchandise( currLev, Merchandise::SELLER_INTERIOR );
    }
*/
    PurchaseReward::OnUpdate( timeins );
}

}; // namespace ActionButton.
