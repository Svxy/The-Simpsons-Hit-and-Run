//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        collectdumpedobjective.cpp
//
// Description: Implement CollectDumpedObjective
//
// History:     1/7/2003 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <radmath/radmath.hpp>
#include <raddebugwatch.hpp>

//========================================
// Project Includes
//========================================
#include <mission/objectives/collectdumpedobjective.h>
#include <mission/gameplaymanager.h>

#include <events/eventmanager.h>

#include <presentation/gui/guisystem.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>

#include <roads/roadmanager.h>
#include <roads/road.h>
#include <roads/roadsegment.h>
#include <roads/lane.h>

#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/hitnrunmanager.h>
#include <ai/vehicle/chaseai.h>

#include <meta/locator.h>

#include <mission/animatedicon.h>

#include <mission/conditions/missioncondition.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

#ifdef DEBUGWATCH
float DEFAULT_FORCE = 0.038f; // was 0.072
int MAX_TIMEOUT = 5000;
static unsigned int CD_DEFAULT_TIMEOUT = 5000;
static unsigned int DEFAULT_DUMP_LIFETIME = 15000;
#else
const float DEFAULT_FORCE = 0.05f; // was 0.072
const int MAX_TIMEOUT = 5000;
static const unsigned int CD_DEFAULT_TIMEOUT = 5000;
static const unsigned int DEFAULT_DUMP_LIFETIME = 15000;
#endif

#if defined( DEBUGWATCH ) || defined( RAD_WIN32 )
extern float DEFAULT_DIST;
#else
extern const float DEFAULT_DIST;
#endif

class CollectionCondition : public MissionCondition
{
public:
    CollectionCondition() { SetType( COND_GET_COLLECTIBLES ); }
    virtual ~CollectionCondition() {};

    void Update( unsigned int elapsedTime ) {};
    void SetViolated(bool flag) {SetIsViolated(true);}

    bool IsClose() { return false; }

private:
    //Prevent wasteful constructor creation.
    CollectionCondition ( const CollectionCondition & c);
    CollectionCondition& operator=( const CollectionCondition & c);
};

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// CollectDumpedObjective::CollectDumpedObjective
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
CollectDumpedObjective::CollectDumpedObjective() :
    mDumpVehicle( NULL ),
    mDumpVehicleAI( NULL ),
    mNumUncollected( 0 ),
    mBumperCars( true ),
    mNumSpawned( 0 ),
    mDumpTimeout( -1 ),
    mWhatToDump( 0 ),
    mAmIDumping( false ),
    mAssaultCar( NULL ),
    mMeDumpTimout( 0 ),
    mDumpLifetime ( 0 ),
    mTerminalDump ( false ),
    mAnimatedIcon ( NULL )
{
    mCondition = NULL;
}

//=============================================================================
// CollectDumpedObjective::~CollectDumpedObjective
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
CollectDumpedObjective::~CollectDumpedObjective()
{  
    mCondition = NULL; //stage will delete it 
}

//=============================================================================
// CollectDumpedObjective::BindCollectibleToWaypoint
//=============================================================================
// Description: Comment
//
// Parameters:  ( int collectibleNum, unsigned int waypointNum )
//
// Return:      void 
//
//=============================================================================
void CollectDumpedObjective::BindCollectibleToWaypoint( int collectibleNum, unsigned int waypointNum )
{
    rAssert( static_cast<int>(waypointNum) < WaypointAI::MAX_WAYPOINTS );
    mDumpData[ waypointNum ].collectibleNum = collectibleNum;

    //Aww...  I was waiting for the candy.
    mBumperCars = false;
}

//=============================================================================
// CollectDumpedObjective::SetDumpVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( Vehicle* vehicle )
//
// Return:      void 
//
//=============================================================================
void CollectDumpedObjective::SetDumpVehicle( Vehicle* vehicle )
{
    rAssert( vehicle );
    mDumpVehicle = vehicle;
}

