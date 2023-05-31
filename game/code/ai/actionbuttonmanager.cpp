//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implementation of class ActionButtonManager
//
// History:     18/07/2002 + Created -- TBJ
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <memory/srrmemory.h>

//========================================
// Project Includes
//========================================
#include <worldsim/character/charactercontroller.h> 

#include <ai/actionbuttonmanager.h>
#include <ai/actionbuttonhandler.h>
#include <ai/actionlist.h>
#include <ai/actionnames.h>

#include <render/DSG/animcollisionentitydsg.h>


#include <memory/srrmemory.h>
#include <meta/actioneventlocator.h>
#include <meta/eventlocator.h>
#include <meta/spheretriggervolume.h>
#include <meta/triggervolumetracker.h>

#include <console/console.h>

#include <render/RenderManager/RenderManager.h>
#include <render/RenderManager/RenderLayer.h>
#include <render/RenderManager/WorldRenderLayer.h>
#include <render/loaders/billboardwrappedloader.h>
#include <mission/gameplaymanager.h>

#include <events/eventmanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

ActionButtonManager* ActionButtonManager::spActionButtonManager = (ActionButtonManager*)0;
//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// ActionButtonManager::ActionButtonManager
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
static int s_currentID = 0;

ActionButtonManager::ActionButtonManager()
:
mpCurrentToResolve( 0 ),
mLoadingIntoInterior( false )
{
    int i;
    for ( i = 0; i < MAX_ACTIONS; i++ )
    {
        mActionButtonList[ i ].handler = NULL;
        mActionButtonList[ i ].sectionName = 0;
        mbActionButtonNeedsUpdate[ i ] = false;
    }

    // Sanity check.
    //
    rAssertMsg( ActionButton::ButtonNameListSize == CharacterController::NUM_INPUTS, "Button Name list size does not match enumeration size defined in .\\worldsim\\character\\charactercontroller.h\n" );
    rAssertMsg( ActionButton::nameIndex == ActionButton::ActionNameSize, "Action Name list size does not match enumeration size!\n" );

    Console* pConsole = GetConsole();
    if ( pConsole )
    {
        pConsole->AddFunction( "AddVehicleSelectInfo", AddVehicleSelectInfo, "", 3, 3 ); //One entry in phone selectable vehicles.
        pConsole->AddFunction( "ClearVehicleSelectInfo", ClearVehicleSelectInfo, "", 0, 0 ); //Clear all entries in phone selectable vehicles.
    }

    GetEventManager()->AddListener( this, EVENT_DUMP_DYNA_SECTION );
}

/*
==============================================================================
ActionButtonManager::FindHandler
==============================================================================
Description:    Returns the ButtonHandler that is associated with the given 
                locator. NULL if none are found

Parameters:     const ActionEventLocator* locator

Return:         ActionButton::ButtonHandler* 

=============================================================================
*/
ActionButton::ActionEventHandler* ActionButtonManager::FindHandler( const ActionEventLocator* locator )const
{
    // Iterate through the ButtonHandlers and find the Handler that contains the given locator
    for ( int i = 0 ; i < MAX_ACTIONS ; i++ )
    {
        // Expensive dynamic cast 
        ActionButton::ActionEventHandler* handler = dynamic_cast< ActionButton::ActionEventHandler* >( mActionButtonList[i].handler );
        if ( handler )
        {
            // Is this the locator we are looking for
            if ( locator == handler->GetActionEventLocator() )
            {
                return handler;
            }
        }
    }
    // No Handler found, return NULL
    return NULL;   
}


/*
==============================================================================
ActionButtonManager::AddVehicleSelectInfo
==============================================================================
Description:    Scripter hook

Parameters:     ( int argc, char** argv )

Return:         void 

=============================================================================
*/
void ActionButtonManager::AddVehicleSelectInfo( int argc, char** argv )
{
    rWarningMsg( false, "ActionButtonManager::AddVehicleSelectInfo() function is deprecated!" );
/*
    int index = ActionButton::SummonVehiclePhone::CarSelectionInfo::GetNumUsedSlots();
    ActionButton::SummonVehiclePhone::CarSelectionInfo* pInfo = ActionButton::SummonVehiclePhone::GetCarSelectInfo( index );
    if ( pInfo )
    {
        pInfo->AddVehicleSelectionInfo( argv[1], argv[2], argv[3] );
    }
    else
    {
        rReleasePrintf( "Too many vehicle select info lines.  Call ClearVehicleSelectInfo() first!." );
    }
*/
}

