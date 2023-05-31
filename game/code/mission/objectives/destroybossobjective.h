//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        destroyobjective.h
//
// Description: Blahblahblah
//
// History:     15/04/2002 + Created -- NAME
//
//=============================================================================

#ifndef DESTROYBOSSOBJECTIVE_H
#define DESTROYBOSSOBJECTIVE_H

//========================================
// Nested Includes
//========================================

#include <mission/objectives/missionobjective.h>
#include <render/culling/swaparray.h>

//========================================
// Forward References
//========================================

class Actor;
class StatePropCollectible;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class DestroyBossObjective : public MissionObjective
{
public:
    DestroyBossObjective();
    virtual ~DestroyBossObjective();

    void SetTarget( Actor* actor );
    virtual void HandleEvent( EventEnum id, void* pEventData );


protected:
    virtual void OnInitialize();
    virtual void OnFinalize();
    virtual void OnUpdate( unsigned int elapsedTime );

    Actor* m_Boss;

private:

    //Prevent wasteful constructor creation.
    DestroyBossObjective( const DestroyBossObjective& objective );
    DestroyBossObjective& operator=( const DestroyBossObjective& objective );
};

#endif //DESTROYOBJECTIVE_H
