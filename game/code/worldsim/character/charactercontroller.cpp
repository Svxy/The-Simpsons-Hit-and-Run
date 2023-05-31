#include <ai/sequencer/actioncontroller.h>
#include <worldsim/character/charactercontroller.h>
#include <worldsim/character/controllereventhandler.h>
#include <worldsim/character/charactermappable.h>
#include <worldsim/character/character.h>
#include <input/inputmanager.h>
#include <mission/gameplaymanager.h>

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>
#include <camera/supercam.h>

#include <p3d/camera.hpp>
#include <p3d/pointcamera.hpp>

#include <events/eventmanager.h>

#include <worldsim/avatar.h>
#include <worldsim/avatarmanager.h>

#include <roads/geometry.h>

#include <worldsim/character/charactermanager.h>

#include <ai/sequencer/action.h>
#include <presentation/mouthflapper.h>
#include <interiors/interiormanager.h>
//#include <presentation/presentation.h>
//#include <presentation/presentationanimator.h>

/*
==============================================================================
CharacterController::CharacterController
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         CharacterController

=============================================================================
*/
CharacterController::CharacterController( void )
:
mpCharacter( 0 ),
mIntention( NONE ),
mPreserveIntention( NONE ),
mActive( true )
{
}
/*
==============================================================================
CharacterController::~CharacterControlller
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         CharacterController

=============================================================================
*/
CharacterController::~CharacterController( void )
{
    if ( mpCharacter )
    {
        mpCharacter = 0;
    }
}
/*
==============================================================================
CharacterController::GetCharacter
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         Character

=============================================================================
*/
Character* CharacterController::GetCharacter( void ) const
{
    return mpCharacter;
}
/*
==============================================================================
CharacterController::SetCharacter
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter )

Return:         void 

=============================================================================
*/
void CharacterController::SetCharacter( Character* pCharacter )
{
    mpCharacter = pCharacter;
}
/*
==============================================================================
PhysicalController::PhysicalController
==============================================================================
Description:    Constructor

Parameters:     ( void )

Return:         n/a

=============================================================================
*/
PhysicalController::PhysicalController( void )
:
mpCharacterMappable( 0 )
{
}
/*
==============================================================================
PhysicalController::~PhysicalController
==============================================================================
Description:    Destructor

Parameters:     ( void )

Return:         n/a

=============================================================================
*/
PhysicalController::~PhysicalController( void )
{
    // This should release and set mpCharacterMappable = 0;
    //
    mpCharacterMappable->Release( );
    mpCharacterMappable = 0;
}
/*
==============================================================================
PhysicalController::SetCharacterMappable
==============================================================================
Description:    Comment

Parameters:     ( CharacterMappable* pMappable )

Return:         void

=============================================================================
*/
void PhysicalController::SetCharacterMappable( CharacterMappable* pMappable )
{
    tRefCounted::Assign( mpCharacterMappable, pMappable );
}
/*
==============================================================================
PhysicalController::GetDirection
==============================================================================
Description:    Comment

Parameters:     ( rmt::Vector& outDirection )

Return:         void

=============================================================================
*/
void PhysicalController::GetDirection( rmt::Vector& outDirection )
{
    if(mActive)
    {
        mpCharacterMappable->GetDirection( outDirection );
    }
    else
    {
        outDirection.Set(0.0f, 0.0f, 0.0f);
    }
}
/*
==============================================================================
PhysicalController::GetValue
==============================================================================
Description:    Comment

Parameters:     ( int buttonId )

Return:         float

=============================================================================
*/
float PhysicalController::GetValue( int buttonId ) const
{
    if(mActive)
    {
        return GetCharacterMappable( )->GetValue( buttonId );
    }
    else
    {
        return 0.0f;
    }
}
/*
==============================================================================
PhysicalController::IsButtonDown
==============================================================================
Description:    Comment

Parameters:     ( int buttonId )

Return:         bool

=============================================================================
*/
bool PhysicalController::IsButtonDown( int buttonId ) const
{
    if(mActive)
    {
        return GetCharacterMappable( )->IsButtonDown( buttonId ) || GetIntention( ) == (eIntention)buttonId;
    }
    else
    {
        return false;
    }
}


int  PhysicalController::TimeSinceChange( int buttonId ) const 
{ 
    return IsButtonDown(buttonId) ? 0 : GetCharacterMappable( )->GetButton( buttonId )->TimeSinceChange(); 
};

/*
==============================================================================
PhysicalController::GetCharacterMappable
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         CharacterMappable

=============================================================================
*/
CharacterMappable* PhysicalController::GetCharacterMappable( void ) const
{
	return mpCharacterMappable;
}
/*
==============================================================================
CameraRelativeCharacterController::CameraRelativeCharacterController
==============================================================================
Description:    Constructor

Parameters:     ( int index, tCamera* pCamera )

Return:         n/a

=============================================================================
*/
CameraRelativeCharacterController::CameraRelativeCharacterController( void )
:
mpCamera( 0 ),
mbCameraChange( false )
{
    mpEventHandler = new CameraRelativeCharacterControllerEventHandler( this );
    GetEventManager()->AddListener( mpEventHandler, EVENT_CAMERA_CHANGE );
}
void CameraRelativeCharacterController::Create( Character* pCharacter,
                                                CharacterMappable* pCharacterMappable )
{
MEMTRACK_PUSH_GROUP( "Character Controller" );   
    SetCharacter( pCharacter );
    SetCharacterMappable( pCharacterMappable );
    pCharacterMappable->SetCharacterController( this );
MEMTRACK_POP_GROUP( "Character Controller" );
}
/*
==============================================================================
CameraRelativeCharacterController::~CameraRelativeCharacterController
==============================================================================
Description:    Destructor

Parameters:     ( void )

Return:         n/a

=============================================================================
*/
CameraRelativeCharacterController::~CameraRelativeCharacterController( void )
{
	if ( mpCamera )
	{
		mpCamera->Release( );
        mpCamera = 0;
	}
    if ( mpEventHandler )
    {
        GetEventManager()->RemoveListener( mpEventHandler, EVENT_CAMERA_CHANGE );
        delete mpEventHandler;
        mpEventHandler = 0;
    }
}


