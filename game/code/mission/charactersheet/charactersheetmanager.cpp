//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        charactersheetmanager.cpp
//
// Description: Implementation for the CharacterSheetManager class.
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
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/charactersheet/charactersheet.h>
#include <mission/rewards/rewardsmanager.h>

#include <cards/cardgallery.h>
#include <cheats/cheatinputsystem.h>
#include <data/gamedatamanager.h>
#include <events/eventmanager.h>
#include <main/commandlineoptions.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/vehiclecentral.h>
#include <p3d/entity.hpp>
#include <string.h>

#ifndef WORLD_BUILDER
#include <memory/srrmemory.h>
#else
#define MEMTRACK_PUSH_GROUP(x)
#define MEMTRACK_POP_GROUP(x)
#define GMA_PERSISTENT 0
#define GMA_DEFAULT 0
#define GMA_TEMP 0
#endif


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.

CharacterSheetManager* CharacterSheetManager::spInstance = NULL;


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// CharacterSheetManager::CreateInstance
//==============================================================================
//
// Description: Creates the CharacterSheetManager.
//
// Parameters:	None.
//
// Return:      Pointer to the CharacterSheetManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
CharacterSheetManager* CharacterSheetManager::CreateInstance()
{
    rAssert( spInstance == NULL );
MEMTRACK_PUSH_GROUP("CharacterSheetManager");

    spInstance = new (GMA_PERSISTENT) CharacterSheetManager;
    rAssert( spInstance );
    
MEMTRACK_POP_GROUP("CharacterSheetManager");
    return spInstance;
}

//==============================================================================
// CharacterSheetManager::GetInstance
//==============================================================================
//
// Description: - Access point for the CharacterSheetManager singleton.  
//
// Parameters:	None.
//
// Return:      Pointer to the CharacterSheetManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
CharacterSheetManager* CharacterSheetManager::GetInstance()
{
    rAssert( spInstance != NULL );
    
    return spInstance;
}


//==============================================================================
// CharacterSheetManager::DestroyInstance
//==============================================================================
//
// Description: Destroy the CharacterSheetManager.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void CharacterSheetManager::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete (GMA_PERSISTENT,spInstance);
    spInstance = NULL;
}



//set a  mission complete this includes StreetRaces and BonusMissions
void CharacterSheetManager::SetMissionComplete(RenderEnums::LevelEnum level,char* name, bool completed_secondary_objective,int seconds)
    {
        MissionRecord* p_missionrecord =NULL;
        
        //p_missionrecord = GetMissionRecord(level,name);     

        if ( GetMissionRecord(level,name)!= NULL)
        {
            p_missionrecord = GetMissionRecord(level,name);
            p_missionrecord->mCompleted =true;
            p_missionrecord->mBonusObjective = completed_secondary_objective;
        
        }
        else if ( GetStreetRaceRecord (level,name) != NULL )
        {
            p_missionrecord = GetStreetRaceRecord (level,name);
            p_missionrecord->mCompleted =true;
            p_missionrecord->mBonusObjective = completed_secondary_objective;
            
            if (seconds > 0)
            {
                //check if we have a legit best time or the intial default of -1
                if ( p_missionrecord->mBestTime < 0) 
                {  //default case so seconds becomes new best time
                    p_missionrecord->mBestTime = seconds;
                }
                else
                {
                    //we didnt beat our best time
                    if(p_missionrecord->mBestTime < seconds)
                    {
                        //do nothing
                    }
                    else
                    {
                        //we have a new best time
                        p_missionrecord->mBestTime=seconds;
                    }
                }
            }

            
            //we check after check mission complete if we have unlocked anything
            if (spInstance->QueryAllStreetRacesCompleted(level) == true)
            {
                Reward* pReward=GetRewardsManager()->GetReward(level,Reward::eStreetRace);
                //assert if we there isnt a reward for the streetrace.
                rTuneAssert(pReward != NULL);
                pReward->UnlockReward();
                GetEventManager()->TriggerEvent(EVENT_COMPLETED_ALLSTREETRACES);
            }


        }
        else if ( GetBonusMissionRecord (level,name ) != NULL)
        {
            p_missionrecord = GetBonusMissionRecord(level,name);
            p_missionrecord->mCompleted =true;
            p_missionrecord->mBonusObjective = completed_secondary_objective; 

             //we check after check mission complete if we have unlocked anything
            if (spInstance->QueryBonusMissionCompleted(level) == true)
            {
                Reward* pReward=GetRewardsManager()->GetReward(level,Reward::eBonusMission);
                //assert if we there isnt a reward for the BonusMission.
                rTuneAssert(pReward != NULL);
                pReward->UnlockReward();
                GetEventManager()->TriggerEvent(EVENT_COMPLETED_BONUSMISSIONS);
            }

        }
        else
        {
            // TC: this is now implemented in MissionStage::Update
            //
/*
            if(strcmp("gr1",name)==0)
            {
                mCharacterSheet.mLevelList[level].mGambleRace.mCompleted = true;
                
                if (seconds > 0)
                {
                    //check if we have a legit best time or the intial default of -1
                    if (  mCharacterSheet.mLevelList[level].mGambleRace.mBestTime < 0) 
                    {  //default case so seconds becomes new best time
                         mCharacterSheet.mLevelList[level].mGambleRace.mBestTime = seconds;
                    }
                    else
                    {
                        //we didnt beat our best time
                        if( mCharacterSheet.mLevelList[level].mGambleRace.mBestTime < seconds)
                        {
                            //do nothing
                        }
                        else
                        {
                            //we have a new best time
                             mCharacterSheet.mLevelList[level].mGambleRace.mBestTime = seconds;
                        }
                    }
                }

            }
            else
            {
                //Mission not found in character sheet trouble
                rReleasePrintf("trouble mission %s not found in charactersheet \n",name);
               // rAssert(0);
            }
*/
        }
    }

    //inc the number of attempts
    void CharacterSheetManager::IncrementMissionAttempt(RenderEnums::LevelEnum level, char* name)
    {
        MissionRecord* pMissionRecord =NULL;
        pMissionRecord = spInstance->GetMissionRecord(level,name);

        if ( pMissionRecord != NULL)
        {
            pMissionRecord->mNumAttempts++;
        }
        else
        {
            //mission not found prolly a sunday drive or something
        }
    }

    //update the mission as user chose to skip it
    void CharacterSheetManager::SetMissionSkipped(RenderEnums::LevelEnum level,char* name)
    {
        MissionRecord* pMissionRecord =NULL;
        pMissionRecord = spInstance->GetMissionRecord(level,name);

        if ( pMissionRecord != NULL)
        {
            pMissionRecord->mSkippedMission = true;
        }
        else
        {
            //mission not found prolly a sunday drive or something
            //rAssertMsg(0,"Mission:  not found \n");
            
        }
    }

    void CharacterSheetManager::SetMissionSkipped(RenderEnums::LevelEnum level,RenderEnums::MissionEnum mission)
    {
        MissionRecord* pMissionRecord =NULL;
        pMissionRecord = spInstance->GetMissionRecord(level,mission);

        if ( pMissionRecord != NULL)
        {
            pMissionRecord->mSkippedMission = true;
        }
        else
        {
            //mission not found prolly a sunday drive or something
            //rAssertMsg(0,"Mission:  not found \n");
            
        }
    }

    //returns the number of attempts the use tried to complete mission
    int CharacterSheetManager::QueryNumberOfAttempts(RenderEnums::LevelEnum level,int index)
    {
        MissionRecord* pMissionRecord =NULL;
        pMissionRecord = spInstance->GetMissionRecord(level,index);

        if ( pMissionRecord != NULL)
        {
            //dividing by 2 since number of attempts is incremented at mission reset, and all mission get reset twice for whatever 
            //reason
            return (pMissionRecord->mNumAttempts / 2) ;
        }
        else
        {
            rAssertMsg(0,"Mission: not found \n");
            return -666;
        }
    }




         

