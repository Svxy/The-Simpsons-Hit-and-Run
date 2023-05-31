//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        missionobjective.cpp
//
// Description: Implement MissionObjective
//
// History:     15/04/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <string.h>

//========================================
// Project Includes
//========================================

#include <meta/locator.h>
#include <meta/carstartlocator.h>
#include <meta/zoneeventlocator.h>

#include <mission/gameplaymanager.h>
#include <mission/missionmanager.h>
#include <mission/objectives/missionobjective.h>

#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <render/culling/worldscene.h>
#include <render/dsg/inststatentitydsg.h>
#include <render/rendermanager/rendermanager.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/backend/guimanagerbackend.h>

#include <worldsim/character/character.h>
#include <worldsim/character/charactermanager.h>

#include <memory/srrmemory.h>

#include <render/IntersectManager/IntersectManager.h>
#include <worldsim/avatarmanager.h>
#include <roads/roadmanager.h>
#include <roads/intersection.h>
#include <roads/road.h>
#include <Render/DSG/AnimEntityDSG.h>
#include <Render/AnimEntityDSGManager/AnimEntityDSGManager.h>
#include <interiors/interiormanager.h>
#include <mission/charactersheet/charactersheetmanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// MissionObjective::MissionObjective
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
MissionObjective::MissionObjective() :
    mObjType( OBJ_INVALID ),
    mbFinished( false ),
    mVehicle( NULL ),
    mNumNPCs( 0 ),
    mNumRemoveNPCs( 0 ),
    mRockOut( false )
{
    
}

//==============================================================================
// MissionObjective::~MissionObjective
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
MissionObjective::~MissionObjective()
{
    
}

bool MissionObjective::AddNPCWaypoint( const char* npc, const char* location )
{
    rTuneAssert( npc );
    rTuneAssert( location );

    for( unsigned int i = 0; i < mNumNPCs; ++i )
    {
        if( strcmp( mNPCNames[i], npc ) == 0 )
        {
            int numWays = mNPCs[i].numWayLocs;
            rAssert( numWays >= 0 ); 

            // add only if we have room
            if( numWays >= (MAX_NPC_WAYPOINT_LOCATORS - 1) )
            {
#if (RAD_TUNE || RAD_DEBUG)
                char errMsg[ 256 ];
                sprintf( errMsg, "Cannot add more NPC waypoints for %s", npc );
                rTuneAssertMsg( false, errMsg );
#endif
                return false;
            }
            mNPCs[i].wayLocs[ numWays ].SetText( location );
            mNPCs[i].numWayLocs++;

            return true;
        }
    }
    // if we got here, the npc is not in our list.
    return false;
}

//=============================================================================
// MissionObjective::Initalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionObjective::Initialize()
{
#if defined(RAD_DEBUG) || defined(RAD_TUNE)
    char errMsg[ 256 ];
#endif 

    //
    // Trigger an event that this objective is now active
    //
    GetEventManager()->TriggerEvent( EVENT_MISSION_OBJECTIVE_NEW, reinterpret_cast< void* >( this ) );

    mbFinished = false;

    //Move the required NPCs to their new locations and store their old pos/rot.
    unsigned int i;

    for(i = 0; i < mNumRemoveNPCs; i++)
    {
        char name[64];
        if(mRemoveNPCs[i].driver)
        {
            sprintf(name, "d_%s", mRemoveNPCs[i].name);
        }
        else
        {
            strcpy(name, mRemoveNPCs[i].name);
        }

        Character* c = GetCharacterManager()->GetCharacterByName(name);
        if(c)
        {
            if(c->IsAmbient())
            {
                c->ResetAmbientPosition();
            }
            else
            {
                if(mRemoveNPCs[i].driver && c->GetTargetVehicle())
                {
                    c->GetTargetVehicle()->SetDriver(NULL);
                }
                GetCharacterManager()->RemoveCharacter(c);
            }
        }
    }
    
    for ( i = 0; i < mNumNPCs; ++i )
    {
        if(mNPCs[ i ].driver)
        {
            Vehicle* vehicle = GetGameplayManager()->GetMissionVehicleByName(mNPCs[ i ].locator);

            char name[64];
            sprintf(name, "d_%s", mNPCNames[i]);
            Character* character = GetCharacterManager( )->GetCharacterByName(name);

            if(character && (character->GetTargetVehicle() == vehicle))
            {
                continue;
            }

            if(character)
            {
                GetCharacterManager()->RemoveCharacter(character);
            }

            character = GetCharacterManager()->AddCharacter(CharacterManager::NPC, name, mNPCNames[i], "npd", NULL );

            static_cast<NPCController*>(character->GetController())->TransitToState(NPCController::NONE);
            character->SetTargetVehicle( vehicle );
            character->AddToWorldScene();
            character->GetStateManager()->SetState<CharacterAi::InCar>();

            vehicle->SetDriver(character);

            character->SetRole(Character::ROLE_DRIVER);
        }
        else
        {
            // pick an appropriate name based on if we are a bonus mission or not
            bool hadToAdd = false;

            mNPCs[ i ].npc = GetCharacterManager( )->GetMissionCharacter(mNPCNames[i]);

            if( !mNPCs[ i ].npc )
            {
                char name[64];
                hadToAdd = true;
                if(GetGameplayManager()->GetCurrentMission()->IsBonusMission())
                {
                    sprintf(name, "b_%s", mNPCNames[i]);
                }
                else
                {
                    strcpy(name, mNPCNames[i]);
                }
                mNPCs[ i ].npc = GetCharacterManager()->AddCharacter(CharacterManager::NPC, name, mNPCNames[i], "npd", NULL );
                mNPCs[ i ].npc->SetRole(Character::ROLE_MISSION);
            }
            else
            {
                mNPCs[ i ].npc->EnableAmbientDialogue(false);
            }


            rAssert( mNPCs[ i ].npc );

            CarStartLocator* newPos = p3d::find<CarStartLocator>(mNPCs[ i ].locator);

            // TODO : is this neccesary
            mNPCs[ i ].npc->RemoveFromWorldScene();
            mNPCs[ i ].npc->AddToWorldScene();

            GetCharacterManager( )->SetGarbage(mNPCs[ i ].npc, false);

            mNPCs[ i ].npc->AddRef();

            NPCController* controller = (NPCController*) (mNPCs[ i ].npc->GetController());
            rAssert( controller );

            // set position & facing
            // if we have no locator, just leave them where they are
            if(newPos)
            {
                rmt::Vector oldpos, pos, dist;
                newPos->GetLocation(&pos);
                mNPCs[ i ].npc->GetPosition(oldpos);
                dist.Sub(pos, oldpos);

                bool loading = ((GetGuiSystem()->GetBackendManager()->GetCurrentScreen() == CGuiWindow::GUI_SCREEN_ID_LOADING) &&
                                 GetGuiSystem()->GetBackendManager()->GetCurrentWindow()->IsRunning());

                if(hadToAdd || (dist.Magnitude() > 20.0f) || 
                    ((GetGameFlow()->GetCurrentContext() == CONTEXT_LOADING_GAMEPLAY) || loading))
                {
                    mNPCs[ i ].npc->RelocateAndReset( pos, newPos->GetRotation() );

                    if(!hadToAdd)
                    {
                        if(mNPCs[ i ].npc->GetRole() != Character::ROLE_MISSION)
                        {
                            controller->SetTempWaypont(pos);
                        }
                    }
                }

                // add the start locator as the first waypoint
                if((mNPCs[ i ].npc->GetRole() ==  Character::ROLE_MISSION))
                {
                    controller->ClearNPCWaypoints();

                    bool res = controller->AddNPCWaypoint( pos );

#if defined(RAD_DEBUG) || defined(RAD_TUNE)
                    sprintf( errMsg, "Can't add more waypoints for this objective NPC %s", mNPCNames[ i ] );
                    rTuneAssertMsg( res, errMsg );
#endif
                }

            }
            else
            {
    #if defined(RAD_DEBUG) || defined(RAD_TUNE)
                // for existing characters it's okay not to have a locator, 
                // but for new ones, its an error
                sprintf( errMsg, "NPC %s created without valid locator (%s). Check spelling!\n", 
                    mNPCNames[i],  mNPCs[ i ].locator);
                rTuneAssertMsg( false, errMsg );
    #endif
            }

            if(mNPCs[ i ].npc->GetRole() ==  Character::ROLE_MISSION)
            {
                for( int j=0; j<mNPCs[ i ].numWayLocs; j++ )
                {
                    Locator* loc = p3d::find<Locator>( mNPCs[ i ].wayLocs[ j ].GetUID() );
                    if( loc == NULL )
                    {
        #if defined(RAD_DEBUG) || defined(RAD_TUNE)
                        sprintf( errMsg, "NPC Waypoint %s not found for NPC %s", 
                            mNPCs[ i ].wayLocs[ j ].GetText(), mNPCNames[ i ] );
                        rTuneAssertMsg( false, errMsg ); 
        #endif
                        continue;
                    }
                    rmt::Vector locPos;
                    loc->GetPosition( &locPos );

                    // the NPCController is created and assigned to the NPC in 
                    // then NPCharacter constructor. So we should expect there
                    // to be a controller at this point 
                    bool res = controller->AddNPCWaypoint( locPos );

        #if defined(RAD_DEBUG) || defined(RAD_TUNE)
                    sprintf( errMsg, "Can't add more waypoints for this objective NPC %s", mNPCNames[ i ] );
                    rTuneAssertMsg( res, errMsg );
        #endif
                }
            }
        }
    }
    
    GetCharacterManager()->AllowGarbageCollection(true);

    GetCharacterManager()->GetCharacter(0)->SetRockinIdle(mRockOut);
    OnInitialize();
}

