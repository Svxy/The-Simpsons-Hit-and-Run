//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        LocatorLoader.cpp
//
// Description: Implement LocatorLoader
//
// History:     24/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <radmath/radmath.hpp>
#include <p3d/chunkfile.hpp>

//========================================
// Project Includes
//========================================
#include <loading/LocatorLoader.h>

#include <constants/srrchunks.h>

#include <memory/srrmemory.h>

#include <meta/carstartlocator.h>
#include <meta/eventlocator.h>
#include <meta/locator.h>
#include <meta/locatortypes.h>
#include <meta/recttriggervolume.h>
#include <meta/spheretriggervolume.h>
#include <meta/splinelocator.h>
#include <meta/triggervolumetracker.h>
#include <meta/zoneeventlocator.h>
#include <meta/carstartlocator.h>
#include <meta/occlusionlocator.h>
#include <meta/interiorentrancelocator.h>
#include <meta/directionallocator.h>
#include <meta/actioneventlocator.h>
#include <meta/fovlocator.h>
#include <meta/staticcamlocator.h>
#include <meta/pedgrouplocator.h>
#include <meta/scriptlocator.h>

#include <camera/railcam.h>
#include <camera/staticcam.h>
#include <camera/supercamcentral.h>
#include <camera/supercammanager.h>

#include <render/RenderManager/RenderManager.h>
#include <render/Culling/WorldScene.h>

#include <mission/AnimatedIcon.h>
#include <mission/gameplaymanager.h>

#include <worldsim/parkedcars/parkedcarmanager.h>
#include <worldsim/coins/coinmanager.h>

#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/worldrenderlayer.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
void Swap(char* data_in, char* data_out, int n);

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************
int LocatorLoader::msZoneNameCount = 0;

//==============================================================================
// LocatorLoader::LocatorLoader
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
LocatorLoader::LocatorLoader() 
: tSimpleChunkHandler( SRR2::ChunkID::LOCATOR  )
{
    mpListenerCB = NULL;
}

//==============================================================================
// LocatorLoader::~LocatorLoader
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
LocatorLoader::~LocatorLoader()
{
}

