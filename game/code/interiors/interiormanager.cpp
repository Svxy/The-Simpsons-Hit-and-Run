//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        interiormanager.cpp
//
// Description: Implementation for the InteriorManager class.
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

//========================================
// System Includes
//========================================
// System
#include <stdlib.h> // for rand()
// Ftech
#include <raddebug.hpp>
#include <choreo/utility.hpp>
#include <p3d/light.hpp>
#include <p3d/matrixstack.hpp>
#include <p3d/view.hpp>
#include <simcommon/simstatearticulated.hpp>

//========================================
// Project Includes
//========================================
#include <interiors/interiormanager.h>

#include <ai/sequencer/actioncontroller.h>
#include <ai/sequencer/action.h>
#include <ai/actionbuttonhandler.h>
#include <camera/supercammanager.h>
#include <console/console.h>
#include <events/eventdata.h>
#include <events/eventmanager.h>
#include <main/game.h>
#include <memory/srrmemory.h>
#include <meta/directionallocator.h>
#include <meta/interiorentrancelocator.h>
#include <meta/triggervolumetracker.h>
#include <presentation/nisplayer.h>
#include <presentation/fmvplayer/fmvplayer.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/ingame/guiscreenletterbox.h>
#include <presentation/presentation.h>
#include <render/rendermanager/renderlayer.h>
#include <render/rendermanager/rendermanager.h>
#include <render/RenderManager/WorldRenderLayer.h>
#include <sound/soundmanager.h>
#include <worldsim/avatar.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/character/character.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/worldphysicsmanager.h>
#include <worldsim/traffic/trafficmanager.h>
#include <meta/locatorevents.h>
#include <meta/eventlocator.h>
#include <meta/spheretriggervolume.h>
#include <meta/triggervolumetracker.h>
#include <cards/cardgallery.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/missionmanager.h>
#include <mission/gameplaymanager.h>
#include <mission/animatedicon.h>
#include <worldsim/coins/sparkle.h>
//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
InteriorManager* InteriorManager::spInstance = NULL;
rmt::Randomizer InteriorManager::sRandom(0);
bool InteriorManager::sRandomSeeded = false;
unsigned InteriorManager::sPersistGagIndex = 0;
extern bool cGuiManagerInGameActive;

const int PLAYER_ONE = 0;

// wow, what a hack

struct InteriorCentre
{
    tName name;
    rmt::Vector centre;
    int level;
};

static InteriorCentre s_InteriorCentres[8];

// another hack
static const int NUM_OPAQUES = 1;
static const char sOpaques[NUM_OPAQUES][16] = {
    "gag_buly.p3d"
};

InteriorManager::GagBinding::GagBinding()
{
    Clear();
}

void InteriorManager::GagBinding::Clear(void)
{
    interiorUID = 0;
    weight = 1;
    random = true;

    gagFileName[0] = 0; 
    cycleMode = DEFAULT_CYCLE_MODE;
    triggered = false;
    action = false;
    retrigger = false;
    useGagLocator = false;
    gagLoc = 0;
    gagPos.Set(0.0f,0.0f,0.0f);
    useTriggerLocator = false;
    triggerLoc = 0;
    triggerPos.Set(0.0f,0.0f,0.0f);
    soundID = 0;
    gagFMVFileName[ 0 ] = 0;
    loopIntro = 0;
    loopOutro = 0;
    cameraShake = false;
    shakeDelay = 0.0f;
    shakeDuration = 0.0f;
    shake.playerID = 0;
    shake.direction.Set(0.0f,1.0f,0.0f);
    shake.looping = false;
    coinDelay = 0.0f;
    coins = 0;
    sparkle = false;
    animBV = false;
    loadDist = 100;
    unloadDist = 150;
    soundLoadDist = 10;
    soundUnloadDist = 20;
    i_S_Movie = 0;
    persistIndex = -1;
    dialogChar1[0] = '\0';
    dialogChar2[0] = '\0';
    acceptDialogID = 0;
    rejectDialogID = 0;
    instructDialogID = 0;
}

class Gag;

class GagDrawable : public DynaPhysDSG
{
    public:
        GagDrawable(Gag* g);

        virtual ~GagDrawable() {    mpDrawstuff->Release(); };

//        void DisplayBoundingBox(tColour colour = tColour(0,255,0));
//        void DisplayBoundingSphere(tColour colour = tColour(0,255,0));

        void GetBoundingBox(rmt::Box3D* box)
        {
            mpDrawstuff->GetBoundingBox(box);
            if(useTranslate)
            {
                box->high += mPosn;
                box->low += mPosn;
            }
        }

        void GetBoundingSphere(rmt::Sphere* sphere)
        {
            mpDrawstuff->GetBoundingSphere(sphere);
            if(useTranslate)
            {
                sphere->centre += mPosn;
            }
//            sphere->radius += 10.0f;
        }

        virtual void Display();
        void SetPosition(bool u, const rmt::Vector& v) 
        { 
            mPosn = v; 
            useTranslate = u; 

            if(useTranslate && mpSimStateObj)
            {
                rmt::Matrix matrix;
                matrix.Identity();
                matrix.FillTranslate(mPosn);
                mpSimStateObj->SetTransform(matrix); 
                if(mPose)
                {
                    mPose->Assign(((tCompositeDrawable*)mpDrawstuff)->GetPose());
                    mPose->GetJoint(0)->SetWorldTranslation(mPose->GetJoint(0)->GetWorldTranslation() + mPosn);
                }
                mpSimStateObj->GetCollisionObject()->SetHasRelocated( true );
                mpSimStateObj->GetCollisionObject()->SetHasMoved( true );
                mpSimStateObj->GetCollisionObject()->Update();
            }
        }

        float sparkleStrength;

        void SimUpdate(float timeins)
        {
            if(mPose)
            {
                mPose->Assign(((tCompositeDrawable*)mpDrawstuff)->GetPose());
                mPose->GetJoint(0)->SetObjectTranslation(mPose->GetJoint(0)->GetObjectTranslation() + mPosn);
                mpSimStateObj->GetCollisionObject()->SetHasMoved(true);
                mpSimStateObj->GetCollisionObject()->Update();
            }
        }

        void ApplyForce( const rmt::Vector& direction, float force )
        {
            // no force
        }

private:
        tDrawable* mpDrawstuff;
        Gag* gag;
        bool useTranslate;
        poser::Pose* mPose;

private:
        //Prevent wasteful constructor creation.
        GagDrawable( const GagDrawable& pGagDrawable );
        GagDrawable& operator=( const GagDrawable& pGagDrawable );
};

class Gag : public ActionButton::ButtonHandler, public AnimationPlayer::LoadDataCallBack, public EventListener, public NISSoundPlaybackCompleteCallback
{
public:
    void Render(void)
    {
        gagPlayer->Render();
    }

    void Load(void)
    {
        char buffy[256];
        sprintf( buffy, "art\\nis\\gags\\%s", binding->gagFileName );
        gagPlayer->LoadData( buffy, this, false, 0 );

        mLoading = true;
    }

    void SoundLoad(void)
    {
        if( binding->soundID != 0 )
        {
            GetSoundManager()->LoadNISSound( binding->soundID );
            mSoundLoaded = true;
        }
    }

    void Unload(void)
    {
        if(draw)
        {
            ((WorldRenderLayer*)GetRenderManager()->mpLayer(RenderEnums::LevelSlot))->pWorldScene()->Remove(draw);
        }

        gagPlayer->Stop();
        gagPlayer->ClearData();
        
        if(trigger)
        {
            GetTriggerVolumeTracker()->RemoveTrigger(trigger);
            trigger->Release();
            trigger = NULL;
        }

        if(locator)
        {
            locator->Release();
            locator = NULL;
        }

        if(draw)
        {
            draw->Release();
            draw = NULL;
        }

        if(collObj)
        {
            collObj->Release();
            collObj = NULL;
        }

        mLoaded = false;
    }

    void SoundUnload(void)
    {
        if( binding->soundID != 0 )
        {
            GetSoundManager()->StopAndDumpNISSound( binding->soundID );
            mSoundLoaded = false;
        }
    }

    // Used when the dialog finishes.
    void HandleEvent(EventEnum id, void* pEventData)
    {
        //Wait for the dialogue system to announce that the dialogue is finished
        //playing. Right now we only listen to one type of event so we don't need
        //to check the id.
        GetInteriorManager()->SetISMovieDialogPlaying( false );
        CGuiScreenLetterBox::ForceOpen();
        gagPlayer->Play();
        GetEventManager()->RemoveListener(this, EVENT_CONVERSATION_DONE);
        TrafficManager::GetInstance()->RemoveCharacterToStopFor(mDialogEventData.char1);
        TrafficManager::GetInstance()->RemoveCharacterToStopFor(mDialogEventData.char2);
        // Display message for the player that they should go to the Aztec theature.
        if(GetCharacterSheetManager()->IsState(1))
        {
            GetGuiSystem()->HandleMessage( GUI_MSG_INGAME_DISPLAY_PROMPT, 10 );
        }
    }

