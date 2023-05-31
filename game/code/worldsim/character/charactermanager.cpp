#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>
#include <worldsim/character/characterrenderable.h>
#include <worldsim/worldphysicsmanager.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/redbrick/vehicle.h>

#include <ai/actionbuttonmanager.h>
#include <ai/actionbuttonhandler.h>
#include <events/eventenum.h>
#include <events/eventmanager.h>
#include <meta/locatorevents.h>
#include <meta/eventlocator.h>
#include <meta/triggervolume.h>
#include <meta/carstartlocator.h>

#include <camera/supercamcentral.h>
#include <camera/supercammanager.h>

#include <memory/srrmemory.h>

#include <raddebug.hpp>
#include <raddebugwatch.hpp>

#include <p3d/anim/drawablepose.hpp>
#include <p3d/shader.hpp>
#include <p3d/shadow.hpp>

// Choreo includes.
//
#include <p3d/anim/pose.hpp>
#include <choreo/bank.hpp>
#include <choreo/load.hpp>
#include <choreo/puppet.hpp>
#include <choreo/utility.hpp>
#include <simcommon/simstate.hpp>
#include <simcommon/simulatedobject.hpp>
#include <simcollision/collisionobject.hpp>

#include <console/console.h>

#include <render/RenderManager/RenderManager.h>
#include <render/RenderManager/RenderLayer.h>
// Temp hack to get NPCs in.
// TBJ [9/3/2002] 
//
#include <mission/gameplaymanager.h>
#include <constants/maxplayers.h>

#include <stdlib.h>
#include <debug/profiler.h>
#include <worldsim/avatarmanager.h>

#include <radtime.hpp>
#include <p3d/anim/skeleton.hpp>

#include <ai/sequencer/actioncontroller.h>
#include <ai/sequencer/action.h>
#include <ai/sequencer/sequencer.h>
#include <presentation/blinker.h>

#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <mission/charactersheet/charactersheetmanager.h>

CharacterManager* CharacterManager::spCharacterManager = 0;

/************ OUTPUT_TIMES FOR DLOP **********/
//#define OUTPUT_TIMES
/**********************************************/

// Init the character tunables.
//  
// Statics.
//
float CharacterTune::sfLocoRotateRate;
float CharacterTune::sfLocoAcceleration;
float CharacterTune::sfLocoDecceleration;
bool CharacterTune::bLocoTest;
float CharacterTune::sfAirRotateRate;
float CharacterTune::sfAirAccelScale;
float CharacterTune::sfAirGravity;
float CharacterTune::sfStompGravityScale;
float CharacterTune::sfDashBurstMax;
float CharacterTune::sfDashAcceleration;
float CharacterTune::sfDashDeceleration;
float CharacterTune::sfJumpHeight;
float CharacterTune::sfDoubleJumpHeight;
float CharacterTune::sfDoubleJumpAllowUp;
float CharacterTune::sfDoubleJumpAllowDown;

float CharacterTune::sfHighJumpHeight;
float CharacterTune::sfMaxSpeed;
float CharacterTune::sfTurboRotateRate;

float CharacterTune::sfGetInOutOfCarAnimSpeed;
rmt::Vector CharacterTune::sGetInPosition;
float CharacterTune::sGetInHeightThreshold;
float CharacterTune::sGetInOpenDelay;
float CharacterTune::sGetInOpenSpeed;
float CharacterTune::sGetInCloseDelay;
float CharacterTune::sGetInCloseSpeed;
float CharacterTune::sGetOutOpenDelay;
float CharacterTune::sGetOutOpenSpeed;
float CharacterTune::sGetOutCloseDelay;
float CharacterTune::sGetOutCloseSpeed;
float CharacterTune::sfKickingForce;
float CharacterTune::sfSlamForce;
// How long in seconds that the character will get shocked for
float CharacterTune::sfShockTime;

bool CharacterManager::sbFixedSimRate;

static const unsigned INVALID_LOAD = 0xffffffff;

Blinker g_Blinkers[ 64 ];  //should match max_characters

/*
==============================================================================
CharacterManager::CreateInstance
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         CharacterManager

=============================================================================
*/
CharacterManager*  CharacterManager::CreateInstance( void )
{
	rAssertMsg( spCharacterManager == 0, "CharacterManager already created.\n" );
#ifdef RAD_GAMECUBE
	spCharacterManager = new ( GMA_GC_VMM ) CharacterManager;
#else
    spCharacterManager = new ( GMA_PERSISTENT ) CharacterManager;
#endif

    return( spCharacterManager );
}
/*
==============================================================================
CharacterManager::GetInstance
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         CharacterManager

=============================================================================
*/
CharacterManager* CharacterManager::GetInstance( void )
{
	rAssertMsg( spCharacterManager != 0, "CharacterManager has not been created yet.\n" );
	return spCharacterManager;
}
/*
==============================================================================
CharacterManager::DestroyInstance
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void CharacterManager::DestroyInstance( void )
{
	rAssertMsg( spCharacterManager != 0, "CharacterManager has not been created.\n" );
	delete ( GMA_PERSISTENT, spCharacterManager );
}

/*
==============================================================================
CharacterManager::CharacterManager
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         CharacterManager

=============================================================================
*/
CharacterManager::CharacterManager( void )
{
	int i;
	for ( i = 0; i < MAX_CHARACTERS; i++ )
	{
		mpCharacter[ i ] = 0;
        mGarbage[i] = false;
	}

    GetEventManager()->AddListener( this, (EventEnum)( EVENT_LOCATOR + LocatorEvent::CAR_DOOR ) );
    GetEventManager()->AddListener( this, (EventEnum)( EVENT_LOCATOR + LocatorEvent::BOUNCEPAD ) );
    GetEventManager()->AddListener( this, (EventEnum)( EVENT_LOCATOR + LocatorEvent::GENERIC_BUTTON_HANDLER_EVENT ) );
    GetEventManager()->AddListener( this, EVENT_DEATH_VOLUME_SCREEN_BLANK );
    GetEventManager()->AddListener( this, EVENT_STAGE_COMPLETE );
    GetEventManager()->AddListener( this, EVENT_MISSION_SUCCESS );
    GetEventManager()->AddListener( this, EVENT_CARD_COLLECTED );
    GetEventManager()->AddListener( this, EVENT_MISSION_CHARACTER_RESET );
    GetEventManager()->AddListener( this, EVENT_GETINTOVEHICLE_START );
    GetEventManager()->AddListener( this, EVENT_GETOUTOFVEHICLE_END );
    GetEventManager()->AddListener( this, EVENT_TOGGLE_FIRSTPERSON );

    CharacterTune::sfLocoRotateRate = 10.0f;
    CharacterTune::sfLocoAcceleration = 20.0f;
    CharacterTune::sfLocoDecceleration = -10.0f;

    CharacterTune::bLocoTest = false;

    CharacterTune::sfAirRotateRate = 4.0f;
    CharacterTune::sfAirAccelScale = 0.078f; //was .1
    CharacterTune::sfAirGravity = -25.0f; //was -30
    CharacterTune::sfStompGravityScale = 3.22f; 
    CharacterTune::sfJumpHeight = 1.9f; //was 1.65
    CharacterTune::sfDoubleJumpHeight = 1.0f; 
    CharacterTune::sfDoubleJumpAllowUp = 2.0f;
    CharacterTune::sfDoubleJumpAllowDown = 12.0f; //was 6
    CharacterTune::sfHighJumpHeight = 1.9f; 

    CharacterTune::sfDashBurstMax = 4.0f; // 7.0
    CharacterTune::sfDashAcceleration = 200.0f;
    CharacterTune::sfDashDeceleration = 200.0f;
    CharacterTune::sfMaxSpeed = 4.0f;
    CharacterTune::sGetInPosition.Set( 1.05f, 0.0f, -0.86f );
    CharacterTune::sGetInHeightThreshold = 0.1f;
    CharacterTune::sGetInOpenDelay = 0.0f;
    CharacterTune::sGetInOpenSpeed = 0.2f;
    CharacterTune::sGetInCloseDelay = 0.0f;
    CharacterTune::sGetInCloseSpeed = 0.2f;
    CharacterTune::sGetOutOpenDelay = 0.0f;
    CharacterTune::sGetOutOpenSpeed = 0.2f;
    CharacterTune::sGetOutCloseDelay = 0.0f;
    CharacterTune::sGetOutCloseSpeed = 0.2f;
    
    CharacterTune::sfTurboRotateRate = 2.0f; // 1.2

    CharacterTune::sfGetInOutOfCarAnimSpeed = 30.0f;
    CharacterTune::sfKickingForce = 400.0f;
    CharacterTune::sfSlamForce = 800.0f;

    CharacterManager::sbFixedSimRate = false;

    Console* pConsole = GetConsole( );
    rAssert( pConsole );
    if ( pConsole )
    {
        pConsole->AddFunction( "SetCharacterPosition", CharacterManager::SetCharacterPosition, "Sets the character position", 3, 3 );
        pConsole->AddFunction( "ResetCharacter", CharacterManager::ResetCharacter, "Sets the character to the named locator", 2, 2 );
        pConsole->AddFunction( "AddTeleportDest", AddTeleportDest, "Set a valid location for a teleport", 3, 5 );

        pConsole->AddFunction( "SetInitialWalk", SetInitialWalk, "Set locator to walk to on startup", 1, 1 );
    }
#ifdef DEBUGWATCH
    const int MAX_LEN = 64;
    char debugName[ MAX_LEN ];
    int len = sprintf( debugName, "Character" );
    rAssert( len < MAX_LEN );

    radDbgWatchAddBoolean(&CharacterTune::bLocoTest, "Steer Facing", debugName, NULL, 0 );
    radDbgWatchAddFloat(&CharacterTune::sfLocoRotateRate, "Loco Turning Rate", debugName, NULL, 0, 0.1f, 50.0f);
    radDbgWatchAddFloat(&CharacterTune::sfLocoAcceleration, "Loco Acceleration", debugName, NULL, 0, 0.1f, 50.0f);
    radDbgWatchAddFloat(&CharacterTune::sfLocoDecceleration, "Loco Deceleration", debugName, NULL, 0, -50.0f, -0.1f);

    radDbgWatchAddFloat(&CharacterTune::sfAirRotateRate, "Air Turning Rate", debugName, NULL, 0, 0.0f, 20.0f);
    radDbgWatchAddFloat(&CharacterTune::sfAirAccelScale, "Air Acceleration", debugName, NULL, 0, 0.0f, 1.0f );
    radDbgWatchAddFloat(&CharacterTune::sfAirGravity, "Air Gravity", debugName, NULL, 0, -100.0f, 0.1f );
    radDbgWatchAddFloat(&CharacterTune::sfStompGravityScale, "Stomp Gravity Scale", debugName, NULL, 0, 0.1f, 4.0f);
    radDbgWatchAddFloat(&CharacterTune::sfJumpHeight, "Jump Height (Tap)", debugName, NULL, 0, 0.0f, 100.0f ); 
    radDbgWatchAddFloat(&CharacterTune::sfDoubleJumpHeight, "Jump Height (Double Jump)", debugName, NULL, 0, 0.0f, 100.0f ); 
    radDbgWatchAddFloat(&CharacterTune::sfDoubleJumpAllowUp, "Double Jump Sweet Spot (Up)", debugName, NULL, 0, 0.0f, 30.0f ); 
    radDbgWatchAddFloat(&CharacterTune::sfDoubleJumpAllowDown, "Double Jump Sweet Spot (Down)", debugName, NULL, 0, 0.0f, 30.0f ); 
    radDbgWatchAddFloat(&CharacterTune::sfHighJumpHeight, "Jump Height (Full Press)", debugName, NULL, 0, 0.0f, 100.0f ); 

    radDbgWatchAddFloat(&CharacterTune::sfDashAcceleration, "Dash Accel", debugName, NULL, 0, 0.0f, 200.0f );
    radDbgWatchAddFloat(&CharacterTune::sfDashDeceleration, "Dash Decel", debugName, NULL, 0, 0.0f, 200.0f );

    radDbgWatchAddFloat(&CharacterTune::sfMaxSpeed, "Max Speed", debugName, NULL, 0, 0.0f, 8.0f ); 

    radDbgWatchAddFloat((float*)&CharacterTune::sGetInPosition.x, "Get In Offset X", "Get In/Get Out", NULL, 0, 0.0f, 3.0f );
    radDbgWatchAddFloat((float*)&CharacterTune::sGetInPosition.z, "Get In Offset Z", "Get In/Get Out", NULL, 0, -1.5f, 1.5f );
    radDbgWatchAddFloat(&CharacterTune::sGetInHeightThreshold, "Height Threshold", "Get In/Get Out", NULL, 0, -2.0f, 2.0f );
    radDbgWatchAddFloat(&CharacterTune::sfGetInOutOfCarAnimSpeed, "Speed", "Get In/Get Out", NULL, 0, 15.0f, 240.0f );
    radDbgWatchAddFloat(&CharacterTune::sGetInOpenDelay, "Get In Open Delay", "Get In/Get Out", NULL, 0, 0.0f, 2.0f );
    radDbgWatchAddFloat(&CharacterTune::sGetInOpenSpeed, "Get In Open Speed", "Get In/Get Out", NULL, 0, 0.0f, 2.0f );
    radDbgWatchAddFloat(&CharacterTune::sGetInCloseDelay, "Get In Close Delay", "Get In/Get Out", NULL, 0, 0.0f, 2.0f );
    radDbgWatchAddFloat(&CharacterTune::sGetInCloseSpeed, "Get In Close Speed", "Get In/Get Out", NULL, 0, 0.0f, 2.0f );
    radDbgWatchAddFloat(&CharacterTune::sGetOutOpenDelay, "Get Out Open Delay", "Get In/Get Out", NULL, 0, 0.0f, 2.0f );
    radDbgWatchAddFloat(&CharacterTune::sGetOutOpenSpeed, "Get Out Open Speed", "Get In/Get Out", NULL, 0, 0.0f, 2.0f );
    radDbgWatchAddFloat(&CharacterTune::sGetOutCloseDelay, "Get Out Close Delay", "Get In/Get Out", NULL, 0, 0.0f, 2.0f );
    radDbgWatchAddFloat(&CharacterTune::sGetOutCloseSpeed, "Get Out Close Speed", "Get In/Get Out", NULL, 0, 0.0f, 2.0f );

    radDbgWatchAddBoolean( &CharacterManager::sbFixedSimRate, "Fixed sim rate", debugName, NULL, 0 );

    radDbgWatchAddFloat(&CharacterTune::sfDashBurstMax, "Turbo Speed", debugName, NULL, 0, 0.0f, 100.0f );
    radDbgWatchAddFloat(&CharacterTune::sfTurboRotateRate, "Turbo Rotate Rate", debugName, NULL, 0, 0.0f, 10.0f );

    radDbgWatchAddString(sCharacterToSpawn, 64, "NPC to Spawn", "CharacterManager");
    radDbgWatchAddFunction( "Spawn NPC", &Spawn, NULL, "CharacterManager");

    radDbgWatchAddFunction( "Next Skin", &NextSkin, NULL, "CharacterManager");
    radDbgWatchAddFloat(&CharacterTune::sfKickingForce, "Kicking Force", debugName, NULL, 0, 0.0f, 2000.0f );
    radDbgWatchAddFloat(&CharacterTune::sfSlamForce, "Stomping Force", debugName, NULL, 0, 0.0f, 4000.0f );
    radDbgWatchAddFloat(&CharacterTune::sfShockTime, "Time(sec) to be shocked",debugName , NULL, NULL ,0 , 10.0f);

#ifdef RAD_XBOX
    extern float g_XBoxMipmapBias;
    radDbgWatchAddFloat(&g_XBoxMipmapBias, "Mipmap Bias", "XBox", NULL, NULL , -10.0f , 10.0f);
#endif

#endif //DEBUGWATCH

    strcpy(mDummyLoadData.modelName, "npd");
    strcpy(mDummyLoadData.animName, "npd");
    FillLoadData( mDummyLoadData, "npd", "npd");
    
    mGarbageCollect = false;

    mUniversalPose = new tPose(64);
    mUniversalPose->AddRef();

    mNumCharactersAdded = 0;
}