void CameraRelativeCharacterController::SetIntention( eIntention intention )
{
    if(!mActive)
    {
        return;
    }

    // ignore input if we're not in locomotion state
    if( mpCharacter->GetStateManager()->GetState() == CharacterAi::INSIM )
    {
        // TODO:
        // Should clear the intention here? or maybe just let the 
        // old intention linger? Hmm...
        mIntention = CharacterController::NONE; 
    }
    else
    {
        mIntention = intention;
    }

}


/*
==============================================================================
CameraRelativeCharacterController::GetDirection
==============================================================================
Description:    Transforms Controller relative inputs into camera relative inputs.
                The character will carry on moving the same world direction independent
                of camera position until the controller direction changes.

Parameters:     ( rmt::Vector& outDirection )

Return:         void

=============================================================================
*/
void CameraRelativeCharacterController::GetDirection( rmt::Vector& outDirection )
{
    if(!mActive)
    {
        outDirection.Set(0.0f,0.0f,0.0f);
        return;
    }

    if( mpCharacter->GetStateManager()->GetState() == CharacterAi::INSIM )
    {
        outDirection.Set( 0.0f, 0.0f, 0.0f ); // feed zero, so no walk anim
        return;
    }

    GetCharacterMappable( )->GetDirection( outDirection );

#ifdef RAD_WIN32
    SuperCam* cam = GetSuperCamManager()->GetSCC( 0 )->GetActiveSuperCam();
    if( cam->GetType() == SuperCam::PC_CAM )
    {
        //We don't do anything to modify the direction.
        return;
    }

#endif
  
    if ( mbCameraChange )
    {
        float cos15 = 0.9659258262890682867497431997289f;
        static float sfAngleTolerance = cos15;
        
        // Compare the old direction with the new direction transformed 
        // by the old matrix.
        //
        outDirection.Transform( mLastCameraMatrix );

        rmt::Vector normDir = outDirection;
        normDir.Normalize();

        rmt::Vector normLast = mLastDirection;
        normLast.Normalize();

        float dot = normDir.DotProduct( normLast );
        
        if ( dot > sfAngleTolerance )
        {
            // Direction is the "same" within given tolerance.
            //
            outDirection = mLastDirection;
        }
        else
        {
            mbCameraChange = false;
        }
    }
    else if ( mpCamera != (tCamera*)0 )
    {
        rmt::Matrix mat = mpCamera->GetCameraToWorldMatrix();
        mat.IdentityTranslation( );
        outDirection.Transform( mat );
    }

    // Rotate by the inv transform of whatever the character is 
    // standing on.
    // 
    rmt::Matrix invMat = mpCharacter->GetInverseParentTransform();
    invMat.IdentityTranslation( );
    outDirection.Transform( invMat );

    // This is designed to address analog stick hysteresis.
    // If you hold the stick in a direction and release it, often it
    // will snap back and momentarily read a value >90 deg from the intended direction.
    // this will address those momentary values.
    //
    Character* pCharacter = GetCharacter();
    rmt::Vector facing;
    rmt::Vector normDir = outDirection;
    normDir.Normalize();
    pCharacter->GetFacing( facing );
    float dot = facing.DotProduct( normDir );
    static float sfDot = 0.000001f;
    if ( dot <= -sfDot )
    {
        //rReleasePrintf( "%f dot\n", dot );
        // gt 90 deg.
        //
        static float toleranceSqr = rmt::Sqr( 0.60f );
        //rReleasePrintf( "%f MagnitudeSqr\n", outDirection.MagnitudeSqr( ) );
        if ( outDirection.MagnitudeSqr( ) < toleranceSqr )
        {
            // Small value.
            //
            static float sfScale = 0.01f;
            outDirection.Scale( sfScale );
        }
    }

}
/*
==============================================================================
CameraRelativeCharacterController::SetCamera
==============================================================================
Description:    Comment

Parameters:     ( tCamera* pCamera )

Return:         void 

=============================================================================
*/
void CameraRelativeCharacterController::SetCamera( tCamera* pCamera )
{
    tRefCounted::Assign( mpCamera, pCamera );
}

/*
==============================================================================
CameraRelativeCharacterController::HandleEvent
==============================================================================
Description:    Implements Devil May Cry style controls when the camera view switches.
                The character will carry on moving the same world direction independent
                of camera position until the controller direction changes.

Parameters:     ( EventEnum id, void* pEventData )

Return:         void 

=============================================================================
*/
void CameraRelativeCharacterController::HandleEvent( int id, void* pEventData )
{   
    EventEnum eventId = (EventEnum)id;
    
    if ( EVENT_CAMERA_CHANGE == eventId )
    {
        bool interior = GetInteriorManager()->IsEntering() || GetInteriorManager()->IsExiting() || GetInteriorManager()->IsInside();

        if (mpCamera && !mbCameraChange && !interior)
        {
            // Get absolute direction we'll transform it down below.
            //
            GetCharacterMappable( )->GetDirection( mLastDirection );

            // Get the camera matrix to store while in transition.
            //
            mLastCameraMatrix =  mpCamera->GetCameraToWorldMatrix();
            mLastCameraMatrix.IdentityTranslation( );
                    
            // Now transform mLastDirection by the camera matrix.
            // We store it to maintain the CameraRelative controls to the previous
            // camera.
            //
            // NB: We don't just call CameraRelativeCharacterController::GetDirection()
            // because that will take into account the parentTransform as well.
            //
            mLastDirection.Transform( mLastCameraMatrix );

            // Tell the controller that we want to hold onto mLastDirection.
            //
            mbCameraChange = true;

            // Update the new camera.
            //
            SuperCam* pSuperCam = static_cast<SuperCam*>( pEventData );
            tCamera* pCamera = pSuperCam->GetCamera();
            SetCamera( pCamera );
        }
    }
    else
    {
        rAssert( false );
    }
}

////////////////////////////////////////////////// NPC CONTROLLER ///////////////////////////////////////////

static const float SECONDS_BACK_TO_FOLLOW_PATH = 3.0f;
static const float SECONDS_TRACKING_PLAYER = 0.3f;
static const float NPC_FOLLOW_PATH_SPEED_MPS = 1.0f;

