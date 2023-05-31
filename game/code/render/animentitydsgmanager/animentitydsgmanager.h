//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   animentitydsgmanager
//
// Description: Single place for updating all loaded animentitydsg's in the world
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef ANIMENTITYDSGMANAGER_H
#define ANIMENTITYDSGMANAGER_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <events/eventlistener.h>
#include <meta/locatorevents.h>
#include <render/Culling/swaparray.h>

//===========================================================================
// Forward References
//===========================================================================

class tMultiController;
class AnimEntityDSG;
class AnimCollisionEntityDSG;
class StatePropDSG;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================


//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//		Manages animation control for AnimEntityDSGs as well as 
//		InstAnimDynaPhysDSGs. Handles animations that are triggered by
//		trigger volumes as well (hence the EventListener inheritence)
//		
//
// Constraints:
//		
//
//===========================================================================


class AnimEntityDSGManager : public EventListener
{
    public:

        // Static Methods (for creating, destroying and acquiring an instance 
        // of the AnimEntityDSGManager)
        static AnimEntityDSGManager* CreateInstance();
        static AnimEntityDSGManager* GetInstance();
        static void DestroyInstance();

        void Add( AnimCollisionEntityDSG* );
		void Add( AnimEntityDSG* );
        void Add( StatePropDSG* );
		// Add a multicontroller to the list of controllers that will get updated every frame
		void Add( tMultiController* );
		// Remove a multicontroller from the list of controllers
		void Remove( tMultiController* );
		void Remove( AnimEntityDSG* );
        void Remove( AnimCollisionEntityDSG* );
        void Remove( StatePropDSG* );

		void RemoveAll();
		void Update( unsigned int elapsedTime );

		// Inherited from EventListener
		// Used to detect when the character walks into a trigger volume
        virtual void HandleEvent( EventEnum id, void* pEventData );

        SwapArray<AnimEntityDSG*> mpFloatingRightWayArrows;
        SwapArray<AnimEntityDSG*> mpFloatingWrongWayArrows;

    protected:

    private:
	
		SwapArray< AnimEntityDSG* > mEntityList;
        SwapArray< AnimCollisionEntityDSG* > mCollEntityList;
		SwapArray< tMultiController* > mMultiControllerlist;
        SwapArray< StatePropDSG* > mStatePropList;

		// Singleton, prevent access to ctors

		AnimEntityDSGManager();
        virtual ~AnimEntityDSGManager();

		AnimEntityDSGManager( const AnimEntityDSGManager& );
        AnimEntityDSGManager& operator=( const AnimEntityDSGManager& );

        static AnimEntityDSGManager* spInstance;
};


//===========================================================================
// Inlines
//===========================================================================


// A little syntactic sugar for getting at this singleton.
inline AnimEntityDSGManager* GetAnimEntityDSGManager() 
{ 
   return( AnimEntityDSGManager::GetInstance() ); 
}

#endif