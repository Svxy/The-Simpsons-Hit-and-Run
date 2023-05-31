//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        presentation.cpp
//
// Description: Implement PresentationManager
//
// History:     16/04/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <raddebugwatch.hpp>
#include <p3d/view.hpp>

//========================================
// Project Includes
//========================================
#include <events/eventmanager.h>
#include <events/eventenum.h>
#include <events/eventdata.h>
#include <memory/srrmemory.h>
#include <contexts/context.h>
#include <contexts/bootupcontext.h>
#include <contexts/gameplay/gameplaycontext.h>
#include <gameflow/gameflow.h>
#include <meta/eventlocator.h>
#include <meta/triggervolume.h>
#include <presentation/playerdrawable.h>
#include <presentation/presentation.h>
#include <presentation/presentationanimator.h>
#include <presentation/nisplayer.h>
#include <presentation/tutorialmanager.h>
#include <presentation/cameraplayer.h>
#include <presentation/transitionplayer.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guitextbible.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/ingame/guiscreenmissionload.h>
#include <presentation/mouthflapper.h>
#include <presentation/fmvplayer/fmvplayer.h>

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>
#include <camera/conversationcam.h>
#include <camera/isupercamtarget.h>

#include <worldsim/character/character.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/character/charactertarget.h>
#include <worldsim/avatarmanager.h>
#include <p3d/matrixstack.hpp>

#include <render/rendermanager/rendermanager.h>
#include <render/RenderManager/RenderLayer.h>
#include <render/enums/renderenums.h>
#include <mission/gameplaymanager.h>
#include <mission/objectives/missionobjective.h>
#include <screen.h>
//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
PresentationManager* PresentationManager::spInstance = NULL;
const int PLAYER_ONE = 0;
const int MAX_DIALOG_LINES = 64;
#define   MAX_CHARACTERS 64
//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//=============================================================================
// PresentationManager::CreateInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      PresentationManager
//
//=============================================================================
PresentationManager* PresentationManager::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "PresentationManager" );
    if( spInstance == NULL )
    {
        spInstance = new(GMA_PERSISTENT) PresentationManager;
        rAssert( spInstance );
    }
MEMTRACK_POP_GROUP( "PresentationManager" );
    
    return spInstance;
}

//=============================================================================
// PresentationManager::GetInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      PresentationManager
//
//=============================================================================
PresentationManager* PresentationManager::GetInstance()
{
    return spInstance;
}

//=============================================================================
// PresentationManager::DestroyInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PresentationManager::DestroyInstance()
{
    if( spInstance != NULL )
    {
        delete( GMA_PERSISTENT, spInstance );
        spInstance = NULL;
    }
}

//==============================================================================
// PresentationManager::PresentationManager
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
PresentationManager::PresentationManager() :
    mTransitionPool( NULL ),
    mNISPool( NULL ),
    mFMVPool( NULL ),
    mFIFOBegin( 0 ),
    mFIFOEnd( 0 ),
    mpCurrent( NULL ),
    mpFMVPlayer( NULL ),
    mpNISPlayer( NULL ),
    mpCameraPlayer( NULL ),
    mpTransitionPlayer( NULL ),
//    mLanguage( Language::MAX_LANGUAGES ),
    mp_PCAnimator( NULL ),
    mp_NPCAnimator( NULL ),
    mDialogLineNumber( -1 ),
    mCameraForLineOfDialog(MAX_DIALOG_LINES),
    mpPlayCallback( 0 ),
    mWaitingOnFade( false ),
    mOverlay( 0 )
{
    mInConversation = false;
    // mOldFOV= 0.0F;

    unsigned int i;

    for( i = 0; i < MAX_EVENT_SIZE; i++ )
    {
        mEventFIFO[ i ] = NULL;
    }

    // what is the current allocator
    radMemoryAllocator current =  HeapManager::GetInstance()->GetCurrentAllocator();
    GameMemoryAllocator gmaCurrent = HeapManager::GetInstance()->GetCurrentHeap();

    mFMVPool            = new FMVEventPool( gmaCurrent, 10 );
    mNISPool            = new NISEventPool( gmaCurrent, 10 );
    mTransitionPool     = new TransitionEventPool( gmaCurrent, 10 );
    mpNISPlayer         = new NISPlayer();
    mpCameraPlayer      = new CameraPlayer();
    mpTransitionPlayer  = new TransitionPlayer();
    mpFMVPlayer         = new( current ) FMVPlayer();
    mpPlayerDrawable    = new PlayerDrawable();
    mpPlayerDrawable->AddRef();
    mOverlay            = new PresentationOverlay();
    mOverlay->AddRef();

    mp_oldcam = NULL;
    mOldCamIndexNum =0;  
}

//==============================================================================
// PresentationManager::~PresentationManager
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
PresentationManager::~PresentationManager()
{
    delete mFMVPool;            mFMVPool            = NULL;
    delete mNISPool;            mNISPool            = NULL;
    delete mTransitionPool;     mTransitionPool     = NULL;
    delete mpNISPlayer;         mpNISPlayer         = NULL;
    delete mpCameraPlayer;      mpCameraPlayer      = NULL;
    delete mpTransitionPlayer;  mpTransitionPlayer  = NULL;
    delete mpFMVPlayer;         mpFMVPlayer         = NULL;

    this->FinalizePlayerDrawable();

    mpPlayerDrawable->ReleaseVerified();
    mOverlay->Release();
}