CharacterManager::~CharacterManager( void )
{ 
    Destroy(true);
    tRefCounted::Release(mUniversalPose);
    GetEventManager()->RemoveAll( this );
}

void CharacterManager::PreLoad( void )
{
    // Load the global textures
    char fname[128];
    sprintf(fname, "art\\chars\\global.p3d");
    char section[128];
    sprintf(section, "Eakkachaichanvet");

    p3d::inventory->AddSection(section);
    GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D, fname, GMA_LEVEL_OTHER, section );

    // Load in a dummy, lightweight model 
    LoadModel("nps");
    LoadModel("ndr");
    LoadModel("npd");

    // these animation banks are loaded all the time
    LoadAnimation("nps");
    LoadAnimation("ndr");
    LoadAnimation("npd");
}


void CharacterManager::Destroy(bool permenant)
{
    mUniversalPose->SetSkeleton(NULL);

    ClearTeleportDests();

    if(permenant)
    {
        p3d::inventory->RemoveSectionElements( "Eakkachaichanvet" );
        p3d::inventory->DeleteSection( "Eakkachaichanvet" );
    }

    int i;
    for ( i = 0; i < MAX_CHARACTERS; i++ )
    {
        g_Blinkers[ i ].SetCharacter( NULL );
        if ( mpCharacter[ i ] != 0 )
        {
            GetWorldPhysicsManager()->RemoveFromAnyOtherCurrentDynamicsListAndCollisionArea(mpCharacter[i]);
            mpCharacter[ i ]->RemoveFromWorldScene( );
            mpCharacter[ i ]->ClearAllActionButtonHandlers();
            mpCharacter[ i ]->SetManaged(false);
            mpCharacter[ i ]->Release();
            mpCharacter[ i ] = 0;
        }

        // Clean out the load data names, to make sure we clean up all memory in use
        //
        mCharacterLoadData[ i ].modelSection.SetText (0);
        mCharacterLoadData[ i ].animSection.SetText (0);
        mCharacterLoadData[ i ].animModelSection.SetText (0);
        mCharacterLoadData[ i ].mModelHigh.SetText (0);
        mCharacterLoadData[ i ].mModelMedium.SetText (0);
        mCharacterLoadData[ i ].mModelLow.SetText (0);
        mCharacterLoadData[ i ].mChoreoName.SetText (0);
    }

    for ( i = permenant ? 0 : 3; i < MAX_LOADS; i++ )
	{
        rAssert(mModelData[i].state != LOADING);
        rAssert(mAnimData[i].state != LOADING);

        if(mModelData[i].state == LOADED || mModelData[i].state == GARBAGE )
        {
            p3d::inventory->RemoveSectionElements(mModelData[i].section);
            p3d::inventory->DeleteSection(mModelData[i].section);
        }

        if(mAnimData[i].state == LOADED || mAnimData[i].state == GARBAGE )
        {
            p3d::inventory->RemoveSectionElements(mAnimData[i].section);
            p3d::inventory->DeleteSection(mAnimData[i].section);
        }

        mModelData[i].state = NOT_LOADED;
        mAnimData[i].state = NOT_LOADED;
    }

    mNumCharactersAdded = 0;
}

void CharacterManager::RemoveCharacter( Character* c)
{
    rAssert(c);
    if(c)
    {
        for ( int i = 0; i < MAX_CHARACTERS; i++ )
        {
            if ( mpCharacter[ i ] == c )
            {
                if(mpCharacter[ i ]->GetRole() == Character::ROLE_COMPLETED_BONUS)
                {
                    GetVehicleCentral()->RemoveSuppressedDriver(this->mRealModelNames[i]);
                }

                // if you get this assert, please leave it running and go get Nigel or Dusit
                rAssert(mpCharacter[ i ]->GetRole() != Character::ROLE_PEDESTRIAN);

                g_Blinkers[ i ].SetCharacter( NULL );
                GetWorldPhysicsManager()->RemoveFromAnyOtherCurrentDynamicsListAndCollisionArea(mpCharacter[i]);
                mpCharacter[ i ]->RemoveFromWorldScene( );
                GetRenderManager()->mEntityDeletionList.Add((tRefCounted*&)mpCharacter[ i ]);
                mpCharacter[ i ]->SetManaged(false);
                mpCharacter[ i ] = 0;

                GarbageCollectModel(mCharacterModelData[i]);
                GarbageCollectAnim(mCharacterAnimData[i]);
                return;
            }
        }
    }
}


