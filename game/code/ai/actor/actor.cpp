//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   actor
//
// Description: actor implementation
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <ai/actor/actor.h>   
#include <ai/actor/actordsg.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/worldrenderlayer.h>
#include <render/intersectmanager/intersectmanager.h>
#include <render/dsg/fenceentitydsg.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================



//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================


//===========================================================================
// Member Functions
//===========================================================================


Actor::Actor():
mp_StateProp( NULL ),
m_ShouldDespawn( true ),
m_IsInDSG( false )
{
    // Lets init the m_IntersectionSphere variable to be at a ridiculous coordinate so
    // that it will be filled on update
    m_IntersectionSphere.centre = rmt::Vector( 0, 0, 0 );
    m_IntersectionSphere.radius = 0.01f;
}

Actor::~Actor()
{
    tRefCounted::Release( mp_StateProp );
}

void Actor::SetPosition( const rmt::Vector& position )
{
    m_PreviousPosition = position;
    mp_StateProp->SetPosition( position );       
}
        
void Actor::GetPosition( rmt::Vector* pPosition )const
{
    mp_StateProp->GetPosition( pPosition );
}


void Actor::GetTransform( rmt::Matrix* pTransform )const
{
    mp_StateProp->GetTransform( pTransform );
}

const ActorDSG* Actor::GetDSG() const
{
    return mp_StateProp;
}

void Actor::SetTransform( const rmt::Matrix& transform )
{
    m_PreviousPosition = transform.Row(3);
    mp_StateProp->SetTransform( transform );
}



void 
Actor::SetState( int state )
{
    mp_StateProp->SetState( state );
}

unsigned int
Actor::GetState()const
{
    return mp_StateProp->GetState();
}

const char* 
Actor::GetName()const
{
    return mp_StateProp->GetName();
}

tUID 
Actor::GetUID()const
{
    return mp_StateProp->GetUID();
}

tUID 
Actor::GetStatePropUID()const
{
    return mp_StateProp->GetStatePropUID();
}

void 
Actor::SetUID( tUID uid )
{
    mp_StateProp->SetUID( uid );
}

void
Actor::SetStatePropID( unsigned int id )
{
    mp_StateProp->SetID( id );
}

void
Actor::AddToDSG()
{
    if ( m_IsInDSG == false )
    {
        WorldRenderLayer* wrl = static_cast< WorldRenderLayer* > ( GetRenderManager()->mpLayer( RenderEnums::LevelSlot ) );
        wrl->pWorldScene()->Add( mp_StateProp );
        m_IsInDSG = true;
    }
}   

void Actor::RemoveFromDSG()
{
    if ( m_IsInDSG )
    {
        WorldRenderLayer* wrl = static_cast< WorldRenderLayer* > ( GetRenderManager()->mpLayer( RenderEnums::LevelSlot ) );
        wrl->pWorldScene()->Remove( mp_StateProp );   
        m_IsInDSG = false;
    }
}

void Actor::LoadShield( const char* statePropName )
{
    if ( mp_StateProp != NULL )
    {
        mp_StateProp->LoadShield( statePropName );
    }   
}

void Actor::LoadTractorBeam( const char* statePropName )
{
    if ( mp_StateProp != NULL )
    {
        mp_StateProp->LoadTractorBeam( statePropName );
    }
}

void Actor::ActivateTractorBeam()
{
    if ( mp_StateProp != NULL )
        mp_StateProp->ActivateTractorBeam();
}

void Actor::DeactivateTractorBeam()
{
    if ( mp_StateProp != NULL )
        mp_StateProp->DeactivateTractorBeam();   
}

bool Actor::IsTractorBeamOn()const
{
    if ( mp_StateProp != NULL )
        return mp_StateProp->IsTractorBeamOn();      
    else
        return false;
}

void Actor::FillIntersectionList( const rmt::Vector& position, float radius )
{
    m_IntersectionList.Clear();
    FillIntersectionListDynamics( position, radius );
    FillIntersectionListStatics( position, radius );


    m_IntersectionSphere.centre = position;
    m_IntersectionSphere.radius = radius;
}

void Actor::FillIntersectionListDynamics( const rmt::Vector& position, float radius )
{
    m_IntersectionList.ClearDynamics();
    // Iterate through the dynamics and grab them all
    ReserveArray< DynaPhysDSG* > dpList(200);
    GetIntersectManager()->FindDynaPhysElems( const_cast< rmt::Vector& >( position ), radius, dpList );
    for ( int i = 0 ; i < dpList.mUseSize ; i++ )
    {
        DynaPhysDSG* object = dpList[i];
        // Lets not add ourselves
        if ( object->GetAIRef() == PhysicsAIRef::ActorStateProp )
        {
            const ActorDSG* actordsg = static_cast< const ActorDSG* >( object );
            if ( actordsg == GetDSG() )
                continue;

            // Are the ids the same? Dont make them intersect if they are
            if ( actordsg->GetID() == GetDSG()->GetID() )
                continue;    
        }

        sim::SimState* simState = object->GetSimState();
        m_IntersectionList.AddDynamic( simState, object );
    }    
}

void Actor::FillIntersectionListStatics( const rmt::Vector& position, float radius )
{
    m_IntersectionList.ClearStatics();
    // Iterate through the dynamics and grab them all
    ReserveArray< StaticPhysDSG* > spList(200);
    GetIntersectManager()->FindStaticPhysElems( const_cast< rmt::Vector& >( position ), radius, spList );
    for ( int i = 0 ; i < spList.mUseSize ; i++ )
    {
        StaticPhysDSG* object = spList[i];
        // Lets not add ourselves
        if ( object != static_cast< const StaticPhysDSG* >( GetDSG() ) )
        {
            sim::SimState* simState = object->GetSimState();
            m_IntersectionList.AddStatic( simState );
        }
    }        
    FillIntersectionListFence( position, radius );
}

void Actor::FillIntersectionListFence( const rmt::Vector& position, float radius )
{
    m_IntersectionList.ClearFencePieces();

    ReserveArray< FenceEntityDSG* > feList(200);
    GetIntersectManager()->FindFenceElems( const_cast< rmt::Vector& > (position), radius, feList );
    for ( int i = 0 ; i < feList.mUseSize ; i++ )
    {
        FenceEntityDSG* object = feList[i];
        m_IntersectionList.AddFencePiece( object );
    }
}