//=============================================================================
// LocatorLoader::LoadObject
//=============================================================================
// Description: Comment
//
// Parameters:  (tChunkFile* f, tEntityStore* store)
//
// Return:      tEntity 
//
//=============================================================================
tEntity* LocatorLoader::LoadObject(tChunkFile* f, tEntityStore* store)
{
MEMTRACK_PUSH_GROUP( "Locator Loading" );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap (GMA_GC_VMM);
    #else
        HeapMgr()->PushHeap (GMA_LEVEL_OTHER);
    #endif

    char name[256];
    f->GetPString( name );

    LocatorType::Type type;
    type = (LocatorType::Type)(f->GetUInt());

    unsigned int numElements = f->GetUInt();
    unsigned int* elements = new unsigned int[ numElements ];

    unsigned int i;
    for( i = 0; i < numElements; i++ )
    {
        elements[ i ] = f->GetUInt();
    }

    rmt::Vector pos;
    
    pos.x = f->GetFloat();
    pos.y = f->GetFloat();
    pos.z = f->GetFloat();

    Locator* locator = NULL;

    unsigned int numTriggers = f->GetUInt();
    bool hasSubChunks = true;

    switch( type )
    {
    case LocatorType::GENERIC:
        {
            MEMTRACK_PUSH_GROUP("Generic Locator");
            locator = new Locator();
            rAssert( locator );

            hasSubChunks = false;
            MEMTRACK_POP_GROUP("Generic Locator");
            break;
        }
    case LocatorType::EVENT:
        {
            MEMTRACK_PUSH_GROUP("Event Locator");
            EventLocator* eventloc = new EventLocator();
            rAssert( eventloc );

            eventloc->SetEventType( (LocatorEvent::Event)elements[ 0 ] );

            if ( eventloc->GetEventType() == LocatorEvent::CAMERA_CUT )
            {
                //Make this locator inactive if the wreckless cam is not active.
                //Hackish
                SuperCam* sc = GetSuperCamManager()->GetSCC( 0 )->GetActiveSuperCam();
                if ( GetGameplayManager()->mIsDemo || (sc && sc->GetType() == SuperCam::WRECKLESS_CAM) )
                {
                    eventloc->SetFlag( Locator::ACTIVE, true );
                }
                else
                {
                    eventloc->SetFlag( Locator::ACTIVE, false );
                }
            }

            if ( numElements == 2 ) 
            {
                rAssert( eventloc->GetEventType() == LocatorEvent::FAR_PLANE ||
                         eventloc->GetEventType() == LocatorEvent::GOO_DAMAGE || 
                         eventloc->GetEventType() == LocatorEvent::LIGHT_CHANGE ||
                         eventloc->GetEventType() == LocatorEvent::TRAP ||
                         eventloc->GetEventType() == LocatorEvent::CHECK_POINT );

                eventloc->SetData( elements[1] );
            }

            locator = eventloc;
            MEMTRACK_POP_GROUP("Event Locator");
            break;
        }
    case LocatorType::SPLINE:
        {
            MEMTRACK_PUSH_GROUP("Spline Locator");
            SplineLocator* splineloc = new SplineLocator();
            rAssert( splineloc );

            locator = splineloc;
            MEMTRACK_POP_GROUP("Spline Locator");
            break;
        }
    case LocatorType::DYNAMIC_ZONE:
        {
            MEMTRACK_PUSH_GROUP("Zone Locator");
            ZoneEventLocator* eventloc = new ZoneEventLocator();
            rAssert( eventloc );

            //this is where we convert the data into a string...  ba-dum-ching!
            eventloc->SetZoneSize( numElements * 4 ); //This is to prevent fragmentation.

            char* zone = new char[numElements * 4 + 4];
            memcpy( zone, elements, numElements * 4 );
            zone[numElements * 4] = '\0';

#ifdef RAD_GAMECUBE
            Swap( zone, zone, numElements * 4 + 4);
#endif

            eventloc->SetZone( zone );

            delete[] zone;

            locator = eventloc;
            MEMTRACK_POP_GROUP("Zone Locator");
            break;
        }
    case LocatorType::CAR_START:
        {
            MEMTRACK_PUSH_GROUP("Car Locator");
            CarStartLocator* carStartloc = new CarStartLocator();
            rAssert( carStartloc );

            float rotation = 0;

            memcpy( (void*)(&rotation), (const void*)&elements[0], sizeof(float) );

            //HACK
            if ( numElements >= 2 )
            {
                //There is parked car data.
                if ( elements[1] == 1 )
                {
                    if ( !GetGameplayManager()->mIsDemo )
                    {
                        //Should place a parked car here.  Add to ParkedCarManager
                        GetPCM().AddLocator( carStartloc );
                    }
                }

                if ( numElements > 2 )
                {
                    //We have a special car here.
                    char* carName = new char[(numElements - 2) * 4 + 4];
                    memcpy( carName, &elements[2], (numElements - 2) * 4 );
                    carName[(numElements - 2) * 4] = '\0'; 
#ifdef RAD_GAMECUBE
                    Swap( carName, carName, (numElements - 2) * 4 + 4 );
#endif
                    
                    //Do something with the name
                    GetPCM().AddFreeCar( carName, carStartloc );

                    delete carName;
                }
            }

            carStartloc->SetRotation( rotation );

            locator = carStartloc;
            hasSubChunks = false;
            MEMTRACK_POP_GROUP("Car Locator");
            break;
        }
    case LocatorType::OCCLUSION:
        {
            MEMTRACK_PUSH_GROUP("Occlusion Locator");
            OcclusionLocator* occLoc = new OcclusionLocator();
            rAssert( occLoc );

            occLoc->SetNumTriggers( numTriggers, GMA_LEVEL_OTHER );

            bool first = true;

            while( f->ChunksRemaining() )
            {
                f->BeginChunk();

                switch( f->GetCurrentID() )
                {
                case SRR2::ChunkID::TRIGGER_VOLUME:
                    {
                        //Only add the first one to the trigger tracker.
                        LoadTriggerVolume( f, occLoc, first );
                        first = false;
                        break;
                    }
                }

                f->EndChunk();
            }

            //How many of the locators are occlusion, the rest are visiblers - 1
            if ( numElements )
            {
                occLoc->SetNumOccTriggers( elements[0] );
            }
            else
            {
                occLoc->SetNumOccTriggers( numTriggers - 1 );
            }

            locator = occLoc;
            hasSubChunks = false;  //Special case, already loaded.
            MEMTRACK_POP_GROUP("Occlusion Locator");
            break;
        }
    case LocatorType::INTERIOR_ENTRANCE:
        {
            MEMTRACK_PUSH_GROUP("Interior Locator");
            InteriorEntranceLocator* intEntLoc = new InteriorEntranceLocator();
            rAssert( intEntLoc );

            rAssert( numElements * 4 < 256 );
            unsigned int length = numElements * 4;

            char charData[256];
            memcpy( charData, elements, numElements * 4 );

            #ifdef RAD_GAMECUBE
                Swap( charData, charData, numElements * 4 );
            #endif
            unsigned int chari = 0;
            for ( chari = 0; chari < length; ++chari )
            {
                if ( charData[chari] == '\0' )
                {
                    //Found the null.
                    break;
                }
            }

            //this is where we convert the data into a string...  ba-dum-ching!
            intEntLoc->SetInteriorFileNameSize( chari ); //This is to prevent fragmentation.
            intEntLoc->SetInteriorFileName( charData );

            //Now get the transform.
            unsigned int index = chari / 4;

            if ( chari % 4 != 0 )
            {
                index++; //Add one to cover the extra.
            }

            rmt::Matrix mat;
            mat.Identity();

            unsigned int i;
            for ( i = 0; i < 3; ++i )
            {
                rmt::Vector v;
                memcpy( &v.x, &elements[index + 3 * i], sizeof(float) * 3 );
                mat.m[i][0] = v.x;
                mat.m[i][1] = v.y;
                mat.m[i][2] = v.z;
            }
            
            intEntLoc->SetTransform( mat );
            locator = intEntLoc;
            MEMTRACK_POP_GROUP("Interior Locator");
            break;
        }
    case LocatorType::DIRECTIONAL:
        {
            MEMTRACK_PUSH_GROUP("Directional Locator");
            DirectionalLocator* dirLoc = new DirectionalLocator();
            rAssert( dirLoc );

            rmt::Matrix mat;
            mat.Identity();

            unsigned int i;
            for ( i = 0; i < 3; ++i )
            {
                rmt::Vector v;
                memcpy( &v.x, &elements[3 * i], sizeof(float) * 3 );
                mat.m[i][0] = v.x;
                mat.m[i][1] = v.y;
                mat.m[i][2] = v.z;
            }

            dirLoc->SetTransform( mat );
            
            locator = dirLoc;
            hasSubChunks = false;
            MEMTRACK_POP_GROUP("Directional Locator");
            break;
        }
    case LocatorType::ACTION:
        {
            MEMTRACK_PUSH_GROUP("Action Locator");
            ActionEventLocator* actLoc = new ActionEventLocator();
            rAssert( actLoc );

            rAssert(numElements >= 5);
            rAssert( (numElements - 2) * 4 < 256 );
            unsigned int length = (numElements - 2) * 4;

            char charData[256];
            memcpy( charData, elements, (numElements - 2) * 4 );

#ifdef RAD_GAMECUBE
            Swap( charData, charData, (numElements - 2) * 4 );
#endif

            //Go through the charData until we find all 3 strings.
            const unsigned int numStrings = 3;
            char stringArray[numStrings][128];  //If the name is larger than 128, I'll shoot myself.

            unsigned int i;
            unsigned int startChari = 0;
            unsigned int chari = 0;
            for ( i = 0; i < numStrings; ++i )
            {
                while ( charData[chari] != '\0' && chari != length )
                {
                    ++chari;
                }

                rAssertMsg( chari != length, "Missing text in action locator!" );

                if ( chari != length )
                {
                    rAssert( strlen(&charData[startChari]) < 128 );
                    strncpy( stringArray[i], &charData[startChari], chari - startChari );
                    stringArray[i][strlen(&charData[startChari])] = '\0';
                    ++chari;
                    if ( i == numStrings - 1 )
                    {
                        //We're done
                        break;
                    }
                }

                //move past the NULLS
                while ( charData[chari] == '\0' && chari != length )
                {
                    ++chari;
                }

                startChari = chari;
            }

            actLoc->SetObjNameSize( strlen(stringArray[0]) );
            actLoc->SetObjName( stringArray[0] );

            actLoc->SetJointNameSize( strlen(stringArray[1]) );
            actLoc->SetJointName( stringArray[1] );

            actLoc->SetActionNameSize( strlen(stringArray[2]) );
            actLoc->SetActionName( stringArray[2] );

            actLoc->SetButtonInput( (CharacterController::eIntention)elements[numElements - 2] );
            actLoc->SetShouldTransform( elements[numElements - 1] == 1 ? true : false );
            
            // TBJ [8/30/2002] 
            //
            // I need to set the position of the locator before I add it to the game.
            //
            actLoc->SetLocation( pos );

            // If AddToGame() fails, actLoc will be released.
            // Hold on to it here.
            //
            actLoc->AddRef();
            if ( actLoc->AddToGame( store ) )
            {
                locator = actLoc;
            }
            else
            {
                locator = NULL;
            }
            // If AddToGame() didn't addref the locator, 
            // this release will delete it.
            //
            actLoc->Release( );
            MEMTRACK_POP_GROUP("Action Locator");
            break;
        }
    case LocatorType::FOV:
        {
            MEMTRACK_PUSH_GROUP("FOV Locator");
            FOVLocator* fovLoc = new FOVLocator();
            rAssert( fovLoc );

            float data[3];
            memcpy( data, elements, sizeof(float) * 3 );

            fovLoc->SetFOV( rmt::DegToRadian( data[0] ) );
            fovLoc->SetTime( data[1] );
            fovLoc->SetRate( data[2] );
            
            locator = fovLoc;
            hasSubChunks = true;
            MEMTRACK_POP_GROUP("FOV Locator");
            break;
        }

    case LocatorType::BREAKABLE_CAMERA:
        {
            MEMTRACK_PUSH_GROUP("Breakable Camera Locator");
/*
            //TODO: what do we do with these??
            float data[10];
            memcpy( data, elements, sizeof(float) * 10 );

            rmt::Matrix mat;
            mat.Identity();

            unsigned int i;
            for ( i = 0; i < 9;)
            {
                mat.m[i][0] = data[i];
                mat.m[i][1] = data[i + 1];
                mat.m[i][2] = data[i + 2];

                i += 3;
            }

            float FOV = data[9];

			// Create a new action event locator
	        ActionEventLocator*	actLoc = new ActionEventLocator();
            /*
            actLoc->SetObjNameSize( strlen(stringArray[0]) );
            actLoc->SetObjName( stringArray[0] );

            actLoc->SetJointNameSize( strlen(stringArray[1]) );
            actLoc->SetJointName( stringArray[1] );

            actLoc->SetActionNameSize( strlen(stringArray[2]) );
            actLoc->SetActionName( stringArray[2] );

            actLoc->SetButtonInput( (CharacterController::eIntention)elements[numElements - 2] );
            actLoc->SetShouldTransform( elements[numElements - 1] == 1 ? true : false );
            */
/*
            actLoc->SetLocation( pos );
			
			const char* ObjName = "Alien Camera Obj";
			actLoc->SetObjNameSize( strlen( ObjName ) );
			actLoc->SetObjName( ObjName );

			const char* ActionName = "AlienCamera";

			actLoc->SetActionNameSize( strlen( ActionName ) );
			actLoc->SetActionName( ActionName );

			actLoc->SetButtonInput( CharacterController::Attack );

			bool addSuccess = actLoc->AddToGame( store );

		//	rAssertMsg( addSuccess, "Could not add camera locator to the game" );
			
*/
            MEMTRACK_POP_GROUP("Breakable Camera Locator");
            break;
        }
    case LocatorType::STATIC_CAMERA:
        {
            MEMTRACK_PUSH_GROUP("Static Camera Locator");
            StaticCamLocator* scLoc = new StaticCamLocator();

            StaticCam* sCam = new StaticCam();
                        
            rAssert( numElements < 32 );
            float data[32];
            memcpy( data, elements, sizeof(float) * numElements );

            rmt::Vector offset;
            offset.x = data[0];
            offset.y = data[1];
            offset.z = data[2];

            sCam->SetTargetOffset( offset );

            float FOV = data[3];
            sCam->SetMaxFOV( rmt::DegToRadian( FOV ) );

            float lag = data[4];
            sCam->SetTargetLag( lag );

            int track;
            memcpy( &track, &data[5], sizeof( int ) ); //This is sucky, sorry.
            bool tracking = track == 1 ? true : false;
            
            float trate = 0.04f;
            if ( numElements >= 7 )
            {
                memcpy( &trate, &data[6], sizeof(float) );
            }

            if ( numElements >= 8 )
            {
                int flags = 0;
                memcpy( &flags, &data[7], sizeof( int ) ); //This is sucky, sorry.
                
                scLoc->SetOneShot( (flags & 1) > 0 );

                if ( (flags & (1 << 1)) ) //This is the hacky way that I export this info.
                {
                    //Disable fov on controller for this cam.
                    sCam->SetFOVLag( 0.0f );
                }
            }

            if ( numElements >= 9 )
            {
                int inOut = 0;
                memcpy( &inOut, &data[8], sizeof( int ) ); //This is sucky, sorry.

                scLoc->SetCutInOut( inOut == 1 );

                int intData = 0;
                memcpy( &intData, &data[9], sizeof( int ) ); //This is sucky, sorry.

                scLoc->SetCarOnly( (intData & 1) == 1 );
                scLoc->SetOnFootOnly( (intData & (1 << 1)) == 1 << 1 );
            }

            sCam->SetTransitionPositionRate( trate );
            sCam->SetTransitionTargetRate( trate );
            
            sCam->SetTracking( tracking ); 

            sCam->SetPosition( pos );

            scLoc->SetStaticCam( sCam );

            locator = scLoc;
            hasSubChunks = true;
            MEMTRACK_POP_GROUP( "Static Camera Locator" );
            break;
        }
    case LocatorType::PED_GROUP:
        {
            MEMTRACK_PUSH_GROUP("Ped Group Locator");

            PedGroupLocator* pgLoc = new PedGroupLocator();
            rAssert( pgLoc );

            unsigned int group = 0;

            memcpy( (void*)(&group), (const void*)elements, sizeof(unsigned int) );

            pgLoc->SetGroupNum( group );

            locator = pgLoc;
            hasSubChunks = true;
            MEMTRACK_POP_GROUP( "Ped Group Locator" );
            break;
        }
    case LocatorType::SCRIPT:
        {
            MEMTRACK_PUSH_GROUP("Script Locator");

            ScriptLocator* sLoc = new ScriptLocator();
            rAssert( sLoc );

            char* text = new char[numElements * 4 + 4];
            memcpy( text, elements, numElements * 4 );
            text[numElements * 4] = '\0';

#ifdef RAD_GAMECUBE
            Swap( text, text, numElements * 4 + 4 );
#endif

            sLoc->SetString( text );

            delete[] text;

            locator = sLoc;
            hasSubChunks = true;
            MEMTRACK_POP_GROUP( "Script Locator" );
            break;
        }
    case LocatorType::COIN:
        {
            MEMTRACK_PUSH_GROUP("Coin Locator");
            //Adding world coin to coin manager
            GetCoinManager()->AddWorldCoin( pos, GetRenderManager()->pWorldRenderLayer()->GetCurSectionName().GetUID() );
            locator = NULL;
            MEMTRACK_POP_GROUP( "Coin Locator" );
            break;
        }
    default:
        {
            MEMTRACK_PUSH_GROUP("Unknown Locator");
            rDebugString( "Bad locator loaded.. UNKNOWN type!\n" );
            Locator* loc = new Locator();
            rAssert( loc );
            locator = loc;
            MEMTRACK_POP_GROUP( "Unknown Locator" );
            break;
        }
    }

    // locator might be NULL if it was not successfully added to game.
    //
    // TBJ [8/30/2002] 
    //
    if( locator != NULL )
    {
        if ( hasSubChunks )
        {
            locator->SetNumTriggers( numTriggers, HeapMgr()->GetCurrentHeap() );

            while( f->ChunksRemaining() )
            {
                f->BeginChunk();

                switch( f->GetCurrentID() )
                {
                case SRR2::ChunkID::TRIGGER_VOLUME:
                    {
                        LoadTriggerVolume( f, static_cast<TriggerLocator*>(locator) );
                        break;
                    }
                case SRR2::ChunkID::SPLINE:
                    {
                        if ( locator->GetDataType() == LocatorType::SPLINE )
                        {
                            RailCam* railCam = LoadSpline( f, static_cast<SplineLocator*>(locator) );
                            rAssert( railCam );
                            static_cast<SplineLocator*>(locator)->SetRailCam( railCam );
                        }
                        else
                        {
                            RailCam* railCam = LoadSpline( f, NULL );
                            rAssert( railCam );
                        }
                        break;
                    }
                case SRR2::ChunkID::EXTRA_MATRIX:
                    {
                        rmt::Matrix mat;
                        f->GetData(&(mat), 16, tFile::DWORD);

                        locator->SetMatrix( mat );
                        
                        break;
                    }
                }

                f->EndChunk();
            }
        }

        locator->SetName( name );
        locator->SetLocation( pos );

        //locator->SetFlag( Locator::ACTIVE, false );
    }
    delete[] elements;

MEMTRACK_POP_GROUP("Locator Loading");

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap (GMA_GC_VMM);
    #else
        HeapMgr()->PopHeap (GMA_LEVEL_OTHER);
    #endif

    //
    // Some locators should not be added to the inventory
    //
    if( type == LocatorType::DYNAMIC_ZONE )
    {
        //
        // All the locators called load_something are never looked up
        //
        if( strncmp( name, "load", 4 ) == 0 )
        {
            msZoneNameCount++;
            char tempName[256];
            sprintf( tempName, "mfa%d%s",  msZoneNameCount, name);
            locator->SetName(tempName);
        }
    }

    return locator;
}

