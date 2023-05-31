//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        deliveryobjective.cpp
//
// Description: Implement DeliveryObjective
//
// History:     16/04/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <raddebugwatch.hpp>
#include <p3d/utility.hpp>

//========================================
// Project Includes
//========================================
#include <mission/objectives/deliveryobjective.h>
#include <mission/gameplaymanager.h>

#include <events/eventmanager.h>
#include <events/eventdata.h>

#include <meta/eventlocator.h> 

#include <presentation/gui/guisystem.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>

#include <worldsim/redbrick/vehicle.h>
#include <render/DSG/StatePropDSG.h>
#include <meta/actioneventlocator.h>
#include <ai/actionbuttonhandler.h>
#include <ai/actionbuttonmanager.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
#ifdef DEBUGWATCH
static unsigned int D_DEFAULT_TIMEOUT = 5000;
#else
static const unsigned int D_DEFAULT_TIMEOUT = 5000;
#endif

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// DeliveryObjective::DeliveryObjective
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
DeliveryObjective::DeliveryObjective() :
    mDumpTimeout( 0 )
{
    // Allocate mStateProps array
    mStateProps.Allocate( MAX_COLLECTIBLES );
}

//==============================================================================
// DeliveryObjective::~DeliveryObjective
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
DeliveryObjective::~DeliveryObjective()
{
    ReleaseAllStateProps();
}

//=============================================================================
// DeliveryObjective::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EvenEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void DeliveryObjective::HandleEvent( EventEnum id, void* pEventData )
{
    if ( id == EVENT_VEHICLE_VEHICLE_COLLISION && GetNumCollected() > 0 && mDumpTimeout == 0 )
    {
        CarOnCarCollisionEventData* data = static_cast<CarOnCarCollisionEventData*>(pEventData);
        if ( data->vehicle != GetGameplayManager()->GetCurrentVehicle() && data->vehicle->mVehicleType == VT_AI )
        {
            rAssert( IsUserDumpAllowed() );

            int dumpId = GetAnyCollectedID();
            rAssert( dumpId != -1 );

            DumpCollectible( dumpId, GetGameplayManager()->GetCurrentVehicle(), data->vehicle );
            Uncollect( dumpId );

            CGuiScreenHud* currentHud = GetCurrentHud();
            if( currentHud )
            {
                //Update the collection count
                currentHud->SetCollectibles( GetNumCollected(), GetNumCollectibles() );
            }

            //Reset the dumo timeout.
            mDumpTimeout = D_DEFAULT_TIMEOUT;
        }
    }
 
    bool handledCollect = false;

    if ( id == EVENT_OBJECT_DESTROYED )
    {
        // Is this a stateprop? If so, deactivate the hud icon
        if ( pEventData )
        {
            Locator* locator = dynamic_cast< Locator* >( static_cast< tEntity* >( pEventData ) );
            // Iterate through the collectible locators and see if we have a pointer match
            for ( unsigned int i = 0 ; i < GetNumCollectibles() ; i++ )
            {                           
                if ( locator == GetCollectibleLocator( i ) )
                {
                    // Match, disable hud icon then abort this loop
                    UnregisterLocator( mCollectibles[ i ].iHUDIndex );
                    bool shouldReset = false;
                    if ( OnCollection( i, shouldReset ) )
                    {
                        Collect( i, shouldReset );
                    }
                    if( mNumCollected == mNumCollectibles )
                    {
                        SetFinished( true );
                    }
                    handledCollect = true;
                    break;
                }
            }
        }
    }

    if ( handledCollect == false )
        CollectibleObjective::HandleEvent( id, pEventData );
}

//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************

//=============================================================================
// DeliveryObjective::InitCollectibles
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DeliveryObjective::OnInitCollectibles()
{
    unsigned int i;
    for( i = 0; i < GetNumCollectibles(); i++ )
    {
        Activate( i, true, false, HudMapIcon::ICON_COLLECTIBLE );
    }

    // Find all the stateprops (could be none)
    FindStateProps();

    SetStatePropHUDIconEnable( true );
    SetButtonHandlersEnabled( false );
    // Make the stateprops visible
    SetStatePropState( 1 );

    SetFocus( 0 );
}

//=============================================================================
// DeliveryObjective::OnInitCollectibleObjective
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DeliveryObjective::OnInitCollectibleObjective()
{
    if( mNumCollectibles > 1 ) // only show collectibles HUD overlay if more than 1 things to collect
    {
        GetGuiSystem()->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_COLLECTIBLES );
    }

    if ( GetCurrentHud() )
    {
        GetCurrentHud()->SetCollectibles( 0, GetNumCollectibles() );
    }

    if ( IsUserDumpAllowed() )
    {
        GetEventManager()->AddListener( this, EVENT_VEHICLE_VEHICLE_COLLISION );
        mDumpTimeout = 0;
#ifdef DEBUGWATCH
        char name[64];
        sprintf( name, "Mission\\Objectives\\Delivery" );
        radDbgWatchAddUnsignedInt( &D_DEFAULT_TIMEOUT, "Dump timeout", name, NULL, 0, 100000 );
#endif
    }
}

//=============================================================================
// DeliveryObjective::OnFinalizeCollectibleObjective
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DeliveryObjective::OnFinalizeCollectibleObjective()
{
    // Turn off the handlers 
    SetButtonHandlersEnabled( false );
    // Lets iterate through the stateprops (if any exist) 
    // And revert them to state 0. In this state, they will be deactivated 
    // invisible, and uncollideable
    SetStatePropState( 0 );
    SetStatePropHUDIconEnable( false );

    GetGuiSystem()->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_COLLECTIBLES );


    if ( IsUserDumpAllowed() )
    {
        GetEventManager()->RemoveListener( this, EVENT_VEHICLE_VEHICLE_COLLISION );
#ifdef DEBUGWATCH
        radDbgWatchDelete( &D_DEFAULT_TIMEOUT );
#endif
    }
    ReleaseAllStateProps();
}

