//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        actionbuttonhandler.h
//
// Description: Blahblahblah
//
// History:     08/07/2002 + Created -- NAME
//
//=============================================================================

#ifndef ACTIONBUTTONHANDLER_H
#define ACTIONBUTTONHANDLER_H

//========================================
// Nested Includes
//========================================
//#include <p3d/refcounted.hpp>
// Just a tDrawable for now, when AnimEntity is created, we can make it a tRefCount.
//
#include <p3d/drawable.hpp>
#include <p3d/anim/pose.hpp>
#include <worldsim/character/charactercontroller.h> 
#include <worldsim/vehiclecentral.h> 
#include <memory/srrmemory.h>


#include <loading/loadingmanager.h>
#include <string.h>

#include <mission/animatedIcon.h>
#include <mission/respawnmanager/respawnentity.h>
#include <presentation/gui/utility/transitions.h>

#include <events/eventlistener.h>
#include <events/eventenum.h>
#include <stateprop/stateprop.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================
class Character;
class Locator;
class EventLocator;
class ActionEventLocator;
class AnimCollisionEntityDSG;
class Sequencer;
class tMultiController;
class tPoseAnimationController;
class tAnimation;
class tCompositeDrawable;
class InteriorEntranceLocator;
class tEntityStore;
class InstDynaPhysDSG;
class AnimatedIcon;
class InstAnimDynaPhysDSG;
class RespawnEntity;
class StatePropDSG;