    void NISSoundPlaybackComplete()
    {
        m_isNISSoundComplete = true;
    }

    void Update( unsigned int elapsedTime )
    {
        if(mLoading)
        {
            return;
        }

        rmt::Vector charPos;
        rmt::Vector gagPos = binding->gagPos;
        GetCharacterManager()->GetCharacter(0)->GetPosition(charPos);
        charPos.Sub(gagPos);

        float dist = rmt::Abs(charPos.Magnitude());

        if(mSoundLoaded && (dist > binding->soundUnloadDist))
        {
            SoundUnload();
        }

        if(!mSoundLoaded && (dist < binding->soundLoadDist))
        {
            SoundLoad();
        }

        // only do distance based loading for non interior gags, interior ones are always loaded
        if( binding->interiorUID == static_cast< tUID >( 0 ) )
        {
            if(mLoaded && (dist > binding->unloadDist)) 
            {
                Unload();
            }

            if(!mLoaded && (dist < binding->loadDist)) 
            {
                Load();
            }
        }

        if(draw)
        {
            if(binding->sparkle)
            {
                draw->sparkleStrength = 1.0f - rmt::Clamp((dist - 10.0f) * 0.1f, 0.0f, 1.0f);
            }
            else
            {
                draw->sparkleStrength = 0.0f;
            }
        }

        if(!mLoaded)
        {
            return;
        }

        bool finished = (binding->gagFMVFileName[ 0 ] == 0) ? gagPlayer->IsFinished() : !GetPresentationManager()->GetFMVPlayer()->IsPlaying();
        finished = finished && m_isNISSoundComplete;

        if(finished && playing)
        {
            playing = false;
            GetEventManager()->TriggerEvent(EVENT_GAG_END, (void*)binding);
            if(binding->coinDelay < 0.0f)
            {
                rmt::Vector pos;
                draw->GetPosition(&pos);
                GetCoinManager()->SpawnInstantCoins(binding->coins, pos);
            }
        }

        if(binding->retrigger)
        {
			if(!((binding->i_S_Movie == 1) && (!GetMissionManager()->IsSundayDrive())))
			{
				if(gagPlayer->IsFinished() && m_isNISSoundComplete)
				{
					gagPlayer->Rewind();

					SoundUnload(); // Just to be sure
					SoundLoad();

					GetTriggerVolumeTracker()->AddTrigger(trigger);

					//sparkles remain off once triggered
					//sparkle = true;
				}
			}
        }

        if((mTimeToCameraShake > 0) && ((mTimeToCameraShake - (int)elapsedTime) <= 0)) 
        {
            mTimeToCameraShake = 0;
            GetSuperCamManager()->GetSCC(0)->GetActiveSuperCam()->AllowShake();
            GetEventManager()->TriggerEvent(EVENT_CAMERA_SHAKE, &binding->shake);
        }

        if((mTimeToCameraShakeEnd > 0) && ((mTimeToCameraShakeEnd - (int)elapsedTime) <= 0)) 
        {
            mTimeToCameraShakeEnd = 0;
            GetSuperCamManager()->GetSCC(0)->GetActiveSuperCam()->DisableShake();
        }

        if((mTimeToCoinSpawn > 0) && ((mTimeToCoinSpawn - (int)elapsedTime) <= 0)) 
        {
            mTimeToCoinSpawn = 0;
            rmt::Vector pos;
            draw->GetPosition(&pos);
            GetCoinManager()->SpawnInstantCoins(binding->coins, pos);
        }

        if(mTimeToCameraShake > 0)
        {
            mTimeToCameraShake -= elapsedTime;
        }
        if(mTimeToCameraShakeEnd > 0)
        {
            mTimeToCameraShakeEnd -= elapsedTime;
        }
        if(mTimeToCoinSpawn > 0)
        {
            mTimeToCoinSpawn -= elapsedTime;
        }

        gagPlayer->Update( elapsedTime );

        draw->SimUpdate( static_cast< float >( elapsedTime) );
    }

    InteriorManager::GagBinding* GetBinding(void) { return binding; }

protected:
    void StartIntro(void)
    {
        if(binding->loopIntro != 0)
        {
            gagPlayer->Play();
        }
    }

    DialogEventData mDialogEventData;

    void PlayDialog(radKey32 DialogID)
    {
        rAssert(strlen(binding->dialogChar1) != 0 && strlen(binding->dialogChar1) < DialogueObjective::MAX_CHAR_NAME_LEN);
        rAssert(strlen(binding->dialogChar2) != 0 && strlen(binding->dialogChar2) < DialogueObjective::MAX_CHAR_NAME_LEN);

        //Register with the event system that you are listening for the dialogue end
        //event.
        GetEventManager()->AddListener( this, EVENT_CONVERSATION_DONE );

        //Send the alert to the dialogue system to start playing this bad-ass
        Character* c1 = GetCharacterManager()->GetCharacterByName(binding->dialogChar1);
        tRefCounted::Assign(mDialogEventData.char1, c1);
        TrafficManager::GetInstance()->AddCharacterToStopFor(c1);

        Character* c2 = GetCharacterManager()->GetMissionCharacter(binding->dialogChar2);
        tRefCounted::Assign(mDialogEventData.char2, c2);
        TrafficManager::GetInstance()->AddCharacterToStopFor(c2);

        mDialogEventData.dialogName = DialogID;
        GetEventManager()->TriggerEvent(EVENT_CONVERSATION_INIT, (void*)(&mDialogEventData));
        GetEventManager()->TriggerEvent(EVENT_CONVERSATION_INIT_DIALOG, (void*)(&mDialogEventData));
        GetEventManager()->TriggerEvent( EVENT_CONVERSATION_START, 0);
    }

