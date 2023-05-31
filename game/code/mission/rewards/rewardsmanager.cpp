//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        Rewardsmanager.cpp
//
// Description: Implementation for the RewardsManager class.
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
#include <mission/rewards/reward.h>
#include <mission/rewards/merchandise.h.>
#include <mission/rewards/rewardsmanager.h>
#include <memory/srrmemory.h>
#include <mission/charactersheet/charactersheet.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/missionscriptloader.h>

#include <string.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

#ifdef RAD_E3
const char* REWARDS_SCRIPT_FILE = "scripts\\missions\\e3rwrds.mfk";
#else
const char* REWARDS_SCRIPT_FILE = "scripts\\missions\\rewards.mfk";
#endif

// Static pointer to instance of singleton.

RewardsManager* RewardsManager::spInstance = NULL;


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// RewardsManager::CreateInstance
//==============================================================================
//
// Description: Creates the RewardsManager.
//
// Parameters:	None.
//
// Return:      Pointer to the RewardsManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
RewardsManager* RewardsManager::CreateInstance()
{
    rAssert( spInstance == NULL );
MEMTRACK_PUSH_GROUP("RewardsManager");

    spInstance = new (GMA_PERSISTENT) RewardsManager;
    rAssert( spInstance );
    
MEMTRACK_POP_GROUP("RewardsManager");
    return spInstance;
}

//==============================================================================
// RewardsManager::GetInstance
//==============================================================================
//
// Description: - Access point for the RewardsManager singleton.  
//
// Parameters:	None.
//
// Return:      Pointer to the RewardsManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
RewardsManager* RewardsManager::GetInstance()
{
    rAssert( spInstance != NULL );
    
    return spInstance;
}


//==============================================================================
// RewardsManager::DestroyInstance
//==============================================================================
//
// Description: Destroy the RewardsManager.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void RewardsManager::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete (GMA_PERSISTENT,spInstance);
    spInstance = NULL;
}


void RewardsManager::InitRewards()
{
    HeapMgr()->PushHeap( GMA_PERSISTENT );

    for (int i =0; i< MAX_LEVELS +1;i++)
    {
        mRewardsList[i].mStreetRace = new Reward;
        mRewardsList[i].mBonusMission = new Reward;
        mRewardsList[i].mCards = new Reward;
        mRewardsList[i].mDefaultCar = new Reward;
        mRewardsList[i].mDefaultSkin = new Reward;
        mRewardsList[i].mGoldCards = new Reward;

        mRewardsList[i].mMaxTokensInLevel = 200;
        mRewardsList[i].mTotalGagsInLevel = 10;
        mRewardsList[i].mTotalWaspsInLevel = 20;
    }

    HeapMgr()->PopHeap( GMA_PERSISTENT );
}


void RewardsManager::ClearRewards()
{
    //re-lock rewards
    for (int i =0; i< MAX_LEVELS +1;i++)
    {
        mRewardsList[i].mStreetRace->LockReward();
        mRewardsList[i].mBonusMission->LockReward();
        mRewardsList[i].mCards->LockReward();       
        mRewardsList[i].mDefaultCar->LockReward();
        mRewardsList[i].mDefaultSkin->LockReward();
        mRewardsList[i].mGoldCards->LockReward();
    }

    //relock items from the tokenstore
    for(int j=0; j<MAX_LEVELS;j++)
    {
        for (int k=0;k<MAX_INVENTORY;k++)
        {
            mLevelTokenStoreList[j].mInventoryList[k]->LockReward();
        }
    }


}

void RewardsManager::OnProcessRequestsComplete( void* pUserData )
{
    mScriptLoaded = true;
}


void RewardsManager::LoadScript()
{
    mScriptLoaded = false;
    GetMissionScriptLoader()->LoadScriptAsync( const_cast<char*>( REWARDS_SCRIPT_FILE ), this );
}


