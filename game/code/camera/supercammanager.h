#ifndef __SuperCamManager_H__
#define __SuperCamManager_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   supercammanager
//
// Description: The supercammanager does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/05/01]
//
//========================================================================

//=================================================
// System Includes
//=================================================
#include <p3d/pointcamera.hpp>

//=================================================
// Project Includes
//=================================================
#include <constants/maxplayers.h>
#include <camera/supercamcentral.h>

//========================================================================
//
// Synopsis:   The supercammanager; Synopsis by Inspection.
//
//========================================================================
class SuperCamManager 
{
public:
   // Static Methods (for creating, destroying and acquiring an instance 
   // of the SuperCamManager )
   static SuperCamManager* CreateInstance();
   static SuperCamManager* GetInstance();
   static void  DestroyInstance();

   void Init( bool shutdown );

   //SuperCamCentral& GetSCC( int iPlayer );

   // I don't think there's any need for reference here
   SuperCamCentral* GetSCC( int iPlayer );
   
   void PreCollisionPrep();

   void Update( unsigned int iElapsedTime, bool isFirstSubstep );
   
   
   // called by worldphysicsmanager for cameras to make their submissions to collision
   void SubmitStatics();

   void ToggleFirstPerson( int controllerID );

private:
   SuperCamManager();
   ~SuperCamManager();

   // Static Private Render Data
   static SuperCamManager* mspInstance;

   SuperCamCentral mSCCs[MAX_PLAYERS];
};

//
// A little syntactic sugar for getting at this singleton.
//
inline SuperCamManager* GetSuperCamManager() 
{ 
   return( SuperCamManager::GetInstance() ); 
}

#endif
