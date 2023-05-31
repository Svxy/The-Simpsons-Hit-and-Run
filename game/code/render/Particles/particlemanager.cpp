//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ParticleManager
//
// Description: The ParticleManager class is a singleton that encapsulates
//              all particle effects in the game. Currently this means
//              Breakable Objects, and InstParticleSystems
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <render/particles/particlemanager.h>
#include <memory/srrmemory.h>   // We are putting this class on the GMA_PERSISTENT heap
#include <algorithm>            // For std algorithms that operate on our std::list of systems
#include <render/RenderManager/RenderLayer.h>
#include <render/RenderManager/RenderManager.h>
#include <render/RenderManager/WorldRenderLayer.h>
#include <render/Culling/WorldScene.h>
#include <memory/srrmemory.h>
#include <p3d/anim/animatedobject.hpp>
#include <p3d/effects/effect.hpp>
#include <p3d/effects/particlesystem.hpp>
#include <worldsim/coins/sparkle.h>
#include <constants/particleenum.h>
//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

// Static instance of our singleton pointer.
ParticleManager* ParticleManager::spInstance = NULL;

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Member Functions
//===========================================================================


//==============================================================================
// ParticleManager::ManagedParticleSystem::ManagedParticleSystem
//==============================================================================
//
// Description: ManagedParticleSystem ctor (instanced particle systems)
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: Must be given valid pFactory and pController pointers
//
//==============================================================================
ParticleManager::ManagedParticleSystem::ManagedParticleSystem( tParticleSystemFactory* pFactory, tEffectController* pController ) 
: mUserID( -1 ),
mIsActive( false ),
mEmissionBias( 1.0f ),
mThrowUpNewParticles( false ),
mIsInDSG( false )
{   
    mpSystem = new ParticleSystemDSG;
	mpSystem->AddRef();

    rAssert( mpSystem != NULL );
    mpSystem->Init( pFactory, pController ); 
}
//==============================================================================
// ParticleManager::ParticleSystemEntityDSG::~ParticleSystemEntityDSG
//==============================================================================
//
// Description: ParticleSystemEntityDSG dtor
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: None.
//
//==============================================================================
ParticleManager::ManagedParticleSystem::~ManagedParticleSystem()
{

	mpSystem->Release();
    mpSystem = NULL;
}
//==============================================================================
// ParticleManager::ManagedParticleSystem::Update
//==============================================================================
//
// Description: Updates animation based upon given time delta
//
// Parameters:	Time delta in milliseconds
//
// Return:      None.
//
// Constraints: None.
//
//==============================================================================
void ParticleManager::ManagedParticleSystem::Update( float deltaTime)
{
    
    if ( IsLocked() )
    {
        if ( mThrowUpNewParticles )
        {
            mThrowUpNewParticles = false;        
        }
        else
        {
            SetBias( p3dParticleSystemConstants::EmitterBias::EMISSION, 0.0f);

            if ( mpSystem->GetNumLiveParticles() == 0 )
            {
                Unlock();    
                SetActive( false );
            }
        }
    }
    mpSystem->Update ( deltaTime );
}
//==============================================================================
// ParticleManager::ManagedParticleSystem::Reset
//==============================================================================
//
// Description: Resets animation to the start
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: None.
//
//==============================================================================
void ParticleManager::ManagedParticleSystem::Reset()
{
    mpSystem->Reset ();
}




//==============================================================================
// ParticleManager::ParticleSystemEntityDSG::SetTransform
//==============================================================================
//
// Description: Sets the transform matrix (local->world)
//
// Parameters:	Transformation matrix that contains the world position/orientation of the system
//
// Return:      None.
//
// Constraints: None.
//
//==============================================================================
void ParticleManager::ManagedParticleSystem::SetTransform( const rmt::Matrix& transform )
{
	// Check to see if the object has moved, if it has, and its in the DSG
	// call DSGtree->Move
    rmt::Vector oldPos = mpSystem->rPosition();
	rmt::Vector newPos = transform.Row(3);
	if ( mIsInDSG && newPos != oldPos )
	{
		rmt::Box3D oldBB;	
		mpSystem->GetBoundingBox( &oldBB );
	    mpSystem->SetTransform( transform );   

        WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( mLayer ));
        // Sanity check
        rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );
        pWorldRenderLayer->pWorldScene()->Move( oldBB, mpSystem );   

    }
	else
	{
	    mpSystem->SetTransform( transform );   
	}
}
//==============================================================================
// ParticleManager::ParticleSystemEntityDSG::LastFrameReached
//==============================================================================
//
// Description: Returns how many times the animation has been played through.
//              Useful for finding out if the animation has reached the end of its lifespan
//              and should be deactivated.
//
// Parameters:	None.
//
// Return:      int, 0 for still playing, 1 for played once through, N for played N times.
//
// Constraints: None.
//
//==============================================================================
int ParticleManager::ManagedParticleSystem::LastFrameReached() const
{
    return mpSystem->LastFrameReached();       
}
      

