//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        parkedcarmanager.cpp
//
// Description: Implement ParkedCarManager
//
// History:     2/6/2003 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <radmath/radmath.hpp>
#include <raddebug.hpp>
#include <string.h>
#include <stdlib.h>
#include <p3d/utility.hpp>
#include <pddi/pdditype.hpp>

//========================================
// Project Includes
//========================================
#include <worldsim/parkedcars/parkedcarmanager.h>
#include <worldsim/redbrick/geometryvehicle.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/character/character.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>

#include <memory/srrmemory.h>

#include <events/eventmanager.h>

#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/worldrenderlayer.h>

#include <meta/carstartlocator.h>

#include <main/commandlineoptions.h>

#include <mission/gameplaymanager.h>

#include <cheats/cheatinputsystem.h>

#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/utility/hudmap.h>

static const char FREE_CAR_SECTION[] = "FreeCar";

//TODO: SHOULD I USE VT_USER?

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

ParkedCarManager* ParkedCarManager::spInstance = NULL;

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// ParkedCarManager::GetInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      ParkedCarManager
//
//=============================================================================
ParkedCarManager& ParkedCarManager::GetInstance()
{
    if ( spInstance == NULL )
    {
        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
        spInstance = new ParkedCarManager();
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
    }

    return *spInstance;
}

//=============================================================================
// ParkedCarManager::DestroyInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ParkedCarManager::DestroyInstance()
{
    if ( spInstance != NULL )
    {
        delete spInstance;
        spInstance = NULL;
    }
}

