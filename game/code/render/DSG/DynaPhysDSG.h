#ifndef __DynaPhysDSG_H__
#define __DynaPhysDSG_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   DynaPhysDSG
//
// Description: The DynaPhysDSG does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/06/17]
//
//========================================================================

//=================================================
// System Includes
//=================================================

//=================================================
// Project Includes
//=================================================
#include <render/DSG/StaticPhysDSG.h>

// a little class to perform smoothing on the velocity values
class Smoother
{
public:
    Smoother(float initial, unsigned c) : 
        rollingAverage(initial),
        factor(1.0f / float(c))
    {
    }

    ~Smoother()
    {
    }

    float Smooth(float sample)
    {
        rollingAverage = (rollingAverage * (1.0f - factor)) + (sample * factor);
        return rollingAverage ;
    }

    void SetAverage( float sample )
    {
        rollingAverage = sample;
    }

protected:
    float rollingAverage;
    float factor;
};

//========================================================================
//
// Synopsis:   The DynaPhysDSG; Synopsis by Inspection.
//
//========================================================================
class DynaPhysDSG 
: public StaticPhysDSG
{
public:
    DynaPhysDSG();
    ~DynaPhysDSG();

    virtual void Update(float dt);

    
    // vehicle's need these
    virtual int FetchGroundPlane();     // tell object to get itself a ground plane, through worldphysicsmanager, through groundplanepool
                                        // update the refs

    virtual void FreeGroundPlane();     // make this safe to call even when we don't have one?

    virtual bool IsAtRest(); // Is this obejct currently at rest
    virtual void RestTest(); // Perform the object rest test
    virtual void OnTransitToAICtrl() {}
    // need these?
    virtual int GetGroundPlaneIndex() {return mGroundPlaneIndex;}
    //virtual void SetGroundPlaneIndex(int index) {mGroundPlaneIndex = index;}
    virtual void AddToSimulation();
    virtual void ApplyForce( const rmt::Vector& direction, float force );
        
    bool IsCollisionEnabled()const;

    bool mIsHit;    // had to move this up from InstDynaPhys

protected:

    Smoother mPastLinear;
    Smoother mPastAngular;

    //sim::PhysicsObject* mpPhysObj;

    //rmt::Matrix*        mpMatrix;
    //tGeometry*          mpGeo;

    int mGroundPlaneIndex;
    int mGroundPlaneRefs;
private:
    

};

#endif