void CharacterManager::SetGarbage(Character* c, bool garbage)
{
    for( int i = 0; i < MAX_CHARACTERS; i++ )
    {
        if ( mpCharacter[ i ] == c)
        {
            mGarbage[i] = garbage; 
        }
    }
}

void CharacterManager::GarbageCollectModel(unsigned modelIndex)
{
    bool modelUsed = (modelIndex < 3); // first three animation are npd, ndr, nps, don't garbage colelct

    for ( int j = 0; (j < MAX_CHARACTERS) && !modelUsed; j++ )
	{
        if(mpCharacter[j])
        {
            modelUsed |= modelIndex == mCharacterModelData[j];
        }
    }

    if(!modelUsed)
    {
        if(mModelData[modelIndex].state == LOADING)
        {
            mModelData[modelIndex].state = LOADING_GARBAGE;
            mModelData[modelIndex].gracePeriod = 3.0f;
        }
        else
        {
            mModelData[modelIndex].state = GARBAGE;
            mModelData[modelIndex].gracePeriod = 3.0f;
        }
    }
}

void CharacterManager::GarbageCollectAnim(unsigned animIndex)
{
    bool animUsed = (animIndex < 3); // first three animation are npd, ndr, nps, don't garbage colelct

    for ( int j = 0; (j < MAX_CHARACTERS) && !animUsed; j++ )
	{
        if(mpCharacter[j])
        {
            animUsed |= animIndex == mCharacterAnimData[j];
        }
    }

    if(!animUsed)
    {
        if(mAnimData[animIndex].state == LOADING)
        {
            mAnimData[animIndex].state = LOADING_GARBAGE;
        }
        else
        {
            mAnimData[animIndex].state = GARBAGE;
        }
        
    }
}

bool CharacterManager::IsModelLoaded(const char* name) 
{ 
    unsigned index = FindLoad(mModelData, tEntity::MakeUID(name));
    if( index != INVALID_LOAD && mModelData[index].state == LOADED )
    {
        return true;
    }
    return false;

    //return FindLoad(mModelData, tEntity::MakeUID(name)) != INVALID_LOAD;
}

bool CharacterManager::IsAnimLoaded(const char* name)
{ 
    unsigned index = FindLoad(mAnimData, tEntity::MakeUID(name));
    if( index != INVALID_LOAD && mAnimData[index].state == LOADED )
    {
        return true;
    }
    return false;

    //return FindLoad(mAnimData, tEntity::MakeUID(name)) != INVALID_LOAD;
}

unsigned CharacterManager::LoadModel(const char* model, LoadingManager::ProcessRequestsCallback* callback, void* userData)
{
    tUID uid = tEntity::MakeUID(model);
    unsigned loadIndex = AllocLoad(mModelData, uid);


    if(mModelData[loadIndex].state == GARBAGE)
    {
        mModelData[loadIndex].state = LOADED;
    }

    if(mModelData[loadIndex].state == LOADING_GARBAGE)
    {
        mModelData[loadIndex].state = LOADING;
    }

    if((mModelData[loadIndex].state == LOADED) && callback)
    {
        callback->OnProcessRequestsComplete(userData);
    }
    else
    {
        mModelData[loadIndex].callback = callback;
        mModelData[loadIndex].userData = userData;
    }

    if(mModelData[loadIndex].state == NOT_LOADED)
    {
        char truncModel[ 32 ];

        // filenames are truncated at 6 characters
        strcpy(truncModel, model);
        truncModel[6] = 0;

        char modelName[128];
        sprintf(modelName, "art\\chars\\%s_m.p3d", truncModel);
        char modelSection[128];
        sprintf(modelSection, "%s_m", model);

        mModelData[loadIndex].name = uid;
        mModelData[loadIndex].section = tEntity::MakeUID(modelSection);
        mModelData[loadIndex].state = LOADING;

        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
        p3d::inventory->AddSection(modelSection);
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );

        GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D, modelName, GMA_CHARS_AND_GAGS, modelSection);
        GetLoadingManager()->AddCallback(this, (void*)loadIndex);
    }

    return loadIndex;
}

unsigned CharacterManager::LoadAnimation(const char* anim)
{
    tUID uid = tEntity::MakeUID(anim);
    unsigned loadIndex = AllocLoad(mAnimData, uid);

    if(mAnimData[loadIndex].state == GARBAGE)
    {
        mAnimData[loadIndex].state = LOADED;
    }

    if(mAnimData[loadIndex].state == LOADING_GARBAGE)
    {
        mAnimData[loadIndex].state = LOADING;
    }

    if(mAnimData[loadIndex].state == NOT_LOADED)
    {
        char truncAnim[ 32 ];

        // filenames are truncated at 6 characters
        strcpy(truncAnim, anim);
        truncAnim[6] = 0;

        char animName[128];
        char choreoName[128];
        sprintf(animName, "art\\chars\\%s_a.p3d", truncAnim);
        sprintf(choreoName, "art\\chars\\%s.cho", anim);
        char animSection[128];
        sprintf(animSection, "%s_a", anim);

        mAnimData[loadIndex].name = uid;
        mAnimData[loadIndex].section = tEntity::MakeUID(animSection);
        mAnimData[loadIndex].state = LOADING;

        HeapMgr()->PushHeap( GMA_TEMP );
        p3d::inventory->AddSection(animSection);
        HeapMgr()->PopHeap( GMA_TEMP );
        GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D, animName, GMA_LEVEL_OTHER, animSection);
        GetLoadingManager()->AddRequest( FILEHANDLER_CHOREO, choreoName, GMA_LEVEL_OTHER, animSection);
        GetLoadingManager()->AddCallback(this, (void*)(0x10000000 | loadIndex));
    }

    return loadIndex;
}

unsigned CharacterManager::FindLoad(Load* loads, tUID name)
{
    for(int i = 0; i < MAX_LOADS; i++)
    {
        if(loads[i].name == name)
        {
            return i;
        }
    }

    return INVALID_LOAD;
}

unsigned CharacterManager::AllocLoad(Load* loads, tUID name)
{
    unsigned index = FindLoad(loads, name);
    
    if(index == INVALID_LOAD)
    {
        for(int i = 0; i < MAX_LOADS; i++)
        {
            if(loads[i].state == NOT_LOADED)
            {
                loads[i].name = name;
                return i;
            }
        }
    }
    else
    {
        return index;
    }

    rAssertMsg(0, "Can't load any more character data");
    return INVALID_LOAD;
}

CharacterManager::LoadState CharacterManager::GetState(Load* loads, tUID name)
{
    unsigned index = FindLoad(loads, name);
    if(index != INVALID_LOAD)
    {
        return loads[index].state;
    }
    return NOT_LOADED;
}

void CharacterManager::FillLoadData( CharacterLoadData& data, const char* useModel, const char* useAnim)
{
    const int MAX_LEN = 32;
    char modelName[ MAX_LEN ];
    
    int len = sprintf( modelName, "%s_h", useModel);
    rAssert( len < MAX_LEN );
    data.mModelHigh.SetText( modelName );
        
    len = sprintf( modelName, "%s_m", useModel );
    rAssert( len < MAX_LEN );
    data.mModelMedium.SetText( modelName );
    
    len = sprintf( modelName, "%s_l", useModel );
    rAssert( len < MAX_LEN );
    data.mModelLow.SetText( modelName );
    
    data.mChoreoName.SetText( useAnim );
    
    char sec[256];
    sprintf(sec, "%s_m", useModel);
    data.modelSection.SetText(sec);
    sprintf(sec, "%s_a", useAnim);
    data.animSection.SetText(sec);
    sprintf(sec, "%s_m", useAnim);
    data.animModelSection.SetText(sec);
}

