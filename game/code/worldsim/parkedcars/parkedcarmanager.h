//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        parkedcarmanager.h
//
// Description: Blahblahblah
//
// History:     2/6/2003 + Created -- NAME
//
//=============================================================================

#ifndef PARKEDCARMANAGER_H
#define PARKEDCARMANAGER_H

//========================================
// Nested Includes
//========================================
#include <p3d/p3dtypes.hpp>

#include <events/eventlistener.h>

#include <worldsim/vehiclecentral.h>

#include <loading/loadingmanager.h>

//========================================
// Forward References
//========================================
class Vehicle;
class CarStartLocator;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class ParkedCarManager : public EventListener, public LoadingManager::ProcessRequestsCallback
{
public:
    enum 
    { 
        MAX_DIFFERENT_CARS = 5,
        MAX_CAR_NAME_LEN = 32,
        MAX_LOCATORS_PER_ZONE = 50,
        MAX_ZONES = 3,
        NUM_TEST_PARKED_CARS = VehicleCentral::MAX_ACTIVE_VEHICLES - 6
    };

    static ParkedCarManager& GetInstance();
    static void DestroyInstance();

    //From EventListener
    virtual void HandleEvent( EventEnum id, void* pEventData );

    //From LoadingManager
    virtual void OnProcessRequestsComplete( void* pUserData );   

    //Local
    void AddCarType( const char* name );
    void AddLocator( CarStartLocator* loc );

    void AddFreeCar( const char* name, CarStartLocator* loc );
    void RemoveFreeCar();
    void RemoveFreeCarIfClose( const rmt::Vector& position );
    
    //Chuck: release the parkcar instances so Gameplaymanager can free up the art
    void MDKParkedCars();

    void EnableParkedCars() {mParkedCarsEnabled = true;}
    void DisableParkedCars() {mParkedCarsEnabled = false;}

private:
    static ParkedCarManager* spInstance;

    struct ParkedCarInfo
    {
        ParkedCarInfo() : 
            mCar( NULL ), 
            mHusk( NULL ),
            mLoadedZoneUID( 0 )
            //mActiveListIndex( -1 ) 
        { mName[0] ='\0'; };

        char mName[ MAX_CAR_NAME_LEN + 1];
        Vehicle* mCar;
        Vehicle* mHusk;
        tUID mLoadedZoneUID;
        //int mActiveListIndex; 
    };
    
    //Where all the parked cars are
    ParkedCarInfo* mParkedCars;
    unsigned int mNumCarTypes;
    unsigned int mNumParkedCars;

    //For loading locators, we store all the locators loaded while a given UID is active.
    CarStartLocator* mLocators[ MAX_LOCATORS_PER_ZONE ];
    unsigned int mNumLocators;
    tUID mLoadingZoneUID;

    //Free "Moment" cars
    ParkedCarInfo mFreeCar;
    CarStartLocator* mFreeCarLocator;

    ParkedCarManager();
    virtual ~ParkedCarManager();

    void CreateFreeCar();

    void FindParkedCarInfo( Vehicle* v, ParkedCarInfo*& info, bool& isFreeCar );

    bool mParkedCarsEnabled;   // little bool to disable these during street races

    //Prevent wasteful constructor creation.
    ParkedCarManager( const ParkedCarManager& parkedcarmanager );
    ParkedCarManager& operator=( const ParkedCarManager& parkedcarmanager );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************
inline ParkedCarManager& GetPCM() { return ParkedCarManager::GetInstance(); };

#endif //PARKEDCARMANAGER_H
