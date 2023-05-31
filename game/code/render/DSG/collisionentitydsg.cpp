//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implementation of class CollisionEntityDSG
//
// History:     6/17/2002 + Created -- TBJ
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/effects/particlesystem.hpp>
#include <simcollision/collisionobject.hpp>
#include <simcollision/collisionvolume.hpp>

#include <simcollision/impulsebasedcollisionsolver.hpp>

//========================================
// Project Includes
//========================================
#include <render/DSG/collisionentitydsg.h>

#include <sound/soundcollisiondata.h>

#include <events/eventmanager.h>

#include <constants/particleenum.h>

#include <memory/srrmemory.h>

// HACK STUFF FOR REMOVING OBJECTS AFTER COLLISION
#include <render/DSG/DynaPhysDSG.h>
#include <render/RenderManager/RenderManager.h>
#include <render/Culling/WorldScene.h>

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

struct BreakablePair
{
    const char* name;
    BreakablesEnum::BreakableID id;
};

const BreakablePair s_BreakableList[] = 
{
    { "eHydrantBreaking", BreakablesEnum::eHydrantBreaking },
    { "eOakTreeBreaking", BreakablesEnum::eOakTreeBreaking },
    { "ePineTreeBreaking", BreakablesEnum::ePineTreeBreaking },
    { "eBigBarrierBreaking", BreakablesEnum::eBigBarrierBreaking },
    { "eRailCrossBreaking", BreakablesEnum::eRailCrossBreaking },
    { "eSpaceNeedleBreaking", BreakablesEnum::eSpaceNeedleBreaking },
    { "eTomaccoBreaking", BreakablesEnum::eTommacoPlantsBreaking },
    { "eCypressTreeBreaking", BreakablesEnum::eCypressTreeBreaking },
    { "eDeadTreeBreaking", BreakablesEnum::eDeadTreeBreaking },
    { "eKrustyGlassBreaking", BreakablesEnum::eKrustyGlassBreaking },
    { "eSkeletonBreaking", BreakablesEnum::eSkeletonBreaking },
    { "eWillow", BreakablesEnum::eWillow },
    { "eGlobeLight", BreakablesEnum::eGlobeLight },
    { "eTreeMorn", BreakablesEnum::eTreeMorn },
    { "ePalmTreeSmall", BreakablesEnum::ePalmTreeSmall },
    { "ePalmTreeLarge", BreakablesEnum::ePalmTreeLarge },
    { "eStopsign", BreakablesEnum::eStopsign },
    { "ePumpkin", BreakablesEnum::ePumpkin },
    { "ePumpkinMed", BreakablesEnum::ePumpkinMed },
    { "ePumpkinSmall", BreakablesEnum::ePumpkinSmall },
    { "eCasinoJump", BreakablesEnum::eCasinoJump }
};

const int NUM_BREAKABLES = sizeof(s_BreakableList) / sizeof(s_BreakableList[0]);

//==============================================================================
// CollisionEntityDSG::CollisionEntityDSG
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
CollisionEntityDSG::CollisionEntityDSG()
:
mPersistentObjectID( -1 ),
mpCollisionAttributes( 0 ),
mWasParticleEffectTriggered( false ),
mRenderLayer( RenderEnums::LevelSlot )
{
    lastUpdate = 0xffffffff; 
}

//==============================================================================
// CollisionEntityDSG::~CollisionEntityDSG
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
CollisionEntityDSG::~CollisionEntityDSG()
{
BEGIN_PROFILE( "CollisionEntityDSG Destroy" );
    if ( mpCollisionAttributes )
    {
        mpCollisionAttributes->Release( );
        mpCollisionAttributes = 0;
    }
END_PROFILE( "CollisionEntityDSG Destroy" );
}

