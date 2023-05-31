#include <p3d/utility.hpp>
#include <p3d/anim/multicontroller.hpp>
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/anim/animatedobject.hpp>
#include <p3d/anim/animate.hpp>

#include "stateprop/stateprop.hpp"
#include "stateprop/statepropdata.hpp"
#include <ai/actor/actoranimation.h>

using namespace StatePropDataTypes;

CStateProp* CStateProp::CreateStateProp( CStatePropData* statePropData , unsigned int state, tPose* pose )
{
    tAnimatedObjectFactory* objectFactory = statePropData->GetAnimatedObjectFactory();

    P3DASSERT( objectFactory );

    if ( objectFactory )
	{
		tAnimatedObject* animatedObject = objectFactory->CreateObject(NULL, pose);
		CStateProp* stateProp = new CStateProp( animatedObject , statePropData , state );
        stateProp->SetName( statePropData->GetName() );
		return stateProp;
	}

	return NULL;
}

CStateProp::CStateProp( tAnimatedObject* animatedObject , CStatePropData* statePropData , unsigned int state ) :
    m_StatePropData( NULL ),
	m_AnimatedObject( NULL ),
	m_BaseFrameController( NULL ), 
    m_FastDisplayDrawable( NULL ),
    m_CurrentState( state ),
    m_NumStatePropListeners( 0 )
{
    //set state data
    P3DASSERT( statePropData );
    m_StatePropData = statePropData;
	m_StatePropData->AddRef();

    //set animated object
    P3DASSERT( animatedObject );
	m_AnimatedObject = animatedObject;
	m_AnimatedObject->AddRef();

	//base frame controller is run in milliseconds (ie the 1 is the timer)
	m_BaseFrameController = new tMultiController(0 , 1000.f);
	m_BaseFrameController->AddRef();
	m_BaseFrameController->Reset();

	//initialize the state
	SetState( state ); 
}

CStateProp::~CStateProp()
{
	if ( m_BaseFrameController )
		m_BaseFrameController->Release();
	if ( m_StatePropData )
		m_StatePropData->Release();
	if ( m_AnimatedObject )
		m_AnimatedObject->Release();

    if ( m_FastDisplayDrawable )
        m_FastDisplayDrawable->Release();
}

void CStateProp::Update( float dt )
{
	const TransitionData& tansdata = m_StatePropData->GetTransitionData( m_CurrentState );

    unsigned int i;

	float lastFrame = m_BaseFrameController->GetFrame();
	m_BaseFrameController->Advance( dt );
	float newFrame = m_BaseFrameController->GetFrame();

	//Check out transition
	if ( tansdata.autoTransition )
	{
		if (tansdata.onFrame >= lastFrame && tansdata.onFrame < newFrame)
		{
			SetState( tansdata.toState );
		}
	}

	//Check callback events
    if ( m_NumStatePropListeners > 0 )
    {
        for ( i = 0; i < m_StatePropData->GetNumberOfCallbacks( m_CurrentState ); i++ )
        {
		    CallbackData callbackData = m_StatePropData->GetCallbackData( m_CurrentState , i );
            if ( callbackData.onFrame >= lastFrame &&  callbackData.onFrame < newFrame)
            {
				unsigned int i;
				for ( i = 0; i < m_NumStatePropListeners; i++ )
				{
    				m_StatePropListener[i]->RecieveEvent( callbackData.callbackID , this );
				}
			}
        }
    }

    if ( m_FastDisplayDrawable == NULL )
    {
        //update frame controllers
	    unsigned int numFrameControllers = GetNumberOfFrameControllers();
        for ( i = 0; i < numFrameControllers; i++ )
        {
            FrameControllerData fcData = m_StatePropData->GetFrameControllerData( m_CurrentState , i );

            //if we need to update
            if ( fcData.minFrame != fcData.maxFrame )
            {
                tFrameController* fc = GetFrameControllerByIndex( i );

                //if the min frame is greater than the max frame then reverse the update
                if ( fcData.minFrame > fcData.maxFrame )
                {
		            fc->Advance( -dt , false );
                }
                else
                {
                    fc->Advance( dt , false );
                }

                if ( fcData.isCyclic && fcData.numberOfCycles > 0 )
                {
                    unsigned int currentNumberOfCycles = fc->LastFrameReached();
                    if ( currentNumberOfCycles >= fcData.numberOfCycles )
                    {
                        fc->SetCycleMode( FORCE_NON_CYCLIC );
                        fc->SetFrame( fcData.maxFrame );
                    }
                }
            }
	    }
    }
}

void CStateProp::UpdateFrameControllersForRender()
{
	unsigned int numFrameControllers = GetNumberOfFrameControllers();
    for ( unsigned int i = 0; i < numFrameControllers; i++ )
    {
		GetFrameControllerByIndex( i )->Advance( 0.f , true );
	}
}

unsigned int CStateProp::GetState()
{
    return m_CurrentState;
}

void CStateProp::NextState()
{
    if ( m_CurrentState + 1 >= m_StatePropData->GetNumberOfStates() )
    {
        SetState( 0 );
    }
    else
    {
        SetState( m_CurrentState + 1 );
    }
}

void CStateProp::PrevState()
{
    if ( m_CurrentState - 1 < 0 )
    {
        SetState( m_StatePropData->GetNumberOfStates() - 1 );
    }
    else
    {
        SetState( m_CurrentState - 1 );
    }
}