static const float SECONDS_BETW_PANIC_CHANGE_DIRECTION = 2.0f;
static const float NPC_PANIC_RUN_SPEED_MPS = 3.0f;

static const int TALK_TIME_MILLISECONDS = 3000;
static const int STOP_TALK_CHECK_INTERVAL_MILLISECONDS = 1000;

static const float SECONDS_BETW_PLAYING_TURN_ANIMS = 0.5f;

NPCController::NPCController()
:
mOffPath( false ),
mMillisecondsInTalk( 0 ),
mTalkTarget( NULL ),
mListening( true ),
mState( FOLLOWING_PATH ),
mSecondsInStopped( 0.0f ),
mNumNPCWaypoints( 0 ),
mCurrNPCWaypoint( -1 ),
mStartPanicking( false ),
mSecondsChangePanicDir( 0.0f ),
mSecondsSinceLastTurnAnim( SECONDS_BETW_PLAYING_TURN_ANIMS ),
mUseTempWaypoint(false),
mTempWaypoint(0.0f, 0.0f , 0.0f)
{
    mSpeedMps = GetFollowPathSpeedMps();
    mDirection.Set( 0.0f, 0.0f, 1.0f );
    mNormalizedDodgeDir.Set( 0.0f, 0.0f, 1.0f );
    mMouthFlapper = new MouthFlapper();
    mMouthFlapper->AddRef();
    mMouthFlapper->SetIsEnabled( false );


    mNormalizedPanicDir.Set( 0.0f, 0.0f, 1.0f );
}


NPCController::~NPCController( void )
{
    if( mMouthFlapper != NULL )
    {
        mMouthFlapper->Release();
    }
}

void NPCController::SetCharacter( Character *pCharacter )
{
    CharacterController::SetCharacter( pCharacter );
}

bool NPCController::AddNPCWaypoint( const rmt::Vector& pt )
{
    // can't add another point
    if( mNumNPCWaypoints >= MAX_NPC_WAYPOINTS )
    {
        return false;
    }
    
    if( mNumNPCWaypoints == 0 )
    {
        // set our sights on the first waypoint
        mCurrNPCWaypoint = 0;
    }
    mNPCWaypoints[ mNumNPCWaypoints ] = pt;
    mNumNPCWaypoints++;

    return true;
}
void NPCController::IncitePanic()
{
    // he's gonna run, so let him RUN .... into things...
    //GetCharacter()->SetSolveCollisions( true );

    // flag this guy to start panicking, if he can't panick right now..
    mStartPanicking = true;

    // don't panick if already panicking or in limbo (deliberately set there)
    if( mState == NPCController::PANICKING || mState == NPCController::NONE )
    {
        return;
    }
    /*
    // if not in an ideal state for panicking...
    if( mState != NPCController::FOLLOWING_PATH &&
        mState != NPCController::STANDING && 
        mState != NPCController::STOPPED )
    {
        return;
    }
    */

    TransitToState( NPCController::PANICKING );

}
void NPCController::QuellPanic()
{
    mStartPanicking = false;
    TransitToState( NPCController::STOPPED );
}