int RewardsManager::BindReward(char* name,char* filename, Reward::eRewardType rtype, Reward::eQuestType qtype, int level)
{
    NameCheck(name);
    FileNameCheck(filename);
    LevelCheck(level);

    switch (qtype)
    {
    case Reward::eStreetRace:
        {
            mRewardsList[level].mStreetRace->SetName(name);
            mRewardsList[level].mStreetRace->SetLevel(level);
            mRewardsList[level].mStreetRace->SetFilename(filename);
            mRewardsList[level].mStreetRace->SetRewardType(rtype);
            mRewardsList[level].mStreetRace->SetQuestType(qtype);

            break;
        }
    case Reward::eBonusMission:
        {
            mRewardsList[level].mBonusMission->SetName(name);
            mRewardsList[level].mBonusMission->SetLevel(level);
            mRewardsList[level].mBonusMission->SetFilename(filename);
            mRewardsList[level].mBonusMission->SetRewardType(rtype);
            mRewardsList[level].mBonusMission->SetQuestType(qtype);

            break;
        }
    case Reward::eCards:
        {
            mRewardsList[level].mCards->SetName(name);
            mRewardsList[level].mCards->SetLevel(level);
            mRewardsList[level].mCards->SetFilename(filename);
            mRewardsList[level].mCards->SetRewardType(rtype);
            mRewardsList[level].mCards->SetQuestType(qtype);

            break;
        }
    case Reward::eDefaultCar:
        {
            mRewardsList[level].mDefaultCar->SetName(name);
            mRewardsList[level].mDefaultCar->SetLevel(level);
            mRewardsList[level].mStreetRace->SetLevel(level);
            mRewardsList[level].mDefaultCar->SetFilename(filename);
            mRewardsList[level].mDefaultCar->SetRewardType(rtype);
            mRewardsList[level].mDefaultCar->SetQuestType(qtype);

            break;
        }
    case Reward::eDefaultSkin:
        {
            mRewardsList[level].mDefaultSkin->SetName(name);
            mRewardsList[level].mDefaultSkin->SetLevel(level);
            mRewardsList[level].mDefaultSkin->SetFilename(filename);
            mRewardsList[level].mDefaultSkin->SetRewardType(rtype);
            mRewardsList[level].mDefaultSkin->SetQuestType(qtype);

            break;
        }
   case Reward::eGoldCards:
        {
            mRewardsList[level].mCards->SetName(name);
            mRewardsList[level].mCards->SetLevel(level);
            mRewardsList[level].mCards->SetFilename(filename);
            mRewardsList[level].mCards->SetRewardType(rtype);
            mRewardsList[level].mCards->SetQuestType(qtype);

            break;
        }
    default:
        {
            printf("Unknown eQuestType!\n");
            break;
        }

    }            

    return 0;
}


Reward* RewardsManager::GetReward(int level, Reward::eQuestType qtype)
{
    rAssert( mScriptLoaded );

    // MikeR - adding a check for bad input level and returning NULL if
    // it fails
    if ( LevelCheck(level) == false )
    {
        rTunePrintf( "RewardsManager - GetReward() INVALID LEVEL INDEX!!\n" );
        return NULL;
    }
    switch (qtype)
    {
    case Reward::eBonusMission:
        {
            return mRewardsList[level].mBonusMission;
            break;
        }
    case Reward::eStreetRace:
        {
            return mRewardsList[level].mStreetRace;
            break;
        }
    case Reward::eCards:
        {
            return mRewardsList[level].mCards;
            break;
        }
    
    case Reward::eDefaultCar:
        {
            return mRewardsList[level].mDefaultCar;
            break;
        }
    case Reward::eDefaultSkin:
        {
            return mRewardsList[level].mDefaultSkin;
            break;
        }
    case Reward::eGoldCards:
        {
            return mRewardsList[level].mGoldCards;
            break;
        }
    default:
        {
            //you shouldnt get this unless you input an invalid quest type
            rAssert(0);
            return NULL;
        }
    }

    return NULL;
     
}//end of GetReward


Reward* RewardsManager::GetReward( int level, tUID id )
{
    // search through all quest types for given level to find reward w/
    // specified UID
    //
    for( int questType = (Reward::eBlank + 1); questType < Reward::NUM_QUESTS; questType++ )
    {
        Reward* pReward = this->GetReward( level, static_cast<Reward::eQuestType>( questType ) );
        if( pReward != NULL && pReward->GetUID() == id )
        {
            // found it!
            //
            return pReward;
        }
    }

    return NULL;
}