namespace ActionButton
{

#define INTERFACE_GameObjectWrapper( terminal ) \
public: \
    virtual void UpdateVisibility( void ) ## terminal   \
    virtual float& GetAnimationDirection( void ) ## terminal    \
    virtual void SetAnimationDirection( float fDirection ) ## terminal    \
    virtual tMultiController* GetAnimController( void ) const ## terminal    \
    virtual tCompositeDrawable* GetDrawable( void ) const ## terminal \
    \
    virtual void Display( void ) ## terminal \
    \
    virtual bool IsManualUpdate( void ) const ## terminal

#define BASE_GameObjectWrapper INTERFACE_GameObjectWrapper(= 0;)
#define IMPLEMENTS_GameObjectWrapper INTERFACE_GameObjectWrapper(;)
    
struct IGameObjectWrapper
:
// Just a tDrawable for now, when AnimEntity is created, we can make it a tRefCount.
//
public tDrawable
{
    BASE_GameObjectWrapper;
};

class AnimCollisionEntityDSGWrapper
:
public IGameObjectWrapper
{
    IMPLEMENTS_GameObjectWrapper;
    AnimCollisionEntityDSGWrapper( void );
    virtual ~AnimCollisionEntityDSGWrapper( void );
    void SetGameObject( AnimCollisionEntityDSG* pGameObject );
protected:
    AnimCollisionEntityDSG* mpGameObject;
};
/*
==============================================================================
AnimCollisionEntityDSGWrapper::IsManualUpdate
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         bool 

=============================================================================
*/
inline bool AnimCollisionEntityDSGWrapper::IsManualUpdate( void ) const
{
    return false;
}

class AnimEntityDSGWrapper
:
public IGameObjectWrapper
{
    IMPLEMENTS_GameObjectWrapper;
    AnimEntityDSGWrapper( void );
    virtual ~AnimEntityDSGWrapper( void );

    void SetDrawable( tCompositeDrawable* pDrawable );
    void SetPose( tPose* pPose );
    void SetAnimController( tMultiController* pAnimController );
    void SetTransform( rmt::Matrix& transform );
    void SetVisible( bool bVisible )
    {
        mbVisible = bVisible;
    }
    bool IsVisible( void ) const
    {
        return mbVisible;
    }
protected:
    tCompositeDrawable* mpDrawable;
    tPose* mpPose;
    tMultiController* mpAnimController;
    float mfDirection;
    rmt::Matrix mTransform;
    bool mbVisible  : 1;
};

/*
==============================================================================
AnimEntityDSGWrapper::IsManualUpdate
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         bool 

=============================================================================
*/
inline bool AnimEntityDSGWrapper::IsManualUpdate( void ) const
{
    return true;
}
//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/24/2002] 
//////////////////////////////////////////////////////////////////////////
class ButtonHandler
:
public tRefCounted
{
public:
    enum Type { MISSION_OBJECTIVE, GET_IN_USER_CAR, SUMMON_PHONE, INTERIOR, GAG, PURCHASE_CAR, PURCHASE_SKIN, GET_IN_CAR, COLLECTOR_CARD, ALIEN_CAMERA, WRENCH_ICON, NITRO_ICON, TELEPORT, OTHER };
   
    ButtonHandler( void );
    virtual ~ButtonHandler();

    virtual bool ButtonPressed( Character* pCharacter );

    virtual void Update( float timeins )
    {
        OnUpdate( timeins );
    }
    virtual void Enter( Character* pCharacter );
    virtual void Exit( Character* pCharacter );

    virtual bool UsesActionButton( void ) const
    {
        return true;
    }
    
    virtual bool NeedsUpdate( void ) const
    {
        return false;
    }
    virtual void Reset( void )
    {
        OnReset( );
    }


    virtual Type GetType( void ) { return OTHER; };

    virtual bool IsInstanceEnabled()const { return true; }

protected:
    virtual void OnReset( void )
    {
    }
    virtual void OnUpdate( float timeins )
    {
    }
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
    {
        return false;
    }
    virtual void OnEnter( Character* pCharacter )
    {
    }
    virtual void OnExit( Character* pCharacter )
    {
    }
    virtual bool IsActionButtonPressed( Character* pCharacter );   
    void SetActionButton( CharacterController::eIntention theActionButton )
    {
        mActionButton = theActionButton;
    } 
    CharacterController::eIntention mActionButton;
private:
    ButtonHandler( const ButtonHandler& actionbuttonhandler );
    ButtonHandler& operator=( const ButtonHandler& actionbuttonhandler );
};

class PropHandler
:
public ButtonHandler
{
public:
	PropHandler( void );
	~PropHandler( void );

	void SetProp( InstDynaPhysDSG* pProp );
	InstDynaPhysDSG* GetProp( void ) const;
protected:
private:
	InstDynaPhysDSG* mpProp;
};

class AttachProp
:
public PropHandler
{
protected:
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );
};
//////////////////////////////////////////////////////////////////////////
//
//
// TBJ [8/12/2002] 
//
//////////////////////////////////////////////////////////////////////////
class EnterInterior
:
public ButtonHandler
{
public:
    EnterInterior( InteriorEntranceLocator* pEventLocator );
    virtual ~EnterInterior();
    void SetLocator ( InteriorEntranceLocator* pLocator );

    virtual Type GetType( void ) { return INTERIOR; };


protected:
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );
    InteriorEntranceLocator* mpLocator;
};
//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/24/2002] 
//////////////////////////////////////////////////////////////////////////
class GetInCar
:
public ButtonHandler
{  
public:
    GetInCar( EventLocator* pEventLocator );
    virtual ~GetInCar();

    static ButtonHandler* NewAction( EventLocator* pEventLocator )
    {
        return new (GMA_LEVEL_OTHER) GetInCar( pEventLocator );  
    }
    

    void SetVehicleId( int vehicleId )
    {
        mVehicleId = vehicleId;
    }
    int GetVehicleId( void ) const
    {
        return mVehicleId;
    }
    void SetEventLocator( EventLocator* pEventLocator );
    EventLocator* GetEventLocator( void ) const
    {
        return mpEventLocator;
    }

    virtual Type GetType() { return GET_IN_CAR; };

protected:
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );
    EventLocator* mpEventLocator;
private:
    int mVehicleId;
    Character* mCharacter;
};

//////////////////////////////////////////////////////////////////////////
//
// TBJ [8/27/2002] 
//
//////////////////////////////////////////////////////////////////////////
#ifndef RAD_RELEASE
    #define ACTIONEVENTHANDLER_DEBUG