    void Play(void)
    {
        rmt::Box3D box;
        rmt::Vector pos;

        playing = true;
        bool alreadyPlayed = false;

        if(binding->persistIndex != -1)
        {
            alreadyPlayed = GetCharacterSheetManager()->QueryGagViewed(GetGameplayManager()->GetCurrentLevelIndex(), binding->persistIndex);
            if(!alreadyPlayed)
            {
                GetCharacterSheetManager()->AddGagViewed(GetGameplayManager()->GetCurrentLevelIndex(), binding->persistIndex);

                GetEventManager()->TriggerEvent( EVENT_GAG_FOUND );
            }
			else
			{
				// We've already played this gag. Don't spawn any more coins. We can safely change this in the
				//binding because we're dealing with a state that is persistent over the entire game. If this
				//functionality changes we'll need to set some sort of flag in the the playing gag which the
				//coin spawning coin can check later on.
				binding->coins = 0;
			}
        }

        //
        // Not getting good position info here.  Play from avatar position
        //
        Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
        rAssert( avatar != NULL );  //NIGEL - I put this assert in for you! love Ian.

        if( avatar != NULL )
        {
            avatar->GetPosition( pos );
            box.Set( pos, pos );
        }
        else
        {
            box.Set( rmt::Vector( 0.0f, 0.0f, 0.0f ), rmt::Vector( 0.0f, 0.0f, 0.0f ) );
        }

        // special case logic for the I&S movie. *sigh*
        if((binding->i_S_Movie == 1) && (!GetCharacterSheetManager()->IsState(1)))
        {
            //The player is trying to pick up the I&S ticket!
            bool haveAllCards = true;
            for(int level = RenderEnums::L1; level < RenderEnums::numLevels; ++level)
            {
                if(GetCardGallery()->IsCardDeckComplete(level) == false)
                {
                    haveAllCards = false;
                    break;
                }
            }
            if(haveAllCards)
            {
                // They have all the cards, give them the ticket.
                GetCharacterSheetManager()->SetState(1, true);
				GetCharacterSheetManager()->SetFMVUnlocked(RenderEnums::L3);
                GetInteriorManager()->SetISMovieDialogPlaying( true );
                PlayDialog(binding->acceptDialogID);
                binding->retrigger = false;
                binding->triggered = false;
                // Do the collection effect.
                GetInteriorManager()->CollectionEffect("card_collect", binding->triggerPos);
            }
            else
            {
                if(GetCharacterSheetManager()->IsState(0))
                {
                    // Not enough cards.
                    GetInteriorManager()->SetISMovieDialogPlaying( true );
                    PlayDialog(binding->rejectDialogID);
//                   GetSoundManager()->PlayNISSound( binding->rejectSoundID, &box );
                }
                else
                {
                    // Give instructions.
                    GetCharacterSheetManager()->SetState(0, true);
                    GetInteriorManager()->SetISMovieDialogPlaying( true );
                    PlayDialog(binding->instructDialogID);
//                    GetSoundManager()->PlayNISSound( binding->instructSoundID, &box );
                }
            }
        }
        else if(binding->i_S_Movie == 2)
        {
            // The player is trying to watch the I&S movie.
            if(GetCharacterSheetManager()->QueryFMVUnlocked(RenderEnums::L3))
            {
                // They have a ticket, play the movie.
                GetPresentationManager()->PlayFMV( binding->gagFMVFileName );
            }
            else
            {
                // They don't have a ticket.
                GetInteriorManager()->SetISMovieDialogPlaying( true );
                PlayDialog(binding->rejectDialogID);
            }
            gagPlayer->Play();
        }
        else
        {
            if( !mSoundLoaded )
            {
                SoundLoad();
            }
            GetSoundManager()->PlayNISSound( binding->soundID, &box, this );
            m_isNISSoundComplete = false;

            if(binding->loopIntro != 0)
            {
                gagPlayer->DoneIntro();
            }
            else
            {
                gagPlayer->Play();
            }

            if( binding->gagFMVFileName[ 0 ] != 0 )
            {
                GetPresentationManager()->PlayFMV( binding->gagFMVFileName, NULL, true, false, false );
            }
        }

        if(trigger)
        {
            GetTriggerVolumeTracker()->RemoveTrigger(trigger);
        }

        if(binding->cameraShake)
        {
            mTimeToCameraShake = (binding->shakeDelay == 0.0f) ? 1 : int(binding->shakeDelay * 1000.0f);
            mTimeToCameraShakeEnd = (binding->shakeDuration == 0.0f) ? 0 : int((binding->shakeDelay + binding->shakeDuration) * 1000.0f);
        }

        if(binding->coins && (binding->coinDelay >= 0.0f) && !alreadyPlayed)
        {
            mTimeToCoinSpawn = (binding->coinDelay == 0.0f) ? 1 : int(binding->coinDelay * 1000.0f);
        }

        GetEventManager()->TriggerEvent(EVENT_GAG_START, (void*)binding);
        sparkle = false;
    }

public:
    Gag(InteriorManager::GagBinding* b)
        : binding(NULL), 
          locator(NULL), 
          trigger(NULL),
          draw(NULL),
          gagPlayer(NULL),
          collObj(NULL),
          mLoaded(false),
          mLoading(false),
          mTimeToCameraShake(0),
          mTimeToCameraShakeEnd(0),
          mTimeToCoinSpawn(0),
          sparkle(false),
          mSoundLoaded(false),
          playing(false),
          m_isNISSoundComplete(true)
    {
        binding = b; 

        gagPlayer = new NISPlayer;
        gagPlayer->SetNameData( "GagController", NULL, "GagScene" );
        gagPlayer->SetPlayAfterLoad( false );
        gagPlayer->SetCycleMode( binding->cycleMode );
        gagPlayer->SetShowAlways( true );
        gagPlayer->SetIntroLoop(binding->loopIntro);
        gagPlayer->SetOutroLoop(binding->loopOutro);

        if(binding->interiorUID != static_cast< tUID >( 0 ) )
        {
            Load();
        }
    }

    ~Gag()
    {
        Unload();
        SoundUnload();
        delete gagPlayer;
    }

    bool IsLoaded(void)
    {
        return mLoaded;
    }

    void OnLoadDataComplete(void)
    {
        bool oldOnly = p3d::inventory->GetCurrentSectionOnly();
        p3d::inventory->PushSection();

        char buffy[256];
        sprintf( buffy, "art\\nis\\gags\\%s", binding->gagFileName );
        p3d::inventory->SelectSection(buffy);
        p3d::inventory->SetCurrentSectionOnly(true);
        mLoaded = true;
        mLoading = false;

        collObj = p3d::find<sim::CollisionObject>("GagScene");
        if(collObj)
        {
            collObj->AddRef();
        }

        p3d::inventory->SetCurrentSectionOnly(false);
        // Add gag animation to renderer
        //
        HeapMgr()->PushHeap(GMA_LEVEL_OTHER);
        draw = new GagDrawable(this);
        draw->AddRef();


        if(binding->triggered)
        {
			if(!((binding->i_S_Movie == 1) && 
                 ((!GetMissionManager()->IsSundayDrive()) || 
                 (GetCharacterSheetManager()->QueryFMVUnlocked(RenderEnums::L3)))))
			{
				if(binding->useTriggerLocator)
				{
					Locator* l = p3d::find<Locator>(binding->triggerLoc);
					//rAssert(l); //get's fixed up later
					if(l)
					{
						l->GetLocation(&binding->triggerPos);
					}
				}

				locator = new EventLocator;
				locator->SetEventType( LocatorEvent::GAG );
				locator->SetData((unsigned int)this);

				trigger = new SphereTriggerVolume(binding->triggerPos, binding->triggerRadius);
				trigger->SetLocator(locator);
                locator->SetNumTriggers( 1, HeapMgr()->GetCurrentHeap() );
                locator->AddTriggerVolume( trigger );

				GetTriggerVolumeTracker()->AddTrigger(trigger);

				locator->AddRef();
				trigger->AddRef();

				StartIntro();
			}
        }
        else
        {
            SoundLoad();
            Play();
        }

        rmt::Vector gagPos = binding->gagPos;
        if(binding->useGagLocator)
        {
            Locator* l = p3d::find<Locator>(binding->gagLoc);
            //rAssert(l); //apparently *something* else positions this properly later
            if(l)
            {
                l->GetLocation(&gagPos);
                binding->gagPos = gagPos;
            }
        }

        if(gagPos == rmt::Vector(0.0f,0.0f,0.0f))
        {
            rmt::Box3D box;
            draw->GetBoundingBox(&box);
            rmt::Vector c = box.high + box.low;
            c /= 2;
            draw->SetPosition(false, c);
        }
        else
        {
            draw->SetPosition(true, gagPos);
        }

        ((WorldRenderLayer*)GetRenderManager()->mpLayer(RenderEnums::LevelSlot))->pWorldScene()->Add(draw);

        p3d::inventory->PopSection();
        p3d::inventory->SetCurrentSectionOnly(oldOnly);
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
        
        bool alreadyPlayed = false;
        if(binding->persistIndex != -1)
        {
            alreadyPlayed = GetCharacterSheetManager()->QueryGagViewed(GetGameplayManager()->GetCurrentLevelIndex(), binding->persistIndex);
        }
        if(alreadyPlayed)
        {
            sparkle = false;
        }
        else
        {
            sparkle = true;
        }
    }

    void Trigger(EventLocator* loc)
    {
        if(((binding->gagFMVFileName[0] != 0) ||  (binding->i_S_Movie != 0)))
        {
            if(!GetGameplayManager()->GetCurrentMission()->IsSundayDrive())
            {
                return;
            }
        }

        if(binding->action)
        {
            Character* pCharacter = GetCharacterManager()->GetCharacter( PLAYER_ONE );
            if(loc->GetPlayerEntered())
            {
                //
                // Send a message about an active interactive gag
                //
                if(binding->triggered)
                {
                    GetEventManager()->TriggerEvent( EVENT_INTERACTIVE_GAG );
                }

                Enter(pCharacter);
                pCharacter->AddActionButtonHandler(this);
            }
            else
            {
                Exit(pCharacter);
                pCharacter->RemoveActionButtonHandler(this);
            }
        }
        else
        {
            if(loc->GetPlayerEntered())
            {
                Play();
            }
        }
    }

protected:
    friend class GagDrawable;

    InteriorManager::GagBinding* binding;
    EventLocator* locator;
    SphereTriggerVolume* trigger;
    GagDrawable* draw;
    NISPlayer* gagPlayer;
    sim::CollisionObject* collObj;
    bool mLoaded;
    bool mLoading;
    int mTimeToCameraShake;
    int mTimeToCameraShakeEnd;
    int mTimeToCoinSpawn;
    bool sparkle;
    bool mSoundLoaded;
    bool playing;
    bool m_isNISSoundComplete;


    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
    {
        Play();
        return true;
    }
};

GagDrawable::GagDrawable(Gag* g) : gag(g), mPose(NULL)
{
    mPosn.Set(0.0f, 0.0f, 0.0f);
    mpDrawstuff = gag->gagPlayer->GetDrawable(); 
    mpDrawstuff->AddRef();
    mTranslucent = true;

    // I need to go take a shower after this
    for(int i = 0; i < NUM_OPAQUES; i++)
    {
        if(strcmp(sOpaques[i], gag->binding->gagFileName) == 0)
        {
            mTranslucent = false;
        }
    }

    useTranslate = false;
    sparkleStrength = 0.0f;

    if(g->collObj)
    {
        tCompositeDrawable* cd = dynamic_cast<tCompositeDrawable*>(mpDrawstuff);
        if(cd && gag->binding->animBV)
        {
            mPose = new poser::Pose(cd->GetPose());
            SetSimState(sim::SimStateArticulated::CreateSimStateArticulated(mPose, g->collObj, NULL));
	        mpSimStateObj->SetControl(sim::simAICtrl);
            mpSimStateObj->GetCollisionObject()->SetIsStatic(false);
        }
        else
        {
            SetSimState(sim::SimState::CreateStaticSimState(g->collObj));
            mpSimStateObj->GetCollisionObject()->SetIsStatic(false);
        }
    }
}