/*
==============================================================================
CollisionEntityDSG::SetSimState
==============================================================================
Description:    Comment

Parameters:     ( sim::SimState* ipCollObj )

Return:         void 

=============================================================================
*/
void CollisionEntityDSG::SetSimState( sim::SimState* ipCollObj )
{
    if ( ipCollObj )
    {
        ipCollObj->mAIRefPointer = this;
    }
    OnSetSimState( ipCollObj );
}

/*
==============================================================================
CollisionEntityDSG::SetCollisionAttributes
==============================================================================
Description:    Comment

Parameters:     ( CollisionAttributes* pCollisionAttributes )

Return:         void 

=============================================================================
*/
void CollisionEntityDSG::SetCollisionAttributes( CollisionAttributes* pCollisionAttributes )
{
    tRefCounted::Assign( mpCollisionAttributes, pCollisionAttributes );
}
/*
==============================================================================
CollisionEntityDSG::SetRenderLayer
==============================================================================
Description:    Sets the renderlayer. It must match up with the actually layer
                that it inhabits otherwise, when the object tries to move() itself
                in the DSG, the move will fail and the RenderManager will spew warnings


Parameters:     RenderEnums::LayerEnum indicating this object's layer)

Return:         void 

=============================================================================
*/
void CollisionEntityDSG::SetRenderLayer( RenderEnums::LayerEnum renderLayer )
{
    mRenderLayer = renderLayer;
}

//=============================================================================
// CollisionEntityDSG::PostReactToCollision
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector& impulse, sim::Collision& inCollision)
//
// Return:      sim
//
//=============================================================================
sim::Solving_Answer CollisionEntityDSG::PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision)
{
    // generic handling of collision
    //
    // fire off a sound event at appropriate intensity

    sim::CollisionObject* collObjA = inCollision.mCollisionObjectA;
    sim::CollisionObject* collObjB = inCollision.mCollisionObjectB;
    
    sim::SimState* simStateA = collObjA->GetSimState();
    sim::SimState* simStateB = collObjB->GetSimState();

    //
    // SOUND EVENT HERE?
    //

    // need to convert the impulse to a nice friendly float between 0.0 and 1.0
    //
    // just looking at some empirical data, the range of impulse magnitude seems to be from around
    // 0.0 to 100000.0 - at the high end would be a heavy vehicle (3000 kg) hitting a static wall at 122kmh
    // a 2000kg car hitting a wall at 150kmh gave 78706.7

    float impulseMagnitude = impulse.Magnitude();

    const float maxIntensity = 100000.0f;

    float soundScale = impulseMagnitude / maxIntensity;
    if(soundScale > 1.0f)
    {
        soundScale = 1.0f;
    }
    if(soundScale < 0.0f)
    {
        rAssert(0);
    }

    SoundCollisionData soundData( soundScale, 
                                  static_cast<CollisionEntityDSG*>(simStateA->mAIRefPointer),
                                  static_cast<CollisionEntityDSG*>(simStateB->mAIRefPointer) );
    GetEventManager()->TriggerEvent( EVENT_COLLISION, &soundData );

    return sim::Solving_Continue;
}




/*
==============================================================================
CollisionEntityDSG::GetCollisionAttributes
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         CollisionAttributes

=============================================================================
*/
CollisionAttributes* CollisionEntityDSG::GetCollisionAttributes( void ) const
{
    return mpCollisionAttributes;
}



CollisionAttributes::CollisionAttributes ()
: mBreakableID( BreakablesEnum::eNull )
{
    memset(mp_Sound, 0, sizeof(mp_Sound));
    memset(mp_Animation, 0, sizeof(mp_Animation));
    mp_Particle=ParticleEnum::eNull;
    mMass=0.0;
    mFriction=0.0;
    mElasticity=0.0;
    mClasstypeid =WTF;

    mPhizProp = 0;
}