//=============================================================================
// PresentationManager::Initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PresentationManager::Initialize()
{
MEMTRACK_PUSH_GROUP( "PresentationManager" );
    //Cary:  I took this out since it was allocating to the levelslot when
    //the level isn't even loaded.  No one seems to know what it's for
    //so Darryl probably did it and it's not used anymore.  Feb 13th 2003
    //pLayer = GetRenderManager()->mpLayer( RenderEnums::LevelSlot );
    //pLayer->AddGuts( mpPlayerDrawable );

    GetEventManager()->AddListener( this, EVENT_LOCATOR );

    //Listen for Conversation 
    GetEventManager()->AddListener( this, EVENT_CONVERSATION_INIT  );
    GetEventManager()->AddListener( this, EVENT_CONVERSATION_SKIP  );
    GetEventManager()->AddListener( this, EVENT_CONVERSATION_DONE  );

    //Listen for MouthFlapping Cues
    GetEventManager()->AddListener( this, EVENT_PC_TALK );
    GetEventManager()->AddListener( this, EVENT_PC_SHUTUP );
    GetEventManager()->AddListener( this, EVENT_NPC_TALK );
    GetEventManager()->AddListener( this, EVENT_NPC_SHUTUP );
    GetEventManager()->AddListener( this, EVENT_GUI_FADE_OUT_DONE );

    rAssert( mp_PCAnimator == NULL );
    rAssert( mp_NPCAnimator == NULL );
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    mp_PCAnimator = new PresentationAnimator();
    mp_NPCAnimator = new PresentationAnimator();
    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
MEMTRACK_POP_GROUP( "PresentationManager" );
}

//=============================================================================
// PresentationManager::Finalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PresentationManager::Finalize()
{
    mCameraForLineOfDialog.erase( mCameraForLineOfDialog.begin(), mCameraForLineOfDialog.end() );

    mp_PCAnimator->SetCharacter( NULL );
    mp_NPCAnimator->SetCharacter( NULL );
    mp_PCAnimator->ClearAmbientAnimations();
    mp_NPCAnimator->ClearAmbientAnimations();

    //remove for Conversation 
    GetEventManager()->RemoveListener( this, EVENT_CONVERSATION_INIT );
    GetEventManager()->RemoveListener( this, EVENT_CONVERSATION_SKIP );
    GetEventManager()->RemoveListener( this, EVENT_CONVERSATION_DONE );

    //remove for MouthFlapping Cues
    GetEventManager()->RemoveListener( this, EVENT_PC_TALK );
    GetEventManager()->RemoveListener( this, EVENT_PC_SHUTUP );
    GetEventManager()->RemoveListener( this, EVENT_NPC_TALK );
    GetEventManager()->RemoveListener( this, EVENT_NPC_SHUTUP );

    delete mp_PCAnimator;       mp_PCAnimator       = NULL;
    delete mp_NPCAnimator;      mp_NPCAnimator      = NULL;
}

//=============================================================================
// PresentationManager::InitializePlayerDrawable
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void
PresentationManager::InitializePlayerDrawable()
{
    RenderLayer* pLayer = GetRenderManager()->mpLayer( RenderEnums::PresentationSlot );
    rAssert( pLayer != NULL );
    pLayer->pView( 0 )->SetClearColour( tColour( 0, 0, 0 ) );
    pLayer->AddGuts( mpPlayerDrawable );
}

//=============================================================================
// PresentationManager::FinalizePlayerDrawable
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void
PresentationManager::FinalizePlayerDrawable()
{
    RenderLayer* pLayer = GetRenderManager()->mpLayer( RenderEnums::PresentationSlot );
    rAssert( pLayer != NULL );
    pLayer->RemoveGuts( mpPlayerDrawable );
}

//=============================================================================
// PresentationManager::GetAnimatorNpc
//=============================================================================
// Description: returns the presentation animator for the NPC
//
// Parameters:  NONE
//
// Return:      pointer to the presentationanimator 
//
//=============================================================================
PresentationAnimator* PresentationManager::GetAnimatorNpc()
{
    return this->mp_NPCAnimator;
}

//=============================================================================
// PresentationManager::GetAnimatorPc
//=============================================================================
// Description: returns the presentation animator for the PC
//
// Parameters:  NONE
//
// Return:      pointer to the presentationanimator 
//
//=============================================================================
PresentationAnimator* PresentationManager::GetAnimatorPc()
{
    return this->mp_PCAnimator;
}

/*=============================================================================
Description:    You're one stop method for playing an FMV during gameplay. It
                unloads the HUD (for memory), plays the FMV, and then
                reloads the HUD. Don't use this in the frontend as the
                un/reloading of the frontend is a waste.
                NOTE that the platform specific path for the movies is prepended
                to the filename. So if you pass "fmv2.rmv" you'll get
                "d:\movies\fmv2.rmv" on XBox, "movies\fmv2.rmv" on PS2,
                "movies/fmv2.rmv" on GC.
=============================================================================*/
void PresentationManager::PlayFMV( const char* FileName,
                                   PresentationEvent::PresentationEventCallBack* pCallback,
                                   bool IsSkippable,
                                   bool StopMusic,
                                   bool IsLocalized )
{
    // this puts the FMV player in the "loading" state 
    // this is needed so that the gag system can tell when a gag has ended 
    // (without this the state right before and right after the fmc playes is the same
    mpFMVPlayer->PreLoad();

#ifdef RAD_E3
    // no in-game FMV's for E3 build
    //
    if( pCallback != NULL )
    {
        pCallback->OnPresentationEventEnd( NULL );
    }

    return;
#endif

    if( mOverlay )
    {
        //
        // Hide the HUD
        //
        CGuiScreenHud* hud = dynamic_cast< CGuiScreenHud* >( GetGuiSystem()->GetInGameManager()->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_HUD ) );
        rAssert( hud != NULL );
        Scrooby::Screen* screen = hud->GetScroobyScreen();
        screen->SetAlpha( 0.0f );


        mOverlay->SetStart( BLACK_TRANSPARENT );
        mOverlay->SetEnd( BLACK );
        mOverlay->SetDuration( 0.5f );
        RenderLayer* pLayer = GetRenderManager()->mpLayer( RenderEnums::PresentationSlot );
        rAssert( pLayer );
        if( pLayer->IsDead() )
        {
            pLayer->Resurrect();
        }
        pLayer->Thaw();
        pLayer->AddGuts( mOverlay );
    }
    mWaitingOnFade = true;
    FMVEvent* pEvent = 0;
    GetPresentationManager()->QueueFMV( &pEvent, this );