#endif
class ActionEventHandler
:
public ButtonHandler
{
public:
    ActionEventHandler( ActionEventLocator* pActionEventLocator );
    virtual ~ActionEventHandler();
    virtual bool Create( tEntityStore* inStore = 0 )
    {
        return true;
    }

    virtual Type GetType( void ) { return OTHER; };

#ifdef ACTIONEVENTHANDLER_DEBUG
    virtual void Enter( Character* pCharacter );
#endif //ACTIONEVENTHANDLER_DEBUG
    void SetActionEventLocator( ActionEventLocator* pActionEventLocator );
    ActionEventLocator* GetActionEventLocator( void ) const
    {
        return mpActionEventLocator;
    }
    void SetInstanceEnabled( bool enabled ) { mIsEnabled = enabled; }

protected:
    ActionEventLocator* mpActionEventLocator;
    bool mIsEnabled : 1;
};
//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/24/2002] 
//////////////////////////////////////////////////////////////////////////
class AnimSwitch
:
public ActionEventHandler
{
public:
    AnimSwitch( ActionEventLocator* pActionEventLocator );
    virtual ~AnimSwitch();

    void Init( IGameObjectWrapper* pAnimCollisionEntityDSG, tPose::Joint* pJoint, bool bAttachToJoint );
    virtual bool ButtonPressed( Character* pCharacter );
    virtual bool Create( tEntityStore* inStore = 0 );
    float& GetAnimationDirection( void );
    void SetAnimationDirection( float fDirection );

    tMultiController* GetAnimController( void ) const;

    void Update( float timeins );

    
    void Destroy( void );
    // Informs the animswitch that the game object (DSG) has been destroyed
    // via a collision or other disaster
    virtual void GameObjectDestroyed(){}


    virtual bool UsesActionButton( void ) const
    {
        return mActionButton == CharacterController::DoAction;
    }
    virtual bool NeedsUpdate( void ) const
    {
        if ( mpGameObject )
        {
            return mpGameObject->IsManualUpdate( );
        }
        
        return false;
    }

    virtual Type GetType( void ) { return GAG; };

protected:
    virtual void OnReset( void );
    virtual void OnUpdate( float timeins ) {}
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq ) = 0;
    virtual void PositionCharacter( Character* pCharacter, Sequencer* pSeq );
    virtual void SequenceActions( Character* pCharacter, Sequencer* pSeq );
    virtual void SetAnimation( Character* pCharacter, Sequencer* pSeq );
    virtual void OnInit( void ) {}
    float mfDirection;
    tPose::Joint* mpJoint;
    IGameObjectWrapper* mpGameObject;
    rmt::Vector mStandPosition;
    const char* mSoundName;
    const char* mSettingsName;
    bool mIsMovingSound : 1;
    bool mbAttachToJoint: 1;
private:

};
//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/24/2002] 
//////////////////////////////////////////////////////////////////////////
// Toggle the animation on/off.
//
class ToggleAnim
:
public AnimSwitch
{
public:
    ToggleAnim( ActionEventLocator* pActionEventLocator );
    virtual ~ToggleAnim();
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) ToggleAnim( pActionEventLocator );  
    }   
protected:
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );
private:
};
//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/24/2002] 
//////////////////////////////////////////////////////////////////////////
// Change the direction of the animation.
//
class ReverseAnim
:
public ToggleAnim
{
public:
    ReverseAnim( ActionEventLocator* pActionEventLocator )
        :
    ToggleAnim( pActionEventLocator )
    {
    }
    virtual ~ReverseAnim() {}
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) ReverseAnim( pActionEventLocator );  
    } 
protected:
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );
private:
};
//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/24/2002] 
// Start an animation.
// Only play once.
//
//////////////////////////////////////////////////////////////////////////
class PlayAnim
:
public AnimSwitch
{
public:
    PlayAnim( ActionEventLocator* pActionEventLocator );
    virtual ~PlayAnim();
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) PlayAnim( pActionEventLocator );  
    }  
protected:
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );
private:
};
//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/24/2002] 
// Start an animation, let it play.
//////////////////////////////////////////////////////////////////////////
class PlayAnimLoop
:
public AnimSwitch
{
public:
    PlayAnimLoop( ActionEventLocator* pActionEventLocator );
    virtual ~PlayAnimLoop();
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) PlayAnimLoop( pActionEventLocator );  
    }  
protected:
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );
private:
};
//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/24/2002] 
//////////////////////////////////////////////////////////////////////////
class AutoPlayAnim
:
public AnimSwitch
{
public:
    AutoPlayAnim( ActionEventLocator* pActionEventLocator );
    virtual ~AutoPlayAnim();
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) AutoPlayAnim( pActionEventLocator );  
    }  
    virtual void OnEnter( Character* pCharacter );
    virtual void OnExit( Character* pCharacter );   