Character* CharacterManager::AddCharacter( CharacterType type, const char* characterName, const char* modelName, const char* choreoPuppet, const char* location)
{
    unsigned int addCharTime = radTimeGetMicroseconds();


    unsigned int MakeUIDTime = radTimeGetMicroseconds();
    tUID modelUID = tEntity::MakeUID(modelName);
    tUID animUID = tEntity::MakeUID(choreoPuppet);
    MakeUIDTime = radTimeGetMicroseconds() - MakeUIDTime;

    Character* character;

    
//    #ifdef RAD_GAMECUBE
//        HeapMgr()->PushHeap( GMA_GC_VMM );
//    #else
        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
//    #endif

    unsigned int NewTime = radTimeGetMicroseconds();
    if(type == PC)
    {
        if( (int)mNumCharactersAdded >= GetGameplayManager()->GetNumPlayers() )
        {
            rReleasePrintf("Tried to add too many PCs, not supported right now. Check level scrips for multiple AddCharacter calls.\n");
//            #ifdef RAD_GAMECUBE
//                HeapMgr()->PopHeap( GMA_GC_VMM );
//            #else
                HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
//            #endif
            return NULL;
        }
        MEMTRACK_PUSH_GROUP( "CharacterManager - Add PC" );
        character = new Character;
        character->mbAllowUnload = false;
        //character->SetSimpleShadow( false );
    }
    else
    {
        rAssert( mNumCharactersAdded == (unsigned int)GetGameplayManager()->GetNumPlayers() );

        MEMTRACK_PUSH_GROUP( "CharacterManager - Add NPC" );
        character = new NPCharacter;
    }

    if((strcmp(modelName, "lisa") == 0) || (strncmp(modelName, "l_", 2) == 0))
    {
        character->SetIsLisa(true);
    }

    if((strcmp(modelName, "marge") == 0) || (strncmp(modelName, "m_", 2) == 0))
    {
        character->SetIsMarge(true);
    }

    rAssert( character );
    NewTime = radTimeGetMicroseconds() - NewTime;

//    #ifdef RAD_GAMECUBE
//        HeapMgr()->PopHeap( GMA_GC_VMM );
//    #else
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
//    #endif

    unsigned int SetNameTime = radTimeGetMicroseconds();
    character->SetName( characterName );
    SetNameTime = radTimeGetMicroseconds() - SetNameTime;
       

    unsigned int AddCharacterTime = radTimeGetMicroseconds();

    int characterIndex = AddCharacter( character, type );
    strcpy(mRealModelNames[ characterIndex ], modelName);
    strcpy(mRealAnimNames[ characterIndex ], choreoPuppet);
    mLoaded[characterIndex] = true;
    mGarbage[characterIndex] = false;

    if ( type == PC )
    {
        rAssert( characterIndex == (int)mNumCharactersAdded );
        ++mNumCharactersAdded;
    }

    AddCharacterTime = radTimeGetMicroseconds() - AddCharacterTime;

    unsigned int FillLoadTime = radTimeGetMicroseconds();
    strcpy(mCharacterLoadData[characterIndex].modelName, modelName);
    strcpy(mCharacterLoadData[characterIndex].animName, choreoPuppet);

    FillLoadData( mCharacterLoadData[characterIndex], modelName, choreoPuppet);
    FillLoadTime = radTimeGetMicroseconds() - FillLoadTime;

    unsigned int LoadModelTime = 0;
    unsigned int SetupCharacterTime;

    LoadModelTime = radTimeGetMicroseconds();
    mCharacterModelData[characterIndex] = LoadModel(modelName);
    mCharacterAnimData[characterIndex] = LoadAnimation(choreoPuppet);
    LoadModelTime = radTimeGetMicroseconds() - LoadModelTime;

    if( (GetState(mAnimData, animUID) == LOADED) && (GetState(mModelData, modelUID) == LOADED) )
    {
        SetupCharacterTime = radTimeGetMicroseconds();
        SetupCharacter( mCharacterLoadData[characterIndex], character );
        SetupCharacterTime = radTimeGetMicroseconds() - SetupCharacterTime;
    }
    else
    {
        SetupCharacterTime = radTimeGetMicroseconds();
        SetupCharacter( mDummyLoadData, character );
        SetupCharacterTime = radTimeGetMicroseconds() - SetupCharacterTime;
    }

    unsigned int InitTime = radTimeGetMicroseconds();
    character->Init();
    InitTime = radTimeGetMicroseconds() - InitTime;

    if(type == NPC)
    {
        // Dusit [Oct 22, 2002]:
        // Don't solve collisions for NPCs.. They just sit there...
        // We'll enable solve collisions as needed...
        character->SetSolveCollisions( false );
    }
    else
    {
        // PCs get immediatly added to scene, NPCs might not be visible (i.e pedestrians)
        character->AddToWorldScene();
    }

    if(location)
    {
        //Set the initial character position.
        Locator* loc = p3d::find<Locator>( location );
        CarStartLocator* cloc = dynamic_cast<CarStartLocator*>( loc );

        rmt::Vector position;
        if ( cloc )
        {
            cloc->GetLocation( &position );
            character->RelocateAndReset( position, cloc->GetRotation() );
        }
        else if ( loc )
        {
            rmt::Vector position;
            loc->GetLocation( &position );
            character->RelocateAndReset( position, 0 );
        }
        else
        {
            rDebugPrintf( "Couldn't find locator \"%s\" for %s\n", location, characterName );
        }

        // if NPC, add the locator as the first waypoint of this character
        if( loc &&  type == NPC )
        {
            // the controller should have been created in NPC constructor
            NPCController* npcController = (NPCController*) character->GetController();
            rAssert( npcController );

            bool res = npcController->AddNPCWaypoint( position );
            rAssert( res );
        }
    }
    addCharTime = radTimeGetMicroseconds() - addCharTime;

#ifdef OUTPUT_TIMES
    rReleasePrintf("AddCharacter %s Time %d\n",characterName, addCharTime);
    rReleasePrintf("   AddCharacter MakeUIDTime %s Time %d\n",characterName, MakeUIDTime);
    rReleasePrintf("   AddCharacter NewTime %s Time %d\n",characterName, NewTime);
    rReleasePrintf("   AddCharacter SetNameTime %s Time %d\n",characterName, SetNameTime);
    rReleasePrintf("   AddCharacter AddCharacterTime %s Time %d\n",characterName, AddCharacterTime);
    rReleasePrintf("   AddCharacter FillLoadTime %s Time %d\n",characterName, FillLoadTime);
    rReleasePrintf("   AddCharacter LoadModelTime %s Time %d\n",characterName, LoadModelTime);
    rReleasePrintf("   AddCharacter SetupCharacterTime %s Time %d\n",characterName, SetupCharacterTime);
    rReleasePrintf("   AddCharacter InitTime %s Time %d\n",characterName, InitTime);
#endif
    if(type == PC)
    {
        MEMTRACK_POP_GROUP( "CharacterManager - Add PC" );
    }
    else
    {
        MEMTRACK_POP_GROUP( "CharacterManager - Add NPC" );
    }

    if(type == PC && GetGameplayManager()->IsSuperSprint() == false && GetGameplayManager()->mIsDemo == false)
    {
        //check to see if there is a valid skin that is not called NULL
        if(strcmp(GetCharacterSheetManager()->QueryCurrentSkin(GetGameplayManager()->GetCurrentLevelIndex()),"NULL") != 0)
        {
            SwapData(character, GetCharacterSheetManager()->QueryCurrentSkin(GetGameplayManager()->GetCurrentLevelIndex()), GetAnimName(character));
        }
    }

    return character;
}

Character* CharacterManager::AddCharacterDeferedLoad( CharacterType type, const char* characterName, const char* modelName, const char* choreoPuppet, const char* location)
{
    Character* c = AddCharacter(type, characterName, "npd", "npd", location);

    unsigned characterIndex = INVALID_LOAD;

	for ( int i = 0; i < MAX_CHARACTERS; i++ )
	{
		if ( mpCharacter[ i ] == c )
        {
            characterIndex = i;
            break;
        }
    }

    rAssert(characterIndex != INVALID_LOAD);

    strcpy(mRealModelNames[ characterIndex ], modelName);
    strcpy(mRealAnimNames[ characterIndex ], choreoPuppet);
    mLoaded[characterIndex] = false;

    return c;
}

void CharacterManager::SwapData(Character* character, const char* modelName, const char* choreoPuppet)
{
    unsigned index = InternalSwapData(character, modelName, choreoPuppet);
    rAssert(index != INVALID_LOAD);
    strcpy(mRealModelNames[ index ], modelName);
    strcpy(mRealAnimNames[ index ], choreoPuppet);
}


unsigned CharacterManager::InternalSwapData(Character* character, const char* modelName, const char* choreoPuppet)
{
    unsigned int SwapDataTime = radTimeGetMicroseconds();    


    unsigned characterIndex = INVALID_LOAD;

	for ( int i = 0; i < MAX_CHARACTERS; i++ )
	{
		if ( mpCharacter[ i ] == character )
        {
            characterIndex = i;
            break;
        }
    }

    rAssert(characterIndex != INVALID_LOAD);

    tUID modelUID;
    tUID animUID;

    unsigned modelIndex = mCharacterModelData[characterIndex];
    unsigned animIndex = mCharacterAnimData[characterIndex];

    if(modelName)
    {
        modelUID = tEntity::MakeUID(modelName);
        strcpy(mCharacterLoadData[characterIndex].modelName, modelName);
    }
    else
    {
        modelUID = tEntity::MakeUID(mCharacterLoadData[characterIndex].modelName);
    }

    if(choreoPuppet)
    {
        animUID = tEntity::MakeUID(choreoPuppet);
        strcpy(mCharacterLoadData[characterIndex].animName, choreoPuppet);
    }
    else
    {
        animUID = tEntity::MakeUID(mCharacterLoadData[characterIndex].animName);
    }


    unsigned int FillLoadTime = radTimeGetMicroseconds();
    FillLoadData( mCharacterLoadData[characterIndex], mCharacterLoadData[characterIndex].modelName, mCharacterLoadData[characterIndex].animName);
    FillLoadTime = radTimeGetMicroseconds() - FillLoadTime;

    unsigned int LoadModelTime = 0;
    unsigned int SetupCharacterTime = 0;
    unsigned int CreatePuppetTime = 0;

    LoadModelTime = radTimeGetMicroseconds();
    mCharacterModelData[characterIndex] = LoadModel(mCharacterLoadData[characterIndex].modelName);
    mCharacterAnimData[characterIndex] = LoadAnimation(mCharacterLoadData[characterIndex].animName);
    LoadModelTime = radTimeGetMicroseconds() - LoadModelTime;

    if((GetState(mAnimData, animUID) == LOADED) && (GetState(mModelData, modelUID) == LOADED))
    {
        SetupCharacterTime = radTimeGetMicroseconds();
        CreatePuppetTime = SetupCharacter( mCharacterLoadData[characterIndex], character );
        SetupCharacterTime = radTimeGetMicroseconds() - SetupCharacterTime;
    }
    else
    {
        SetupCharacterTime = radTimeGetMicroseconds();
        CreatePuppetTime = SetupCharacter( mDummyLoadData, character );
        SetupCharacterTime = radTimeGetMicroseconds() - SetupCharacterTime;
    }

    unsigned int GarbageCollectTime = radTimeGetMicroseconds();
    GarbageCollectModel(modelIndex);
    GarbageCollectAnim(animIndex);
    GarbageCollectTime = radTimeGetMicroseconds() - GarbageCollectTime;

    SwapDataTime = radTimeGetMicroseconds() - SwapDataTime;

#ifdef OUTPUT_TIMES
    rReleasePrintf("SwapDataTime %s Time %d\n",modelName, SwapDataTime);
    rReleasePrintf("   FillLoadTime %s Time %d\n",modelName, FillLoadTime);
    if( LoadModelTime != 0 )
        rReleasePrintf("   LoadModelTime %s Time %d\n",modelName, LoadModelTime);
    rReleasePrintf("   GarbageCollectTime %s Time %d\n",modelName, GarbageCollectTime  );
    rReleasePrintf("   SetupCharacterTime %s Time %d\n",modelName, SetupCharacterTime );
    rReleasePrintf("      CreatePuppetTime %s Time %d\n\n",modelName, CreatePuppetTime );    
#endif

    return characterIndex;
}