void GagDrawable::Display()
{
    if((gag->binding->i_S_Movie == 1) && (GetCharacterSheetManager()->QueryFMVUnlocked(RenderEnums::L3)))
    {
        return;
    }
    if(useTranslate)
    {
        p3d::stack->Push();
        p3d::stack->Translate(mPosn);
    }

    gag->Render();

    if(useTranslate)
    {
        p3d::stack->Pop();
    }

    if((sparkleStrength > 0.0f) && gag->sparkle)
    {
        float size = gag->binding->interiorUID == static_cast< tUID >( 0 ) ? 1.0f : 0.5f;
        GetSparkleManager()->AddGagSparkle(gag->binding->triggerPos, size, sparkleStrength, (unsigned int)this);
    }
}

class InteriorExit : public ActionButton::ButtonHandler
{
public:
    virtual bool OnButtonPressed( Character* pCharacter, Sequencer* pSeq )
    {
        if( cGuiManagerInGameActive && !GetGuiSystem()->GetInGameManager()->IsEnteringPauseMenu() )
        {
            GetEventManager()->TriggerEvent( EVENT_EXIT_INTERIOR_START );
        }
        return true;
    }
};

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// InteriorManager::CreateInstance
//==============================================================================
//
// Description: Creates the InteriorManager.
//
// Parameters:	None.
//
// Return:      Pointer to the InteriorManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
InteriorManager* InteriorManager::CreateInstance()
{
    s_InteriorCentres[0].name.SetText("SpringfieldElementary");
    s_InteriorCentres[0].centre.Set(500, -20, -350);
    s_InteriorCentres[0].level = RenderEnums::L1;

    s_InteriorCentres[1].name.SetText("KwikEMart");
    s_InteriorCentres[1].centre.Set(500, -20, -300);
    s_InteriorCentres[1].level = RenderEnums::L1;

    s_InteriorCentres[2].name.SetText("SimpsonsHouse");
    s_InteriorCentres[2].centre.Set(500, -20, -400);
    s_InteriorCentres[2].level = RenderEnums::L1;

    s_InteriorCentres[3].name.SetText("dmv");
    s_InteriorCentres[3].centre.Set(0, -20, -200);
    s_InteriorCentres[3].level = RenderEnums::L2;

    s_InteriorCentres[4].name.SetText("moe1");
    s_InteriorCentres[4].centre.Set(50, -20, -200);
    s_InteriorCentres[4].level = RenderEnums::L2;

    s_InteriorCentres[5].name.SetText("Android");
    s_InteriorCentres[5].centre.Set(0, -20, -350);
    s_InteriorCentres[5].level = RenderEnums::L3;

    s_InteriorCentres[6].name.SetText("Observatory");
    s_InteriorCentres[6].centre.Set(150, -20, -350);
    s_InteriorCentres[6].level = RenderEnums::L3;

    s_InteriorCentres[7].name.SetText("bartroom");
    s_InteriorCentres[7].centre.Set(500, -20, -450);
    s_InteriorCentres[7].level = RenderEnums::L1;

    MEMTRACK_PUSH_GROUP( "InteriorManager" );
        
    rAssert( spInstance == NULL );

    spInstance = new InteriorManager;
    rAssert( spInstance );

    MEMTRACK_POP_GROUP("InteriorManager");
    
    return spInstance;
}

const tName& InteriorManager::ClassifyPoint(const rmt::Vector& point)
{
    static tName none((tUID)0);

    for(int i = 0; i < 8; i++)
    {
        if((GetGameplayManager()->GetCurrentLevelIndex() % 3)  == s_InteriorCentres[i].level)
        {
            rmt::Vector dist = point - s_InteriorCentres[i].centre;
            if(dist.Magnitude() < 24.0f)
            {
                return s_InteriorCentres[i].name;
            }
        }
    }

    return none;
}

//==============================================================================
// InteriorManager::GetInstance
//==============================================================================
//
// Description: - Access point for the InteriorManager singleton.  
//
// Parameters:	None.
//
// Return:      Pointer to the InteriorManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
InteriorManager* InteriorManager::GetInstance()
{
    rAssert( spInstance != NULL );
    
    return spInstance;
}


//==============================================================================
// InteriorManager::DestroyInstance
//==============================================================================
//
// Description: Destroy the InteriorManager.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void InteriorManager::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete( GMA_PERSISTENT, spInstance );
    spInstance = NULL;
}


//==============================================================================
// InteriorManager::OnBootupStart
//==============================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
//==============================================================================
void InteriorManager::OnBootupStart()
{
    GetEventManager()->AddListener( this, EVENT_INTERIOR_LOAD_START );
    GetEventManager()->AddListener( this, EVENT_INTERIOR_LOADED );
    GetEventManager()->AddListener( this, EVENT_INTERIOR_DUMPED );
    GetEventManager()->AddListener( this, EVENT_ENTER_INTERIOR_START );
    GetEventManager()->AddListener( this, EVENT_EXIT_INTERIOR_START );
    GetEventManager()->AddListener( this, EVENT_ENTER_INTERIOR_TRANSITION_START );
    GetEventManager()->AddListener( this, EVENT_ENTER_INTERIOR_TRANSITION_END );
    GetEventManager()->AddListener( this, static_cast<EventEnum>(EVENT_LOCATOR + LocatorEvent::INTERIOR_EXIT) );
    GetEventManager()->AddListener( this, static_cast<EventEnum>(EVENT_LOCATOR + LocatorEvent::GAG) );
    GetEventManager()->AddListener( this, EVENT_GUI_IRIS_WIPE_CLOSED );
    GetEventManager()->AddListener( this, EVENT_GUI_IRIS_WIPE_OPEN );
    GetEventManager()->AddListener( this, EVENT_CHARACTER_POS_RESET );
    
    GetConsole()->AddFunction( "ClearGagBindings", 
                               InteriorManager::ConsoleClearGagBindings,
                               "ClearGagBindings();",
                               0, 0 );

    GetConsole()->AddFunction( "AddGagBinding", 
                               InteriorManager::ConsoleAddGagBinding, 
                               "AddGagBinding(InteriorName, GagFileName, CycleMode, Weight, SoundResourceName);",
                               5, 5 );

    GetConsole()->AddFunction( "GagBegin", 
                               InteriorManager::ConsoleGagBegin, 
                               ",",
                               1, 1 );

    GetConsole()->AddFunction( "GagSetInterior", 
                               InteriorManager::ConsoleGagSetInterior, 
                               ",",
                               1, 1 );

    GetConsole()->AddFunction( "GagSetCycle", 
                               InteriorManager::ConsoleGagSetCycle, 
                               ",",
                               1, 1 );

    GetConsole()->AddFunction( "GagSetWeight", 
                               InteriorManager::ConsoleGagSetWeight, 
                               ",",
                               1, 1 );

    GetConsole()->AddFunction( "GagSetSound", 
                               InteriorManager::ConsoleGagSetSound, 
                               ",",
                               1, 1 );

    GetConsole()->AddFunction( "GagSetTrigger", 
                               InteriorManager::ConsoleGagSetTrigger, 
                               ",",
                               3, 5 );

    GetConsole()->AddFunction( "GagPlayFMV", InteriorManager::ConsoleGagPlayFMV,
                               "Play FMV specified by file name", 1, 1 );

    GetConsole()->AddFunction( "GagSetPosition", 
                               InteriorManager::ConsoleGagSetPosition, 
                               ",",
                               1, 3 );

    GetConsole()->AddFunction( "GagSetRandom", 
                               InteriorManager::ConsoleGagSetRandom, 
                               ",",
                               1, 1 );

    GetConsole()->AddFunction( "GagSetIntro", 
                               InteriorManager::ConsoleGagSetIntro, 
                               ",",
                               1, 1 );

    GetConsole()->AddFunction( "GagSetOutro", 
                               InteriorManager::ConsoleGagSetOutro, 
                               ",",
                               1, 1 );

    GetConsole()->AddFunction( "GagSetCameraShake", 
                               InteriorManager::ConsoleGagSetCameraShake, 
                               ",",
                               2, 3 );

    GetConsole()->AddFunction( "GagSetCoins", 
                               InteriorManager::ConsoleGagSetCoins, 
                               ",",
                               1, 2 );

    GetConsole()->AddFunction( "GagSetSparkle", 
                               InteriorManager::ConsoleGagSetSparkle, 
                               ",",
                               1, 1 );

    GetConsole()->AddFunction( "GagSetAnimCollision", 
                               InteriorManager::ConsoleGagSetAnimCollision, 
                               ",",
                               1, 1 );

    GetConsole()->AddFunction( "GagEnd", 
                               InteriorManager::ConsoleGagEnd, 
                               ",",
                               0, 0 );

    GetConsole()->AddFunction( "GagSetLoadDistances",
                               InteriorManager::ConsoleGagSetLoadDistances,
                               ",",
                               2, 2 );

    GetConsole()->AddFunction( "GagSetSoundLoadDistances",
                               InteriorManager::ConsoleGagSetSoundLoadDistances,
                               ",",
                               2, 2 );

    GetConsole()->AddFunction( "GagSetPersist",
                               InteriorManager::ConsoleGagSetPersist,
                               ",",
                               1, 1 );
    GetConsole()->AddFunction( "GagCheckCollCards",
                               InteriorManager::ConsoleGagCheckCollCards,
                               ",",
                               5, 5);
    GetConsole()->AddFunction( "GagCheckMovie",
                               InteriorManager::ConsoleGagCheckMovie,
                               ",",
                               4, 4);
}