#ifdef RAD_XBOX
    strcpy( pEvent->fileName, "D:\\movies\\" );
#elif RAD_PS2
    strcpy( pEvent->fileName, "movies\\" );
#elif RAD_WIN32
    strcpy( pEvent->fileName, "movies\\" );
#else
    strcpy( pEvent->fileName, "movies/" );
#endif
    strcat( pEvent->fileName, FileName );
    pEvent->SetRenderLayer( RenderEnums::PresentationSlot );
    pEvent->SetAutoPlay( true );

#ifdef PAL
    if( IsLocalized )
    {
        switch( CGuiTextBible::GetCurrentLanguage() )
        {
            case Scrooby::XL_FRENCH:
            {
                pEvent->SetAudioIndex( FMVEvent::AUDIO_INDEX_FRENCH );

                break;
            }
            case Scrooby::XL_GERMAN:
            {
                pEvent->SetAudioIndex( FMVEvent::AUDIO_INDEX_GERMAN );

                break;
            }
            case Scrooby::XL_SPANISH:
            {
                pEvent->SetAudioIndex( FMVEvent::AUDIO_INDEX_SPANISH );

                break;
            }
            default:
            {
                rAssert( CGuiTextBible::GetCurrentLanguage() == Scrooby::XL_ENGLISH );
                pEvent->SetAudioIndex( FMVEvent::AUDIO_INDEX_ENGLISH );

                break;
            }
        }
    }
    else
#endif // PAL
    {
        pEvent->SetAudioIndex( FMVEvent::AUDIO_INDEX_ENGLISH );
    }

#ifdef RAD_GAMECUBE
    pEvent->SetAllocator( GMA_ANYWHERE_IN_LEVEL );
#else
    pEvent->SetAllocator( GMA_LEVEL_HUD );
#endif
    pEvent->SetClearWhenDone( true );
    pEvent->SetKeepLayersFrozen( true );
	pEvent->SetSkippable(IsSkippable);
    if( StopMusic )
    {
        pEvent->KillMusic();
    }
    mpPlayCallback = pCallback;
}
/*=============================================================================
Description:    This is a callback when the event queued by PlayFMV.
=============================================================================*/
void PresentationManager::OnPresentationEventBegin( PresentationEvent* pEvent )
{
    Context* context = GetGameFlow()->GetContext( GetGameFlow()->GetCurrentContext() );
    if( dynamic_cast<GameplayContext*>( context ) )
    {
        static_cast<GameplayContext*>( context )->PauseAllButPresentation( true );
    }
    GetGuiSystem()->HandleMessage( GUI_MSG_RELEASE_INGAME );
    if( mpPlayCallback )
    {
        mpPlayCallback->OnPresentationEventBegin( pEvent );
    }
    RenderLayer* rl = GetRenderManager()->mpLayer( RenderEnums::PresentationSlot );
    rAssert( rl );
    rl->RemoveGuts( mOverlay );
}
/*=============================================================================
Description:    This is a callback when the event queued by PlayFMV.
=============================================================================*/
void PresentationManager::OnPresentationEventLoadComplete( PresentationEvent* pEvent )
{
    if( mpPlayCallback )
    {
        mpPlayCallback->OnPresentationEventLoadComplete( pEvent );
    }
}
/*=============================================================================
Description:    This is a callback when the event queued by PlayFMV.
=============================================================================*/
void PresentationManager::OnPresentationEventEnd( PresentationEvent* pEvent )
{
    HeapMgr()->PushHeap (GMA_LEVEL_HUD);
    GetGuiSystem()->HandleMessage( GUI_MSG_INIT_INGAME );
    HeapMgr()->PopHeap (GMA_LEVEL_HUD);
    GetLoadingManager()->AddCallback( this );
    if( mOverlay )
    {
        mOverlay->SetStart( BLACK_TRANSPARENT );
        mOverlay->SetEnd( BLACK );
        mOverlay->SetDuration( 0.5f );
        //mOverlay->SetRemoveOnComplete( true );
        RenderLayer* pLayer = GetRenderManager()->mpLayer( RenderEnums::PresentationSlot );
        rAssert( pLayer );
        pLayer->AddGuts( mOverlay );
    }
}
/*=============================================================================
Description:    
=============================================================================*/
void PresentationManager::OnProcessRequestsComplete( void* pUserData )
{
    Context* context = GetGameFlow()->GetContext( GetGameFlow()->GetCurrentContext() );
    if( dynamic_cast<GameplayContext*>( context ) )
    {
        static_cast<GameplayContext*>( context )->PauseAllButPresentation( false );
    }
    GetRenderManager()->ThawFromPresentation();
    GetGuiSystem()->HandleMessage( GUI_MSG_RUN_INGAME );
    if( mpPlayCallback )
    {
        mpPlayCallback->OnPresentationEventEnd( 0 );
    }
    mpPlayCallback = 0;
    if( mOverlay )
    {
        mOverlay->SetEnd( BLACK );
        mOverlay->SetFrames( 10 );
        mOverlay->SetRemoveOnComplete( true );
        RenderLayer* pLayer = GetRenderManager()->mpLayer( RenderEnums::PresentationSlot );
        rAssert( pLayer );
        pLayer->AddGuts( mOverlay );
    }
}
//=============================================================================
// PresentationManager::QueueFMV
//=============================================================================
// Description: Comment
//
// Parameters:  ( FMVEvent** pFMVEvent, PresentationEvent::PresentationEventCallBack* pCallback )
//
// Return:      void 
//
//=============================================================================
void PresentationManager::QueueFMV( FMVEvent** pFMVEvent, PresentationEvent::PresentationEventCallBack* pCallback )
{
    (*pFMVEvent) = mFMVPool->AllocateFromPool();
    (*pFMVEvent)->Init();
	(*pFMVEvent)->SetAllocator(GMA_LEVEL_MOVIE);
    (*pFMVEvent)->pCallback = pCallback;

    AddToQueue( *pFMVEvent );
}