///////////////////////////////////////////////////////////////////////
// IWrappedLoader
///////////////////////////////////////////////////////////////////////
//========================================================================
// LocatorLoader::SetRegdListener
//========================================================================
//
// Description: Register a new listener/caretaker, notify old listener of 
//              severed connection.
//
// Parameters:  pListenerCB: Callback to call OnChunkLoaded
//              pUserData:   Data to pass along for filtering, etc
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void LocatorLoader::SetRegdListener
(
   ChunkListenerCallback* pListenerCB,
   int iUserData 
)
{
   //
   // Follow protocol; notify old Listener, that it has been 
   // "disconnected".
   //
   if( mpListenerCB != NULL )
   {
      mpListenerCB->OnChunkLoaded( NULL, iUserData, 0 );
   }
   mpListenerCB  = pListenerCB;
   mUserData     = iUserData;
}

//========================================================================
// LocatorLoader::ModRegdListener
//========================================================================
//
// Description: Just fuck with the current pUserData
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void LocatorLoader::ModRegdListener
( 
   ChunkListenerCallback* pListenerCB,
   int iUserData 
)
{
#if 0
   char DebugBuf[255];
   sprintf( DebugBuf, "GeometryWrappedLoader::ModRegdListener: pListenerCB %X vs mpListenerCB %X\n", pListenerCB, mpListenerCB );
   rDebugString( DebugBuf );
#endif
   rAssert( pListenerCB == mpListenerCB );

   mUserData = iUserData;
}
//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// LocatorLoader::LoadTriggerVolume
//=============================================================================
// Description: Comment
//
// Parameters:  ( tChunkFile* f, 
//                TriggerLocator* locator,
//                bool addToTracker )
//
// Return:      void 
//
//=============================================================================
bool LocatorLoader::LoadTriggerVolume( tChunkFile* f, 
                                       TriggerLocator* locator,
                                       bool addToTracker )
{
    bool good = true;

    MEMTRACK_PUSH_GROUP("Trigger Volume Loading");

    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap (GMA_LEVEL_OTHER);
    #endif


    if( f->GetCurrentID() == SRR2::ChunkID::TRIGGER_VOLUME )
    {
        char volname[256];
        rmt::Matrix mat;
        rmt::Vector scale;
        rmt::Vector pos;

        TriggerVolume* vol = NULL;

        f->GetPString( volname );
        unsigned int voltype = f->GetUInt();

        scale.x = f->GetFloat();
        scale.y = f->GetFloat();
        scale.z = f->GetFloat();

        float val;

        for( unsigned int v = 0; v < 4; v++ )
        {
            for( unsigned int u = 0; u < 4; u++ )
            {
                val = f->GetFloat();

                mat.m[v][u] = val;
            }
        }
        pos = mat.Row(3);

        switch( voltype )
        {
        case TriggerVolume::SPHERE:
            {
                MEMTRACK_PUSH_GROUP("Sphere Trigger");
                SphereTriggerVolume* sphere = new SphereTriggerVolume();

                // this is a sphere, so the radii should be equal
                sphere->SetSphereRadius( scale.x );
                vol = sphere;
                MEMTRACK_POP_GROUP("Sphere Trigger");
                break;
            }
        case TriggerVolume::RECTANGLE:
            {
                MEMTRACK_PUSH_GROUP("Rectangle Trigger");
                RectTriggerVolume* box = new RectTriggerVolume( pos, 
                                                                           mat.Row(0), 
                                                                           mat.Row(1), 
                                                                           mat.Row(2), 
                                                                           scale.x, 
                                                                           scale.y, 
                                                                           scale.z );
                vol = box;
                MEMTRACK_POP_GROUP("Rectangle Trigger");
                break;
            }
        default:
            {
                // never come here again!
                MEMTRACK_PUSH_GROUP("Unknown Trigger");
                rAssert( false );
                MEMTRACK_POP_GROUP("Unknown Trigger");
                break;
            }
        }

        vol->SetName( volname );

        vol->SetLocator( locator );
        locator->AddTriggerVolume( vol );

        vol->SetPosition( pos );

        if ( addToTracker )
        {
            GetTriggerVolumeTracker()->AddTrigger( vol );
            // dont notify the render manager (listener), instead, circumvent and add directly to the tree 
            //mpListenerCB->OnChunkLoaded(vol, mUserData, _id);
            //GetRenderManager()->pWorldScene()->Add(vol);  //Now we are adding to the DSG.
        }

        good = true;
    }
    else
    {
        good = false;
    }

    MEMTRACK_POP_GROUP("Trigger Volume Loading");

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap (GMA_LEVEL_OTHER);
    #endif
    return good;
}

