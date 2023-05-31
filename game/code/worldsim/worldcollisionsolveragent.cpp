//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implementation of class WorldCollisionSolverAgentManager
//
// History:     6/14/2002 + Created -- NAME
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
#include <worldsim/worldcollisionsolveragent.h>
#include <worldsim/redbrick/redbrickcollisionsolveragent.h>
#include <worldsim/physicsairef.h>
#include <memory/srrmemory.h>

#include <camera/supercamcentral.h>

#include <render/DSG/collisionentitydsg.h>

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
// WorldCollisionSolverAgentManager::WorldCollisionSolverAgentManager
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
WorldCollisionSolverAgentManager::WorldCollisionSolverAgentManager()
{
    mpCollisionSolverAgentArray[ 0 ] = new(GMA_PERSISTENT) RedBrickCollisionSolverAgent;  
    mpCollisionSolverAgentArray[ 0 ]->AddRef();
}

//==============================================================================
// WorldCollisionSolverAgentManager::~WorldCollisionSolverAgentManager
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
WorldCollisionSolverAgentManager::~WorldCollisionSolverAgentManager()
{
    int i;
    for ( i = 0; i < NUM_SOLVERS; i++ )
    {
        if ( mpCollisionSolverAgentArray[ i ] )
        {
            mpCollisionSolverAgentArray[ i ]->Release();
            mpCollisionSolverAgentArray[ i ] = 0;
        }
    }
}




//=============================================================================
// WorldCollisionSolverAgentManager::CollisionEvent
//=============================================================================
// Description: Comment
//
// Parameters:  (SimState* inSimStateA, int indexA, SimState* inSimStateB, int indexB, const rmt::Vector& inPos, float inDvN, float inDvT, SimulatedObject** simA, SimulatedObject** simB)
//
// Return:      Solving_Answer 
//
//=============================================================================
Solving_Answer WorldCollisionSolverAgentManager::CollisionEvent(SimState* inSimStateA, int indexA, SimState* inSimStateB, int indexB, const rmt::Vector& inPos, float inDvN, float inDvT, SimulatedObject** simA, SimulatedObject** simB)
{
    return Solving_Continue;
}


/*
==============================================================================
WorldCollisionSolverAgentManager::PreCollisionEvent
==============================================================================
Description:    Comment

Parameters:     (Collision& inCollision, int inPass)

Return:         Solving_Answer 

=============================================================================
*/
Solving_Answer WorldCollisionSolverAgentManager::PreCollisionEvent(Collision& inCollision, int inPass)
{
    CollisionObject* collObjA = inCollision.mCollisionObjectA;
    CollisionObject* collObjB = inCollision.mCollisionObjectB;
    
    SimState* simStateA = collObjA->GetSimState();
    SimState* simStateB = collObjB->GetSimState();


    if(inCollision.mCollisionVolumeA->Type() == sim::BBoxVolumeType || inCollision.mCollisionVolumeB->Type() == sim::BBoxVolumeType)
    {
        return Solving_Aborted;
    }




    int i;
    for ( i = 0; i < NUM_SOLVERS; i++ )
    {
        rAssert( mpCollisionSolverAgentArray[ i ] != (CollisionSolverAgent*)0 );
        if ( Solving_Aborted == mpCollisionSolverAgentArray[ i ]->PreCollisionEvent( inCollision, inPass ) )
        {
            return Solving_Aborted;
        }
    }


    
    // this is a bit messy, but I don't want to have to derive supercamcentral from CollisionEntityDSG
    if(simStateA && simStateA->mAIRefIndex == PhysicsAIRef::CameraSphere)
    {    
        rmt::Vector fixOffset = inCollision.mNormal;
        float dist = inCollision.mDistance;

        if(dist < 0.0f)
        {
            dist *= -1.0f;
        }

        fixOffset.Scale(dist);

        SuperCamCentral* scc = (SuperCamCentral*)(simStateA->mAIRefPointer);
        scc->AddCameraCollisionOffset(fixOffset);

        return Solving_Aborted;

    }
    if(simStateB && simStateB->mAIRefIndex == PhysicsAIRef::CameraSphere)
    {
        // normal always points B to A
        // distance -ve means interpenetration

        rmt::Vector fixOffset = inCollision.mNormal;


        float dist = inCollision.mDistance;

        if(dist < 0.0f)
        {
            dist *= -1.0f;
        }

        fixOffset.Scale(-1.0f * dist);

        SuperCamCentral* scc = (SuperCamCentral*)(simStateB->mAIRefPointer);
        scc->AddCameraCollisionOffset(fixOffset);

        return Solving_Aborted;

    }



    /*
    if( simStateA && simStateB && ( simStateA->mAIRefIndex == PhysicsAIRef::redBrickPhizMoveableGroundPlane ||
                                    simStateB->mAIRefIndex == PhysicsAIRef::redBrickPhizMoveableGroundPlane) )
    {
        //char buffy[128];
        rReleaseString("something collided with a moveableobject groundplane\n");

    }
    */

    CollisionEntityDSG* pObjA = static_cast<CollisionEntityDSG*>( simStateA->mAIRefPointer );
    CollisionEntityDSG* pObjB = static_cast<CollisionEntityDSG*>( simStateB->mAIRefPointer );

    //bool bSolveA = false;
    //bool bSolveB = false;
    Solving_Answer answerA = Solving_Continue;
    Solving_Answer answerB = Solving_Continue;

    if ( pObjA )
    {
        //bSolveA = pObjA->ReactToCollision( simStateB, inCollision );
        answerA = pObjA->PreReactToCollision( simStateB, inCollision );
    }
    if ( pObjB )
    {
        //bSolveB = pObjB->ReactToCollision( simStateA, inCollision );
        answerB = pObjB->PreReactToCollision( simStateA, inCollision );
    }
    //if ( bSolveA || bSolveB )
    if(answerA == Solving_Aborted || answerB == Solving_Aborted)
    {
        return Solving_Aborted;
    }
    return CollisionSolverAgent::PreCollisionEvent(inCollision, inPass);
}