//get a ptr to the record, you shouldn't  modify the record though, use the set methods to modify instead
MissionRecord* CharacterSheetManager::QueryMissionStatus(RenderEnums::LevelEnum level, char*  name)
    {
        return (GetMissionRecord(level,name) );
    }

//get a ptr to the record, you shouldn't  modify the record though, use the set methods to modify instead
MissionRecord* CharacterSheetManager::QueryMissionStatus(RenderEnums::LevelEnum level, int index)
    {
        return (GetMissionRecord(level,index) );
    }

//get a ptr to the record, you shouldn't  modify the record though, use the set methods to modify instead
MissionRecord* CharacterSheetManager::QueryStreetRaceStatus(RenderEnums::LevelEnum level, char*  name)
    {
        return (GetStreetRaceRecord(level,name) );
    }

    MissionRecord* CharacterSheetManager::QueryBonusMissionStatus( RenderEnums::LevelEnum level, char* name )
    {
        return GetBonusMissionRecord( level, name );        
    }



bool CharacterSheetManager::QueryAllCardsCollected(RenderEnums::LevelEnum level )
    {
        for (int i=0;i<MAX_CARDS;i++)
            {
                //cycle through the list of cards, check if they have been collected
                if( spInstance->mCharacterSheet.mLevelList[level].mCard.mList[i].mCompleted )
                    {
                        //do nothing
                    }
                else
                    {  
                        //we have come across a card that hasn't been collected return false
                        return false;
                    }
            }
        //checked the entire list and didnt encounter a card that wasnt collected so we return true
        return true;
    }




    // method returns boolean of bonus mission
    bool CharacterSheetManager::QueryBonusMissionCompleted(RenderEnums::LevelEnum level)
    {
        return spInstance->mCharacterSheet.mLevelList[level].mBonusMission.mCompleted != 0;
    }

    
    //returns true if we cycle the entire list of streetraces and do not encounter a streetrace thats not completed
    bool CharacterSheetManager::QueryAllStreetRacesCompleted(RenderEnums::LevelEnum level)
    {
        //cycle throught the streetraces
        for (int i =0; i<MAX_STREETRACES;i++)
        {
            //check for incomplete races, if found return false
            if ( spInstance->mCharacterSheet.mLevelList[level].mStreetRace.mList[i].mCompleted == false)
            {
                return false;
            }
        }

        return true;
    }



   
         




//Fill the Character Sheet with default data.
void CharacterSheetManager::InitCharacterSheet()
    {
        for (int i = 0; i<MAX_LEVELS;i++)
        {
            int j ;
            for (j=0;j<MAX_CARDS;j++)            
                {
                    strcpy(mCharacterSheet.mLevelList[i].mCard.mList[j].mName, "NULL");
                    mCharacterSheet.mLevelList[i].mCard.mList[j].mCompleted = false;
                }
           
            for (j=0;j<MAX_MISSIONS;j++)
                {
                    strcpy(mCharacterSheet.mLevelList[i].mMission.mList[j].mName,"NULL");
                    mCharacterSheet.mLevelList[i].mMission.mList[j].mCompleted = false;
                    mCharacterSheet.mLevelList[i].mMission.mList[j].mBonusObjective = false;
                    mCharacterSheet.mLevelList[i].mMission.mList[j].mSkippedMission = false;
                    mCharacterSheet.mLevelList[i].mMission.mList[j].mNumAttempts = 0;
                    mCharacterSheet.mLevelList[i].mMission.mList[j].mBestTime = -1;
                }
            for (j=0;j<MAX_STREETRACES;j++)
                {
                    strcpy(mCharacterSheet.mLevelList[i].mStreetRace.mList[j].mName , "NULL");
                    mCharacterSheet.mLevelList[i].mStreetRace.mList[j].mCompleted = false;
                    mCharacterSheet.mLevelList[i].mStreetRace.mList[j].mBonusObjective = false;
                    mCharacterSheet.mLevelList[i].mStreetRace.mList[j].mNumAttempts =0;
                    mCharacterSheet.mLevelList[i].mStreetRace.mList[j].mBestTime = -1;

                }
            //Defaults
            strcpy(mCharacterSheet.mLevelList[i].mBonusMission.mName,"NULL");
            //Bonus Mission
            mCharacterSheet.mLevelList[i].mBonusMission.mCompleted =false;
            mCharacterSheet.mLevelList[i].mBonusMission.mBonusObjective =false;
            mCharacterSheet.mLevelList[i].mBonusMission.mNumAttempts = 0;
            mCharacterSheet.mLevelList[i].mBonusMission.mBestTime = -1;

            //Gamble Race
            mCharacterSheet.mLevelList[i].mGambleRace.mCompleted =false;
            mCharacterSheet.mLevelList[i].mGambleRace.mBonusObjective =false;
            mCharacterSheet.mLevelList[i].mGambleRace.mNumAttempts = 0;
            mCharacterSheet.mLevelList[i].mGambleRace.mBestTime = -1;
            
            //Current skin
            strcpy(mCharacterSheet.mLevelList[i].mCurrentSkin,"NULL");

            mCharacterSheet.mLevelList[i].mFMVunlocked = false;
            mCharacterSheet.mLevelList[i].mNumCarsPurchased =0;
            mCharacterSheet.mLevelList[i].mNumSkinsPurchased =0;
            mCharacterSheet.mLevelList[i].mWaspsDestroyed =0;
            mCharacterSheet.mLevelList[i].mGagsViewed = 0;
            mCharacterSheet.mLevelList[i].mGagMask = 0;


            //set default for purchased rewards.
            for(j=0;j<MAX_PURCHASED_ITEMS;j++)
            {
                mCharacterSheet.mLevelList[i].mPurchasedRewards[j]=false;
            }
        }

        //top level defaults
#ifdef RAD_DEMO
        mCharacterSheet.mNumberOfTokens = 500;
#else
        if( CommandLineOptions::Get( CLO_COINS ) ||
            GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_EXTRA_COINS ) )
        {
            mCharacterSheet.mNumberOfTokens = 1000; // start game w/ some money
        }
        else
        {
            mCharacterSheet.mNumberOfTokens = 0;
        }
#endif // RAD_DEMO

        strcpy(mCharacterSheet.mPlayerName,"Player1");
        mCharacterSheet.mCurrentMissionInfo.mLevel = RenderEnums::L1;
        mCharacterSheet.mCurrentMissionInfo.mMissionNumber = RenderEnums::M1;
        mCharacterSheet.mHighestMissionPlayed.mLevel = RenderEnums::L1;
        mCharacterSheet.mHighestMissionPlayed.mMissionNumber = RenderEnums::M1;
        
        for(int i=0;i<MAX_CARS_OWNED;i++)
        {
            mCharacterSheet.mCarInventory.mCars[i].mCurrentHealth = -1.0f;
            mCharacterSheet.mCarInventory.mCars[i].mMaxHealth = -1.0f;
            strcpy(mCharacterSheet.mCarInventory.mCars[i].mName,"n/a");
        }
        mCharacterSheet.mCarInventory.mCounter=0;