//=============================================================================
// LocatorLoader::LoadSpline
//=============================================================================
// Description: Comment
//
// Parameters:  ( tChunkFile* f, SplineLocator* splineLoc )
//
// Return:      RailCam* 
//
//=============================================================================
                                           
RailCam* LocatorLoader::LoadSpline( tChunkFile* f, SplineLocator* splineLoc )
{
    if( f->GetCurrentID() == SRR2::ChunkID::SPLINE )
    {
MEMTRACK_PUSH_GROUP("Rail Cam");

        char name[256];
        f->GetPString( name );

        int numCVs = f->GetInt();

        rmt::SplineCurve newSpline;
        newSpline.SetNumVertices( numCVs );

        for( int i = 0; i < numCVs; i++ )
        {
            rmt::Vector v;

            v.x = f->GetFloat();
            v.y = f->GetFloat();
            v.z = f->GetFloat();

            newSpline.SetCntrlVertex( i, v );
        }
        
        rAssert( f->ChunksRemaining() );

        //Load the railcam...
        RailCam* newRailCam = new RailCam();

        f->BeginChunk();

        f->GetString( name );
        newRailCam->SetBehaviour( (RailCam::Behaviour)f->GetUInt() );
        newRailCam->SetMinRadius( f->GetFloat() );
        newRailCam->SetMaxRadius( f->GetFloat() );
        newRailCam->SetTrackRail( f->GetUInt() == 1 ? true : false );
        newRailCam->SetTrackDist( f->GetFloat() );
        newRailCam->SetReverseSense( f->GetUInt() == 1 ? true : false );
        newRailCam->SetMaxFOV( rmt::DegToRadian( f->GetFloat() ) );

        rmt::Vector targetOffset;
        f->GetData( &targetOffset.x, 3, tFile::DWORD );
        newRailCam->SetTargetOffset( targetOffset );

        rmt::Vector play;
        f->GetData( &play.x, 3, tFile::DWORD );
        //newRailCam->SetAxisPlay( play );
        //Hack to get some data
        if ( play.x > 0.001f )
        {
            newRailCam->SetTransitionPositionRate( play.x );
            newRailCam->SetTransitionTargetRate( play.x );
        }

        if ( play.y == 1.0f ) //This is set to one when we want to disable the lag.
        {
            //Disable FOV lagging.
            newRailCam->SetFOVLag( 0.0f );
        }

        if ( splineLoc )
        {
            //Play.z is hiding stuff too
            int hiddenX = (int)play.z;  //I hate this.
    
            splineLoc->SetCarOnly( (hiddenX & 0x00000001) != 0 );
            splineLoc->SetCutInOut( (hiddenX & 0x0000002) != 0 );
            splineLoc->SetReset( (hiddenX & 0x00000004) != 0 ); 
            splineLoc->SetOnFootOnly( (hiddenX & 0x00000008) != 0 );
        }

        newRailCam->SetPositionLag( f->GetFloat() );
        newRailCam->SetTargetLag( f->GetFloat() );

        newRailCam->SetSplineCurve( newSpline );

        f->EndChunk();

MEMTRACK_POP_GROUP("Rail Cam");

        return newRailCam;        
    }

    return NULL;
}

//Endian swap.
inline void Swap( char* data_in, char* data_out, int n )
{ 
    rAssert( n % 4 == 0 );

    char tmp1, tmp2, tmp3, tmp4;
    for(int i = 0; i < n; i += 4)
    {
        tmp1 = data_in[i];
        tmp2 = data_in[i+1];
        tmp3 = data_in[i+2];
        tmp4 = data_in[i+3];

        data_out[i] = tmp4;
        data_out[i+1] = tmp3;
        data_out[i+2] = tmp2;
        data_out[i+3] = tmp1;
    }
}