//=============================================================================
// ParkedCarManager::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void ParkedCarManager::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
    case EVENT_DYNAMIC_ZONE_LOAD_ENDED:
        {
            //Now we've finished loading locators (maybe)
            //Let's choose some for this zone and reset the data.
            if ( mNumLocators > 0 )
            {
                //Choose a locator or two.
                unsigned int max = mNumCarTypes / MAX_ZONES;
                bool isOdd = mNumCarTypes % 2 == 1;
                if ( isOdd || mNumCarTypes < MAX_ZONES ) //This is an odd number
                {
                    max += 1;
                }

                //Allocate the max num of parked cars among these locators.
                unsigned int i;
                unsigned int allocated = 0;
                for ( i = 0; i < mNumLocators && i < max && allocated < mNumCarTypes - mNumParkedCars; ++i )
                {
                    unsigned int which = rand() % mNumLocators;
                    if ( !(mLocators[ which ]->GetFlag( Locator::ACTIVE )) )
                    {
                        //Find another one, this one is used.
                        unsigned int j;
                        for ( j = (which + 1) % mNumLocators; j != which; j = (j + 1) % mNumLocators )
                        {
                            if ( mLocators[ j ]->GetFlag( Locator::ACTIVE ) )
                            {
                                which = j;
                                break;
                            }
                        }
                    }

                    if ( CommandLineOptions::Get( CLO_PARKED_CAR_TEST ) || rand() % mNumCarTypes <= 2 )  //Weight it to placing the car.
                    {
                        //Mark the locator as used.

                        //Park this one!
                        Vehicle* car = NULL;
                        ParkedCarInfo* info = NULL;

                        //Find an available car
                        unsigned int j;
                        for ( j = 0; j < mNumCarTypes; ++j )
                        {
                            bool inUseByPlayer = false;
                            Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
                            inUseByPlayer = ( avatar && 
                                              avatar->GetCharacter()->IsInCar() &&
                                              avatar->GetVehicle() == mParkedCars[ j ].mCar );

                            if ( mParkedCars[ j ].mLoadedZoneUID == static_cast< tUID >( 0 ) && !inUseByPlayer )
                            {
                                //We found one!
                                info = &mParkedCars[ j ];
                                car = mParkedCars[ j ].mCar;
                                mParkedCars[ j ].mLoadedZoneUID = mLoadingZoneUID;
                                break;
                            }
                        }

                        //rAssert( car );  //If this fails then mNumParkedCars is invalid or there's mem stompage

                        if ( car )
                        {
                            //Assign a new random colour
                            pddiColour colour;
                            TrafficManager::GetInstance()->GenerateRandomColour( colour );
                            car->mGeometryVehicle->SetTrafficBodyColour( colour );

                            int added = GetVehicleCentral()->AddVehicleToActiveList( car );
                            rAssert( added != -1 );
                            //info->mActiveListIndex = added;

                            //Set the position and facing and reset.
                            rmt::Vector location;
                            mLocators[ which ]->GetLocation( &location );
                            car->SetInitialPositionGroundOffsetAutoAdjust( &location );

                            car->SetResetFacingInRadians( mLocators[ which ]->GetRotation() );

                            car->Reset();

                            car->GetSimState()->SetControl( sim::simSimulationCtrl );

                            //Turn off the headlights
                            car->mGeometryVehicle->EnableLights( false );

                            ++mNumParkedCars;
                            ++allocated;
                        }
                    }                
                }

                //Remove all the locators
                for ( i = 0; i < mNumLocators; ++i )
                {
                    p3d::inventory->Remove( mLocators[ i ] );
                }

                //Reset the data.
                mLoadingZoneUID = 0;
                mNumLocators = 0;
            }
            break;
        }
    case EVENT_DUMP_DYNA_SECTION:
        {
            //Make all the parked cars in the dynazone available to the new zone.
            tUID sectionNameUID = static_cast<tName*>(pEventData)->GetUID();
            unsigned int i;
            for ( i = 0; i < mNumCarTypes; ++i )
            {
                if ( mParkedCars[ i ].mLoadedZoneUID == sectionNameUID )
                {
                    //Make this puppy available.
                    mParkedCars[ i ].mLoadedZoneUID = 0;
                    mNumParkedCars--;

                    if( mParkedCars[ i ].mHusk )
                    {
                        // remove husk
                        bool succeeded = GetVehicleCentral()->RemoveVehicleFromActiveList( mParkedCars[ i ].mHusk );
                        rAssert( succeeded );

                        ::GetVehicleCentral()->mHuskPool.FreeHusk( mParkedCars[ i ].mHusk );
                        mParkedCars[ i ].mHusk->Release();
                        mParkedCars[ i ].mHusk = NULL;
                    }
                }
            }

            //Now test to see if we should dump the free car too.
            if (
                            (sectionNameUID == mFreeCar.mLoadedZoneUID || mFreeCar.mLoadedZoneUID == static_cast< tUID > ( 0 ) )
                    && 
                            (mFreeCar.mCar != NULL) 
               ) 
                
            {
                //Can we dump this car?
                bool dump = true;
                Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
                rAssert( avatar );
                
                if ( ( GetGameplayManager()->GetCurrentVehicle() == mFreeCar.mCar ) ||
                     ( GetVehicleCentral()->IsCarUnderConstruction(mFreeCar.mCar) == true ) )
                {
                    dump = false;
                }
                else
                {
                    //Either the guy's not in the car or he's in another car.
                    rmt::Vector charToCar;
                    rmt::Vector carPos, charPos;
                    avatar->GetPosition( charPos );
                    rTuneAssertMsg(mFreeCar.mCar != NULL,"Attempting to calculate distance from Null free car to Character, Crash Imminent\n");
                    mFreeCar.mCar->GetPosition( &carPos );

                    charToCar.Sub( carPos, charPos );

                    const float minDist = 200.0f * 200.0f;
                    if ( charToCar.MagnitudeSqr() > minDist )
                    {
                        dump = true;
                    }
                }

                if ( dump )
                {
                    RemoveFreeCar();
                    p3d::inventory->RemoveSectionElements( FREE_CAR_SECTION );
                }

                //Reset this as it is either a message that we can try to dump the car, 
                //or the car is already gone.
                mFreeCar.mLoadedZoneUID = 0;

                if ( mFreeCarLocator )
                {
                    mFreeCarLocator->Release();
                    mFreeCarLocator = NULL;
                }
            }
            break;
        }

    case EVENT_VEHICLE_DESTROYED:
        {
            Vehicle* v = (Vehicle*) pEventData;
            rAssert( v );

            // the destroyed vehicle could be a free "moment" car or 
            // a normal parked traffic car. We must search for both

            ParkedCarInfo* info = NULL;
            bool isFreeCar = false;
            FindParkedCarInfo( v, info, isFreeCar );

            // if we never found it, this event doesn't concern us
            if( info == NULL )
            {
                break;
            }
            // otherwise, it sure does...

            // First, grab some information about the car
            rmt::Vector initPos, initDir;
            v->GetPosition( &initPos );
            initDir = v->GetFacing();
            rAssert( rmt::Epsilon( initDir.MagnitudeSqr(), 1.0f, 0.001f ) );

            // Now remove the old car from the activelist.. 
            bool succeeded = GetVehicleCentral()->RemoveVehicleFromActiveList( v );
            rAssert( succeeded );
            //GetVehicleCentral()->SetVehicleController( info->mActiveListIndex, NULL );
            //info->mActiveListIndex = -1;

            tUID zoneUID = info->mLoadedZoneUID;
            info->mLoadedZoneUID = 0;
            if( !isFreeCar )
            {
                mNumParkedCars--;
            }

            // Now grab a husk (if available), add it to the activelist, 
            // and place it exactly where theother car is...

            Vehicle* husk = NULL;

            // since we are just now destroying this car, it shouldn't have 
            // a husk yet.
            rAssert( info->mHusk == NULL );
            if( info->mHusk )
            {
                // weird! we shouldn't be here.. but we'll handle it gracefully
                // otherwise we'll leak.
                husk = info->mHusk; 
            }
            else
            {
                husk = GetVehicleCentral()->mHuskPool.RequestHusk( VT_TRAFFIC, v );
                if( husk )
                {
                    husk->AddRef();
                }
            }

            if( husk )
            {
                int res = GetVehicleCentral()->AddVehicleToActiveList( husk );
                if( res == -1 )
                {
                    // not supposed to happen since the list can't be full!!!
                    // we TOOK something out of the list before adding something in
                    // If this assert happens, it is both fatal and strange
                    rAssert( false );
                    break;
                }
                //info->mActiveListIndex = res;

                husk->SetInitialPosition( &initPos );
                float angle = GetRotationAboutY( initDir.x, initDir.z );
                husk->SetResetFacingInRadians( angle );
                husk->Reset();
                husk->SetLocomotion( VL_PHYSICS );
                
                info->mHusk = husk;
                info->mLoadedZoneUID = zoneUID;

                // add back the numparkedcars
                mNumParkedCars++;
            }
        }
        break;

    case EVENT_REPAIR_CAR:
        {
            // NOTE: When we want to support other cars than the user car 
            // picking up Wrench/repair icons in the main game, we should pass in
            // the vehicle pointer when we trigger this event.
            //
            Vehicle* v = GetGameplayManager()->GetCurrentVehicle();
            if( v == NULL )
            {
                return; // no current vehicle to repair.. oh well...
            }

            ParkedCarInfo* info = NULL;
            bool isFreeCar = false;
            FindParkedCarInfo( v, info, isFreeCar );

            if( info == NULL )
            {
                return;
            }

            // if the vehicle is a husk.. gotta replace husk with original vehicle
            if( v->mVehicleDestroyed )
            {                
                if( info->mHusk )
                {
                    // Destroyed, but has a husk
                    // Husk had to have existed to get to this point
                    Vehicle* husk = info->mHusk;

                    // obtain info from the husk
                    rmt::Vector initPos, initDir;
                    husk->GetPosition( &initPos );
                    initDir = husk->GetFacing();
                    rAssert( rmt::Epsilon( initDir.MagnitudeSqr(), 1.0f, 0.001f ) );

                    // remove husk
                    bool succeeded = GetVehicleCentral()->RemoveVehicleFromActiveList( husk );
                    rAssert( succeeded );
                    //GetVehicleCentral()->SetVehicleController( info->mActiveListIndex, NULL );
                    //info->mActiveListIndex = -1;

                    ::GetVehicleCentral()->mHuskPool.FreeHusk( husk );
                    husk->Release();
                    husk = NULL;
                    info->mHusk = NULL;

                    v->SetInitialPosition( &initPos );
                    float angle = GetRotationAboutY( initDir.x, initDir.z );
                    v->SetResetFacingInRadians( angle );
                    v->Reset();

                }

                // put in original vehicle
                int res = GetVehicleCentral()->AddVehicleToActiveList( v );
                if( res == -1 )
                {
                    // not supposed to happen since the list can't be full!!!
                    // we TOOK something out of the list before adding something in
                    // If this assert happens, it is both fatal and strange
                    rAssert( false );
                    return;
                }
                //info->mActiveListIndex = res;

                // if the avatar is inside a vehicle the vehicle
                // is probably a husk, update this vehicle to be the original 
                // vehicle and place the character in this new vehicle
                // 
                Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
                rAssert( avatar );
                if( avatar->IsInCar() )
                {
                    rAssert( avatar->GetVehicle() );
                    rAssert( GetVehicleCentral()->mHuskPool.IsHuskType( avatar->GetVehicle()->mVehicleID ) );

                    avatar->SetVehicle( v );

                    Character* character = avatar->GetCharacter();
                    GetAvatarManager()->PutCharacterInCar( character, v );
                }

                // fire off event so Esan can know when we switch the vehicle on him.
                GetEventManager()->TriggerEvent(
                    EVENT_VEHICLE_DESTROYED_SYNC_SOUND, v );

            }
            // repair any damage to original vehicle
            bool resetDamage = true;
            v->ResetFlagsOnly( resetDamage );
        }
        break;

    default:
        {
            rAssert( false ); //Why?
            break;
        }
    }
}

