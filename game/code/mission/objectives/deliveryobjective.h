//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        deliveryobjective.h
//
// Description: Blahblahblah
//
// History:     15/04/2002 + Created -- NAME
//
//=============================================================================

#ifndef DELIVERYOBJECTIVE_H
#define DELIVERYOBJECTIVE_H

//========================================
// Nested Includes
//========================================

#include <mission/objectives/collectibleobjective.h>
#include <render/culling/swaparray.h>

//========================================
// Forward References
//========================================

class StatePropDSG;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class DeliveryObjective : public CollectibleObjective
{
public:
    DeliveryObjective();
    virtual ~DeliveryObjective();
    
    //From EventListener
    virtual void HandleEvent( EventEnum id, void* pEventData );

protected:
    virtual void OnInitCollectibles();
    virtual void OnInitCollectibleObjective();
    virtual void OnFinalizeCollectibleObjective();
    virtual bool OnCollection( unsigned int collectibleNum, bool &shouldReset );
    virtual void OnUpdateCollectibleObjective( unsigned int elapsedTime );

    void FindStateProps();
    // Iterates through the mStateProp array and decrements their refcounts
    // Kills mStateProp
    void ReleaseAllStateProps();

    // This iterates through the collectibles and turns their associated
    // ActionEventHandlers enabled flag on or off
    // This is used to have the HUD icon only visible when the mission is in play
    void SetButtonHandlersEnabled( bool enable );

    // Iterates through the collectibles, any stateprops it finds associated
    // with the collectible locators gets set to the given
    // state. This Allows the stateprops to appear only when the mission is active
    // Note that Aryan set it up so that state 0 is disabled (invisible)
    // And state 1+ is enabled (visible)
    void SetStatePropState( int state );

    // Iterates through the stateprops (if any) and enables/disables HUD icons
    void SetStatePropHUDIconEnable( bool enable );

    SwapArray< StatePropDSG* > mStateProps;

private:
    unsigned int mDumpTimeout;
};



#endif //DELIVERYOBJECTIVE_H
