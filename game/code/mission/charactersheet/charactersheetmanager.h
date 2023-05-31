#ifndef CHARACTERSHEETMANAGER_H
#define CHARACTERSHEETMANAGER_H
#include <mission/charactersheet/charactersheet.h>
#include <presentation/tutorialmode.h>
#include <render/enums/renderenums.h>
#include <data/gamedata.h>
#include <p3d/entity.hpp>

//CharacterSheet Manager. Use this class to manipuated and retrieve data from the character sheet
//The Character sheet records the users progress and items collected.



class CharacterSheetManager: GameDataHandler
    {
    public:

        enum eCollectableType
        {
            eCard,
            eCoin,
            eCamera
        };
            

        //Load Save Functionality
        virtual void LoadData( const GameDataByte* dataBuffer, unsigned int numBytes );
        virtual void SaveData( GameDataByte* dataBuffer,unsigned int numBytes );

        inline virtual void ResetData() { InitCharacterSheet(); }

        //Updates the Reward contained by the Rewards Manager
        //This should be called whenever the CharacterSheet is loaded from
        //the Memory Card or Hard Disk
        int UpdateRewardsManager();


        //Set a Collectable to be true
        //you can use the cards name or the index of the card in the array (0 - 6)
        //as parameters

      
        
        void SetCardCollected(RenderEnums::LevelEnum level,int CardID);
        void SetCardCollected(RenderEnums::LevelEnum level,char* CardName);



        //use these methods to modifiy the number of Tokens the character has in their inventory
        //Chuck: Update the methods below shouldnt take parameters anymore since number of tokens
        //The player has is carried over from level to level damn, lousy designers changing their mind. 
        // so just use any valid parameters.

        int     GetNumberOfTokens(RenderEnums::LevelEnum level);
        void   SetNumberOfTokens(RenderEnums::LevelEnum level,int number);
        void   AddTokens(RenderEnums::LevelEnum level,int number);
        void   SubtractTokens(RenderEnums::LevelEnum level,int number);

        
        //methods to fill in a blank character sheet

        //can add a collectable, using the colllectables name as unique key or by trying to place it in the slot by index 
        //methods return 0 for success and -1 for failure  , -2 if the item is already in the list.
        int AddCard(RenderEnums::LevelEnum level,char* name);
        int AddCard(RenderEnums::LevelEnum level,int index);


        
        int AddMission(RenderEnums::LevelEnum level, char* name);
        int AddStreetRace(RenderEnums::LevelEnum level, char* name);
        int AddBonusMission(RenderEnums::LevelEnum level,char* name);
        int AddGambleRace(RenderEnums::LevelEnum level,char* name);

        void SetStreetRaceBestTime(RenderEnums::LevelEnum level,char* name,int seconds);
        void SetGambleRaceBestTime(RenderEnums::LevelEnum level,int seconds);
       
        int GetGambleRaceBestTime(RenderEnums::LevelEnum level);
        int GetStreetRaceBestTime(RenderEnums::LevelEnum level,char* name);
        int GetStreetRaceBestTime(RenderEnums::LevelEnum level,int index);


        char* QueryCurrentSkin(RenderEnums::LevelEnum level);
        int SetCurrentSkin(RenderEnums::LevelEnum level,char* skinName);


        //Init CharacterSheet with defaults, clear all info. FE should call this for new games.
        void InitCharacterSheet();

        //methods for recording mission results, this is called for all missions,streetraces and the bonus mission
        void SetMissionComplete(RenderEnums::LevelEnum level,char* name, bool completed_secondary_objective,int seconds = -1);
        void IncrementMissionAttempt(RenderEnums::LevelEnum level,char* name);
        void SetMissionSkipped (RenderEnums::LevelEnum level,char* name);
        void SetMissionSkipped (RenderEnums::LevelEnum level,RenderEnums::MissionEnum mission);
        void SetFMVUnlocked (RenderEnums::LevelEnum level);
       
        
        //this method gets called/updated in gameplaymanagers::SetCurrentMissionMethod
        void SetCurrentMission(RenderEnums::LevelEnum level, RenderEnums::MissionEnum mission_number);
        
        //methods for Token operations
        void SetPurchasedRewards (RenderEnums::LevelEnum level, int index);
        bool QueryPurchasedReward (RenderEnums::LevelEnum level, int index);

        
        //PhoneBooth Methods Used to  Record and Retrieve Info about the Cars the User has purchased
        //this -1 if inventory is full, or car index. Use this index to update the state of the
        //car in the inventory. If the car already exists the state is set to the provided values.
        int AddCarToInventory( const char* Name );
        // Returns -1 if car isn't in inventory.
        int GetCarIndex( const char* Name );
        int GetNumberOfCarsInInventory();
      
        // Returns eNull if index is out of range,
        // eNoDamage if hitpoints == total hitpoints.
        // eDamaged if hitpoints < total hitpoints, > 0.0f;
        // eHusk if hitpoints == 0.0f;
        CarDamageStateEnum GetCarDamageState( int CarIndex );

        //returns car's health. 0.0f = husk. 1.0f = no damage.
        float GetCarHealth( int CarIndex );
        void UpdateCarHealth( int CarIndex, float Health );

        //FE STUFF: Use these methods for the Scrapbook 
        int QueryNumMissionsCompleted(RenderEnums::LevelEnum level);
        int QueryNumBonusMissionsCompleted(RenderEnums::LevelEnum level);
        int QueryNumStreetRacesCompleted(RenderEnums::LevelEnum level);
        int QueryNumCarUnlocked(RenderEnums::LevelEnum level);
        int QueryNumSkinsUnlocked(RenderEnums::LevelEnum level);
        int QueryNumCardsCollected(RenderEnums::LevelEnum level);
        int QueryNumWaspsDestroyed(RenderEnums::LevelEnum level);
        int QueryNumGagsViewed(RenderEnums::LevelEnum level);
        bool QueryGagViewed(RenderEnums::LevelEnum level, unsigned which);
        bool QueryFMVUnlocked(RenderEnums::LevelEnum level);
        float QueryPercentLevelCompleted(RenderEnums::LevelEnum level) const;
        float QueryPercentGameCompleted() const;

        bool IsAllStoryMissionsCompleted();

        //Wasp and gag recording

        void IncNumWaspsDestroyed(RenderEnums::LevelEnum level);
        void AddGagViewed(RenderEnums::LevelEnum level, unsigned which);


        //query methods for races
        MissionRecord* QueryMissionStatus(RenderEnums::LevelEnum ,char* name);
        MissionRecord* QueryMissionStatus(RenderEnums::LevelEnum ,int index);
        
        //get the number of attempts for a mission
        int QueryNumberOfAttempts(RenderEnums::LevelEnum level, int index);

        MissionRecord* QueryStreetRaceStatus(RenderEnums::LevelEnum ,char* name);
        MissionRecord* QueryStreetRaceStatus(RenderEnums::LevelEnum ,int index);

        MissionRecord* QueryBonusMissionStatus(RenderEnums::LevelEnum level, char* name);
        MissionRecord* QueryBonusMissionStatus(RenderEnums::LevelEnum level, int index);



        //query methods for Collectables. the Loaders and Creation processes should query
        //on chunk encounter before creating the instance        

        bool QueryCardCollected(RenderEnums::LevelEnum level,int index);
        bool QueryCardCollected(RenderEnums::LevelEnum level,char* CardName);      
        


        //used by the FE for resume game.
        CurrentMissionStruct QueryCurrentMission();
        CurrentMissionStruct QueryHighestMission();

        bool QueryNavSystemSetting();
        void SetNavSystemOn(bool setting);

        //query methods for rewards
        bool QueryBonusMissionCompleted(RenderEnums::LevelEnum level);
        bool QueryAllStreetRacesCompleted(RenderEnums::LevelEnum level);        
        bool QueryAllCardsCollected(RenderEnums::LevelEnum level);

        unsigned char* GetPersistentObjectStates( void ) { return &(mCharacterSheet.mPersistentObjectStates[0]); }

        bool IsMiniGameUnlocked() const;

        bool IsState(tUID State);
        bool IsState(int Index);
        void SetState(tUID State, bool IsSet);
        void SetState(int Index, bool IsSet);

        //Static Methods for getting access
        static CharacterSheetManager* GetInstance();
        static CharacterSheetManager* CreateInstance();
        static void DestroyInstance ();

    private:

        CharacterSheetManager();
        virtual ~CharacterSheetManager();

        //declared but not defined.
        CharacterSheetManager ( const CharacterSheetManager& );
        CharacterSheetManager& operator=( const CharacterSheetManager& );
//        PlayerOptions* GetPlayerOptions ();

        MissionRecord* GetMissionRecord(RenderEnums::LevelEnum level,char* name);
        MissionRecord* GetMissionRecord(RenderEnums::LevelEnum level,int index );
        
        MissionRecord* GetStreetRaceRecord(RenderEnums::LevelEnum level, char* name);
        MissionRecord* GetStreetRaceRecord(RenderEnums::LevelEnum level, int index );

        MissionRecord* GetBonusMissionRecord(RenderEnums::LevelEnum level,char* name);

        Record* GetCollectableRecord(RenderEnums::LevelEnum level,eCollectableType type, char* name);
        Record* GetCollectableRecord(RenderEnums::LevelEnum level,eCollectableType type, int index);

        CarCharacterSheet* GetCarCharacterSheet(char * name);

        //implement if dusit doesnt do the above.
        //CardRecord* GetCardRecord(eLevelID,char* name);

            
        //member variables 
        CharacterSheet mCharacterSheet;
       
        static CharacterSheetManager* spInstance;
    };


//Global function  to get access to the singleton
inline CharacterSheetManager* GetCharacterSheetManager()
    {
        return ( CharacterSheetManager::GetInstance() );
    }


        
#endif //CHARACTERSHEETMANAGER_H

        
            