//=============================================================================
// CharacterManager::SetupCharacter
//=============================================================================
// Description: Comment
//
// Parameters:  ( CharacterLoadData& data, Character* pCharacter, bool isNPC )
//
// Return:      void 
//
//=============================================================================
unsigned int CharacterManager::SetupCharacter( CharacterLoadData& data, Character* pCharacter)
{
    p3d::inventory->PushSection();

    bool currOnly = p3d::inventory->GetCurrentSectionOnly();
    tName curr = p3d::inventory->GetCurrentSection()->GetName();
    p3d::inventory->SetCurrentSectionOnly(true);

//    #ifdef RAD_GAEMCUBE
//        HeapMgr()->PushHeap( GMA_GC_VMM );
//    #else
        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
//    #endif

MEMTRACK_PUSH_GROUP( "CharacterManager - CharacterRenderable" );
    p3d::inventory->SelectSection(data.modelSection.GetUID());

    CharacterRenderable* pCharacterRenderable;
    if(data.modelSection.GetUID() != tEntity::MakeUID("npd_m"))
    {
        pCharacterRenderable = new CharacterRenderable( 
            p3d::find<tDrawablePose>( data.mModelHigh.GetUID() ),
            p3d::find<tDrawablePose>( data.mModelMedium.GetUID() ),
            p3d::find<tDrawablePose>( data.mModelLow.GetUID() ));
        pCharacterRenderable->SetSwatchShader( p3d::find<tShader>( "char_swatches_lit_m" ) );
    }
    else
    {
        pCharacterRenderable = new CharacterRenderable(NULL, NULL, NULL);
        pCharacterRenderable->SetSwatchShader( NULL );
    }

    pCharacterRenderable->SetShadowColour( pCharacter->GetShadowColour() );

    // Lets find the electrocution effect
    p3d::inventory->SelectSection( P3D_DEFAULT_INV_SECTION );
    p3d::inventory->SetCurrentSectionOnly( false );
    pCharacterRenderable->SetShockEffect( p3d::find< tDrawable >( "electrocuted" ) );
    p3d::inventory->SetCurrentSectionOnly( true );
    

    p3d::inventory->SelectSection( "Eakkachaichanvet" );
    pCharacterRenderable->SetSwatchTexture( 0, p3d::find<tTexture>( "char_swatches_lit.bmp" ) );
    pCharacterRenderable->SetSwatchTexture( 1, p3d::find<tTexture>( "char_swatches1.bmp" ) );
    pCharacterRenderable->SetSwatchTexture( 2, p3d::find<tTexture>( "char_swatches2.bmp" ) );
    pCharacterRenderable->SetSwatchTexture( 3, p3d::find<tTexture>( "char_swatches3.bmp" ) );
    pCharacterRenderable->SetSwatchTexture( 4, p3d::find<tTexture>( "char_swatches4.bmp" ) );
    pCharacterRenderable->SetSwatch( 0 );
    p3d::inventory->SelectSection( data.modelSection.GetUID() );

    pCharacter->SetDrawable( pCharacterRenderable );
MEMTRACK_POP_GROUP("CharacterManager - CharacterRenderable");

    tDrawablePose* pDrawablePose = pCharacterRenderable->GetDrawable();
    tSkeleton* pSkeleton = NULL;

    if(pDrawablePose)
    {
        pSkeleton = pDrawablePose->GetSkeleton();
    }
    else
    {
        p3d::inventory->SelectSection(data.animModelSection.GetUID());
        pSkeleton = p3d::find<tSkeleton>( data.mChoreoName.GetUID() );
    }

    if(!pSkeleton)
    {
        tUID uid = tEntity::MakeUID("npd");
        p3d::inventory->SelectSection(uid);
        pSkeleton = p3d::find<tSkeleton>(uid);
    }

    rAssert( pSkeleton);

    p3d::inventory->SelectSection(data.animSection.GetUID());
    choreo::Bank* bank = p3d::find<choreo::Bank>( data.mChoreoName.GetUID() );
    rAssert( bank != 0 );

MEMTRACK_PUSH_GROUP( "CharacterManager - Puppet" );
    unsigned int PuppetTime = radTimeGetMicroseconds();
    // Make sure the skeleton to be remapped are equivalent.
    //
    choreo::Rig* rig = bank->GetRig();
    rAssert( rig != 0 );
    tSkeleton* rigSkeleton = rig->GetSkeleton( );
    int i = 0;
    if ( rigSkeleton->GetNumJoint() != pSkeleton->GetNumJoint() )
    {
        rDebugPrintf( "%s skeleton is not the same as choreo::Rig skeleton %s.  Animations will be hooped.\n",
            rigSkeleton->GetName( ), pSkeleton->GetName( ) );
    }
    
    mUniversalPose->SetSkeleton(pSkeleton);
    mUniversalPose->ResetToRestPose();

    pCharacter->SetInCar(false);
    pCharacter->SetPuppet( new choreo::Puppet( mUniversalPose, bank, false, 0x20, 5 ) );

    choreo::Puppet* pPuppet = pCharacter->GetPuppet();
    
    PuppetTime = radTimeGetMicroseconds() - PuppetTime;

//    #ifdef RAD_GAEMCUBE
//        HeapMgr()->PopHeap( GMA_GC_VMM );
//    #else
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
//    #endif
   
MEMTRACK_POP_GROUP( "CharacterManager - Puppet" );


    //
    // Determine & store y offset needed for this character
    //

    float yAdjustDrawable = 0.0f, yAdjustPuppet = 0.0f;
    if( pCharacterRenderable->GetDrawable() && 
        pCharacterRenderable->GetDrawable()->GetSkeleton() )
    {
        yAdjustDrawable = pCharacterRenderable->GetDrawable()->GetSkeleton()->
        FindJoint( tEntity::MakeUID("Balance_Root") )->worldMatrix.Row(3).y;
    }
    if( pPuppet && pPuppet->GetSkeleton() )
    {
        yAdjustPuppet = pPuppet->GetSkeleton()->
        FindJoint( tEntity::MakeUID("Balance_Root") )->worldMatrix.Row(3).y;
    }

    pCharacter->SetYAdjust((yAdjustDrawable - yAdjustPuppet) * 0.30f);

    //
    // Setup the blinker for this character
    //
    unsigned int characterIndex = GetCharacterIndex( pCharacter );
    g_Blinkers[ characterIndex ].SetCharacter( pCharacter );

    p3d::inventory->SetCurrentSectionOnly(currOnly);
    p3d::inventory->SelectSection( curr );

    p3d::inventory->PopSection();

    return PuppetTime;

}

void CharacterManager::GarbageCollect(bool ignoreDist)
{
	int i;

    const float GARBAGE_COLLECT_DISTANCE = 100.0f;

    // garbage collect
    if(mGarbageCollect)
    {
        for(i = MAX_PLAYERS; i < MAX_CHARACTERS; i++)
        {
            if(mpCharacter[i])
            {
                rmt::Vector charPos;
                rmt::Vector camPos;
                rmt::Vector distance;

                mpCharacter[i]->GetPosition(charPos);
//                camPos = GetSuperCamManager()->GetSCC(0)->GetCamera()->GetPosition();
                GetCharacter(0)->GetPosition(camPos);
                distance.Sub(camPos, charPos);
                
                float fDist = rmt::Abs(distance.Magnitude());

                if(fDist > 10000)
                {
                    continue;
                }
                
                bool suppressLoad = false;

                if(mpCharacter[i]->GetRole() == Character::ROLE_ACTIVE_BONUS)
                {
                    if(!GetGameplayManager()->GetCurrentMission()->IsBonusMission() &&
                       !GetGameplayManager()->GetCurrentMission()->IsRaceMission() &&
                       !GetGameplayManager()->GetCurrentMission()->IsWagerMission() &&
                       !GetGameplayManager()->GetCurrentMission()->IsSundayDrive())
                    {
                        suppressLoad = true;
                    }
                }

                if(mGarbage[i])
                {
                    if(fDist > GARBAGE_COLLECT_DISTANCE || GetGameplayManager()->IsIrisClosed() || ignoreDist )
                    {
                        if(mpCharacter[i]->GetRole() != Character::ROLE_ACTIVE_BONUS)
                        {
                            if(mpCharacter[i]->IsAmbient())
                            {
                                mGarbage[i] = false;
                                mpCharacter[i]->EnableAmbientDialogue(true);
                                mpCharacter[i]->ResetAmbientPosition();
                            }
                            else
                            {
                                RemoveCharacter(mpCharacter[i]);
                            }
                        }
                        else
                        {
                            mGarbage[i] = false;
                        }
                    }
                } 
                else if (mLoaded[i] && (fDist > 150) && mpCharacter[i]->mbAllowUnload)
                {
                    InternalSwapData(mpCharacter[i], "npd", "npd");
                    mLoaded[i] = false;
                    mpCharacter[i]->RemoveFromPhysics();
                } 
                else if (!mLoaded[i] && (fDist < 150) && !suppressLoad)
                {
                    if( !CommandLineOptions::Get( CLO_NO_PEDS ) )
                        InternalSwapData(mpCharacter[i], mRealModelNames[i], mRealAnimNames[i]);
                    mLoaded[i] = true;
                    if(!mpCharacter[i]->IsInCar())
                    {
                        mpCharacter[i]->AddToPhysics();
                    }
                }
            }
                
        }

        for(i = 1; i < MAX_LOADS; i++)
        {
            if(mModelData[i].state == GARBAGE)
            {
                if(mModelData[i].gracePeriod < 0.0f)
                {
                    p3d::inventory->RemoveSectionElements(mModelData[i].section);
                    p3d::inventory->DeleteSection(mModelData[i].section);
                    mModelData[i].state = NOT_LOADED;
                }
            }

            if(mAnimData[i].state == GARBAGE)
            {
                if(mAnimData[i].gracePeriod < 0.0f)
                {
                    p3d::inventory->RemoveSectionElements(mAnimData[i].section);
                    p3d::inventory->DeleteSection(mAnimData[i].section);
                    mAnimData[i].state = NOT_LOADED;
                }
            }
        }
    }
}