//==============================================================================
// ParticleManager::CreateInstance
//==============================================================================
//
// Description: Creates the ParticleManager.
//
// Parameters:	None.
//
// Return:      Pointer to the ParticleManager.
//
// Constraints: Multiple calls to CreateInstance without a DestroyInstance call in between
//              will result in an assertion (or lost memory if assertions not enabled)
//
//==============================================================================
ParticleManager* ParticleManager::CreateInstance()
{
    rAssert( spInstance == NULL );

    spInstance = new(GMA_PERSISTENT) ParticleManager;
    rAssert( spInstance != NULL );
    
    return spInstance;
}
//==============================================================================
// ParticleManager::GetInstance
//==============================================================================
//
// Description: Returns a ParticleManager singleton object.
//
// Parameters:	None.
//
// Return:      Pointer to a ParticleManager object.
//
// Constraints: Assertion failure if CreateInstance was not called first.
//
//
//==============================================================================
ParticleManager* ParticleManager::GetInstance()
{
    if ( spInstance == NULL )
    {
        CreateInstance();
    }
    rAssert ( spInstance != NULL);

    return spInstance;
}
//==============================================================================
// ParticleManager::DestroyInstance
//==============================================================================
//
// Description: Frees the ParticleManager singleton.
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: Assertion failure if CreateInstance was not called first.
//
//
//==============================================================================
void ParticleManager::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete( GMA_PERSISTENT, spInstance );
    spInstance = NULL;
}


//==============================================================================
// ParticleManager::DeactiveateAll
//==============================================================================
//
// Description: Removes all active particles from the DSG
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: 
//
//
//==============================================================================

void ParticleManager::DeactiveateAll()
{
	// for each active particle system type
    for (unsigned int i = 0 ; i < mActiveSystems.Size() ; ++i)
    {
        for (unsigned int j = 0 ; j < mActiveSystems[i].Size() ; ++j)
        {
            // Advance controller based upon time elapsed but only if it is active
            if ( mActiveSystems[i][j]->IsActive() )
            {
				mActiveSystems[i][j]->SetActive( false );    
            }
        }
    }
}


//==============================================================================
// ParticleManager::ParticleManager
//==============================================================================
//
// Description: ParticleManager constructor.
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: 
//
//
//==============================================================================
ParticleManager::ParticleManager()
{
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_PERSISTENT );
    #endif
    // Allocate the (std:vector) arrays that will be filled out using the InitializeSystem function
    mActiveSystems.Grow( ParticleEnum::eNumParticleTypes );
    mIsParticleTypeDynamicallyLoaded.resize( ParticleEnum::eNumParticleTypes, false );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_PERSISTENT );
    #endif
}
//==============================================================================
// ParticleManager::~ParticleManager
//==============================================================================
//
// Description: ParticleManager destructor, frees all particle systems via ClearSystems()
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: 
//
//
//==============================================================================
ParticleManager::~ParticleManager()
{

    // Clear out active particle systems
    ClearSystems();
}


//==============================================================================
// ParticleManager::Clear
//==============================================================================
//
// Description: Frees all particle systems that were added to the manager. 
//              GetNumParticleSystems() will return zero after this call.
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: Removal of the system from the scene graph not implemented yet!
//
//
//==============================================================================
void ParticleManager::ClearSystems()
{
    // for each active particle system
    //  remove it from the scene graph
	DeactiveateAll();
    //  free it
    for (unsigned int i = 0 ; i < mActiveSystems.Size() ; ++i)
    {   
        for (unsigned int j = 0 ; j < mActiveSystems[i].Size () ; ++j)
        {
            delete mActiveSystems[i][j];
        }
		mActiveSystems[i].Shrink(0);
    }
}