//=============================================================================
// PresentationManager::QueueNIS
//=============================================================================
// Description: Comment
//
// Parameters:  ( NISEvent** pNISEvent, PresentationEvent::PresentationEventCallBack* pCallback )
//
// Return:      void 
//
//=============================================================================
void PresentationManager::QueueNIS( NISEvent** pNISEvent, PresentationEvent::PresentationEventCallBack* pCallback )
{
    (*pNISEvent) = mNISPool->AllocateFromPool();
    (*pNISEvent)->Init();

    (*pNISEvent)->pCallback = pCallback;

    AddToQueue( *pNISEvent );
}

//=============================================================================
// PresentationManager::QueueTransition
//=============================================================================
// Description: Comment
//
// Parameters:  ( TransitionEvent** pTransitionEvent, 
//                PresentationEvent::PresentationEventCallBack* pCallback )
//
// Return:       void 
//
//=============================================================================
void PresentationManager::QueueTransition( TransitionEvent** pTransitionEvent, 
                                           PresentationEvent::PresentationEventCallBack* pCallback )
{
    (*pTransitionEvent) = mTransitionPool->AllocateFromPool();
    (*pTransitionEvent)->Init();

    (*pTransitionEvent)->pCallback = pCallback;

    AddToQueue( *pTransitionEvent );
}

bool PresentationManager::IsBusy(void) const
{
	 return (mpCurrent != 0) || mWaitingOnFade;
}

//=============================================================================
// PresentationManager::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void PresentationManager::Update( unsigned int elapsedTime )
{
    GetTutorialManager()->Update( static_cast< float >( elapsedTime ) );

    if( mp_PCAnimator != NULL )
    {
        mp_PCAnimator->Update( static_cast<int>( elapsedTime ));
    }

    if( mp_NPCAnimator != NULL )
    {
        mp_NPCAnimator->Update( static_cast<int>( elapsedTime ));
    }

    rAssert( mOverlay != NULL );
    mOverlay->Update( elapsedTime );

    if( mWaitingOnFade )
    {
        if( mOverlay->GetAlpha() == 0.0f )
        {
            // get one extra frame of full black.
            mWaitingOnFade = false;
        }
        return;
    }
    if( mpCurrent == NULL )
    {
        mpCurrent = GetFirst();
        
        if (mpCurrent != NULL )
        {
            if( mpCurrent->pCallback != NULL )
            {
                mpCurrent->pCallback->OnPresentationEventBegin( mpCurrent );
            }
            mpPlayerDrawable->SetPlayer( mpCurrent->GetPlayer() );
            mpPlayerDrawable->SetRenderLayer( mpCurrent->GetRenderLayer() );

            mpCurrent->Start();
            mWaitingOnFade = false;
        }
    }

    if( mpCurrent != NULL )
    {
        bool finished = !mpCurrent->Update( elapsedTime );

        if( finished )
        {
            if( mpCurrent->pCallback != NULL )
            {
                mpCurrent->pCallback->OnPresentationEventEnd( mpCurrent );
            }
            mpCurrent->Stop();
            mpPlayerDrawable->SetPlayer( NULL );

            ReturnToPool( mpCurrent );

            mpCurrent = NULL;
        }
    }
}

//=============================================================================
// PresentationManager::ClearQueue
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PresentationManager::ClearQueue()
{
    if( mpCurrent == NULL )
    {
        mpCurrent = GetFirst();
    }

    while ( mpCurrent != NULL )
    {
        if( mpCurrent->pCallback != NULL )
        {
            mpCurrent->pCallback->OnPresentationEventEnd( mpCurrent );
        }

		mpCurrent->Stop();
		
        ReturnToPool( mpCurrent );

        mpCurrent = GetFirst();
    };

    mpPlayerDrawable->SetPlayer( NULL );
}