protected:
    virtual bool IsActionButtonPressed( Character* pCharacter );
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );
    virtual void OnUpdate( float timeins );
    virtual void PositionCharacter( Character* pCharacter, Sequencer* pSeq );
    virtual void SetAnimation( Character* pCharacter, Sequencer* pSeq );
    virtual bool GetIsCyclic( void ) const
    {
        return false;
    }
    int mCharacterCount;
    bool mbJustEmpty    : 1;
private:
    
};
//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/24/2002] 
//////////////////////////////////////////////////////////////////////////
class AutoPlayAnimLoop
:
public AutoPlayAnim
{
public:
    AutoPlayAnimLoop( ActionEventLocator* pActionEventLocator );
    virtual ~AutoPlayAnimLoop();
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) AutoPlayAnimLoop( pActionEventLocator );  
    }    
protected:
    virtual bool GetIsCyclic( void ) const
    {
        return true;
    }
private:
};
//////////////////////////////////////////////////////////////////////////
// 
// TBJ [8/4/2002] 
//
//////////////////////////////////////////////////////////////////////////
class AutoPlayAnimInOut
:
public AutoPlayAnim
{
public:
    AutoPlayAnimInOut( ActionEventLocator* pActionEventLocator );
    virtual ~AutoPlayAnimInOut();
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) AutoPlayAnimInOut( pActionEventLocator );  
    }    
protected:
    virtual bool IsActionButtonPressed( Character* pCharacter );
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );  
    virtual void OnUpdate( float timeins );
    virtual void OnExit( Character* pCharacter ); 
private:
};
//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/30/2002]
//
//////////////////////////////////////////////////////////////////////////
class DestroyObject
:
public AnimSwitch,
public CStatePropListener
{
public:
    DestroyObject( ActionEventLocator* pActionEventLocator );
    virtual ~DestroyObject();
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) DestroyObject( pActionEventLocator );  
    }  
	// Override Create 
	virtual bool Create( tEntityStore* inStore = 0 );
    virtual bool NeedsUpdate( void ) const;

    virtual void Enter( Character* pCharacter );
	virtual void RecieveEvent( int callback , CStateProp* stateProp );

    virtual Type GetType( void ) { return MISSION_OBJECTIVE; };
    
    bool IsInstanceEnabled()const { return mIsEnabled; }



protected:
    virtual void OnReset( void );
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );
    virtual void PositionCharacter( Character* pCharacter, Sequencer* pSeq );
    virtual void SetAnimation( Character* pCharacter, Sequencer* pSeq );
    virtual void OnUpdate( float timeins );
    bool mbDestroyed : 1;

protected:
    StatePropDSG*   mpStatePropDSG;


private:

};

//////////////////////////////////////////////////////////////////////////
// 
// MKR [10/25/2002]
//
// A power coupling for the power plants. When it breaks, it disappears and plays
// Aryan's power coupling explosion effect via the ParticleManager
// 
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/30/2002]
//
//////////////////////////////////////////////////////////////////////////
class UseVendingMachine
:
public PlayAnim
{
public:
    UseVendingMachine( ActionEventLocator* pActionEventLocator );
    virtual ~UseVendingMachine();
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) UseVendingMachine( pActionEventLocator );  
    }  
protected:
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );
    virtual void SetAnimation( Character* pCharacter, Sequencer* pSeq );
private:
};
//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/30/2002]
//
//////////////////////////////////////////////////////////////////////////
class PrankPhone
:
public PlayAnim
{
public:
    PrankPhone( ActionEventLocator* pActionEventLocator );
    virtual ~PrankPhone();
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) PrankPhone( pActionEventLocator );  
    }  
protected:
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );
    virtual void SetAnimation( Character* pCharacter, Sequencer* pSeq );
private:
};
//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/30/2002]
//
//////////////////////////////////////////////////////////////////////////
class SummonVehiclePhoneStaticCallback
:
public LoadingManager::ProcessRequestsCallback
{
public:
    SummonVehiclePhoneStaticCallback( void ) : mpActionEventLocator( 0 ) {}
    ~SummonVehiclePhoneStaticCallback( void );
    void OnProcessRequestsComplete( void* pUserData );  
    ActionEventLocator* mpActionEventLocator;
    VehicleCentral::DriverInit mDriver;
};

