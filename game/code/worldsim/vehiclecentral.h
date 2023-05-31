//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehiclecentral.h
//
// Description: holds a list of all the vehicles in the game and provides 
//              easy access for the rest of the game objects that care
//
// History:     May 1, 2002 - created, gmayer
//
//=============================================================================

#ifndef VEHICLECENTRAL_H
#define VEHICLECENTRAL_H

//========================================
// Nested Includes
//========================================
#include <p3d/p3dtypes.hpp>

#include <loading/loadingmanager.h>

#include <worldsim/redbrick/vehicle.h>
#include <worldsim/huskpool.h>



//========================================
// Forward References
//========================================
class VehicleAI;
class AiVehicleController;
class VehicleController;
class EventLocator;
class tBillboardQuadGroup;

//=============================================================================
//
// Synopsis:    they lived happily ever after....
//
//=============================================================================



class VehicleCentral : public LoadingManager::ProcessRequestsCallback
{
    public:
        enum DriverInit
        {
            ALLOW_DRIVER,
            FORCE_NO_DRIVER,
            FORCE_DRIVER
        };

        // Static Methods for accessing this singleton.
        static VehicleCentral* GetInstance();
        static VehicleCentral* CreateInstance();
        static void DestroyInstance();

        void PreLoad();
        void Unload();
        
        Vehicle* InitVehicle( const char* name, bool addToActiveVehicleList = true, char* confile = 0, VehicleType vt = VT_USER, 
                              DriverInit s = ALLOW_DRIVER, bool playercar = false, bool startoutofcar = true);
        
        // returns index if successful, otherwise -1
        int AddVehicleToActiveList(Vehicle* vehicle);
        // also make this put vehicles in the dsg.
        //
        // this is also where the vehicle should ask for a collision index, and insert itself

        // returns whether or not it was even there in the first place
        bool RemoveVehicleFromActiveList(Vehicle* vehicle);              
        // remove from dsg

        //Triage hack, only for demo mode, or until Greg actually 
        //addrefs and releases --dm 12/01/02
        VehicleController* RemoveVehicleController( int mAIIndex );

        void KillEmAll();

        void ClearSpot(rmt::Vector& point, float radius, Vehicle* skipCar);

        //void Suspend(); // temporarily freeze all action - leave vehicles in the active list
        //void Resume();  // unfreeze

        int GetNumVehicles() const { return mNumActiveVehicles; }
        Vehicle* GetVehicle( int id ) const;
        
        void SetupConsoleFunctionsForVehicleTuning();

        void SetVehicleController( int id, VehicleController* pVehicleController );
        VehicleController* GetVehicleController( int id ) const;
        
        int GetVehicleId( Vehicle* pVehicle, bool checkStrict = true ) const;
        
        void SubmitStatics();
        void SubmitDynamics();
        void SubmitAnimCollisions();


        void PreSubstepUpdate(float dt);
        void PostSubstepUpdate(float dt);
        void Update(float dt);
        void PreCollisionPrep(float dt, bool firstSubstep);

        enum { MAX_ACTIVE_VEHICLES = 30 };

        static int GetMaxActiveVehicles() {return MAX_ACTIVE_VEHICLES;}

        // *** //
        void GetActiveVehicleList(Vehicle** &vList, int& nVehicles);
        bool ActiveVehicleListIsFull() const;
        // *** //

        // hmmm... is there a nicer way to do this?
        // need this so we have object to call script hooks on
        Vehicle* mVehicleUnderConstruction[MAX_ACTIVE_VEHICLES];
        unsigned int mCurrentVehicleUnderContructionHead;
        unsigned int mCurrentVehicleUnderConstructionTail;
        Vehicle* GetCurrentVehicleUnderConstruction() { return mVehicleUnderConstruction[ mCurrentVehicleUnderContructionHead ]; };
        void OnProcessRequestsComplete( void* pUserData );

    
        Vehicle* GetVehicleByName( const char* name ) const;
        Vehicle* GetVehicleByUID( tUID uid ) const;

        void ActivateVehicleTriggers(bool);
        
        HuskPool mHuskPool; // just use default constructor

        void InitHuskPool();
        void FreeHuskPool();
             
        void ClearSuppressedDrivers(void);
        void AddSuppressedDriver(const char* name);
        void RemoveSuppressedDriver(const char* name);
        bool IsDriverSuppressed(const char* name);

        void SetupDriver(Vehicle*);

        //
        bool IsCarUnderConstruction(const char* name); 
        bool IsCarUnderConstruction(const Vehicle* vehicle); 

        bool GetVehicleTriggersActive(void) {return mbVehicleTriggersActive;}

        VehicleAI* GetVehicleAI( Vehicle* vehicle );

        // Removes all collectibles attached to various vehicles
        void DetachAllCollectibles();

        // store the headlights here!
        enum 
        {
            NUM_HEADLIGHT_BBQGS = 3,
            NUM_HEADLIGHT_BBQS = 7 // combined total of BBQs of all headlight BBQGs
        };
        tBillboardQuadGroup* mHeadLights[NUM_HEADLIGHT_BBQGS];
        tColour mOriginalHeadLightColours[NUM_HEADLIGHT_BBQS]; 

    private:

        // No public access to these, use singleton interface.
        VehicleCentral();
        ~VehicleCentral();


        // pointer to the single instance
        static VehicleCentral* spInstance;

        Vehicle* mActiveVehicleList[MAX_ACTIVE_VEHICLES];
        VehicleController* mActiveVehicleControllerList[ MAX_ACTIVE_VEHICLES ];
        //EventLocator* mDoorTriggerList[ MAX_ACTIVE_VEHICLES ];
        int mNumActiveVehicles;

        //bool mSuspended;

        bool mbVehicleTriggersActive;

        static const unsigned int MAX_SuppressED_DRIVERS = 32;
        unsigned int mSuppressedDriverCount;
        tName mSuppressedDrivers[MAX_SuppressED_DRIVERS];

        static AiVehicleController* spGenericAI;
};

// A little syntactic sugar for getting at this singleton.
inline VehicleCentral* GetVehicleCentral() { return( VehicleCentral::GetInstance() ); }



// *** //
inline void VehicleCentral::GetActiveVehicleList(Vehicle** &vList, int& nVehicles)
{
    vList = mActiveVehicleList;
    nVehicles = mNumActiveVehicles;
}
inline bool VehicleCentral::ActiveVehicleListIsFull() const
{
    return (mNumActiveVehicles>=MAX_ACTIVE_VEHICLES);
}
// *** //
                   
#endif //VEHICLECENTRAL_H