/*        
        //setting the player preference option defaults
        mCharacterSheet.mPlayerOptions.mRumble = true;
        mCharacterSheet.mPlayerOptions.mDialogVolume = 0.5;
        mCharacterSheet.mPlayerOptions.mMusicVolume = 0.5;
        mCharacterSheet.mPlayerOptions.mSFXVolume = 0.5;
        mCharacterSheet.mPlayerOptions.mDriverCam = SuperCam::FOLLOW_CAM;
        mCharacterSheet.mPlayerOptions.mInvertCamera = false;
        mCharacterSheet.mPlayerOptions.mNavSystemOn = true;
        mCharacterSheet.mPlayerOptions.mRadarOn = true;
        mCharacterSheet.mPlayerOptions.mTutorialOn = true;

        mCharacterSheet.mTutoiralsSeen.mBitfield = 0x00;
*/
        mCharacterSheet.mIsNavSystemEnabled = true;

        memset( mCharacterSheet.mPersistentObjectStates, 0xFF, NUM_BYTES_FOR_PERSISTENT_STATES );
        memset(mCharacterSheet.mStates, 0x00, NUM_BYTES_FOR_STATES);
    }

//this is called from the missionscriptloader to fill in charactersheet                
int CharacterSheetManager::AddMission(RenderEnums::LevelEnum level, char* name)
    {
        //if mission is not in the list then
       if (GetMissionRecord(level,name) == NULL)       
           {
                //check to see if the slot is open
                for ( int i =0;i<MAX_MISSIONS;i++)
                    {
                        //look for an empty slot 
                        if (strcmp(mCharacterSheet.mLevelList[level].mMission.mList[i].mName,"NULL") == 0 )
                            {
                                if(strlen (name) > 16)
                                    {
                                        //name too big
                                        rAssert(0);
                                    }
                                strcpy(mCharacterSheet.mLevelList[level].mMission.mList[i].mName,name);
                                mCharacterSheet.mLevelList[level].mMission.mList[i].mName[15]='\0';
//                                mCharacterSheet.mLevelList[level].mMission.mList[i].mCompleted = false;
                                mCharacterSheet.mLevelList[level].mMission.mList[i].mBonusObjective = false;
                                mCharacterSheet.mLevelList[level].mMission.mList[i].mNumAttempts =0;
                                return 0;
                            }//end if                                                  
                    }//end for
                //return -1 list is full
                return -1;
                rAssert(0);
           }
       else
           {
                return 0;

           }
    }//end function


//this is called from the missionscriptloader to fill in charactersheet                
int CharacterSheetManager::AddStreetRace(RenderEnums::LevelEnum level, char* name)
{
    //if mission is not in the list then
    if (GetStreetRaceRecord(level,name) == NULL)       
        {
            //check to see if the slot is open
            for ( int i =0;i<MAX_STREETRACES;i++)
                {
                    //look for an empty slot 
                    if (strcmp(mCharacterSheet.mLevelList[level].mStreetRace.mList[i].mName,"NULL") == 0 )
                        {
                            if(strlen (name) > 16)
                                {
                                    //name too big
                                    rAssert(0);
                                }
                            strcpy(mCharacterSheet.mLevelList[level].mStreetRace.mList[i].mName,name);
                            mCharacterSheet.mLevelList[level].mStreetRace.mList[i].mName[15]='\0';
//                            mCharacterSheet.mLevelList[level].mStreetRace.mList[i].mCompleted = false;
                            mCharacterSheet.mLevelList[level].mStreetRace.mList[i].mBonusObjective = false;
                            mCharacterSheet.mLevelList[level].mStreetRace.mList[i].mNumAttempts =0;
                            mCharacterSheet.mLevelList[level].mStreetRace.mList[i].mBestTime = -1;
                            return 0;
                        }//end if                                                  
                }//end for
            //return -1 list is full
            return -1;
        }
    else
        {
            return 0;
        }
}//end function


    //this is called from the missionscriptloader to fill in charactersheet                
int CharacterSheetManager::AddBonusMission(RenderEnums::LevelEnum level, char* name)
    {

        
        //if mission is not in the list then
       if (GetBonusMissionRecord(level,name) == NULL)       
           {                
                //look for an empty slot 
                if (strcmp(mCharacterSheet.mLevelList[level].mBonusMission.mName,"NULL") == 0 )
                    {
                        if(strlen (name) > 16)
                            {
                                //name too big
                                rAssert(0);
                            }
                        strcpy(mCharacterSheet.mLevelList[level].mBonusMission.mName,name);
                        mCharacterSheet.mLevelList[level].mBonusMission.mName[15]='\0';
//                        mCharacterSheet.mLevelList[level].mBonusMission.mCompleted = false;
                        mCharacterSheet.mLevelList[level].mBonusMission.mBonusObjective = false;
                        mCharacterSheet.mLevelList[level].mBonusMission.mNumAttempts =0;
                        return 0;
                    }//end if                                                  
                    
                //return -1 list is full
                return -1;
           }
       else
           {
                return 0;
           }

           
        return 0;

    }//end function

    //Add a card the Card List for a level            
int CharacterSheetManager::AddCard(RenderEnums::LevelEnum level, char* name)
    {
        //if mission is not in the list then
       if (GetCollectableRecord(level,eCard,name) == NULL)       
           {
                //check to see if the slot is open
                for ( int i =0;i<MAX_CARDS;i++)
                    {
                        //look for an empty slot 
                        if (strcmp(mCharacterSheet.mLevelList[level].mCard.mList[i].mName,"NULL") == 0 )
                            {
                                if(strlen (name) > 16)
                                    {
                                        //name too big
                                        rAssert(0);
                                    }
                                strcpy(mCharacterSheet.mLevelList[level].mCard.mList[i].mName,name);
                                mCharacterSheet.mLevelList[level].mCard.mList[i].mName[15]='\0';
                                mCharacterSheet.mLevelList[level].mCard.mList[i].mCompleted = false;
                                return 0;
                            }//end if                                                  
                    }//end for
                //return -1 list is full
                return -1;
           }
       else
           {
               //card already exists.
                return -2;
           }
    }//end function


    //Add a card the Card List for a level, insert by index
    //danger this type of insertion has less safeguards than insert by name
int CharacterSheetManager::AddCard(RenderEnums::LevelEnum level, int index)
    {
        Record* pRecord = spInstance->GetCollectableRecord(level,eCard,index);
        
        if ( pRecord == NULL)
        {
            //trouble retrieving record
            rAssert(0);
            return -1;
        }
        else
        {
            if ( strcmp (pRecord->mName,"NULL") !=0 )
            {
                rAssertMsg(0,"Warning: You are attempting to overwriting an existing Record! \n");
                strcpy(pRecord->mName,"Cardx");
                pRecord->mName[15]='\0';
                pRecord->mCompleted = false;
                return 0;
            }
            else
            {
                strcpy(pRecord->mName,"Cardx");
                pRecord->mName[15]='\0';
                pRecord->mCompleted = true;
                return 0;
            }
        }
    }//end function



//memory card load method
void CharacterSheetManager::LoadData(const GameDataByte* dataBuffer,unsigned int numBytes )
    {  
        memcpy(&mCharacterSheet,dataBuffer,sizeof(CharacterSheet));
        GetRewardsManager()->IncUpdateQue();
        int size = sizeof(CharacterSheet);
    }

    
//memory card save method