//=============================================================================
// PresentationManager::GetCameraTargetForLineOfDialog
//=============================================================================
// Description: what should the camera look at for this line of dialog
//
// Parameters:  lineOfDialog - which line are we on?
//
// Return:      CameraTarget - PC, NPC, or Don't Care
//
//=============================================================================
const tName PresentationManager::GetCameraTargetForLineOfDialog( const unsigned int lineOfDialog ) const
{
    size_t size = mCameraForLineOfDialog.size();
    if( lineOfDialog >= size )
    {
        return tName( "NONE" );
    }
    else
    {
        const tName& returnMe = mCameraForLineOfDialog[ lineOfDialog ];
        return returnMe;
    }
}
//=============================================================================
// PresentationManager::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void PresentationManager::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
    case EVENT_GUI_FADE_OUT_DONE:
        {
            if( mWaitingOnFade )
            {
                mWaitingOnFade = false;
            }
        }
        break;
    case EVENT_LOCATOR:
        {
/*
            EventLocator* locator = (EventLocator*)pEventData;
            if( locator->GetFlag(Locator::ACTIVE) )
            {
                if( strcmp( locator->GetTriggerVolume()->GetName(), "TestTrigger" ) == 0 )
                {
                    NISEvent* pEvent;
                    QueueNIS( &pEvent, NULL );
                    pEvent->fileName = "art\\camtest.p3d";
                    pEvent->type = NISEvent::NIS_CAMERA;
                    strcpy( pEvent->camera, "cameraShape1" );
                    strcpy( pEvent->animation, "CAM_cameraShape1" );
                    strcpy( pEvent->controller, "MasterController" );
                    pEvent->LoadFromInventory();
                }
            }
*/
            break;
        }

    case EVENT_CONVERSATION_INIT:
        {  
            DialogEventData* p_dialogstruct = static_cast <DialogEventData*> (pEventData);

            //
            // Register the character and the NPC with the conversation camera
            //
            SuperCam* camera = GetSuperCamManager()->GetSCC( 0 )->GetSuperCam( SuperCam::CONVERSATION_CAM );
            ConversationCam* convCam = dynamic_cast< ConversationCam* >( camera );
            convCam->SetCharacter( 0, p_dialogstruct->char1 );
            convCam->SetCharacter( 1, p_dialogstruct->char2 );
            
            rAssert(p_dialogstruct);
            rAssert( p_dialogstruct->char1 != NULL );
            rAssert( p_dialogstruct->char2 != NULL );

            //
            // some conversations are special - race missions for example 
            // require different bitmaps 
            //
            Character* char1 = p_dialogstruct->char1;
            Character* char2 = p_dialogstruct->char2;
            Character* npc = NULL;
            Character* pc = NULL;

            if( char1->IsNPC() )
            {
                npc = char1;
                pc = char2;
            }
            else
            {
                rAssert( char2->IsNPC() );
                npc = char2;
                pc = char1;
            }

            mp_PCAnimator->SetCharacter(pc);
            mp_NPCAnimator->SetCharacter(npc);

            if(pc->GetStateManager()->GetState() == CharacterAi::INCAR)
            {
                GetAvatarManager()->PutCharacterOnGround(pc, pc->GetTargetVehicle());
            }

            //
            // Make the characters face one another
            //
            
            // If we're talking to patty and selma, we need to make selma face the user
            GameplayManager* gpm = GetGameplayManager();
            Mission* mission = gpm->GetCurrentMission();
            bool pattyAndSelma = mission->GetCurrentStage()->GetObjective()->IsPattyAndSelmaDialog();
            if( pattyAndSelma )
            {
                CharacterManager* cm = GetCharacterManager();
                Character* selma = cm->GetCharacterByName( "selma" );
                if(selma == NULL && GetGameplayManager()->GetCurrentLevelIndex() == RenderEnums::L7)
                {
                    selma = cm->GetMissionCharacter("zmale1");
                    rTuneAssert(selma != NULL);
                }
                MakeCharactersFaceEachOther( selma, pc );

            }
            MakeCharactersFaceEachOther( char1, char2 );

            mDialogLineNumber = -1;
            mOldCamIndexNum= GetSuperCamManager()->GetSCC(PLAYER_ONE)->GetActiveSuperCamIndex();
            
            // don't swap to close up camera if we are using a static cam (generally in an interior, 
            // where the camera movement could screw us up)
            if(GetSuperCamManager()->GetSCC(PLAYER_ONE)->AllowCameraToggle())
            {
                if ( GetGameplayManager()->GetCurrentMission()->DialogueCharactersTeleported() )
                {
                    GetSuperCamManager()->GetSCC(PLAYER_ONE)->SelectSuperCam(SuperCam::CONVERSATION_CAM, SuperCamCentral::CUT, 0 );
                }
                else
                {
                    GetSuperCamManager()->GetSCC(PLAYER_ONE)->SelectSuperCam(SuperCam::CONVERSATION_CAM, 0 );
                }
                GetSuperCamManager()->GetSCC(PLAYER_ONE)->AddTarget( mp_NPCAnimator->GetCharacter()->GetTarget() );
            }
            
            //TO DO call Character eye blinking
            mInConversation =true;


            // make the talk-to NPC stand still 
            rAssert( npc );
            static_cast<NPCController*>(npc->GetController())->TransitToState( NPCController::TALKING_WITH_PLAYER );

            tName name = npc->GetNameObject();
            ReplaceMissionBriefingBitmap( name );
            break;
        }

    case EVENT_PC_TALK:
        {
            if( InConversation() )
            {
                ++mDialogLineNumber;
                tName target = GetCameraTargetForLineOfDialog( mDialogLineNumber );
                if( target == tName( "NONE" ) )
                {
                    ConversationCam::UsePcCam();
                }
                else
                {
                    ConversationCam::SetCameraByName( target );
                }
                mp_PCAnimator->PlaySpecialAmbientAnimation();
                mp_NPCAnimator->PlaySpecialAmbientAnimation();
            }
            else
            {
                Avatar* av = GetAvatarManager()->GetAvatarForPlayer( 0 );
                Character* character = av->GetCharacter();
                mp_PCAnimator->SetCharacter( character );

            }
            mp_PCAnimator->StartTalking();
            break;
        }
    case EVENT_PC_SHUTUP:
        {
            mp_PCAnimator->StopTalking();
            break;
        }
    case EVENT_NPC_TALK:
        {
            if( InConversation() )
            {
                ++mDialogLineNumber;
                const tName& target = GetCameraTargetForLineOfDialog( mDialogLineNumber );
                if( target == tName( "NONE" ) )
                {
                    ConversationCam::UseNpcCam();
                }
                else
                {
                    ConversationCam::SetCameraByName( target );
                }
                mp_NPCAnimator->PlaySpecialAmbientAnimation();
                mp_PCAnimator->PlaySpecialAmbientAnimation();
            }
            else
            {
                Character* character = reinterpret_cast< Character* >( pEventData );
                mp_NPCAnimator->SetCharacter( character );
            }
            mp_NPCAnimator->StartTalking();
            break;
        }
    case EVENT_NPC_SHUTUP:
        {
            mp_NPCAnimator->StopTalking();
            if( !InConversation() )
            {
                mp_NPCAnimator->SetCharacter( NULL );
            }
            break;
        }
        
    case EVENT_CONVERSATION_SKIP:
        {
            //
            // Eventually this will take us to CONVERSATION_DONE
            //
            mInConversation = false;
            break;
        }
    case EVENT_CONVERSATION_DONE:
        {
            //change camera to previous
            
            if ( GetGameplayManager()->GetCurrentMission()->DialogueCharactersTeleported() )
            {
                GetSuperCamManager()->GetSCC(PLAYER_ONE)->SelectSuperCam(mOldCamIndexNum, SuperCamCentral::CUT, 0);            
            }
            else
            {
                GetSuperCamManager()->GetSCC(PLAYER_ONE)->SelectSuperCam(mOldCamIndexNum, 0);            
            }
           
            // resume walking... 
            Character* npc = mp_NPCAnimator->GetCharacter();
            if( npc )
            {
                static_cast<NPCController*>(npc->GetController())->TransitToState( NPCController::STOPPED );
            }

            //TO DO stop eyeblinking.

            mInConversation = false;
            if ( mp_PCAnimator->GetCharacter() != NULL)
            {
                mp_PCAnimator->StopTalking();
                mp_PCAnimator->SetCharacter(NULL);
            }

            if ( mp_NPCAnimator->GetCharacter() != NULL)
            {
                mp_NPCAnimator->StopTalking();
                mp_NPCAnimator->SetCharacter(NULL);
            }            
    
           
            
            break;
        }
    default:
        {
            // don't be lazy!  handle the event!
            rAssert( false );
        }
    }
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// PresentationManager::AddToQueue
//=============================================================================
// Description: Comment
//
// Parameters:  ( PresentationEvent* pEvent )
//
// Return:      void 
//
//=============================================================================
void PresentationManager::AddToQueue( PresentationEvent* pEvent )
{
    // if you hit this assert then chances are the presentation queue is full
    rAssert( mFIFOEnd < MAX_EVENT_SIZE );
    rAssert( mEventFIFO[ mFIFOEnd ] == NULL );

    mEventFIFO[ mFIFOEnd ] = pEvent;

    mFIFOEnd++;
    if( mFIFOEnd >= MAX_EVENT_SIZE )
    {
        mFIFOEnd = 0;
    }
}

