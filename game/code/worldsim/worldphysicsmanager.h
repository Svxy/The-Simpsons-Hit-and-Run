//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        worldphysicsmanager.h
//
// Description: manage the world physics
//
// History:     May 1, 2002 - created, gmayer
//
//=============================================================================

#ifndef WORLDPHYSICSMANAGER_H
#define WORLDPHYSICSMANAGER_H

//========================================
// Nested Includes
//========================================

//========================================
// Forward References
//========================================
class AnimCollisionEntityDSG;
class CollisionEntityDSG;
class DynaPhysDSG;
class WorldCollisionSolverAgentManager;
class StaticPhysDSG;
class GroundPlanePool;
class FenceEntityDSG;
class Vehicle;
namespace sim
{
    class CollisionManager;
    class ManualSimState;
    class SimEnvironment;
    class SimState;
    class PhysicsProperties;
}

//=============================================================================
//
// Synopsis:    this is the key tie-in point to the gameplay context to
//              run the simulation of the game world
//
//=============================================================================
class WorldPhysicsManager
{
    public:

        // Structure thats filled out upon call to ApplyForceToDynamics
        // indicating the counts for the various types of objects that got hit
        struct NumObjectsHit
        {
            NumObjectsHit() : numNPCsHit(0) {}
            int numNPCsHit;
        };

        struct CollisionEntityDSGList
        {
            static const int NUM_COLLISION_LIST_ENTITIES = 3;

            CollisionEntityDSG* collisionEntity[NUM_COLLISION_LIST_ENTITIES];
        };

        // Static Methods for accessing this singleton.
        static WorldPhysicsManager* GetInstance();
        static WorldPhysicsManager* CreateInstance();
        static void DestroyInstance();

        // Important!
        // this method must be called before we load _anything_!
        //static void SetSimUnits();

        void Init();    // TODO - need this?
        
        void Update(unsigned int timeDeltaMilliSeconds);

        // kind of a hack?
        void SuspendForInterior();
        void ResumeForOutside();

        void OnQuitLevel();

        sim::SimEnvironment* mSimEnvironment;
        float mCollisionDistanceCGS;
        rmt::Vector mWorldUp;

        sim::CollisionManager* mCollisionManager;

        WorldCollisionSolverAgentManager* mpWorldCollisionSolverAgentManager;

        int GetCameraCollisionAreaIndex();  // return the next free one
        int GetVehicleCollisionAreaIndex();
        int GetCharacterCollisionAreaIndex();

        void FreeCollisionAreaIndex(int index);
        void FreeAllCollisionAreaIndicies();
        void EmptyCollisionAreaIndex(int index);
        static const int INVALID_COLLISION_AREA = -1;
        void SubmitStaticsPseudoCallback(rmt::Vector& position, float radius, int collisionAreaIndex, sim::SimState* callerSimState, bool allowAutoPairing = false);
        void SubmitFencePiecesPseudoCallback(rmt::Vector& position, float radius, int collisionAreaIndex, sim::SimState* callerSimState, bool allowAutoPairing = false);
        //void SubmitFencePiecesPseudoCallback(rmt::Vector& position, float radius, int collisionAreaIndex);
        void SubmitDynamicsPseudoCallback(rmt::Vector& position, float radius, int collisionAreaIndex, sim::SimState* caller, bool allowAutoPairing = false);
        
        //bool WorldPhysicsManager::IsASubmitter(sim::SimState* testSimState);
        
        void SubmitAnimCollisionsPseudoCallback(rmt::Vector& position, float radius, int collisionAreaIndex, sim::SimState* callerSimState);

        void SubmitAnimCollisionsForUpdateOnly(rmt::Vector& position, float radius, int collisionAreaIndex);

        void UpdateAnimCollisions(float dt, int collisionAreaIndex);
        void UpdateDynamicObjects(float dt, int collisionAreaIndex);


        void DisplayFencesInArea(int area);

        // need to do this when we remove a vehicle from an active collision area index...
        void RemoveFromAnyOtherCurrentDynamicsListAndCollisionArea(DynaPhysDSG* vehicle);

        // allow access to groundplanepool
        int GetNewGroundPlane(sim::SimState* simStateOwner);    // user refers to it with the returned index
        void FreeGroundPlane(int index);

        // add pair to collision
        // called by object
        //void SubmitGroundPlaneToCollisionManager(sim::SimState* simStateToPair, int groundPlaneIndex);
        //void RemoveGroundPlaneFromCollisionManager(int groundPlaneIndex);

        void EnableGroundPlaneCollision(int index);
        void DisableGroundPlaneCollision(int index);
        
        void StartTimer();
        void StopTimer();
        void ResetTimer();

        void ToggleTimerState();