void CharacterSheetManager::SaveData(GameDataByte* dataBuffer,unsigned int numBytes )
    {
        if( CommandLineOptions::Get( CLO_MEMCARD_CHEAT ) )
        {
            rReleasePrintf( "Saving data w/ memory card cheat ... ...\n" );

            HeapMgr()->PushHeap( GMA_TEMP );

            CharacterSheet* tempCharacterSheet = new CharacterSheet;
            rAssert( tempCharacterSheet != NULL );

            memcpy( tempCharacterSheet, &mCharacterSheet, sizeof( CharacterSheet ) );

            tempCharacterSheet->mHighestMissionPlayed.mLevel = RenderEnums::L7;
            tempCharacterSheet->mHighestMissionPlayed.mMissionNumber = RenderEnums::M7;
            tempCharacterSheet->mNumberOfTokens = 1000;

            // unlock items for all levels
            //
            for( int i = 0; i < MAX_LEVELS; i++ )
            {
                int j = 0;

                // unlock all missions
                for( j = 0; j < (i == RenderEnums::L1 ? MAX_MISSIONS : MAX_MISSIONS - 1); j++ )
                {
                    tempCharacterSheet->mLevelList[ i ].mMission.mList[ j ].mCompleted = true;
                    tempCharacterSheet->mLevelList[ i ].mMission.mList[ j ].mNumAttempts = 1;
                }

                // unlock all street races
                for( j = 0; j < MAX_STREETRACES; j++ )
                {
                    tempCharacterSheet->mLevelList[ i ].mStreetRace.mList[ j ].mCompleted = true;
                    tempCharacterSheet->mLevelList[ i ].mStreetRace.mList[ j ].mNumAttempts = 1;
                }

                // unlock bonus mission
                tempCharacterSheet->mLevelList[ i ].mBonusMission.mCompleted = true;
                tempCharacterSheet->mLevelList[ i ].mBonusMission.mNumAttempts = 1;

                // unlock movie
                tempCharacterSheet->mLevelList[ i ].mFMVunlocked = true;

                // unlock merchandise skins and cars
                const int MAX_CARS = 3;
                const int MAX_SKINS = 3;
                tempCharacterSheet->mLevelList[ i ].mNumCarsPurchased = MAX_CARS;
                tempCharacterSheet->mLevelList[ i ].mNumSkinsPurchased = MAX_SKINS;
                for( j = 0; j < (MAX_CARS + MAX_SKINS); j++ )
                {
                    tempCharacterSheet->mLevelList[ i ].mPurchasedRewards[ j ] = true;
                }

                // unlock cards
                for( j = 0; j < MAX_CARDS; j++ )
                {
                    tempCharacterSheet->mLevelList[ i ].mCard.mList[ j ].mCompleted = true;
                }

                // unlock gags
                tempCharacterSheet->mLevelList[ i ].mGagsViewed = GetRewardsManager()->GetTotalGags( i );

                // unlock wasps
                tempCharacterSheet->mLevelList[ i ].mWaspsDestroyed = GetRewardsManager()->GetTotalWasps( i );
            }

            memcpy( dataBuffer, tempCharacterSheet, sizeof( CharacterSheet ) );

            if( tempCharacterSheet != NULL )
            {
                delete tempCharacterSheet;
                tempCharacterSheet = NULL;
            }

            HeapMgr()->PopHeap( GMA_TEMP );
        }
        else
        {
            memcpy(dataBuffer,&mCharacterSheet,sizeof(CharacterSheet));
        }
    }
   

//updates the charactersheets current mission struct

    void CharacterSheetManager::SetCurrentMission(RenderEnums::LevelEnum level,RenderEnums::MissionEnum mission_number)
    {
        if( ( level == RenderEnums::L1 && mission_number == RenderEnums::M9 ) ||
            ( level != RenderEnums::L1 && mission_number == RenderEnums::M8 ) )
        {
            // set level and mission to next level's first mission (and wrap back to
            // L1 M2 if last mission of the game)
            //
            int nextLevel = (level + 1) % RenderEnums::numLevels;
            level = static_cast<RenderEnums::LevelEnum>( nextLevel );
            mission_number = (level != RenderEnums::L1) ? RenderEnums::M1 : RenderEnums::M2;
        }

        spInstance->mCharacterSheet.mCurrentMissionInfo.mLevel = level;
        spInstance->mCharacterSheet.mCurrentMissionInfo.mMissionNumber = mission_number;

        // update highest mission played
        //
        if( static_cast<int>( level ) > static_cast<int>( spInstance->mCharacterSheet.mHighestMissionPlayed.mLevel ) )
        {
            spInstance->mCharacterSheet.mHighestMissionPlayed.mLevel = level;
            spInstance->mCharacterSheet.mHighestMissionPlayed.mMissionNumber = mission_number;
        }
        else if( level == spInstance->mCharacterSheet.mHighestMissionPlayed.mLevel )
        {
            if( static_cast<int>( mission_number ) > static_cast<int>( spInstance->mCharacterSheet.mHighestMissionPlayed.mMissionNumber ) )
            {
                spInstance->mCharacterSheet.mHighestMissionPlayed.mMissionNumber = mission_number;
            }
        }
    }


    //method for collecting stuff
    void CharacterSheetManager::SetCardCollected(RenderEnums::LevelEnum level,char* name)
    {
        Record* pCollectableRecord = spInstance->GetCollectableRecord(level,eCard,name);
        rAssert(pCollectableRecord);        
        pCollectableRecord->mCompleted = true;
        
        //check if we have collected all cards
        if( spInstance->QueryAllCardsCollected(level) == true)
        {
            Reward* pReward = GetRewardsManager()->GetReward(level,Reward::eCards);
            //Assert if no reward found
            rTuneAssert(pReward != NULL);
            pReward->UnlockReward();
            GetEventManager()->TriggerEvent(EVENT_COLLECTED_ALLCARDS);
        }

    }

    
    void CharacterSheetManager::SetCardCollected(RenderEnums::LevelEnum level,int index)
    {
        Record* pCollectableRecord = spInstance->GetCollectableRecord(level,eCard,index);
        rAssert(pCollectableRecord);        
        pCollectableRecord->mCompleted = true;
         
        //check if we have collected all cards
        if( spInstance->QueryAllCardsCollected(level) == true)
        {
            Reward* pReward = GetRewardsManager()->GetReward(level,Reward::eCards);
            //Assert if no reward found
            rTuneAssert(pReward != NULL);
            pReward->UnlockReward();
            GetEventManager()->TriggerEvent(EVENT_COLLECTED_ALLCARDS);
        }
    }

 

    //Returns the number of tokens that the character has in their inventory for that level
    int CharacterSheetManager::GetNumberOfTokens(RenderEnums::LevelEnum level)
    {
        return spInstance->mCharacterSheet.mNumberOfTokens;
    }

    //Set the number of tokens in the character sheet of that level to number
    void CharacterSheetManager::SetNumberOfTokens(RenderEnums::LevelEnum level,int number)
    {
        if( number >= 0 )
        {
            spInstance->mCharacterSheet.mNumberOfTokens;
        }
    }

    //add number to the current number of tokens
    void CharacterSheetManager::AddTokens(RenderEnums::LevelEnum level,int number)
    {
        spInstance->mCharacterSheet.mNumberOfTokens += number;

        if( spInstance->mCharacterSheet.mNumberOfTokens < 0 )
        {
            spInstance->mCharacterSheet.mNumberOfTokens = 0;
        }
    }
    
    //subtracts the number from the current number of tokens
    void CharacterSheetManager::SubtractTokens(RenderEnums::LevelEnum level,int number)
    {
        spInstance->mCharacterSheet.mNumberOfTokens -= number;

        if( spInstance->mCharacterSheet.mNumberOfTokens < 0 )
        {
            spInstance->mCharacterSheet.mNumberOfTokens = 0;
        }
    }

    //returns the current mission mission info
CurrentMissionStruct CharacterSheetManager::QueryCurrentMission()
    {
        return spInstance->mCharacterSheet.mCurrentMissionInfo;
    }

CurrentMissionStruct CharacterSheetManager::QueryHighestMission()
    {
        return spInstance->mCharacterSheet.mHighestMissionPlayed;
    }
    
    //use this to query if player has collect a card. This should be called at load time so we dont create the card uneccessarily
    bool CharacterSheetManager::QueryCardCollected(RenderEnums::LevelEnum level,char* CardName)
    {
        Record* pRecord = NULL;

        pRecord = spInstance->GetCollectableRecord(level,eCard,CardName);
        if ( pRecord != NULL)
        {
            return pRecord->mCompleted != 0;
        }
        else
        {
            rAssertMsg(0,"Card Name doesnt Exists\n");
            return false;
        }
    }

    //use this to query if player has collect a card. This should be called at load time so we dont create the card uneccessarily
    bool CharacterSheetManager::QueryCardCollected(RenderEnums::LevelEnum level,int index)
    {
        Record* pRecord = NULL;

        pRecord = spInstance->GetCollectableRecord(level,eCard,index);
        if ( pRecord != NULL)
        {
            return pRecord->mCompleted != 0;
        }
        else
        {
            rAssertMsg(0,"Card Name doesnt Exists\n");
            return false;
        }
    }

  
