#include <events/eventmanager.h>
#include <mission\rewards\reward.h>
#include <mission\charactersheet\charactersheetmanager.h>
#include <p3d/entity.hpp>
#include<string.h>

    Reward::Reward ( )
        :   mUID( 0 ),
            mLevel( -1 ),
            mRepairCost( 10 )
    {
        mRewardType = eNULL;
        mQuestType = eBlank;
        strcpy(mName,"NULL");
        strcpy(mFile, "NULL");
        mEarned = false;
    }
    Reward::Reward(char* name, int level, Reward::eRewardType type, Reward::eQuestType qtype)
        :   mLevel( level ),
            mRepairCost( 10 )
    {
        mUID = tEntity::MakeUID( name );
        strncpy(mName,name,16);
        mName[15] = '\0';
        mRewardType=type;
        mQuestType =qtype;
        strcpy(mFile, "NULL");
        mEarned = false;
    }


Reward::~Reward ()
    {

    }

void Reward::SetName (char* name)
    {
        mUID = tEntity::MakeUID( name );
        strncpy(mName,name,16);
        mName[15] = '\0';       
    }

void Reward::SetRewardType(Reward::eRewardType type)
    {
        mRewardType=type;
    }


void Reward::SetQuestType(Reward::eQuestType type)
    {
        mQuestType =type;
    }

void Reward::SetFilename(char* file)
    {
        strncpy(mFile,file,64);
        mName[63] = '\0';
    }


void Reward::UnlockReward()
{
    mEarned = true;
    if( mRewardType == Reward::ALT_PLAYERCAR )
    {
        GetCharacterSheetManager()->AddCarToInventory( GetName() );

        if( mQuestType != eDefaultCar )
        {
            //
            // Fire off an event that a new car has been unlocked
            //
            GetEventManager()->TriggerEvent( EVENT_UNLOCKED_CAR );
        }
    }
}


                  