//==============================================================================
// InteriorManager::OnMissionRestart
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void InteriorManager::OnMissionRestart()
{
}


//==============================================================================
// InteriorManager::OnGameplayStart
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void InteriorManager::OnGameplayStart()
{
    if(!mLoadedGags)
    {
        mEntryRequested = false;
        LoadGagNIS(0);
    }
    mCollectionEffect = new AnimatedIcon();
    rAssert(mCollectionEffect);
}


//==============================================================================
// InteriorManager::OnGameplayEnd
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void InteriorManager::OnGameplayEnd()
{
    ClearGags();
    this->ClearGagBindings();

    SwitchToExterior();
    delete mCollectionEffect;
    mCollectionEffect = 0;
}

//=============================================================================
// InteriorManager::UnloadGagSounds
//=============================================================================
// Description: We need to do this before movies get played (i.e. Larry the
//              Looter)
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void InteriorManager::UnloadGagSounds()
{
    int i;

    for( i = 0; i < mGagCount; i++ )
    {
        gags[i]->SoundUnload();
    }
}

void InteriorManager::Enter(InteriorEntranceLocator* entry, Character* pCharacter, Sequencer* pSeq)
{
    //
    // Once this sequence starts, we're on autopilot until the character
    // appears and hits his cue in the interior so disable controller input.
    //
    pCharacter->GetController()->SetActive( false );
    
    rmt::Matrix transform = entry->GetTransform();
    rmt::Vector standPosition;
    entry->GetLocation( &standPosition ) ;

    Action* pAction = 0;

    // trigger the event.
    //
    pSeq->BeginSequence();
    pAction = new TriggerEventAction( EVENT_ENTER_INTERIOR_START, (void*)&mLoadedInteriorUID );
    pSeq->AddAction( pAction );
    pAction = new TriggerEventAction( EVENT_ENTER_INTERIOR_TRANSITION_START, (void*)&mLoadedInteriorUID );
    pSeq->AddAction( pAction );
    pSeq->EndSequence( );

    // Walk to entry point
    /*
    pSeq->BeginSequence();

    pAction = new Arrive( pCharacter, standPosition );
    pSeq->AddAction( pAction );
    pAction = pCharacter->GetWalkerLocomotionAction();
    pSeq->AddAction( pAction );
    pSeq->EndSequence();
    */

    // Orient
    rmt::Vector direction = transform.Row( 2 );
    direction.y = 0.0f;
    // If direction != 0, 0, 0, then rotate us to align with direction.
    //
    if ( direction.NormalizeSafe( ) )
    {
        pSeq->BeginSequence();
        pAction = new Orient( pCharacter, direction );
        pSeq->AddAction( pAction );
        // SlaveLoco
        //
        pAction = 0;

        pAction = pCharacter->GetWalkerLocomotionAction();
	    
	    pSeq->AddAction( pAction );
        pSeq->EndSequence( );
    }

/*   
    // Move character.
    //
    pSeq->BeginSequence();
    pAction = new Position( pCharacter, standPosition, 0.1f );
    pSeq->AddAction( pAction );
    pSeq->EndSequence( );
*/

}

//==============================================================================
// InteriorManager::Update
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void InteriorManager::Update( unsigned int elapsedTime )
{
    for(int i = 0; i < mGagCount; i++)
    {
        if(gags[i])
        {
            gags[i]->Update(elapsedTime);
        }
    }

    if(mInteriorAnimations)
    {
        mInteriorAnimations->Advance((float)elapsedTime);
    }

    AttemptEntry();

    mCollectionEffect->Update(elapsedTime);
}


//==============================================================================
// InteriorManager::HandleEvent
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void InteriorManager::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
        case EVENT_INTERIOR_LOAD_START:
        {
            rAssert( pEventData != 0 );
            InteriorLoadedEventData* pData = reinterpret_cast<InteriorLoadedEventData*>( pEventData );

            mSection = pData->sectionName.GetUID(); 
            mLoadedInteriorUID = pData->interiorName.GetUID();
        }
        break;

        case EVENT_ENTER_INTERIOR_TRANSITION_START:
        {
            mState = ENTER;
            GetExitPos();
        }
        break;
    
        case EVENT_INTERIOR_LOADED:
        {
            rAssert( pEventData != 0 );
            InteriorLoadedEventData* pData = reinterpret_cast<InteriorLoadedEventData*>( pEventData );

            mSection = pData->sectionName.GetUID(); 
            mLoadedInteriorUID  = pData->interiorName.GetUID();

            mInteriorLoaded = true;

            if(mIn)
            {
                SetupLightsAndAnims();
            }
        }
        break;

        case EVENT_INTERIOR_DUMPED:
        {
            mInteriorLoaded = false;
            mLoadedInteriorUID  = 0;
        }
        break;

        case EVENT_CHARACTER_POS_RESET :
        {
            rmt::Vector pos;
            GetCharacterManager()->GetCharacter(0)->GetPosition(pos);
            LoadLevelGags(pos);
        }
        break;

        case EVENT_GUI_IRIS_WIPE_OPEN:
        {
            Character* pCharacter = GetCharacterManager()->GetCharacter( PLAYER_ONE );
            pCharacter->GetController()->SetActive( true );
            
            if(mState == ENTER)
            {
                mState = INSIDE;
            }

            if(mState == EXIT)
            {
                mState = NONE;
            }
        }
        break;

        case EVENT_GUI_IRIS_WIPE_CLOSED:
        {
            switch( mState )
            {
                case ENTER:
                {
                    GetRenderManager()->mpLayer(RenderEnums::LevelSlot)->Freeze();
                    mEntryRequested = true;
                    mLoadedGags = false;
                }
                break;

                case EXIT:
                {
                    this->ExitInterior();
                }
                break;
    
                default:
                {
                    //rAssert( 0 );
                }
            }
        }
        break;

        case EVENT_EXIT_INTERIOR_START:
        {
            Character* pCharacter = GetCharacterManager()->GetCharacter( PLAYER_ONE );
            pCharacter->GetController()->SetIntention( CharacterController::NONE );
            pCharacter->GetController()->SetActive( false );
            mState = EXIT;
        }
        break;

        case EVENT_LOCATOR + LocatorEvent::INTERIOR_EXIT:
        {
            if((mState == INSIDE) || (mState == ENTER))
            {
                if(1)
                {
                    Character* pCharacter = GetCharacterManager()->GetCharacter( PLAYER_ONE );
                    EventLocator* loc = (EventLocator*)pEventData;
                    if(loc->GetPlayerEntered())
                    {
                        mExit->Enter(pCharacter);
                        pCharacter->AddActionButtonHandler(mExit);
                    }
                    else
                    {
                        mExit->Exit(pCharacter);
                        pCharacter->RemoveActionButtonHandler(mExit);
                    }
                }
                else
                {
                    EventLocator* loc = (EventLocator*)pEventData;
                    if(loc->GetPlayerEntered())
                    {
                        mExit->OnButtonPressed(NULL, NULL);
                    }
                }
            }
        }
        break;

        case EVENT_LOCATOR + LocatorEvent::GAG:
        {
            EventLocator* loc = (EventLocator*)pEventData;
            for(int i = 0; i < mGagCount; i++)
            {
                if(loc->GetData() == (unsigned int)gags[i])
                {
                    gags[i]->Trigger(loc);
                }
            }
        }
        break;

        default:
        {
        }
    }
}


//==============================================================================
// InteriorManager::ConsoleClearGagBindings
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void InteriorManager::ConsoleClearGagBindings( int argc, char** argv )
{
    GetInteriorManager()->ClearGagBindings();
}

void InteriorManager::ClearGagBindings()
{
    for(int i = 0; i < mBindingCount; i++)
    {
        mGagBindings[i].Clear();
    }

    mBindingCount = 0;
    sPersistGagIndex = 0;
}