void NPCController::Update( float seconds )
{

    if( mpCharacter == NULL )
    {
        return;
    }

    // if character in simulation, no point
    if( mpCharacter->GetSimState()->GetControl() == sim::simSimulationCtrl )
    {
        return;
    }

    // if character is not in locomotion state, no point...
    if( mpCharacter->GetStateManager()->GetState() != CharacterAi::LOCO )
    {
        return;
    }

    //BEGIN_PROFILE( "NPCController::Update" );

    // Grab the simstate control. we'll be checking this sporadically.
    int control = mpCharacter->GetSimState()->GetControl();

    if( mStartPanicking )
    {
        IncitePanic();
    }

    switch( mState )
    {
    case FOLLOWING_PATH:
        {
            //rAssert( control == sim::simAICtrl );
            FollowPath( seconds );
            DetectAndDodge( seconds );
        }
        break;
    case STOPPED:
        {
            //rAssert( control == sim::simAICtrl );
            mSecondsInStopped += seconds;
            mSecondsSinceLastTurnAnim += seconds;

            // set to 1.5f and they'll follow you terminator style!
            mSpeedMps = 0.0f;
            mpCharacter->SetSpeed( mSpeedMps );

            //
            // Track player for awhile then we can start deciding
            // whether or not we want/need to dodge...
            //
            rmt::Vector playerPos, myPos, myFacing;

            Avatar* avatar = ::GetAvatarManager()->GetAvatarForPlayer(0);
            avatar->GetPosition( playerPos );

            mpCharacter->GetPosition( &myPos );

            mpCharacter->GetFacing( myFacing );
            //rAssert( rmt::Epsilon( myFacing.MagnitudeSqr(), 1.0f, 0.001f ) );

            rmt::Vector toPlayer = playerPos - myPos;
            toPlayer.NormalizeSafe();
            rAssert( rmt::Epsilon( toPlayer.MagnitudeSqr(), 1.0f, 0.001f ) );

            //cosN for N being > half the animation turn angle
            const float COSALPHA_TURN_TOLERANCE = 0.8387f; 

            // cosN, so anything > than N we just set the direction directly rather than play anim
            const float COSALPHA_QUICKTURN_TOLERANCE = 0.7071f; 

            float cosAlpha = myFacing.Dot( toPlayer );
            if( cosAlpha < COSALPHA_QUICKTURN_TOLERANCE )
            {
                // just turn (no anim)
                SetDirection( toPlayer );
                float dir = choreo::GetWorldAngle( toPlayer.x, toPlayer.z );
                mpCharacter->SetDesiredDir( dir );

                // reset so we can do turn anim right away after this
                mSecondsSinceLastTurnAnim = SECONDS_BETW_PLAYING_TURN_ANIMS;
            }
            else if( COSALPHA_QUICKTURN_TOLERANCE <= cosAlpha && cosAlpha <= COSALPHA_TURN_TOLERANCE )
            {
                if( mSecondsSinceLastTurnAnim >= SECONDS_BETW_PLAYING_TURN_ANIMS )
                {
                    // figure out whether to turn left or right
                    rmt::Vector myRight = Get90DegreeRightTurn( myFacing );
                    if( myRight.Dot( toPlayer ) > 0.0f )
                    {
                        // player's on my right, turn right
                        SetIntention( TurnRight );
                    }
                    else
                    {
                        // player's on my left, turn left
                        SetIntention( TurnLeft );
                    }
                    mSecondsSinceLastTurnAnim = 0.0f; // back to zero!
                }
            }

            if( mSecondsInStopped > SECONDS_BACK_TO_FOLLOW_PATH )
            {

                // if we are going to be too close to other characters, transit
                // to stopped here.
                CharacterManager* cm = GetCharacterManager();

                const float TOO_CLOSE_TO_NPC_DIST_SQR = 2.0f;

                //
                // if I'm a ped, I can ignore other peds (in THIS check only) cuz I already
                // deal with them safely elsewhere... If I'm a non-ped NPC, I would 
                // want to check for peds, so I don't walk through them.
                //
                bool ignorePeds = (mpCharacter->GetRole() == Character::ROLE_PEDESTRIAN)? true : false;

                int maxChars = cm->GetMaxCharacters();

                bool gonnaHit = false;

                for( int i=0; i<maxChars; i++ )
                {
                    Character* c = cm->GetCharacter( i );
                    if( c && c != mpCharacter )
                    {
                        if( ignorePeds && c->GetRole() == Character::ROLE_PEDESTRIAN )
                        {
                            continue;
                        }
                        rmt::Vector cPos;
                        c->GetPosition( cPos );
                        
                        float distSqr = (cPos - myPos).MagnitudeSqr();
                        if( distSqr <= TOO_CLOSE_TO_NPC_DIST_SQR )
                        {
                            gonnaHit = true;
                            break;
                        }
                    }
                }

                if( gonnaHit )
                {
                    mSecondsInStopped = 0.0f;
                }
                else
                {
                    TransitToState(FOLLOWING_PATH);
                }
            }
            //
            // If we've been tracking player long enough, 
            // time to dodge!!!
            //
            if( mSecondsInStopped > SECONDS_TRACKING_PLAYER )
            {
                DetectAndDodge( seconds );
            }

        }
        break;
    case DODGING:
        {
            //rAssert( control == sim::simAICtrl );
            // so NPCs don't dodge into someplace indeterminate
            //mpCharacter->SetSolveCollisions( true );

            // sets NeedsUpdate flag for character (so it doens't pause
            // in midmotion if player moves out of physics simming radius)
            mpCharacter->Update( seconds );
        }
        break;
    case CRINGING:
        {
            //rAssert( control == sim::simAICtrl );
        }
        break;
    case TALKING:
        {
            rAssert( mTalkTarget != NULL );
            //rAssert( control == sim::simAICtrl );

            int coinflip = 0;

            // Direction is set for us when we transited to this state
            // it never needs changing.
            //SetDirection( );

            // if target is no longer in TALKING state, we stop too... 
            if( mTalkTarget->GetState() != TALKING )
            {
                TransitToState(FOLLOWING_PATH);
                break;
            }

            if( mListening )
            {
                rAssert( mTalkTarget != NULL );
                rAssert( !mMouthFlapper->IsEnabled() );
                rAssert( !mTalkTarget->mListening ); // one of us needs to do the talking! geez...
                rAssert( mTalkTarget->mMouthFlapper->IsEnabled() );

                mMillisecondsInTalk = 0;
            }
            else
            {
                rAssert( mTalkTarget != NULL );
                rAssert( mMouthFlapper->IsEnabled() );
                rAssert( mTalkTarget->mListening ); // only one of us can talk at once! geez...
                rAssert( !mTalkTarget->mMouthFlapper->IsEnabled() );

                mMillisecondsInTalk += (int)(seconds*1000);
                if( mMillisecondsInTalk >= TALK_TIME_MILLISECONDS )
                {
                    // ok, by now we've talked for longer than alotted time, 
                    // so every check interval, we have a chance to 
                    // stop talking and let the other person start
                    // talking.

                    if( (mMillisecondsInTalk - TALK_TIME_MILLISECONDS) >= 
                        STOP_TALK_CHECK_INTERVAL_MILLISECONDS )
                    {
                        mMillisecondsInTalk = TALK_TIME_MILLISECONDS;

                        coinflip = rand() % 3; 
                        // 2 in 3 chance in favor of stopping our yammer
                        if( coinflip != 2 ) 
                        {
                            StopTalking();
                            mTalkTarget->StartTalking();
                        }
                    }
                }
            }

            /*
            // there's a good chance we won't bother to detect cars while talking... uh oh!
            coinflip = rand() % 1000;
            if( coinflip == 0 )
            {
                DetectAndDodge( seconds );
            }
            */
        }
        break;
    case STANDING:
        {
            //rAssert( control == sim::simAICtrl );
            mSecondsInStopped += seconds;
            if( mSecondsInStopped > SECONDS_BACK_TO_FOLLOW_PATH )
            {
                TransitToState(FOLLOWING_PATH);
                break;
            }
            DetectAndDodge( seconds );
        }
        break;
    case PANICKING:
        {
            //rAssert( control == sim::simAICtrl );

            rmt::Vector myPos;
            mpCharacter->GetPosition( myPos );

            mSecondsChangePanicDir += seconds;
            if( mSecondsChangePanicDir > SECONDS_BETW_PANIC_CHANGE_DIRECTION )
            {
                Avatar* player = GetAvatarManager()->GetAvatarForPlayer( 0 );
                rAssert( player );

                rmt::Vector playerPos;
                player->GetPosition( playerPos );

                rmt::Vector awayFromPlayer = myPos - playerPos;

                // vary direction of panic run by modifying at random .x & .z 
                // of the awayFromPlayer vector...

                float randomXVariance = 0.0f;
                float randomZVariance = 0.0f;
                const float FAR_ENOUGH_TO_CHANGE_DIR_SQR = 36.0f;
                if( awayFromPlayer.MagnitudeSqr() > FAR_ENOUGH_TO_CHANGE_DIR_SQR )
                {
                    randomXVariance = (float)(rand()%500) / 100.0f;
                    randomXVariance *= (rand() % 2)? 1.0f : -1.0f;
                    randomZVariance = (float)(rand()%500) / 100.0f;
                    randomZVariance *= (rand() % 2)? 1.0f : -1.0f;
                }
                awayFromPlayer.x += randomXVariance;
                awayFromPlayer.z += randomZVariance;

                awayFromPlayer.NormalizeSafe(); // *** SQUARE ROOT! ***

                mNormalizedPanicDir = awayFromPlayer;
                mSecondsChangePanicDir = 0.0f;
            }
            SetDirection( mNormalizedPanicDir );

        }
        break;
    case TALKING_WITH_PLAYER: // fall thru
    case NONE:
        break; // do nothing
    default:
        {
            // bad state
            rAssert( false );
        }
    }
    //END_PROFILE( "NPCController::Update" );
}

