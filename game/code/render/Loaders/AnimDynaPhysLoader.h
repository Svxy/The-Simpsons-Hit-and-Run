//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   AnimDynaPhysLoader
//
// Description: Loader for instanced, animated, dynaphys DSG objects
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef ANIMDYNAPHYSLOADER_H
#define ANIMDYNAPHYSLOADER_H


//===========================================================================
// Nested Includes
//===========================================================================
#include <render/Loaders/IWrappedLoader.h>
#include <memory/map.h>

//===========================================================================
// Forward References
//===========================================================================

class tCompositeDrawableLoader;
class tMultiControllerLoader;
class tSkeletonLoader;
class tFrameControllerLoader;
class tMultiController;
class tAnimationLoader;
class CStatePropDataLoader;
class tAnimatedObjectFactoryLoader;
class tAnimatedObjectLoader;
namespace sim
{
	class CollisionObjectLoader;
	class PhysicsObjectLoader;
	class PhysicsObject;
	class CollisionObject;
};




//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//		Loads an InstAnimDynaPhysDSG chunk, and builds N DSG objects from it.
//
// Constraints:
//	
//
//===========================================================================
class AnimDynaPhysLoader
: public tSimpleChunkHandler,
  public IWrappedLoader 
{
    public:
        AnimDynaPhysLoader();
        virtual ~AnimDynaPhysLoader();
  
		virtual tEntity* LoadObject(tChunkFile* file, tEntityStore* store);

		void SetRegdListener( ChunkListenerCallback* pListenerCB,
							 int   iUserData );

		void ModRegdListener( ChunkListenerCallback* pListenerCB,
							 int   iUserData );

        static void SetShadowElement( const char* compDrawName, 
                                      const char* compDrawElement );

        static void ClearShadowList();
        static tUID GetShadowElement( tUID );

    protected:

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow AnimDynaPhysLoader from being copied and assigned.
        AnimDynaPhysLoader( const AnimDynaPhysLoader& );
        AnimDynaPhysLoader& operator=( const AnimDynaPhysLoader& );

        bool m_IsStateProp;
    
        bool IsMissionProp( const char* name )const;
        static Map< tUID, tUID > s_ShadowList;


		//void LoadAnimWrapper( tChunkFile* file, tEntityStore* store, AnimDynaPhysWrapper* opAnimDynaPhysWrapper );
};
//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//		Loads an InstAnimDynaPhysDSG WRAPPER chunk and returns it from LoadObject
//
// Constraints:
//		Puts all subchunks into the store, as well as itself. CreateSimStateArticulated
//		requires a store search to find physics and collision objects
//
//===========================================================================
class AnimDynaPhysWrapperLoader 
: public tSimpleChunkHandler,
  public IWrappedLoader 
{
public:
    AnimDynaPhysWrapperLoader();
    virtual ~AnimDynaPhysWrapperLoader();
	void SetRegdListener( ChunkListenerCallback* pListenerCB,
						 int   iUserData );

	void ModRegdListener( ChunkListenerCallback* pListenerCB,
						 int   iUserData );

	virtual tEntity* LoadObject( tChunkFile* file, tEntityStore* store );
 
private:
    // These methods defined as private and not implemented ensure that
    // clients will not be able to use them.  For example, we will
    // disallow AnimDynaPhysLoader from being copied and assigned.
    AnimDynaPhysWrapperLoader( const AnimDynaPhysWrapperLoader& );
    AnimDynaPhysWrapperLoader& operator=( const AnimDynaPhysWrapperLoader& );

private:

	sim::CollisionObjectLoader* mpCollObjLoader;
	tCompositeDrawableLoader*	mpCompDLoader;
	tMultiControllerLoader*		mpMCLoader;
	tSkeletonLoader*			mpSkelLoader;
	sim::PhysicsObjectLoader*	mpPhysObjLoader;
	tFrameControllerLoader*		mpFCLoader;
	tAnimationLoader*			mpAnimationLoader;


};
//===========================================================================
//
// Description: 
//		Loads an ANIM_OBJ_DSG_WRAPPER chunk and returns it from LoadObject
//
// Constraints:
//		Puts all subchunks into the store, as well as itself. CreateSimStateArticulated
//		requires a store search to find physics and collision objects
//
//===========================================================================
class AnimObjDSGWrapperLoader 
: public tSimpleChunkHandler,
  public IWrappedLoader 
{
public:
    AnimObjDSGWrapperLoader();
    virtual ~AnimObjDSGWrapperLoader();
	void SetRegdListener( ChunkListenerCallback* pListenerCB,
						 int   iUserData );

	void ModRegdListener( ChunkListenerCallback* pListenerCB,
						 int   iUserData );

	virtual tEntity* LoadObject( tChunkFile* file, tEntityStore* store );
 
private:
    // These methods defined as private and not implemented ensure that
    // clients will not be able to use them.  For example, we will
    // disallow AnimDynaPhysLoader from being copied and assigned.
    AnimObjDSGWrapperLoader( const AnimObjDSGWrapperLoader& );
    AnimObjDSGWrapperLoader& operator=( const AnimObjDSGWrapperLoader& );

private:

	sim::CollisionObjectLoader* mpCollObjLoader;
	tCompositeDrawableLoader*	mpCompDLoader;
	tMultiControllerLoader*		mpMCLoader;
	tSkeletonLoader*			mpSkelLoader;
	sim::PhysicsObjectLoader*	mpPhysObjLoader;
	tFrameControllerLoader*		mpFCLoader;
	tAnimationLoader*			mpAnimationLoader;
    CStatePropDataLoader*       mpStatePropLoader;
    tAnimatedObjectFactoryLoader* mpFactoryLoader;
 	tAnimatedObjectLoader*      mpAnimObjectLoader;



};

#endif