int RewardsManager::SetCarAttributes(char* carname,float speed,float acceleration,float toughness,float stability)
{
    //check if the array is full
    if (mCarAttributeRecordIndex >= MAX_CAR_ATTRIBUTE_RECORDS)
    {
        rTuneAssertMsg(0,"Cant Set any more Car Attributes the Records Array  is FULL!!!\n");
        return -1;
    }
    else
    {
        strncpy(mCarAttributeList[mCarAttributeRecordIndex].mName,carname,16);
        mCarAttributeList[mCarAttributeRecordIndex].mName[15]= '\0';
        mCarAttributeList[mCarAttributeRecordIndex].mSpeed = speed;
        mCarAttributeList[mCarAttributeRecordIndex].mAcceleration = acceleration;
        mCarAttributeList[mCarAttributeRecordIndex].mToughness =  toughness;
        mCarAttributeList[mCarAttributeRecordIndex].mStability = stability;
        mCarAttributeRecordIndex++;
        return 0;
    }
}

void RewardsManager::InitCarAttributeRecords( )
{
    for (int i=0;i<MAX_CAR_ATTRIBUTE_RECORDS;i++)
    {
        strcpy(mCarAttributeList[i].mName,"NULL");
        mCarAttributeList[i].mSpeed = 0.0f;
        mCarAttributeList[i].mAcceleration = 0.0f;
        mCarAttributeList[i].mToughness = 0.0f;
        mCarAttributeList[i].mStability = 0.0f;
    }
    mCarAttributeRecordIndex = 0;
}



CarAttributeRecord* RewardsManager::GetCarAttributeRecord(char* carname)
{
    rAssert( mScriptLoaded );

    for (int i=0;i <MAX_CAR_ATTRIBUTE_RECORDS;i++)
    {
        if (strcmp(mCarAttributeList[i].mName,carname) == 0 )
        {
            return &(mCarAttributeList[i]);
        }
    }
    return NULL;
}

float RewardsManager::ComputeOverallCarRating( CarAttributeRecord* carStats )
{
    rAssert( carStats != NULL );

    // TC: [TODO] need to get a designer to come up w/ a proper formula
    //
    return (carStats->mAcceleration + carStats->mSpeed + carStats->mStability + carStats->mToughness) / 4.0f;
}

int RewardsManager::GetNumberOfTokensInLevel(int level)
{
    return spInstance->mRewardsList[level].mMaxTokensInLevel;
}

void RewardsManager::SetTokensInLevel(int level,int tokens)
{
    spInstance->mRewardsList[level].mMaxTokensInLevel = tokens;
}

//returns the number of tokens a event should generate. Ie breaking glass,bee cam etc.
//I need to determine what to use as input. perhaps a event trigger is the best way.???
int RewardsManager::GenerateTokens( )
{
    //
    int tokens = 5;    

    return tokens;
}

void RewardsManager::InitTokenStore()
{
    for (int i =0; i<MAX_LEVELS;i++)
    {
        mLevelTokenStoreList[i].mCounter =0;
        for (int j=0;j<MAX_INVENTORY;j++)
        {
            mLevelTokenStoreList[i].mInventoryList[j] = new (GMA_PERSISTENT) Merchandise;
        }

        mLevelTokenStoreSearchIndex[ i ] = 0;
    }
}