/*
==============================================================================
CharacterManager::PreSimUpdate
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void CharacterManager::PreSimUpdate( float timeins )
{
	int i;

	for ( i = 1; i < MAX_CHARACTERS; i++ )
	{
		if ( mpCharacter[ i ] && (mpCharacter[ i ]->GetRenderLayer() == GetRenderManager()->rCurWorldRenderLayer()) )
		{
BEGIN_PROFILE("Character::PreSimUpdate")
			mpCharacter[ i ]->PreSimUpdate( timeins );
END_PROFILE("Character::PreSimUpdate")
		}
	}

    for(i = 1; i < MAX_LOADS; i++)
    {
        if(mModelData[i].state == GARBAGE)
        {
            mModelData[i].gracePeriod -= timeins;
        }

        if(mAnimData[i].state == GARBAGE)
        {
            mAnimData[i].gracePeriod -= timeins;
        }
    }

}

/*
==============================================================================
CharacterManager::PostSimUpdate
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void CharacterManager::PostSimUpdate( float timeins )
{
	int i;
	for ( i = 1; i < MAX_CHARACTERS; i++ )
	{
		if ( mpCharacter[ i ] && (mpCharacter[ i ]->GetRenderLayer() == GetRenderManager()->rCurWorldRenderLayer()) )
		{
BEGIN_PROFILE("Character::PostSimUpdate")
			mpCharacter[ i ]->PostSimUpdate( timeins );
END_PROFILE("Character::PostSimUpdate")
        }
	}
}


/*
==============================================================================
CharacterManager::Update
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void CharacterManager::Update( float timeins )
{
	int i;

	for( i = 0; i < MAX_CHARACTERS; i++ )
	{
        Character* character = mpCharacter[i];
		if( character && (character->GetRenderLayer() == GetRenderManager()->rCurWorldRenderLayer()) )
		{
            g_Blinkers[ i ].Update( static_cast< int >( timeins * 1000 ) );

            int collisionAreaIndex = character->GetCollisionAreaIndex();
            if( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA )
            {
                BEGIN_PROFILE("Character::UpdatePhObjects");
                character->UpdatePhysicsObjects( timeins, collisionAreaIndex );
                END_PROFILE("Character::UpdatePhObjects");
            }

    	    character->UpdateRoot( timeins );

            sim::SimState* simState = character->GetSimState();
            if( simState != NULL && simState->GetControl() == sim::simSimulationCtrl )
            {
                simState->GetSimulatedObject()->Update( timeins );
                character->UpdateSimState( timeins );

                sim::CollisionObject* collObj = simState->GetCollisionObject();
                collObj->Update();

            }
        }
	}

}

/*
==============================================================================
CharacterManager::PreSubstepUpdate
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void CharacterManager::PreSubstepUpdate( float timeins )
{
    for( int i=0; i<MAX_CHARACTERS; i++ )
    {
        if( mpCharacter[ i ] && mpCharacter[ i ]->IsInSubstep() )
	    {
    BEGIN_PROFILE("Character::PreSimUpdate")
		    mpCharacter[ i ]->PreSimUpdate( timeins );
    END_PROFILE("Character::PreSimUpdate")
        }
	}
}

/*
==============================================================================
CharacterManager::PostSubstepUpdate
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void CharacterManager::PostSubstepUpdate( float timeins )
{
	if ( mpCharacter[ 0 ] )
	{
BEGIN_PROFILE("Character::PostSimUpdate")
		mpCharacter[ 0 ]->PostSimUpdate( timeins );
END_PROFILE("Character::PostSimUpdate")
	}
}


void CharacterManager::ClearTargetVehicle(Vehicle* v)
{
	for (int i = 0; i < MAX_CHARACTERS; i++ )
	{
		if ( NULL != mpCharacter[ i ] )
		{
            if(mpCharacter[ i ]->GetTargetVehicle() == v)
            {
                mpCharacter[ i ]->SetTargetVehicle(NULL); 
            }
		}
	}
}

void CharacterManager::ResetBonusCharacters(void)
{
	for (int i = 0; i < MAX_CHARACTERS; i++ )
	{
		if ( NULL != mpCharacter[ i ] )
		{
            if(mpCharacter[ i ]->GetRole() == Character::ROLE_ACTIVE_BONUS)
            {
                ((NPCController*)(mpCharacter[ i ]->GetController()))->TeleportToPath();
            }
		}
	}
}

/*
==============================================================================
CharacterManager::AddCharacter
==============================================================================
Description:     Add a character to the array.  Returns true if succesfully added.

Parameters:     ( Character* pCharacter, int& number, bool isNPC )

Return:         bool 

=============================================================================
*/
int CharacterManager::AddCharacter( Character* pCharacter, CharacterType type )
{
	int i = 0;

    // if it's an NPC, skip the first couple of entries (reserved for players)
    if ( type == NPC )
    {
        i = MAX_PLAYERS;
    }
    
    // find an empty slot
	for (; i < MAX_CHARACTERS; i++ )
	{
		if ( 0 == mpCharacter[ i ] )
		{
			break;
		}
	}

    rAssertMsg(( i < MAX_CHARACTERS ), "Couldn't add character, no space in character array");
    
    tRefCounted::Assign( mpCharacter[ i ], pCharacter );
    mpCharacter[ i ]->SetManaged(true);
    mGarbage[i] = false;
    return i;
}

void CharacterManager::OnProcessRequestsComplete( void* pUserData )
{
    unsigned index = (unsigned)pUserData;


    if(index & 0x10000000)
    {
        index &= 0x0fffffff;

        if(mAnimData[index].state == LOADING)
        {
            mAnimData[index].state = LOADED;

            for(int i = 0; i < MAX_CHARACTERS; i++)
            {
                if(mpCharacter[i] && (mCharacterAnimData[i] == index))
                {
                    if(mModelData[mCharacterModelData[i]].state == LOADED)
                    {
                        unsigned int PuppetTime = SetupCharacter(mCharacterLoadData[i], mpCharacter[i] );
    #ifdef OUTPUT_TIMES
                        rReleasePrintf( "A) Calling SetupCharacter from OnProcessRequestsComplete for %s took %d\n",
                            mCharacterLoadData[i].modelName, PuppetTime );
    #endif
                    }
                }
            }
        } else if(mAnimData[index].state == LOADING_GARBAGE)
        {
            mAnimData[index].state = GARBAGE;
        }
        else
        {
            rAssert(0);
        }
    }
    else
    {
        if(mModelData[index].state == LOADING)
        {
            mModelData[index].state = LOADED;

            for(int i = 0; i < MAX_CHARACTERS; i++)
            {
                if(mpCharacter[i] && (mCharacterModelData[i] == index))
                {
                    if(mAnimData[mCharacterAnimData[i]].state == LOADED)
                    {
                        unsigned int PuppetTime = SetupCharacter(mCharacterLoadData[i], mpCharacter[i] );
    #ifdef OUTPUT_TIMES
                        rReleasePrintf( "B) Calling SetupCharacter from OnProcessRequestsComplete for %s took %d\n",
                            mCharacterLoadData[i].modelName, PuppetTime );
    #endif
                    }
                }
            }

            if(mModelData[index].callback)
            {
                mModelData[index].callback->OnProcessRequestsComplete(mModelData[index].userData);
                mModelData[index].callback = NULL;
            }
        } else if(mModelData[index].state == LOADING_GARBAGE)
        {
            mModelData[index].state = GARBAGE;
        }
        else
        {
            rAssert(0);
        }

    }
}

//=============================================================================
Character* CharacterManager::GetCharacterByName( const char* name ) const
{
    return GetCharacterByName( tEntity::MakeUID( name ) );
}

/*
==============================================================================
CharacterManager::GetCharacterByName
==============================================================================
Description:    Comment

Parameters:     ( const tUID uid )

Return:         Character

=============================================================================
*/
Character* CharacterManager::GetCharacterByName( const tUID uid ) const
{
    int i;
    for( i = 0; i < MAX_CHARACTERS; i++ )
    {
        if ( mpCharacter[ i ] != 0 && mpCharacter[ i ]->GetUID() == uid )
        {
            return mpCharacter[ i ];
        }
    }
    return NULL;

}

//=============================================================================
Character* CharacterManager::GetMissionCharacter( const char* name ) const
{
    Character* c = NULL;

    // pick an appropriate name based on if we are a bonus mission or not
    char n[64];
    if(GetGameplayManager()->GetCurrentMission()->IsBonusMission())
    {
        sprintf(n, "b_%s", name);
        c = GetCharacterManager()->GetCharacterByName( n );
    }

    if(!c)
    {
        sprintf(n, "reward_%s", name);
        c = GetCharacterManager()->GetCharacterByName( n );
    }

    if(!c)
    {
        c = GetCharacterManager()->GetCharacterByName( name );
    }

    return c;
}

/*
==============================================================================
CharacterManager::GetCharacter
==============================================================================
Description:    Return a pointer to the character.

Parameters:     ( int i )

Return:         Character

=============================================================================
*/
Character* CharacterManager::GetCharacter( int i ) const
{
	if ( i < MAX_CHARACTERS )
	{
		return mpCharacter[ i ];
	}
	else
	{
        return NULL;
	}
}