        // Apply the given force to nearby objects. Returns the number of objects affected
        int ApplyForceToDynamics( int collisionAreaIndex, 
                                  const rmt::Vector& position, 
                                  float radius, 
                                  const rmt::Vector& direction, 
                                  float force, 
                                  WorldPhysicsManager::NumObjectsHit* pObjectsHit = NULL,
                                  CollisionEntityDSGList* pCollisionEntityList = NULL );

        int ApplyForceToDynamicsSpherical( int collisionAreaIndex,
                                           const rmt::Vector& position, 
                                           float radius, 
                                           float force,
                                           CollisionEntityDSGList* pCollisionEntityList = NULL );
        float GetLoopTime();

        //--------
        // statics
        //--------
        struct StaticsInCollisionDetection
        {
            StaticPhysDSG* mStaticPhysDSG;
            bool clean;
        };
        int mMaxStatics;   // per area           

        // for each collision area, need the 'clean' list
        StaticsInCollisionDetection** mCurrentStatics;

        void PurgeDynamics( int collisionAreaIndex );

        bool FenceSanityCheck(unsigned collisionAreaIndex, const rmt::Vector lastFrame, const rmt::Vector thisFrame, rmt::Vector* fixPos);

    private:
    
        // No public access to these, use singleton interface.
        WorldPhysicsManager();
        ~WorldPhysicsManager();

        void InitCollisionManager();


        void WorldSimSubstepGuts(float dt, bool firstSubstep);

        // pointer to the single instance
        static WorldPhysicsManager* spInstance;
    
        float mTotalTime;
        float mTimerTime;
        bool mTimerOn;


        void DebugInfoDisplay();

        //----------------------
        // anim collision entity
        //----------------------

        // these are for collision detection only
        struct AnimCollisionInCollisionDetection
        {
            AnimCollisionEntityDSG* mAnimCollisionEntityDSG;
            bool clean;
        };
        int mMaxAnimCollisions;

        AnimCollisionInCollisionDetection** mCurrentAnimCollisions;

        // these ones are for update only - we can and want to update stuff that is too far away to collide with, for appearance sake
        AnimCollisionInCollisionDetection** mCurrentUpdateAnimCollisions;
        int mMaxUpdateAnimCollisions;

        //---------
        // dynamics
        //---------
        struct DynamicsInCollisionDetection
        {
            //InstDynaPhysDSG* mDynamicPhysDSG;
            DynaPhysDSG* mDynamicPhysDSG;
            bool clean;
            //int groundPlaneIndex;     have to hold this in the InstDynaPhysDSG class since object could be in more than one update list
        };
        int mMaxDynamics;   // per area           

        // the 'clean' list
        //
        DynamicsInCollisionDetection** mCurrentDynamics;
        
        // this list will hold all dynamics that should be tested for collision against.
        // some will be under ai ctrl, and one's that have been collided with will be under
        // simulation ctrl
        //
        // only the simulation ctrl ones need a physics update and ground plane and a rest test
        //
        // for now... don't remove from the list until object has come to rest
     
        GroundPlanePool* mGroundPlanePool;
        void UpdateSimluatingDynamicObjectGroundPlanes();
    
        bool* mCollisionAreaAllocated;      // has been requested for use by a vehicle, character, or camera
        bool* mCollisionAreaActive;     // for intermediate, temporary disabling
        int mNumCollisionAreas;
        

        // for debugging?
        int mReservedCollisionAreas;    // reserve 0...mReservedCollisionAreas - 1

        int mMaxVehicles;
        int mMaxChars;
        int mMaxCameras;

        //-------------
        // fence pieces
        //-------------

        sim::PhysicsProperties* mFencePhysicsProperties;
        
        // have a pool of these

        struct FencePieces
        {
            sim::ManualSimState* mFenceSimState;
            bool mInCollision;
            bool mClean;
            rmt::Vector start, end;
        };
    
        FencePieces** mFences;

        // debug drawing:
        // note this only really works for one thing calling submit fence pieces!
        int mNumDebugFences;
        FenceEntityDSG** mFenceDSGResults;
        

        //sim::ManualSimState*** mFenceSimStates;   // a 2D array of pointers

        int mMaxFencePerArea;
        int* mFencesInEachArea; // ? still need this
        
        bool UpdateFencePiece(rmt::Vector& callerPosition, sim::ManualSimState* fencePiece, rmt::Vector& end0, rmt::Vector& end1, rmt::Vector& normal, sim::SimState* callerSimState);

        
        // ? bit of a hack here for player character going into interiors
        bool mInInterior;

        // current physics "frame" (for detecting duplicate updating 
        // when an object is in multip[le lists
        unsigned updateFrame;

        float mLoopTime;
        unsigned mLastTime;
};

// A little syntactic sugar for getting at this singleton.
inline WorldPhysicsManager* GetWorldPhysicsManager() { return( WorldPhysicsManager::GetInstance() ); }


         
#endif //WORLDPHYSICSMANAGER_H
