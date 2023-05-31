#include <string.h>
#include <p3d/utility.hpp>
#include <p3d/anim/animatedobject.hpp>
#include <constants/chunkids.hpp>
#include <p3d/chunkfile.hpp>

#include "statepropdata.hpp"

//=============================================================================
// Class Declarations
// Prop
//=============================================================================

CStatePropData::CStatePropData( tAnimatedObjectFactory* factory ) : 
	tEntity(),
    m_NumStates(1)
{
    P3DASSERT ( factory );
    m_ObjectFactory = factory;
	m_ObjectFactory->AddRef();
	SetName( m_ObjectFactory->GetName() );

	ResetData();
}

CStatePropData::CStatePropData( ) : 
	tEntity(),
    m_NumStates(1),
	m_ObjectFactory( NULL )
{
	ResetData();
}

CStatePropData::~CStatePropData()
{
	m_ObjectFactory->Release();
}

//=============================================================================
//State Data
//=============================================================================

//Get
unsigned int CStatePropData::GetNumberOfStates()
{
    return m_NumStates;
}
StateData CStatePropData::GetStateData( unsigned int state )
{
    return m_StateData[ state ];
}

//Set
void CStatePropData::InsertState( unsigned int state )
{
    if ( m_NumStates >= MAX_STATES - 1)
        return;

    //increase the number of states and copy all the data up a state
    m_NumStates++;

    unsigned int i , j;
	for ( i = m_NumStates - 1; i > state + 1; i-- )
	{
		//transition data for each state
		m_StateData[i].transitionData.autoTransition = m_StateData[i-1].transitionData.autoTransition;
		m_StateData[i].transitionData.onFrame = m_StateData[i-1].transitionData.onFrame;
		m_StateData[i].transitionData.toState = m_StateData[i-1].transitionData.toState;

		//visibility data for each drawable in each state
		for (j = 0; j < MAX_VISIBILITIES; j++ )
		{
			m_StateData[i].visibilityData[j].isVisible = m_StateData[i-1].visibilityData[j].isVisible;
		}
    
		//framecontroller data in each state
		for (j = 0; j < MAX_FRAMECONTROLLERS; j++ )
		{
			m_StateData[i].frameControllerData[j].isCyclic = m_StateData[i-1].frameControllerData[j].isCyclic;
			m_StateData[i].frameControllerData[j].maxFrame = m_StateData[i-1].frameControllerData[j].maxFrame;
			m_StateData[i].frameControllerData[j].minFrame = m_StateData[i-1].frameControllerData[j].minFrame;
			m_StateData[i].frameControllerData[j].relativeSpeed = m_StateData[i-1].frameControllerData[j].relativeSpeed;
            m_StateData[i].frameControllerData[j].holdFrame = m_StateData[i-1].frameControllerData[j].holdFrame;
            m_StateData[i].frameControllerData[j].numberOfCycles = m_StateData[i-1].frameControllerData[j].numberOfCycles;
		}
		
		//event data for each state
		m_StateData[i].numEvents = m_StateData[i-1].numEvents;
        m_StateData[i].numCallbacks = m_StateData[i-1].numCallbacks;
		for (j = 0; j < MAX_EVENTS; j++ )
		{
			strcpy( m_StateData[i].eventData[j].eventName , m_StateData[i-1].eventData[j].eventName );
			m_StateData[i].eventData[j].toState = m_StateData[i-1].eventData[j].toState;
			m_StateData[i].eventData[j].eventID = m_StateData[i-1].eventData[j].eventID;

			strcpy( m_StateData[i].callbackData[j].callbackName , m_StateData[i-1].callbackData[j].callbackName );
			m_StateData[i].callbackData[j].onFrame = m_StateData[i-1].callbackData[j].onFrame;
			m_StateData[i].callbackData[j].callbackID = m_StateData[i-1].callbackData[j].callbackID;
		}
	}

    //Initialize the newly added state

   	//transition data for each state
	m_StateData[state + 1].transitionData.autoTransition = 0;
	m_StateData[state + 1].transitionData.onFrame = 0.f;
	m_StateData[state + 1].transitionData.toState = 0;

	//visibility data for each drawable in each state
	for (j = 0; j < MAX_VISIBILITIES; j++ )
	{
		m_StateData[state + 1].visibilityData[j].isVisible = 1;
	}

	//framecontroller data in each state
	for (j = 0; j < MAX_FRAMECONTROLLERS; j++ )
	{
		m_StateData[state + 1].frameControllerData[j].isCyclic = 0;
		m_StateData[state + 1].frameControllerData[j].maxFrame = 0.f;
		m_StateData[state + 1].frameControllerData[j].minFrame = 0.f;
		m_StateData[state + 1].frameControllerData[j].relativeSpeed = 1.f;
        m_StateData[state + 1].frameControllerData[j].holdFrame = 0;
        m_StateData[state + 1].frameControllerData[j].numberOfCycles = 0;
	}
	
	//event data for each state
	m_StateData[state + 1].numEvents = 0;
    m_StateData[state + 1].numCallbacks = 0;
}

