#ifndef REWARDSMANAGER_H
#define REWARDSMANAGER_H
#include <mission/rewards/reward.h>
#include <mission/rewards/merchandise.h>
#include <render/enums/renderenums.h>
#include <loading/loadingmanager.h>
#include <mission/charactersheet/charactersheet.h>

//maximum number of cars that can get purchase via coins.
const int MAX_INVENTORY = 8;

struct TokenStoreInventory
{
    Merchandise* mInventoryList[MAX_INVENTORY];
    int mCounter;
};




struct LevelRewardRecord
{
    Reward* mStreetRace;    
    Reward* mBonusMission;
    Reward* mCards;
    Reward* mDefaultCar;
    Reward* mDefaultSkin;
    Reward* mGoldCards;

    int mMaxTokensInLevel;

    int mTotalGagsInLevel;
    int mTotalWaspsInLevel;

};

struct CarAttributeRecord
{
    char mName [16];
    float mSpeed;
    float mAcceleration;
    float mToughness;
    float mStability;
};

const int MAX_CAR_ATTRIBUTE_RECORDS = 50;


//Rewards Manager. 

class RewardsManager : public LoadingManager::ProcessRequestsCallback
    {
    public:             

        void OnProcessRequestsComplete( void* pUserData );

        //Init Rewards with defaults.
        void InitRewards();

        //Init the CarAttribute records with defaults
        void InitCarAttributeRecords ();

        //Clear all rewards. FE should call this for new games.
        void ClearRewards();

        //Load rewards script file
        void LoadScript();
        bool ScriptLoaded() const { return mScriptLoaded; };

        //Static Methods for getting access 
        static RewardsManager* GetInstance();
        static RewardsManager* CreateInstance();
        static void DestroyInstance ();


        int BindReward (char* name,char* filename,Reward::eRewardType rtype, Reward::eQuestType qtype,int level);
                
        //methods for getting the nubmer of tokens in a level and setting them
        void SetTokensInLevel(int level,int tokens);
        int GetNumberOfTokensInLevel(int level);
        

        int GenerateTokens();
        
        //Init the token store
        void InitTokenStore();

        int AddMerchandise(char* name,char* filename,Reward::eRewardType rtype,int level,int cost,Merchandise::eSellerType sellerType);

        //Accessor methods for rewards purchased with coins/tokens

        //get a complete list of rewards that can be purchased with tokens for an entire level.
        TokenStoreInventory* GetTokenStoreInventoryList(int level);
        
        //get a specific Reward you can query by name or index.
        Merchandise* GetMerchandise(int level,const char* name);
        Merchandise* GetMerchandise(int level,int index);
        int GetMerchandiseIndex(int level,const char* name);

        // Iteration Methods for Retieving Merchandise
        //
        Merchandise* FindFirstMerchandise( int level, Merchandise::eSellerType sellerType );
        Merchandise* FindNextMerchandise( int level, Merchandise::eSellerType sellerType );

        //Attempt to buy an item from the token store, method  will return bool for sucess or failure
        //failure will usually mean not enough coins.

        bool BuyMerchandise(int level,const char * name);

        //query methods, input the level and what kind of quest like eCards,eCoins,eBonusMission,eStreetRace,eCamera.       
        Reward* GetReward(int level,Reward::eQuestType qtype);
        Reward* GetReward(int level,tUID id);

        //query method input the car's name, it will either return a record or NULL
        CarAttributeRecord* GetCarAttributeRecord(char* carname);
        static float ComputeOverallCarRating( CarAttributeRecord* carStats );
        
        int SetCarAttributes(char* carname,float speed,float acceleration,float toughness,float stability);

        //updates the Rewards manager's rewards  so it matches with the charactersheet.
        int SynchWithCharacterSheet();
        void IncUpdateQue();

        void SetTotalGags( int level, int numGags );
        int GetTotalGags( int level ) const;

        void SetTotalWasps( int level, int numWasps );
        int GetTotalWasps( int level ) const;

    private:
    
        //private constructor and destructor
        RewardsManager();
        virtual ~RewardsManager();
        
        //private methods

        void NameCheck(char*  name);
        void FileNameCheck(char* filename);
        // Verify that the level is within the proper range. Returns true if it is, false if out of bounds
        // (and bad-array deindex will almost certainly happen)
        bool LevelCheck(int level);


       //Array with all rewards, Note that index 0 is blank/dummy level

        LevelRewardRecord mRewardsList [8];
        CarAttributeRecord mCarAttributeList [MAX_CAR_ATTRIBUTE_RECORDS];
        int mCarAttributeRecordIndex;

        //Chuck: Creating a internal update que, whenever the character sheet get loaded/reloaded it will increment this 
        //que. in the FE update the Rewards manager will check this que, if que not empty then the RewardManager will update itself
        //so it is synched with the character sheet.

        int mUpdateQue;

        TokenStoreInventory mLevelTokenStoreList [MAX_LEVELS];
        int mLevelTokenStoreSearchIndex[ MAX_LEVELS ];

        //declared but not defined.
        RewardsManager ( const RewardsManager& );
        RewardsManager& operator=( const RewardsManager& );           
       
        static RewardsManager* spInstance;

        bool mScriptLoaded;
    };


//Global function  to get access to the singleton
inline RewardsManager* GetRewardsManager()
    {
        return ( RewardsManager::GetInstance() );
    }


        
#endif //RewardsMANAGER_H

        
            