/*
==============================================================================
CharacterManager::HandleEvent
==============================================================================
Description:    Comment

Parameters:     ( EventEnum id, void* pEventData )

Return:         void 

=============================================================================
*/
void CharacterManager::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
    case EVENT_GETINTOVEHICLE_START :
        {
            Character* c = (Character*)pEventData;
            if(c->GetTargetVehicle())
            {
                if(c->GetTargetVehicle()->mpDriver && (c->GetTargetVehicle()->mpDriver != c))
                {
                    c->GetTargetVehicle()->mpDriver->GetController()->SetIntention(CharacterController::WaveHello);
                }
            }
        }
        break;
    case EVENT_GETOUTOFVEHICLE_END :
        {
            Character* c = (Character*)pEventData;
            if(c->GetTargetVehicle())
            {
                if(c->GetTargetVehicle()->mpDriver && (c->GetTargetVehicle()->mpDriver != c))
                {
                    c->GetTargetVehicle()->mpDriver->GetController()->SetIntention(CharacterController::WaveGoodbye);
                }
            }
        }
        break;
    case EVENT_TOGGLE_FIRSTPERSON :
        {
            CGuiScreenHud* currentHud = GetCurrentHud();
            if(pEventData == 0)
            {            
                if(GetCharacter(0)->GetActionButtonHandler())
                {
                    if ( GetCharacter(0)->GetActionButtonHandler()->IsInstanceEnabled() )
                    {
                        if( currentHud != NULL )
                        {
                            currentHud->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_ACTION_BUTTON );
                        }
                    }
                }
            }
            else
            {
                if( currentHud != NULL )
                {
                    currentHud->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_ACTION_BUTTON );
                }
            }
        }
        break;

    case EVENT_STAGE_COMPLETE :
        {
            if(pEventData)
            {
                Character* ch = GetCharacter(0);
                ch->GetController()->SetIntention(CharacterController::CelebrateSmall);
            }
        }
        break;

    case EVENT_CARD_COLLECTED :
    case EVENT_MISSION_SUCCESS :
        {
            Character* ch = GetCharacter(0);
            ch->GetController()->SetIntention(CharacterController::CelebrateBig);
        }
        break;

    case EVENT_MISSION_CHARACTER_RESET :
        {
            if(pEventData && (sInitialWalkLocator[0] != 0))
            {
                rmt::Vector dest;
                Locator* l = p3d::find<Locator>(sInitialWalkLocator);

                if(l)
                {
                    l->GetLocation(&dest);

                    Character* c = GetCharacterManager()->GetCharacter(0);

                    if(c->GetWalkerLocomotionAction()->GetStatus() != RUNNING)
                    {
                        c->GetWalkerLocomotionAction()->SetStatus(SLEEPING);
                    }

                    Sequencer* pSeq = c->GetActionController()->GetNextSequencer();
                    pSeq->BeginSequence();
                    pSeq->AddAction( new Arrive( c, dest) );
                    pSeq->AddAction( c->GetWalkerLocomotionAction() );
                    pSeq->EndSequence( );

                }
            }
            sInitialWalkLocator[0] = 0;
        }
        break;

    case EVENT_LOCATOR + LocatorEvent::CAR_DOOR:
        {
            EventLocator* pLocator = static_cast<EventLocator*>( pEventData );
            rAssert( pLocator );
            unsigned int playerId = pLocator->GetPlayerID();
            Character* pCharacter = GetCharacter( playerId );

            unsigned int actionId = pLocator->GetData( );
            rAssert( actionId >= 0 );
            
            ActionButton::GetInCar* pGetInCarAction = static_cast<ActionButton::GetInCar* >( GetActionButtonManager()->GetActionByIndex( actionId ) );
            rAssert( dynamic_cast<ActionButton::GetInCar*>( pGetInCarAction ) );
            rAssert( pGetInCarAction ); 

            if ( pLocator->GetPlayerEntered() )
            {
                
                // Entered a door volume.
                //

#ifdef RAD_DEBUG
                int vehicleId = pGetInCarAction->GetVehicleId();
                Vehicle* pVehicle = GetVehicleCentral()->GetVehicle( vehicleId );
                rAssert( pVehicle );
#endif

                pCharacter->AddActionButtonHandler( pGetInCarAction );
                pGetInCarAction->Enter( pCharacter );

                // TODO: Think about what will happen when the character is in two vols.
                // at the same time.
                //

            }
            else if ( !pLocator->GetPlayerEntered() )
            {
                // Exited a door volume.
                //
                pGetInCarAction->Exit( pCharacter );
                pCharacter->RemoveActionButtonHandler( pGetInCarAction );
            }
            break;
        }
    case EVENT_LOCATOR + LocatorEvent::BOUNCEPAD:
        {

            EventLocator* pLocator = static_cast<EventLocator*>( pEventData );
            if ( pLocator->GetPlayerEntered() )
            {
                rAssert( pLocator );
                unsigned int playerId = pLocator->GetPlayerID();
                Character* pCharacter = GetCharacter( playerId );

                ActionButton::Bounce::OnEnter( pCharacter, pLocator );
            }
            break;
        }
    case EVENT_LOCATOR + LocatorEvent::GENERIC_BUTTON_HANDLER_EVENT:
        {
            EventLocator* pLocator = static_cast<EventLocator*>( pEventData );
            rAssert( pLocator );

            if ( pLocator->GetPlayerEntered() )
            {
                unsigned int playerId = pLocator->GetPlayerID();
                Character* pCharacter = GetCharacter( playerId );
                
                unsigned int actionId = pLocator->GetData( );
                rAssert( actionId >= 0 );
            
                ActionButton::GenericEventButtonHandler* pTED = static_cast<ActionButton::GenericEventButtonHandler* >( GetActionButtonManager()->GetActionByIndex( actionId ) );
                rAssert( dynamic_cast< ActionButton::GenericEventButtonHandler* > (pTED) );
                rAssert( pTED ); 

                if ( pCharacter != static_cast<Character*>(pTED->GetEventData()) )
                {
                    pCharacter->AddActionButtonHandler( pTED );
                    pTED->Enter( pCharacter );
                }
            }
            else
            {
                unsigned int playerId = pLocator->GetPlayerID();
                Character* pCharacter = GetCharacter( playerId );
                
                unsigned int actionId = pLocator->GetData( );
                rAssert( actionId >= 0 );
            
                ActionButton::GenericEventButtonHandler* pTED = static_cast<ActionButton::GenericEventButtonHandler* >( GetActionButtonManager()->GetActionByIndex( actionId ) );

                if ( pTED && (pCharacter != static_cast<Character*>(pTED->GetEventData())) )
                {
                    rAssert( dynamic_cast<ActionButton::GenericEventButtonHandler* >( pTED ) != NULL );
                    pTED->Exit( pCharacter );
                    pCharacter->RemoveActionButtonHandler( pTED );
                }
            }

            break;
        }
    case EVENT_DEATH_VOLUME_SCREEN_BLANK:
        {
            //TODO:  Make the blends go away.
            //Also, cut the camera properly...
            EventLocator* pLocator = static_cast<EventLocator*>( pEventData );
            if( pLocator == NULL )
            {
                break;
            }

            if ( pLocator->GetPlayerID() >= static_cast<unsigned int>(MAX_PLAYERS) )
            {
                //Ignore this!
                break;
            }

            //Ignore when player leaves.
            if ( pLocator->GetPlayerEntered() )
            {
                unsigned int playerId = pLocator->GetPlayerID();
                Character* pCharacter = GetCharacter( playerId );

                //Only do this if he's on foot.
                if ( pCharacter->GetStateManager()->GetState() == CharacterAi::GET_IN)
                {
                    GetAvatarManager()->PutCharacterInCar(pCharacter, pCharacter->GetTargetVehicle());
                    GetEventManager()->TriggerEvent(EVENT_GETINTOVEHICLE_END, pCharacter);
                }
                else 
                {
                    if ( pCharacter->GetStateManager()->GetState() == CharacterAi::GET_OUT )
                    {
                        GetAvatarManager()->PutCharacterOnGround( pCharacter, pCharacter->GetTargetVehicle() );
                    }
                    else if ( pCharacter->IsInCar() )
                    {
                        return;
                    }
                    
                    rmt::Matrix mat = pLocator->GetMatrix();
                    rmt::Vector facing = mat.Row(2);
                    rmt::Vector pos;
                    pLocator->GetLocation( &pos );
                    
                    float fDesiredDir = choreo::GetWorldAngle( facing.x, facing.z );           
                    pCharacter->RelocateAndReset( pos, fDesiredDir, true, false );

                    GetSuperCamManager()->GetSCC( 0 )->DoCameraCut();
                }
            }

            break;
        }
    default:
        {
            break;
        }
    }
}

const char* CharacterManager::GetModelName(Character* c)
{
    for(int i = 0; i < MAX_CHARACTERS; i++)
    {
        if(c == mpCharacter[i])
        {
            return mRealModelNames[i];
        }
    }
    return NULL;
}
    
const char* CharacterManager::GetAnimName(Character* c)
{
    for(int i = 0; i < MAX_CHARACTERS; i++)
    {
        if(c == mpCharacter[i])
        {
            return mRealAnimNames[i];
        }
    }
    return NULL;
}

/*
==============================================================================
CharacterManager::SubmitStatics
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void CharacterManager::SubmitStatics( void )
{
    int i;
    for ( i = 0; i < MAX_CHARACTERS; i++ )
	{
        Character* character = mpCharacter[i];
		if( character != NULL )
        {
            character->SubmitStatics();
        }
    }
}

/*
==============================================================================
CharacterManager::SubmitAnimCollisions
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void CharacterManager::SubmitAnimCollisions( void )
{
    int i;
    for ( i = 0; i < MAX_CHARACTERS; i++ )
	{
		if ( mpCharacter[ i ]  )
        {
            mpCharacter[ i ]->SubmitAnimCollisions();
        }
    }
}

    /*
==============================================================================
CharacterManager::SubmitDynamics
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         void 

=============================================================================
*/
void CharacterManager::SubmitDynamics( void )
{
    int i;
    for ( i = 0; i < MAX_CHARACTERS; i++ )
	{
        Character* character = mpCharacter[i];
        if( character != NULL )
        {
            character->SubmitDynamics();
        }
    }
}


/*
==============================================================================
CharacterManager::SetCharacterPosition
==============================================================================
Description:    Comment

Parameters:     ( int argc, char** argv )

Return:         void 

=============================================================================
*/
void CharacterManager::SetCharacterPosition( int argc, char** argv )
{
    int index = ::atoi( argv[ 1 ] );
    Character* pCharacter = GetCharacterManager()->GetCharacter( index );
    if ( pCharacter )
    {
        rmt::Vector pos;
        pos.x = static_cast<float>( atoi( argv[ 2 ] ) );
        pos.y = 0.0f;
        pos.z = static_cast<float>( atoi( argv[ 3 ] ) );
        pCharacter->SetPosition( pos );
    }   
}
/*
==============================================================================
CharacterManager::ResetCharacter
==============================================================================
Description:    Comment

Parameters:     ( int argc, char** argv )

Return:         void 

=============================================================================
*/
void CharacterManager::ResetCharacter( int argc, char** argv )
{
    int index = ::atoi( argv[ 1 ] );
    Character* pCharacter = GetCharacterManager()->GetCharacter( index );
    Locator* loc = p3d::find<Locator>( argv[2] );
    if ( pCharacter && loc )
    {
        rmt::Vector pos;
        loc->GetLocation( &pos );
        pCharacter->RelocateAndReset(pos, pCharacter->GetDesiredDir());
    }
}


char CharacterManager::sInitialWalkLocator[64] = "";

void CharacterManager::SetInitialWalk(int argc, char** argv)
{
    strcpy(sInitialWalkLocator, argv[1]);
}