//=============================================================================
// CollectDumpedObjective::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void CollectDumpedObjective::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
    case EVENT_WAYAI_HIT_WAYPOINT:
        {
            //Test to see if this is a waypoint we care about
            int lastWaypoint = mDumpVehicleAI->GetCurrentWayPoint();
            rAssert( lastWaypoint >= 0 );
            int collectibleNum = mDumpData[ lastWaypoint ].collectibleNum;
            if ( !mDumpData[ lastWaypoint ].collected && lastWaypoint >= 0 && 
                 collectibleNum != -1 && mDumpTimeout == -1 )
            { 
                //This is a dumping waypoint
                mWhatToDump = static_cast<unsigned int>(collectibleNum);
                mDumpTimeout = rmt::FtoL( rmt::LtoF(rand()) / rmt::LtoF(RAND_MAX) * rmt::LtoF(MAX_TIMEOUT) );
                mAmIDumping = false;
                mDumpLifetime = 0;

                mDumpData[ lastWaypoint ].active = true;
            }
            break;
        }
    case EVENT_VEHICLE_DESTROYED:
        {
            if ( !mBumperCars || mNumCollectibles > 1 )
            {
                break;
            }

            Vehicle* data = static_cast<Vehicle*>(pEventData);
            if ( data == mDumpVehicle )
            {
                //This is the end!
                //Throw mamma from the train.
                unsigned int collectibleNum = FindFreeSlot();

                //Since there are no bindings, we'll fill the mDumpData table. 
                mDumpData[ collectibleNum ].collectibleNum = collectibleNum;
                mDumpData[ collectibleNum ].active = true;

                //Set the drop timout to something small to give the user a ittle reaction time
                mDumpTimeout = 1000;
                mWhatToDump = collectibleNum;
                mAmIDumping = false;
                mDumpLifetime = 0;
                mTerminalDump = true;

                ++mNumSpawned;
            }

            break;
        }
    case EVENT_VEHICLE_VEHICLE_COLLISION:
        {
            if ( (!mBumperCars && !IsUserDumpAllowed()) || mNumSpawned >= GetNumCollectibles() || mNumCollectibles == 1 )
            {
                break;
            }

            CarOnCarCollisionEventData* data = static_cast<CarOnCarCollisionEventData*>(pEventData);
            if ( data->vehicle == mDumpVehicle && mBumperCars )
            {
                if ( data->force >= DEFAULT_FORCE && mDumpTimeout == -1 && mNumCollectibles > 1 )  //If numCollectibles is == 1 then it's on Death only
                {
                    //Throw mamma from the train.
                    unsigned int collectibleNum = FindFreeSlot();

                    //Since there are no bindings, we'll fill the mDumpData table. 
                    mDumpData[ collectibleNum ].collectibleNum = collectibleNum;
                    mDumpData[ collectibleNum ].active = true;

                    //Set the drop timout to something small to give the user a ittle reaction time
                    mDumpTimeout = 1000;
                    mWhatToDump = collectibleNum;
                    mAmIDumping = false;
                    mDumpLifetime = DEFAULT_DUMP_LIFETIME;

                    ++mNumSpawned;

                    //Let's damage the dumper car.
                    /*
                    float damage = data->vehicle->mDesignerParams.mHitPoints * rmt::LtoF(mNumSpawned) / rmt::LtoF(GetNumCollectibles());
                    float currDamage = data->vehicle->mDesignerParams.mHitPoints - data->vehicle->mHitPoints;
                    damage -= currDamage;
                    if ( damage < 0.0f )
                    {
                        damage = 0.0f;
                    }

                    data->vehicle->SwitchOnDamageTypeAndApply( damage, *(data->collision) );
                    */
                }
            }
            else
            {
                //This is a car hit me type thing.
                //Let's dump some of our cargo.
                /*
                
                    greg
                    july 3, 2003
                    
                    could this actually be my last bug?
                    
                    Cary says we don't need this block.
                
                
                
                if ( GetNumCollected() > 0 && mMeDumpTimout == 0 )
                {
                    int id = GetVehicleCentral()->GetVehicleId( data->vehicle );
                    VehicleController* controller = GetVehicleCentral()->GetVehicleController( id );

                    if ( controller )
                    {
                        //This sounds slow
                        ChaseAI* pai = dynamic_cast<ChaseAI*>( controller );
                        if ( pai )
                        {
                            int collectedID = GetAnyCollectedID();
                            rAssert( collectedID > -1 );

                            //This is a guy who hit me.
                            //Set the drop timout to 0
                            mDumpTimeout = 0;
                            mWhatToDump = static_cast<unsigned int>( collectedID );
                            mAmIDumping = true;
                            mAssaultCar = data->vehicle;
                            mDumpLifetime = 0;

                            //reset the timeout
                            mMeDumpTimout = CD_DEFAULT_TIMEOUT;
                        }
                    }
                }
                */
                
            }
            break;
        }
    default:
        break;
    }

    CollectibleObjective::HandleEvent( id, pEventData );
}

//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// CollectDumpedObjective::OnInitCollectibles
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void CollectDumpedObjective::OnInitCollectibles()
{

}

