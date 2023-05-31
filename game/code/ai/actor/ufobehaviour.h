//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ufobeamalwayson
//
// Description: Generic ufo behaviours for tractor beam, target searching, etc
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef UFOBEHAVIOUR_H
#define UFOBEHAVIOUR_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <ai\actor\behaviour.h>
#include <render\DSG\DynaPhysDSG.h>
#include <memory\map.h>

//===========================================================================
// Forward References
//===========================================================================

class EventLocator;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      Actor never moves. Beam is on
//
// Constraints:
//
//
//===========================================================================
class UFOBehaviour : public Behaviour
{
    public:
        UFOBehaviour();
        virtual ~UFOBehaviour()=0;
        virtual void Apply( Actor*, unsigned int timeInMS );
            
        void SetCharacterRespawnPosition( EventLocator* locator );


    protected:

        bool WithinTractorBeamRange( const rmt::Vector& position, IEntityDSG* dsg )const;
        // Suck an object into the UFO. returns true if object is successfully sucked
        bool SuckIntoUFO( const rmt::Vector& actorPosition, DynaPhysDSG*, float timeInMS );
        bool ReachedTopOfTractorBeam( const rmt::Vector& actorPosition, DynaPhysDSG* );
        // Destroy an object in the tractor beam, returns true if the object damaged the UFO
        bool DestroyObject( DynaPhysDSG* );
        void GetTargetsInTractorRange( rmt::Vector& actorPosition, float maxRange, SwapArray< DynaPhysDSG* >* targetList );

        // Used for getting an axis of rotation to spin 
        // an object thats being sucked up
        rmt::Vector GetPseudoRandomRotationAxis( int hash )const;

    protected:
    
        EventLocator* m_RespawnLocator;
        float m_Rotation;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow UFOBehaviour from being copied and assigned.
        UFOBehaviour( const UFOBehaviour& );
        UFOBehaviour& operator=( const UFOBehaviour& );
};


#endif