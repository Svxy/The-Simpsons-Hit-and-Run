#ifndef __DSGFactory_H__
#define __DSGFactory_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   DSGFactory
//
// Description: The DSGFactory does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/05/06]
//
//========================================================================

//=================================================
// System Includes
//=================================================

//=================================================
// Project Includes
//=================================================
class IEntityDSG;
class IntersectDSG;
class tDrawable;
class tGeometry;

//========================================================================
//
// Synopsis:   The DSGFactory; Synopsis by Inspection.
//
//========================================================================
class DSGFactory 
{
public:
   // Static Methods (for creating, destroying and acquiring an instance 
   // of the RenderManager)
   static DSGFactory* CreateInstance();
   static DSGFactory* GetInstance();
   static void  DestroyInstance();

   ///////////////////////////////////////////////////////////////////////
   //Meat Interface
   ///////////////////////////////////////////////////////////////////////
   IEntityDSG*    CreateEntityDSG(     tDrawable* ipDrawable );
   IntersectDSG*  CreateIntersectDSG(  tGeometry* ipGeometry );

private:
   DSGFactory();
   ~DSGFactory();

   static DSGFactory* mspInstance;

   //MS7 Allocations can be pooled here
};

//
// A little syntactic sugar for getting at this singleton.
//
inline DSGFactory* GetDSGFactory() 
{ 
   return( DSGFactory::GetInstance() ); 
}

#endif
