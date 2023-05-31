//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehiclecentral.cpp
//
// Description: bleek
//
// History:     May 1, 2002 - created, gmayer
//
//=============================================================================

//========================================
// System Includes
//========================================

#include <raddebug.hpp>

#include <stdlib.h> // for atof

//========================================
// Project Includes
//========================================

#include <worldsim/worldphysicsmanager.h>
#include <worldsim/vehiclecentral.h>

#include <worldsim/redbrick/vehiclecontroller/vehiclecontroller.h>

#include <worldsim/character/character.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/avatarmanager.h>

#include <ai/actionbuttonhandler.h>
#include <ai/actionbuttonmanager.h>

#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>

#include <meta/eventlocator.h>
#include <meta/spheretriggervolume.h>
#include <meta/triggervolumetracker.h>

#include <debug/debuginfo.h>

#include <mission/gameplaymanager.h>
#include <mission/missionscriptloader.h>

#include <render/RenderManager/RenderManager.h>
#include <render/Culling/WorldScene.h>

#include <gameflow/gameflow.h>
#include <console/console.h>
#include <cheats/cheatinputsystem.h>

#include <p3d/billboardobject.hpp>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

VehicleCentral* VehicleCentral::spInstance = 0;
AiVehicleController* VehicleCentral::spGenericAI = NULL;

//
// Dusit here: 
// First, keep MAX_HUSKS defined in the .cpp so we can tweak without 
// recompiling the whole game... 
// Now, how many husks can we possibly need in the WORST case??
//  traffic 5
//  parked cars 5
//  harass 5
//  player 1
//  mission ai 4
//  main ai 1
//  free (moment) cars 1-2 (?)
// This is too many (23). At 17KB per Vehicle instance + 35KB Husk model, 
// it would total between 420 and 500 KB... Unacceptable.
// So we have forced the other systems to deal safely with not having 
// husks available when they need it (RequestHusk may return NULL)
// 
const int MAX_HUSKS = 5;


//=============================================================================
// VehicleCentral::VehicleCentral
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      VehicleCentral
//
//=============================================================================
VehicleCentral::VehicleCentral() :
    mCurrentVehicleUnderContructionHead( 0 ),
    mCurrentVehicleUnderConstructionTail( 0 ),
    mbVehicleTriggersActive(true),
    mSuppressedDriverCount(0)
{
    spGenericAI = new AiVehicleController( NULL );
    spGenericAI->AddRef();

    int i;
    for(i = 0; i < MAX_ACTIVE_VEHICLES; i++)
    {
        mActiveVehicleList[i] = 0;

        //mActiveVehicleControllerList[ i ] = NULL;
        mActiveVehicleControllerList[ i ] = spGenericAI;
        mActiveVehicleControllerList[ i ]->AddRef();

        mVehicleUnderConstruction[ i ] = NULL;
        //mDoorTriggerList[ i ] = 0;
    }
    
    mNumActiveVehicles = 0;

    // perhaps this would be a good place to setup the console functions for the vehicle
    SetupConsoleFunctionsForVehicleTuning();

    for( i=0; i<NUM_HEADLIGHT_BBQGS; i++ )
    {
        mHeadLights[i] = NULL;
    }
}

//==============
// console hooks
//==============

static void ConsoleHookSetGasScale(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetGasScale(value);
}

static void ConsoleHookSetSlipGasScale(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetSlipGasScale(value);
}

static void ConsoleHookSetHighSpeedGasScale(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetHighSpeedGasScale(value);
}


static void ConsoleHookSetGasScaleSpeedThreshold(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetGasScaleSpeedThreshold(value);
}
        

static void ConsoleHookSetBrakeScale(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetBrakeScale(value);
}

static void ConsoleHookSetTopSpeedKmh(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetTopSpeedKmh(value);
}

static void ConsoleHookSetMass(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetMass(value);
}

static void ConsoleHookSetMaxWheelTurnAngle(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetMaxWheelTurnAngle(value);
}


static void ConsoleHookSetHighSpeedSteeringDrop(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetHighSpeedSteeringDrop(value);
}


static void ConsoleHookSetTireLateralStaticGrip(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetTireLateralStaticGrip(value);
}


static void ConsoleHookSetTireLateralResistanceNormal(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetTireLateralResistanceNormal(value);
}


static void ConsoleHookSetTireLateralResistanceSlip(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetTireLateralResistanceSlip(value);
}


static void ConsoleHookSetEBrakeEffect(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetEBrakeEffect(value);
}


static void ConsoleHookSetCMOffsetX(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetCMOffsetX(value);
}


static void ConsoleHookSetCMOffsetY(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetCMOffsetY(value);
}


static void ConsoleHookSetCMOffsetZ(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetCMOffsetZ(value);
}


static void ConsoleHookSetSuspensionLimit(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetSuspensionLimit(value);
}


static void ConsoleHookSetSpringK(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetSuspensionSpringK(value);
}


static void ConsoleHookSetDamperC(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetSuspensionDamperC(value);
}


static void ConsoleHookSetHitPoints(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetHitPoints(value);
}


static void ConsoleHookSetSuspensionYOffset(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetSuspensionYOffset(value);
}

static void ConsoleHookSetBurnoutRange(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetBurnoutRange(value);
}


static void ConsoleHookSetMaxSpeedBurstTime(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetMaxSpeedBurstTime(value);
}


static void ConsoleHookSetDonutTorque(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetDonutTorque(value);
}


static void ConsoleHookSetSlipSteeringNoEBrake(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetTireLateralResistanceSlipWithoutEBrake(value);
}

static void ConsoleHookSetSlipEffectNoEBrake(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetSlipEffectWithoutEBrake(value);
}


static void ConsoleHookSetWeebleOffset(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetWeebleOffset(value);
}


static void ConsoleHookSetWheelieRange(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetWheelieRange(value);
}


static void ConsoleHookSetWheelieOffsetY(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetWheelieYOffset(value);
}

static void ConsoleHookSetWheelieOffsetZ(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetWheelieZOffset(value);
}

static void ConsoleHookSetShininess( int argc, char** argv )
{
    if( argc != 2 )
    {
        return;
    }
    float shininess = static_cast<float>( atof( argv[ 1 ] ) );
    unsigned char ref = rmt::Clamp( int( 0xff * shininess ), 0, 0xff );
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetShininess( ref );
}

static void ConsoleHookSetShadowAdjustments( int argc, char** argv )
{
    if( argc != 9 )
    {
        return;
    }
    float Adjustments[ 4 ][ 2 ];
    for( int i = 0; i < 4; ++i )
    {
        Adjustments[ i ][ 0 ] = static_cast<float>( atof( argv[ 1 + ( i * 2 ) ] ) );
        Adjustments[ i ][ 1 ] = static_cast<float>( atof( argv[ 2 + ( i * 2 ) ] ) );
    }
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetShadowAdjustments( Adjustments );
}

static void ConsoleHookSetDriverName(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetDriverName(argv[1]);
}

static void ConsoleHookSetHasDoors(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->mHasDoors = (atoi(argv[1]) != 0);
}

static void ConsoleHookSetCharactersVisible(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->mVisibleCharacters = (atoi(argv[1]) != 0);
}

static void ConsoleHookSetIrisTransition(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->mIrisTransition = (atoi(argv[1]) != 0);
}