/*
==============================================================================
ActionButtonManager::ClearVehicleSelectInfo
==============================================================================
Description:    Scripter hook

Parameters:     ( int argc, char** argv )

Return:         void 

=============================================================================
*/
void ActionButtonManager::ClearVehicleSelectInfo( int argc, char** argv )
{
    rWarningMsg( false, "ActionButtonManager::ClearVehicleSelectInfo() function is deprecated!" );
/*
    ActionButton::SummonVehiclePhone::ClearCarSelectInfo( );
*/
}

//=============================================================================
// ActionButtonManager::OnProcessRequestsComplete
//=============================================================================
// Description: Comment
//
// Parameters:  ( void* pUserData )
//
// Return:      void 
//
//=============================================================================
void ActionButtonManager::OnProcessRequestsComplete( void* pUserData )
{
    BillboardWrappedLoader::OverrideLoader( false );
    GetRenderManager()->pWorldRenderLayer()->DoPostDynaLoad();
}


//==============================================================================
// ActionButtonManager::~ActionButtonManager
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
ActionButtonManager::~ActionButtonManager()
{
    Destroy( );

    GetEventManager()->RemoveAll( this );
}
/*
==============================================================================
ActionButtonManager::Destroy
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void ActionButtonManager::Destroy( void )
{
    int i;
    for ( i = 0; i < MAX_ACTIONS; i++ )
    {
        // Delete the actionbuttonhandler.
        //
        RemoveActionByArrayPos( i );
    }

    s_currentID = 0;
}
/*
==============================================================================
ActionButtonManager::CreateInstance
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ActionButtonManager

=============================================================================
*/
ActionButtonManager*  ActionButtonManager::CreateInstance( void )
{
	rAssertMsg( spActionButtonManager == 0, "ActionButtonManager already created.\n" );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_PERSISTENT );
    #endif

	spActionButtonManager = new ActionButtonManager;

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_PERSISTENT );
    #endif

    return( spActionButtonManager );
}
/*
==============================================================================
ActionButtonManager::GetInstance
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         ActionButtonManager

=============================================================================
*/
ActionButtonManager* ActionButtonManager::GetInstance( void )
{
	rAssertMsg( spActionButtonManager != 0, "ActionButtonManager has not been created yet.\n" );
	return spActionButtonManager;
}

/*
==============================================================================
ActionButtonManager::DestroyInstance
==============================================================================
Description:    Destroy the singleton.

Parameters:     ( void )

Return:         n/a

=============================================================================
*/
void ActionButtonManager::DestroyInstance( void )
{
    rAssert( spActionButtonManager != NULL );

    #ifdef RAD_GAMECUBE
        delete( GMA_GC_VMM, spActionButtonManager );
    #else
        delete( GMA_PERSISTENT, spActionButtonManager );
    #endif

    spActionButtonManager = NULL;
}

/*
==============================================================================
ActionButtonManager::EnterGame
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void ActionButtonManager::EnterGame( void )
{
}

/*
==============================================================================
ActionButtonManager::Update
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void ActionButtonManager::Update( float timeins )
{
    int i;
    for ( i = 0; i < MAX_ACTIONS; i++ )
    {
        if ( mActionButtonList[ i ].handler != 0 && mActionButtonList[ i ].handler->NeedsUpdate( ) )
        {
            mActionButtonList[ i ].handler->Update( timeins );
        }
    }

     unsigned int time = rmt::FtoL( timeins * 1000.0f );

     //These are the collection effects.  They are one shot and remove
     //themselves from the world when they're done.
    ActionButton::CollectibleCard::UpdateThing( time  );
    ActionButton::WrenchIcon::UpdateThing( time );
    ActionButton::NitroIcon::UpdateThing( time );
}

/*
==============================================================================
ActionButtonManager::AddActionEventLocator
==============================================================================
Description:    Comment

Parameters:     ( ActionEventLocator* pActionEventLocator )

Return:         bool 

=============================================================================
*/
bool ActionButtonManager::AddActionEventLocator( ActionEventLocator* pActionEventLocator, tEntityStore* inStore )
{
    rAssert( pActionEventLocator );
    return LinkActionToLocator( pActionEventLocator, inStore );
}

/*
==============================================================================
ActionButtonManager::GetActionAt
==============================================================================
Description:    Comment

Parameters:     ( int i )

Return:         ActionButtonHandler

=============================================================================
*/
ActionButton::ButtonHandler* ActionButtonManager::GetActionByIndex( int i ) const
{
    i = Find(i);

    if ( i < MAX_ACTIONS && i >= 0 )
    {
        return mActionButtonList[ i ].handler;
    }
    else
    {
        return 0;
    }
}