class SummonVehiclePhone
:
public ActionEventHandler
{
public:
    SummonVehiclePhone( ActionEventLocator* pActionEventLocator );
    virtual ~SummonVehiclePhone();
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) SummonVehiclePhone( pActionEventLocator );  
    }  

    virtual bool Create( tEntityStore* inStore = 0 );  

    virtual Type GetType( void ) { return ButtonHandler::SUMMON_PHONE; };

    virtual bool UsesActionButton() const;

    class CarSelectionInfo
    {
    public:
        void AddVehicleSelectionInfo( const char* filename, const char* vehiclename, const char* scroobyname, bool bIncrementSlots = true )
        {
            rAssert( ::strlen( filename ) < static_cast<int>( MAX_STRING ) );
            ::strcpy( mFileName, filename );

            rAssert( ::strlen( vehiclename ) < static_cast<int>( MAX_STRING ) );
            ::strcpy( mVehicleName, vehiclename );

            rAssert( ::strlen( scroobyname ) < static_cast<int>( MAX_STRING ) );
            ::strcpy( mScroobyIconName, scroobyname );
            if ( bIncrementSlots )
            {
                sNumUsedSlots++;
            }
        }
        void AddDebugVehicleSelectionInfo( const char* filename, const char* vehiclename, const char* scroobyname )
        {
            AddVehicleSelectionInfo( filename, vehiclename, scroobyname, false );
        }
        static int GetNumUsedSlots( void )
        {
            return sNumUsedSlots;
        }
        static void Reset( void )
        {
            sNumUsedSlots = 0;
        }
        const char* GetFileName( void ) const
        {
            return mFileName;
        }
        const char* GetVehicleName( void ) const
        {
            return mVehicleName;
        }
        const char* GetScroobyIconName( void ) const
        {
            return mScroobyIconName;
        }
    private:
        static const int MAX_STRING = 32;
        char mFileName[MAX_STRING];
        char mVehicleName[MAX_STRING];
        char mScroobyIconName[MAX_STRING];

        static int sNumUsedSlots;
    };  

    static CarSelectionInfo* GetCarSelectInfo( int index )
    {
        if ( index < NUM_CAR_SELECTION_SLOTS )
        {
            return &sCarSelectInfo[ index ];
        }
        return 0;
    }

    static CarSelectionInfo* GetDebugCarSelectInfo( void )
    {
        return GetCarSelectInfo( NUM_CAR_SELECTION_SLOTS - 1 );
    }

    static void ClearCarSelectInfo( void )
    {
        int i;
        for( i = 0; i < NUM_CAR_SELECTION_SLOTS; i++ )
        {
            sCarSelectInfo[ i ].AddVehicleSelectionInfo( "", "", "" );
        }
        CarSelectionInfo::Reset( );
    }
/*
    static int GetSelectedVehicleIndex( void )
    {
        return sSelectedVehicle;
    }
*/
    static const char* GetSelectedVehicleName()
    {
        return sSelectedVehicleName;
    }

//    static void SelectLoadedVehicle( int selectedVehicleIndex );
    static void LoadVehicle( int selectedVehicleIndex );
    static void LoadVehicle( const char* name, const char* filename, VehicleCentral::DriverInit );
    static void LoadDebugVehicle( void );

    // Dumps the currently loaded PBC.
    //
    static void DumpVehicle( void );

    // Enable/Disable the PBCs.
    //
    static void SetEnabled( bool bEnabled )
    {
        sbEnabled = bEnabled;
    }
    // Query the state of the PBCs.
    //
    static bool IsEnabled( void )
    {
        return sbEnabled;
    }

    static SummonVehiclePhoneStaticCallback sCallback;
protected:
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );
    virtual bool OnButtonPressed( Character* pCharacter )
    {
        return false;
    }
    virtual void OnEnter( Character* pCharacter )
    {
    }
    virtual void OnExit( Character* pCharacter )
    {
    }
    virtual void SetAnimation( Character* pCharacter, Sequencer* pSeq );
    virtual void OnUpdate( float timeins );
    virtual bool NeedsUpdate( void ) const
    {
        return true;
    }

private:
    // + 1 for a debug menu.
    //
    static const int NUM_CAR_SELECTION_SLOTS = 3 + 1;
    static CarSelectionInfo sCarSelectInfo[ NUM_CAR_SELECTION_SLOTS ];