void CStatePropData::DeleteState( unsigned int state )
{
	if ( m_NumStates == 1 )
	{
		ResetData();
		return;
	}

	int i;
	int j;

	for ( i = state; i < m_NumStates - 1; i++ )
	{
		//transition data for each state
		m_StateData[i].transitionData.autoTransition = m_StateData[i+1].transitionData.autoTransition;
		m_StateData[i].transitionData.onFrame = m_StateData[i+1].transitionData.onFrame;
		m_StateData[i].transitionData.toState = m_StateData[i+1].transitionData.toState;

		//visibility data for each drawable in each state
		for (j = 0; j < MAX_VISIBILITIES; j++ )
		{
			m_StateData[i].visibilityData[j].isVisible = m_StateData[i+1].visibilityData[j].isVisible;
		}
    
		//framecontroller data in each state
		for (j = 0; j < MAX_FRAMECONTROLLERS; j++ )
		{
			m_StateData[i].frameControllerData[j].isCyclic = m_StateData[i+1].frameControllerData[j].isCyclic;
			m_StateData[i].frameControllerData[j].maxFrame = m_StateData[i+1].frameControllerData[j].maxFrame;
			m_StateData[i].frameControllerData[j].minFrame = m_StateData[i+1].frameControllerData[j].minFrame;
			m_StateData[i].frameControllerData[j].relativeSpeed = m_StateData[i+1].frameControllerData[j].relativeSpeed;
            m_StateData[i].frameControllerData[j].holdFrame = m_StateData[i+1].frameControllerData[j].holdFrame;
            m_StateData[i].frameControllerData[j].numberOfCycles = m_StateData[i+1].frameControllerData[j].numberOfCycles;
		}
		
		//event data for each state
		m_StateData[i].numEvents = m_StateData[i+1].numEvents;
        m_StateData[i].numCallbacks = m_StateData[i+1].numCallbacks;
		for (j = 0; j < MAX_EVENTS; j++ )
		{
			strcpy( m_StateData[i].eventData[j].eventName , m_StateData[i+1].eventData[j].eventName );
			m_StateData[i].eventData[j].toState = m_StateData[i+1].eventData[j].toState;
			m_StateData[i].eventData[j].eventID = m_StateData[i+1].eventData[j].eventID;

			strcpy( m_StateData[i].callbackData[j].callbackName , m_StateData[i+1].callbackData[j].callbackName );
			m_StateData[i].callbackData[j].onFrame = m_StateData[i+1].callbackData[j].onFrame;
			m_StateData[i].callbackData[j].callbackID = m_StateData[i+1].callbackData[j].callbackID;
		}
	}
    
	m_NumStates--;
}

//=============================================================================
//Transition Data
//=============================================================================

//Get
TransitionData CStatePropData::GetTransitionData( int state )
{
    return m_StateData[state].transitionData;
}