/*
    //get the player settings
    bool CharacterSheetManager::QueryRumble()
    {
        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        return pPlayerOptions->mRumble;
    }

    float CharacterSheetManager::QuerySFXVolume()
    {
        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        return pPlayerOptions->mSFXVolume;
    }

    
    float CharacterSheetManager::QueryMusicVolume()
    {
        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        return pPlayerOptions->mMusicVolume;
    }

    
    float CharacterSheetManager::QueryDialogVolume()
    {
        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        return pPlayerOptions->mDialogVolume;
    }

    SuperCam::Type CharacterSheetManager::QueryDriverCam()
    {
        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        return pPlayerOptions->mDriverCam;
    }

    bool CharacterSheetManager::QueryInvertedCameraSetting()
    {
        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        return pPlayerOptions->mInvertCamera;
    }

    bool CharacterSheetManager::QueryRadarSetting()
    {

        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        return pPlayerOptions->mRadarOn;
    }

    bool CharacterSheetManager::QueryTutorialSetting()
    {
        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        return pPlayerOptions->mTutorialOn;
    }

//methods for setting the player setting at the FE or PM
    void CharacterSheetManager::SetRumble(bool setting)
    {
        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        pPlayerOptions->mRumble = setting;
    }

    void CharacterSheetManager::SetSFXVolume(float volume)
    {
        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        pPlayerOptions->mSFXVolume = volume;
    }

     void CharacterSheetManager::SetMusicVolume(float volume)
    {
        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        pPlayerOptions->mMusicVolume = volume;
    }

     void CharacterSheetManager::SetDialogVolume(float volume)
    {
        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        pPlayerOptions->mDialogVolume = volume;
    }

    void CharacterSheetManager::SetDriverCamera(SuperCam::Type camera)
    {
        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        pPlayerOptions->mDriverCam = camera;
    }

    void CharacterSheetManager::SetTutorialOn(bool setting)
    {
        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        pPlayerOptions->mTutorialOn = setting;
    }

    void CharacterSheetManager::SetRadarOn(bool setting)
    {
        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        pPlayerOptions->mRadarOn = setting;
    }
    void CharacterSheetManager::SetInvertedCamera(bool setting)
    {
        PlayerOptions* pPlayerOptions = spInstance->GetPlayerOptions();
        rAssert(pPlayerOptions);
        pPlayerOptions->mInvertCamera = setting;
    }
*/
    bool CharacterSheetManager::QueryNavSystemSetting()
    {
        return mCharacterSheet.mIsNavSystemEnabled != 0;
    }

    void CharacterSheetManager::SetNavSystemOn(bool setting)
    {
        mCharacterSheet.mIsNavSystemEnabled = setting;
    }
        
    //Called from the Rewards Manager, basically records which reward the player has purchased from the RewardManager's token store 
    void CharacterSheetManager::SetPurchasedRewards(RenderEnums::LevelEnum level,int index)
    {
        mCharacterSheet.mLevelList[level].mPurchasedRewards[index] = true;
        
        Merchandise* pmerchandise = NULL;
        //update the nNumCarsPurchased or nNumSkinsPurchased
        pmerchandise = GetRewardsManager()->GetMerchandise( (int) level, index);
        
        //if NULL then We got a problem see Chuck!       
        rTuneAssert(pmerchandise != NULL);

        if (pmerchandise->GetRewardType() == Reward::ALT_SKIN_OTHER)
        {
            spInstance->mCharacterSheet.mLevelList[level].mNumSkinsPurchased++;
        }
        else if ( pmerchandise->GetRewardType() == Reward::ALT_PLAYERCAR)
        {
            spInstance->mCharacterSheet.mLevelList[level].mNumCarsPurchased++;
            //spInstance->AddCarToInventory(pmerchandise->GetName(),eNoDamage, -10.0f);
        }
        else
        {
            //unknown type of reward being purchased
            rAssert(0);
        }
    }


    

    bool CharacterSheetManager::QueryPurchasedReward(RenderEnums::LevelEnum level, int index)
    {
        if ((index > 0 ) && (index<MAX_PURCHASED_ITEMS))
        {
            return  mCharacterSheet.mLevelList[level].mPurchasedRewards[index] != 0;
        }
        else
        {
            //invalid index as input
            rTuneAssert(0);
            return false;
        }
    }

    //add a car to carinventory to the charactersheet
    int CharacterSheetManager::AddCarToInventory( const char* Name )
    {
        rAssertMsg( mCharacterSheet.mCarInventory.mCounter < MAX_CARS_OWNED, "Car inventory is full.\n" );
        int i = 0;
        Vehicle* pVehicle = NULL;
        for( i = 0; i < mCharacterSheet.mCarInventory.mCounter; ++i )
        {
            if( strcmp( mCharacterSheet.mCarInventory.mCars[ i ].mName, Name ) == 0 )
            {
                return i;
            }
        }
        mCharacterSheet.mCarInventory.mCars[ i ].mCurrentHealth = 1.0f;
        
        pVehicle = GetVehicleCentral()->GetVehicleByName(Name);

       

        //check if vehicle is intialized already.
        if ( pVehicle != NULL)
        {
            mCharacterSheet.mCarInventory.mCars[ i ].mMaxHealth = pVehicle->mDesignerParams.mHitPoints;
        }
        strcpy( mCharacterSheet.mCarInventory.mCars[ i ].mName, Name );
        ++mCharacterSheet.mCarInventory.mCounter;
        return i;
    }

    int CharacterSheetManager::GetCarIndex( const char* Name )
    {
        // Simple sequential search.
        for( int i = 0; i < mCharacterSheet.mCarInventory.mCounter; ++i )
        {
            if( strcmp( mCharacterSheet.mCarInventory.mCars[ i ].mName, Name ) == 0 )
            {
                return i;
            }
        }
        return -1;
    }

    //Return Number of Cars player has in there inventory
    int CharacterSheetManager::GetNumberOfCarsInInventory()
    {
        return mCharacterSheet.mCarInventory.mCounter;
    }


    //Returns The cars state or eNull if it can't find the car.
    CarDamageStateEnum CharacterSheetManager::GetCarDamageState( int CarIndex )
    {
        if( CarIndex < 0 || CarIndex >= mCharacterSheet.mCarInventory.mCounter )
        {
            return eNull;
        }
        else if( mCharacterSheet.mCarInventory.mCars[ CarIndex ].mCurrentHealth >= 1.0f )
        {
            return eNoDamage;
        }
        else if( mCharacterSheet.mCarInventory.mCars[ CarIndex ].mCurrentHealth <= 0.0f )
        {
            return eHusk;
        }
        else
        {
            return eDamaged;
        }
    }

    // Returns the Car's health, or -1 if index out of range.
    float CharacterSheetManager::GetCarHealth( int CarIndex )
    {
        if( CarIndex < 0 || CarIndex >= mCharacterSheet.mCarInventory.mCounter )
        {
            return -1.0f;
        }
        return mCharacterSheet.mCarInventory.mCars[ CarIndex ].mCurrentHealth;
    }

     void CharacterSheetManager::UpdateCarHealth( int CarIndex, float Health )
    {
        if( CarIndex >= 0 && CarIndex < mCharacterSheet.mCarInventory.mCounter )
        {
            mCharacterSheet.mCarInventory.mCars[ CarIndex ].mCurrentHealth = Health;
        }
    }




