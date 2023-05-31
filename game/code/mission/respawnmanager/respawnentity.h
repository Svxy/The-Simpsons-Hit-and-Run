#ifndef RESPAWN_ENTITY_H
#define RESPAWN_ENTITY_H

// Respawn Entity
// Base Class for objects in the world that respawn in the world such at the wrenches and possibly Nitro for the Super Sprint mode. 
// wasps will either inherit from this class or be controlled by James H. Coin manager.


class RespawnEntity
{
public:
    enum eRespawnEntity  
        {
            eNull,
            eWrench,
            eWasp,
            eCoinBox,
            eNitro,

            NUMBER_OF_ENUMS
        };


    //update the internal timer
    virtual void Update (unsigned int milliseconds);
    
    //call this to decide if entity needs to respawn, 
    //this should probably get called after an update in the derived class
    bool ShouldEntityRespawn();
    
    //call this to updated the entity's collected state once player picks up or destroys entity
    void EntityCollected();

    void SetRespawnTime(int milliseconds);
    void SetCollectedTime(int milliseconds);
    void SetRespawnEntity(eRespawnEntity entitytype);


    RespawnEntity();
    //derived classes should use this constructor in their initialization list.
    RespawnEntity(eRespawnEntity entitytype);
    virtual ~RespawnEntity();
private:

    bool mCollected;               
    int mRespawnTime;                  //in milliseconds    
    unsigned int mCollectedTime;    //this a counter used to tally the passing of time from when the object was collected. 
                                                    //its set to 0 at the start then incremented upon update if the mCollected has been set.
    eRespawnEntity mRespawnEntityType; 

        
    //declared by not defined.
    RespawnEntity(const RespawnEntity&);
    RespawnEntity& operator= (const RespawnEntity& ); 
};


#endif // RESPAWN_ENITITY_H