//=============================================================================
// PresentationManager::ReturnToPool
//=============================================================================
// Description: Comment
//
// Parameters:  ( PresentationEvent* presevent )
//
// Return:      void 
//
//=============================================================================
void PresentationManager::ReturnToPool( PresentationEvent* presevent )
{
    // change to use appropriate pool without doing it the dumb way
    mFMVPool->ReturnToPool( (unsigned int)presevent );
    mNISPool->ReturnToPool( (unsigned int)presevent );
    mTransitionPool->ReturnToPool( (unsigned int)presevent );
}

//=============================================================================
// PresentationManager::GetFirst
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      PresentationEvent
//
//=============================================================================
PresentationEvent* PresentationManager::GetFirst()
{
    rAssert( mFIFOBegin < MAX_EVENT_SIZE );

    if( mEventFIFO[ mFIFOBegin ] != NULL )
    {
        PresentationEvent* pEvent = mEventFIFO[ mFIFOBegin ];

        mEventFIFO[ mFIFOBegin ] = NULL;

        mFIFOBegin++;
        if( mFIFOBegin >= MAX_EVENT_SIZE )
        {
            mFIFOBegin = 0;
        }

        return( pEvent );
    }
    else
    {
        return( NULL );
    }
}


//Do stuff once we are in the gameplay context
void PresentationManager::OnGameplayStart()
{
    //get ptr to the Players Avatar
    mp_PCAnimator->SetCharacter(GetCharacterManager()->GetCharacter(0) ); 
}

void PresentationManager::OnGameplayStop()
{
}

//=============================================================================
// PresentationManager::SetCamerasForLineOfDialog
//=============================================================================
// Description: sets the cameras for specific lines of dialog
//
// Parameters:  names - a vector of names that represnet cameras for lines of
//              dialog
//
// Return:      NONE
//
//=============================================================================
void PresentationManager::SetCamerasForLineOfDialog( const TNAMEVECTOR& names )
{
    #ifdef RAD_DEBUG
        size_t size = names.size();
        size_t i;
        for( i = 0; i < size; ++i )
        {
            tName name = names[ i ];
        }
    #endif
    mCameraForLineOfDialog.erase(  mCameraForLineOfDialog.begin(), mCameraForLineOfDialog.end() );
    mCameraForLineOfDialog.insert( mCameraForLineOfDialog.begin(), names.begin(), names.end() );
}