//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************


//==============================================================================
// CharacterSheetManager::CharacterSheetManager
//==============================================================================
//
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================// 
CharacterSheetManager::CharacterSheetManager()
{   
   GetGameDataManager()->RegisterGameData( this, sizeof( CharacterSheet ),
                                           "Character Sheet" );
}


//==============================================================================
// CharacterSheetManager::~CharacterSheetManager
//==============================================================================
//
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================// 
CharacterSheetManager::~CharacterSheetManager()
{
    
}




//Get a ptr to a mission record for missions
MissionRecord* CharacterSheetManager::GetMissionRecord(RenderEnums::LevelEnum level, char* name)
    {
        //check to see if name array is too long
        if (strlen (name) > 16)
            {
                //you asserted here cause your name array is greater than 64 bytes
                rAssert(0);
            }
        for (int i=0;i<MAX_MISSIONS;i++)
            {

                if (strcmp (spInstance->mCharacterSheet.mLevelList[level].mMission.mList[i].mName,name) ==0)
                    {
                        return &(spInstance->mCharacterSheet.mLevelList[level].mMission.mList[i]) ;                          
                    }
            }
        return NULL;
    }

//Get a ptr to a mission record for missions query by index
MissionRecord* CharacterSheetManager::GetMissionRecord(RenderEnums::LevelEnum level, int index)
    {
        //check to see if index is valid
        if ((index > MAX_MISSIONS) || (index < 0))
            {
                //you asserted here cause the input index is too large or less than 0
                rAssert(0);
                return NULL;
            }
        else
            {
                return &(spInstance->mCharacterSheet.mLevelList[level].mMission.mList[index]) ;                          
            }
    }

//Get a ptr to a mission record for streetraces
MissionRecord* CharacterSheetManager::GetStreetRaceRecord(RenderEnums::LevelEnum level, char* name)
    {
        //check to see if name array is too long
        if (strlen (name) > 16)
            {
                //you asserted here cause your name array is greater than 16 bytes
                rAssert(0);
            }
        for (int i=0;i<MAX_STREETRACES;i++)
            {

                if (strcmp (spInstance->mCharacterSheet.mLevelList[level].mStreetRace.mList[i].mName,name) ==0)
                    {
                        return &(spInstance->mCharacterSheet.mLevelList[level].mStreetRace.mList[i]) ;                          
                    }
            }
        return NULL;
    }

//Get a ptr to a mission record for streetraces
MissionRecord* CharacterSheetManager::GetStreetRaceRecord(RenderEnums::LevelEnum level, int index)
    {
         //check to see if index is valid
        if ((index > MAX_STREETRACES) || (index < 0))
            {
                //you asserted here cause the input index is too large or less than 0
                rAssert(0);
                return NULL;
            }
        else
            {
                return &(spInstance->mCharacterSheet.mLevelList[level].mStreetRace.mList[index]) ;                          
                    
            }
    }


    //get a ptr to a bonusmission record
    MissionRecord* CharacterSheetManager::GetBonusMissionRecord (RenderEnums::LevelEnum level, char* name)
    {
       if (strlen(name)>16)
       {
           //Name is too long
           rAssert(0);
       }
       
       if (strcmp (spInstance->mCharacterSheet.mLevelList[level].mBonusMission.mName,name) ==0)
       {
           return & (spInstance->mCharacterSheet.mLevelList[level].mBonusMission) ;
       }
       else 
       {   //return NULL if there is no match
           return NULL;
       }
    }



    //private method for getting a collectable record internal use only
    Record* CharacterSheetManager::GetCollectableRecord(RenderEnums::LevelEnum level,CharacterSheetManager::eCollectableType type,char* name)
    {
        if (strlen(name) >16)
        {
            //Asserted because the name is too long
            rAssert(0);
        }

        switch (type)
        {
        case eCard:
            {  
                for( int i=0; i<MAX_CARDS;i++)
                {
                    if (strcmp(name,spInstance->mCharacterSheet.mLevelList[level].mCard.mList[i].mName))
                    {
                        return (&(spInstance->mCharacterSheet.mLevelList[level].mCard.mList[i]) );
                    }
                }
                //we have reached the end of the list and no matches return A NULL ptr
                return NULL;                           
                break;
            }
        case eCoin:
            {
                /*
                for( int i=0; i<MAX_COINS;i++)
                {
                    if (strcmp(name,spInstance->mCharacterSheet.mLevelList[level].mCoin.mList[i].mName))
                    {
                        return (&(spInstance->mCharacterSheet.mLevelList[level].mCoin.mList[i]) );
                    }
                }
                */
                return NULL;
                break;
            }
        case eCamera:
            {
                /*
                for( int i=0; i<MAX_CAMERAS;i++)
                {
                    if (strcmp(name,spInstance->mCharacterSheet.mLevelList[level].mCamera.mList[i].mName))
                    {
                        return (&(spInstance->mCharacterSheet.mLevelList[level].mCamera.mList[i]) );
                    }
                }
                */
                return NULL;
                break;                
            }
        default:
            {
                //error unknown type
                rAssert(0);
                return NULL;
            }
        }//end switch

    }//end function

   //private method for getting a collectable record internal use only, retrieving by index
    Record* CharacterSheetManager::GetCollectableRecord(RenderEnums::LevelEnum level,CharacterSheetManager::eCollectableType type,int index)
    {
        
        switch (type)
        {
        case eCard:
            {  
                //check for valid index. it must be 0-6
                if ( index > -1 && index < MAX_CARDS)
                {
                        return (&(spInstance->mCharacterSheet.mLevelList[level].mCard.mList[index]) );
                }
                //we have reached the end of the list and no matches return A NULL ptr
                return NULL;                           
                break;
            }
        case eCoin:
            {
                /*
                 //check for valid index. it must be 0-99
                if ( index > -1 && index < MAX_COINS)
                {
                        return (&(spInstance->mCharacterSheet.mLevelList[level].mCoin.mList[index]) );
                }
                //we have reached the end of the list and no matches return A NULL ptr

                */
                return NULL;                           
                break;
            }
        case eCamera:
            {
                /*
                //check for valid index. it must be 0-11
                if ( index > -1 && index < MAX_CAMERAS)
                {
                        return (&(spInstance->mCharacterSheet.mLevelList[level].mCamera.mList[index]) );
                }
                //we have reached the end of the list and no matches return A NULL ptr
                */
                return NULL;                           
                break;              
            }
        default:
            {
                //error unknown type
                rAssert(0);
                return NULL;
            }
        }//end switch

    }//end function

/*
    //Returns a ptr to the player options Struct.
    PlayerOptions* CharacterSheetManager::GetPlayerOptions ()
    {
        return &(spInstance->mCharacterSheet.mPlayerOptions);
    }
*/

/*
//=============================================================================
// CharacterSheetManager::QueryTutorialSeen
//=============================================================================
// Description: checks to see if a given tutorial has already been played
//
// Parameters:  tutorial - enum of the tutorial in question
//
// Return:      bool - has the tutorial been shown
//
//=============================================================================
bool CharacterSheetManager::QueryTutorialSeen( const TutorialMode tutorial )
{
    rAssert( spInstance != NULL );
    return (spInstance->mCharacterSheet.mTutoiralsSeen.mBitfield & (1 << tutorial)) > 0;

}

//=============================================================================
// CharacterSheetManager::SetTutorialSeen
//=============================================================================
// Description: sets the status of a specific tutorial
//
// Parameters:  tutorial - enum of the tutorial in question
//
// Return:      bool - has the tutorial been shown
//
//=============================================================================
void CharacterSheetManager::SetTutorialSeen( const TutorialMode tutorial, const bool seen )
{
    rAssert( spInstance != NULL );
    if ( seen )
    {
        spInstance->mCharacterSheet.mTutoiralsSeen.mBitfield |= (1 << tutorial);
    }
    else
    {
        spInstance->mCharacterSheet.mTutoiralsSeen.mBitfield &= ~(1 << tutorial);
    }
}
*/


