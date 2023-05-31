//=============================================================================

#ifndef ATCMANAGER_H
#define ATCMANAGER_H

//========================================
// System Includes
//========================================


//========================================
// Project Includes
//========================================


//========================================
// Forward References
//========================================
class CollisionAttributes;

const unsigned int MAX_CHAR_LENGTH = 64;

struct AttributeRow
{
    char mSound [MAX_CHAR_LENGTH];
    char mAnimation [MAX_CHAR_LENGTH];
    char mParticle [MAX_CHAR_LENGTH];
    float mMass;
    float mFriction;
    float mElasticity;
};






//==============================================================================
//
// Synopsis: Create the ATC table and create CollosionAttribute objects
//
//==============================================================================
class ATCManager
{
    public:

        // Static Methods for accessing this singleton.
        static ATCManager* CreateInstance();
        static ATCManager* GetInstance();
        static void DestroyInstance();


        // Methods 
		void Init();
        CollisionAttributes* CreateCollisionAttributes(unsigned int classtypeid, unsigned int physpropid, float volume = 0.0f);
        void SetATCTable(AttributeRow* p_attributerow,unsigned int rows);

	

    private:
        
        // No public access to these, use singleton interface.
        ATCManager();
        ~ATCManager();

        // Declared but not defined to prevent copying and assignment.
        ATCManager( const ATCManager& );
        ATCManager& operator=( const ATCManager& );
        
        // Pointer the Master AtrributeTableChunk 
        AttributeRow* mp_ATCTable;
        unsigned int mNumRows;
        // Pointer to the one and only instance of this singleton.
        static ATCManager* spInstance;

};


// A little syntactic sugar for getting at this singleton.
inline ATCManager* GetATCManager() { return( ATCManager::GetInstance() ); }


#endif // ATCMANAGER_H