float NPCController::GetFollowPathSpeedMps() const
{
    if( mOffPath )
    {
        return NPC_FOLLOW_PATH_SPEED_MPS;        
    }
    if( mNumNPCWaypoints > 1 )
    {
        return NPC_FOLLOW_PATH_SPEED_MPS;
    }
    return 0.0f;
}


void NPCController::FollowPath( float seconds )
{
    // if we're in collision, we should transit to STOPPED.
    if( mpCharacter->mbCollidedWithVehicle )
    {
        TransitToState( STOPPED );
        return;
    }

    // get our pos
    rmt::Vector currPos;
    mpCharacter->GetPosition( &currPos );


    // if we are going to be too close to other characters, transit
    // to stopped here.
    CharacterManager* cm = GetCharacterManager();

    const float TOO_CLOSE_TO_NPC_DIST_SQR = 2.0f;

    //
    // if I'm a ped, I can ignore other peds (in THIS check only) cuz I already
    // deal with them safely elsewhere... If I'm a non-ped NPC, I would 
    // want to check for peds, so I don't walk through them.
    //
    bool ignorePeds = (mpCharacter->GetRole() == Character::ROLE_PEDESTRIAN)? true : false;

    int maxChars = cm->GetMaxCharacters();
    for( int i=0; i<maxChars; i++ )
    {
        Character* c = cm->GetCharacter( i );
        if( c && c != mpCharacter )
        {
            if( ignorePeds && c->GetRole() == Character::ROLE_PEDESTRIAN )
            {
                continue;
            }
            rmt::Vector cPos;
            c->GetPosition( cPos );
            
            float distSqr = (cPos - currPos).MagnitudeSqr();
            if( distSqr <= TOO_CLOSE_TO_NPC_DIST_SQR )
            {
                TransitToState( STOPPED );             
                return;
            }
        }
    }


    //mpCharacter->SetSolveCollisions( true );

    mSpeedMps = GetFollowPathSpeedMps(); 
    mpCharacter->SetSpeed( mSpeedMps );

    // If we're off our path, pathfind way back on path
    rmt::Vector lastPos;

    //rAssert( 1 <= mNumNPCWaypoints && mNumNPCWaypoints < MAX_NPC_WAYPOINTS );
    //rAssert( 0 <= mCurrNPCWaypoint && mCurrNPCWaypoint < mNumNPCWaypoints );

    // Find the good pos on path that we want to be at, or the point 
    // itself if no waypoints, ok?
    if(mUseTempWaypoint)
    {
        lastPos = mTempWaypoint;
        if((lastPos - mNPCWaypoints[0]).Magnitude() < 20.0f) // were close to our path again, clear the temp waypoint
        {
            lastPos = mNPCWaypoints[0]; 
            mUseTempWaypoint = false;
        }
    }
    else if( mNumNPCWaypoints == 1 )
    {
        lastPos = mNPCWaypoints[0]; // only one waypoint, so this is it.
    }
    else if( mNumNPCWaypoints > 1 )
    {
        rmt::Vector start, end;
        end = mNPCWaypoints[ mCurrNPCWaypoint ];
        
        int lastPt = mCurrNPCWaypoint - 1;
        if( lastPt < 0 )
        {
            lastPt = mNumNPCWaypoints - 1;
        }
        rAssert( 0 <= lastPt && lastPt < mNumNPCWaypoints );
        start = mNPCWaypoints[ lastPt ];

        end.y = start.y = currPos.y; // ignore heights

        FindClosestPointOnLine( start, end, currPos, lastPos );
    }
    else
    {
        // do nothing!
        lastPos = currPos;
    }

    rmt::Vector epsilon;
    GetAllowedPathOffset( epsilon );

    bool same = false;
    if( rmt::Epsilon( currPos.x, lastPos.x, epsilon.x ) &&
        //rmt::Epsilon( currPos.y, lastPos.y, epsilon.y ) && // IGNORE y VALUE
        rmt::Epsilon( currPos.z, lastPos.z, epsilon.z ) )
    {
        same = true;
    }

    if( !mOffPath && same )
    {
        // we were on path, and we're still on path
        return;
    }
    else if( mOffPath && same )
    {
        // we were off path, now we're on path
        mOffPath = false;
        return;
    }
    else if( !same )
    {
        // we are not on path...
        mOffPath = true;

        bool tooFar = (lastPos - currPos).Magnitude() > 100.0f;

        // If no one is looking, might as well warp baby!
        if( !mpCharacter->mbInAnyonesFrustrum || tooFar)
        {
            // Make sure where we're warping to is also not in anyone's frustrum...
            bool bLastPosInAnyonesFrustrum = false; 
            for( int i=0; i<GetGameplayManager()->GetNumPlayers(); i++ )
            {
                if( mpCharacter->PosInFrustrumOfPlayer( lastPos, i ) )
                {
                    bLastPosInAnyonesFrustrum = true;
                    break;
                }
            }
            if( !bLastPosInAnyonesFrustrum || tooFar)
            {
                float facing = mpCharacter->GetFacingDir();
                mpCharacter->RelocateAndReset( lastPos, facing, false );
                mOffPath = false;
                return;
            }
        }

        //mpCharacter->SetSolveCollisions( true );
        OnOffPath( lastPos, currPos );
    }
}