void ParkedCarManager::FindParkedCarInfo( Vehicle* v, ParkedCarInfo*& info, bool& isFreeCar )
{
    info = NULL;
    isFreeCar = false;

    if( mFreeCar.mCar == v )
    {
        info = &mFreeCar;
        isFreeCar = true;
    }
    if( info == NULL )
    {
        for( unsigned int i=0; i<mNumCarTypes; i++ )
        {
            if( mParkedCars[i].mCar == v )
            {
                // found it
                info = &(mParkedCars[i]);
                break;
            }
        }
    }
}
    


//=============================================================================
// ParkedCarManager::OnProcessRequestsComplete
//=============================================================================
// Description: Comment
//
// Parameters:  ( void* pUserData )
//
// Return:      void 
//
//=============================================================================
void ParkedCarManager::OnProcessRequestsComplete( void* pUserData )
{
    if ( mFreeCar.mLoadedZoneUID != static_cast< tUID >( 0 ) )
    {
        //Create the car.
        CreateFreeCar();
    }
    else
    {
        //Throw this data away...
        rAssert( false );

        //Fragment.
        p3d::inventory->RemoveSectionElements( FREE_CAR_SECTION );
    }
}


//=============================================================================
// ParkedCarManager::AddCarType
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
void ParkedCarManager::AddCarType( const char* name )
{
    //return;

#ifdef RAD_DEBUG
    if ( CommandLineOptions::Get( CLO_PARKED_CAR_TEST ) )
    {
        rAssert( mNumCarTypes < NUM_TEST_PARKED_CARS );
    }
    else
    {
        rAssert( mNumCarTypes < MAX_DIFFERENT_CARS );
    }
#endif

    rAssert( strlen( name ) < MAX_CAR_NAME_LEN );

    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );

    unsigned int i;
    unsigned int iterations = CommandLineOptions::Get( CLO_PARKED_CAR_TEST ) ? NUM_TEST_PARKED_CARS / MAX_DIFFERENT_CARS : 1;
    for ( i = 0; i < iterations; ++i )
    {
        //Set the name.
        unsigned int nameLen = strlen( name ) > MAX_CAR_NAME_LEN ? MAX_CAR_NAME_LEN : strlen( name );
        strncpy( mParkedCars[ mNumCarTypes ].mName, name, nameLen );
        mParkedCars[ mNumCarTypes ].mName[ nameLen ] = '\0';

        mParkedCars[ mNumCarTypes ].mLoadedZoneUID = 0;

        Vehicle* car = GetVehicleCentral()->InitVehicle( mParkedCars[ mNumCarTypes ].mName, false, NULL, VT_TRAFFIC );  //TODO: Should I change the NULL to a con file?
        rAssert( car );
        car->mCreatedByParkedCarManager = true;

        car->AddRef();

        if ( car )
        {
            mParkedCars[ mNumCarTypes ].mCar = car;
            ++mNumCarTypes;
        }
    }

    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
}