//=============================================================================
// PresentationManager::InConversation()
//=============================================================================
// Description: determines if the game is in a conversation or not
//
// Parameters:  NONE
//
// Return:      bool - are we in a conversation or not?
//
//=============================================================================
bool PresentationManager::InConversation() const
{
    return mInConversation;
}

//=============================================================================
// PresentationManager::StopAll
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PresentationManager::StopAll()
{
#ifdef RAD_WIN32
    mpFMVPlayer->ForceStop();
#else
    mpFMVPlayer->Stop();
#endif
    mpNISPlayer->Stop();
    mpTransitionPlayer->Stop();
    mpCameraPlayer->Stop();
}

PresentationOverlay::PresentationOverlay() :
mAlpha( 0.0f ),
mInvDuration( 0.0f ),
mStart( BLACK_TRANSPARENT ),
mEnd( BLACK_TRANSPARENT ),
mFrameCount( -1 ),
mIsAutoRemove( false )
{}

void PresentationOverlay::Update( unsigned int ElapsedTime )
{
    if( mFrameCount == 0 )
    {
        mAlpha = 0.0f;
        mInvDuration = 0.0f;
        mFrameCount = -1;
        if( mIsAutoRemove )
        {
            RenderLayer* rl = GetRenderManager()->mpLayer( RenderEnums::PresentationSlot );
            rAssert( rl );
            rl->RemoveGuts( this );
            rl->Freeze();
            mIsAutoRemove = false;
        }
    }
    else if( mAlpha > 0.0f )
    {
        mAlpha -= ( (float)ElapsedTime * mInvDuration );
        if( mAlpha < 0.0f )
        {
            mAlpha = 0.0f;
            mInvDuration = 0.0f;
            if( mIsAutoRemove )
            {
                RenderLayer* rl = GetRenderManager()->mpLayer( RenderEnums::PresentationSlot );
                rAssert( rl );
                rl->RemoveGuts( this );
                rl->Freeze();
                mIsAutoRemove = false;
            }
        }
    }
}

void PresentationOverlay::Display( void )
{
    // First check if anything is visible...
    int alpha = 0;
    if( mFrameCount == -1 )
    {
        alpha = int( ( mStart.Alpha() * mAlpha ) + ( mEnd.Alpha() * ( 1.0f - mAlpha ) ) );
    }
    else
    {
        --mFrameCount;
        if( mFrameCount < 0 )
        {
            mFrameCount = 0;
        }
        alpha = mEnd.Alpha();
    }
    if( alpha <= 0 )
    {
        return;
    }
    // Now figure out the colour.
    tColour c;
    if( mFrameCount == -1 )
    {
        int red   = int( ( mStart.Red()   * mAlpha ) + ( mEnd.Red()   * ( 1.0f - mAlpha ) ) );
        int green = int( ( mStart.Green() * mAlpha ) + ( mEnd.Green() * ( 1.0f - mAlpha ) ) );
        int blue  = int( ( mStart.Blue()  * mAlpha ) + ( mEnd.Blue()  * ( 1.0f - mAlpha ) ) );
        c.Set( red, green, blue, alpha );
    }
    else
    {
        c.c = mEnd.c;
    }

    // Let's draw poly!
    p3d::stack->Push();
    bool oldZWrite = p3d::pddi->GetZWrite();
    pddiCompareMode oldZComp = p3d::pddi->GetZCompare();
    if( oldZWrite )
    {
        p3d::pddi->SetZWrite( false );
    }
    if( oldZComp != PDDI_COMPARE_ALWAYS )
    {
        p3d::pddi->SetZCompare( PDDI_COMPARE_ALWAYS );
    }
    p3d::stack->LoadIdentity();
    p3d::pddi->SetProjectionMode( PDDI_PROJECTION_ORTHOGRAPHIC );
    pddiColour oldAmbient = p3d::pddi->GetAmbientLight();
    p3d::pddi->SetAmbientLight( pddiColour( 255, 255, 255 ) );

    pddiPrimStream* overlay = 0;

    pddiShader* overlayShader = BootupContext::GetInstance()->GetSharedShader();
    rAssert( overlayShader );

    overlayShader->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ALPHA );
    overlayShader->SetInt( PDDI_SP_ISLIT, 0 );
    overlayShader->SetInt( PDDI_SP_SHADEMODE, PDDI_SHADE_FLAT );
    overlayShader->SetInt( PDDI_SP_TWOSIDED, 1  );

    overlay = p3d::pddi->BeginPrims( overlayShader, PDDI_PRIM_TRISTRIP, PDDI_V_C, 4 );

    overlay->Colour( c );
    overlay->Coord( 0.5f, -0.5f, 1.0f );
    overlay->Colour( c );
    overlay->Coord(  -0.5f, -0.5f, 1.0f );
    overlay->Colour( c );
    overlay->Coord( 0.5f,  0.5f, 1.0f );
    overlay->Colour( c );
    overlay->Coord( -0.5f,  0.5f, 1.0f );
    
    p3d::pddi->EndPrims( overlay );
    p3d::pddi->SetProjectionMode(PDDI_PROJECTION_PERSPECTIVE);
    p3d::pddi->SetAmbientLight( oldAmbient );
    if( oldZWrite )
    {
        p3d::pddi->SetZWrite( true );
    }
    if( oldZComp != PDDI_COMPARE_ALWAYS )
    {
       	p3d::pddi->SetZCompare( oldZComp );
    }
	p3d::stack->Pop();
}