void CStateProp::SetState( unsigned int state )
{
    if ( state < m_StatePropData->GetNumberOfStates() && state >= 0 )
    {
        m_CurrentState = state;
    }

	m_BaseFrameController->SetFrame(0.f);

	unsigned int i;
	
	//reset the FC for new state
	unsigned int numFrameControllers = GetNumberOfFrameControllers();
    int numActiveControllers = 0;
    for ( i = 0; i < numFrameControllers; i++ )
    {
		FrameControllerData frameControllerData = m_StatePropData->GetFrameControllerData( m_CurrentState , i );
        tFrameController* fc = GetFrameControllerByIndex(i);

        if ( frameControllerData.minFrame != frameControllerData.maxFrame )
            numActiveControllers++;


        //if we are holding a frame over from last state dont reset
        if ( !frameControllerData.holdFrame )
        {
            //Reset() MUST come first or it will trounce frame ranges etc... 
		    fc->Reset();
        }
		fc->SetRelativeSpeed( frameControllerData.relativeSpeed );
		fc->SetCycleMode( (frameControllerData.isCyclic == 1) ? FORCE_CYCLIC : FORCE_NON_CYCLIC );
        if ( frameControllerData.minFrame > frameControllerData.maxFrame )
        {
		    fc->SetFrameRange( frameControllerData.maxFrame , frameControllerData.minFrame );
        }
        else
        {
            fc->SetFrameRange( frameControllerData.minFrame , frameControllerData.maxFrame );
        }
        
        if ( !frameControllerData.holdFrame )
        {
            fc->SetFrame( frameControllerData.minFrame );
        }
    

    }

	//Set visibility for new state
	unsigned int numElements =  m_AnimatedObject->GetBaseObject()->GetNumDrawableElement();
    int numDrawableElementsVisible = 0;
    int visibleElement = 0;
    for ( i = 0; i < numElements; i++ )
	{
        tCompositeDrawable::DrawableElement* de = GetDrawableElement(i);
		
        de->SetLockVisibility(false);
        VisibilityData visibilityData = m_StatePropData->GetVisibilityData( m_CurrentState , i );
        bool visible = visibilityData.isVisible == 1;
        de->SetVisibility( visible );

        //lock visibility if visiblility if false
        de->SetLockVisibility(!visible);

        if ( visible ) 
        {
            numDrawableElementsVisible++;
            visibleElement = i;
        }
	}

    //notify listeners of a state change
	for ( i = 0; i < m_NumStatePropListeners; i++ )
	{
    	m_StatePropListener[i]->RecieveEvent( STATEPROP_CHANGE_STATE_EVENT , this );
	}

    // Determine if this state is a special case fast drawable
    if ( m_FastDisplayDrawable )
    {
        m_FastDisplayDrawable->Release();
        m_FastDisplayDrawable = NULL;
    }

    if ( numDrawableElementsVisible == 1 && numActiveControllers == 0 )   
    {
        m_FastDisplayDrawable = GetDrawableElement( visibleElement )->GetDrawable();
        m_FastDisplayDrawable->AddRef();
    }

}

bool CStateProp::OnEvent( unsigned int eventID )
{
    bool wasEventHandled = false;
    unsigned int i;
	unsigned int numEvents = m_StatePropData->GetNumberOfEvents( m_CurrentState );
    for ( i = 0; i < numEvents; i++ )
    {
        EventData eventData = m_StatePropData->GetEventData( m_CurrentState , i );
        if ( eventData.eventID == eventID )
        {
            SetState( eventData.toState );
            wasEventHandled = true;
        }
    }
    return wasEventHandled;
}


void CStateProp::AddStatePropListener( CStatePropListener* statePropListener )
{
	if ( m_NumStatePropListeners < MAX_LISTENERS )
	{
		m_StatePropListener[m_NumStatePropListeners] = statePropListener;
		m_NumStatePropListeners++;
	}
}
void CStateProp::RemoveStatePropListener( CStatePropListener* statePropListener )
{
	unsigned int i = 0;
	//find this entry
	for ( i = 0; i < m_NumStatePropListeners; i++ )
	{
		if ( m_StatePropListener[i] == statePropListener )
		{
			m_NumStatePropListeners--;
			break;			
		}
	}

	//copy other entries back over 
	for ( i; i < m_NumStatePropListeners; i++ )
	{
		m_StatePropListener[i] = m_StatePropListener[i+1];
	}
}

tDrawable* CStateProp::GetDrawable()
{
    return m_AnimatedObject->GetBaseObject();
}

unsigned int CStateProp::GetNumberOfFrameControllers()
{
    return m_AnimatedObject->GetCurrentAnimation()->GetNumFrameControllers();
}

tFrameController* CStateProp::GetFrameControllerByIndex( unsigned int i )
{
    return m_AnimatedObject->GetCurrentAnimation()->GetFrameControllerByIndex(i);
}

unsigned int CStateProp::GetNumberOfDrawableElements()
{
    return m_AnimatedObject->GetBaseObject()->GetNumDrawableElement();
}

tCompositeDrawable::DrawableElement* CStateProp::GetDrawableElement( unsigned int i )
{
    return m_AnimatedObject->GetBaseObject()->GetDrawableElement( i );
}

void
CStateProp::Display( StatePropDSGProcAnimator* ProcAnimator )
{
    if ( m_FastDisplayDrawable != NULL )
    {
        m_FastDisplayDrawable->Display();
    }
    else
    {
        UpdateFrameControllersForRender();
        if( ProcAnimator )
        {
            ProcAnimator->UpdateForRender( static_cast<tCompositeDrawable*>( GetDrawable() ) );
        }
        GetDrawable()->Display();
    }
}