//=============================================================================
// ParkedCarManager::AddLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ( CarStartLocator* loc )
//
// Return:      void 
//
//=============================================================================
void ParkedCarManager::AddLocator( CarStartLocator* loc )
{
    if(!mParkedCarsEnabled)
    {
        return;
    }
    
    //return;
    rAssert( mNumLocators < MAX_LOCATORS_PER_ZONE );

    //Adding locators from the same dyna zone.
    if ( mNumLocators < MAX_LOCATORS_PER_ZONE )
    {
        mLocators[ mNumLocators ] = loc;
        ++mNumLocators;
    }

    if ( mLoadingZoneUID == static_cast< tUID >( 0 ) )
    {
        //Set the zone ID
        mLoadingZoneUID = GetRenderManager()->pWorldRenderLayer()->GetCurSectionName().GetUID();
    }
}

//=============================================================================
// ParkedCarManager::AddFreeCar
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name, CarStartLocator* loc )
//
// Return:      void 
//
//=============================================================================
void ParkedCarManager::AddFreeCar( const char* name, CarStartLocator* loc )
{
    if ( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_REDBRICK ) )
    {
        //This Bud's for you.
        name = "redbrick";
    }

    if(!mParkedCarsEnabled)
    {
        return;
    }
    
    if ( mFreeCar.mCar == NULL )
    {
        unsigned int nameLen = strlen( name ) > MAX_CAR_NAME_LEN ? MAX_CAR_NAME_LEN : strlen( name );
        strncpy( mFreeCar.mName, name, nameLen );
        mFreeCar.mName[ nameLen ] = '\0';

        mFreeCar.mLoadedZoneUID = GetRenderManager()->pWorldRenderLayer()->GetCurSectionName().GetUID();
        rAssert( mFreeCar.mLoadedZoneUID != static_cast< tUID >( 0 ) );

        tRefCounted::Assign( mFreeCarLocator, loc );

        //Try to load the car
        char fileName[MAX_CAR_NAME_LEN + 5 + 32];
        sprintf( fileName, "art\\cars\\%s.p3d", name );
        GetLoadingManager()->AddRequest( FILEHANDLER_LEVEL, fileName, GMA_LEVEL_OTHER, FREE_CAR_SECTION, FREE_CAR_SECTION, this );
    }
}

