//Implementation of Respawn Entity.CPP

#include <mission/respawnmanager/respawnentity.h>
#include <mission/respawnmanager/respawnmanager.h>
#include <mission/gameplaymanager.h>


//default constructor, rarely used
RespawnEntity::RespawnEntity()
{
    mCollected = false;
    mCollectedTime = 0;
    mRespawnTime = 30000; //default respawn of 30 seconds
    mRespawnEntityType = RespawnEntity::eNull;
}


// the more commonly used constructor
RespawnEntity::RespawnEntity(RespawnEntity::eRespawnEntity entitytype)
{
    mCollected =false;
    mCollectedTime =0;
    mRespawnTime = GetGameplayManager()->GetRespawnManager()->GetRespawnTime(entitytype);
    mRespawnEntityType = entitytype;
}

RespawnEntity::~RespawnEntity()
{

}

void RespawnEntity::Update(unsigned int milliseconds)
{
    //check if the item has been collected
    if ( mCollected == true) 
    {
        //item has been collected so we calculate how much time has passed since 
        //we collected the item       
        mCollectedTime += milliseconds;     

    }    

}


void RespawnEntity::EntityCollected()
{
    mCollected=true;
    mCollectedTime =0;    
}

void RespawnEntity::SetRespawnTime(int milliseconds)
{
    mRespawnTime = milliseconds;
}

void RespawnEntity::SetCollectedTime(int milliseconds)
{
    mCollectedTime = milliseconds;
}

void RespawnEntity::SetRespawnEntity(RespawnEntity::eRespawnEntity respawnentitytype)
{
    mRespawnEntityType = respawnentitytype;
}

bool RespawnEntity::ShouldEntityRespawn()
{
    unsigned int respawntimedelta = mRespawnTime;
  
    //check if enough time has passed
    if (mCollectedTime >= respawntimedelta)
    {
        //enough time has pass , time passed greater or equal to respawntime set by designers
        //reset states
        mCollected = false;
        mCollectedTime = 0;
        return true;
    }
    else
    {
        return false;
    }
}