//==============================================================================
// InteriorManager::ConsoleAddGagBinding
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void InteriorManager::ConsoleAddGagBinding( int argc, char** argv )
{
    // Convert interior name to UID
    //
    tUID interiorUID = tName::MakeUID( argv[1] );

    char* gagFileName = argv[2];
    
    // Select cycle mode.
    //
    p3dCycleMode cycleMode;
    
    if( !strcmp( "default", argv[3] ) )
    {
        cycleMode = DEFAULT_CYCLE_MODE;
    }
    else if( !strcmp( "cycle", argv[3] ) )
    {
        cycleMode = FORCE_CYCLIC;
    }
    else if( !strcmp( "single", argv[3] ) )
    {
        cycleMode = FORCE_NON_CYCLIC;
    }
    else
    {
        rAssertMsg( 0, "Invalid cycle mode" );
        cycleMode = DEFAULT_CYCLE_MODE;
    }

    // Assign weighted probability of selecting gag.
    //
    int weight = atoi( argv[4] );

    //
    // Get the name of the sound resource to play with this gag
    //
    char* gagSound = argv[5];

    // Send it on down...
    //
    GetInteriorManager()->AddGagBinding( interiorUID, gagFileName, cycleMode, weight, gagSound );
}


//==============================================================================
// InteriorManager::AddGagBinding
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void InteriorManager::AddGagBinding
( 
    tUID interiorUID, 
    char* gagFileName, 
    p3dCycleMode cycleMode, 
    int weight,
    char* gagSound
)
{
    // Can raise the max if necessary.
    //
    rAssert( mBindingCount < MAX_BINDINGS );

    // Convert interior name to UID
    //
    mGagBindings[mBindingCount].interiorUID = interiorUID;
    
    // Save the gag filename.
    //
    rAssert( strlen( gagFileName ) < 13 );
    strcpy( mGagBindings[mBindingCount].gagFileName, gagFileName );

    // Select cycle mode.
    //
    mGagBindings[mBindingCount].cycleMode = cycleMode;

    // Assign weighted probability of selecting gag.
    //
    mGagBindings[mBindingCount].weight = weight;

    //
    // Make a radKey out of the gag sound name
    //
    mGagBindings[mBindingCount].soundID = ::radMakeKey32( gagSound );

    mGagBindings[ mBindingCount ].gagFMVFileName[ 0 ] = 0;

    if(mGagBindings[mBindingCount].interiorUID)
    {
        if(!mGagBindings[ mBindingCount ].useGagLocator)
        {
            mGagBindings[ mBindingCount ].useGagLocator = true;
            mGagBindings[ mBindingCount ].gagLoc = tEntity::MakeUID("InteriorOrigin");
        }
    }

    ++mBindingCount;
}

void InteriorManager::GagBegin(char* gagFileName)
{
    rAssert(!mBuildingGag);
    mBuildingGag = true;

    // Can raise the max if necessary.
    rAssert( mBindingCount < MAX_BINDINGS );

    // Save the gag filename.
    rAssert( strlen( gagFileName ) < 13 );
    strcpy( mGagBindings[mBindingCount].gagFileName, gagFileName );
}

InteriorManager::GagBinding* InteriorManager::GetBuildBinding(void)
{
    rAssert(mBuildingGag);
    return &mGagBindings[mBindingCount];
}

void InteriorManager::GagEnd(void)
{
    rAssert(mBuildingGag);
    mBuildingGag = false;
    if((mGagBindings[mBindingCount].i_S_Movie == 1) && (GetCharacterSheetManager()->IsState(1)))
    {
        // Oops we've already got the ticket! Don't put it in again.
        mGagBindings[mBindingCount].Clear();
        return;
    }
    ++mBindingCount;
}

void InteriorManager::ConsoleGagBegin(int argc, char** argv )
{
    GetInteriorManager()->GagBegin(argv[1]);
}

void InteriorManager::ConsoleGagSetInterior(int argc, char** argv )
{
    GetInteriorManager()->GetBuildBinding()->interiorUID = tEntity::MakeUID(argv[1]);

    if(GetInteriorManager()->GetBuildBinding()->interiorUID)
    {
        if(!GetInteriorManager()->GetBuildBinding()->useGagLocator)
        {
            GetInteriorManager()->GetBuildBinding()->useGagLocator = true;
            GetInteriorManager()->GetBuildBinding()->gagLoc = tEntity::MakeUID("InteriorOrigin");
        }
    }
}

void InteriorManager::ConsoleGagSetCycle(int argc, char** argv )
{
    // Select cycle mode.
    //
    p3dCycleMode cycleMode;
    
    if( !strcmp( "default", argv[1] ) )
    {
        cycleMode = DEFAULT_CYCLE_MODE;
    }
    else if( !strcmp( "cycle", argv[1] ) )
    {
        cycleMode = FORCE_CYCLIC;
    }
    else if( !strcmp( "single", argv[1] ) )
    {
        cycleMode = FORCE_NON_CYCLIC;
    }
    else if( !strcmp( "reset", argv[1] ) )
    {
        cycleMode = FORCE_NON_CYCLIC;
        GetInteriorManager()->GetBuildBinding()->retrigger = true;
    }
    else
    {
        rAssertMsg( 0, "Invalid cycle mode" );
        cycleMode = DEFAULT_CYCLE_MODE;
    }

    GetInteriorManager()->GetBuildBinding()->cycleMode = cycleMode;
}

void InteriorManager::ConsoleGagSetWeight(int argc, char** argv )
{
    GetInteriorManager()->GetBuildBinding()->weight = atoi(argv[1]);
}

void InteriorManager::ConsoleGagSetSound(int argc, char** argv )
{
    GetInteriorManager()->GetBuildBinding()->soundID = ::radMakeKey32(argv[1]);
}

void InteriorManager::ConsoleGagSetIntro( int argc, char** argv )
{
    GetInteriorManager()->GetBuildBinding()->loopIntro = atoi(argv[1]);
}

void InteriorManager::ConsoleGagSetOutro( int argc, char** argv )
{
    GetInteriorManager()->GetBuildBinding()->loopOutro = atoi(argv[1]);
}

/*=============================================================================
Description:    Play the FMV specified by file name. Note that PlayFMV causes
                the HUD to be un/reloaded.
=============================================================================*/
void InteriorManager::ConsoleGagPlayFMV( int argc, char** argv )
{
    rAssert( argc > 1 );
    rAssert( strlen( argv[ 1 ] ) < 13 ); // Enforce 8.3 file naming.
    strcpy( GetInteriorManager()->GetBuildBinding()->gagFMVFileName, argv[ 1 ] );
}

void InteriorManager::ConsoleGagSetTrigger(int argc, char** argv )
{
    rAssert((argc == 4) || (argc == 6));

    GetInteriorManager()->GetBuildBinding()->triggered = true;
   
    if( strcmp( "touch", argv[1] ) == 0 )
    {
        GetInteriorManager()->GetBuildBinding()->action = false;
    }
    else if( strcmp( "action", argv[1]) == 0 ) 
    {
        GetInteriorManager()->GetBuildBinding()->action = true;
    }

    if(argc == 4)
    {
        GetInteriorManager()->GetBuildBinding()->useTriggerLocator = true;
        GetInteriorManager()->GetBuildBinding()->triggerLoc = tEntity::MakeUID(argv[2]);
    }
    else
    {
        GetInteriorManager()->GetBuildBinding()->useTriggerLocator = false;
        GetInteriorManager()->GetBuildBinding()->triggerPos.Set((float)atof(argv[2]), (float)atof(argv[3]), (float)atof(argv[4]));
    }

    GetInteriorManager()->GetBuildBinding()->triggerRadius = (float)atof(argv[argc-1]);
}

void InteriorManager::ConsoleGagSetPosition(int argc, char** argv )
{
    rAssert((argc == 2) || (argc == 4));

    if(argc == 2)
    {
        GetInteriorManager()->GetBuildBinding()->useGagLocator = true;
        GetInteriorManager()->GetBuildBinding()->gagLoc = tEntity::MakeUID(argv[1]);
    }
    else
    {
        GetInteriorManager()->GetBuildBinding()->useGagLocator = false;
        GetInteriorManager()->GetBuildBinding()->gagPos.Set((float)atof(argv[1]), (float)atof(argv[2]), (float)atof(argv[3]));
    }
}

void InteriorManager::ConsoleGagSetRandom(int argc, char** argv )
{
    GetInteriorManager()->GetBuildBinding()->random = atoi(argv[1]) != 0;
}