//=============================================================================
// ParkedCarManager::RemoveFreeCar
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ParkedCarManager::RemoveFreeCar()
{
    //TODO:  Do I need this?  What if I make sure the car is not in the view frustrum?
    //p3d::pddi->DrawSync();
    if ( mFreeCar.mCar != NULL )
    {
        if( mFreeCar.mHusk == NULL )
        {
            GetVehicleCentral()->RemoveVehicleFromActiveList( mFreeCar.mCar );
        }
        else
        {
            GetVehicleCentral()->RemoveVehicleFromActiveList( mFreeCar.mHusk );

            GetVehicleCentral()->mHuskPool.FreeHusk( mFreeCar.mHusk );
            mFreeCar.mHusk->Release();
            mFreeCar.mHusk = NULL;
        }
        /*
        if( mFreeCar.mActiveListIndex != -1 )
        {
            GetVehicleCentral()->SetVehicleController( mFreeCar.mActiveListIndex, NULL );
        }
        mFreeCar.mActiveListIndex = -1;
        */

        CGuiScreenHud* currentHud = GetCurrentHud();
        if( currentHud != NULL )
        {
            HudMapIcon* playerCarIcon = currentHud->GetHudMap( 0 )->FindIcon( HudMapIcon::ICON_PLAYER_CAR );
            if( playerCarIcon != NULL && playerCarIcon->m_dynamicLocator == mFreeCar.mCar )
            {
                GetGameplayManager()->UnregisterVehicleHUDIcon();
            }
        }

        mFreeCar.mCar->Release();
        mFreeCar.mCar = NULL;
        mFreeCar.mLoadedZoneUID = 0;
    }

    if ( mFreeCarLocator )
    {
        mFreeCarLocator->Release();
        mFreeCarLocator = NULL;
    }   
}