//=============================================================================
// MissionObjective::Finalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionObjective::Finalize()
{
    OnFinalize();

    GetCharacterManager()->GetCharacter(0)->SetRockinIdle(false);

    //return all NPCs to their original locations.
    unsigned int i;
    for ( i = 0; i < mNumNPCs; ++i )
    {
        if( mNPCs[ i ].driver)
        {
            continue;
        }

        rAssert( mNPCs[ i ].npc );
        
        // make all NPC's in this mission canidates for garbage colleciton 
        GetCharacterManager( )->SetGarbage(mNPCs[ i ].npc, true);

        /***
        mNPCs[ i ].npc->SetPosition( mNPCs[ i ].oldPos );
        mNPCs[ i ].npc->SetFacingDir( mNPCs[ i ].oldRotation );
        mNPCs[ i ].npc->UpdateTransformToLoco();
        ***/
        mNPCs[ i ].npc->Release();
        mNPCs[ i ].npc = NULL;
    }

    GetCharacterManager()->AllowGarbageCollection(false);
}

//=============================================================================
// MissionObjective::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( float elapsedTime )
//
// Return:      void 
//
//=============================================================================
void MissionObjective::Update( unsigned int elapsedTime )
{
    OnUpdate( elapsedTime );
}

//=============================================================================
// MissionObjective::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void MissionObjective::HandleEvent( EventEnum id, void* pEventData )
{
}


//=============================================================================
// MissionObjective::AddNPC
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name, CarStartLocator* newPos )
//
// Return:      void 
//
//=============================================================================
void MissionObjective::AddNPC( const char* name, const char* loc, bool driver)
{
#if (RAD_TUNE || RAD_DEBUG)
    char errMsg[ 128 ];

    rTuneAssertMsg( name, "An NPC's name can't be null!" );

    sprintf( errMsg, "Locator for NPC %s cannot be null!", name );
    rTuneAssertMsg( loc, errMsg );

    sprintf( errMsg, "Can't add more NPCs. Already reached max count of %d.", mNumNPCs );
    rTuneAssertMsg( mNumNPCs < (MAX_NPCS - 1), errMsg );
#endif

    //Add the NPC name to the list.
    unsigned int len = strlen(name) < MAX_NPC_NAME_LENGTH - 1 ? strlen(name) : MAX_NPC_NAME_LENGTH - 2;
    strncpy( mNPCNames[mNumNPCs], name, len );
    mNPCNames[mNumNPCs][len] = '\0';

    strcpy(mNPCs[ mNumNPCs ].locator, loc);  

    // NOTE: Don't need to add the char start locator to list of waypoints here
    //       because when AddCharacter is called on this NPC, the start locator
    //       will be automatically added first in the list of NPC waypoints
    mNPCs[ mNumNPCs ].numWayLocs = 0;

    mNPCs[ mNumNPCs ].driver = driver;

    ++mNumNPCs;
}