//=============================================================================
// CollectDumpedObjective::OnInitCollectibleObjective
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void CollectDumpedObjective::OnInitCollectibleObjective()
{
    int i;
    for( i = 0; i < static_cast<int>( mNumCollectibles ); i++ )
    {
        Activate( i, false, false );
    }

    mAnimatedIcon = new AnimatedIcon();

    if ( mBumperCars )
    {
        //Listen for the collision event.
        GetEventManager()->AddListener( this, EVENT_VEHICLE_VEHICLE_COLLISION );
        
        if ( mNumCollectibles == 1 )
        {
            GetGuiSystem()->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_DAMAGE_METER );

            // update damage meter
            if ( GetCurrentHud() )
            {
                GetCurrentHud()->SetDamageMeter( 0.0f ); // between 0.0 and 1.0
            }

            GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED );
        }

        rmt::Vector carPos;
        mDumpVehicle->GetPosition( &carPos );
        mAnimatedIcon->Init( ARROW_DESTROY, carPos );
        mAnimatedIcon->ScaleByCameraDistance( MIN_ARROW_SCALE, MAX_ARROW_SCALE, MIN_ARROW_SCALE_DIST, MAX_ARROW_SCALE_DIST );

    }
    else
    {
        //Look for the bound waypoints
        GetEventManager()->AddListener( this, EVENT_WAYAI_HIT_WAYPOINT );

        if ( IsUserDumpAllowed() )
        {
            //Listen for the collision event.
            GetEventManager()->AddListener( this, EVENT_VEHICLE_VEHICLE_COLLISION );
        }

        rmt::Vector carPos;
        mDumpVehicle->GetPosition( &carPos );
        mAnimatedIcon->Init( ARROW_EVADE, carPos );
        mAnimatedIcon->ScaleByCameraDistance( MIN_ARROW_SCALE, MAX_ARROW_SCALE, MIN_ARROW_SCALE_DIST, MAX_ARROW_SCALE_DIST );

    }

    if( mNumCollectibles > 1 ) // only show collectibles HUD overlay if more than 1 things to collect
    {
        GetGuiSystem()->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_COLLECTIBLES );
    }

    if ( mBumperCars )
    //void disable damage on this guy
    //The vehicle is damaged only by collecting the 
    {
        if ( mNumCollectibles == 1 )
        {
            //void disable damage on this guy
            //The vehicle is damaged only by collecting the items he has to offer.
            //When you've smashed all the candy out of him, he damages out.
            mDumpVehicle->SetVehicleCanSustainDamage( true );
            mDumpVehicle->VehicleIsADestroyObjective( true ); 
            GetGameplayManager()->GetCurrentVehicle()->SetVehicleCanSustainDamage( false );
        }
        else
        {
            //void disable damage on this guy
            //The vehicle is damaged only by collecting the items he has to offer.
            //When you've smashed all the candy out of him, he damages out.
            mDumpVehicle->SetVehicleCanSustainDamage( false );
            GetGameplayManager()->GetCurrentVehicle()->SetVehicleCanSustainDamage(false);
        }
        GetHitnRunManager()->RegisterVehicleImmunity( mDumpVehicle );
    }
    else
    {
        mDumpVehicle->SetVehicleCanSustainDamage( false );
    }

    //Set this vehicle as the focus of the HUD.
    rAssert( mDumpVehicle );
    VehicleAI* ai = GetVehicleCentral()->GetVehicleAI( mDumpVehicle );
    rAssert( ai );

    mDumpVehicleAI = dynamic_cast<WaypointAI*>(ai);
    rAssert( mDumpVehicleAI );
    mDumpVehicleAI->AddRef();

    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud )
    {
        //Update the collection count
        currentHud->SetCollectibles( 0, GetNumCollectibles() );

        //Now set the focus back on the car.
        currentHud->GetHudMap( 0 )->SetFocalPointIcon( mDumpVehicleAI->GetHUDIndex() );
    }

    for ( i = 0; i < WaypointAI::MAX_WAYPOINTS; ++i )
    {
        mDumpData[i].active = false;
        mDumpData[i].collected = false;
    }

    mMeDumpTimout = 0;