char CharacterManager::sCharacterToSpawn[64] = "homer";
Character* CharacterManager::sSpawnedCharacter = NULL;

void CharacterManager::Spawn(void*)
{
    if(sSpawnedCharacter)
    {
        GetCharacterManager()->RemoveCharacter(sSpawnedCharacter);
        sSpawnedCharacter = NULL;
    }

    sSpawnedCharacter = GetCharacterManager()->AddCharacter(NPC, "dummy", sCharacterToSpawn, "npd", "next_to_pc"); 

    rmt::Vector position;
    rmt::Vector facing;
    GetCharacterManager()->GetCharacter(0)->GetPosition( &position );
    GetCharacterManager()->GetCharacter(0)->GetFacing( facing );
    position += facing;
    position += facing;

    sSpawnedCharacter->RelocateAndReset( position, 0);

    if(sSpawnedCharacter)
        sSpawnedCharacter->AddToWorldScene();
}


static const int sNumBartSkins = 7;
static int sCurBartSkin = 1;
static char sBartSkins [sNumBartSkins][16] = 
{
    "bart",
    "b_man",
    "b_military",
    "b_ninja",
    "b_football",
    "b_tall",
    "b_hugo"
};

static const int sNumHomerSkins = 7;
static int sCurHomerSkin = 1;
static char sHomerSkins [sNumHomerSkins][16] = 
{
    "homer",
    "h_fat",
    "h_donut",
    "h_stcrobe",
    "h_undrwr",
    "h_scuzzy",
    "h_evil"
};

static const int sNumLisaSkins = 4;
static int sCurLisaSkin = 1;
static char sLisaSkins [sNumLisaSkins][16] = 
{
    "lisa",
    "l_cool",
    "l_florida",
    "l_jersey"
};

static const int sNumMargeSkins = 4;
static int sCurMargeSkin = 1;
static char sMargeSkins [sNumMargeSkins][16] = 
{
    "marge",
    "m_police",
    "m_pink",
    "m_prison"
};


static const int sNumApuSkins = 4;
static int sCurApuSkin = 1;
static char sApuSkins [sNumApuSkins][16] = 
{
    "apu",
    "a_american",
    "a_besharp",
    "a_army"
};

void CharacterManager::NextSkin( void* )
{
    tUID bart = tEntity::MakeUID("bart");
    tUID homer = tEntity::MakeUID("homer");
    tUID lisa = tEntity::MakeUID("lisa");
    tUID marge = tEntity::MakeUID("marge");
    tUID apu = tEntity::MakeUID("apu");

    Character* pc = GetCharacterManager()->GetCharacter(0);

    if(pc->GetUID() == bart)
    {
        GetCharacterManager()->SwapData(pc, sBartSkins[sCurBartSkin], "bart");
        sCurBartSkin = (sCurBartSkin + 1) % sNumBartSkins;
    }

    if(pc->GetUID() == homer)
    {
        GetCharacterManager()->SwapData(pc, sHomerSkins[sCurHomerSkin], "homer");
        sCurHomerSkin = (sCurHomerSkin + 1) % sNumHomerSkins;
    }

    if(pc->GetUID() == lisa)
    {
        GetCharacterManager()->SwapData(pc, sLisaSkins[sCurLisaSkin], "lisa");
        sCurLisaSkin = (sCurLisaSkin + 1) % sNumLisaSkins;
    }

    if(pc->GetUID() == marge)
    {
        GetCharacterManager()->SwapData(pc, sMargeSkins[sCurMargeSkin], "marge");
        sCurMargeSkin = (sCurMargeSkin + 1) % sNumMargeSkins;
    }

    if(pc->GetUID() == apu)
    {
        GetCharacterManager()->SwapData(pc, sApuSkins[sCurApuSkin], "apu");
        sCurApuSkin = (sCurApuSkin + 1) % sNumApuSkins;
    }

}

static const int s_numCheatModels = 107;

static char s_CheatModels[s_numCheatModels][107] =
{
    "apu",
    "askinner",
    "a_american",
    "a_army",
    "a_besharp",
    "barney",
    "bart",
    "beeman",
    "brn_unf",
    "burns",
    "b_football",
    "b_hugo",
    "b_man",
    "b_military",
    "b_ninja",
    "b_tall",
    "captain",
    "carl",
    "cbg",
    "cletus",
    "dolph",
    "eddie",
    "frink",
    "gil",
    "grandpa",
    "hibbert",
    "homer",
    "h_donut",
    "h_evil",
    "h_fat",
    "h_scuzzy",
    "h_stcrobe",
    "h_undrwr",
    "jasper",
    "jimbo",
    "kearney",
    "krusty",
    "lenny",
    "lisa",
    "lou",
    "louie",
    "l_cool",
    "l_florida",
    "l_jersey",
    "marge",
    "milhouse",
    "moe",
    "moleman",
    "m_pink",
    "m_police",
    "m_prison",
    "ned",
    "nelson",
    "nriviera",
    "otto",
    "patty",
    "ralph",
    "selma",
    "skinner",
    "smithers",
    "snake",
    "teen",
    "wiggum",
    "willie",
    "boy1",
    "boy2",
    "boy3",
    "bum",
    "busm1",
    "busm2",
    "busw1",
    "const1",
    "const2",
    "farmr1",
    "fem1",
    "fem2",
    "fem3",
    "fem4",
    "girl1",
    "girl2",
    "hooker",
    "joger1",
    "joger2",
    "male1",
    "male2",
    "male3",
    "male4",
    "male5",
    "male6",
    "mobstr",
    "nuclear",
    "olady1",
    "olady2",
    "olady3",
    "rednk1",
    "rednk2",
    "sail1",
    "sail2",
    "sail3",
    "sail4",
    "zfem1",
    "zfem5",
    "zmale1",
    "zmale3",
    "zmale4",
    "frankenstein",
    "witch"
};

static int s_curCheatModel = 0;

void CharacterManager::NextCheatModel(void)
{
    if( CommandLineOptions::Get( CLO_NO_PEDS ) )
        return;

    Character* pc = GetCharacterManager()->GetCharacter(0);
    GetCharacterManager()->SwapData(pc, s_CheatModels[s_curCheatModel], GetCharacterManager()->GetAnimName(pc));
    s_curCheatModel = (s_curCheatModel + 1) % s_numCheatModels;
}

#include <mission/gameplaymanager.h>
#include <meta/zoneeventlocator.h>

static const unsigned MAX_TELEPORT_DESTS = 64;
static ZoneEventLocator* s_dynaloadLoc = NULL;
static unsigned s_numTeleportDests = 0;
struct TeleportDest
{
    char name[32]; 
    bool useLoc;
    char loc[32];
    rmt::Vector pos;
    char zone[64];
} s_teleportDests[MAX_TELEPORT_DESTS];

unsigned CharacterManager::GetNumTeleportDests(void)
{
    return s_numTeleportDests;
}

const char* CharacterManager::GetTeleportDest(unsigned i)
{
    static char dummy[] = "Invalid Teleport Destination";

    if(i < s_numTeleportDests)
    {
        return s_teleportDests[i].name;
    }
    
    return dummy;
}

void CharacterManager::ClearTeleportDests(void)
{
    for(unsigned i = 0; i < s_numTeleportDests; i++)
    {
        radDbgWatchDelete( &DoTeleport );
    }
    tRefCounted::Release(s_dynaloadLoc);
    s_numTeleportDests = 0;
}

void CharacterManager::AddTeleportDest(int argc, char** argv)
{
    rAssert(argc != 5);
    strcpy(s_teleportDests[s_numTeleportDests].name, argv[1]);

    if(argc == 4)
    {
        strcpy(s_teleportDests[s_numTeleportDests].loc, argv[2]);
        strcpy(s_teleportDests[s_numTeleportDests].zone, argv[3]);
        s_teleportDests[s_numTeleportDests].useLoc = true;
    }
    else
    {
        s_teleportDests[s_numTeleportDests].pos.Set((float)atof(argv[2]), (float)atof(argv[3]), (float)atof(argv[4]));
        strcpy(s_teleportDests[s_numTeleportDests].zone, argv[5]);
        s_teleportDests[s_numTeleportDests].useLoc = false;
    }
    radDbgWatchAddFunction( argv[1], &DoTeleport, (void*)s_numTeleportDests, "Teleport");
    s_numTeleportDests++;
}

void CharacterManager::DoTeleport(void* data)
{
    int which = (int)data; 
    GetRenderManager()->mpLayer( RenderEnums::LevelSlot )->DumpAllDynaLoads(1, GetRenderManager()->mEntityDeletionList );

    tRefCounted::Assign(s_dynaloadLoc,new(GetGameplayManager()->GetCurrentMissionHeap()) ZoneEventLocator);
    s_dynaloadLoc->SetZoneSize( strlen(s_teleportDests[which].zone) + 1 );
    s_dynaloadLoc->SetZone( s_teleportDests[which].zone );
    s_dynaloadLoc->SetPlayerEntered();
    GetEventManager()->TriggerEvent( EVENT_FIRST_DYNAMIC_ZONE_START, s_dynaloadLoc );

    rmt::Vector pos = s_teleportDests[which].pos;
    if(s_teleportDests[which].useLoc)
    {
        Locator* l = p3d::find<Locator>(s_teleportDests[which].loc);
        l->GetLocation(&pos);
    }

    if(GetCharacterManager()->GetCharacter(0)->IsInCar())
    {
        GetCharacterManager()->GetCharacter(0)->GetTargetVehicle()->SetPosition(&pos);
    }
    else
    {
        GetCharacterManager()->GetCharacter(0)->RelocateAndReset(pos, 0);
    }
}


//=============================================================================
// CharacterManager::ResetCharacter
//=============================================================================
// Description:    Comment
//
// Parameters:     ( int argc, char** argv )
//
// Return:         void 
//
//=============================================================================
unsigned int CharacterManager::GetCharacterIndex( const Character* character ) const
{
    unsigned int i;
    unsigned int size = MAX_CHARACTERS;
    for( i = 0; i < size; ++i )
    {
        if( mpCharacter[ i ] == character )
        {
            return i;
        }
    }
    rAssertMsg( false, "Searched for a chraracter that does not exist" );
    return 0;
}