//    static const int INVALID_VEHICLE = -1;
//    static int sSelectedVehicle;
    static const int MAX_VEHICLE_NAME_LENGTH = 16;
    static char sSelectedVehicleName[ MAX_VEHICLE_NAME_LENGTH ];
    static bool sbEnabled;
    AnimatedIcon mPhoneIcon;
    int mHudMapIconID;
};
//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/30/2002]
//
//////////////////////////////////////////////////////////////////////////
class Bounce
:
public ActionEventHandler
{
public:
    Bounce( ActionEventLocator* pActionEventLocator );
    virtual ~Bounce();
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) Bounce( pActionEventLocator );  
    }  

    virtual bool Create( tEntityStore* inStore = 0 );

    virtual bool UsesActionButton( void ) const
    {
        return false;
    }
    static void OnEnter( Character* pCharacter, Locator* pLocator );
protected:
    virtual void OnEnter( Character* pCharacter );
private:
};

//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/30/2002]
//
//////////////////////////////////////////////////////////////////////////
class Doorbell
:
public PlayAnim
{
public:
    Doorbell( ActionEventLocator* pActionEventLocator ) : PlayAnim( pActionEventLocator ) {};
    virtual ~Doorbell() {};
    bool Create( tEntityStore* store = 0 );

    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) Doorbell( pActionEventLocator );  
    }  

protected:

    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );
private:
};

//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/30/2002]
//
//////////////////////////////////////////////////////////////////////////
class OpenDoor
:
public PlayAnim
{
public:
    OpenDoor( ActionEventLocator* pActionEventLocator ) : PlayAnim( pActionEventLocator ) {};
    virtual ~OpenDoor() {};
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) OpenDoor( pActionEventLocator );  
    }  
protected:
private:
};

//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/30/2002]
//
//////////////////////////////////////////////////////////////////////////
class TalkFood
:
public PlayAnim
{
public:
    TalkFood( ActionEventLocator* pActionEventLocator ) : PlayAnim( pActionEventLocator ) {};
    virtual ~TalkFood() {};
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) TalkFood( pActionEventLocator );  
    }  
protected:
private:
};

//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/30/2002]
//
//////////////////////////////////////////////////////////////////////////
class TalkCollectible
:
public PlayAnim
{
public:
    TalkCollectible( ActionEventLocator* pActionEventLocator ) : PlayAnim( pActionEventLocator ) {};
    virtual ~TalkCollectible() {};
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) TalkCollectible( pActionEventLocator );  
    }  
protected:
private:
};

//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/30/2002]
//
//////////////////////////////////////////////////////////////////////////
class TalkDialog
:
public PlayAnim
{
public:
    TalkDialog( ActionEventLocator* pActionEventLocator ) : PlayAnim( pActionEventLocator ) {}
    virtual ~TalkDialog() {};
    static ButtonHandler* NewAction( ActionEventLocator* pEventLocator )
    {
        return new (GMA_LEVEL_OTHER) TalkDialog( pEventLocator );  
    }  
protected:
private:
};
//////////////////////////////////////////////////////////////////////////
// 
// TBJ [7/30/2002]
//
//////////////////////////////////////////////////////////////////////////
class TalkMission
:
public PlayAnim
{
public:
    TalkMission( ActionEventLocator* pActionEventLocator ) : PlayAnim( pActionEventLocator ) {};
    virtual ~TalkMission() {};
    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) TalkMission( pActionEventLocator );  
    }  
protected:
private:
};

//////////////////////////////////////////////////////////////////////////
//
// TBJ [8/6/2002] 
//
//////////////////////////////////////////////////////////////////////////
class Collectible
:
public ActionEventHandler
{
public:
    Collectible( ActionEventLocator* pActionEventLocator );
    virtual ~Collectible(); 

    virtual bool Create( tEntityStore* inStore = 0 );
    virtual void ResetCollectible (); //needed a method to reset the mbCollected member
protected:
    virtual void OnReset( void );
    virtual bool ShouldRespawn( ) const
    {
        return false;
    }
    virtual bool IsRespawnTimeExpired( void ) const
    {
        return GetRespawnTime( ) <= 0.0f;
    }
    virtual float GetRespawnTime( void ) const
    {
        return 0.0f;
    }
    virtual void SetRespawnTime( float fTime ) {}
    virtual void UpdateRespawnTime( float timeins ) {}
    virtual void OnUpdate( float timeins );
    virtual void OnEnter( Character* pCharacter );
    virtual void OnExit( Character* pCharacter );
    bool IsCollected( void ) const 
    {
        return  mbCollected;
    }
    virtual bool NeedsUpdate( void ) const
    {
        return true;
    }
protected:
//    AnimEntityDSGWrapper* mpGameObject;
    AnimatedIcon* mAnimatedIcon;
private:
    bool mbCollected : 1;
};