//Set
void CStatePropData::SetAutoTransition( int state, bool b )
{
    m_StateData[state].transitionData.autoTransition = b;
}
void CStatePropData::SetAutoTransitionOnFrame( int state, float onFrame )
{
    m_StateData[state].transitionData.onFrame = onFrame;
}
void CStatePropData::SetAutoTransitionToState( int state, int toState )
{
    m_StateData[state].transitionData.toState = toState;
}

//=============================================================================
//Visibility Data
//=============================================================================

//Get
VisibilityData CStatePropData::GetVisibilityData( int state , int index)
{
    return ( m_StateData[state].visibilityData[index] );
}

//Set
void CStatePropData::SetVisible( int state , int index , bool b )
{
    m_StateData[state].visibilityData[index].isVisible = b;
}
void CStatePropData::SetAllVisibilities( int state , bool b )
{
	for ( int i = 0; i < MAX_VISIBILITIES; i++ )
	{
		m_StateData[state].visibilityData[i].isVisible = b;
	}
}
void CStatePropData::ShowAll(int state)
{
    SetAllVisibilities( state , true );
}
void CStatePropData::HideAll(int state)
{
    SetAllVisibilities( state , false );
}

//=============================================================================
//Frame controller data
//=============================================================================

//Get
FrameControllerData CStatePropData::GetFrameControllerData( int state, int fc )
{
    return m_StateData[state].frameControllerData[fc];
}    

//Set
void CStatePropData::SetCyclic( int state ,int fc, bool isCyclic )
{
    m_StateData[state].frameControllerData[fc].isCyclic = isCyclic;
}
void CStatePropData::SetRelativeSpeed( int state ,int fc, float speed )
{
    m_StateData[state].frameControllerData[fc].relativeSpeed = speed;
}
void CStatePropData::SetFrameRange( int state , int fc, float min, float max )
{
    m_StateData[state].frameControllerData[fc].minFrame = min;
    m_StateData[state].frameControllerData[fc].maxFrame = max;
}
void CStatePropData::SetHoldFrame( int state , int fc , bool holdFrame )
{
    m_StateData[state].frameControllerData[fc].holdFrame = holdFrame;
}
void CStatePropData::SetNumberOfCycles( int state , int fc , unsigned int numberOfCycles )
{
    m_StateData[state].frameControllerData[fc].numberOfCycles = numberOfCycles;
}


//=============================================================================
//Event data
//=============================================================================

//Get
unsigned int CStatePropData::GetNumberOfEvents( int state )
{
    return m_StateData[state].numEvents;
} 
EventData CStatePropData::GetEventData( int state , int eventindex )
{
    return m_StateData[state].eventData[eventindex];
}

//Set
void CStatePropData::AddEvent( const char* event , int eventEnum , int toState , int fromState )
{
    int n_events = m_StateData[fromState].numEvents;

    strcpy( m_StateData[fromState].eventData[n_events].eventName , event );
    m_StateData[fromState].eventData[n_events].toState = toState;
	m_StateData[fromState].eventData[n_events].eventID = eventEnum;

    m_StateData[fromState].numEvents = n_events + 1;
}
void CStatePropData::EditEvent( int state, int EventIndex, char* eventName, int eventID , int toState )
{
    strcpy( m_StateData[state].eventData[EventIndex].eventName , eventName );
    m_StateData[state].eventData[EventIndex].toState = toState;
	m_StateData[state].eventData[EventIndex].eventID = eventID;
}
void CStatePropData::DeleteEvent( int state , int index )
{
	int numEvents = m_StateData[state].numEvents;
    for ( int i = index; i <  numEvents - 1; i++ )
    {
        strcpy( m_StateData[state].eventData[i].eventName , m_StateData[state].eventData[i+1].eventName );
        m_StateData[state].eventData[i].toState = m_StateData[state].eventData[i+1].toState;
		m_StateData[state].eventData[i].eventID = m_StateData[state].eventData[i+1].eventID;
    }

	strcpy( m_StateData[state].eventData[numEvents - 1].eventName , "" );
    m_StateData[state].eventData[numEvents - 1].toState = 0;
	m_StateData[state].eventData[numEvents - 1].eventID = 0;

	m_StateData[state].numEvents = numEvents - 1;
}