int RewardsManager::AddMerchandise(char* name,char* filename,Reward::eRewardType rtype,int level,int cost,Merchandise::eSellerType sellerType)
{

    //check if the array of merchandise is full
   if(mLevelTokenStoreList[level].mCounter < MAX_INVENTORY)
   {
        mLevelTokenStoreList[level].mInventoryList[mLevelTokenStoreList[level].mCounter]->SetName(name);
        mLevelTokenStoreList[level].mInventoryList[mLevelTokenStoreList[level].mCounter]->SetLevel(level);
        mLevelTokenStoreList[level].mInventoryList[mLevelTokenStoreList[level].mCounter]->SetFilename(filename);
        mLevelTokenStoreList[level].mInventoryList[mLevelTokenStoreList[level].mCounter]->SetRewardType(rtype);
        mLevelTokenStoreList[level].mInventoryList[mLevelTokenStoreList[level].mCounter]->SetCost(cost);
        mLevelTokenStoreList[level].mInventoryList[mLevelTokenStoreList[level].mCounter]->SetSellerType(sellerType);
        mLevelTokenStoreList[level].mCounter++;
        return 0;
   }
   else
   {
       //list is full return -1 
       return -1;
   }          
    
}


TokenStoreInventory* RewardsManager::GetTokenStoreInventoryList(int level)
{
    LevelCheck(level);
    return &(mLevelTokenStoreList[level]);
}


Merchandise* RewardsManager::GetMerchandise(int level,const char* name)
{
    LevelCheck(level);
    for (int i=0; i< mLevelTokenStoreList[level].mCounter;i++)
    {
        if(strcmp (name,mLevelTokenStoreList[level].mInventoryList[i]->GetName())==0)
        {
            return mLevelTokenStoreList[level].mInventoryList[i];
        }
    }
    //got to the end of the array and no match return NULL
    return NULL;
}


Merchandise* RewardsManager::GetMerchandise(int level,int index)
{
    if ( (index >= 0 ) && (index < mLevelTokenStoreList[level].mCounter))
    {
        return mLevelTokenStoreList[level].mInventoryList[index];
    }
    else
    {
        //got to the end of the array and no match return NULL
        return NULL;
    }
}



int RewardsManager::GetMerchandiseIndex(int level,const char* name)
{
    LevelCheck(level);
    for (int i=0; i< mLevelTokenStoreList[level].mCounter;i++)
    {
        if(strcmp (name,mLevelTokenStoreList[level].mInventoryList[i]->GetName())==0)
        {
            return i;
        }
    }
    //got to the end of the array and no match return NULL
    return -1;
}

Merchandise*
RewardsManager::FindFirstMerchandise( int level, Merchandise::eSellerType sellerType )
{
    // reset search index
    //
    mLevelTokenStoreSearchIndex[ level ] = 0;

    return( this->FindNextMerchandise( level, sellerType ) );
}

Merchandise*
RewardsManager::FindNextMerchandise( int level, Merchandise::eSellerType sellerType )
{
    for( int i = mLevelTokenStoreSearchIndex[ level ]; i < mLevelTokenStoreList[level].mCounter; i++ )
    {
        // search for next merchandise w/ specified seller type
        //
        Merchandise* merchandise = this->GetMerchandise( level, i );
        if( merchandise->GetSellerType() == sellerType )
        {
            // found it! update current search index
            //
            mLevelTokenStoreSearchIndex[ level ] = i + 1;

            // return reference to merchandise
            //
            return merchandise;
        }
    }

    // merchandise not found, return NULL
    //
    return NULL;
}

bool RewardsManager::BuyMerchandise(int level,const char* name)
{
    Merchandise* pMerchandise =NULL;
    pMerchandise = GetMerchandise(level,name);

    if ( pMerchandise == NULL)
    {
        //you asserted here because you are trying to buy an item that doesn't exist
        rTuneAssert(0);
        return false;
    }
    
    //check if the player has enough tokens to buy the item.    
    int index = -1;
    index = GetMerchandiseIndex(level,name);

    //if index is -1 we have a problem. chuck
    rTuneAssert(index != -1);

    if (GetCharacterSheetManager()->GetNumberOfTokens((RenderEnums::LevelEnum) level) < pMerchandise->GetCost() )
    {
        //player doesn't have enough tokens to buy the item 
        return false;
    }
    else
    {
        pMerchandise->UnlockReward();
        GetCharacterSheetManager()->SubtractTokens((RenderEnums::LevelEnum) level,pMerchandise->GetCost());
        GetCharacterSheetManager()->SetPurchasedRewards( (RenderEnums::LevelEnum) level,index);
        return true;
    }
    
}
    
