#ifndef CHARACTERMANAGER_H_
#define CHARACTERMANAGER_H_

#include <events/eventlistener.h>
#include <loading/loadingmanager.h>
#include <worldsim/ped/pedestrianmanager.h>
// Forward declarations.
//
class Character;
class CharacterRenderable;
class ActionButtonHandler;
class tSkeleton;
class tPose;
class Vehicle;

namespace choreo
{
    class Puppet;
};

class CharacterManager
:
public EventListener,
public LoadingManager::ProcessRequestsCallback
{
public:
    // Are we a PC or NPC
    //
    enum CharacterType
    {
        PC,
        NPC
    };

    // Singleton stuff
    // 
	static CharacterManager* GetInstance( );
	static CharacterManager* CreateInstance( );
    static void DestroyInstance( );

    // Called before loading begins to preload a little neccesary data
    //
    void PreLoad( void );

    // Called when exiting gameplay, cleans up everything
    //
    void Destroy( bool permenant = false);

    // Create a new character and add it to the manager
    //
    Character* AddCharacter( CharacterType, const char* characterName, const char* modelName, const char* choreoPuppet, const char* location = NULL);
    Character* AddCharacterDeferedLoad( CharacterType, const char* characterName, const char* modelName, const char* choreoPuppet, const char* location = NULL);

    // preload data for a character
    //
    void PreloadCharacter(const char* name, const char* anim, LoadingManager::ProcessRequestsCallback* callback = NULL, void* userData = NULL) { LoadAnimation(anim); LoadModel(name, callback, userData); }
    bool IsModelLoaded(const char* name);
    bool IsAnimLoaded(const char* name);

    // legacy add functions (just map to AddCharacter now)
    //
    Character* AddPCCharacter( const char* characterName, const char* choreoPuppet ) { return AddCharacter(PC, characterName, characterName, choreoPuppet); } 

    // Get character by name
    //
    Character* GetCharacterByName( const char* name ) const;
    Character* GetCharacterByName( const tUID uid ) const;
    Character* GetMissionCharacter( const char* name ) const;

    // swap character data
    void SwapData(Character*, const char* mesh, const char* anim);

    // Return a pointer to the character.
	//
	Character* GetCharacter( int i ) const;

    // remove a character
    //
    void RemoveCharacter( Character* ) ;

    // Character garbage collection
    void AllowGarbageCollection(bool allow)  { mGarbageCollect = allow;}
    void SetGarbage(Character*, bool garbage);

    // event handling (mainly when characters touch various locators)
    // 
    virtual void HandleEvent( EventEnum id, void* pEventData );

    void GarbageCollect(bool ignoreDist = false);

    const char* GetModelName(Character*);
    const char* GetAnimName(Character*);

    void ClearTargetVehicle(Vehicle*);
    void ResetBonusCharacters(void);

    // Various simulation stuff
    //
    void PreSimUpdate(float timeins);
    void PostSimUpdate(float timeins);
    void PreSubstepUpdate(float timeins);
    void PostSubstepUpdate(float timeins);
    void Update( float timeins );
    void SubmitStatics( void );
    void SubmitAnimCollisions( void );
    void SubmitDynamics( void );
    
    // Scripter hooks
    //
    static void SetCharacterPosition( int argc, char** argv );
    static void ResetCharacter( int argc, char** argv );

    // ???
    //
    static bool sbFixedSimRate;

    static float sfKickingForce;
    static float sfSlamForce;

    enum LoadState 
    {
        NOT_LOADED,
        GARBAGE,
        LOADING,
        LOADING_GARBAGE,
        LOADED
    };

    static void NextCheatModel(void);

    static int GetMaxCharacters();

    static void Teleport(unsigned i) { DoTeleport((void*)i); } 
    static unsigned GetNumTeleportDests(void);
    static const char* GetTeleportDest(unsigned i);

    void ClearInitialWalk(void) {sInitialWalkLocator[0] = 0;}

protected:
    unsigned int GetCharacterIndex( const Character* character ) const;
private:
    static const int MAX_NPCS = 8;
    static const int MAX_PEDESTRIANS = PedestrianManager::MAX_PEDESTRIANS;
    static const int MAX_CHARACTERS = 64; //MAX_PLAYERS + MAX_PEDESTRIANS + MAX_NPCS;
    static const int MAX_LOADS = 40;

    // Data we need for a character load.
    //
    struct CharacterLoadData
    {
        char modelName[64];
        char animName[64];

        tName modelSection;
        tName animSection;
        tName animModelSection;

        tName mModelHigh;
        tName mModelMedium;
        tName mModelLow;
        tName mChoreoName;
    };

    struct Load
    {
        Load() { state = NOT_LOADED; callback = NULL; }
        tUID name;
        tUID section;
        LoadState state;
        float gracePeriod;
        LoadingManager::ProcessRequestsCallback* callback;
        void* userData;
    };

    CharacterManager( void );
    ~CharacterManager( void );

	int  AddCharacter( Character* pCharacter, CharacterType );

    // [Dusit: Nov 26,2002]
    // Hacked setupcharacter to return the radTime for OUTPUT_TIMES debugging
    // Uncomment #define OUTPUT_TIMES in .cpp to have rReleasePrintfs show time taken
    // in dynaload calls for peds & other characters
    unsigned int SetupCharacter( CharacterLoadData& data, Character* pCharacter);

    void FillLoadData( CharacterLoadData& data, const char* model, const char* anim);

    unsigned  LoadModel(const char* model, LoadingManager::ProcessRequestsCallback* callback = NULL, void* userData = NULL);
    unsigned  LoadAnimation(const char* anim);
    unsigned  FindLoad(Load* loads, tUID name);
    unsigned  AllocLoad(Load* loads, tUID name);
    LoadState GetState(Load* loads, tUID name);

    unsigned InternalSwapData(Character*, const char* mesh, const char* anim);

    void GarbageCollectModel(unsigned index);
    void GarbageCollectAnim(unsigned index);

    void OnProcessRequestsComplete( void* pUserData );   

 	static CharacterManager* spCharacterManager;
  
    CharacterLoadData mDummyLoadData;

    Character* mpCharacter[ MAX_CHARACTERS ];
    char mRealModelNames[ MAX_CHARACTERS ][64];
    char mRealAnimNames[ MAX_CHARACTERS ][64];
    bool mLoaded[MAX_CHARACTERS];
    CharacterLoadData mCharacterLoadData[ MAX_CHARACTERS ];
    unsigned mCharacterModelData[MAX_CHARACTERS];
    unsigned mCharacterAnimData[MAX_CHARACTERS];
    Load mModelData[MAX_LOADS];
    Load mAnimData[MAX_LOADS];

    bool mGarbageCollect;
    bool mGarbage[MAX_CHARACTERS];

    tPose* mUniversalPose;

    static char sInitialWalkLocator[64];
    static void SetInitialWalk(int argc, char** argv);

    static char sCharacterToSpawn[64];
    static Character* sSpawnedCharacter; 
    static void Spawn(void*);
    static void NextSkin( void* );

    static void DoTeleport(void*);
    static void ClearTeleportDests(void);
    static void AddTeleportDest(int argc, char** argv);

    unsigned int mNumCharactersAdded;
};

// A little syntactic sugar for getting at this singleton.
inline CharacterManager* GetCharacterManager() { return( CharacterManager::GetInstance() ); }

inline int CharacterManager::GetMaxCharacters() { return CharacterManager::MAX_CHARACTERS; }

#endif //CHARACTERMANAGER_H_