void NPCController::TeleportToPath( void )
{
    float facing = mpCharacter->GetFacingDir();
    mpCharacter->RelocateAndReset( mNPCWaypoints[0], facing, false );
    mOffPath = false;
    ClearTempWaypoint();
}

void NPCController::DetectAndDodge( float seconds )
{
    // if we're in a street race, don't do any detecting or dodging
    // cuz it could put us outside the race props boundaries.
    Mission* m = GetGameplayManager()->GetCurrentMission();
    if( m && m->mIsStreetRace1Or2 )
    {
        rmt::Vector dir( 0.0f, 0.0f, 0.0f );
        SetDirection( dir );
        mSpeedMps = 0.0f;
        mpCharacter->SetSpeed( mSpeedMps );
        return;
    }


    float hisVel = 0.0f;
    float epsilon = 0.001f;

    bool willGetHit = Detect( seconds, hisVel );

    if( willGetHit )
    {
        //If we haven't tried stopping...
        if( mState != STOPPED )
        {
            TransitToState( STOPPED );
        }
        // ok, we're stopped, but are they're still coming?
        else 
        {
            Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
            rAssert( avatar != NULL );

            if( hisVel > epsilon ) // we're already stopped, but he's still coming
            {
                // Well if they're not in a vehicle, who cares!
                if( !avatar->IsInCar() )
                {
                    return;
                }

                int coinflip = rand() % 2;
                if( coinflip == 0 )
                {
                    //TransitToState(CRINGING);
                    PerformCringe();
                    return;
                }
                else
                {
                    //TransitToState(DODGING);
                    PerformDodge( );
                    return;
                }
            }
        }
    }

    //
    // The reason we do this after is that we may have needed to transit 
    // back to STOPPED (in the code above) after we've just transitted 
    // from there to FOLLOWING_PATH because we might hit the player.
    // So if the state is still FOLLOWING_PATH despite detecting and 
    // dodging, we'll go ahead and traverse the pedpathsegments
    //
    if( mState == FOLLOWING_PATH )
    {
        TraversePath( seconds );
    }
}


void NPCController::TraversePath( float seconds )
{
    rmt::Vector dir( 0.0f, 0.0f, 0.0f );
    if( mOffPath )
    {
        // allow pathfinding back (don't set speed to zero or set direction to zero)
        return;
    }

    if( mCurrNPCWaypoint == -1 )
    {
        SetDirection( dir );
        mSpeedMps = 0.0f;
        mpCharacter->SetSpeed( mSpeedMps );
        return;
    }

    if( (mNumNPCWaypoints < 2) || mUseTempWaypoint)
    {
        SetDirection( dir );
        mSpeedMps = 0.0f;
        mpCharacter->SetSpeed( mSpeedMps );
        return;
    }

    // loop back to the first waypoint
    if( mCurrNPCWaypoint >= mNumNPCWaypoints )
    {
        mCurrNPCWaypoint = 0;
    }

    rAssert( 0 <= mCurrNPCWaypoint && mCurrNPCWaypoint < mNumNPCWaypoints );
    rmt::Vector wayPos = mNPCWaypoints[ mCurrNPCWaypoint ];

    rmt::Vector myPos;
    GetCharacter()->GetPosition( myPos );

    // if player not close enough, don't bother to do this...
    rmt::Vector playerPos;
    GetAvatarManager()->GetAvatarForPlayer( 0 )->GetPosition( playerPos );
    
    const float DIST_PLAYER_TOO_FAR_TO_BOTHER_SQR = 22500.0f;
    if( (playerPos-myPos).MagnitudeSqr() >= DIST_PLAYER_TOO_FAR_TO_BOTHER_SQR )
    {
        SetDirection( dir );
        mSpeedMps = 0.0f;
        mpCharacter->SetSpeed( mSpeedMps );
        return;
    }

    // ignore height differences
    wayPos.y = myPos.y;
    dir = wayPos - myPos;

    float distToWaypoint = dir.Magnitude(); // *** SQUARE ROOT! ***
    const float DIST_CLOSE_ENOUGH_TO_NPC_WAYPOINT = 0.5f;
    if( distToWaypoint < DIST_CLOSE_ENOUGH_TO_NPC_WAYPOINT )
    {
        // increment curr waypoint
        mCurrNPCWaypoint++;
        if( mCurrNPCWaypoint >= mNumNPCWaypoints )
        {
            mCurrNPCWaypoint = 0; // increment for next round
        }

        // Non-ped NPCs will want to do some idle anims... Peds
        // will just keep on going...
        OnReachedWaypoint();
    }

    if( distToWaypoint > 0.0f )
    {
        SetDirection( dir / distToWaypoint );
        mSpeedMps = GetFollowPathSpeedMps();
        mpCharacter->SetSpeed( mSpeedMps );
    }
}

void NPCController::OnReachedWaypoint()
{
    /*
    // Reached a waypoint, so do some anim here if we're in the player's
    // frustrum
    if( mpCharacter->IsVisible() && mpCharacter->mbInAnyonesFrustrum )
    {
        PresentationAnimator* npcAnimator = GetPresentationManager()->GetAnimatorNpc();
        Character* oldCharacter = npcAnimator->GetCharacter();
        const bool oldRandomSelection = npcAnimator->GetRandomSelection();

        npcAnimator->SetCharacter( mpCharacter );
        npcAnimator->SetRandomSelection( true );
        npcAnimator->PlaySpecialAmbientAnimation();
        npcAnimator->SetCharacter( oldCharacter );
        npcAnimator->SetRandomSelection( oldRandomSelection );
    }
    */
}



static const float DODGE_SPEED_MPS = 1.0f;  // doesn't seem to matter as long as > 0.0f
static const float CRINGE_SPEED_MPS = 0.0f;  

static const float VEHICLE_SPAN_METERS = 1.5f;
static const float CHARACTER_SPAN_METERS = 0.5f;
static const float COLLISION_LOOK_AHEAD_SECONDS = 1.0f;
static const float COLLISION_DISTANCE_SQUARED = 400.0f; // must be within this to calc collisions

