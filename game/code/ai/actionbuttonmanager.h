//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        actionbuttonmanager.h
//
// Description: Blahblahblah
//
// History:     18/07/2002 + Created -- TBJ
//
//=============================================================================

#ifndef ACTIONBUTTONMANAGER_H
#define ACTIONBUTTONMANAGER_H

//========================================
// Nested Includes
//========================================
#include <p3d/p3dtypes.hpp>
#include <radmath/radmath.hpp>
#include <loading/loadingmanager.h>
#include <events/eventlistener.h>

//========================================
// Forward References
//========================================
namespace ActionButton
{
    class ButtonHandler;
    class ActionEventHandler;
};
class Character;
class ActionEventLocator;
class AnimCollisionEntityDSG;
class tCompositeDrawable;
class tEntityStore;
//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class ActionButtonManager : public LoadingManager::ProcessRequestsCallback, public EventListener
{
public:
    enum { MAX_ACTIONS = 128 };

    ActionButtonManager();
    ~ActionButtonManager();

    static ActionButtonManager* GetInstance( );
    static ActionButtonManager* CreateInstance( );
    static void DestroyInstance();

    void EnterGame( void );
    void Destroy( void );
    
    void Update( float timeins );

    bool AddActionEventLocator( ActionEventLocator* pActionEventLocator, tEntityStore* inStore = 0 );

    ActionButton::ButtonHandler* GetActionByIndex( int i ) const;
    int GetIndexByAction( ActionButton::ButtonHandler* ) const;

    bool AddAction( ActionButton::ButtonHandler* pAction, int& id, tUID section = 0);
    bool RemoveActionByIndex( int id );
    int RemoveAction( ActionButton::ButtonHandler* pAction );
    ActionButton::ButtonHandler* NewActionButtonHandler( const char* typeName, ActionEventLocator* pActionEventLocator );

    bool CreateActionEventTrigger( const char* triggerName, rmt::Vector& pos, float r );
    bool LinkActionToObjectJoint( const char* objectName, const char* jointName, const char* triggerName, const char* typeName, const char* buttonName );
    bool LinkActionToObject( const char* objectName, const char* jointName, const char* triggerName, const char* typeName, const char* buttonName, bool bAttachToJoint );
    
    bool LinkActionToLocator( ActionEventLocator* pActionEventLocator, tEntityStore* inStore = 0 );

    void EnterActionTrigger( Character* pCharacter, int index );
    void ExitActionTrigger( Character* pCharacter, int index );

    bool ResolveActionTrigger( AnimCollisionEntityDSG* pAnimObject, tEntityStore* inStore = 0 );

    AnimCollisionEntityDSG* GetCurrentAnimObjectToResolve( void ) const
    {
        return mpCurrentToResolve;
    }

    // Returns the button handler that owns the given locator
    // Returns NULL if none found
    ActionButton::ActionEventHandler* FindHandler( const ActionEventLocator* locator )const;

    static void AddVehicleSelectInfo( int argc, char** argv );
    static void ClearVehicleSelectInfo( int argc, char** argv );

    virtual void OnProcessRequestsComplete( void* pUserData );

    virtual void HandleEvent( EventEnum id, void* pEventData );

    //HACK HACK HACK
    void LoadingIntoInterior() { mLoadingIntoInterior = true; };


private:
    //Prevent wasteful constructor creation.
    ActionButtonManager( const ActionButtonManager& actionbuttonmanager );
    ActionButtonManager& operator=( const ActionButtonManager& actionbuttonmanager );

    // Singleton.
    //
    static ActionButtonManager* spActionButtonManager;

    struct HandlerData
    {
        HandlerData() : handler( NULL ), sectionName( 0 ) { };
        ActionButton::ButtonHandler* handler;
        tUID sectionName;   //This is for knowing when to release these.  The worldrenderlayer section UID when this is loaded is stored here.
        int id;
    };

    HandlerData mActionButtonList[ MAX_ACTIONS ];
    bool mbActionButtonNeedsUpdate[ MAX_ACTIONS ];

    // Big huge hack here.  But it'll work.
    //
    AnimCollisionEntityDSG* mpCurrentToResolve;

    bool mLoadingIntoInterior : 1;

    int Find(int id) const;
    bool RemoveActionByArrayPos(int i);
};

// A little syntactic sugar for getting at this singleton.
inline ActionButtonManager* GetActionButtonManager() { return( ActionButtonManager::GetInstance() ); }


#endif //ACTIONBUTTONMANAGER_H