/*
bool RewardsManager::BuyMerchandise(int level,int index)
{
    Merchandise* pMerchandise =NULL;
    pMerchandise = GetMerchandise(level,index);

    if ( pMerchandise == NULL)
    {
        //you asserted here because you are trying to buy an item that doesn't exist
        rTuneAssert(0);
        return false;
    }
    else
    {
        //check if player has enough tokens to buy the object.
        if (GetCharacterSheetManager()->GetNumberOfTokens((RenderEnums::LevelEnum) level) < pMerchandise->GetCost() )
        {
            //player doesn't have enough tokens to buy the item 
            return false;
        }
        else
        {
            pMerchandise->UnlockReward();
            GetCharacterSheetManager()->SubtractTokens((RenderEnums::LevelEnum) level,pMerchandise->GetCost());
            GetCharacterSheetManager()->SetPurchasedRewards( (RenderEnums::LevelEnum) level,index);
            return true;
        }
    }
}
*/


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************


//==============================================================================
// RewardsManager::RewardsManager
//==============================================================================
//
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================// 
RewardsManager::RewardsManager() :
    mScriptLoaded( false )
{

   InitRewards();
   InitCarAttributeRecords();
   InitTokenStore();
   mUpdateQue = 0;
}


//==============================================================================
// RewardsManager::~RewardsManager
//==============================================================================
//
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================// 
RewardsManager::~RewardsManager()
{
    for (int i = 0; i < MAX_LEVELS+1; i++)
    {
        delete mRewardsList[i].mBonusMission;
        delete mRewardsList[i].mStreetRace;
        delete mRewardsList[i].mCards;       
        delete mRewardsList[i].mDefaultCar;
        delete mRewardsList[i].mDefaultSkin;
    }
    
    for (int i =0; i<MAX_LEVELS;i++)
    {
        spInstance->mLevelTokenStoreList[i].mCounter =0;
        for (int j=0;j<MAX_INVENTORY;j++)
        {
            delete mLevelTokenStoreList[i].mInventoryList[j];
        }
    }

    
}



void RewardsManager::NameCheck(char* name)
{
    rTuneAssertMsg(strlen( name ) < 16,"ERROR: Name is greater than 16 chars! \n");
}


void RewardsManager::FileNameCheck(char* filename)
{
    rTuneAssertMsg(strlen( filename ) < 64,"ERROR: FileName is greater than 64 chars! \n");
}

bool RewardsManager::LevelCheck(int level)
{
    bool success;
    if (level > -1 && level <= MAX_LEVELS)
        success = true;
    else
        success = false;

    rTuneAssertMsg(success,"Level is either less than 0 or Greater than 7! \n");

    return success;
}



//Inc the Internal Update Counter. This will be called from the LoadChararctersheet complete method.
void RewardsManager::IncUpdateQue()
{
    mUpdateQue++;
}

//The method is called from the FE context. 
int RewardsManager::SynchWithCharacterSheet()
{
    //if our que is greater than 0 than means the charactersheet has been loaded
    //so we needed call the charactersheetsmanager, and update the RewardManager

    if (mUpdateQue > 0)
    {
        GetCharacterSheetManager()->UpdateRewardsManager();
        mUpdateQue--;
        return 0;
    }
    else
    {
        return 0;
    }
}

// Set/Get Methods for Total Gags in Level
//
void
RewardsManager::SetTotalGags( int level, int numGags )
{
    spInstance->mRewardsList[ level ].mTotalGagsInLevel = numGags;
}

int
RewardsManager::GetTotalGags( int level ) const
{
    return spInstance->mRewardsList[ level ].mTotalGagsInLevel;
}

// Set/Get Methods for Total Wasps in Level
//
void
RewardsManager::SetTotalWasps( int level, int numWasps )
{
    spInstance->mRewardsList[ level ].mTotalWaspsInLevel = numWasps;
}

int
RewardsManager::GetTotalWasps( int level ) const
{
    return spInstance->mRewardsList[ level ].mTotalWaspsInLevel;
}