bool NPCController::Detect( float seconds, float& hisVel )
{
    hisVel = 0.0f; // init return value to something

    // Clear out previous use of mDodgeInfo
    mDodgeInfo.wasSetThisFrame = false;


    // Things we use over and over again in this function
    rmt::Vector playerPos, playerHeading, playerSide, playerUp, playerVel;
    rmt::Vector myPos, myHeading, mySide, myUp, myVel;


    //////////////////////////////////////
    // Information about player
    //////////////////////////////////////
    Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
    rAssert( avatar != NULL );
    avatar->GetPosition( playerPos );
    float playerCollisionRadius = 
        (avatar->IsInCar())? VEHICLE_SPAN_METERS : CHARACTER_SPAN_METERS;


    //////////////////////////////////////
    // Information about self
    //////////////////////////////////////
    mpCharacter->GetPosition( myPos );
    float myCollisionRadius = CHARACTER_SPAN_METERS; 


    //////////////////////////////////////
    // Detect if player will hit us 
    //////////////////////////////////////

    //
    // CRUDE TEST
    // ==========
    // Test against hard-coded distanceSquared to see if we're 
    // close enough to player to even bother to proceed...
    //
    rmt::Vector myPosToPlayerPos = playerPos - myPos;
    if( myPosToPlayerPos.MagnitudeSqr() > COLLISION_DISTANCE_SQUARED )
    {
        return false;
    }

    //
    // MORE DETAILED TEST
    // ===================
    // Treat everything only on x-z plane. In other words, we won't 
    // detect a vehicle coming at us from above or below.
    //

    // Get more info about player and self
    float epsilon = 0.005f;
    bool gonnaHit = false;
    float spanDistSum = playerCollisionRadius + myCollisionRadius;

    myPos.y = 0.0f;
    mpCharacter->GetFacing( myHeading );
    rAssert( rmt::Epsilon(myHeading.MagnitudeSqr(), 1.0f, epsilon) );
    myVel = myHeading * GetSpeedMps();
    myVel.y = 0.0f;
    myUp.Set( 0.0f, 1.0f, 0.0f );
    float mySpeedMps = myVel.Length();          // *** SQUARE ROOT! ***


    playerPos.y = 0.0f;
    avatar->GetVelocity( playerVel );
    playerVel.y = 0.0f;
    playerUp.Set( 0.0f, 1.0f, 0.0f );
    float playerSpeedMps = playerVel.Length();  // *** SQUARE ROOT! ***

    hisVel = playerSpeedMps;

    /////////////////////////////////////////////////////////
    // If player is stationary 
    //
    if( playerSpeedMps <= epsilon || avatar->GetCharacter()->mbIsPlayingIdleAnim )
    {
        mySide.CrossProduct( myUp, myHeading );
        mySide.Normalize();

        // sanity check lengths to 1.0f. myHeading should not be 0.0f since
        // we're in the case where mySpeedMps > 0.0f. mySide shouldn't be 0.0f
        // since we made myUp and myHeading orthonormal

        rAssert( rmt::Epsilon( myHeading.MagnitudeSqr(), 1.0f, epsilon ) );
        rAssert( rmt::Epsilon( mySide.MagnitudeSqr(), 1.0f, epsilon ) );

        float lookAheadDist = GetFollowPathSpeedMps() * COLLISION_LOOK_AHEAD_SECONDS + spanDistSum;

        //float lookAheadDist = mySpeedMps * COLLISION_LOOK_AHEAD_SECONDS + spanDistSum;
        bool playerPosLiesOnMyRight;

        gonnaHit = WillCollide( myPos, 
                                myHeading, // Must be normalized to 1
                                mySide,    // Must be normalized to 1
                                spanDistSum, 
                                lookAheadDist,
                                playerPos,
                                playerPosLiesOnMyRight );
    }
    //////////////////////////////////////////////////////////////
    // If player is not stationary, but I am... 
    //
    else if( playerSpeedMps > epsilon && mySpeedMps <= epsilon ) 
    {
        playerHeading = playerVel / playerSpeedMps;
        playerSide.CrossProduct( playerUp, playerHeading );
        playerSide.Normalize();

        rAssert( rmt::Epsilon( playerHeading.MagnitudeSqr(), 1.0f, epsilon ) );
        rAssert( rmt::Epsilon( playerSide.MagnitudeSqr(), 1.0f, epsilon ) );

        float lookAheadDist = playerSpeedMps * COLLISION_LOOK_AHEAD_SECONDS + spanDistSum;
        bool myPosLiesOnPlayersRight;

        gonnaHit = WillCollide( playerPos, 
                                playerHeading,  // Must be normalized to 1
                                playerSide,     // Must be normalized to 1
                                spanDistSum, 
                                lookAheadDist,
                                myPos,
                                myPosLiesOnPlayersRight );

        //
        // Store Dodge info to be used in PerformDodge
        // because this should be the only control flow that allows you
        // to call PerformDodge() later.
        //
        if( gonnaHit )
        {
            mDodgeInfo.wasSetThisFrame = true;
            mDodgeInfo.myPosIsOnPlayersRightSide = myPosLiesOnPlayersRight;
            mDodgeInfo.playerRightSide = playerSide;
        }
    }
    /////////////////////////////////////////////////////////////
    // If both moving, gotta do full collision test
    //
    else 
    {
        rAssert( playerSpeedMps > epsilon && mySpeedMps > epsilon );
        //
        // Easiest, Crudest way to go about this is to do sphere-sphere 
        // collision on the 2 motion vectors. We don't need sophisticated
        // check because this only provides an early warning about nearby
        // activities...
        //

        //
        // Compute future positions
        //
        rmt::Vector playerPos2, myPos2;
        playerPos2 = playerPos + playerVel * COLLISION_LOOK_AHEAD_SECONDS;
        myPos2 = myPos + myVel * COLLISION_LOOK_AHEAD_SECONDS;

        //
        // Find midpoints & radii
        //
        rmt::Vector playerMid, myMid;
        playerMid = ( playerPos2 + playerPos ) * 0.5f;
        myMid = (myPos2 + myPos ) * 0.5f;

        float playerMotionRadius, myMotionRadius;
        playerMotionRadius = playerSpeedMps * COLLISION_LOOK_AHEAD_SECONDS * 0.5f;
        myMotionRadius = mySpeedMps * COLLISION_LOOK_AHEAD_SECONDS * 0.5f;

        //
        // Test for sphere-sphere collision
        //
        float minDist = myMotionRadius + playerMotionRadius;
        float minDistSqr = minDist * minDist;
        float actualDistSqr = (playerMid - myMid).MagnitudeSqr();

        if( actualDistSqr < minDistSqr )
        {
            gonnaHit = true;
        }
        else
        {
            gonnaHit = false;
        }

    }
    return gonnaHit;

}