#ifdef DEBUGWATCH
    if ( mBumperCars )
    {
        char name[64];
        sprintf( name, "Mission\\Objectives\\BumperCar" );
        radDbgWatchAddFloat( &DEFAULT_FORCE, "Min Force", name, NULL, NULL, 0.0f, 1.0f );
        radDbgWatchAddFloat( &DEFAULT_DIST, "Min Dist", name, NULL, NULL, 0.0f, 10.0f );
        radDbgWatchAddInt( &MAX_TIMEOUT, "Max Timeout", name, NULL, NULL, 0, 10000 );
        radDbgWatchAddUnsignedInt( &CD_DEFAULT_TIMEOUT, "Dump timeout", name, NULL, 0, 100000 );
        radDbgWatchAddUnsignedInt( &DEFAULT_DUMP_LIFETIME, "Dump lifetime", name, NULL, 0, 100000 );
    }
#endif

    MissionStage* stage = GetGameplayManager()->GetCurrentMission()->GetCurrentStage();
    
    // not sure why this is needed, Init seems to be called twice in some cases
    bool alreadyHave = false;
    if ( mCondition == NULL)
    {
        mCondition = new CollectionCondition;
    }

    for(int i = 0; i < static_cast<int>( stage->GetNumConditions() ); i++)
    {

        if(stage->GetCondition(i) == mCondition)
        {            
            alreadyHave = true;
            break;
        }
    }

    if(!alreadyHave)
    {
        stage->SetCondition(stage->GetNumConditions(), mCondition);
        stage->SetNumConditions(stage->GetNumConditions() + 1);
    }
}

//=============================================================================
// CollectDumpedObjective::OnFinalizeCollectibleObjective
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void CollectDumpedObjective::OnFinalizeCollectibleObjective()
{
    GetEventManager()->RemoveAll( this );

    GetEventManager()->TriggerEvent(EVENT_DUMP_STATUS, (void*)0);

    GetGuiSystem()->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_COLLECTIBLES );

    if ( mNumCollectibles == 1 )
    {
        GetGuiSystem()->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_DAMAGE_METER );
    }

    mNumUncollected = 0 ;
    mNumSpawned = 0;

    if ( mBumperCars )
    {
        radDbgWatchDelete( (void*)(&DEFAULT_FORCE) );
        radDbgWatchDelete( (void*)(&DEFAULT_DIST) );
        radDbgWatchDelete( (void*)(&MAX_TIMEOUT) );
        radDbgWatchDelete( (void*)(&CD_DEFAULT_TIMEOUT) );
        
        GetGameplayManager()->GetCurrentVehicle()->SetVehicleCanSustainDamage(true);
        GetHitnRunManager()->RegisterVehicleImmunity( NULL );
    }
    else
    {
        mDumpVehicle->SetVehicleCanSustainDamage(true);
    }

    rAssert( mDumpVehicleAI );
    mDumpVehicleAI->Release();

    delete mAnimatedIcon;
    mAnimatedIcon = NULL;
}

//=============================================================================
// CollectDumpedObjective::OnCollection
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int collectibleNum, bool &shouldReset )
//
// Return:      bool 
//
//=============================================================================
bool CollectDumpedObjective::OnCollection( unsigned int collectibleNum, bool &shouldReset )
{
    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud )
    {
        //Update the collection count
        currentHud->SetCollectibles( GetNumCollected() + 1, GetNumCollectibles() );  //I add 1 because the number is incremented after...  Sorry
    }

    mNumUncollected--;
    GetEventManager()->TriggerEvent(EVENT_DUMP_STATUS, (void*)mNumUncollected);

    //Mark the collectible as collected.
    int i;
    for ( i = 0; i < WaypointAI::MAX_WAYPOINTS; ++i )
    {
        if ( mDumpData[i].collectibleNum == static_cast<int>(collectibleNum) )
        {
            mDumpData[i].collected = true;
            break;
        }
    }

    if ( currentHud )
    {
        if ( mNumUncollected == 0 )
        {
            //Now set the focus back on the car.
            currentHud->GetHudMap( 0 )->SetFocalPointIcon( mDumpVehicleAI->GetHUDIndex() );
        }
        else
        {
            //Select the next uncollected object.
            for ( i = 0; i < WaypointAI::MAX_WAYPOINTS; ++i )
            {
                if ( mDumpData[i].active && !mDumpData[i].collected )
                {
                    SetFocus( mDumpData[i].collectibleNum );
                }
            }
        }
    }

    //You should only be able to collect things that have been dropped, so this is always true.
    return true;
}