/*
==============================================================================
ActionButtonManager::AddAction
==============================================================================
Description:    Comment

Parameters:     ( ActionButtonHandler* pAction )

Return:         bool 

=============================================================================
*/
bool ActionButtonManager::AddAction( ActionButton::ButtonHandler* pAction, int& id, tUID section)
{
    rAssert( pAction );
    int i;
    for ( i = 0; i < MAX_ACTIONS; i++ )
    {
        if ( mActionButtonList[ i ].handler == (ActionButton::ButtonHandler*)0 )
        {
            break;
        }
    }
    if ( i < MAX_ACTIONS )
    {
        mActionButtonList[ i ].handler = pAction;
        mActionButtonList[ i ].handler->AddRef();
        mActionButtonList[ i ].id = s_currentID++;
        mActionButtonList[ i ].sectionName = section;
        id = mActionButtonList[ i ].id;
        return true;
    }
    id = -1;
    return false;
}

/*
==============================================================================
ActionButtonManager::RemoveActionByIndex
==============================================================================
Description:    Comment

Parameters:     ( int id )

Return:         bool 

=============================================================================
*/
bool ActionButtonManager::RemoveActionByIndex( int id )
{
    return RemoveActionByArrayPos(Find(id));
}
/*
==============================================================================
ActionButtonManager::RemoveAction
==============================================================================
Description:    Comment

Parameters:     ( ActionButton::ButtonHandler* pAction )

Return:         int 

=============================================================================
*/
int ActionButtonManager::RemoveAction( ActionButton::ButtonHandler* pAction )
{
    rAssert( pAction );
    int id = -1;
    int i;
    for ( i = 0; i < MAX_ACTIONS; i++ )
    {
        if ( mActionButtonList[ i ].handler == pAction )
        {
            break;
        }
    }
    if ( i < MAX_ACTIONS )
    {
        RemoveActionByArrayPos( i );
        id = i;
    }    
    return id;    
}
/*
==============================================================================
ActionButtonManager::CreateActionEventTrigger
==============================================================================
Description:    Comment

Parameters:     ( const char* triggerName, rmt::Vector& pos, float r )

Return:         bool 

=============================================================================
*/
bool ActionButtonManager::CreateActionEventTrigger( const char* triggerName, rmt::Vector& pos, float r )
{
    return true;
}
/*
==============================================================================
ActionButtonManager::LinkActionToObjectJoint
==============================================================================
Description:    Comment

Parameters:     ( const char* objectName, const char* jointName, const char* triggerName, const char* typeName, const char* buttonName )

Return:         bool 

=============================================================================
*/
bool ActionButtonManager::LinkActionToObjectJoint( const char* objectName, const char* jointName, const char* triggerName, const char* typeName, const char* buttonName )
{   
    return LinkActionToObject( objectName, jointName, triggerName, typeName, buttonName, true );
}
/*
==============================================================================
ActionButtonManager::LinkActionToObject
==============================================================================
Description:    Comment

Parameters:     ( const char* objectName, const char* triggerName, const char* typeName, const char* buttonName )

Return:         bool 

=============================================================================
*/
bool ActionButtonManager::LinkActionToObject( const char* objectName, const char* jointName, const char* triggerName, const char* typeName, const char* buttonName, bool attachToJoint )
{
    return true;
} 

/*
==============================================================================
ActionButtonManager::LinkActionToObject
==============================================================================
Description:    Comment

Parameters:     ( ActionEventLocator* pActionEventLocator )

Return:         bool 

=============================================================================
*/
bool ActionButtonManager::LinkActionToLocator( ActionEventLocator* pActionEventLocator, tEntityStore* inStore )
{
    ActionButton::ButtonHandler* pABHandler = this->NewActionButtonHandler( pActionEventLocator->GetActionName(), pActionEventLocator );
    bool bAdded = false;
    
    if ( pABHandler != 0 )
    {
        rAssert( dynamic_cast< ActionButton::ActionEventHandler*>( pABHandler ) != NULL );
        ActionButton::ActionEventHandler* pActionHandler = static_cast<ActionButton::ActionEventHandler*>( pABHandler );    
        rAssert( pActionHandler );
        pActionHandler->AddRef();
    
        
        int id = -1;
        bool bCreated = pActionHandler->Create( inStore );
        if ( bCreated )
        {
            tUID section = 0;
            if ( GetRenderManager()->pWorldRenderLayer()->GetCurrentState() == WorldRenderLayer::msLoad )
            {
                section = GetRenderManager()->pWorldRenderLayer()->GetCurSectionName().GetUID();
            }

            bAdded = this->AddAction( pActionHandler, id, section);
            rAssert( bAdded );
            pActionEventLocator->SetData( id );
        }
        else
        {
            rReleasePrintf( "****************** Failed to create %s, action type %s\n", pActionEventLocator->GetObjName( ), pActionEventLocator->GetActionName( ) );
        } 

        //HACK
        mLoadingIntoInterior = false;

        // We are done with it at this level.
        // If it was not added, then this will delete the object.
        //
        pActionHandler->Release( );
    }    
    return bAdded;
}

