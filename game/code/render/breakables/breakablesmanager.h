//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   breakablesmanager
//
// Description: Singleton class that manages breakable objects in game
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef BREAKABLESMANAGER_H
#define BREAKABLESMANAGER_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <memory\srrmemory.h> // Needed for my STL allocations to go on the right heap
#include <render\DSG\breakableobjectdsg.h>
#include <constants/breakablesenum.h>
#include <render/culling/swaparray.h>
#include <render/culling/reservearray.h>
#include <memory/stlallocators.h>
#include <events/eventlistener.h>
#include <render/enums/renderenums.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

const int BREAKABLE_QUEUE_SIZE = 50;

//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      Breakables manager is necessary because we want to have an number of 
//      breakable objects (DSG objects) in the world in a ready pool. Possibly
//      less than the number of intact objects because it is unlikely the user
//      could smash all the fire hydrants in the world simultaneously. The user
//      could however smash more than one object at once and thus we need to 
//      allocate and manage a pool of breakable objects.
//      
//
// Constraints:
//      Hasn't been memory leak tested yet.
//
//===========================================================================
class BreakablesManager : public EventListener
{
    public:

        // Static Methods (for creating, destroying and acquiring an instance 
        // of the ParticleManager)
        static BreakablesManager* CreateInstance();
        static BreakablesManager* GetInstance();
        static void DestroyInstance();

        // Time in milliseconds
        void Update( unsigned int deltaTime );

        void AllocateBreakables( BreakablesEnum::BreakableID, tAnimatedObjectFactory* pFactory, tAnimatedObjectFrameController* pController, int numInstances );
        void FreeBreakables( BreakablesEnum::BreakableID );    

        // Calls FreeBreakables for every allocated breakable
		// Killing all memory, also calls RemoveAllFromDSG to get rid of any that
		// are still playing
        void FreeAllBreakables();

        // Removes breakables from the specified type from the DSG
        void RemoveFromDSG( BreakablesEnum::BreakableID );

		// Removes every breakable thats in the DSG
		void RemoveAllFromDSG();

        // Trigger a breakable object animation
        void Play( BreakablesEnum::BreakableID, const rmt::Matrix& transform );

		// Flags the given DSG object as breakable object that was broken.
		// It will need to be removed from the world, but not right away.
		// It might still be in use with the collision system
        void RemoveBrokenObjectFromWorld( IEntityDSG*, RenderEnums::LayerEnum layer, bool useRemoveGuts = true );

        void DebugRender()const;
        bool IsLoaded( BreakablesEnum::BreakableID type );
        void AddToZoneList( BreakablesEnum::BreakableID type );

        // The section name associated with breakables
        const char* GetInvSectionName()const { return sInventorySectionName; }
		tUID GetInvSectionUID()const { return mInventorySectionUID; }

        virtual void HandleEvent( EventEnum id, void* pEventData );

    protected:

    private:
        
        static const char* sInventorySectionName;
		const tUID mInventorySectionUID;

        // Its a singleton, prevent access to its constructors
        
        BreakablesManager();
        ~BreakablesManager();
        BreakablesManager( const BreakablesManager& );
        BreakablesManager& operator=( const BreakablesManager& );
        
        struct ManagedBreakable
        {
            ManagedBreakable();
            ~ManagedBreakable();
            RenderEnums::LayerEnum mLayer;

            BreakableObjectDSG* mpBreakableDSG;
            bool mIsActive;
            void AddToDSG();
            void RemoveFromDSG();
        };
        struct BreakableInstances
        {
            BreakableInstances() : list(0), currElement(0), size(0) {}

            ManagedBreakable** list;
            // Acts as a queue, gets the next item in the list and
            // increments next
            ManagedBreakable* Next();
            unsigned int currElement;
            unsigned int size;
        };

        std::vector< BreakableInstances, s2alloc<BreakableInstances> > mBreakablesList;

        // each breakable type can be held in multiple zones
        typedef SwapArray< tName > tNameList;
        SwapArray< tNameList > mZoneList;

		// A list of DSG objects that were broken and need to be removed at the end of the frame
        struct BrokenObject
        {
            IEntityDSG* pDSG;
            RenderEnums::LayerEnum layer;
            bool useRemoveGuts;
        };
        SwapArray< BrokenObject > mBreakableRemoveQueue;

        static BreakablesManager* spInstance;
};


// A little syntactic sugar for getting at this singleton.
inline BreakablesManager* GetBreakablesManager() 
{ 
   return( BreakablesManager::GetInstance() ); 
}
#endif