void InteriorManager::ConsoleGagSetCameraShake(int argc, char** argv )
{
    GetInteriorManager()->GetBuildBinding()->cameraShake = true;
    GetInteriorManager()->GetBuildBinding()->shakeDelay = (float)atof(argv[1]);
    GetInteriorManager()->GetBuildBinding()->shake.force = (float)atof(argv[2]);

    if(argc == 4)
    {
        GetInteriorManager()->GetBuildBinding()->shakeDuration = (float)atof(argv[3]);
        GetInteriorManager()->GetBuildBinding()->shake.looping = GetInteriorManager()->GetBuildBinding()->shakeDuration != 0.0f;
    }
}

void InteriorManager::ConsoleGagSetCoins(int argc, char** argv )
{
    GetInteriorManager()->GetBuildBinding()->coins = atoi(argv[1]);
    if(argc == 3)
    {
        GetInteriorManager()->GetBuildBinding()->coinDelay = (float)atof(argv[2]);
    }
}

void InteriorManager::ConsoleGagSetSparkle(int argc, char** argv )
{
    GetInteriorManager()->GetBuildBinding()->sparkle = atoi(argv[1]) != 0;
}

void InteriorManager::ConsoleGagSetAnimCollision(int argc, char** argv )
{
    GetInteriorManager()->GetBuildBinding()->animBV = atoi(argv[1]) != 0;
}

void InteriorManager::ConsoleGagSetLoadDistances( int argc, char** argv )
{
    GetInteriorManager()->GetBuildBinding()->loadDist = atoi(argv[1]);
    GetInteriorManager()->GetBuildBinding()->unloadDist = atoi(argv[2]);
}

void InteriorManager::ConsoleGagSetSoundLoadDistances( int argc, char** argv )
{
    GetInteriorManager()->GetBuildBinding()->soundLoadDist = atoi(argv[1]);
    GetInteriorManager()->GetBuildBinding()->soundUnloadDist = atoi(argv[2]);
}

void InteriorManager::ConsoleGagSetPersist( int argc, char** argv )
{
    if(atoi(argv[1]))
    {
        GetInteriorManager()->GetBuildBinding()->persistIndex = sPersistGagIndex++;
    }
    else
    {
        GetInteriorManager()->GetBuildBinding()->persistIndex = -1;
    }
} 

void InteriorManager::ConsoleGagCheckCollCards(int argc, char** argv)
{
    GetInteriorManager()->GetBuildBinding()->i_S_Movie = 1;

    unsigned int len = strlen(argv[1]) < DialogueObjective::MAX_CHAR_NAME_LEN - 1 ? strlen(argv[1]) : DialogueObjective::MAX_CHAR_NAME_LEN - 2;
    strncpy( GetInteriorManager()->GetBuildBinding()->dialogChar1, argv[1], len );
    GetInteriorManager()->GetBuildBinding()->dialogChar1[len] = '\0';

    len = strlen(argv[2]) < DialogueObjective::MAX_CHAR_NAME_LEN - 1 ? strlen(argv[2]) : DialogueObjective::MAX_CHAR_NAME_LEN - 2;
    strncpy( GetInteriorManager()->GetBuildBinding()->dialogChar2, argv[2], len );
    GetInteriorManager()->GetBuildBinding()->dialogChar2[len] = '\0';

    GetInteriorManager()->GetBuildBinding()->acceptDialogID = ::radMakeKey32(argv[3]);
    GetInteriorManager()->GetBuildBinding()->instructDialogID = ::radMakeKey32(argv[4]);
    GetInteriorManager()->GetBuildBinding()->rejectDialogID = ::radMakeKey32(argv[5]);
}

void InteriorManager::ConsoleGagCheckMovie(int argc, char** argv)
{
    GetInteriorManager()->GetBuildBinding()->i_S_Movie = 2;

    unsigned int len = strlen(argv[1]) < DialogueObjective::MAX_CHAR_NAME_LEN - 1 ? strlen(argv[1]) : DialogueObjective::MAX_CHAR_NAME_LEN - 2;
    strncpy( GetInteriorManager()->GetBuildBinding()->dialogChar1, argv[1], len );
    GetInteriorManager()->GetBuildBinding()->dialogChar1[len] = '\0';

    len = strlen(argv[2]) < DialogueObjective::MAX_CHAR_NAME_LEN - 1 ? strlen(argv[2]) : DialogueObjective::MAX_CHAR_NAME_LEN - 2;
    strncpy( GetInteriorManager()->GetBuildBinding()->dialogChar2, argv[2], len );
    GetInteriorManager()->GetBuildBinding()->dialogChar2[len] = '\0';

    rAssert( strlen( argv[ 3 ] ) < 13 ); // Enforce 8.3 file naming.
    strcpy( GetInteriorManager()->GetBuildBinding()->gagFMVFileName, argv[ 3 ] );
    GetInteriorManager()->GetBuildBinding()->rejectDialogID = ::radMakeKey32(argv[4]);
}

void InteriorManager::ConsoleGagEnd(int argc, char** argv )
{
    GetInteriorManager()->GagEnd();
}

void InteriorManager::LoadLevelGags(const rmt::Vector& startPos, bool initial)
{
    tUID interior = ClassifyPoint(startPos).GetUID();

    bool needTo = true;

    if(!initial)
    {
        // this will happen if we are trying to reset from one interior into another
        // the mission system sends too many events, so we'll ignore the bogus ones
        if(!((interior == static_cast< tUID >( 0 )) || (interior == mLoadedInteriorUID)))
        {
            return;
        }

        needTo = mCurrentInteriorUID != interior;
    }

    if(needTo)
    {
        mCurrentInteriorUID = interior;

        if(mCurrentInteriorUID)
        {
            LoadGagNIS(mCurrentInteriorUID);
            mState = INSIDE;
            mIn = true;
            mEntryRequested = false;
            GetExitPos();
            SwitchToInterior();
        }
        else
        {
            mState = NONE;
            mEntryRequested = false;
            SwitchToExterior();
            LoadGagNIS(0);
        }

        GetCharacterManager()->GetCharacter(0)->SetPosition(startPos);
        for(int i = 0; i < mGagCount; i++)
        {
            if(gags[i])
            {
                gags[i]->Update(0);
            }
        }
    }
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************


//==============================================================================
// InteriorManager::InteriorManager
//==============================================================================
//
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//============================================================================== 
InteriorManager::InteriorManager()
:
mEntryRequested( false ),
mInteriorLoaded( false ),
mLoadedInteriorUID( 0 ),
mCurrentInteriorUID( 0 ),
mCollectionEffect(0),
mIn (false),
mBindingCount( 0 ),
mGagCount(0),
mBuildingGag(false),
mExit(new InteriorExit),
mInteriorAnimations(NULL),
m_isPlayingISDialog( false )
{
    mExit->AddRef();

    for(int i = 0; i < MAX_GAGS; i++)
    {
        gags[i] = NULL;
    }

    if (!sRandomSeeded)
    {
        sRandom.Seed (Game::GetRandomSeed ());
        sRandomSeeded = true;
    }
}


//==============================================================================
// InteriorManager::~InteriorManager
//==============================================================================
//
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================
InteriorManager::~InteriorManager()
{
    mExit->Release();

    GetEventManager()->RemoveAll(this);

    ClearGags();
}


//==============================================================================
void InteriorManager::SwitchToInterior()
{
    if(mLoadedInteriorUID != tUID(0))
    {
        mCurrentInteriorUID = mLoadedInteriorUID;
    }

    if(mCurrentInteriorUID == tEntity::MakeUID("moe1"))
    {
        static rmt::Matrix mirrorMatrix(-1.0f, 0.0f,  0.0f,  0.0f, 
                                        0.0f, 1.0f,  0.0f,  0.0f, 
                                        0.0f, 0.0f,  1.0f,  0.0f, 
                                        109.5f, 0.0f,  0.0f,  1.0f); 
        WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( RenderEnums::LevelSlot ));
        pWorldRenderLayer->SetMirror(true, &mirrorMatrix);

        Character* moe = GetCharacterManager()->GetCharacterByName("moe");
        if(moe && moe->IsAmbient())
        {
            moe->ResetAmbientPosition();
        }

    } else if(mCurrentInteriorUID == tEntity::MakeUID("bartroom"))
    {
        static rmt::Matrix mirrorMatrix(1.0f, 0.0f,  0.0f,  0.0f, 
                                        0.0f, 1.0f,  0.0f,  0.0f, 
                                        0.0f, 0.0f,  -1.0f,  0.0f, 
                                        0.0f, 0.0f,  -894.0f,  1.0f); 
        WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( RenderEnums::LevelSlot ));
        pWorldRenderLayer->SetMirror(true, &mirrorMatrix);
    }

    SetupLightsAndAnims();

    mIn = true;
 
    GetEventManager()->TriggerEvent(EVENT_INTERIOR_SWITCH, (void*)1);
}