//=============================================================================
//Callback Data
//=============================================================================

//Get
unsigned int CStatePropData::GetNumberOfCallbacks( int state )
{
    return m_StateData[state].numCallbacks;
} 
CallbackData CStatePropData::GetCallbackData( int state , int eventindex )
{
    return m_StateData[state].callbackData[eventindex];
}

//Set
void CStatePropData::AddCallback( int state , const char* callback , int callbackID , float onFrame )
{
    int n_callbacks = m_StateData[state].numCallbacks;

    strcpy( m_StateData[state].callbackData[n_callbacks].callbackName , callback );
    m_StateData[state].callbackData[n_callbacks].onFrame = onFrame;
	m_StateData[state].callbackData[n_callbacks].callbackID = callbackID;

    m_StateData[state].numCallbacks = n_callbacks + 1;
}
void CStatePropData::EditCallback( int state, int CBIndex, char* callbackname, int callbackID , float onFrame )
{
    strcpy( m_StateData[state].callbackData[CBIndex].callbackName , callbackname );
    m_StateData[state].callbackData[CBIndex].onFrame = onFrame;
	m_StateData[state].callbackData[CBIndex].callbackID = callbackID;
}
void CStatePropData::DeleteCallback( int state , int index )
{
	int numCallbacks = m_StateData[state].numCallbacks;
    for ( int i = index; i <  numCallbacks - 1; i++ )
    {
        strcpy( m_StateData[state].callbackData[i].callbackName , m_StateData[state].callbackData[i+1].callbackName );
        m_StateData[state].callbackData[i].onFrame = m_StateData[state].callbackData[i+1].onFrame;
		m_StateData[state].callbackData[i].callbackID = m_StateData[state].callbackData[i+1].callbackID;
    }

	strcpy( m_StateData[state].callbackData[numCallbacks - 1].callbackName , "" );
    m_StateData[state].callbackData[numCallbacks - 1].onFrame = 0.f;
	m_StateData[state].callbackData[numCallbacks - 1].callbackID = 0;

	m_StateData[state].numCallbacks = numCallbacks - 1;
}



//=============================================================================
//Utility
//=============================================================================
void CStatePropData::ResetData()
{
    unsigned int state;
	unsigned int j;

    for ( state = 0; state < MAX_STATES; state++ )
    {
		//transition data
		m_StateData[state].transitionData.autoTransition = 0;
		m_StateData[state].transitionData.onFrame = 0.f;
		m_StateData[state].transitionData.toState = 0;

		//framecontrollers
		for ( j = 0; j < MAX_FRAMECONTROLLERS; j++ )
        {
			m_StateData[state].frameControllerData[j].isCyclic = false;
			m_StateData[state].frameControllerData[j].minFrame = 0.f;
			m_StateData[state].frameControllerData[j].maxFrame = 0.f;
			m_StateData[state].frameControllerData[j].relativeSpeed = 1.f;
            m_StateData[state].frameControllerData[j].holdFrame = 0;
            m_StateData[state].frameControllerData[j].numberOfCycles = 0;

		}

		//visibilites
		for ( j = 0; j < MAX_VISIBILITIES; j++ )
		{
			m_StateData[state].visibilityData[j].isVisible = true;
		}
		
		//events && callbacks
		m_StateData[state].numEvents = 0;
        m_StateData[state].numCallbacks = 0;
		for ( j = 0; j < MAX_EVENTS; j++ )
        {
            strcpy( m_StateData[state].eventData[j].eventName , "" );
            m_StateData[state].eventData[j].toState = 0;
			m_StateData[state].eventData[j].eventID = 0;

            strcpy( m_StateData[state].callbackData[j].callbackName , "" );
            m_StateData[state].callbackData[j].onFrame = 0.f;
			m_StateData[state].callbackData[j].callbackID = 0;
        }
    }
}