CollisionAttributes::CollisionAttributes(char* p_sound,char* p_particle,char* p_animation,float friction, float mass,float elasticity,unsigned int classtypeid, float volume)
{
    // if the item creates a specific particle effect when hit
    // get it from p_particle string
    if ( strcmp( p_particle,"eGarbage" )==0)
    {
        mp_Particle = ParticleEnum::eGarbage;
    }
    else if ( strcmp( p_particle, "eShrub" )==0 )
	{
		mp_Particle = ParticleEnum::eShrub;
	}
	else if ( strcmp( p_particle, "eOakTreeLeaves" )==0 )
	{
		mp_Particle = ParticleEnum::eOakTreeLeaves;
	}
	else if ( strcmp( p_particle, "eMail" )==0 )
	{
		mp_Particle = ParticleEnum::eMail;
	}
	else if ( strcmp( p_particle, "ePineTreeNeedles" )==0)
	{
		mp_Particle = ParticleEnum::ePineTreeNeedles;
	}
	else if ( strcmp( p_particle, "eStars" ) == 0)
	{
		mp_Particle = ParticleEnum::eStars;
	}
    else if(strcmp( p_particle, "eParkingMeter" ) == 0)
    {
        mp_Particle = ParticleEnum::eParkingMeter ;
    }
    else if(strcmp( p_particle, "eCarExplosion" ) == 0)
    {
        mp_Particle = ParticleEnum::eCarExplosion ;
    }
    else if(strcmp( p_particle, "ePopsicles" ) == 0)
    {
        mp_Particle = ParticleEnum::ePopsicles ;
    }
    else
    {
        mp_Particle = ParticleEnum::eNull;
    }

    // If the item is breakable, the enumeration string is stored in
    // p_animation

    mBreakableID = BreakablesEnum::eNull;
    for ( int i = 0 ; i < NUM_BREAKABLES ; i++ )
    {
        if ( strcmp( p_animation, s_BreakableList[i].name ) == 0 )
        {
            mBreakableID = s_BreakableList[i].id;
            break;
        }
    }
   // mp_Particle=p_particle;
    strcpy(mp_Animation,p_animation);
    strcpy(mp_Sound, p_sound);
    mMass=mass;
    //mMass = 1000.0f;
    switch (classtypeid)
    {
        case 0:
            {
                mClasstypeid = WTF;
                break;
            }
        case 1:
            {
                mClasstypeid = GROUND;
                break;
            }
        case 2:
            {
                mClasstypeid = PROP_STATIC;
                break;
            }
        case 3:
            {
                mClasstypeid = PROP_MOVEABLE;
                break;
            }
        case 4:
            {
                mClasstypeid = PROP_BREAKABLE;
                break;
            }
        case 5:
            {
                mClasstypeid = ANIMATED_BV;
                break;
            }
        case 6:
            {
                mClasstypeid = DRAWABLE;
                break;
            }
        case 7:
            {
                mClasstypeid = STATIC;
                break;
            }
        case 8:
            {
                mClasstypeid = PROP_DRAWABLE;
                break;
            }
        case 10:
            {    
            
                mClasstypeid = PROP_ONETIME_MOVEABLE;
                break;
            }
        default:
            {
                printf("ERROR: Unknown ClasstypeID \n");
            }
    }


    // need to make new physics properties for this thing:

    // TODO - verify that incoming values make any sense

    MEMTRACK_PUSH_GROUP("PhysicsProperties");

    mPhizProp = new(GMA_LEVEL_OTHER)sim::PhysicsProperties;
    mPhizProp->AddRef();

    MEMTRACK_POP_GROUP("PhysicsProperties");

    
    if(volume > 0.0f)
    {
        if(mClasstypeid == PROP_MOVEABLE || mClasstypeid == PROP_BREAKABLE || mClasstypeid == PROP_ONETIME_MOVEABLE)
        {   
            float density = mMass / volume;
            mPhizProp->SetDensityCGS(density);
        }
        else
        {
            rAssert(0); // shouldn't be setting volume (mass) on this thing
        }
    }
    
        
    
    
    if(friction > 0.0f && friction < 3.5f)  // changed from friction >= 0.0f - don't think we ever want somethign with 0.0 friction
    {
        mFriction = friction;
        mPhizProp->SetFrictCoeffCGS(friction);  // good    
    }
    else
    {
        mFriction = 1.2f;
        mPhizProp->SetFrictCoeffCGS(1.2f);  // good    
        
    }
    
    if(elasticity >= 1.0f && elasticity <= 2.0f)
    {
        mElasticity = elasticity;    
        mPhizProp->SetRestCoeffCGS(elasticity);       // 1.0 to 2.0        
    }
    else
    {
        mElasticity = 1.5f;
        mPhizProp->SetRestCoeffCGS(1.5f);       // 1.0 to 2.0                
    }      
    
    mPhizProp->SetTangRestCoeffCGS(0.0f);   // leave this one at 0
    
    
}