class RespawnCollectible
:
public Collectible
{
public:
    RespawnCollectible( ActionEventLocator* pActionEventLocator );
    virtual ~RespawnCollectible(); 
protected:
    virtual bool ShouldRespawn( ) const
    {
        return true;
    }
    virtual float GetRespawnTime( void ) const
    {
        return mfRespawnTime;
    }
    virtual void SetRespawnTime( float fTime )
    {
        mfRespawnTime = fTime;
    }
    virtual void UpdateRespawnTime( float timeins )
    {
        mfRespawnTime -= timeins;
    }
private:
    float mfRespawnTime;
};
class CollectibleFood
:
public RespawnCollectible
{
public:
    CollectibleFood( ActionEventLocator* pActionEventLocator, float fTurboGain );
    virtual ~CollectibleFood(); 
    static ButtonHandler* NewFoodSmallAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) CollectibleFood( pActionEventLocator, sfSmallTurboGain );  
    } 
    static ButtonHandler* NewFoodLargeAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) CollectibleFood( pActionEventLocator, sfLargeTurboGain );  
    } 
protected:
    virtual void OnEnter( Character* pCharacter );
    float mfTurboGain;
private:
    static float sfSmallTurboGain;
    static float sfLargeTurboGain;
};

class CollectibleCard
:
public Collectible
{
public:
    CollectibleCard( ActionEventLocator* pActionEventLocator );
    virtual ~CollectibleCard(); 
    
    virtual bool Create( tEntityStore* inStore = 0 );  

    virtual Type GetType( void ) { return ButtonHandler::COLLECTOR_CARD; };

    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) CollectibleCard( pActionEventLocator );  
    } 

    static void UpdateThing( unsigned int milliseconds );

protected:
    virtual void OnEnter( Character* pCharacter );
private:
    static AnimatedIcon* mAnimatedCollectionThing;
    static unsigned int mCollectibleCardCount;
};

////////////////////////////////////////////////////////
//wrench class used to fix the car
///////////////////////////////////////////////////////


class WrenchIcon :public Collectible, public RespawnEntity
{
public:
    WrenchIcon( ActionEventLocator* pActionEventLocator );
    virtual ~WrenchIcon(); 
    
    virtual bool Create( tEntityStore* inStore = 0 );  

    virtual Type GetType( void ) { return ButtonHandler::WRENCH_ICON; };

    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) WrenchIcon( pActionEventLocator );  
    } 

   static void UpdateThing( unsigned int milliseconds );
   
   //overriding travis's action button handler virtuals
   void Update(float timeins);
   bool NeedsUpdate()
   {
       return true;
   }
   

protected:
    virtual void OnEnter( Character* pCharacter );
private:
  static AnimatedIcon* mAnimatedCollectionThing;
  static unsigned int mWrenchCount;
};


////////////////////////////////////////////////////////
//Nitro class used in the supersprint
///////////////////////////////////////////////////////


class NitroIcon :public Collectible, public RespawnEntity
{
public:
    NitroIcon( ActionEventLocator* pActionEventLocator );
    virtual ~NitroIcon(); 
    
    virtual bool Create( tEntityStore* inStore = 0 );  

    virtual Type GetType( void ) { return ButtonHandler::NITRO_ICON; };

    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        return new (GMA_LEVEL_OTHER) NitroIcon( pActionEventLocator );  
    } 

   static void UpdateThing( unsigned int milliseconds );
   
   //overriding travis's action button handler virtuals
   void Update(float timeins);
   bool NeedsUpdate()
   {
       return true;
   }
   

protected:
    virtual void OnEnter( Character* pCharacter );
private:
  static AnimatedIcon* mAnimatedCollectionThing;
  static unsigned int mNitroCount;
};

class GenericEventButtonHandler : public ButtonHandler
{
public:
    GenericEventButtonHandler( EventLocator* pEventLocator, EventEnum event );
    virtual ~GenericEventButtonHandler();

    static ButtonHandler* NewAction( EventLocator* pEventLocator, EventEnum event, GameMemoryAllocator alloc = GMA_LEVEL_OTHER );
    void SetEventLocator( EventLocator* pEventLocator );