/*
==============================================================================
WorldCollisionSolverAgentManager::TestImpulse
==============================================================================
Description:    Comment

Parameters:     (rmt::Vector& mImpulse, Collision& inCollision)

Return:         Solving_Answer 

=============================================================================
*/
Solving_Answer WorldCollisionSolverAgentManager::TestImpulse(rmt::Vector& impulse, Collision& inCollision)
{
    int i;

    // vehicle is special and will be dealt with in the redbrick collision solver agent

    // other ordinary shit will get the PostReactToCollision call here.

    for ( i = 0; i < NUM_SOLVERS; i++ )
    {
        rAssert( mpCollisionSolverAgentArray[ i ] != (CollisionSolverAgent*)0 );
        if ( Solving_Aborted == mpCollisionSolverAgentArray[ i ]->TestImpulse( impulse, inCollision ) )
        {
            return Solving_Aborted;
        }
    }

    CollisionObject* collObjA = inCollision.mCollisionObjectA;
    CollisionObject* collObjB = inCollision.mCollisionObjectB;

    SimState* simStateA = collObjA->GetSimState();
    SimState* simStateB = collObjB->GetSimState();

    CollisionEntityDSG* pObjA = static_cast<CollisionEntityDSG*>( simStateA->mAIRefPointer );
    CollisionEntityDSG* pObjB = static_cast<CollisionEntityDSG*>( simStateB->mAIRefPointer );

    Solving_Answer answerA = Solving_Continue;
    Solving_Answer answerB = Solving_Continue;

    if(pObjA)
    {
        answerA = pObjA->PostReactToCollision(impulse, inCollision);
    }
    if(pObjB)
    {
        answerB = pObjB->PostReactToCollision(impulse, inCollision);        
    }
    
    if(answerA == Solving_Aborted || answerB == Solving_Aborted)
    {
        return Solving_Aborted;
    }

    /*
    if( simStateA && simStateB && ( simStateA->mAIRefIndex == PhysicsAIRef::redBrickPhizMoveableGroundPlane ||
                                    simStateB->mAIRefIndex == PhysicsAIRef::redBrickPhizMoveableGroundPlane) )
    {
        //char buffy[128];
        rReleaseString("something is testing impulse with a moveableobject groundplane\n");

    }
    */


    return CollisionSolverAgent::TestImpulse(impulse, inCollision);

    
}

/*
==============================================================================
WorldCollisionSolverAgentManager::TestCache
==============================================================================
Description:    Comment

Parameters:     (SimState* inSimState, int inIndex)

Return:         Solving_Answer 

=============================================================================
*/
Solving_Answer WorldCollisionSolverAgentManager::TestCache(SimState* inSimState, int inIndex)
{
    int i;
    for ( i = 0; i < NUM_SOLVERS; i++ )
    {
        rAssert( mpCollisionSolverAgentArray[ i ] != (CollisionSolverAgent*)0 );
        if ( Solving_Aborted == mpCollisionSolverAgentArray[ i ]->TestCache( inSimState, inIndex ) )
        {
            return Solving_Aborted;
        }
    }
    return Solving_Continue;
}




Solving_Answer WorldCollisionSolverAgentManager::EndObjectCollision(SimState* inSimState, int inIndex)
{

    int i;
    for ( i = 0; i < NUM_SOLVERS; i++ )
    {
        rAssert( mpCollisionSolverAgentArray[ i ] != (CollisionSolverAgent*)0 );
        if ( Solving_Aborted == mpCollisionSolverAgentArray[ i ]->EndObjectCollision(inSimState, inIndex) )
        {
            return Solving_Aborted;
        }
    }
    return Solving_Continue;

}



/*
==============================================================================
WorldCollisionSolverAgentManager::ResetCollisionFlags
==============================================================================
Description:    Comment

Parameters:     ()

Return:         void 

=============================================================================
*/
void WorldCollisionSolverAgentManager::ResetCollisionFlags()
{
    int i;
    for ( i = 0; i < NUM_SOLVERS; i++ )
    {
        rAssert( mpCollisionSolverAgentArray[ i ] != (CollisionSolverAgent*)0 );
        mpCollisionSolverAgentArray[ i ]->ResetCollisionFlags();
    }
}
//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