//=============================================================================
// CollectDumpedObjective::OnUpdateCollectibleObjective
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTimeMilliseconds )
//
// Return:      void 
//
//=============================================================================
void CollectDumpedObjective::OnUpdateCollectibleObjective( unsigned int elapsedTimeMilliseconds )
{
    //Update the arrow
    rmt::Vector carPos;
    Vehicle* vehicle = mDumpVehicle;
    vehicle->GetPosition( &carPos );

    rmt::Box3D bbox;
    vehicle->GetBoundingBox( &bbox );
    carPos.y = bbox.high.y;

    mAnimatedIcon->Move( carPos );
    mAnimatedIcon->Update( elapsedTimeMilliseconds );

    if ( mNumCollectibles == 1 )
    {
        //Update the HUD.
        // update damage meter
        if ( GetCurrentHud() )
        {
            GetCurrentHud()->SetDamageMeter( 1.0f - mDumpVehicle->GetVehicleLifePercentage(mDumpVehicle->mHitPoints) ); // between 0.0 and 1.0
        }  
    }

    for(int i = 0; i < MAX_COLLECTIBLES; i++)
    {
        if((mDumpData[ i ].lifetime > 0) && mDumpData[ i ].active && !mDumpData[ i ].collected)
        {
            if((mDumpData[ i ].lifetime - (int)elapsedTimeMilliseconds) < 0)
            {
                mDumpData[ i ].lifetime = 0;
                Activate(i, false, false);
                mDumpData[ i ].active = false;
                mDumpData[ i ].collected= false;
                --mNumUncollected;
                --mNumSpawned;
                GetEventManager()->TriggerEvent(EVENT_DUMP_STATUS, (void*)mNumUncollected);
            }
            else
            {
                mDumpData[ i ].lifetime -= elapsedTimeMilliseconds;
            }

            if(mDumpData[ i ].lifetime < 5000)
            {
                if( mCollectibles[ i ].mAnimatedIcon != NULL )
		        {
                    mCollectibles[ i ].mAnimatedIcon->ShouldRender( ((mDumpData[ i ].lifetime >> 8) & 1) != 0 );
		        }
            }
        }
    }

    if ( IsUserDumpAllowed() )
    {
        if ( mMeDumpTimout != 0 )
        {
            if ( mMeDumpTimout < elapsedTimeMilliseconds )
            {
                mMeDumpTimout = 0;
            }
            else
            {
                mMeDumpTimout -= elapsedTimeMilliseconds;
            }
        }
    }

    if(!mBumperCars) // don't fail on dropped objectives, they'll timeout anyway
    {
        for(int i = 0; i < MAX_COLLECTIBLES; i++)
        {
            if(mCollectibles[ i ].pLocator && mCollectibles[ i ].pLocator->GetFlag(Locator::ACTIVE))
            {
                rmt::Vector pos, charPos;
                mCollectibles[ i ].pLocator->GetPosition(&pos);
                GetAvatarManager()->GetAvatarForPlayer(0)->GetPosition(charPos);
                pos.Sub(charPos);
                if(pos.Magnitude() > 200)
                {
                    if(mCondition)
                    {
                        mCondition->SetViolated(true);
                    }
                    break;
                }
            }
        }
    }

    if ( mDumpTimeout == -1 || mDumpVehicle->IsAirborn() )
    {
        //NO dumping!
        return;
    }

    if ( mDumpTimeout - static_cast<int>(elapsedTimeMilliseconds) <= 0 )
    {
        if ( mAmIDumping )
        {
            DumpCollectible( mWhatToDump, GetGameplayManager()->GetCurrentVehicle(), mAssaultCar );
            Uncollect( mWhatToDump );

            ++mNumUncollected;
            GetEventManager()->TriggerEvent(EVENT_DUMP_STATUS, (void*)mNumUncollected);

            CGuiScreenHud* currentHud = GetCurrentHud();
            if( currentHud )
            {
                //Update the collection count
                currentHud->SetCollectibles( GetNumCollected(), GetNumCollectibles() );
            }

        }
        else
        {
            DumpCollectible( mWhatToDump, mDumpVehicle, GetGameplayManager()->GetCurrentVehicle(), true, mTerminalDump );
            mDumpData[ mWhatToDump ].lifetime = mDumpLifetime;

            ++mNumUncollected;
            GetEventManager()->TriggerEvent(EVENT_DUMP_STATUS, (void*)mNumUncollected);
        }
        mDumpTimeout = -1;
    }
    else
    {
        mDumpTimeout -= static_cast<int>(elapsedTimeMilliseconds);
    }
}

//=============================================================================
// CollectDumpedObjective::FindFreeSlot
//=============================================================================
int CollectDumpedObjective::FindFreeSlot(void)
{
    for(int i = 0; i < MAX_COLLECTIBLES; i++)
    {
        if(!mDumpData[ i ].active && !mDumpData[ i ].collected)
        {
            return i;
        }
    }

    return -1;
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************