void ParkedCarManager::RemoveFreeCarIfClose( const rmt::Vector& position )
{
    // where is the free car?
    rmt::Vector freeCarPosition;
    if ( mFreeCar.mCar != NULL )
    {
        if(mFreeCar.mCar == GetAvatarManager()->GetAvatarForPlayer(0)->GetVehicle())
        {
            return;
        }
    
        mFreeCar.mCar->GetPosition( &freeCarPosition );
    }
    if( ( position - freeCarPosition ).MagnitudeSqr() < 10.0 * 10.0 )
    {
        RemoveFreeCar();
    }
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************

//=============================================================================
// ParkedCarManager::ParkedCarManager
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
ParkedCarManager::ParkedCarManager() :
    mNumCarTypes( 0 ),
    mNumParkedCars( 0 ),
    mNumLocators( 0 ),
    mLoadingZoneUID( 0 ),
    mFreeCarLocator( NULL )
{
    GetEventManager()->AddListener( this, EVENT_DUMP_DYNA_SECTION );
    GetEventManager()->AddListener( this, EVENT_DYNAMIC_ZONE_LOAD_ENDED );
    GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED );
    GetEventManager()->AddListener( this, EVENT_REPAIR_CAR );

    if ( CommandLineOptions::Get( CLO_PARKED_CAR_TEST ) )
    {
        mParkedCars = new ParkedCarInfo[ NUM_TEST_PARKED_CARS ];
    }
    else
    {
        mParkedCars = new ParkedCarInfo[ MAX_DIFFERENT_CARS ];
    }

    //We want an inventory section for the car
    p3d::inventory->AddSection( FREE_CAR_SECTION );
    
    mParkedCarsEnabled = true;
    
}

//=============================================================================
// ParkedCarManager::~ParkedCarManager
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
ParkedCarManager::~ParkedCarManager()
{
    GetEventManager()->RemoveAll( this );

    //Remove all the car types
    MDKParkedCars();
    delete[] mParkedCars;

    //Clear out leftover locators.
    for( unsigned int i = 0; i < mNumLocators; ++i )
    {
        p3d::inventory->Remove( mLocators[ i ] );
    }

    RemoveFreeCar();

    if ( mFreeCarLocator )
    {
        p3d::inventory->Remove( mFreeCarLocator );

        mFreeCarLocator->Release();
        mFreeCarLocator = NULL;
    }

    p3d::inventory->RemoveSectionElements( FREE_CAR_SECTION );
    p3d::inventory->DeleteSection( FREE_CAR_SECTION );

}

//=============================================================================
// ParkedCarManager::CreateFreeCar
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void ParkedCarManager::CreateFreeCar()
{
    rAssert( mFreeCarLocator );

    Vehicle* car = GetVehicleCentral()->InitVehicle( mFreeCar.mName, true, NULL, VT_TRAFFIC );  //TODO: Should I change the NULL to a con file?
    rAssert( car );
    car->mCreatedByParkedCarManager = true;
    
    car->AddRef();


    //Set the position and facing and reset.
    rmt::Vector location;
    mFreeCarLocator->GetLocation( &location );
    car->SetInitialPositionGroundOffsetAutoAdjust( &location );

    car->SetResetFacingInRadians( mFreeCarLocator->GetRotation() );

    car->Reset();


    int added = GetVehicleCentral()->AddVehicleToActiveList( car );
    rAssert( added != -1 );
    //mFreeCar.mActiveListIndex = added;


    //Turn off the lights.
    car->mGeometryVehicle->EnableLights( false );

    mFreeCar.mCar = car;

    //Throw away the locator since we'll not need it again and we'll prevent the little memory block.
    mFreeCarLocator->Release();
    mFreeCarLocator = NULL;
}

//=============================================================================
// ParkedCarManager::MDKParkCar
//=============================================================================
// Description: Deletes the park car instances , so that the art can be removed from the inventory.
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================


void ParkedCarManager::MDKParkedCars()
{
    //Remove all the car types
    for( unsigned int i = 0; i < mNumCarTypes; ++i )
    {
        rAssert( mParkedCars[ i ].mCar != NULL );

        if( mParkedCars[ i ].mHusk == NULL )
        {
            GetVehicleCentral()->RemoveVehicleFromActiveList( mParkedCars[ i ].mCar );
        }
        else
        {
            GetVehicleCentral()->RemoveVehicleFromActiveList( mParkedCars[ i ].mHusk );

            GetVehicleCentral()->mHuskPool.FreeHusk( mParkedCars[ i ].mHusk );
            mParkedCars[ i ].mHusk->Release();
            mParkedCars[ i ].mHusk = NULL;

        }
        /*
        if( mParkedCars[ i ].mActiveListIndex != -1 )
        {
            GetVehicleCentral()->SetVehicleController( mParkedCars[ i ].mActiveListIndex, NULL );
        }
        mParkedCars[ i ].mActiveListIndex = -1;
        */
        mParkedCars[ i ].mCar->Release();
        mParkedCars[ i ].mCar = NULL;
        mParkedCars[ i ].mLoadedZoneUID = 0;
    }

    mNumParkedCars = 0;

}