void InteriorManager::SetupLightsAndAnims()
{
    // Setup the lights.
    //
    p3d::inventory->PushSection();
    p3d::inventory->SelectSection(mSection);
    p3d::inventory->SetCurrentSectionOnly(true);
    tLightGroup* pLightGroup = p3d::find<tLightGroup>("InteriorLightGroup");   

    if( !pLightGroup )
    {
        pLightGroup = p3d::find<tLightGroup>("LightGroup");   
    }

    if( pLightGroup )
    {
        RenderLayer* rl = GetRenderManager()->mpLayer(RenderEnums::LevelSlot);
        rAssert(rl);
        tView* view = rl->pView( PLAYER_ONE );
        if(view == 0)
        {
            // We've come in here before the view has been set up by the render manager.
            rl->SetUpViewCam();
            view = rl->pView(PLAYER_ONE);
        }
        rAssert(view);
        view->RemoveAllLights();
            
        for( int i = 0; i < pLightGroup->GetNumLights(); ++i )
        {
            tLight* light = pLightGroup->GetLight( i );
            rAssert( light != NULL );
            if( light != NULL )
            {
                GetRenderManager()->mpLayer(RenderEnums::LevelSlot)->pView( PLAYER_ONE )->AddLight( light );
            }
        }
    }

    tRefCounted::Assign(mInteriorAnimations, p3d::find<tFrameController>("MasterController"));
    p3d::inventory->SetCurrentSectionOnly(false);

    if(mInteriorAnimations)
    {
        mInteriorAnimations->SetCycleMode(FORCE_CYCLIC);
    }
    p3d::inventory->PopSection();
}

//==============================================================================
void InteriorManager::AttemptEntry()
{
    if( mEntryRequested && mInteriorLoaded && !mIn)
    {
        SwitchToInterior();

        //----------------------------------------------------------------------
        // Teleport the character to the interior.
        //----------------------------------------------------------------------

        Character* pCharacter = GetCharacterManager()->GetCharacter( PLAYER_ONE );
        rAssert( pCharacter != 0 );

        p3d::inventory->PushSection();
        p3d::inventory->SelectSection(mSection);
        p3d::inventory->SetCurrentSectionOnly(true);

        // Find the end locator.
        //
        DirectionalLocator* pEndLoc = p3d::find<DirectionalLocator>( "InteriorEntryEnd" );
        rAssert( pEndLoc != 0 );

        p3d::inventory->SetCurrentSectionOnly(false);
        p3d::inventory->PopSection();

        rmt::Matrix endTransform = pEndLoc->GetTransform();
        rmt::Vector standPosition;
        pEndLoc->GetLocation( &standPosition ) ;

        // Put the character in position.
        //
        rmt::Matrix transform = pEndLoc->GetTransform();
        float facingDir = choreo::GetWorldAngle( transform.Row(2).x, transform.Row(2).z );

        pCharacter->RelocateAndReset(standPosition, facingDir);
    }

    if( !mLoadedGags && mEntryRequested && !GetLoadingManager()->IsLoading())
    {
        mLoadedGags = true;
        LoadGagNIS(mLoadedInteriorUID);
    }
    else if( mEntryRequested && mInteriorLoaded && mLoadedGags && !GetLoadingManager()->IsLoading())
    {
        GetRenderManager()->mpLayer(RenderEnums::LevelSlot)->Thaw();

        mEntryRequested = false;

        Character* pCharacter = GetCharacterManager()->GetCharacter( PLAYER_ONE );

        pCharacter->GetActionController()->Clear();
        Sequencer* pSeq = pCharacter->GetActionController()->GetNextSequencer();
        Action* pAction = 0;

        // Wait a while for camera to reorient itself
        pSeq->BeginSequence();
        pSeq->AddAction( new DelayAction(0.5f));
        pSeq->EndSequence();

        // Open Iris
        //
        pSeq->BeginSequence();
        pAction = new TriggerEventAction( EVENT_ENTER_INTERIOR_TRANSITION_END, (void*)pCharacter );
        pSeq->AddAction( pAction );
        pSeq->EndSequence();

        // Finish
        //
        pSeq->BeginSequence();
        pAction = new TriggerEventAction( EVENT_ENTER_INTERIOR_END, (void*)pCharacter );
        pSeq->AddAction( pAction );
        pSeq->EndSequence();

        GetSuperCamManager()->GetSCC( 0 )->SetIsInitialCamera(true);
    }
}


void InteriorManager::SwitchToExterior()
{
    if(!mIn)
        return;

    mCurrentInteriorUID = 0;

    mIn = false;

    // reset the exterior lights 
    //
    tView* view = GetRenderManager()->mpLayer(RenderEnums::LevelSlot)->pView( PLAYER_ONE );
    view->RemoveAllLights();

    p3d::inventory->PushSection();
    p3d::inventory->SelectSection("Default");
    tLightGroup* sun = p3d::find<tLightGroup>("sun");   
    rAssert( sun );
    p3d::inventory->PopSection();

    for(int j=0;j<sun->GetNumLights();j++)
    {
        view->AddLight( sun->GetLight(j) );
    }

    tRefCounted::Release(mInteriorAnimations);

    ClearGags();

    // Show the HUD Map.
    //
//    GetGuiSystem()->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_MAP );


    WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( RenderEnums::LevelSlot ));
    pWorldRenderLayer->SetMirror(false, NULL);

    GetSuperCamManager()->GetSCC( 0 )->SetIsInitialCamera(true);

    GetEventManager()->TriggerEvent(EVENT_INTERIOR_SWITCH, 0);
}

void InteriorManager::ExitInterior()
{
    if(mState != EXIT)
        return;

    SwitchToExterior();
    this->LoadGagNIS(0);

    Character* pCharacter = GetCharacterManager()->GetCharacter( PLAYER_ONE );
    rAssert( pCharacter != 0 );

    // position the character outside
    pCharacter->RelocateAndReset(mExitPos, mExitFacing);

    pCharacter->GetActionController()->Clear();
    Sequencer* pSeq = pCharacter->GetActionController()->GetNextSequencer();

    // Wait a while for camera to reorient itself
    pSeq->BeginSequence();
    pSeq->AddAction( new DelayAction(1.0f));
    pSeq->EndSequence();

    // Open iris, and notify anyone else that might be interested 
    // (ambient sound, etc) that we are outside again
    pSeq->BeginSequence();
    pSeq->AddAction( new TriggerEventAction( EVENT_EXIT_INTERIOR_END, (void*)pCharacter ) );
    pSeq->EndSequence();
}


//==============================================================================
// InteriorManager::LoadGagNIS
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void InteriorManager::LoadGagNIS(tUID interiorUID)
{
    ClearGags();

    int totalWeight = 0;

    // Which gags are available for this interior?
    //
    int i;
    for( i = 0; i < mBindingCount; ++i )
    {
        if( (mGagBindings[i].interiorUID == interiorUID) && mGagBindings[i].random)
        {
            totalWeight += mGagBindings[i].weight;
        }
    }

    int pick = 0;
    if(totalWeight)
    {
        pick = sRandom.IntRanged(0,totalWeight-1);
    }
    totalWeight = 0;

    for( i = 0; i < mBindingCount; ++i )
    {
        if( (mGagBindings[i].interiorUID == interiorUID) && mGagBindings[i].random)
        {
            if((pick >= totalWeight) && (pick < (totalWeight + mGagBindings[i].weight)))
            {
                HeapMgr()->PushHeap(GMA_LEVEL_OTHER);
                rAssert(mGagCount < MAX_GAGS);
                gags[mGagCount] = new Gag(&mGagBindings[i]);
                gags[mGagCount]->AddRef();
                mGagCount++;
                HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
            }
            totalWeight += mGagBindings[i].weight;
        }
        else if( (mGagBindings[i].interiorUID == interiorUID) && !mGagBindings[i].random)
        {
            HeapMgr()->PushHeap(GMA_LEVEL_OTHER);
            rAssert(mGagCount < MAX_GAGS);
            gags[mGagCount] = new Gag(&mGagBindings[i]);
            gags[mGagCount]->AddRef();
            mGagCount++;
            HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
        }
    }
}

void InteriorManager::ClearGags()
{
    for(int i = 0; i < mGagCount; i++)
    {
        tRefCounted::Release(gags[i]);
    }
    mGagCount = 0;
}

void InteriorManager::GetExitPos(void)
{
    // get the position and facing of the exit locator
    rmt::Vector heading;

    Locator* loc = p3d::find<Locator>(mLoadedInteriorUID);
    if(!loc)
    {
        loc = p3d::find<Locator>(mCurrentInteriorUID);
    }
    rAssert(loc);

    loc->GetPosition(&mExitPos);
    loc->GetHeading(&heading);
    mExitFacing = choreo::GetWorldAngle( heading.x, heading.z ) + rmt::PI;
}

void InteriorManager::CollectionEffect(const char* Name, const rmt::Vector& Pos)
{
    mCollectionEffect->Init(Name, Pos, true, true);
}
