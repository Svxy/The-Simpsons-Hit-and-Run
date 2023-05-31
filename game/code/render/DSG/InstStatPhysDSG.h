#ifndef __InstStatPhysDSG_H__
#define __InstStatPhysDSG_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   InstStatPhysDSG
//
// Description: The InstStatPhysDSG does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/06/17]
//
//========================================================================

//=================================================
// System Includes
//=================================================
#include <simphysics/physicsobject.hpp>
    

//=================================================
// Project Includes
//=================================================
#include <render/DSG/StaticPhysDSG.h>

class tGeometry;

//========================================================================
//
// Synopsis:   The InstStatPhysDSG; Synopsis by Inspection.
//
//========================================================================
class InstStatPhysDSG 
: public StaticPhysDSG
{
public:
    InstStatPhysDSG();
    ~InstStatPhysDSG();

    //////////////////////////////////////////////////////////////////////////
    // tDrawable
    //////////////////////////////////////////////////////////////////////////
    void Display();
    void GetBoundingBox(rmt::Box3D* box);
    void GetBoundingSphere(rmt::Sphere* pSphere);

    virtual void OnSetSimState(sim::SimState* ipCollObj); 

    ///////////////////////////////////////////////////////////////////////
    // Accessors
    ///////////////////////////////////////////////////////////////////////
    sim::SimState*      pSimStateObj(); 
    rmt::Matrix*        pMatrix(); 
    tGeometry*          pGeo();     
   
    ///////////////////////////////////////////////////////////////////////
    // Load interface
    ///////////////////////////////////////////////////////////////////////
    void LoadSetUp(  sim::SimState*      ipSimStateObj, 
					CollisionAttributes* ipCollAttr,
                    const rmt::Matrix&  iMatrix, 
                    tGeometry*          ipGeo     );
    InstStatPhysDSG* Clone(const char* Name, const rmt::Matrix& iMatrix) const;

protected:

    virtual void SetInternalState();

    rmt::Matrix         mMatrix;
    tGeometry*          mpGeo;

    /*
        inherited data:

    rmt::Box3D mBBox;
    rmt::Sphere mSphere;
    rmt::Vector mPosn;
    sim::SimState* mpSimStateObj;
    */

private:
};

#endif