//==============================================================================
// ParticleManager::InitializeSystem
//==============================================================================
//
// Description: Creates maxInstances of InstParticleSystems objects via cloning the given factory
//              and frame controllers
//
// Parameters:	
//              ParticleEnum::ParticleID, type of particle to create
//              tParticleSystemFactory*, typically from a instparticlesystem object chunk file
//              tEffectController*, typically from a instparticlesystem object chunk file
//              int maxInstances, number of instanes to instantiate
//
// Return:      None.
//
// Constraints: InitializeSystem cannot be called twice for the same type
//
//
//==============================================================================
void ParticleManager::InitializeSystem( ParticleEnum::ParticleID type, 
                                        tParticleSystemFactory* pFactory, 
                                        tEffectController* pController, 
                                        int maxInstances,
                                        bool isDynamic )
{
    rAssert( type >=0 && type < ParticleEnum::eNumParticleTypes );
    rAssert( pFactory != NULL );
    rAssert( pController != NULL );
    rAssert( maxInstances > 0 );

    // Remember we are holding pointers, if we call resize, any valid pointers that are killed
    // will not be freed properly, assert to make sure this never happens
    rAssert( mActiveSystems[ type ].Size() == 0 );
    

    MEMTRACK_PUSH_GROUP( "InitParticleSystem" );

    //This should be looked at.  TODO
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    #endif
    mActiveSystems[ type ].Resize( maxInstances );


    for (int i = 0 ; i < maxInstances ; ++i)
    {
        mActiveSystems[ type ][ i ] = new ManagedParticleSystem( pFactory, pController);
    }
    mIsParticleTypeDynamicallyLoaded[ type ] = isDynamic;
    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
    #endif
    MEMTRACK_POP_GROUP( "InitParticleSystem" );


}

//==============================================================================
// ParticleManager::DeleteSystem
//==============================================================================
//
// Description: Kills all particles of the given type
//
// Parameters:	The ParticleEnum type.
//
// Return:      None.
//
// Constraints: None.
//
//
//==============================================================================
void ParticleManager::DeleteSystem( ParticleEnum::ParticleID type )
{
    rAssert( type >=0 && type < ParticleEnum::eNumParticleTypes );
    for (unsigned int i = 0 ; i < mActiveSystems[ type ].Size() ; ++i)
    {

		// Remove it from the DSG
		mActiveSystems[ type ][ i ]->SetActive( false );
        delete mActiveSystems[ type ][ i ];
    }
    mActiveSystems[ type ].Shrink(0);
}
//==============================================================================
// ParticleManager::DumpDynaLoad
//==============================================================================
//
// Description: Kills all the particles of any type that was 
//              Initialized with isDynamic to true
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: None.
//
//
//==============================================================================

void ParticleManager::DumpDynaLoad()
{
    for (int i = 0 ; i < ParticleEnum::eNumParticleTypes ; ++i)
    {
        if ( mIsParticleTypeDynamicallyLoaded[ i ] )
        {
            DeleteSystem( ParticleEnum::ParticleID( i ) );
            mIsParticleTypeDynamicallyLoaded[ i ] = false;
        }
    }
}

//==============================================================================
// ParticleManager::GetUniqueID
//==============================================================================
//
// Description: Returns an identifier that can be used to reference
//              a continuously playing particle effect
//
// Parameters:	None.
//
// Return:      ParticlePlayerID, different each time function is called
//
// Constraints: None.
//
//
//==============================================================================

ParticlePlayerID ParticleManager::GetUniqueID()const
{
    static int sUniqueID = 0;
    return sUniqueID++;
}

//==============================================================================
// ParticleManager::PlayCyclic
//==============================================================================
//
// Description: Tells the PM to play a cycling particle system. Note that this function
//              must be called every frame to have the emitter keep adding particles to
//              it. Otherwise, EMISSION bias is set to zero. Alive particles will gradually
//              fall to earth and disappear.
//
// Parameters:	ParticlePlayerID - identifying a particle system that is currently playing
//              or assigning a new one if never played before
//
//              ParticleAttributes structure. Identifying particle type and attributes.
//              
//              rmt::Matrix, with local->world transform matrix
//  
// Return:      None.
//
// Constraints: None.
//
//
//==============================================================================