//=============================================================================
// Class Declarations
// PropLoader
//=============================================================================
CStatePropDataLoader::CStatePropDataLoader() : 
    tSimpleChunkHandler(StateProp::STATEPROP)
{
}

//-------------------------------------------------------------------------
tEntity* CStatePropDataLoader::LoadObject(tChunkFile* f, tEntityStore* store)
{
	unsigned int version = f->GetLong();

	CStatePropData* statePropData = new CStatePropData();
	statePropData->ResetData();

	char buf[256];

	f->GetPString(buf);
	statePropData->SetName(buf);

	strcpy( statePropData->m_FactoryName , f->GetPString(buf) );   
	statePropData->m_ObjectFactory = p3d::find<tAnimatedObjectFactory>(store, statePropData->m_FactoryName);
	if ( statePropData->m_ObjectFactory )
	{
		statePropData->m_ObjectFactory->AddRef();
	}

	statePropData->m_NumStates = f->GetLong();

    for ( int currState = 0; currState < statePropData->m_NumStates; currState++ )
	{
        f->BeginChunk();

		f->GetPString(buf);
		statePropData->m_StateData[currState].transitionData.autoTransition = ( f->GetLong() != 0 );
		statePropData->m_StateData[currState].transitionData.toState = f->GetLong();
		int numDrawables = f->GetLong();
		int numFrameControllers = f->GetLong();
		statePropData->m_StateData[currState].numEvents = f->GetLong();
        statePropData->m_StateData[currState].numCallbacks = f->GetLong();
		statePropData->m_StateData[currState].transitionData.onFrame = f->GetFloat();

		for ( int currDrawable=0; currDrawable < numDrawables; currDrawable++ )
		{
			f->BeginChunk();
			f->GetPString(buf);
			statePropData->m_StateData[currState].visibilityData[currDrawable].isVisible = ( f->GetLong() != 0 );
			f->EndChunk();
		}

		for ( int currFrameController=0; currFrameController < numFrameControllers; currFrameController++ )
		{
			f->BeginChunk();
			f->GetPString(buf);
			statePropData->m_StateData[currState].frameControllerData[currFrameController].isCyclic = ( f->GetLong() != 0 );
            statePropData->m_StateData[currState].frameControllerData[currFrameController].numberOfCycles = f->GetLong();
            statePropData->m_StateData[currState].frameControllerData[currFrameController].holdFrame = ( f->GetLong() != 0 );
			statePropData->m_StateData[currState].frameControllerData[currFrameController].minFrame = f->GetFloat();
			statePropData->m_StateData[currState].frameControllerData[currFrameController].maxFrame = f->GetFloat();
			statePropData->m_StateData[currState].frameControllerData[currFrameController].relativeSpeed = f->GetFloat();
			f->EndChunk();
		}

		for ( unsigned int currEvent=0; currEvent < statePropData->m_StateData[currState].numEvents; currEvent++ )
		{
			f->BeginChunk();
			strcpy( statePropData->m_StateData[currState].eventData[currEvent].eventName , f->GetPString(buf) );
			statePropData->m_StateData[currState].eventData[currEvent].toState = f->GetLong();
			statePropData->m_StateData[currState].eventData[currEvent].eventID = f->GetLong();
			f->EndChunk();
		}

		for ( unsigned int currCallback=0; currCallback < statePropData->m_StateData[currState].numCallbacks; currCallback++ )
		{
			f->BeginChunk();
			strcpy( statePropData->m_StateData[currState].callbackData[currCallback].callbackName , f->GetPString(buf) );
			statePropData->m_StateData[currState].callbackData[currCallback].callbackID = f->GetLong();
			statePropData->m_StateData[currState].callbackData[currCallback].onFrame = f->GetFloat();
			f->EndChunk();
		}

        f->EndChunk();
    }

    return statePropData;
}