static void ConsoleHookSetAllowSlide(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->mAllowSlide = (atoi(argv[1]) != 0);
}

static void ConsoleHookSetHighRoof(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->mHighRoof = (atoi(argv[1]) != 0);
}

static void ConsoleHookSetCharacterScale(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->mCharacterScale = static_cast< float >( atof(argv[1]) );
}

//Chuck: added this for Gambling Races.
static void ConsoleHookSetGamblingOdds(int argc, char** argv)
{
    if(argc != 2)
    {
        return;
    }
    float value = static_cast<float>(atof(argv[1]));    
    GetVehicleCentral()->GetCurrentVehicleUnderConstruction()->SetGamblingOdds(value);
}

static void ConsoleHookSuppressDriver(int argc, char** argv)
{
    GetVehicleCentral()->AddSuppressedDriver(argv[1]);
}


//=============================================================================
// VehicleCentral::SetupConsoleFunctionsForVehicleTuning
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::SetupConsoleFunctionsForVehicleTuning()
{

    GetConsole()->AddFunction("SetMass", ConsoleHookSetMass, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetGasScale", ConsoleHookSetGasScale, "help your goddamn self", 1, 1);
    
    GetConsole()->AddFunction("SetHighSpeedGasScale", ConsoleHookSetHighSpeedGasScale, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetGasScaleSpeedThreshold", ConsoleHookSetGasScaleSpeedThreshold, "help your goddamn self", 1, 1);
    
    
    GetConsole()->AddFunction("SetSlipGasScale", ConsoleHookSetSlipGasScale, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetBrakeScale", ConsoleHookSetBrakeScale, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetTopSpeedKmh", ConsoleHookSetTopSpeedKmh, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetMaxWheelTurnAngle", ConsoleHookSetMaxWheelTurnAngle, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetHighSpeedSteeringDrop", ConsoleHookSetHighSpeedSteeringDrop, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetTireGrip", ConsoleHookSetTireLateralStaticGrip, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetNormalSteering", ConsoleHookSetTireLateralResistanceNormal, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetSlipSteering", ConsoleHookSetTireLateralResistanceSlip, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetEBrakeEffect", ConsoleHookSetEBrakeEffect, "help your goddamn self", 1, 1);

    GetConsole()->AddFunction("SetCMOffsetX", ConsoleHookSetCMOffsetX, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetCMOffsetY", ConsoleHookSetCMOffsetY, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetCMOffsetZ", ConsoleHookSetCMOffsetZ, "help your goddamn self", 1, 1);

    GetConsole()->AddFunction("SetSuspensionLimit", ConsoleHookSetSuspensionLimit, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetSpringK", ConsoleHookSetSpringK, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetDamperC", ConsoleHookSetDamperC, "help your goddamn self", 1, 1);

    GetConsole()->AddFunction("SetSuspensionYOffset", ConsoleHookSetSuspensionYOffset, "help your goddamn self", 1, 1);

    GetConsole()->AddFunction("SetHitPoints", ConsoleHookSetHitPoints, "help your goddamn self", 1, 1);

    GetConsole()->AddFunction("SetBurnoutRange", ConsoleHookSetBurnoutRange, "help your goddamn self", 1, 1);

    GetConsole()->AddFunction("SetMaxSpeedBurstTime", ConsoleHookSetMaxSpeedBurstTime, "help your goddamn self", 1, 1);

    GetConsole()->AddFunction("SetDonutTorque", ConsoleHookSetDonutTorque, "help your goddamn self", 1, 1);

    GetConsole()->AddFunction("SetSlipSteeringNoEBrake", ConsoleHookSetSlipSteeringNoEBrake, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetSlipEffectNoEBrake", ConsoleHookSetSlipEffectNoEBrake, "help your goddamn self", 1, 1);

    GetConsole()->AddFunction("SetWeebleOffset", ConsoleHookSetWeebleOffset, "help your goddamn self", 1, 1);

    GetConsole()->AddFunction("SetWheelieRange", ConsoleHookSetWheelieRange, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetWheelieOffsetY", ConsoleHookSetWheelieOffsetY, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetWheelieOffsetZ", ConsoleHookSetWheelieOffsetZ, "help your goddamn self", 1, 1);

    GetConsole()->AddFunction( "SetShadowAdjustments", ConsoleHookSetShadowAdjustments, "Move the shadow points around", 8, 8 );
    GetConsole()->AddFunction( "SetShininess", ConsoleHookSetShininess, "Set the environmental reflection 0 to 1", 1, 1 );

    GetConsole()->AddFunction("SetDriver", ConsoleHookSetDriverName, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetGamblingOdds",ConsoleHookSetGamblingOdds,"Set Gambling Odds",1,1);

    GetConsole()->AddFunction("SetHasDoors", ConsoleHookSetHasDoors, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetCharactersVisible", ConsoleHookSetCharactersVisible, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetIrisTransition", ConsoleHookSetIrisTransition, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetAllowSeatSlide", ConsoleHookSetAllowSlide, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetHighRoof", ConsoleHookSetHighRoof, "help your goddamn self", 1, 1);
    GetConsole()->AddFunction("SetCharacterScale", ConsoleHookSetCharacterScale, "help your goddamn self", 1, 1);

    GetConsole()->AddFunction("SuppressDriver", ConsoleHookSuppressDriver, "help your goddamn self", 1, 1);
}

//=============================================================================
// VehicleCentral::~VehicleCentral
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      VehicleCentral
//
//=============================================================================
VehicleCentral::~VehicleCentral()
{
    ClearSuppressedDrivers();


    int i;
    for(i = 0; i < MAX_ACTIVE_VEHICLES; i++)
    {
        if(mActiveVehicleList[i])
        {
            // TODO - will the mission manager delete these instead??
            //
            // for now, only delete the ones that were left in there...            
            delete mActiveVehicleList[i];
        }

        if ( this->mActiveVehicleControllerList[ i ] )
        {
            mActiveVehicleControllerList[ i ]->Release();
            mActiveVehicleControllerList[ i ] = NULL;
        }
/*
        if ( mDoorTriggerList[ i ] )
        {
            mDoorTriggerList[ i ]->Release( );
            mDoorTriggerList[ i ] = 0;
        }
*/
    }

    // create a static vehicle AI and put it in limbo...
    if( spGenericAI )
    {
        spGenericAI->ReleaseVerified();
    }

    for( i=0; i<NUM_HEADLIGHT_BBQGS; i++ )
    {
        if( mHeadLights[i] )
        {
            mHeadLights[i]->Release();
            mHeadLights[i] = NULL;
        }
    }

}


//=============================================================================
// VehicleCentral::GetInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      VehicleCentral
//
//=============================================================================
VehicleCentral* VehicleCentral::GetInstance()
{
    rAssert(spInstance);
    return spInstance;
}


//=============================================================================
// VehicleCentral::CreateInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      VehicleCentral
//
//=============================================================================
VehicleCentral* VehicleCentral::CreateInstance()
{
    rAssert(spInstance == 0);

    spInstance = new(GMA_PERSISTENT) VehicleCentral;
    rAssert(spInstance);
    
    return spInstance;

}


//=============================================================================
// VehicleCentral::DestroyInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::DestroyInstance()
{
    rAssert(spInstance);

    delete(GMA_PERSISTENT, spInstance);
    spInstance = NULL;


}


//=============================================================================
// VehicleCentral::InitHuskPool
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::InitHuskPool()
{
}


//=============================================================================
// VehicleCentral::FreeHuskPool
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::FreeHuskPool()
{


}
        
        
//=============================================================================
// VehicleCentral::PreLoad
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::PreLoad()
{
    // bunch of actual loading moved to frontend startup to reduce load times - nbrooke 3/7/2003

    this->mHuskPool.Init(MAX_HUSKS);
 
    rAssert( mHeadLights[0] == NULL && mHeadLights[1] == NULL && mHeadLights[2] == NULL );

    // also might as well load the vehicle commons here.. 
    p3d::inventory->PushSection();
    p3d::inventory->SelectSection( "Global" );
    bool oldCurrSectionOnly = p3d::inventory->GetCurrentSectionOnly();
    p3d::inventory->SetCurrentSectionOnly( true );

    tRefCounted::Assign( mHeadLights[0], p3d::find<tBillboardQuadGroup>(tEntity::MakeUID("headlightShape8")) );
    tRefCounted::Assign( mHeadLights[1], p3d::find<tBillboardQuadGroup>(tEntity::MakeUID("headlight2Shape")) );
    tRefCounted::Assign( mHeadLights[2], p3d::find<tBillboardQuadGroup>(tEntity::MakeUID("glowGroupShape2")) );

    p3d::inventory->SetCurrentSectionOnly( oldCurrSectionOnly );
    p3d::inventory->PopSection();

    // grab the colours from the bbqgs
    int count = 0;
    for( int i=0; i<NUM_HEADLIGHT_BBQGS; i++ )
    {
        rAssert( mHeadLights[i] );
        for( int j=0; j<mHeadLights[i]->GetNumQuads(); j++ )
        {
            tBillboardQuad* quad = mHeadLights[i]->GetQuad( j );
            rAssert( quad );
            mOriginalHeadLightColours[count] = quad->GetColour();
            count++;
        }
    }
    rAssert( mHeadLights[0] && mHeadLights[1] && mHeadLights[2] );

}



//=============================================================================
// VehicleCentral::Unload
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::Unload()
{
    // empty out husk manager here also
    this->mHuskPool.Empty(); 
    
    ClearSuppressedDrivers();

    int count = 0;

    // restore prev states and release all preloaded headlights bbqgs
    for( int i=0; i<NUM_HEADLIGHT_BBQGS; i++ )
    {
        rAssert( mHeadLights[i] );
        for( int j=0; j<mHeadLights[i]->GetNumQuads(); j++ )
        {
            tBillboardQuad* quad = mHeadLights[i]->GetQuad( j );
            rAssert( quad );
            quad->SetColour( mOriginalHeadLightColours[count] );
            count++;
        }
        // ProcessShaders screwed us by causing us to lose the additive blend
        // We set it back.
        mHeadLights[i]->GetShader()->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ADD );

        mHeadLights[i]->Release();
        mHeadLights[i] = NULL;
    }

    rAssert(mNumActiveVehicles == 0);
    
}

//=============================================================================
// VehicleCentral::InitVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
Vehicle* VehicleCentral::InitVehicle( const char* name, bool addToActiveVehicleList, char* confile, VehicleType vt, DriverInit driver, bool playercar, bool startoutofcar)
{
    HeapMgr()->PushHeap( GetGameplayManager()->GetCurrentMissionHeap() );

    MEMTRACK_PUSH_GROUP( "VehicleCentral" );
//#ifdef RAD_GAMECUBE
//    Vehicle* vehicle = new( GMA_GC_VMM ) Vehicle;
//#else
    Vehicle* vehicle = new(GetGameplayManager()->GetCurrentMissionHeap()) Vehicle;
//#endif

    //if this car is owned by the player then set the flag.
    if(playercar ==true)
    {
        vehicle->mbPlayerCar=true;
    }  

    // note: moved the car open-door locator to the vehicle init
    bool ok = vehicle->Init( name, GetWorldPhysicsManager()->mSimEnvironment, VL_PHYSICS, vt, startoutofcar);
    rAssert(ok);     

    // so the script hooks can get at it
    rAssert( mVehicleUnderConstruction[mCurrentVehicleUnderConstructionTail] == NULL );
    mVehicleUnderConstruction[mCurrentVehicleUnderConstructionTail] = vehicle;
    mCurrentVehicleUnderConstructionTail = (mCurrentVehicleUnderConstructionTail + 1) % MAX_ACTIVE_VEHICLES;

    //If this happens, we're in big shit. We'll need to up the number of active vehicles or look closer at the loading.
    rAssert( mCurrentVehicleUnderContructionHead != mCurrentVehicleUnderConstructionTail );  

    // see TODO in comment blocks above
    // DL: now loads the car based on name

    char scriptname[64];
    strcpy( scriptname, "scripts/cars/" );

    if(confile != NULL && confile[0] != '\0')
    {
        strcat( scriptname, confile );

    }
    else
    {
        strcat( scriptname, name );
        strcat( scriptname, ".con" );
    }

    vehicle->mDriverInit = driver;
    GetMissionScriptLoader()->LoadScriptAsync( scriptname, this );

    //vehicle->CalculateValuesBasedOnDesignerParams();

    if(addToActiveVehicleList)
    {
        // return slot ignored here...
        int dummy = AddVehicleToActiveList(vehicle);
    }

    MEMTRACK_POP_GROUP( "VehicleCentral" );

    HeapMgr()->PopHeap( GetGameplayManager()->GetCurrentMissionHeap() );   
    
    return vehicle;
}


//=============================================================================
// VehicleCentral::AddVehicleToActiveList
//=============================================================================
// Description: Comment
//
// returns index if successful, otherwise -1
//
// Parameters:  VehicleCentral::AddVehicleToActiveList
//
// Return:      int 
//
//=============================================================================
int VehicleCentral::AddVehicleToActiveList(Vehicle* vehicle)
{


    //==============================================================================
    //
    // let's be clear here:
    //
    // an 'active' vehicle is one that will be added to the dsg
    // it will also request a collision area index and put itself in there,
    // but it won't do it's own query to fill that area unless it's under VL_PHYSICS
    //
    //==============================================================================

    // find this vehicle a slot
    int slot = -1;

    int i;
    for(i = 0; i < MAX_ACTIVE_VEHICLES; i++)
    {
        // debug test
        if(mActiveVehicleList[i])
        {
            if(mActiveVehicleList[i] == vehicle)
            {
                // this is already in our list!!!!
                //rAssertMsg( false, "This vehicle is already in the active list\n");
                return i;

                //chuck: draw the driver
                if (vehicle->GetDriver() !=NULL)
                {
                    vehicle->GetDriver()->AddToWorldScene();
                }

            }
        }

        
    }
    
    for(i = 0; i < MAX_ACTIVE_VEHICLES; i++)
    {
        if(mActiveVehicleList[i] == 0)
        {
            // got it.
            slot = i;
            break;
        }
    }

    
    if(slot == -1)
    {
        // couldn't find room for it.
        //?
        rAssertMsg(0,"Trying to add too many active vehicles!");
        return  -1;
    }

    //----------
    // add it in
    //----------

    mActiveVehicleList[slot] = vehicle;

    if (vehicle->GetDriver() !=NULL)
    {
        vehicle->GetDriver()->AddToWorldScene();
    }



    // this is perhaps totally redundant right now
    mNumActiveVehicles++;

    if(mNumActiveVehicles > MAX_ACTIVE_VEHICLES)
    {
        rAssertMsg(0,"Too many active vehicles!");
        return -1;
    }

    
    //--------------------
    // trigger volume shit
    //--------------------

    if( vehicle->mVehicleType == VT_AI &&
        ::GetGameFlow()->GetCurrentContext() != CONTEXT_SUPERSPRINT &&
        !GetGameplayManager()->mIsDemo )
    {
        GetTriggerVolumeTracker()->RegisterAI( vehicle );
    }



    int id = vehicle->mpEventLocator->GetData();
    ActionButton::GetInCar* pABHandler = static_cast<ActionButton::GetInCar*>( GetActionButtonManager()->GetActionByIndex(id) );
    rAssert( dynamic_cast<ActionButton::GetInCar*>( pABHandler ) != NULL );
    rAssert( pABHandler );
    pABHandler->SetVehicleId(slot);


    // leave AI untouched! Their triggers remain OFF!
    vehicle->ActivateTriggers(true);

    //-----------
    // add to dsg
    //-----------
    GetRenderManager()->pWorldScene()->Add((DynaPhysDSG*)vehicle);

    vehicle->GetCollisionAreaIndexAndAddSelf();
    
    int curWorldRenderLayer = GetRenderManager()->rCurWorldRenderLayer();
    vehicle->SetRenderLayerEnum((RenderEnums::LayerEnum)curWorldRenderLayer);


    // temp - for car on car collisions
    // TODO - remove    

    // until the vehicles are moving around in the scenegraph, we need to make sure the cars have a collision pair for each other
           
    /*
    for(i = 0; i < MAX_ACTIVE_VEHICLES; i++)
    {
        if(mActiveVehicleList[i] != 0 && i != slot)
        {
            vehicle->AddToOtherCollisionArea(mActiveVehicleList[i]->mCollisionAreaIndex);
        }
    }
    */

    //
    // Notify the world about new user vehicles
    //
    if( vehicle->mVehicleType == VT_USER )
    {
        GetEventManager()->TriggerEvent( EVENT_USER_VEHICLE_ADDED_TO_WORLD, vehicle );
    }

    // april 22, 2003
    // new
    // vehicle will hold it's slot.
    
    vehicle->mVehicleCentralIndex = slot;

    vehicle->AddRef();

    return slot;

}



//=============================================================================
// VehicleCentral::RemoveVehicleFromActiveList
//=============================================================================
// Description: Comment
//
// returns whether or not it was even there in the first place
//
//
// Parameters:  (Vehicle* vehicle)
//
// Return:      bool 
//
//=============================================================================
bool VehicleCentral::RemoveVehicleFromActiveList(Vehicle* vehicle)
{
    int i;
    for(i = 0; i < MAX_ACTIVE_VEHICLES; i++)
    {
        if(mActiveVehicleList[i] == vehicle)
        {
            
            //Chuck: If car getting dumped was the last car used by the player
            //set the mCurrentVehicle to NULL
            
            
            // greg
            // jan 4, 2003
            //
            // TODO
            // not sure if we want this code
            // mCurrentVehicle should be updated elsewhere
            
            // PHONEBOOTH CAR SHOULD REPLACE DEFAULT VEHICLE.
            // this change is going in rsn           
                
                //if (GetGameplayManager()->GetCurrentVehicle() == vehicle)
                //{
                //    Vehicle* p_vehicle = GetGameplayManager()->GetVehicle(GetGameplayManager()->mDefaultVehicle);
                //    GetGameplayManager()->SetCurrentVehicle(p_vehicle);
                //}

            // later in the same day
            //
            // GameplayManager::DumpCurrentCar ()
            // now sets mCurrentVehicle to NULL as the name would imply


            mActiveVehicleList[i] = 0;
            mNumActiveVehicles--;

            // make sure controller slot doesn't have junk left lying around in it.
            if( mActiveVehicleControllerList[i] )
            {
                mActiveVehicleControllerList[i]->Release();
            }
            //mActiveVehicleControllerList[i] = NULL;
            mActiveVehicleControllerList[i] = spGenericAI;
            mActiveVehicleControllerList[i]->AddRef();
    
            // !! need to remove this from any other vehicles mCurrentDynamics list
            //
            // what an ugly pain in the ass:
            GetWorldPhysicsManager()->RemoveFromAnyOtherCurrentDynamicsListAndCollisionArea(vehicle);

            vehicle->RemoveSelfAndFreeCollisionAreaIndex();


            //----------------
            // remove from DSG
            //----------------

            int& curRenderLayer = GetRenderManager()->rCurWorldRenderLayer();
            int orgRenderLayer = curRenderLayer;
            
            // swtich to the vehicle's render layer
            curRenderLayer = vehicle->GetRenderLayerEnum();

            //Chuck: if we're removing the car lets check if there is a AI NPC driver in that car
            if(vehicle->GetDriver() != NULL)
            {
                vehicle->GetDriver()->RemoveFromWorldScene();
            }
            GetRenderManager()->pWorldScene()->Remove(/*(DynaPhysDSG*)*/ vehicle);


            // restore the orignal render layer
            curRenderLayer = orgRenderLayer;

            // hack
            //vehicle->Release();

            // leave AI untouched! Their triggers remain OFF!
            vehicle->ActivateTriggers(false);

            //
            // Notify the world about deleted user vehicles
            //
            if( vehicle->mVehicleType == VT_USER )
            {
                GetEventManager()->TriggerEvent( EVENT_USER_VEHICLE_REMOVED_FROM_WORLD, vehicle );
            }

            // stop tracking AI... 
            if( GetGameFlow()->GetCurrentContext() != CONTEXT_SUPERSPRINT )
            {
                GetTriggerVolumeTracker()->UnregisterAI( vehicle );
            }

            vehicle->mVehicleCentralIndex = -1;

            vehicle->Release();

            return true;
        }
        /*
        else if(mActiveVehicleList[i])
        {
            // this isn't super-efficient but it's only temporary and should stop Darryl's whining
            
            // try and remove from all other collision areas just in case it's in there...
            GetWorldPhysicsManager()->mCollisionManager->
                RemoveCollisionObject(vehicle->mSimStateArticulated->GetCollisionObject(), mActiveVehicleList[i]->mCollisionAreaIndex);
            
   
        }
        */

    }


    return false;
}


//=============================================================================
// VehicleCentral::KillEmAll
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::KillEmAll()
{
    // not sure if I should keep this method around, because whoever creates the 
    // vehicle should destroy it
    //
    // but the method name is so fucking cool....


}

/*
//=============================================================================
// VehicleCentral::Suspend
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::Suspend()
{
    mSuspended = true;
}


//=============================================================================
// VehicleCentral::Resume
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::Resume()
{
    mSuspended = false;
}
*/

//=============================================================================
// VehicleCentral::GetVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( int id )
//
// Return:      Vehicle
//
//=============================================================================
Vehicle* VehicleCentral::GetVehicle( int id ) const
{
    //rAssert( id < this->mNumActiveVehicles );
    return this->mActiveVehicleList[ id ];
}


//=============================================================================
// VehicleCentral::SetVehicleController
//=============================================================================
// Description: Comment
//
// Parameters:  ( int id, VehicleController* pVehicleController )
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::SetVehicleController( int id, VehicleController* pVehicleController )
{

    if ( id >= 0 && id < MAX_ACTIVE_VEHICLES )
    {
        //rAssert( id < this->mNumActiveVehicles ); // don't think this is necessary anymore
        if ( pVehicleController )
        {
            pVehicleController->Init();
        }
        else
        {
            if ( mActiveVehicleControllerList[ id ] )
            {
                mActiveVehicleControllerList[ id ]->Shutdown();
            }
            pVehicleController = spGenericAI;
        }

        tRefCounted::Assign( mActiveVehicleControllerList[ id ], pVehicleController );
    }
    else
    {
        rAssertMsg( 0, "SetVehicleController - id out of range" );
    }
}

//Triage hack, only for demo mode, or until Greg actually 
//addrefs and releases --dm 12/01/02
VehicleController* VehicleCentral::RemoveVehicleController( int mAIIndex )
{
    if ( mActiveVehicleControllerList[ mAIIndex ] )
    {
        VehicleController* FoundVehicleController = mActiveVehicleControllerList[ mAIIndex ];
        mActiveVehicleControllerList[ mAIIndex ] = 0;
        return FoundVehicleController;
    }
    return NULL;
}
//=============================================================================
// VehicleCentral::GetVehicleController
//=============================================================================
// Description: Comment
//
// Parameters:  ( int id )
//
// Return:      VehicleController
//
//=============================================================================
VehicleController* VehicleCentral::GetVehicleController( int id ) const
{
    if( 0 <= id && id < MAX_ACTIVE_VEHICLES )
    {
        if( mActiveVehicleControllerList[ id ] == spGenericAI )
        {
            return NULL;
        }
        return mActiveVehicleControllerList[ id ];
    }
    return NULL;
}


//=============================================================================
// VehicleCentral::GetVehicleId
//=============================================================================
// Description: Comment
//
// Parameters:  ( Vehicle* pVehicle )
//
// Return:      int 
//
//=============================================================================
int VehicleCentral::GetVehicleId( Vehicle* pVehicle, bool checkStrict ) const
{
    int i;
    for ( i = 0; i < MAX_ACTIVE_VEHICLES; i++ )              
    {
        if(pVehicle == mActiveVehicleList[i])
        {
            return i;
        }
    }

    if(checkStrict)
    {
        rAssertMsg( false, "Vehicle not found in Vehicle Database!\n" );
    }

    return (int)-1;
}

//=============================================================================
// VehicleCentral::PreSubstepUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( )
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::PreSubstepUpdate( float dt )
{
    int i;
    for ( i = 0; i < MAX_ACTIVE_VEHICLES; i++ )      
    {
        Vehicle* vehicle = mActiveVehicleList[i];
        if( vehicle ) // TODO - wrap with render layer test
        {
            vehicle->PreSubstepUpdate(dt);

            // Update controller
            VehicleController* vController = mActiveVehicleControllerList[i];
            if( vController != 0 )
            {
                vController->Update( dt );

                float test = vController->GetGas();
                //?
                // safe to just apply these as-is to vehicle?
                vehicle->SetGas(test);


                test = vController->GetBrake( );
                vehicle->SetBrake( test );


                test = vController->GetThrottle();
                if(test > 0.1f)
                {
                    vehicle->SetGas( test ); 
                }
                if(test < -0.1f)
                {
                    vehicle->SetBrake( -test );
                }


                //This sucks.
                bool isWheel = false;
                test = vController->GetSteering( isWheel );
                
                
                // test stick and dpad and set highest fabs
                float left = 0.0f;
                if( vController->GetSteerLeft() )
                {
                    left = vController->GetSteerLeft( );
                }
                    
                float right = 0.0f;
                if( vController->GetSteerRight() )
                {
                    right = vController->GetSteerRight( );
                }
                
                if(rmt::Fabs(test) > left && rmt::Fabs(test) > right)
                {
                    // use stick value
                    vehicle->SetWheelTurnAngle(test, isWheel, dt);

                }      
                else if(right > left)
                {
                    vehicle->SetWheelTurnAngle(right, false, dt);
                }
                else
                {
                   vehicle->SetWheelTurnAngle(-left, false, dt);                
                }
                
                
                
                /*
                // only human controlled vehicles will return a pointer here
                if(vController->GetSteerLeft())
                {
                    float left = vController->GetSteerLeft( );
                    if(test <= 0.0f && left > rmt::Fabs(test))
                    {
                        vehicle->SetWheelTurnAngle(-left, false, dt);
                    }
                }        
                if(vController->GetSteerRight())
                {
                    float right = vController->GetSteerRight( );
                    if(test >= 0.0f && right > test)
                    {
                        vehicle->SetWheelTurnAngle(right, false, dt);
                    }
                }        
                */

                test = vController->GetReverse( );
                vehicle->SetReverse(test);

                test = vController->GetHandBrake();
                vehicle->SetEBrake(test, dt);
                
                if( GetCheatInputSystem()->IsCheatEnabled(CHEAT_ID_CAR_JUMP_ON_HORN) && 
                    vehicle == GetAvatarManager()->GetAvatarForPlayer(0)->GetVehicle() )    //   vehicle->mVehicleType == VT_USER )
                {
                    test = vController->GetHorn( );
                    if(test > 0.1f)                    
                    {
                        vehicle->JumpOnHorn(test);
                    }
                }

                if( vController->GetHorn() > 0.0f )
                {
                    // Only the player vehicle's controller uses Set/GetHorn. 
                    // AI cars never at any time set the Horn value. 
                    if( vehicle == GetAvatarManager()->GetAvatarForPlayer(0)->GetVehicle() ||
                        ::GetGameplayManager()->GetGameType() == GameplayManager::GT_SUPERSPRINT )
                    {
                        // if in a certain game type, use the horn button for speedboost
                        if( ::GetGameplayManager()->GetGameType() == GameplayManager::GT_SUPERSPRINT )
                        {
                            // may or may not do turbo depending on if turbo is available..
                            vehicle->TurboOnHorn();
                        }
                        else
                        {
                            // outside of Supersprint context, we want to fire off an event
                            // so that traffic can honk back and peds can panick.
                            GetEventManager()->TriggerEvent( EVENT_PLAYER_VEHICLE_HORN, vehicle );
                        }
                    }
                }
                
                
            }
            else
            {
                vehicle->SetGas(0.0f);
                vehicle->SetBrake(0.0f);
                vehicle->SetWheelTurnAngle(0.0f, false, dt);
                vehicle->SetReverse(0.0f);
                vehicle->SetEBrake(0.0f, dt);
            }

            //
            // [Dusit Matthew Eakkachaichanvet: Dec 10, 2002]
            // ok, the controller has been updated and knows of 
            // last frame's collision with a vehicle, so 
            // we can clear this value now.
            //
            vehicle->mCollidedWithVehicle = false;
            vehicle->mNormalizedMagnitudeOfVehicleHit = 0.0f;
            vehicle->mWasHitByVehicle = false;

        }
    }
}

//=============================================================================
// VehicleCentral::Update
//=============================================================================
// Description: Comment
//
// Parameters:  (float dt)
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::Update(float dt)       
{
 
    int i;
    for ( i = 0; i < MAX_ACTIVE_VEHICLES; i++ )
    {
        //rAssert( mActiveVehicleList[ i ] );
        if( mActiveVehicleList[i] && 
            (GetRenderManager()->rCurWorldRenderLayer() == mActiveVehicleList[i]->GetRenderLayerEnum()))
        {
            BEGIN_PROFILE("ActiveVehicle->Update")
            mActiveVehicleList[i]->Update( dt );
            END_PROFILE("ActiveVehicle->Update")

            //rAssertMsg( mActiveVehicleList[i] != NULL, "VehicleCentral::Update - trying to add a null vehicle* pointer!" );

            if ( mActiveVehicleList[i] != NULL )
            {
                if(mActiveVehicleList[i]->GetLocomotionType() == VL_PHYSICS)
                {
                    BEGIN_PROFILE("WorldPhysManager()->UpdateDyna")
                    GetWorldPhysicsManager()->UpdateDynamicObjects(dt, mActiveVehicleList[i]->mCollisionAreaIndex);
                    GetWorldPhysicsManager()->UpdateAnimCollisions(dt, mActiveVehicleList[i]->mCollisionAreaIndex);
                    END_PROFILE("WorldPhysManager()->UpdateDyna")
                    // Temp removed this because objects were being updated twice.
                    // more important (for me) to have the character update anim objects.
                    //
                    // TBJ [8/27/2002] 
                    //
                    //GetWorldPhysicsManager()->UpdateAnimCollisions(dt, mActiveVehicleList[i]->mCollisionAreaIndex);
                    
                }
            }
        }
    }
}


//=============================================================================
// VehicleCentral::ClearSpot
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector& point, float radius)
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::ClearSpot(rmt::Vector& point, float radius, Vehicle* skipCar)
{
    rAssert(0); // don't call this
    int i;
    for ( i = 0; i < MAX_ACTIVE_VEHICLES; i++ )
    {        
        if( mActiveVehicleList[i] && 
            GetRenderManager()->rCurWorldRenderLayer() == mActiveVehicleList[i]->GetRenderLayerEnum() &&
            mActiveVehicleList[i] != skipCar)
        {
        
            rmt::Vector carPosition = mActiveVehicleList[i]->rPosition();
            
            rmt::Vector vectorToCar = carPosition;            
            vectorToCar.Sub(point);
            
            float carsWheelBase = mActiveVehicleList[i]->GetWheelBase();
            
            float dist = vectorToCar.Magnitude();
            if(dist - carsWheelBase < radius)
            {
                // this car has to move back
                
                float amount = radius - (dist - carsWheelBase);
                
                // first test
                // blast it back by some ratio of amount?
            
                rmt::Vector fix = vectorToCar;
                fix.NormalizeSafe();
                fix.Scale(amount);
                
                carPosition.Add(fix);
                
                // quick test for fun and safety
                carPosition.y += 2.0f;
                
                float ang = mActiveVehicleList[i]->GetFacingInRadians();
                                                
                rmt::Matrix m;
                m.Identity();
                m.FillRotateXYZ( 0.0f, ang, 0.0f );
                m.FillTranslate(carPosition);
                
                mActiveVehicleList[i]->SetTransform(m);
                
                
                /*
                rmt::Vector& linearVelocity = mActiveVehicleList[i]->mSimStateArticulated->GetLinearVelocity();
                
                vectorToCar.NormalizeSafe();
                
                static float magicshit = 1.0f;
                
                vectorToCar.Scale(amount * magicshit);
                
                linearVelocity = vectorToCar;
                */
            
            }

            
            
        }
    }
    
    
}



//=============================================================================
// VehicleCentral::PostSubstepUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  (float dt)
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::PostSubstepUpdate(float dt)
{

    int i;
    for ( i = 0; i < MAX_ACTIVE_VEHICLES; i++ )
    {
        //rAssert( mActiveVehicleList[ i ] );
        if( mActiveVehicleList[i] && 
            GetRenderManager()->rCurWorldRenderLayer() == mActiveVehicleList[i]->GetRenderLayerEnum())
        {
            mActiveVehicleList[i]->PostSubstepUpdate(dt);
        }
    }
}


//=============================================================================
// VehicleCentral::PreCollisionPrep
//=============================================================================
// Description: Comment
//
// Parameters:  ( )
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::PreCollisionPrep(float dt, bool firstSubstep)
{
    int i;
    for ( i = 0; i < MAX_ACTIVE_VEHICLES; i++ )            
    {
        if( mActiveVehicleList[i] &&
            GetRenderManager()->rCurWorldRenderLayer() == mActiveVehicleList[i]->GetRenderLayerEnum()) 
        {
            mActiveVehicleList[ i ]->PreCollisionPrep(dt, firstSubstep);
        }
    }
}


//=============================================================================
// VehicleCentral::SubmitStatics
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::SubmitStatics()
{
    int i;
    for ( i = 0; i < MAX_ACTIVE_VEHICLES; i++ )          
    {
        if( mActiveVehicleList[i] &&
            GetRenderManager()->rCurWorldRenderLayer() == mActiveVehicleList[i]->GetRenderLayerEnum())
        {     
            if(1)  
            //if(mActiveVehicleList[i]->GetLocomotionType() == VL_PHYSICS)               
            {
                rmt::Vector position = mActiveVehicleList[i]->GetPosition();
                
                // at 200kmh, we cover 55.5 m/s
                // a long frame would be 50ms...
                //
                // at that rate we'd cover 2.775 m
                
                // hmm.... ferrini wheel base is approx 3m
                
                //float radius = mActiveVehicleList[i]->GetWheelBase() * 3.0f;    // TODO - what magic number?
                float radius = mActiveVehicleList[i]->GetWheelBase() * 2.0f;    // TODO - what magic number?
                
                int collisionAreaIndex = mActiveVehicleList[i]->mCollisionAreaIndex;
                rAssert(collisionAreaIndex != -1);
        
                Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer(0);
                Vehicle* playerVehicle = 0;
                if(playerAvatar)
                {    
                    playerVehicle = playerAvatar->GetVehicle();
                }

                if(playerVehicle && mActiveVehicleList[i] == playerVehicle)
                {
                    GetWorldPhysicsManager()->SubmitStaticsPseudoCallback(position, radius, collisionAreaIndex, mActiveVehicleList[i]->GetSimState(), true);
                    GetWorldPhysicsManager()->SubmitFencePiecesPseudoCallback(position, radius, collisionAreaIndex, mActiveVehicleList[i]->GetSimState(), true);
                }
                else
                {
                    GetWorldPhysicsManager()->SubmitStaticsPseudoCallback(position, radius, collisionAreaIndex, mActiveVehicleList[i]->GetSimState(), false);                
                    GetWorldPhysicsManager()->SubmitFencePiecesPseudoCallback(position, radius, collisionAreaIndex, mActiveVehicleList[i]->GetSimState());
                }

                // TODO - should probably only do this one if it's a human driver or AI ... ie. not traffic bouncing around
                //GetWorldPhysicsManager()->SubmitFencePiecesPseudoCallback(position, radius, collisionAreaIndex, mActiveVehicleList[i]->GetSimState());
            }
            else
            {
                /*
                // an active vehicle, but not VL_PHYSICS
                // so we want to do a DSG query and cleanup of our collision area if necessary
                
                rmt::Vector position = mActiveVehicleList[i]->GetPosition();
                
                float radius = mActiveVehicleList[i]->GetWheelBase() * 2.0f;    // TODO - what magic number?
                
                int collisionAreaIndex = mActiveVehicleList[i]->mCollisionAreaIndex;
                rAssert(collisionAreaIndex != -1);

                GetWorldPhysicsManager()->CleanOnlyStaticsPseudoCallback(position, radius, collisionAreaIndex, mActiveVehicleList[i]->GetSimState());

                // TODO - should probably only do this one if it's a human driver or AI ... ie. not traffic bouncing around
                GetWorldPhysicsManager()->SubmitFencePiecesPseudoCallback(position, radius, collisionAreaIndex, mActiveVehicleList[i]->GetSimState());
                */
                
            }
            
        }
    }

}



//=============================================================================
// VehicleCentral::SubmitDynamics
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::SubmitDynamics()
{
    int i;
    for ( i = 0; i < MAX_ACTIVE_VEHICLES; i++ )          
    {
        if( mActiveVehicleList[i] &&
            GetRenderManager()->rCurWorldRenderLayer() == mActiveVehicleList[i]->GetRenderLayerEnum())
        {       
            // new
            // call this for all (one) VT_USER cars first
            //if(mActiveVehicleList[i]->GetLocomotionType() == VL_PHYSICS && mActiveVehicleList[i]->mVehicleType == VT_USER)
            if(mActiveVehicleList[i]->mVehicleType == VT_USER)
            {
                rmt::Vector position = mActiveVehicleList[i]->GetPosition();
        
                //float radius = mActiveVehicleList[i]->GetWheelBase() * 3.0f;    // TODO - what magic number?
                float radius = mActiveVehicleList[i]->GetWheelBase() * 2.0f;    // TODO - what magic number?
                    
                int collisionAreaIndex = mActiveVehicleList[i]->mCollisionAreaIndex;
                rAssert(collisionAreaIndex != -1);
        
        
                Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer(0);
                Vehicle* playerVehicle = 0;
                if(playerAvatar)
                {    
                    playerVehicle = playerAvatar->GetVehicle();
                }

                if(playerVehicle && mActiveVehicleList[i] == playerVehicle)
                {
                    GetWorldPhysicsManager()->SubmitDynamicsPseudoCallback(position, radius, collisionAreaIndex, mActiveVehicleList[i]->GetSimState(), true);
                }
                else
                {
                   GetWorldPhysicsManager()->SubmitDynamicsPseudoCallback(position, radius, collisionAreaIndex, mActiveVehicleList[i]->GetSimState(), false);                
                }
            }    
        }
    }
    
    for ( i = 0; i < MAX_ACTIVE_VEHICLES; i++ )          
    {        
        if( mActiveVehicleList[i] &&
            GetRenderManager()->rCurWorldRenderLayer() == mActiveVehicleList[i]->GetRenderLayerEnum())
        {       
            //if(mActiveVehicleList[i]->GetLocomotionType() == VL_PHYSICS && mActiveVehicleList[i]->mVehicleType != VT_USER)
            if(mActiveVehicleList[i]->mVehicleType != VT_USER)
            {
                rmt::Vector position = mActiveVehicleList[i]->GetPosition();
        
                //float radius = mActiveVehicleList[i]->GetWheelBase() * 3.0f;    // TODO - what magic number?
                float radius = mActiveVehicleList[i]->GetWheelBase() * 2.0f;    // TODO - what magic number?
                    
                int collisionAreaIndex = mActiveVehicleList[i]->mCollisionAreaIndex;
                rAssert(collisionAreaIndex != -1);
        
        
        
                Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer(0);
                Vehicle* playerVehicle = 0;
                if(playerAvatar)
                {    
                    playerVehicle = playerAvatar->GetVehicle();
                }

                if(playerVehicle && mActiveVehicleList[i] == playerVehicle)
                {
                    GetWorldPhysicsManager()->SubmitDynamicsPseudoCallback(position, radius, collisionAreaIndex, mActiveVehicleList[i]->GetSimState(), true);
                }
                else
                {
                   GetWorldPhysicsManager()->SubmitDynamicsPseudoCallback(position, radius, collisionAreaIndex, mActiveVehicleList[i]->GetSimState(), false);                
                }
        
                //GetWorldPhysicsManager()->SubmitDynamicsPseudoCallback(position, radius, collisionAreaIndex, mActiveVehicleList[i]->GetSimState(), false);
            }    
        }
    }

}


//=============================================================================
// VehicleCentral::SubmitAnimCollisions
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::SubmitAnimCollisions()
{  
    int i;
    for ( i = 0; i < MAX_ACTIVE_VEHICLES; i++ )          
    {
        if( mActiveVehicleList[i] &&
            GetRenderManager()->rCurWorldRenderLayer() == mActiveVehicleList[i]->GetRenderLayerEnum())
        {       
            if(1)
            //if(mActiveVehicleList[i]->GetLocomotionType() == VL_PHYSICS)
            {
                rmt::Vector position = mActiveVehicleList[i]->GetPosition();
        
                //float collradius = mActiveVehicleList[i]->GetWheelBase() * 3.0f;    // TODO - what magic number?
                float collradius = mActiveVehicleList[i]->GetWheelBase() * 2.0f;    // TODO - what magic number?
                    
                int collisionAreaIndex = mActiveVehicleList[i]->mCollisionAreaIndex;
                rAssert(collisionAreaIndex != -1);
                
                GetWorldPhysicsManager()->SubmitAnimCollisionsPseudoCallback(position, collradius, collisionAreaIndex, mActiveVehicleList[i]->GetSimState());

                float updateradius = collradius * 1.0f;    //? TODO - what magic number
                GetWorldPhysicsManager()->SubmitAnimCollisionsForUpdateOnly(position, updateradius, collisionAreaIndex);
            }    
        }
    }



}

//=============================================================================
// VehicleCentral::GetVehicleByName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      Vehicle
//
//=============================================================================
Vehicle* VehicleCentral::GetVehicleByName( const char* name ) const
{
    return GetVehicleByUID( tEntity::MakeUID( name ) );
}

/*
==============================================================================
VehicleCentral::GetVehicleByUID
==============================================================================
Description:    Comment

Parameters:     ( tUID uid )

Return:         Vehicle

=============================================================================
*/
Vehicle* VehicleCentral::GetVehicleByUID( tUID uid ) const
{
    Vehicle* vehicle = 0;
    for( int i = 0; i < MAX_ACTIVE_VEHICLES; i++ )
    {
        if ( mActiveVehicleList[ i ] != 0 )
        {
            if( uid == tEntity::MakeUID( mActiveVehicleList[ i ]->GetName() ) )
            {
                vehicle = mActiveVehicleList[ i ];
                break;
            }
        }
    }
    return vehicle;
}


//=============================================================================
// VehicleCentral::OnProcessRequestsComplete
//=============================================================================
// Description: Comment
//
// Parameters:  ( void* pUserData )
//
// Return:      void 
//
//=============================================================================
void VehicleCentral::OnProcessRequestsComplete( void* pUserData )
{
    Character* theDriver;

    Vehicle* vehicle = GetCurrentVehicleUnderConstruction();
    rAssert( vehicle );

    vehicle->CalculateValuesBasedOnDesignerParams();

    SetupDriver(vehicle);

    theDriver = vehicle->GetDriver();
    if( theDriver != NULL )
    {
        //
        // Despite the name, send this for every vehicle load.  The dialogue
        // system keeps a flag to determine which ones come from phone booths,
        // and throws the rest of 'em out.  Not pretty, but we're going final.
        // -- Esan
        //
        GetEventManager()->TriggerEvent( EVENT_PHONE_BOOTH_NEW_VEHICLE_SELECTED, theDriver );
    }
    else
    {
        GetEventManager()->TriggerEvent( EVENT_PHONE_BOOTH_CANCEL_RIDEREPLY_LINE );
    }

    //Clear the head.
    mVehicleUnderConstruction[mCurrentVehicleUnderContructionHead] = NULL;
    mCurrentVehicleUnderContructionHead = (mCurrentVehicleUnderContructionHead + 1) % MAX_ACTIVE_VEHICLES;

    if ( mCurrentVehicleUnderContructionHead == mCurrentVehicleUnderConstructionTail )
    {
        rAssert( mVehicleUnderConstruction[mCurrentVehicleUnderContructionHead] == NULL );
    }
}

void VehicleCentral::ActivateVehicleTriggers(bool active)
{
    if(active == mbVehicleTriggersActive)
    {
        return;
    }

    mbVehicleTriggersActive = active;

    Vehicle* v = NULL;

    if( mbVehicleTriggersActive )
    {
        for(int i = 0; i < MAX_ACTIVE_VEHICLES; i++)
        {
            v = mActiveVehicleList[i];
            if(v)
            {
                v->ActivateTriggers(true);
            }
        }
    }
    else
    {
        for(int i = 0; i < MAX_ACTIVE_VEHICLES; i++)
        {
            v = mActiveVehicleList[i];
            if( v )
            {
                v->ActivateTriggers(false);
            }
        }
    }

}

void VehicleCentral::ClearSuppressedDrivers(void)
{
    for(unsigned i = 0; i < mSuppressedDriverCount; i++)
    {
        mSuppressedDrivers[i].SetText(NULL);
    }

    mSuppressedDriverCount = 0;
}

void VehicleCentral::AddSuppressedDriver(const char* name)
{
    rAssert(mSuppressedDriverCount < MAX_SuppressED_DRIVERS);
    mSuppressedDrivers[mSuppressedDriverCount++].SetText(name);
}

void VehicleCentral::RemoveSuppressedDriver(const char* name)
{
    tUID uid = tEntity::MakeUID(name);
    for(unsigned i = 0; i < mSuppressedDriverCount; i++)
    {
        if(mSuppressedDrivers[i].GetUID() == uid)
        {
            mSuppressedDrivers[i].SetUID((tUID)0);
            return;
        }
    }
}

bool VehicleCentral::IsDriverSuppressed(const char* name)
{
    tUID uid = tEntity::MakeUID(name);
    for(unsigned i = 0; i < mSuppressedDriverCount; i++)
    {
        if(mSuppressedDrivers[i].GetUID() == uid)
            return true;
    }
    return false;
}

void VehicleCentral::SetupDriver(Vehicle* vehicle)
{
    if(vehicle->GetDriver())
    {
        if((vehicle->GetDriver() != GetCharacterManager()->GetCharacter(0)) && (vehicle->GetDriver()->GetRole() != Character::ROLE_PEDESTRIAN))
        {
            GetCharacterManager()->RemoveCharacter(vehicle->GetDriver());
            vehicle->SetDriver(NULL);
        }
    }

    if(vehicle->mDriverInit != FORCE_NO_DRIVER)
    {
        if((vehicle->GetDriverName()[0] != 0))
        {
            if(strcmp(vehicle->GetDriverName(),"phantom") == 0)
            {
                vehicle->SetPhantomDriver(true);
            }
            else
            {
                if((strcmp(vehicle->GetDriverName(),"none") != 0) &&
                    ((vehicle->mDriverInit == FORCE_DRIVER) ||  !IsDriverSuppressed(vehicle->GetDriverName())))
                {
                    char uniqueName[16];
                    sprintf(uniqueName, "d_%s", vehicle->GetDriverName());

                    Character* character  = NULL;
                    character = GetCharacterManager()->GetCharacterByName(uniqueName);
                    if(character && ((character->GetTargetVehicle() == vehicle) || (character->GetTargetVehicle() == NULL)))
                    {
                        GetCharacterManager()->RemoveCharacter(character);
                    }

                    character = GetCharacterManager()->AddCharacter(CharacterManager::NPC, uniqueName, vehicle->GetDriverName(), "npd", "");

                    static_cast<NPCController*>(character->GetController())->TransitToState(NPCController::NONE);
                    character->SetTargetVehicle( vehicle );
                    character->AddToWorldScene();
                    character->GetStateManager()->SetState<CharacterAi::InCar>();

                    vehicle->SetDriver(character);

                    character->SetRole(Character::ROLE_DRIVER);
                }
            }
        }
    }
}


//chuck adding this method so we can determine if car is still underconstruction.

bool VehicleCentral::IsCarUnderConstruction(const char* name)
{
    //unsigned int pListPtr = mCurrentVehicleUnderContructionHead;

    for(int i=0;i<MAX_ACTIVE_VEHICLES;i++)
    {
        if (mVehicleUnderConstruction[i] != NULL)
        {
            if( strcmp(mVehicleUnderConstruction[i]->GetName(),name) == 0)
            {
                return true;
            }
        }
    }
    return false;
}

bool VehicleCentral::IsCarUnderConstruction(const Vehicle* vehicle)
{
    for(int i=0;i<MAX_ACTIVE_VEHICLES;i++)
    {
        if (mVehicleUnderConstruction[i] != NULL)
        {
            if( mVehicleUnderConstruction[i] ==  vehicle )
            {
                return true;
            }
        }
    }
    return false;
}

VehicleAI* VehicleCentral::GetVehicleAI( Vehicle* vehicle )
{
    if( vehicle )
    {
        //////////////////////////
        // if Demo context, it's ok to fetch AI for 
        // non VT_AI types. Otherwise, we must boot!!
        //
        // Note: 
        // Supersprint doesn't worry about these, the AI cars should
        // be VT_AI...
        //
        if( GetGameFlow()->GetCurrentContext() != CONTEXT_DEMO && 
            vehicle->mVehicleType != VT_AI )
        {
            return NULL;
        }

        int id = GetVehicleCentral()->GetVehicleId( vehicle );
        VehicleController* controller = GetVehicleCentral()->GetVehicleController( id );
        if( controller )
        {
            rAssert( dynamic_cast<VehicleAI*>( controller ) );
            return static_cast<VehicleAI*>( controller );
        }
    }        
    return NULL;
}


void VehicleCentral::DetachAllCollectibles()
{
    for ( int i = 0 ; i < GetNumVehicles() ; i++ )
    {
        Vehicle* v = GetVehicle( i );
        if ( v != NULL )
        {
            v->DetachCollectible( rmt::Vector(0,0,0), false );
        }
    }
}
