#ifndef __EntityDSG_H__
#define __EntityDSG_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   IEntityDSG
//
// Description: The IEntityDSG does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/05/03]
//
//========================================================================

//=================================================
// System Includes
//=================================================
#include <p3d/drawable.hpp>

#ifndef WORLD_BUILDER
#include <render/Culling/NodeFLL.h>
#else
#include "../Culling/NodeFLL.h"
#endif

#include <radtime.hpp>
#include <debug/profiler.h>

class tShadowSkin;
class tShadowMesh;
class SpatialNode;
//=================================================
// Project Includes
//=================================================

//========================================================================
//
// Synopsis:   The IEntityDSG; Synopsis by Inspection.
//
//========================================================================
//#define DONT_DRAW_EXPENSIVE
//#define BREAK_DOWN_PROFILE

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class IEntityDSG : 
    public tDrawable,
    public tDrawable::ShaderCallback
    //,public ISortPriority
{
public:
   IEntityDSG();

   virtual rmt::Vector*       pPosition() = 0;
   virtual const rmt::Vector& rPosition() = 0;

   virtual void SetShadow( tShadowSkin* pShadowSkin ){ rAssert ("SetShadow not implemented"); }
   virtual void SetShadow( tShadowMesh* pShadowMesh ){ rAssert ("SetShadow not implemented"); }
   virtual int CastsShadow(){return 0;}
   virtual void DisplayShadow() { rAssert( "DisplayShadow not implemented" ); }
   virtual void DisplaySimpleShadow( void ) { rAssert( false && "DisplaySimpleShadow not implemented" ); }

   virtual void RenderUpdate();

   virtual void SetShader(tShader* pShader, int i){}
   const tName& GetShaderName(){ return mShaderName; }
   const tUID   GetShaderUID(){ return mShaderName.GetUID(); }

   tShader* Process(tShader* pShaders); 

    virtual void GetPosition( rmt::Vector* ipPosn ){}
    virtual void  SetRank(rmt::Vector& irRefPosn, rmt::Vector& mViewVector);
    float Rank();
    float mRank;


   bool  mTranslucent;
   tName mShaderName;
   static bool msDeletionsSafe;

    SpatialNode* mpSpatialNode;

#ifdef BREAK_DOWN_PROFILE
    static char msMarker;
    unsigned int mMicros;

    void BeginProfile(char* iString)
    {
        iString[0] = IEntityDSG::msMarker; 
        BEGIN_PROFILE(iString)
        mMicros = radTimeGetMicroseconds();
    }
    
    void EndProfile(char* iString)
    {
        mMicros = radTimeGetMicroseconds()-mMicros;
        iString[0] = IEntityDSG::msMarker; 
        END_PROFILE(iString)
    }
#endif

#ifdef DONT_DRAW_EXPENSIVE
    bool IsDrawLong();
#endif

protected:
   virtual ~IEntityDSG();
};

#ifdef DONT_DRAW_EXPENSIVE
    #define IS_DRAW_LONG this->IsDrawLong()
#else
    #define IS_DRAW_LONG false
#endif

#ifdef  BREAK_DOWN_PROFILE
    #define DSG_BEGIN_PROFILE(string) {this->BeginProfile(string);}
    #define DSG_END_PROFILE(string)   {this->EndProfile(string);}
    #define DSG_SET_PROFILE(marker) {IEntityDSG::msMarker=marker;}
#else
    #define DSG_BEGIN_PROFILE(string)
    #define DSG_END_PROFILE(string)
    #define DSG_SET_PROFILE(marker)
#endif
/*
   bool operator<(IEntityDSG* x, IEntityDSG* y)
   {
       if(x->mRank==0.0f)
       {
           return x->mShaderUID-y->mShaderUID;
       }
       else
       {
           return 0.1f*(x->mRank-y->mRank);
       }
   }
*/
#endif
