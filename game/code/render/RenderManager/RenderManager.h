#ifndef __RENDER_MANAGER_H__
#define __RENDER_MANAGER_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   RenderManager   
//
// Description: The RenderManager Controller provides all interfaces 
//              neccessary for interaction with the render systems from
//              Systems external to the Render directory.      [4/17/2002]
//
// History:     + Implemented Initial interfaces      -- Devin [4/17/2002]
//
//========================================================================

//=================================================
// System Includes
//=================================================

//=================================================
// Project Includes
//=================================================
//#include <render/RenderManager/RenderLayer.h>
#include <render/Culling/SwapArray.h>
#include <render/Enums/RenderEnums.h>
#include <render/Loaders/ChunkListenerCallback.h>
#include <loading/loadingmanager.h>
#include <events/eventlistener.h>
#include <raddebugwatch.hpp>

//fwd dec's
class IEntityDSG;
class RenderLayer;
class WorldRenderLayer;
class WorldScene;
class tLightGroup;
class ZoneEventLocator;

//MS7: Cary added this to make the clouds go.
class tMultiController;

//=================================================
// TODOs: Re-encapsulate (into other file(s)) the
//        data-sets below:
//=================================================

//========================================================================
//
// Synopsis:   The RenderManager; 
//                -The entry point for all non render systems
//                -Supports
//                   -ContextSwitch Interface           --Devin[4/17/2002]
//                   -DSG Interface                     
//========================================================================
class RenderManager 
:  public ChunkListenerCallback,
   public LoadingManager::ProcessRequestsCallback,
   public EventListener
{
public:

   // Static Methods (for creating, destroying and acquiring an instance 
   // of the RenderManager)
   static RenderManager* CreateInstance();
   static RenderManager* GetInstance();
   static void  DestroyInstance();

   ///////////////////////////////////////////////////////////////////////
   // Accessors
   ///////////////////////////////////////////////////////////////////////
   WorldScene* pWorldScene();
   WorldRenderLayer* pWorldRenderLayer();
   inline int& rCurWorldRenderLayer();

   // Context Interface
   bool LoadAllNeededData(); 
   void SetLoadData(       RenderEnums::LayerEnum   isLayer,
                           RenderEnums::LevelEnum   isLevel, 
                           RenderEnums::MissionEnum isMission );

   void InitLevel();
   // Copies light values from the given sun group into the RenderManager
   // Dynamic allocations on GMA_LEVEL_OTHER
   // Pass in NULL to disable
   void SetLevelLayerLights( tLightGroup* SunGroup );
   // Releases sun group information set in SetLevelLayerLights
   void ClearLevelLayerLights();

   void SetLightMod( const tColour& LightMod );
   void ResetMoodLighting( bool Immediate = false );
   void DumpAllLoadedData();
   void ContextUpdate(     unsigned int iElapsedTime );

   // Layer Interface
   RenderLayer* mpLayer( RenderEnums::LayerEnum isLayer );
   void FreezeAllLayers();
   void FreezeForPresentation( void );
   void ThawFromPresentation( void );

   void RedirectChunks( int ChunkDestinationMask );

   ///////////////////////////////////////////////////////////////////////
   // ChunkListenerCallback's
   ///////////////////////////////////////////////////////////////////////
   virtual void OnChunkLoaded( tEntity*   ipEntity, 
                               int        iUserData,
                               unsigned   iChunkID    );        

   ///////////////////////////////////////////////////////////////////////
   // LoadingManager::ProcessRequestsCallback's
   ///////////////////////////////////////////////////////////////////////
   void OnProcessRequestsComplete( void* pUserData );
   bool DoneInitialLoad();
   void DoPostLevelLoad();

   ///////////////////////////////////////////////////////////////////////
   // EventListener
   ///////////////////////////////////////////////////////////////////////
   void HandleEvent( EventEnum id, void* pEventData );

   void FlushDelList();
   void MunchDelList(unsigned howManyMicroseconds);

   SwapArray<tRefCounted*> mEntityDeletionList;
   void SetBlurAlpha( float alpha ) { mBlurAlpha = alpha; }
private:
   //Initialize the mpLayers
   void InitLayers();

   // Private: it's a fuckin singleton.
   RenderManager();
   ~RenderManager();

   // Private Member Data
   RenderLayer*   mpRenderLayers[RenderEnums::numLayers];
   tMultiController* mClouds;

   // Static Private Render Data
   static RenderManager* mspInstance;

   int msLayer;   //RenderEnums::LayerEnum   
   int msLevel;   //RenderEnums::LevelEnum   
   int msMission; //RenderEnums::MissionEnum 

   int mCurWorldLayer;

   bool mDoneInitialLoad;
   
   SwapArray<ZoneEventLocator*> mZELs;
   ZoneEventLocator* mpZEL;
   bool mbDynaLoading;
   bool mbFirstDynamicZone;
   bool mbDrivingTooFastLoad;
   bool mbLoadZonesDumped;
   bool mbIgnoreVisibilityClear;
   bool mbInVsibilityVolume;
   // Motion blur accessors / mutator functions
   void SetMotionBlurEnable( bool enable )  { mEnableMotionBlur = enable; }
   bool IsBlurEnabled() const               { return mEnableMotionBlur; }

#ifdef RAD_PS2
   void ApplyPS2Blur();
#endif
   void AdjustBlurByFrameRate( unsigned int elapsedTime );

#ifdef DEBUGWATCH
   unsigned int mDebugSwapTime, mDebugRenderTime;
   bool mDebugDumpAllZones;
#endif

    bool mEnableMotionBlur;
    float mBlurAlpha;

    // Simple class to log the time it took each frame and
    // average it out
    class AvgTimeCounter
    {
    public:
        AvgTimeCounter();
        void Tick( unsigned int elapsedTime );
        unsigned int GetAverageTimePerFrame()const;

        enum { ELAPSED_TIME_ARRAY_SIZE = 32 };

    private:

        // Index to the current spot in mElapsedTimeCount. Increment every tick
        int mCurrentArrayIndex;
        // Sum of all values in the mElapsedTimeCount array
        unsigned int mElapsedTimeSum;
        // Array holds the elapsed time counts for the past N updates
        SwapArray< unsigned int > mElapsedTimeCount;
    };


    struct MoodLighting
    {
        MoodLighting();
        ~MoodLighting();
        tLightGroup* mSunGroup;
        tColour      mSrcModulus;
        tColour      mDstModulus;
        tColour*     mOriginals;
        float        mTransition;
        int          mVolumeCount; // Count how many volumes of the same modulus we've entered.
        tColour     CalculateModulus( void );
    };
    MoodLighting mMood;
    void TransitionMoodLighting( unsigned int elapsedTime );
};

//
// A little syntactic sugar for getting at this singleton.
//
inline RenderManager* GetRenderManager() 
{ 
   return( RenderManager::GetInstance() ); 
}


inline int& RenderManager::rCurWorldRenderLayer()
{
    return mCurWorldLayer;
}


#endif