CollisionAttributes::~CollisionAttributes()
{
    if ( mPhizProp != NULL )
    {
        mPhizProp->Release();
    }
}

char* CollisionAttributes::GetAnimation()
{
    return mp_Animation;
}
BreakablesEnum::BreakableID CollisionAttributes::GetBreakable()
{
    return mBreakableID;
}
ParticleEnum::ParticleID CollisionAttributes::GetParticle()
{
    return mp_Particle;
}

char* CollisionAttributes::GetSound()
{
    return mp_Sound;
}


float CollisionAttributes::GetFriction()
{
    return mFriction;
}

float CollisionAttributes::GetMass()
{
    return mMass;
}

float CollisionAttributes::GetElasticity()
{
    return mElasticity;
}

unsigned int CollisionAttributes::GetClasstypeid ()
{
    return mClasstypeid;
}

void CollisionAttributes::SetAnimation(char* p_animation)
{
    //mp_Animation=p_animation;
    strcpy(mp_Animation,p_animation);
    
}
void CollisionAttributes::SetBreakable( BreakablesEnum::BreakableID id )
{
    mBreakableID = id;
}
void CollisionAttributes::SetParticle(ParticleEnum::ParticleID p_particle)
{
    mp_Particle=p_particle;
}

void CollisionAttributes::SetSound(char* p_sound)
{
    //mp_Sound=p_sound;    
    strcpy(mp_Sound,p_sound);

}

void CollisionAttributes::SetMass(const float mass)
{
    mMass=mass;
}

void CollisionAttributes::SetFriction(const float friction)
{
    mFriction=friction;
}

void CollisionAttributes::SetElasticity(const float elasticity)
{
    mElasticity=elasticity;
}

void CollisionAttributes::SetClasstypeid(unsigned int classtypeid)
{
      switch (classtypeid)
    {
        case 0:
            {
                mClasstypeid = WTF;
                break;
            }
        case 1:
            {
                mClasstypeid = GROUND;
                break;
            }
        case 2:
            {
                mClasstypeid = PROP_STATIC;
                break;
            }
        case 3:
            {
                mClasstypeid = PROP_MOVEABLE;
                //mClasstypeid = PROP_ONETIME_MOVEABLE;
                break;
            }
        case 4:
            {
                mClasstypeid = PROP_BREAKABLE;
                break;
            }
        case 5:
            {
                mClasstypeid = ANIMATED_BV;
                break;
            }
        case 6:
            {
                mClasstypeid = DRAWABLE;
                break;
            }
        case 7:
            {
                mClasstypeid = STATIC;
                break;
            }
        case 8:
            {
                mClasstypeid = PROP_DRAWABLE;
                break;
            }
        case 9:
            {
                mClasstypeid = PROP_ANIM_BREAKABLE;
                break;
            }
        case 10:
            {
                mClasstypeid = PROP_ONETIME_MOVEABLE;
                //mClasstypeid = PROP_MOVEABLE;
                
                break;
            }

        default:
            {
                printf("ERROR: Unknown ClasstypeID \n");
            }
    }
}