void NPCController::PerformCringe( )
{
    SetIntention( Cringe );
    SetSpeedMps( CRINGE_SPEED_MPS );

    // Determine direction...
    rmt::Vector myPos, playerPos;
    mpCharacter->GetPosition( myPos );
    
    Avatar* avatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
    rAssert( avatar != NULL );
    avatar->GetPosition( playerPos );

    rmt::Vector toPlayer = playerPos - myPos;
    toPlayer.Normalize(); // *** SQUARE ROOT! ***

    SetDirection( toPlayer );
}

void NPCController::PerformDodge( )
{
    if( !mDodgeInfo.wasSetThisFrame )
    {
        return;
    }

    // NOTE: 
    // I wouldn't be here unless I transited from STOPPED state,
    // meaning my speed was 0.0f when Detect() was called just before
    // this method was called, meaning that mDodgeInfo should 
    // have been set by the control flow through Detect() that 
    // happens when I am stationary and player is not.

    // Set Controller information
    SetIntention( Dodge );
    SetSpeedMps( DODGE_SPEED_MPS );

    // Determine which way to dodge...
    if( mDodgeInfo.myPosIsOnPlayersRightSide )
    {
        mNormalizedDodgeDir = mDodgeInfo.playerRightSide;
    }
    else
    {
        mNormalizedDodgeDir = mDodgeInfo.playerRightSide * -1;
    }
    SetDirection( mNormalizedDodgeDir );

    //
    // Say funny ha-ha line
    //
    GetEventManager()->TriggerEvent( EVENT_PEDESTRIAN_DODGE, mpCharacter );
}

void NPCController::GetDirection( rmt::Vector& outDirection )
{
    // mDirection initialized and kept at 0,0,0
    rAssert( mpCharacter->GetMaxSpeed() > 0.0f );
    outDirection = mDirection * (mSpeedMps/mpCharacter->GetMaxSpeed());
}

void NPCController::TransitToState( State state )
{
    // If in TALKING but transiting to state other than TALKING,
    // must StopTalking().
    if( mState == TALKING && state != TALKING )
    {
        StopTalking();
        mTalkTarget = NULL;
    }

    mpCharacter->SetInSubstep( false );

    switch( state )
    {
    case TALKING:
        {
            // initially, tell anyone who transits to TALKING to start
            // by listening. It's up to the function that sets them TALKING
            // to call StartTalking() on one of the parties...
            mListening = true; 
            mSpeedMps = 0.0f;
            mpCharacter->SetSpeed( mSpeedMps );
        }
        break;
    case TALKING_WITH_PLAYER: // fall thru
    case STOPPED: // fall thru
    case STANDING: // fall thru
    case NONE: 
        {
            mSpeedMps = 0.0f;
            mpCharacter->SetSpeed( mSpeedMps );
            mSecondsInStopped = 0.0f;
            mSecondsSinceLastTurnAnim = 0.0f;
        }
        break;
    case PANICKING:
        {
            // test to see if we were ever told to transit to panic 
            // while already in panic. This isn't anything fatal, it's just
            // more work than necessary
            rAssert( mState != PANICKING );

            mSpeedMps = NPC_PANIC_RUN_SPEED_MPS;
            mpCharacter->SetSpeed( mSpeedMps );
            mSecondsChangePanicDir = 0.0f;

            Avatar* player = GetAvatarManager()->GetAvatarForPlayer( 0 );
            rAssert( player );

            rmt::Vector myPos, playerPos;
            mpCharacter->GetPosition( myPos );
            player->GetPosition( playerPos );

            rmt::Vector awayFromPlayer = myPos - playerPos;
            awayFromPlayer.NormalizeSafe(); // *** SQUARE ROOT! ***

            mNormalizedPanicDir = awayFromPlayer;
        }
        break;
    case DODGING:
        {
            mpCharacter->SetInSubstep( true );
        }
        break;
    default:
        {
            //nothing
        }
    // Put other cases here if they need specific operations...
    }

    mState = state;
}
void NPCController::GetAllowedPathOffset( rmt::Vector& offset )
{
    offset.Set( 1.0f, 1.0f, 1.0f );
}

void NPCController::OnOffPath( rmt::Vector lastPos, rmt::Vector currPos )
{
    rAssert( mOffPath );

    // set direction...
    rmt::Vector dirBackToPath = lastPos - currPos;
    dirBackToPath.y = 0.0f;

    dirBackToPath.Normalize(); // *** SQUARE ROOT! ***
    SetDirection( dirBackToPath );
}


void NPCController::StartTalking()
{
    if( mListening && mState == TALKING )
    {
        mMouthFlapper->SetCharacter( mpCharacter );
        mMillisecondsInTalk = 0;
        mListening = false;
        mpCharacter->GetPuppet()->GetEngine()->GetPoseEngine()->AddPoseDriver( 2, mMouthFlapper );
        mMouthFlapper->SetIsEnabled( true );
    }
}
void NPCController::StopTalking()
{
    if( !mListening && mState == TALKING )
    {
        mMillisecondsInTalk = 0;
        mListening = true;
        mMouthFlapper->SetIsEnabled( false );
        mpCharacter->GetPuppet()->GetEngine()->GetPoseEngine()->RemovePoseDriver( 2, mMouthFlapper );
    }
}

void NPCController::SetTempWaypont(const rmt::Vector& w)
{
    mTempWaypoint = w;
    mUseTempWaypoint = true;
}

void NPCController::ClearTempWaypoint(void)
{
    mUseTempWaypoint = false;
}
