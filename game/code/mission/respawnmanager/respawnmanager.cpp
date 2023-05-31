//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        Respawnmanager.cpp
//
// Description: Implementation for the RespawnManager class.
//
// History:     + Created -- Chuck Chow
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include <mission/respawnmanager/respawnmanager.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************




//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

 



 




//==============================================================================
// RespawnManager::RespawnManager
//==============================================================================
//
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================// 
 RespawnManager::RespawnManager() 
{
    //set some default vaules in milliseconds
    mWrenchRespawnTime = 30000;
    mWaspRespawnTime = 20000;
    mNitroRespawnTime = 15000;

}


//==============================================================================
// RespawnManager::~ReSpawnManager
//==============================================================================
//
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================// 
RespawnManager::~RespawnManager()
{

    
}



int RespawnManager::SetNitroRespawnTime(int milliseconds)
{
    mNitroRespawnTime = milliseconds;
    return 0;
}

int RespawnManager::SetWrenchRespawnTime(int milliseconds)
{
    mWrenchRespawnTime = milliseconds;
    return 0;
}

int RespawnManager::SetWaspRespawnTime(int milliseconds)
{
    mWaspRespawnTime =milliseconds;
    return 0;
}

int RespawnManager::GetWrenchRespawnTime()
{
    return mWrenchRespawnTime;
}

int RespawnManager::GetNitroRespawnTime()
{
    return mNitroRespawnTime;
}

int RespawnManager::GetWaspRespawnTime()
{
    return mWaspRespawnTime;
}


int RespawnManager::GetRespawnTime(RespawnEntity::eRespawnEntity entitytype)
{
    switch (entitytype)
    {
    case RespawnEntity::eWrench:
        {
            return GetWrenchRespawnTime();
            break;
        }
    case RespawnEntity::eNitro:
        {
            return GetNitroRespawnTime();
            break;
        }
    case RespawnEntity::eWasp:
        {
            return GetWaspRespawnTime();
            break;
        }
    default:
        {
            rTuneAssertMsg(0,"Unknown type of RespawnEntity passed in!\n");
            return -666; //red flag this
            break;
        }
    }
}









            




 //******************************************************************************
//
// Private Member Functions
//
//******************************************************************************      
