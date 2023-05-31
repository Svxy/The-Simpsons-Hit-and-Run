//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        EntityDSG.cpp
//
// Description: Implementation for IEntityDSG class.
//
// History:     Implemented	                         --Devin [5/3/2002]
//========================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#ifdef WORLD_BUILDER
#include "IEntityDSG.h"
#else
#include <memory/classsizetracker.h>
#include <meta/triggervolumetracker.h>
#include <render/DSG/IEntityDSG.h>
#include <render/Culling/SwapArray.h>
#include <render/RenderManager/RenderManager.h>
#include <p3d/shader.hpp>
#endif //WORLD_BUILDER


//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************
bool IEntityDSG::msDeletionsSafe = false;
#ifdef BREAK_DOWN_PROFILE
    char IEntityDSG::msMarker = '_';
#endif

//************************************************************************
//
// Public Member Functions : IEntityDSG Interface
//
//************************************************************************
//========================================================================
// IEntityDSG::IEntityDSG()
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
IEntityDSG::IEntityDSG() :
    mTranslucent(0),
    mpSpatialNode(NULL)
{
    CLASSTRACKER_CREATE( IEntityDSG );
#ifndef WORLD_BUILDER
    //if(msDeletionsSafe)
    //{
    //     GetRenderManager()->MunchDelList(5);
    //}
    mShaderName = "__none__";
    /*
    SwapArray<IEntityDSG*>& rEDL = GetRenderManager()->mEntityDeletionList;
    int i=3;
    while(rEDL.mUseSize && i)
    {
        rEDL[0]->Release();
        rEDL.Remove(0);
    }
    */
#endif
#ifdef BREAK_DOWN_PROFILE
    mMicros = 0;
#endif
}
//========================================================================
// IEntityDSG::
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
tShader* IEntityDSG::Process(tShader* pShader)
{
#ifndef WORLD_BUILDER
    if( mShaderName != "__none__" && 
        mShaderName != pShader->GetNameObject() )
    {
        //more than one shader, can't sort by shader
       // rDebugPrintf("More than one Shader in IEntityDSG %s\n", this->GetName() );
        mShaderName = "__none__"; 
    }
    else
    {
        mShaderName = pShader->GetName();
    }
#endif
    return pShader;
}
//========================================================================
// EntityDSG::~IEntityDSG()
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
IEntityDSG::~IEntityDSG()
{
    CLASSTRACKER_DESTROY( IEntityDSG );
}
//========================================================================
// IEntityDSG::RenderUpdate()
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
void IEntityDSG::RenderUpdate()
{
   //Do Nothing for the base case
   //Animated objects/textures will update here in subclasses
}
//========================================================================
// IEntityDSG::
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
const tUID BAD_LIGHT = tEntity::MakeUID("l7_searchlights");
void  IEntityDSG::SetRank(rmt::Vector& irRefPosn, rmt::Vector& irRefVect)
{
//    rmt::Vector posn;
//    GetPosition(&posn);
    rmt::Vector temp;
    rmt::Sphere sphere;
    GetBoundingSphere(&sphere);

    //sphere.centre.Sub(sphere.radius);
    sphere.centre.Sub(irRefPosn);
    temp = irRefVect;
    temp.Scale(sphere.radius);
    sphere.centre.Sub(temp);

    if(sphere.centre.Dot(irRefVect)<0.0f)
        mRank = -sphere.centre.MagnitudeSqr();//-(1.75f*sphere.radius*sphere.radius);//posn.MagnitudeSqr();
    else
        mRank = sphere.centre.MagnitudeSqr();//-(1.75f*sphere.radius*sphere.radius);//posn.MagnitudeSqr();
	
	// Evil hack for L7 military spotlights
	if(GetUID() == BAD_LIGHT)
	{
		mRank = FLT_MAX;
	}
    //posn.Sub(irRefPosn);
}

float IEntityDSG::Rank()
{
    return mRank;
}

#ifdef DONT_DRAW_EXPENSIVE
bool IEntityDSG::IsDrawLong()
{
    if(mMicros>90)
    {
        /*rmt::Matrix Trans;
        rmt::Vector posn;
        GetPosition(&posn);
        Trans.Identity();
        Trans.FillTranslate(posn);
        p3d::pddi->PushMatrix(PDDI_MATRIX_MODELVIEW);
	        p3d::pddi->MultMatrix(PDDI_MATRIX_MODELVIEW,&Trans);
            GetTriggerVolumeTracker()->mpTriggerSphere->Display();
        p3d::pddi->PopMatrix(PDDI_MATRIX_MODELVIEW);*/
        rReleasePrintf("%s is taking %u us to draw.\n", GetName(), mMicros );
        return false;
    }
    return false;
}
#endif

//************************************************************************
//
// Protected Member Functions : IEntityDSG 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : IEntityDSG
//
//************************************************************************