//=============================================================================
// PresentationManager::CheckRaceMissionBitmaps
//=============================================================================
// Description: updates bitmaps on the mission start screen if we're entering
//              a race mission
//
// Parameters:  none7
//
// Return:      void 
//
//=============================================================================
void PresentationManager::CheckRaceMissionBitmaps()
{
}

//=============================================================================
// PresentationManager::ReplaceMissionBriefingBitmap
//=============================================================================
// Description: depending on who we're talking to, we need to replace the 
//              picture on the mission briefing screen
//
// Parameters:  conversationCharacterName - name of the character that led to 
//              this screen
//
// Return:      void 
//
//=============================================================================
void PresentationManager::ReplaceMissionBriefingBitmap( const tName& conversationCharacterName )
{
    const tName& name = conversationCharacterName;

    //
    // These are for the race missions
    //
    if( name == "b_nelson" )
    {
        CGuiScreenMissionLoad::SetBitmapName( "art/frontend/dynaload/images/misXX_CT.p3d" );
        CGuiScreenMissionLoad::ClearBitmap();
    }
    else if( name == "b_milhouse" )
    {
        CGuiScreenMissionLoad::SetBitmapName( "art/frontend/dynaload/images/misXX_TT.p3d" );
        CGuiScreenMissionLoad::ClearBitmap();
    }
    else if( name == "b_ralph" )
    {
        CGuiScreenMissionLoad::SetBitmapName( "art/frontend/dynaload/images/misXX_CP.p3d" );
        CGuiScreenMissionLoad::ClearBitmap();
    }
    else if( name == "b_louie" )
    {
        CGuiScreenMissionLoad::SetBitmapName( "art/frontend/dynaload/images/misXX_GB.p3d" );
        CGuiScreenMissionLoad::ClearBitmap();
    }
    else if( name == "b_witch" )
    {
        CGuiScreenMissionLoad::SetBitmapName( "art/frontend/dynaload/images/misXX_HW.p3d" );
        CGuiScreenMissionLoad::ClearBitmap();
    }
    else if( name == "b_zfem1" )
    {
        CGuiScreenMissionLoad::SetBitmapName( "art/frontend/dynaload/images/misXX_HW.p3d" );
        CGuiScreenMissionLoad::ClearBitmap();
    }
    else if( name == "b_zmale1" )
    {
        CGuiScreenMissionLoad::SetBitmapName( "art/frontend/dynaload/images/misXX_HW.p3d" );
        CGuiScreenMissionLoad::ClearBitmap();
    }
    else if( name == "b_zmale3" )
    {
        CGuiScreenMissionLoad::SetBitmapName( "art/frontend/dynaload/images/misXX_HW.p3d" );
        CGuiScreenMissionLoad::ClearBitmap();
    }

    //
    // these are for the bonus missions
    //
    else if( name == "b_cletus")
    {
        CGuiScreenMissionLoad::SetBitmapName( "art/frontend/dynaload/images/mis01_08.p3d" );
        CGuiScreenMissionLoad::ClearBitmap();
    }
    else if( name == "b_grandpa")
    {
        CGuiScreenMissionLoad::SetBitmapName( "art/frontend/dynaload/images/mis02_08.p3d" );
        CGuiScreenMissionLoad::ClearBitmap();
    }
    else if( name == "b_skinner")
    {
        CGuiScreenMissionLoad::SetBitmapName( "art/frontend/dynaload/images/mis03_08.p3d" );
        CGuiScreenMissionLoad::ClearBitmap();
    }
    else if( name == "b_cbg")
    {
        CGuiScreenMissionLoad::SetBitmapName( "art/frontend/dynaload/images/mis04_08.p3d" );
        CGuiScreenMissionLoad::ClearBitmap();
    }
    else if( name == "b_frink")
    {
        CGuiScreenMissionLoad::SetBitmapName( "art/frontend/dynaload/images/mis05_08.p3d" );
        CGuiScreenMissionLoad::ClearBitmap();
    }
    else if( name == "b_snake")
    {
        CGuiScreenMissionLoad::SetBitmapName( "art/frontend/dynaload/images/mis06_08.p3d" );
        CGuiScreenMissionLoad::ClearBitmap();
    }
    else if( name == "b_smithers")
    {
        CGuiScreenMissionLoad::SetBitmapName( "art/frontend/dynaload/images/mis07_08.p3d" );
        CGuiScreenMissionLoad::ClearBitmap();
    }
    else
    {
        return;
    }

    //
    // Trigger an update of the picture via scrooby
    //
    CGuiScreenMissionLoad::ReplaceBitmap();
}

//=============================================================================
// PresentationManager::MakeCharactersFaceEachOther
//=============================================================================
// Description: depending on who we're talking to, we need to replace the 
//              picture on the mission briefing screen
//
// Parameters:  c1, c2 - the two characters that we want to face each other
//
// Return:      void 
//
//=============================================================================
void PresentationManager::MakeCharactersFaceEachOther( Character* c0, Character* c1 )
{
    rmt::Vector position0;
    rmt::Vector position1;
    
    c0->GetPosition( &position0 );
    c1->GetPosition( &position1 );
    rmt::Vector offset = position1 - position0;
    offset.Normalize();
    float rotation = choreo::GetWorldAngle( offset.x, offset.z );
    c0->RelocateAndReset( position0, rotation,           false );
    c1->RelocateAndReset( position1, rotation + rmt::PI, false );
}