//Returns a bool, if the player has unlocked the FMV for the level, We make the presumption that only ONE FMV per level.
bool CharacterSheetManager::QueryFMVUnlocked(RenderEnums::LevelEnum level)
{
    return spInstance->mCharacterSheet.mLevelList[level].mFMVunlocked != 0;
}


// Record that the FMV is unlocked, call this after the FMV is done , from the FMV objective.
void CharacterSheetManager::SetFMVUnlocked(RenderEnums::LevelEnum level)
{
    spInstance->mCharacterSheet.mLevelList[level].mFMVunlocked = true;
}


//returns the number of missions completed by the player. Use this for the FE Scrapbook
int CharacterSheetManager::QueryNumMissionsCompleted(RenderEnums::LevelEnum level)
{
    int numMissionsCompleted =0;
    for (int i =0;i<MAX_MISSIONS;i++)
    {
        if (strcmp(spInstance->mCharacterSheet.mLevelList[level].mMission.mList[i].mName,"m0") ==0 )
        {
            //M0 mission dont count so we ingnore counting of these
        }
        else 
        {
            if (spInstance->mCharacterSheet.mLevelList[level].mMission.mList[i].mCompleted)
            {
                numMissionsCompleted++;
            }
        }
    }

    return numMissionsCompleted;
}


int CharacterSheetManager::QueryNumWaspsDestroyed(RenderEnums::LevelEnum level)
{
    return mCharacterSheet.mLevelList[level].mWaspsDestroyed;
}

int CharacterSheetManager::QueryNumGagsViewed(RenderEnums::LevelEnum level)
{
    return mCharacterSheet.mLevelList[level].mGagsViewed;
}

bool CharacterSheetManager::QueryGagViewed(RenderEnums::LevelEnum level, unsigned which)
{
    rAssert(which < MAX_LEVEL_GAGS);
    unsigned mask = 1 << which;
    return (mCharacterSheet.mLevelList[level].mGagMask & mask) != 0;
}

void CharacterSheetManager::IncNumWaspsDestroyed(RenderEnums::LevelEnum level)
{
    mCharacterSheet.mLevelList[level].mWaspsDestroyed++;
}

void CharacterSheetManager::AddGagViewed(RenderEnums::LevelEnum level, unsigned which)
{
    rAssert(which < MAX_LEVEL_GAGS);
    unsigned mask = 1 << which;

    if(!(mCharacterSheet.mLevelList[level].mGagMask & mask))
    {
        mCharacterSheet.mLevelList[level].mGagMask |= mask;
        mCharacterSheet.mLevelList[level].mGagsViewed++;
    }
}

//Called by the FE scrapbook 

int CharacterSheetManager::QueryNumCarUnlocked(RenderEnums::LevelEnum level )
{
    int defaultCar = 0;
    int bm =0;
    int str =0;

    if( static_cast<int>( QueryHighestMission().mLevel ) >= static_cast<int>( level ) )
    {
        defaultCar = 1;
    }

    if (QueryAllStreetRacesCompleted(level) == true)
    {
        str =1;
    }
    if (QueryBonusMissionCompleted(level) == true)
    {
        bm = 1;
    }

    return ( spInstance->mCharacterSheet.mLevelList[level].mNumCarsPurchased + bm + str );
}


//Called by the FE scrapbook
int CharacterSheetManager::QueryNumSkinsUnlocked(RenderEnums::LevelEnum level)
{
    return spInstance->mCharacterSheet.mLevelList[level].mNumSkinsPurchased;
}

//Returns number of bonus missions completed this should be 0 or 1 since only one Bonus Mission per level
int CharacterSheetManager::QueryNumBonusMissionsCompleted(RenderEnums::LevelEnum level)
{
    int counter =0;
    
    if(spInstance->mCharacterSheet.mLevelList[level].mBonusMission.mCompleted)
    {
        counter++;
    }

    return counter;
}


//Returns number of street races  completed this should be 0 to 3
int CharacterSheetManager::QueryNumStreetRacesCompleted(RenderEnums::LevelEnum level)
{
    int counter =0;

    for (int i=0;i<MAX_STREETRACES;i++)
    {
        if (spInstance->mCharacterSheet.mLevelList[level].mStreetRace.mList[i].mCompleted)
        {
            counter++;
        }
    }
    return counter;
}


CarCharacterSheet* CharacterSheetManager::GetCarCharacterSheet(char* name)
{
    for (int i=0;i<=mCharacterSheet.mCarInventory.mCounter;i++)
    {
        if ( strcmp(name,mCharacterSheet.mCarInventory.mCars[i].mName) ==0)
        {        
            return (&mCharacterSheet.mCarInventory.mCars[i]);
        }
    }
    return NULL;
}

int CharacterSheetManager::QueryNumCardsCollected(RenderEnums::LevelEnum level)
{
    int numCards=0;

    for (int i=0;i<MAX_CARDS;i++)
    {
        if (spInstance->mCharacterSheet.mLevelList[level].mCard.mList[i].mCompleted)
        {
            numCards++;
        }
    }

    return numCards;

}


float CharacterSheetManager::QueryPercentLevelCompleted(RenderEnums::LevelEnum level) const
    {
       float level_completed = 0.0f;
       float cars = 0.0f;
       float skins = 0.0f;
       float missions = 0.0f;
       float cards = 0.0f;
       float wasps= 0.0f;
       float gags = 0.0f;
       float bonusmission =0.0f;
       float streetraces = 0.0f;


           missions     = static_cast< float >( spInstance->QueryNumMissionsCompleted(level) );
           bonusmission = static_cast< float >( spInstance->QueryNumBonusMissionsCompleted(level) );
           streetraces  = static_cast< float >( spInstance->QueryNumStreetRacesCompleted(level) );
           skins        = static_cast< float >( spInstance->QueryNumSkinsUnlocked(level) );
           cars         = static_cast< float >( spInstance->QueryNumCarUnlocked(level) );
           cards        = static_cast< float >( spInstance->QueryNumCardsCollected(level) );
           wasps        = static_cast< float >( spInstance->QueryNumWaspsDestroyed(level) );
           gags         = static_cast< float >( spInstance->QueryNumGagsViewed(level) );
           
           missions = missions/7;
           skins = skins/3;
           cars = cars/5;
           cards= cards/7;
           streetraces = streetraces/3;
           wasps = wasps/GetRewardsManager()->GetTotalWasps((int) level);
           gags = gags /GetRewardsManager()->GetTotalGags( (int) level);


           level_completed = ((missions+bonusmission+streetraces+skins+cars+cards+wasps+gags)/8);

        if(level ==RenderEnums::L7) //level 7 has only 5 missions so we divide by 21 (5+3+6+7)
        {
            float floatnumber = 0.0f;
                floatnumber = (float) level_completed;
                floatnumber*= 100;
            return floatnumber;
        }
        else
        {
            float floatnumber = 0.0f;
                floatnumber = (float) level_completed;
                floatnumber *= 100.0f;
            return floatnumber;
           
        }
    }
         
    

    //Calculate percent game completed. return a whole number.