//=============================================================================
// DeliveryObjective::OnCollection
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int collectibleNum, bool &shouldReset )
//
// Return:      bool 
//
//=============================================================================
bool DeliveryObjective::OnCollection( unsigned int collectibleNum, bool &shouldReset )
{
    if ( GetCurrentHud() )
    {
        GetCurrentHud()->SetCollectibles( GetNumCollected() + 1, GetNumCollectibles() );  //I add 1 because the number is incremented after...  Sorry
    }

    unsigned int i;
    for ( i = 0; i < GetNumCollectibles(); ++i )
    {
        if ( !IsCollected( i ) && i != collectibleNum )
        {
            SetFocus( i );
            break;
        }
    }

    return true;  //The order doesn't matter
}

//=============================================================================
// DeliveryObjective::OnUpdateCollectibleObjective
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void DeliveryObjective::OnUpdateCollectibleObjective( unsigned int elapsedTime )
{
    if ( IsUserDumpAllowed() )
    {
        if ( mDumpTimeout != 0 )            
        {
            if ( elapsedTime > mDumpTimeout )
            {
                mDumpTimeout = 0;
            }
            else
            {
                mDumpTimeout -= elapsedTime;
            }
        }
    }
}


//=============================================================================
// DeliveryObjective::FindStateProps
//=============================================================================
// Description: 
//              Attempts to find the stateprops (if any) associated with this objective
//
// Parameters:  (none)
//
// Return:      void 
//
//=============================================================================
void DeliveryObjective::FindStateProps()
{
    // Is the array already filled out
    if ( mStateProps.mUseSize > 0 )
        return;

    for( unsigned int i = 0; i < GetNumCollectibles(); i++ )
    {
        const Locator* loc = GetCollectibleLocator( i );
        // Check to see if the locator is an ActionEventLocator
        const ActionEventLocator* actionEventLoc = dynamic_cast< const ActionEventLocator* >( loc );
        if( actionEventLoc )
        {
            // Find the stateprop and set its state to 1 (active)
            const char* objName = actionEventLoc->GetObjName();
            StatePropDSG* pDSG = p3d::find< StatePropDSG >( objName );
            if ( pDSG )
            {
                pDSG->AddRef();
                mStateProps.Add( pDSG );
            }
        }
    }
}

//=============================================================================
// DeliveryObjective::ReleaseAllStateProps
//=============================================================================
// Description: 
//              Iterates through mStateProps and decrements their refcounts
//
// Parameters:  
//
// Return:      void 
//
//=============================================================================
void DeliveryObjective::ReleaseAllStateProps()
{
    for ( int i = 0 ; i < mStateProps.mUseSize ; i++ )
    {
        if ( mStateProps[i] != NULL )
        {
            mStateProps[i]->Release();
            mStateProps[i] = NULL;
        }
    }
    mStateProps.ClearUse();
}


//=============================================================================
// DeliveryObjective::SetButtonHandlersEnabled
//=============================================================================
// Description: 
//              This iterates through the collectibles and turns their associated
//              ActionEventHandlers enabled flag on or off
//              This is used to have the HUD icon only visible when the mission is in play
//
// Parameters:  (bool enable)
//
// Return:      void 
//
//=============================================================================
void DeliveryObjective::SetButtonHandlersEnabled( bool enable )
{
    for( unsigned int i = 0; i < GetNumCollectibles(); i++ )
    {
        const Locator* loc = GetCollectibleLocator( i );
        // Check to see if the locator is an ActionEventLocator
        const ActionEventLocator* actionEventLoc = dynamic_cast< const ActionEventLocator* >( loc );
        if( actionEventLoc )
        {
            ActionButton::ActionEventHandler* handler = GetActionButtonManager()->FindHandler( actionEventLoc );
            if ( handler )
            {
                handler->SetInstanceEnabled( enable );
            }
        }
    }
}

//=============================================================================
// DeliveryObjective::SetStatePropState
//=============================================================================
// Description:   Iterates through the collectibles, any stateprops it finds associated
//                with the collectible locators gets set to the given
//                state. This Allows the stateprops to appear only when the mission is active
//                Note that Aryan set it up so that state 0 is disabled (invisible)
//                And state 1+ is enabled (visible)
//
// Parameters:  (int state)
//
// Return:      void 
//
//=============================================================================
void DeliveryObjective::SetStatePropState( int state )
{
    for ( int i = 0 ; i < mStateProps.mUseSize ; i++ )
    {
        mStateProps[i]->SetState( state );
    }
}

//=============================================================================
// DeliveryObjective::SetStatePropHUDIconEnable
//=============================================================================
// Description:  Iterates through the stateprops (if any) and enables/disables HUD icons
//
// Parameters:  (bool enable)
//
// Return:      void 
//
//=============================================================================
// Iterates through the stateprops (if any) and enables/disables HUD icons
void DeliveryObjective:: SetStatePropHUDIconEnable( bool enable )
{
    // Dont do anything if this is not a stateprop mission
    if ( mStateProps.mUseSize == 0 )
        return;

    const bool primary = true;
    for( unsigned int i = 0; i < GetNumCollectibles(); i++ )
    {
        if ( enable )
            RegisterLocator( mCollectibles[ i ].pLocator, mCollectibles[ i ].iHUDIndex, primary, HudMapIcon::ICON_COLLECTIBLE );
        else
            UnregisterLocator( mCollectibles[ i ].iHUDIndex );
    }
}
