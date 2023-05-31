//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   AttractionBehaviour
//
// Description: This behaviour has two states. In the first state, it will
//              cause the actor to remain motionless until something of interest
//              happens. Then this happens, the actor will move down to near the avatar
//              and watch him until the avatar does something violent
//              
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef ATTRACTIONBEHAVIOUR_H
#define ATTRACTIONBEHAVIOUR_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <ai/actor/behaviour.h>
#include <radmath/vector.hpp>
#include <events/eventlistener.h>

//===========================================================================
// Forward References
//===========================================================================

class Actor;
class Avatar;


//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================


//===========================================================================
// Interface Definitions
//===========================================================================


class AttractionBehaviour : public Behaviour, public EventListener
{
    public:
        AttractionBehaviour( float minWatchDistance, float maxWatchDistance, float speedKPH );
        virtual ~AttractionBehaviour();
        virtual void Apply( Actor*, unsigned int timeInMS );
        virtual void HandleEvent( EventEnum id, void* pEventData );

        virtual void Activate();
        virtual void Deactivate();

        bool IsMovementDisabled()const { return m_Speed <= 0.0f; }

        enum State 
        {   
            eIdle,
            eWatching
        };

    protected:
        
        State m_CurrentState;
        float m_SensoryRangeSqr;
        float m_MinWatchDistanceSqr;
        float m_MaxWatchDistanceSqr;
        float m_WatchDistanceFromAvatar;
        float m_Speed;
        bool m_ForceFindNewWatchPosition;
        Actor* m_ParentActor;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow AttractionBehaviour from being copied and assigned.
        AttractionBehaviour( const AttractionBehaviour& );
        AttractionBehaviour& operator=( const AttractionBehaviour& );
    private:

        bool WithinSensoryRange( const Actor& actor, const Avatar& avatar )const;
        bool FindNewWatchPosition( const Actor& actor, const rmt::Vector& avatarPosition, rmt::Vector* result )const;
};

#endif