void MissionObjective::RemoveNPC( const char* npc, bool driver)
{
    rAssert(mNumRemoveNPCs < MAX_NPCS);
    strcpy(mRemoveNPCs[mNumRemoveNPCs].name, npc);
    mRemoveNPCs[mNumRemoveNPCs].driver = driver;
    mNumRemoveNPCs++;
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// MissionObjective::RegisterLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ( Locator* locator, int& hudIndex, bool primary, HudMapIcon::eIconType icon )
//
// Return:      void 
//
//=============================================================================
void MissionObjective::RegisterLocator( Locator* locator, 
                                        int& hudIndex, 
                                        bool primary, 
                                        HudMapIcon::eIconType icon )
{
    rmt::Vector destLoc;
    locator->GetLocation( &destLoc );

    RegisterPosition( destLoc, hudIndex, primary, icon, locator );
}

//=============================================================================
// MissionObjective::RegisterPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& pos, int& hudIndex, bool primary, HudMapIcon::eIconType icon )
//
// Return:      void 
//
//=============================================================================
void MissionObjective::RegisterPosition( const rmt::Vector& pos, 
                                         int& hudIndex, 
                                         bool primary, 
                                         HudMapIcon::eIconType icon,
                                         IHudMapIconLocator* hml )
{
    CGuiScreenMultiHud* currentHud = GetCurrentHud();
    if( currentHud )
    {
        hudIndex = currentHud->GetHudMap( 0 )->RegisterIcon( icon, pos, hml, primary );
    }
}

//=============================================================================
// MissionObjective::UnregisterLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ( int hudIndex )
//
// Return:      void 
//
//=============================================================================
void MissionObjective::UnregisterLocator( int& hudIndex )
{
    // unregister hud map icon
    //
    if( hudIndex != -1 )
    {
        CGuiScreenMultiHud* currentHud = GetCurrentHud();
        if( currentHud != NULL )
        {
            currentHud->GetHudMap( 0 )->UnregisterIcon( hudIndex );
        }
        
        hudIndex = -1;
    }
}

//=============================================================================
// MissionObjective::AddEntity
//=============================================================================
// Description: Comment
//
// Parameters:  ( char* entityname )
//
// Return:      InstStatEntityDSG* 
//
//=============================================================================
InstStatEntityDSG* MissionObjective::AddEntity( char* entityname, rmt::Vector pos )
{
MEMTRACK_PUSH_GROUP( "Mission - Entities" );
	tDrawable* pGeometry = p3d::find<tDrawable>( entityname );
	rAssert( pGeometry != NULL );

	InstStatEntityDSG* pEnt = 
		new( GetGameplayManager()->GetCurrentMissionHeap() ) 
		InstStatEntityDSG;

	rAssert( pEnt != NULL );

	//
	// The entity takes overship of m, so spake Devin
	//
	rmt::Matrix* m = new( GetGameplayManager()->GetCurrentMissionHeap() ) rmt::Matrix;

    m->Identity();
	m->FillTranslate( pos );

	pEnt->LoadSetUp( m, pGeometry );

MEMTRACK_POP_GROUP("Mission - Entities");
	return pEnt;
}

//=============================================================================
// MissionObjective::DrawEntity
//=============================================================================
// Description: Comment
//
// Parameters:  ( InstStatEntityDSG* entity )
//
// Return:      void 
//
//=============================================================================
void MissionObjective::DrawEntity( InstStatEntityDSG* entity )
{
	rAssert( entity != NULL );
	GetRenderManager()->pWorldScene()->Add( entity );
}

//=============================================================================
// MissionObjective::HideEntity
//=============================================================================
// Description: Comment
//
// Parameters:  ( InstStatEntityDSG* entity )
//
// Return:      void 
//
//=============================================================================
void MissionObjective::HideEntity( InstStatEntityDSG* entity )
{
	rAssert( entity != NULL );
	GetRenderManager()->pWorldScene()->Remove( entity );
}

//=============================================================================
// MissionObjective::UnregisterPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( int& hudIndex )
//
// Return:      void 
//
//=============================================================================
void MissionObjective::UnregisterPosition( int& hudIndex )
{
    UnregisterLocator( hudIndex );
}

//=============================================================================
// MissionObjective::ChangeIconType
//=============================================================================
// Description: Comment
//
// Parameters:  ( int hudIndex, HudMapIcon::eIconType type )
//
// Return:      void 
//
//=============================================================================
int MissionObjective::ChangeIconType( int hudIndex, HudMapIcon::eIconType type )
{
    if( hudIndex != -1 )
    {
        CGuiScreenMultiHud* currentHud = GetCurrentHud();
        if( currentHud != NULL )
        {
            return( currentHud->GetHudMap( 0 )->ChangeIconType( hudIndex, type ) );
        }
    }

    return -1;
}

//=============================================================================
// MissionObjective::GetObjectiveType
//=============================================================================
// Description: return what type of objective this is
//
// Parameters:  none
//
// Return:      ObjectiveTypeEnum - the type of this objective 
//
//=============================================================================
MissionObjective::ObjectiveTypeEnum MissionObjective::GetObjectiveType() const
{
    return mObjType;
}

//=============================================================================
// MissionObjective::SetObjectiveType
//=============================================================================
// Description: set the type of the objective
//
// Parameters:  objType - the type of objective we're setting
//
// Return:      NONE
//
//=============================================================================
void MissionObjective::SetObjectiveType( const ObjectiveTypeEnum objType ) 
{ 
    mObjType = objType; 
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void MissionObjective::UnlightPath( SwapArray<RoadManager::PathElement>& orPathRoute )
{
//    rReleasePrintf("UnLightPath. 0x%x\n", orPathRoute.mpData );
    int i;
    for(i=orPathRoute.mUseSize-1; i>-1; i-- )
    {
        if( orPathRoute[i].type == RoadManager::ET_INTERSECTION )
        {
            ((Intersection*)(orPathRoute[i].elem))->AnimEntity(0)->SetVisibility(false);
            ((Intersection*)(orPathRoute[i].elem))->AnimEntity(1)->SetVisibility(false);
        }
    }
    for(i=GetAnimEntityDSGManager()->mpFloatingRightWayArrows.mUseSize-1; i>-1; i--) 
    {
        GetAnimEntityDSGManager()->mpFloatingRightWayArrows[i]->SetVisibility(false);
    }
    for(i=GetAnimEntityDSGManager()->mpFloatingWrongWayArrows.mUseSize-1; i>-1; i--) 
    {
        GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[i]->SetVisibility(false);
    }

    orPathRoute.ClearUse();
    GetAnimEntityDSGManager()->mpFloatingRightWayArrows[0]->SetVisibility(false);
    GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[0]->SetVisibility(false);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void MissionObjective::LightCurrentPath(SwapArray<RoadManager::PathElement>& orPathRoute )
{
    int i;
    for(i=orPathRoute.mUseSize-1; i>-1; i-- )
    {
        if( orPathRoute[i].type == RoadManager::ET_INTERSECTION )
        {
            ((Intersection*)(orPathRoute[i].elem))->AnimEntity(0)->SetVisibility(true);
            ((Intersection*)(orPathRoute[i].elem))->AnimEntity(1)->SetVisibility(false);
        }
    }
    for(i=GetAnimEntityDSGManager()->mpFloatingRightWayArrows.mUseSize-1; i>-1; i--) 
    {
        GetAnimEntityDSGManager()->mpFloatingRightWayArrows[i]->SetVisibility(false);
    }
    for(i=GetAnimEntityDSGManager()->mpFloatingWrongWayArrows.mUseSize-1; i>-1; i--) 
    {
        GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[i]->SetVisibility(false);
    }

    GetAnimEntityDSGManager()->mpFloatingRightWayArrows[0]->SetVisibility(true);
    GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[0]->SetVisibility(false);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void MissionObjective::UnlightCurrentPath(SwapArray<RoadManager::PathElement>& orPathRoute )
{
    int i;
    for(i=orPathRoute.mUseSize-1; i>-1; i-- )
    {
        if( orPathRoute[i].type == RoadManager::ET_INTERSECTION )
        {
            ((Intersection*)(orPathRoute[i].elem))->AnimEntity(0)->SetVisibility(false);
            ((Intersection*)(orPathRoute[i].elem))->AnimEntity(1)->SetVisibility(false);
        }
    }
    for(i=GetAnimEntityDSGManager()->mpFloatingRightWayArrows.mUseSize-1; i>-1; i--) 
    {
        GetAnimEntityDSGManager()->mpFloatingRightWayArrows[i]->SetVisibility(false);
    }
    for(i=GetAnimEntityDSGManager()->mpFloatingWrongWayArrows.mUseSize-1; i>-1; i--) 
    {
        GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[i]->SetVisibility(false);
    }

    GetAnimEntityDSGManager()->mpFloatingRightWayArrows[0]->SetVisibility(false);
    GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[0]->SetVisibility(false);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void MissionObjective::LightPathRightWay( SwapArray<RoadManager::PathElement>& orPathRoute, int iStart, int iPastEnd )
{
    int i;
    if( DirectionalArrowEnum::INTERSECTION & mArrowType )
    {
        iStart--;
        for( i=iPastEnd-1; i>iStart; i-- )
        {
            if( orPathRoute[i].type == RoadManager::ET_INTERSECTION )
            {
                ((Intersection*)(orPathRoute[i].elem))->AnimEntity(0)->SetVisibility(true);
                ((Intersection*)(orPathRoute[i].elem))->AnimEntity(1)->SetVisibility(false);
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void MissionObjective::LightPathWrongWay( SwapArray<RoadManager::PathElement>& orPathRoute, int iStart, int iPastEnd )
{
    int i;

    if( DirectionalArrowEnum::INTERSECTION & mArrowType )
    {
        iStart--;
        for(i=iPastEnd; i>iStart; i-- )
        {
            if( orPathRoute[i].type == RoadManager::ET_INTERSECTION )
            {
                ((Intersection*)(orPathRoute[i].elem))->AnimEntity(0)->SetVisibility(false);
                ((Intersection*)(orPathRoute[i].elem))->AnimEntity(1)->SetVisibility(true);
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void MissionObjective::UpdateLightPath( PathStruct& orPathStruct)
{
    SwapArray<RoadManager::PathElement>& orPathRoute = orPathStruct.mPathRoute;
    if(!GetCharacterSheetManager()->QueryNavSystemSetting())
    {
        UnlightCurrentPath(orPathRoute);
        return;
    }
    else
    {
        //LightCurrentPath(orPathRoute);
    }

    orPathStruct.mUpdateCount = (orPathStruct.mUpdateCount+1)%orPathStruct.msUpdateModulo;
    if( orPathStruct.mUpdateCount == 0 )
        UpdateGroundHeights(orPathRoute);

    UpdateLongRoadArrows(orPathStruct);

    rmt::Vector charPosn, charFacing, iconPosn, iconFacing, tempVect, tempVect2;
    GetAvatarManager()->GetAvatarForPlayer(0)->GetPosition(charPosn);
    GetAvatarManager()->GetAvatarForPlayer(0)->GetHeading(charFacing);

    RoadManager::PathElement  lastPathElem;
    RoadSegment*              pLastRoadSegment;
    float                     lastSegmentT;
    float                     lastRoadT;
    int                       playerOnPathPosn;
    float                     distPlayerFromRoad;
   

    GetAvatarManager()->GetAvatarForPlayer(0)->GetLastPathInfo( lastPathElem, 
                                                                pLastRoadSegment,
                                                                lastSegmentT,
                                                                lastRoadT );
    if(lastPathElem.type == RoadManager::ET_INTERSECTION)
    {
        playerOnPathPosn = FindIntersectionInPath( orPathRoute, (Intersection*)lastPathElem.elem );
    }
    else
    {
        playerOnPathPosn = FindRoadInPath( orPathRoute, (Road*)lastPathElem.elem );

        if(playerOnPathPosn>0)
        {
            ((Intersection*)(orPathRoute[playerOnPathPosn-1].elem))->GetLocation(tempVect);
            tempVect2.Sub( charPosn, tempVect );
            distPlayerFromRoad = tempVect2.MagnitudeSqr(); //ensure it's out of view first, if we're setting it to red/wrong way
            if(distPlayerFromRoad<120.0f)
                playerOnPathPosn--;
        }
    }

    if(playerOnPathPosn!=-1)
    {
        LightPathWrongWay(orPathRoute, 0,                playerOnPathPosn);
        LightPathRightWay(orPathRoute, playerOnPathPosn, orPathRoute.mUseSize);
    }

/*
    if(FindNearestRoadIconAhead(orPathRoute, charPosn, iconPosn, iconFacing))
    {
        charFacing.Normalize();
        iconFacing.Normalize(); //if two per frame kills us, we're already dead.

        if(iconFacing.Dot(charFacing)>-0.3f)
        {
            LightPathRightWay(orPathRoute);
        }
        else
        {
            LightPathWrongWay(orPathRoute);
        }
        
    }
    else
    {
//        rAssert(false);
    }
    */
}
//========================================================================
// missionobjective::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
int MissionObjective::FindIntersectionInPath(SwapArray<RoadManager::PathElement>& irPath, Intersection* ipIntToFind)
{
    //
    // Determine whether the players on the path that we want to light.
    //
    for(int i=irPath.mUseSize-1; i>-1; i--)
    {
        if(irPath[i].elem == ipIntToFind)
            return i;
    }
    return -1;
}
//========================================================================
// missionobjective::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
int MissionObjective::FindRoadInPath(SwapArray<RoadManager::PathElement>& irPath, Road* ipRoadToFind)
{
    //
    // Determine whether the players on the path that we want to light.
    //
    for(int i=irPath.mUseSize-1; i>-1; i--)
    {
        if(irPath[i].type == RoadManager::ET_INTERSECTION)
            continue;
        if( (((Road*)(irPath[i].elem))->GetSourceIntersection()         ==  ipRoadToFind->GetSourceIntersection() 
            && ((Road*)(irPath[i].elem))->GetDestinationIntersection()  ==  ipRoadToFind->GetDestinationIntersection())
            ||
            (((Road*)(irPath[i].elem))->GetSourceIntersection()         == ipRoadToFind->GetDestinationIntersection()
            && ((Road*)(irPath[i].elem))->GetDestinationIntersection()  == ipRoadToFind->GetSourceIntersection())
            ) //found matching path
        {
            return i;
        }
    }
    return -1;
}
//////////////////////////////////////////////////////////////////////////
// MissionObjective::UpdateGroundHeights
//
// This function is neccessary because not all ground intersects are 
// loaded at any given time, due to memory considerations
// 
// Should be called sparingly, since it needs to do ground intersects
// Solution: To be Amortised internally
//////////////////////////////////////////////////////////////////////////
void MissionObjective::UpdateGroundHeights( SwapArray<RoadManager::PathElement>& orPathRoute )
{
    for(int i=orPathRoute.mUseSize-1; i>-1; i--)
    {
        if( orPathRoute[i].type == RoadManager::ET_INTERSECTION )
        {
            ((Intersection*)(orPathRoute[i].elem))->AnimEntity(0)->PlaceOnGround(0.2f, false);            
            ((Intersection*)(orPathRoute[i].elem))->AnimEntity(1)->PlaceOnGround(0.2f, false);            
        }
    }

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void MissionObjective::UpdateLongRoadArrows( PathStruct& orPathStruct )
{
    SwapArray<RoadManager::PathElement>& orPathRoute  = orPathStruct.mPathRoute;

    if( !(DirectionalArrowEnum::INTERSECTION & mArrowType) )
        return;

    //This is a temporary hack to prevent crashes, when the list is size 1
    // it will be removed as soon as this system is cleaned up to follow paths
    // arbitrarily instead of at the quantised road/intersection level.
    if( orPathRoute.mUseSize<2 )
        return;

    RoadManager::PathElement  lastPathElem;
    RoadSegment*             pLastRoadSegment;
    float                     lastSegmentT;
    float                     lastRoadT;
    rmt::Vector               playerPosn;

    GetAvatarManager()->GetAvatarForPlayer(0)->GetLastPathInfo( lastPathElem, 
                                                                pLastRoadSegment,
                                                                lastSegmentT,
                                                                lastRoadT );

    GetAvatarManager()->GetAvatarForPlayer(0)->GetPosition(playerPosn);
    if(lastPathElem.type == RoadManager::ET_INTERSECTION)
    {
        return;
    }

    //
    // Determine whether the players on the path that we want to light.
    //
    for(int i=orPathRoute.mUseSize-1; i>-1; i--)
    {
        if(orPathRoute[i].type == RoadManager::ET_INTERSECTION)
            continue;
        if( (((Road*)(orPathRoute[i].elem))->GetSourceIntersection() == ((Road*)(lastPathElem.elem))->GetSourceIntersection() 
            && ((Road*)(orPathRoute[i].elem))->GetDestinationIntersection() == ((Road*)(lastPathElem.elem))->GetDestinationIntersection())
            ||
            (((Road*)(orPathRoute[i].elem))->GetSourceIntersection() == ((Road*)(lastPathElem.elem))->GetDestinationIntersection()
            && ((Road*)(orPathRoute[i].elem))->GetDestinationIntersection() == ((Road*)(lastPathElem.elem))->GetSourceIntersection())
            ) //found matching path
        {

            Road* pRoad = ((Road*)(orPathRoute[i].elem));
            float roadLen = pRoad->GetRoadLength();

            int numArrowsToPlace = static_cast< int >( ( roadLen / 40.0f ) - 1.0f );

            if(numArrowsToPlace>5) numArrowsToPlace=5;
            if(numArrowsToPlace<0) numArrowsToPlace=0;

            RoadSegment* pRoadSegmentPlacement = NULL;
            RoadSegment* pRoadSegReference = NULL;
            rmt::Vector  pathHeading, roadSegPlacementFacing, pathPosn, roadStartPosn, roadEndPosn, tempVect, tempVect2;
            float distStartToNextRoad, distEndToNextRoad, scaleFactor, distPlayerFromRoad;

            RoadSegment* pRoadStart  = pRoad->GetRoadSegment(0);
            RoadSegment* pRoadEnd    = pRoad->GetRoadSegment(pRoad->GetNumRoadSegments()-1);

            int playerRoadSegIndex = 0;
            pLastRoadSegment->GetPosition(&tempVect);
            RoadManager::FindClosestPointOnRoad( pRoad, tempVect, tempVect2, distPlayerFromRoad, playerRoadSegIndex );
            

            //
            //Determine if our path lands on a road heading the wrong direction
            //If so, we need to invert arrow headings
            //
            pRoadEnd->GetPosition(&roadEndPosn);
            pRoadStart->GetPosition(&roadStartPosn);

            //
            // Yay! Another Special Case
            //
            if(pRoadEnd==pRoadStart)
            {
                pRoadStart->GetCorner(0,roadStartPosn);
                pRoadStart->GetCorner(1,roadEndPosn);
            }

            if(i>0)
            {
                rAssert(orPathRoute[i-1].type == RoadManager::ET_INTERSECTION);
                ((Intersection*)(orPathRoute[i-1].elem))->GetLocation(tempVect2);

                //invert distances, since we're using the previous intersection in this case
                tempVect.Sub(tempVect2, roadEndPosn);
                distStartToNextRoad = tempVect.MagnitudeSqr();
                tempVect.Sub(tempVect2, roadStartPosn);
                distEndToNextRoad = tempVect.MagnitudeSqr();
            }
            else
            {
                if(orPathRoute.mUseSize==1) //return for now, since there's only one road, and we don't have origin, destination info
                    return;

                rAssert(orPathRoute[i+1].type == RoadManager::ET_INTERSECTION);
                ((Intersection*)(orPathRoute[i+1].elem))->GetLocation(tempVect2);

                tempVect.Sub(tempVect2, roadEndPosn);
                distEndToNextRoad = tempVect.MagnitudeSqr();
                tempVect.Sub(tempVect2, roadStartPosn);
                distStartToNextRoad = tempVect.MagnitudeSqr();
            }

            if(distStartToNextRoad<distEndToNextRoad)
                scaleFactor = -1.0f;
            else
                scaleFactor = 1.0f;

            /*
            if(i>0) //if we have an intersection previous
            {
                rAssert(orPathRoute[i-1].type == RoadManager::ET_INTERSECTION);

                ((Intersection*)(orPathRoute[i-1].elem))->AnimEntity(0)->GetAnimRootHeading(pathHeading);
                ((Intersection*)(orPathRoute[i-1].elem))->AnimEntity(0)->GetPosition(&pathPosn);
            }
            else
            {
                GetAnimEntityDSGManager()->mpFloatingRightWayArrows[0]->GetAnimRootHeading(pathHeading);
                GetAnimEntityDSGManager()->mpFloatingRightWayArrows[0]->GetPosition(&pathPosn);
            }

            //
            // Find out whic part of the road we're closer to, so that we can use that
            // piece of road for directional reference
            //
            pRoadStart->GetPosition(0.0f,0.0f,&roadStartPosn);
            pRoadEnd->GetPosition(  0.0f,0.0f,&roadEndPosn);
            
            tempVect.Sub(roadStartPosn,pathPosn);
            distToStart = tempVect.MagnitudeSqr();

            tempVect.Sub(roadEndPosn,pathPosn);
            distToEnd   = tempVect.MagnitudeSqr();

            if(distToStart<distToEnd)
                pRoadSegReference = (RoadSegment*)pRoadStart;                
            else
                pRoadSegReference = (RoadSegment*)pRoadEnd;                

            pRoadSegReference->GetEdgeNormal(0,tempVect);
            if( tempVect.Dot(pathHeading)<0.0f )
                scaleFactor = -1.0f;
            else
                scaleFactor = 1.0f;
            */
            for(int j=5; j>0; j--)
            {
                if(j<=numArrowsToPlace)
                {
                    float curDistPlacement = j*40.0f;
                    float realDistPlacement = 0.0f;
                    
                    pRoad->FindRoadSegmentAtDist(curDistPlacement, &pRoadSegmentPlacement);

                    //if we're at the end of the path, cut arrows off past the goal
                    if(pRoad == orPathStruct.mpTargetRoadSeg->GetRoad() ) 
                    {
                        if( scaleFactor == 1.0f ) //if we're going forward on the road
                        {
                            if( pRoadSegmentPlacement->GetSegmentIndex() > orPathStruct.mpTargetRoadSeg->GetSegmentIndex() )
                                continue;
                        }
                        else // we're going backward on the road
                        {
                            if( pRoadSegmentPlacement->GetSegmentIndex() < orPathStruct.mpTargetRoadSeg->GetSegmentIndex() )
                                continue;
                        }
                    }

                    pRoadSegmentPlacement->GetPosition(0.0f, 0.0f, &tempVect);

                    UpdateAnimPosition(GetAnimEntityDSGManager()->mpFloatingRightWayArrows[j], tempVect);
                    UpdateAnimPosition(GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[j], tempVect);

                    tempVect2.Sub( playerPosn, tempVect );
                    distPlayerFromRoad = tempVect2.MagnitudeSqr(); //ensure it's out of view first
                    if( distPlayerFromRoad > 120.0f &&
                        (    scaleFactor == 1.0f  && ((int)pRoadSegmentPlacement->GetSegmentIndex()) < playerRoadSegIndex
                         ||  scaleFactor == -1.0f && ((int)pRoadSegmentPlacement->GetSegmentIndex()) > playerRoadSegIndex) 
                        )
                    {
                        //we're lighting a road piece further away from the goal than the user
                        GetAnimEntityDSGManager()->mpFloatingRightWayArrows[j]->SetVisibility(false);
                        GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[j]->SetVisibility(true);
                    }
                    else
                    {
                        //we're lighting a road piece further toward the goal than the user
                        GetAnimEntityDSGManager()->mpFloatingRightWayArrows[j]->SetVisibility(true);
                        GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[j]->SetVisibility(false);
                    }

                    //GetHeadingAlongPath(orPathRoute, i, (RoadSegment*)pRoadSegmentPlacement, tempVect);

                    //pRoadSegmentPlacement->GetPosition(0.0f, 0.0f, &tempVect2);
                    //tempVect.Sub( tempVect, tempVect2 );

                    pRoadSegmentPlacement->GetEdgeNormal(0,tempVect);

                    tempVect.Scale(scaleFactor);

                    GetAnimEntityDSGManager()->mpFloatingRightWayArrows[j]->SetAnimRootHeadingYUp( tempVect );
                    GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[j]->SetAnimRootHeadingYUp( tempVect );
                    /*

                    if( pRoadSegmentPlacement != NULL )
                    {
                        pRoadSegmentPlacement->GetPosition(0.0f, 0.0f, &tempVect);

                        UpdateAnimPosition(GetAnimEntityDSGManager()->mpFloatingRightWayArrows[j], tempVect);
                        UpdateAnimPosition(GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[j], tempVect);

                        GetAnimEntityDSGManager()->mpFloatingRightWayArrows[j]->SetVisibility(true);
                        GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[j]->SetVisibility(false);

                        pRoadSegmentPlacement->GetEdgeNormal(0,tempVect);
                        tempVect.Scale(scaleFactor);

                        GetAnimEntityDSGManager()->mpFloatingRightWayArrows[j]->SetAnimRootHeadingYUp( tempVect );
                        GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[j]->SetAnimRootHeadingYUp( tempVect );
                    }
                    else
                    {
                        rAssert(false);
                    }
                    */
                }
                else
                {
                    tempVect.Set(0.0f,0.0f,0.0f);

                    UpdateAnimPosition(GetAnimEntityDSGManager()->mpFloatingRightWayArrows[j], tempVect);
                    UpdateAnimPosition(GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[j], tempVect);

                    GetAnimEntityDSGManager()->mpFloatingRightWayArrows[j]->SetVisibility(false);
                    GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[j]->SetVisibility(false);
                }
            }


            i=-1;
        }
    }

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool MissionObjective::FindNearestRoadIconAhead
( 
    SwapArray<RoadManager::PathElement>& orPathRoute, 
    rmt::Vector&                         irPosn, 
    rmt::Vector&                         orIconPosn,
    rmt::Vector&                         orIconFacing
)
{
    float nearestInter = 36000000.0f;//6000.0f*6000.0f;
    float temp;
    rmt::Vector tempPosn;
    bool retVal = false;
    int i;

    if( DirectionalArrowEnum::INTERSECTION & mArrowType )
    {
        //
        // Go through the intersections in the path, finding the nearest
        //
        for(i=orPathRoute.mUseSize-1; i>-1; i--)
        {
            if( orPathRoute[i].type == RoadManager::ET_INTERSECTION )
            {
                ((Intersection*)(orPathRoute[i].elem))->GetLocation(orIconPosn);
                tempPosn.Sub(orIconPosn,irPosn);
                
                temp = tempPosn.MagnitudeSqr();

                if( temp < nearestInter )
                {
                    nearestInter = temp;
                    ((Intersection*)(orPathRoute[i].elem))->AnimEntity(0)->GetAnimRootHeading(orIconFacing);            
                }
            }
        }

        //
        // Go through the active right way markers, finding the nearest
        //
        for(i=GetAnimEntityDSGManager()->mpFloatingRightWayArrows.mUseSize-1; i>0; i--)
        {
            if(!GetAnimEntityDSGManager()->mpFloatingRightWayArrows[i]->GetVisibility())
                continue;
            GetAnimEntityDSGManager()->mpFloatingRightWayArrows[i]->GetPosition(&orIconPosn);
            tempPosn.Sub(orIconPosn,irPosn);
            
            temp = tempPosn.MagnitudeSqr();

            if( temp < nearestInter )
            {
                nearestInter = temp;
                GetAnimEntityDSGManager()->mpFloatingRightWayArrows[i]->GetAnimRootHeading(orIconFacing);            
            }
        }
        //
        // Go through the active wrong way markers, finding the nearest
        //
        for(i=GetAnimEntityDSGManager()->mpFloatingWrongWayArrows.mUseSize-1; i>0; i--)
        {
            if(!GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[i]->GetVisibility())
                continue;
            GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[i]->GetPosition(&orIconPosn);
            tempPosn.Sub(orIconPosn,irPosn);
            
            temp = tempPosn.MagnitudeSqr();

            if( temp < nearestInter )
            {
                nearestInter = temp;
                GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[i]->GetAnimRootHeading(orIconFacing);            
            }
        } 
    }

    if( DirectionalArrowEnum::NEAREST_ROAD & mArrowType )
    {
        GetAnimEntityDSGManager()->mpFloatingRightWayArrows[0]->GetPosition(&orIconPosn);
        tempPosn.Sub(orIconPosn,irPosn);
        
        temp = tempPosn.MagnitudeSqr();

        if( temp < nearestInter )
        {
            nearestInter = temp;
            GetAnimEntityDSGManager()->mpFloatingRightWayArrows[0]->GetAnimRootHeading(orIconFacing);            
        }
    }

    if(nearestInter<36000000.0f)
        retVal = true;

    return retVal;
}
//////////////////////////////////////////////////////////////////////////
void MissionObjective::LightPath
( 
    rmt::Vector& irTargetPosn, 
    PathStruct& orPathStruct
)
{
    if( DirectionalArrowEnum::NEITHER & mArrowType )
        return;

    orPathStruct.mUpdateCount=0; //initialize the update for modulo updates (so we dont update arrows on ground height per frame)
    SwapArray<RoadManager::PathElement>& orPathRoute = orPathStruct.mPathRoute;
//    rReleasePrintf("LightPath. 0x%x\n", orPathRoute.mpData );
    RoadSegment* pSourceRoadSeg = NULL;
    RoadSegment* pTargetRoadSeg = NULL;
    rmt::Vector  sourcePosn, targetPosn, closestPosn;
    float SourceTVal, TargetTVal;

    if( orPathRoute.mUseSize != 0 )
    {
        //this shouldn't happen but we are going to ignore it.
        rTuneAssert(0);
        return;
    }

    if( GetInteriorManager()->IsInside() )
    {
        //Check for an all-interior mission, in which case,
        //forget lighting entirely
        GetAvatarManager()->GetAvatarForPlayer(0)->GetPosition(sourcePosn);
        sourcePosn.Sub(sourcePosn,irTargetPosn);

        //I'm assured that the interiors ar <100m in size and >=200m from the rest of the world
        if(sourcePosn.MagnitudeSqr()< 10000.0f) 
            return;

        GetInteriorManager()->ExteriorCharPosn(sourcePosn);

    }
    else
    {
        GetAvatarManager()->GetAvatarForPlayer(0)->GetPosition(sourcePosn);
    }

    //////////////////////////////////////////////////////////////////////////
    // If Target is in an interior, and source is not, we want to get 
    // the target's exterior posn
    //////////////////////////////////////////////////////////////////////////
    tName interiorName = GetInteriorManager()->ClassifyPoint( irTargetPosn );

    float dummy;

    if(interiorName.GetUID()!= static_cast< tUID >( 0 ) )
    {
        ZoneEventLocator* pZEL = p3d::find<ZoneEventLocator>(interiorName.GetUID());
        pZEL->GetPosition(&targetPosn);
        GetIntersectManager()->FindClosestRoad(targetPosn, 50.0f, pTargetRoadSeg, dummy );
    }
    else
    {
        targetPosn = irTargetPosn;
    }

    GetIntersectManager()->FindClosestRoad(targetPosn, 50.0f, pTargetRoadSeg, dummy );
    rAssert( pTargetRoadSeg != NULL );

    GetIntersectManager()->FindClosestRoad(sourcePosn, 50.0f, pSourceRoadSeg, dummy );
    rAssert( pSourceRoadSeg != NULL );

    RoadManager::PathElement sourcePath, targetPath;

    if( pSourceRoadSeg != NULL )    sourcePath.elem = pSourceRoadSeg->GetRoad();
    sourcePath.type = RoadManager::ET_NORMALROAD;

    if( pTargetRoadSeg != NULL )    targetPath.elem = pTargetRoadSeg->GetRoad();
    targetPath.type = RoadManager::ET_NORMALROAD;

    //////////////////////////////////////////////////////////////////////////
    // Determine Source and Target T Values
    //////////////////////////////////////////////////////////////////////////
    RoadManager* pRoadManager = RoadManager::GetInstance();

    float closestDist;
    int closestSegIndex;
    pRoadManager->FindClosestPointOnRoad( (Road*)sourcePath.elem, sourcePosn, 
                                          closestPosn, closestDist, closestSegIndex );

    SourceTVal = pRoadManager->DetermineRoadT( pSourceRoadSeg, pRoadManager->DetermineSegmentT( closestPosn, pSourceRoadSeg ) );

    pRoadManager->FindClosestPointOnRoad( (Road*)targetPath.elem, targetPosn, 
                                          closestPosn, closestDist, closestSegIndex );
    
    TargetTVal = pRoadManager->DetermineRoadT( pTargetRoadSeg, pRoadManager->DetermineSegmentT( closestPosn, pTargetRoadSeg ) );

    //////////////////////////////////////////////////////////////////////////
    // Find Path
    //////////////////////////////////////////////////////////////////////////
    if( pTargetRoadSeg != NULL && pSourceRoadSeg != NULL )
    {
        RoadManager::GetInstance()->FindPathElementsBetween( 
            false,
            sourcePath, SourceTVal, sourcePosn,
            targetPath, TargetTVal, targetPosn,
            orPathRoute );
    }
    else
    {
        rAssert(false);
        //we should always have nav info for this 
    }

#ifdef RAD_DEBUG
    pSourceRoadSeg->GetPosition( &mPathStart );
    pTargetRoadSeg->GetPosition( &mPathEnd );
#endif
    orPathStruct.mpTargetRoadSeg    = pTargetRoadSeg;
    orPathStruct.mTargetPosn        = targetPosn;

    AnimEntityDSG* pAnimEDSG;
    AnimEntityDSG* pWrongwayAnimDSG;
    rmt::Vector pathHeading;
    rmt::Vector pathTemp;
    int i;

    if( DirectionalArrowEnum::INTERSECTION & mArrowType )
    {
        for( i=orPathRoute.mUseSize-1; i>-1; i-- )
        {
            //Set visibility and align arrows for path highlighting
            if( orPathRoute[i].type == RoadManager::ET_INTERSECTION )
            {
                ((Intersection*)(orPathRoute[i].elem))->GetLocation(sourcePosn);
                pWrongwayAnimDSG = ((Intersection*)(orPathRoute[i].elem))->AnimEntity(1);
                pAnimEDSG = ((Intersection*)(orPathRoute[i].elem))->AnimEntity(0);
                pAnimEDSG->SetVisibility(true);
                if( i+2 < orPathRoute.mUseSize ) 
                   //There's definitely some(other intersection) beyond this intersection
                {
                    if( orPathRoute[i+1].type == RoadManager::ET_INTERSECTION ) 
                    {
                        ((Intersection*)(orPathRoute[i+1].elem))->GetLocation(pathHeading);
                    }
                    else
                    {
                        ((Road*)(orPathRoute[i+1].elem))->GetRoadSegment(0)->GetPosition(0.0f,0.0f,&pathHeading);
                        ((Road*)(orPathRoute[i+1].elem))->GetRoadSegment(((Road*)(orPathRoute[i+1].elem))->GetNumRoadSegments()-1)->GetPosition(0.0f, 0.0f,&pathTemp);
                        if( (pathHeading - sourcePosn).MagnitudeSqr() > (pathTemp - sourcePosn).MagnitudeSqr() )
                        {
                            pathHeading = pathTemp;
                        }
                    }
                }
                else //No(other intersection) beyond this intersection
                {
                    //Alpha semi-hack: Doing this because intersections aren't as easy 
                    //to find because they're not in the tree. So, only do direct
                    //point if intersection is closer than the nearest road seg.

                    float intersectionToTarget = (targetPosn - sourcePosn).MagnitudeSqr();

                    ((Road*)(orPathRoute[i+1].elem))->GetRoadSegment(0)->GetPosition(0.0f,0.0f,&pathHeading);
                    ((Road*)(orPathRoute[i+1].elem))->GetRoadSegment(((Road*)(orPathRoute[i+1].elem))->GetNumRoadSegments()-1)->GetPosition(0.0f, 0.0f,&pathTemp);
                    if( (pathHeading - sourcePosn).MagnitudeSqr() > (pathTemp - sourcePosn).MagnitudeSqr() )
                    {
                        pathHeading = pathTemp;
                    }

                    if( //intersectionToTarget < (targetPosn - pathHeading).MagnitudeSqr() ||
                        intersectionToTarget < 1600.0f ) 
                    {
                        pathHeading = targetPosn;
                    }
                    /*
                    if( (targetPosn - sourcePosn).MagnitudeSqr() < 900.0f) 
                    {
                        pathHeading = targetPosn;
                    }
                    else
                    {
                        ((Road*)(orPathRoute[i+1].elem))->GetRoadSegment(0)->GetPosition(0.0f,0.0f,&pathHeading);
                        ((Road*)(orPathRoute[i+1].elem))->GetRoadSegment(((Road*)(orPathRoute[i+1].elem))->GetNumRoadSegments()-1)->GetPosition(0.0f, 0.0f,&pathTemp);
                        if( (pathHeading - sourcePosn).MagnitudeSqr() > (pathTemp - sourcePosn).MagnitudeSqr() )
                        {
                            pathHeading = pathTemp;
                        }
                    }
                    */
                }
                pathHeading.Sub( pathHeading, sourcePosn );
                //pAnimEDSG->PlaceOnGround( 0.2f );
                pAnimEDSG->SetAnimRootHeadingYUp( pathHeading );
                pWrongwayAnimDSG->SetAnimRootHeadingYUp( pathHeading ); 
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    //move the floating arrow to the correct place
    //////////////////////////////////////////////////////////////////////////
 
    pSourceRoadSeg->GetPosition(0.0f, 0.0f, &sourcePosn);
    UpdateAnimPosition(GetAnimEntityDSGManager()->mpFloatingRightWayArrows[0], sourcePosn);
    UpdateAnimPosition(GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[0], sourcePosn);

    if( DirectionalArrowEnum::NEAREST_ROAD & mArrowType )
    {    
/*
        rmt::Box3D oldBBox;
        GetAnimEntityDSGManager()->mpFloatingRightWayArrows[0]->GetBoundingBox( &oldBBox );
        pSourceRoadSeg->GetPosition(0.0f, 0.0f, &sourcePosn);
        GetAnimEntityDSGManager()->mpFloatingRightWayArrows[0]->SetPosition( sourcePosn );
        GetAnimEntityDSGManager()->mpFloatingRightWayArrows[0]->PlaceOnGround( 0.2f );
        GetRenderManager()->pWorldScene()->Move(oldBBox, GetAnimEntityDSGManager()->mpFloatingRightWayArrows[0]);




        GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[0]->GetBoundingBox( &oldBBox );
        pSourceRoadSeg->GetPosition(0.0f, 0.0f, &sourcePosn);
        GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[0]->SetPosition( sourcePosn );
        GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[0]->PlaceOnGround( 0.2f ); 
        GetRenderManager()->pWorldScene()->Move(oldBBox, GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[0]);
*/
        GetAnimEntityDSGManager()->mpFloatingRightWayArrows[0]->SetVisibility(true);

        i = 0; 
        if(i+1<orPathRoute.mUseSize) //There's something in this path
        {
            if( orPathRoute[i+1].type == RoadManager::ET_INTERSECTION ) 
            {
                GetHeadingAlongPath(orPathRoute, i, pSourceRoadSeg, pathHeading);
                //todo: extract this code into a single function to call from for all long roads too.
            }
            else
            {
                ((Road*)(orPathRoute[i].elem))->GetRoadSegment(0)->GetPosition(0.0f,0.0f,&pathHeading);
                ((Road*)(orPathRoute[i].elem))->GetRoadSegment(((Road*)(orPathRoute[i].elem))->GetNumRoadSegments()-1)->GetPosition(0.0f, 0.0f,&pathTemp);
                if( (pathHeading - sourcePosn).MagnitudeSqr() > (pathTemp - sourcePosn).MagnitudeSqr() )
                {
                    pathHeading = pathTemp;
                }
            }
        }
        else //Nothing in this path
        {
            pathHeading = targetPosn;
        }
        pathHeading.Sub( pathHeading, sourcePosn );
        GetAnimEntityDSGManager()->mpFloatingRightWayArrows[0]->SetAnimRootHeadingYUp( pathHeading );
        GetAnimEntityDSGManager()->mpFloatingWrongWayArrows[0]->SetAnimRootHeadingYUp( pathHeading );
    }
    if(!GetCharacterSheetManager()->QueryNavSystemSetting())
    {
        UnlightCurrentPath(orPathRoute);
        return;
    }
}
//////////////////////////////////////////////////////////////////////////
void MissionObjective::GetHeadingAlongPath
(
    SwapArray<RoadManager::PathElement>& irPathRoute, int i, 
    RoadSegment* ipSourceRoadSeg, 
    rmt::Vector& orPathHeading 
)
{
//    bool invertResults;
//    rmt::Vector pathTemp2, pathTemp;
    rmt::Vector tempVect, tempVect2;
    rmt::Vector roadEndPosn, roadStartPosn;
    float distStartToNextRoad, distEndToNextRoad, scaleFactor;

    RoadSegment* pRoadStart =  ipSourceRoadSeg->GetRoad()->GetRoadSegment(0);
    RoadSegment* pRoadEnd = ipSourceRoadSeg->GetRoad()->GetRoadSegment(ipSourceRoadSeg->GetRoad()->GetNumRoadSegments()-1);

    //
    // Yay! Another Special Case
    //
    if(pRoadEnd==pRoadStart)
    {
        pRoadStart->GetCorner(0,roadStartPosn);
        pRoadStart->GetCorner(1,roadEndPosn);
    }
    else
    {
        pRoadStart->GetPosition( &roadStartPosn );
        pRoadEnd->GetPosition( &roadEndPosn );
    }


    if(i>0)
    {
        rAssert(irPathRoute[i-1].type == RoadManager::ET_INTERSECTION);
        ((Intersection*)(irPathRoute[i-1].elem))->GetLocation(tempVect2);

        //invert distances, since we're using the previous intersection in this case
        tempVect.Sub(tempVect2, roadEndPosn);
        distStartToNextRoad = tempVect.MagnitudeSqr();
        tempVect.Sub(tempVect2, roadStartPosn);
        distEndToNextRoad = tempVect.MagnitudeSqr();
    }
    else
    {
        if(irPathRoute.mUseSize==1) //return for now, since there's only one road, and we don't have origin, destination info
            return;

        rAssert(irPathRoute[i+1].type == RoadManager::ET_INTERSECTION);
        ((Intersection*)(irPathRoute[i+1].elem))->GetLocation(tempVect2);

        tempVect.Sub(tempVect2, roadEndPosn);
        distEndToNextRoad = tempVect.MagnitudeSqr();
        tempVect.Sub(tempVect2, roadStartPosn);
        distStartToNextRoad = tempVect.MagnitudeSqr();
    }

    if(distStartToNextRoad<distEndToNextRoad)
        scaleFactor = -1.0f;
    else
        scaleFactor = 1.0f;

    ipSourceRoadSeg->GetEdgeNormal(0,orPathHeading);

    orPathHeading.Scale(scaleFactor);
    ipSourceRoadSeg->GetPosition(0.0f, 0.0f, &tempVect);
    orPathHeading.Add(tempVect);

    /*
    if(i+1 < irPath.mUseSize)
    {
        ((Intersection*)(irPath[i+1].elem))->GetLocation(orPathHeading);
        if(((Intersection*)(irPath[i+1].elem)) == ipSourceRoadSeg->GetRoad()->GetSourceIntersection())
            invertResults=true;
        else
            invertResults=false;
    }
    else
    {
        ((Intersection*)(irPath[i-1].elem))->GetLocation(orPathHeading);
        if(((Intersection*)(irPath[i-1].elem)) == ipSourceRoadSeg->GetRoad()->GetSourceIntersection())
            invertResults=true;
        else
            invertResults=false;
    }

    if(ipSourceRoadSeg->GetSegmentIndex()+1 > ((Road*)(irPath[i].elem))->GetNumRoadSegments()-1)
        ipSourceRoadSeg->GetPosition(0.0f,0.0f,&pathTemp);
    else
        ((Road*)(irPath[i].elem))->GetRoadSegment(ipSourceRoadSeg->GetSegmentIndex()+1)->GetPosition(0.0f,0.0f,&pathTemp);
                    
    float roadToIntersection1 = (orPathHeading  - pathTemp).MagnitudeSqr();

    if(ipSourceRoadSeg->GetSegmentIndex() == 0)
        ipSourceRoadSeg->GetPosition(0.0f,0.0f,&pathTemp2);
    else
        ((Road*)(irPath[i].elem))->GetRoadSegment(ipSourceRoadSeg->GetSegmentIndex()-1)->GetPosition(0.0f,0.0f,&pathTemp2);

    float roadToIntersection2 = (orPathHeading - pathTemp2).MagnitudeSqr();

    if( roadToIntersection1 < roadToIntersection2 && !invertResults )
    {
        orPathHeading = pathTemp;
    }
    else
    {
        orPathHeading = pathTemp2;
    }*/
}
//////////////////////////////////////////////////////////////////////////
void MissionObjective::UpdateAnimPosition( AnimEntityDSG* pAnimEntityDSG, rmt::Vector& irPosn )
{
    rmt::Box3D oldBBox;
    pAnimEntityDSG->GetBoundingBox( &oldBBox );
    pAnimEntityDSG->SetPosition( irPosn );
    pAnimEntityDSG->PlaceOnGround( 0.2f, false );
    GetRenderManager()->pWorldScene()->Move(oldBBox, pAnimEntityDSG );
}

//=============================================================================
// MissionObjective::IsPattyAndSelmaDialog
//=============================================================================
// Description: determines if this is a patty and selma dialog objective
//
// Parameters:  NONE
//
// Return:      true/false - is this patty and selma talking to one another?
//
//=============================================================================
bool MissionObjective::IsPattyAndSelmaDialog() const
{
    unsigned int i = 0;
    for( ; i < mNumNPCs; ++i )
    {
        if( strcmp( mNPCNames[ i ], "patty" ) == 0 )
        {
            return true;
        }
        if( strcmp( mNPCNames[ i ], "zmale1" ) == 0 )
        {
            return true;
        }
        if( strcmp( mNPCNames[ i ], "zmale2" ) == 0 )
        {
            return true;
        }
        if( strcmp( mNPCNames[ i ], "zmale3" ) == 0 )
        {
            return true;
        }
    }
    return false;
}

//=============================================================================
// MissionObjective::IsRaceDialog
//=============================================================================
// Description: in a race dialog, you will be talking to milhouse, ralph, or
//              nelson
//
// Parameters:  NONE
//
// Return:      true/false - is this one of the race characters we're talking 
//              to
//
//=============================================================================
bool MissionObjective::IsRaceDialog() const
{
    unsigned int i = 0;
    for( ; i < mNumNPCs; ++i )
    {
        if( strcmp( mNPCNames[ i ], "milhouse" ) == 0 )
        {
            return true;
        }
        if( strcmp( mNPCNames[ i ], "ralph" ) == 0 )
        {
            return true;
        }
        if( strcmp( mNPCNames[ i ], "nelson" ) == 0 )
        {
            return true;
        }
    }
    return false;
}