void ParticleManager::PlayCyclic( ParticlePlayerID id, const ParticleAttributes& attr, const rmt::Matrix& localMatrix )
{
    rAssert( attr.mType >=0 && attr.mType < ParticleEnum::eNumParticleTypes );
    // get animation associated with UniqueID, or assign an unused one.

    if( attr.mType == ParticleEnum::eStars )
    {
        return;
    }

    rmt::Matrix orientedLocalMatrix;
    ReorientUpAxis( localMatrix, &orientedLocalMatrix );

    ManagedParticleSystem* pFreeSystem = NULL;
    bool wasFreeSystemFound = false;

    rWarningMsg( mActiveSystems[ attr.mType ].Size() > 0 , "ParticleManager::PlayCyclic, particles of that type have not been loaded!" );

    for ( unsigned int i = 0 ; i < mActiveSystems[ attr.mType ].Size() ; i++ )
    {
        ManagedParticleSystem* currentSystem = mActiveSystems[ attr.mType ][ i ];  
        if ( id == currentSystem->GetUserID() )
        {
            // we have found the system we are looking for
            // tell it to keep playing
            currentSystem->ThrowUpNewParticles();
            currentSystem->SetTransform( orientedLocalMatrix );
            currentSystem->SetBias( p3dParticleSystemConstants::EmitterBias::EMISSION, attr.mEmissionBias );
            currentSystem->SetVelocity( attr.mVelocity );
            return;
        }
        else if ( wasFreeSystemFound == false &&
                  currentSystem->IsActive() == false &&
                  currentSystem->GetUserID() < 0 )
        {
            // this thing is unused. Lets flag it so that we can use it for 
            // a locked player if needed
            wasFreeSystemFound = true;
            pFreeSystem = currentSystem;
        }
    }
    // The system was not assigned yet. Assign it now.
    if ( pFreeSystem != NULL )
    {
        pFreeSystem->SetTransform( orientedLocalMatrix );
        pFreeSystem->SetActive( true );
        pFreeSystem->Lock( id );
        pFreeSystem->ThrowUpNewParticles();
        pFreeSystem->SetVelocity( attr.mVelocity );
        pFreeSystem->SetBias( p3dParticleSystemConstants::EmitterBias::EMISSION, attr.mEmissionBias );
    }
}

//==============================================================================
// ParticleManager::PlayCyclic
//==============================================================================
//
// Description: Tells the PM to play a cycling particle system. Note that this function
//              must be called every frame to have the emitter keep adding particles to
//              it. Otherwise, EMISSION bias is set to zero. Alive particles will gradually
//              fall to earth and disappear.
//
// Parameters:	ParticlePlayerID - identifying a particle system that is currently playing
//              or assigning a new one if never played before
//
//              ParticleAttributes structure. Identifying particle type and attributes.
//              
//              rmt::Vector, with world position
//  
// Return:      None.
//
// Constraints: None.
//
//
//==============================================================================


void ParticleManager::PlayCyclic( ParticlePlayerID id, const ParticleAttributes& attr, const rmt::Vector& position )
{
    rmt::Matrix localMatrix;
    localMatrix.Identity();
    localMatrix.FillTranslate( position );

    PlayCyclic( id, attr, localMatrix );
}