/*
==============================================================================
ActionButtonManager::EnterActionTrigger
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, int index )

Return:         void 

=============================================================================
*/
void ActionButtonManager::EnterActionTrigger( Character* pCharacter, int index )
{
    index = Find(index);

    rAssert( index < MAX_ACTIONS && index >= 0 );
    
    if ( index < MAX_ACTIONS && index >= 0 )
    {
        rAssert( mActionButtonList[ index ].handler != 0 );
        if ( mActionButtonList[ index ].handler != 0 )
        {
            mActionButtonList[ index ].handler->Enter( pCharacter );
        }
    }
}
/*
==============================================================================
ActionButtonManager::ExitActionTrigger
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, int index )

Return:         void 

=============================================================================
*/
void ActionButtonManager::ExitActionTrigger( Character* pCharacter, int index )
{
    index = Find(index);

    rAssert( index < MAX_ACTIONS && index >= 0 );
    
    if ( index < MAX_ACTIONS && index >= 0 )
    {
        rAssert( mActionButtonList[ index ].handler != 0 );
        if ( mActionButtonList[ index ].handler != 0 )
        {
            mActionButtonList[ index ].handler->Exit( pCharacter );
        }
    }
}
/*
==============================================================================
ActionButtonManager::NewActionButtonHandler
==============================================================================
Description:    Comment

Parameters:     ( const char* typeName, ActionEventLocator* pActionEventLocator )

Return:         ActionButtonHandler

=============================================================================
*/
ActionButton::ButtonHandler* ActionButtonManager::NewActionButtonHandler( const char* typeName, ActionEventLocator* pActionEventLocator )
{
    ActionButton::ButtonHandler* pActionButtonHandler = 0;  
    int i;
    for ( i = 0; i < ActionButton::ActionListSize; i++ )
    {
        if ( ActionButton::CompareActionType( typeName, ActionButton::theListOfActions[ i ].mActionKey ) )
        {
            pActionButtonHandler = ActionButton::theListOfActions[ i ].actionPtr( pActionEventLocator );
            break;
        }
    }
    
    if ( !pActionButtonHandler )
    {
        rDebugPrintf("Could not create action button handler type %s.  Update your worldbuilder and reexport.", typeName );
    }

    return pActionButtonHandler;
}
/*
==============================================================================
ActionButtonManager::ResolveActionTrigger
==============================================================================
Description:    Comment

Parameters:     ( AnimCollisionEntityDSG* pAnimObject )

Return:         bool, true if successfully resolved. 

=============================================================================
*/
bool ActionButtonManager::ResolveActionTrigger( AnimCollisionEntityDSG* pAnimObject, tEntityStore* inStore )
{
    p3d::inventory->PushSection(); 
    p3d::inventory->SelectSection("Default");
    mpCurrentToResolve = pAnimObject;

    ActionEventLocator* pLocator = p3d::find<ActionEventLocator>( pAnimObject->GetUID() );
    bool bAdded = false;
    if ( pLocator )
    {
        bAdded = LinkActionToLocator( pLocator, inStore );
        rAssert( bAdded );
    }
    mpCurrentToResolve = 0;
    p3d::inventory->PopSection(); 
    return bAdded;
}

//=============================================================================
// ActionButtonManager::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void ActionButtonManager::HandleEvent( EventEnum id, void* pEventData )
{
    //When the event EVENT_DUMP_DYNA_SECTION is sent, the dyna load section UID is 
    //compared to the stored section of the action butons.  Any buttons created during the
    //specified load section are released.
    unsigned int i;
    for ( i = 0; i < MAX_ACTIONS; ++i )
    {
        if ( mActionButtonList[ i ].handler != NULL &&
             mActionButtonList[ i ].sectionName == static_cast<tName*>(pEventData)->GetUID() )
        {
            //Remove this action button.
            RemoveActionByArrayPos( i );
        }
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

int ActionButtonManager::Find(int id) const
{
    unsigned int i;
    for ( i = 0; i < MAX_ACTIONS; ++i )
    {
        if ( mActionButtonList[ i ].handler != NULL &&
             mActionButtonList[ i ].id == id )
        {
            return i;
        }
    }
    return -1;
}

bool  ActionButtonManager::RemoveActionByArrayPos(int id)
{
    if( id < MAX_ACTIONS && id >= 0 )
    {
        if ( mActionButtonList[ id ].handler != 0 )
        {
            mActionButtonList[ id ].handler->Release( );
            mActionButtonList[ id ].handler = 0;
            return true;
        }
    }
    return false;
}
