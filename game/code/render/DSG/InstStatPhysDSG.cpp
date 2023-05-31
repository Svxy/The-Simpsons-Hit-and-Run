//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        InstStatPhysDSG.cpp
//
// Description: Implementation for InstStatPhysDSG class.
//
// History:     Implemented	                         --Devin [6/17/2002]
//========================================================================

//========================================
// System Includes
//========================================
#include <p3d/geometry.hpp>
#include <p3d/utility.hpp>
#include <simcollision/collisionobject.hpp>
#include <simcollision/collisionvolume.hpp>
#include <p3d/billboardobject.hpp>

//========================================
// Project Includes
//========================================
#include <render/DSG/InstStatPhysDSG.h>
#include <memory/srrmemory.h>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

//************************************************************************
//
// Public Member Functions : InstStatPhysDSG Interface
//
//************************************************************************
//========================================================================
// InstStatPhysDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
InstStatPhysDSG::InstStatPhysDSG()
{
}
//========================================================================
// InstStatPhysDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
InstStatPhysDSG::~InstStatPhysDSG()
{
BEGIN_PROFILE( "InstStatPhysDSG Destroy" );
    if(mpGeo)
    {
        mpGeo->Release();
    }
END_PROFILE( "InstStatPhysDSG Destroy" );
}
//========================================================================
// InstStatPhysDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void InstStatPhysDSG::Display()
{
#ifdef PROFILER_ENABLED
    char profileName[] = "  InstStatPhysDSG Display";
#endif
    DSG_BEGIN_PROFILE(profileName)
    if(CastsShadow())
    {
        BillboardQuadManager::Enable();
    }
    p3d::pddi->PushMultMatrix(PDDI_MATRIX_MODELVIEW, &mMatrix);
        mpGeo->Display();
    p3d::pddi->PopMatrix(PDDI_MATRIX_MODELVIEW);
    if(CastsShadow())
    {
        BillboardQuadManager::Disable();
        DisplaySimpleShadow();
    }
    DSG_END_PROFILE(profileName)
}
//========================================================================
// InstStatPhysDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void InstStatPhysDSG::GetBoundingBox(rmt::Box3D* box)
{
    // the box is in world space already:
    
    box->low = mBBox.low;
    box->high = mBBox.high;
    
    
}
//========================================================================
// InstStatPhysDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void InstStatPhysDSG::GetBoundingSphere(rmt::Sphere* pSphere)
{
    // the sphere is in world space already:

    pSphere->centre = mSphere.centre;
    pSphere->radius = mSphere.radius;
}


//=============================================================================
// InstStatPhysDSG::OnSetSimState
//=============================================================================
// Description: Comment
//
// Parameters:  ( sim::SimState* ipSimState )
//
// Return:      void 
//
//=============================================================================
void InstStatPhysDSG::OnSetSimState( sim::SimState* ipSimState )
{
    tRefCounted::Assign( mpSimStateObj, ipSimState );

    // ok for this to have the same ai ref I guess    
    //mpSimStateObj->mAIRefIndex = StaticPhysDSG::GetAIRef();
    mpSimStateObj->mAIRefIndex = this->GetAIRef();

    // maybe a bit useless to have this in another method, but oh well...
    SetInternalState();
}


//=============================================================================
// InstStatPhysDSG::SetInternalState
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      virtual 
//
//=============================================================================
void InstStatPhysDSG::SetInternalState()
{



    mPosn = mpSimStateObj->GetPosition();

    // this box will be in world space:
    mpGeo->GetBoundingBox(&mBBox);
    mBBox.low.Transform(mMatrix);
    mBBox.high.Transform(mMatrix);

    mpGeo->GetBoundingSphere(&mSphere);
    mSphere.centre.Transform(mMatrix);

/*
    mBBox.low   = mPosn;
    mBBox.high  = mPosn;
    mBBox.high += mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;
    mBBox.low  -= mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;


    mSphere.centre.Sub(mBBox.high,mBBox.low);
    mSphere.centre *= 0.5f;
    mSphere.centre.Add(mBBox.low);
    mSphere.radius = mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mSphereRadius;
    */



    /*
        this is what InstDynaPhys does



    mPosn = mpSimStateObj->GetPosition();
    // set up box and sphere
    //
    // note, unlike the StaticPhysDSG, this box and sphere are in local space

    rmt::Vector center = mpPhysObj->GetExternalCMOffset();
    float radius = mpSimStateObj->GetSphereRadius();

    mBBox.low   = center;
    mBBox.high  = center;
    mBBox.high += mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;
    mBBox.low  -= mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;

    mSphere.centre = center;
    mSphere.radius = radius;
   
    */






    /*
        this is what StaticPhysDSG does:


    mPosn = mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mPosition;
   
    mBBox.low   = mPosn;
    mBBox.high  = mPosn;
    mBBox.high += mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;
    mBBox.low  -= mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;

    mSphere.centre.Sub(mBBox.high,mBBox.low);
    mSphere.centre *= 0.5f;
    mSphere.centre.Add(mBBox.low);
    mSphere.radius = mpS

    */




}

   ///////////////////////////////////////////////////////////////////////
   // Accessors
   ///////////////////////////////////////////////////////////////////////
//========================================================================
// InstStatPhysDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
sim::SimState*  InstStatPhysDSG::pSimStateObj()
{
    return mpSimStateObj; 
}
//========================================================================
// InstStatPhysDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
rmt::Matrix* InstStatPhysDSG::pMatrix()
{
    return &mMatrix;
}
//========================================================================
// InstStatPhysDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
tGeometry* InstStatPhysDSG::pGeo()
{
    return mpGeo;
}
   
   ///////////////////////////////////////////////////////////////////////
   // Load interface
   ///////////////////////////////////////////////////////////////////////
//========================================================================
// InstStatPhysDSG::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void InstStatPhysDSG::LoadSetUp
(  
    sim::SimState*      ipSimStateObj, 
	CollisionAttributes* ipCollAttr,
    const rmt::Matrix&  iMatrix, 
    tGeometry*          ipGeo     
)
{
 
    mMatrix    = iMatrix;
    mpGeo       = ipGeo;
    mpGeo->AddRef();

    SetCollisionAttributes(ipCollAttr);

    // recall: this is the non-virtual method
    SetSimState(ipSimStateObj);

//    mShaderUID = ipGeo->GetShader(0)->GetUID();
    ipGeo->ProcessShaders(*this);
}

InstStatPhysDSG* InstStatPhysDSG::Clone(const char* Name, const rmt::Matrix& iMatrix) const
{
    InstStatPhysDSG* c = new InstStatPhysDSG();
    rAssert(c);
    c->SetName(Name);
    c->mMatrix = iMatrix;
    c->mpGeo = this->mpGeo;
    c->mpGeo->AddRef();
    c->SetCollisionAttributes(this->GetCollisionAttributes());
    sim::SimState* simState = sim::SimState::CreateSimState(this->GetSimState());
    simState->SetControl(sim::simAICtrl);
    simState->GetCollisionObject()->SetIsStatic(false);
    simState->GetCollisionObject()->SetManualUpdate(false);
    simState->SetTransform(iMatrix); 
    simState->GetCollisionObject()->Update();      
    simState->GetCollisionObject()->SetIsStatic(true);
    simState->GetCollisionObject()->SetManualUpdate(true);
    c->SetSimState(simState);
    mpGeo->ProcessShaders(*c);
    c->SetShadow(this->mpShadow);
    return c;
}
//************************************************************************
//
// Protected Member Functions : InstStatPhysDSG 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : InstStatPhysDSG 
//
//************************************************************************
