#ifndef RESPAWNMANAGER_H
#define RESPAWNMANAGER_H

#include <mission\respawnmanager\respawnentity.h>


//ReSpawnManager. 

class RespawnManager 
    {
    public:                   


         //constructor and destructor
        RespawnManager();
        ~RespawnManager();        
        
   
        int SetWrenchRespawnTime(int milliseconds);
        int SetWaspRespawnTime(int milliseconds);
        int SetNitroRespawnTime(int milliseconds);

        int GetWrenchRespawnTime();
        int GetWaspRespawnTime();
        int GetNitroRespawnTime();       

        //called in the constructor of the the RespawnEntity
        int GetRespawnTime(RespawnEntity::eRespawnEntity entitytype); 
       
    
    private:
    
        int mWrenchRespawnTime; //in milliseconds
        int mWaspRespawnTime;
        int mNitroRespawnTime;


       
        
        //private methods

       
    };



        
#endif //RESPAWNMANAGER_H

        
            