float CharacterSheetManager::QueryPercentGameCompleted( ) const
    {
        float gameCompleted = 0.0f;

        float l1,l2,l3,l4,l5,l6,l7 = 0.0f;

        l1 = spInstance->QueryPercentLevelCompleted(RenderEnums::L1);
        l2 = spInstance->QueryPercentLevelCompleted(RenderEnums::L2);
        l3 = spInstance->QueryPercentLevelCompleted(RenderEnums::L3);
        l4 = spInstance->QueryPercentLevelCompleted(RenderEnums::L4);
        l5 = spInstance->QueryPercentLevelCompleted(RenderEnums::L5);
        l6 = spInstance->QueryPercentLevelCompleted(RenderEnums::L6);
        l7 = spInstance->QueryPercentLevelCompleted(RenderEnums::L7);

        // Magic number here: convention is L3 is the bonus movie since there is not story movie for that level
        //
        bool bFMVBonus = spInstance->QueryFMVUnlocked (RenderEnums::L3);

        gameCompleted = l1+l2+l3+l4+l5+l6+l7;   
        
        gameCompleted = gameCompleted /7;

        // Levels complete = 99%; Bonus movie = 1% (kind of arbitrary)
        //
        gameCompleted = (gameCompleted * 0.99f) + (bFMVBonus ? 1.0f : 0.0f);

        return gameCompleted;
    }

bool
CharacterSheetManager::IsAllStoryMissionsCompleted()
{
    bool isAllMissionsCompleted = true;

    for( int i = 0; i < RenderEnums::numLevels; i++ )
    {
        if( this->QueryNumMissionsCompleted( static_cast<RenderEnums::LevelEnum>( i ) ) < 7 )
        {
            isAllMissionsCompleted = false;

            break;
        }
    }

    return isAllMissionsCompleted;
}


    int CharacterSheetManager::UpdateRewardsManager()
    {
        for (int i = 0; i<MAX_LEVELS;i++)
        {
            //update default vehicle
            if( static_cast<int>( QueryHighestMission().mLevel ) >= i )
            {
                GetRewardsManager()->GetReward((RenderEnums::LevelEnum) i,Reward::eDefaultCar)->UnlockReward();
            }
            else
            {
                GetRewardsManager()->GetReward((RenderEnums::LevelEnum) i,Reward::eDefaultCar)->LockReward();
            }

            //update the cards unlockable
            if (spInstance->QueryAllCardsCollected((RenderEnums::LevelEnum)i) == true)
            {
                GetRewardsManager()->GetReward((RenderEnums::LevelEnum) i,Reward::eCards)->UnlockReward();
            }
            else
            {
                GetRewardsManager()->GetReward((RenderEnums::LevelEnum) i,Reward::eCards)->LockReward();
            }

            //update the streetraces
            if (spInstance->QueryAllStreetRacesCompleted((RenderEnums::LevelEnum) i) == true)
            {
                GetRewardsManager()->GetReward((RenderEnums::LevelEnum) i,Reward::eStreetRace)->UnlockReward();
            }
            else
            {
                GetRewardsManager()->GetReward((RenderEnums::LevelEnum) i,Reward::eStreetRace)->LockReward();
            }

            //update the bonus mission
            if (spInstance->QueryBonusMissionCompleted((RenderEnums::LevelEnum)i) == true )
            {
                GetRewardsManager()->GetReward((RenderEnums::LevelEnum) i,Reward::eBonusMission)->UnlockReward();
            }
            else
            {
                GetRewardsManager()->GetReward((RenderEnums::LevelEnum) i,Reward::eBonusMission)->LockReward();
            }

            //update the purchase rewards now

            //cycle thru the rewards that we have used coins to by and unlock the rewards in the manager
            for (int j=0;j<MAX_PURCHASED_ITEMS;j++)
            {
                if (spInstance->mCharacterSheet.mLevelList[i].mPurchasedRewards[j])
                {
                    GetRewardsManager()->GetMerchandise(i,j)->UnlockReward();
                }
                else
                {
                    Merchandise* merchandise = GetRewardsManager()->GetMerchandise(i,j);
                    if( merchandise != NULL )
                    {
                        merchandise->LockReward();
                    }
                }
            }

        }
        return 0;

    } 

    bool CharacterSheetManager::IsMiniGameUnlocked() const
    {
        return( GetCardGallery()->GetNumCardDecksCompleted() > 0 );
    }

    static const int NUM_STATES = 2;
    static tUID sKeyTable[NUM_STATES] = 
    {
        tEntity::MakeUID("is_cbg_first"),
        tEntity::MakeUID("is_ticket")
    };

    bool CharacterSheetManager::IsState(tUID State)
    {
        for(int i = 0; i < NUM_STATES; ++i)
        {
            if(State == sKeyTable[i])
            {
                return IsState(i);
            }
        }
        return false;
    }

    bool CharacterSheetManager::IsState(int Index)
    {
        int offset = Index >> 3;
        unsigned char bitMask = 1 << (Index & 7);
        return ((mCharacterSheet.mStates[offset]) & bitMask) == bitMask;
    }

    void CharacterSheetManager::SetState(tUID State, bool IsSet)
    {
        for(int i = 0; i < NUM_STATES; ++i)
        {
            if(State == sKeyTable[i])
            {
                SetState(i, IsSet);
                return;
            }
        }
    }

    void CharacterSheetManager::SetState(int Index, bool IsSet)
    {
        int offset = Index >> 3;
        unsigned char bitMask = 1 << (Index & 7);
        if(IsSet)
        {
            mCharacterSheet.mStates[offset] |= bitMask;
        }
        else
        {
            mCharacterSheet.mStates[offset] = mCharacterSheet.mStates[offset] & ~bitMask;
        }
    }

    int CharacterSheetManager::AddGambleRace(RenderEnums::LevelEnum level,char* name)
    {
        strcpy(mCharacterSheet.mLevelList[level].mGambleRace.mName,name);
//        mCharacterSheet.mLevelList[level].mGambleRace.mBestTime = -1;
        mCharacterSheet.mLevelList[level].mGambleRace.mBonusObjective =false;
//        mCharacterSheet.mLevelList[level].mGambleRace.mCompleted = false;
//        mCharacterSheet.mLevelList[level].mGambleRace.mNumAttempts =0;
//        mCharacterSheet.mLevelList[level].mGambleRace.mSkippedMission = false;
        return 0;
    }

    void CharacterSheetManager::SetGambleRaceBestTime( RenderEnums::LevelEnum level, int seconds )
    {
        mCharacterSheet.mLevelList[ level ].mGambleRace.mBestTime = seconds;
    }

    int CharacterSheetManager::GetGambleRaceBestTime(RenderEnums::LevelEnum level)
    {
        return mCharacterSheet.mLevelList[level].mGambleRace.mBestTime;
    }

    int CharacterSheetManager::GetStreetRaceBestTime(RenderEnums::LevelEnum level,char* name)
    {
        for (int i =0; i <MAX_STREETRACES;i++)
        {
            if (strcmp (name,mCharacterSheet.mLevelList[level].mStreetRace.mList[i].mName) ==0)
            {
                return mCharacterSheet.mLevelList[level].mStreetRace.mList[i].mBestTime;
            }
        }

#ifndef FINAL
        //you asserted here cause your name for the street race is not found
       rTuneAssert(0);
#endif
       return -666;
    }


    int CharacterSheetManager::GetStreetRaceBestTime(RenderEnums::LevelEnum level,int index)
    {
        if( (index >= 0) && (index<MAX_STREETRACES))
        {
                return mCharacterSheet.mLevelList[level].mStreetRace.mList[index].mBestTime;
        }
        else
        {
            //bad invalid index
            rTuneAssert(0);
        }

        return 666;
    }
     

    char* CharacterSheetManager::QueryCurrentSkin(RenderEnums::LevelEnum level)
    {
        return mCharacterSheet.mLevelList[level].mCurrentSkin;
    }

    int CharacterSheetManager::SetCurrentSkin(RenderEnums::LevelEnum level,char* skinName)
    {
        strncpy(mCharacterSheet.mLevelList[level].mCurrentSkin,skinName,16);
        mCharacterSheet.mLevelList[level].mCurrentSkin[15] = '\0';
        return 0;
    }