    EventLocator* GetEventLocator( void ) const
    {
        return mpEventLocator;
    }

    void SetEventData( void* data ) { mEventData = data; };
    void* GetEventData() { return mEventData; };

    virtual Type GetType( void ) { return MISSION_OBJECTIVE; };

protected:
    EventLocator* mpEventLocator;

    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );

private:
    EventEnum mEvent;
    void* mEventData;
};

class TeleportAction : public ActionEventHandler, public EventListener
{
public:
    TeleportAction( ActionEventLocator* pActionEventLocator );
    virtual ~TeleportAction();

    static ButtonHandler* NewAction( ActionEventLocator* pEventLocator )
    {
        ButtonHandler* handler = NULL;
        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
        handler =  new TeleportAction( pEventLocator );  
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
        return handler;
    }  

    virtual bool Create( tEntityStore* inStore = 0 );
    virtual Type GetType( void ) { return TELEPORT; };

    virtual void HandleEvent( EventEnum id, void* pEventData );

protected:
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq );

    virtual void OnEnter( Character* pCharacter );
    virtual void OnExit( Character* pCharacter );
};

class PurchaseReward : public ActionEventHandler
{
public:
    PurchaseReward( ActionEventLocator* pActionEventLocator );
    virtual ~PurchaseReward();

    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator );
    virtual bool Create( tEntityStore* inStore = 0 ) = 0;
    virtual Type GetType( void ) = 0;
    virtual bool UsesActionButton() const = 0;

    static void SetEnabled( bool bEnabled )
    {
        sbEnabled = bEnabled;
    }
    static bool IsEnabled( void )
    {
        return sbEnabled;
    }

protected:
    bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq ) { return OnButtonPressed( pCharacter ); };
    virtual bool OnButtonPressed( Character* pCharacter ) = 0;
    virtual void OnEnter( Character* pCharacter ) = 0;
    virtual void OnExit( Character* pCharacter ) = 0;
    virtual void OnUpdate( float timeins );
    bool NeedsUpdate( void ) const { return true; };
    bool CanEnable() const;

    AnimatedIcon mIcon;
    bool mAllPurchased : 1;
    bool mIsActive     : 1;
    int mHudMapIconID;

private:
    static bool sbEnabled;

};

class PurchaseCar : public PurchaseReward
{
public:
    PurchaseCar( ActionEventLocator* pActionEventLocator );
    virtual ~PurchaseCar();

    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        ButtonHandler* handler = NULL;
        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
        handler =  new PurchaseCar( pActionEventLocator );  
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
        return handler;
    }  

    virtual bool Create( tEntityStore* inStore = 0 );
    virtual Type GetType( void ) { return ButtonHandler::PURCHASE_CAR; };
    virtual bool UsesActionButton() const { return true; };

protected:
    virtual bool OnButtonPressed( Character* pCharacter );
    virtual void OnEnter( Character* pCharacter );
    virtual void OnExit( Character* pCharacter );
    virtual void OnUpdate( float timeins );

private:
    GuiSFX::InputStateChange m_DisableInput;
    GuiSFX::GotoScreen    m_GotoLetterbox;
    GuiSFX::SwitchContext m_ContextSwitch;
    GuiSFX::RecieveEvent  m_WaitForConversationDone;
    GuiSFX::GotoScreen    m_GotoHud;
    GuiSFX::GotoScreen    m_GotoScreen;
};

class PurchaseSkin : public PurchaseReward
{
public:
    PurchaseSkin( ActionEventLocator* pActionEventLocator );
    virtual ~PurchaseSkin();

    static ButtonHandler* NewAction( ActionEventLocator* pActionEventLocator )
    {
        ButtonHandler* handler = NULL;
        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
        handler =  new PurchaseSkin( pActionEventLocator );  
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
        return handler;
    }  

    virtual bool Create( tEntityStore* inStore = 0 );
    virtual Type GetType( void ) { return ButtonHandler::PURCHASE_SKIN; };
    virtual bool UsesActionButton() const { return true; };

protected:
    virtual bool OnButtonPressed( Character* pCharacter );
    virtual void OnEnter( Character* pCharacter );
    virtual void OnExit( Character* pCharacter );
    virtual void OnUpdate( float timeins );
};

}; // namespace ActionButton
#endif //ACTIONBUTTONHANDLER_H