//==============================================================================
// ParticleManager::Add
//==============================================================================
//
// Description: Adds a new particle system into the manager, 2nd parameter is a matrix
//                       
// Parameters:	ParticleAttributes structure, matrix describing orientation/
//              position of particles
//
// Return:      None.
//
// Constraints: Currently all systems are set to non cyclic mode. 
//              Attributes structure is extremely simplified at this point
//              
//
//
//==============================================================================
void ParticleManager::Add( const ParticleAttributes& attr, const rmt::Matrix& localMatrix )
{
    if( attr.mType == ParticleEnum::eStars )
    {
        GetSparkleManager()->AddStars( localMatrix.Row( 3 ), 1.0f );
        return; // Early return. This particle system is now done procedurally.
    }
    rWarningMsg( mActiveSystems[ attr.mType ].Size() > 0, "ParticleManager::Add(), Particles of that type have not been loaded" );

    for (unsigned int i = 0 ; i < mActiveSystems[ attr.mType ].Size() ; ++i)
    {
        
        if ( mActiveSystems[ attr.mType ][ i ]->IsActive() == false )
        {
            rmt::Matrix orientedLocalMatrix;
            ReorientUpAxis( localMatrix, &orientedLocalMatrix );
            mActiveSystems[ attr.mType ][ i ]->Reset();
            mActiveSystems[ attr.mType ][ i ]->SetTransform( localMatrix );
            mActiveSystems[ attr.mType ][ i ]->SetActive( true );
            mActiveSystems[ attr.mType ][ i ]->SetBias( p3dParticleSystemConstants::EmitterBias::EMISSION, attr.mEmissionBias );
            break;
        }
    } 
  

}
//==============================================================================
//
// Description: Adds a new particle system into the manager, 2nd parameter is a vector
//                       
// Parameters:	ParticleAttributes structure, vector containing position (no rotation associated)
//              with the particles
//
// Return:      None.
//
// Constraints: Currently all systems are set to non cyclic mode. 
//              Attributes structure is extremely simplified at this point
//              
//
//
//==============================================================================
void ParticleManager::Add( const ParticleAttributes& attr, const rmt::Vector& position )
{
    rmt::Matrix localMatrix;
    localMatrix.Identity();
    localMatrix.FillTranslate ( position );
    Add ( attr, localMatrix );
}
//==============================================================================
// ParticleManager::DebugRender
//==============================================================================
//
// Description: DSG doesnt have sorting yet. Though to see whats going on
//              use this function to force a render of active systems
//             
// Parameters:	None.
//
// Return:      None.
//
// Constraints: Don't call it!.
//              
//              
//
//==============================================================================


//==============================================================================
// ParticleManager::Update
//==============================================================================
//
// Description: Updates the particle animation frame for each active system
//              thats being managed 
//             
// Parameters:	Elapsed time in milliseconds.
//
// Return:      None.
//
// Constraints: None.
//              
//              
//
//==============================================================================

void ParticleManager::Update( unsigned int deltaTime )
{

	float fDeltaTime = static_cast< float >( deltaTime );

    // for each active particle system type
    for (unsigned int i = 0 ; i < mActiveSystems.Size() ; ++i)
    {
        for (unsigned int j = 0 ; j < mActiveSystems[i].Size() ; ++j)
        {
            // Advance controller based upon time elapsed but only if it is active
            if ( mActiveSystems[i][j]->IsActive() )
            {
                // Advance animation and/or throw up new particles.
                mActiveSystems[i][j]->Update( fDeltaTime );

                if ( mActiveSystems[i][j]->IsLocked() )    
                {
                    // Locked systems can be in one of several states
                    // 1) they were told to throw up new particles
                    // in between calls to Update

                    // 2) they were not told to throw up new particles
                    // but still have active particles inside them
                    // (keep system assigned to a user)
                    
                    // 3) no throw, no active particles, return it to the 
                    // system for reassignment

                }
                else 
                {
                    // if the animation is complete and the system is not cyclic   
                    if ( mActiveSystems[i][j]->LastFrameReached() )
                    {
                        // tell the DSG to remove the system                           
                        // disable playback
                        mActiveSystems[i][j]->SetActive( false );    
                    }
                }
            }
        }
    }

}
void ParticleManager::ManagedParticleSystem::SetActive( bool isActive )
{
	if ( mIsActive && !isActive && mIsInDSG)
	{
		// We are deactivating the system
		// remove it from the DSG
        
        WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( mLayer ));
        // Sanity check
        rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );
        pWorldRenderLayer->pWorldScene()->Remove( mpSystem );   

        mIsInDSG = false;
	}
	else if ( !mIsActive && isActive && !mIsInDSG )
	{
        mLayer = static_cast< RenderEnums::LayerEnum > (GetRenderManager()->rCurWorldRenderLayer() );
        
        WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( mLayer ));
        // Sanity check
        rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );
        pWorldRenderLayer->pWorldScene()->Add( mpSystem );   

		mIsInDSG = true;
	}
	mIsActive = isActive;
	
}

void 
ParticleManager::ReorientUpAxis( const rmt::Matrix& in, rmt::Matrix* out )
{
    *out = in;
    out->Row(1) = rmt::Vector( 0.0f, 1.0f, 0.0f );
    out->Row(0).CrossProduct( out->Row(1), out->Row( 0 ) );
}
