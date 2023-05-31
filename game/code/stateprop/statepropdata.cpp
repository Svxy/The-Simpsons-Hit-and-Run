#include <string.h>
#include <p3d/utility.hpp>
#include <p3d/anim/animatedobject.hpp>
#include <constants/chunkids.hpp>
#include <p3d/chunkfile.hpp>

#include "stateprop/statepropdata.hpp"

using namespace StatePropDataTypes;

//=============================================================================
// Class Declarations
// Prop
//=============================================================================
CStatePropData::CStatePropData( ) : 
	m_ObjectFactory( NULL ),
    m_NumStates(0),
    m_StateData(NULL)
{
}

CStatePropData::~CStatePropData()
{
	if (m_ObjectFactory)
    {
        m_ObjectFactory->Release();
    }

    unsigned int i = 0;
    for( i; i < m_NumStates; i++ )
    {
        delete m_StateData[i];
    }
    delete m_StateData;
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
    return ( * (m_StateData[ state ]) ) ;
}

//=============================================================================
//Transition Data
//=============================================================================

//Get
TransitionData CStatePropData::GetTransitionData( int state )
{
    return m_StateData[state]->transitionData;
}

//=============================================================================
//Visibility Data
//=============================================================================

//Get
VisibilityData CStatePropData::GetVisibilityData( int state , int index)
{
    return ( m_StateData[state]->visibilityData[index] );
}

//=============================================================================
//Frame controller data
//=============================================================================

//Get
FrameControllerData CStatePropData::GetFrameControllerData( int state, int fc )
{
    return m_StateData[state]->frameControllerData[fc];
}    

//=============================================================================
//Event data
//=============================================================================

//Get
unsigned int CStatePropData::GetNumberOfEvents( int state )
{
    return m_StateData[state]->numEvents;
} 
EventData CStatePropData::GetEventData( int state , int eventindex )
{
    return m_StateData[state]->eventData[eventindex];
}

//=============================================================================
//Callback Data
//=============================================================================

//Get
unsigned int CStatePropData::GetNumberOfCallbacks( int state )
{
    return m_StateData[state]->numCallbacks;
} 
CallbackData CStatePropData::GetCallbackData( int state , int eventindex )
{
    return m_StateData[state]->callbackData[eventindex];
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
    
    CStatePropData* statePropData = new CStatePropData();

    unsigned int version = f->GetLong();
        
    //get the name
    char buf[256];     
    f->GetPString(buf);
    statePropData->SetName(buf);
    
    //get the object factory
    f->GetPString(buf);   
    statePropData->m_ObjectFactory = p3d::find<tAnimatedObjectFactory>(store, buf);
    if ( statePropData->m_ObjectFactory )
    {
        statePropData->m_ObjectFactory->AddRef();
    }
    
    //get the num states
    statePropData->m_NumStates = f->GetLong();
    

    statePropData->m_StateData = new StateData*[statePropData->m_NumStates];
    memset(statePropData->m_StateData,0,statePropData->m_NumStates*sizeof(StateData*));
    
    unsigned int currState;
    unsigned int i;
    for ( currState = 0; currState < statePropData->m_NumStates; currState++ )
    {
        f->BeginChunk();
        
        //get name
        f->GetPString(buf);

        bool autoTransition = ( f->GetLong() != 0 );
        unsigned int toState = f->GetLong();
        unsigned int numDrawables = f->GetLong();
        unsigned int numFrameControllers = f->GetLong();
        unsigned int numEvents = f->GetLong();
        unsigned int numCallbacks = f->GetLong();
        float onFrame = f->GetFloat();
        
        //give me memory dammit
        int memsize = ( sizeof(StateData) ) + 
            ( sizeof(VisibilityData) * numDrawables ) +
            ( sizeof(FrameControllerData) * numFrameControllers ) +
            ( sizeof(EventData) * numEvents ) +
            ( sizeof(CallbackData) * numCallbacks );

        unsigned char* memptr = new unsigned char[ memsize ];

        memset( memptr,0, memsize );
        
        statePropData->m_StateData[currState] = (StateData*)memptr; 

        statePropData->m_StateData[currState]->numEvents = numEvents;
        statePropData->m_StateData[currState]->numCallbacks = numCallbacks;
        
        //Transition data
        statePropData->m_StateData[currState]->transitionData.autoTransition = autoTransition;
        statePropData->m_StateData[currState]->transitionData.onFrame = onFrame;
        statePropData->m_StateData[currState]->transitionData.toState = toState;
        
        //Visibility data
        statePropData->m_StateData[currState]->visibilityData = (VisibilityData*) ( memptr + sizeof(StateData) );
        for ( i=0; i < numDrawables; i++ )
        {
            f->BeginChunk();
            f->GetPString(buf);
            VisibilityData *p = &(statePropData->m_StateData[currState]->visibilityData[i]);
            p->isVisible = (f->GetLong() != 0);
            f->EndChunk();
        }
        
        //FrameController data
        statePropData->m_StateData[currState]->frameControllerData = (FrameControllerData*)
            ( memptr + sizeof(StateData) +
            ( sizeof(VisibilityData) * numDrawables ) );

        for ( i=0; i < numFrameControllers; i++ )
        {
            f->BeginChunk();
            f->GetPString(buf);
            FrameControllerData* p = &(statePropData->m_StateData[currState]->frameControllerData[i]);
            p->isCyclic = ( f->GetLong() != 0 );
            p->numberOfCycles = f->GetLong();
            p->holdFrame = ( f->GetLong() != 0 );
            p->minFrame = f->GetFloat();
            p->maxFrame = f->GetFloat();
            p->relativeSpeed = f->GetFloat();
            f->EndChunk();
        }
        
        //Event data
        statePropData->m_StateData[currState]->eventData = (EventData*)
            ( memptr + sizeof(StateData) +
            ( sizeof(VisibilityData) * numDrawables ) +
            ( sizeof(FrameControllerData) * numFrameControllers ) );

        for ( i=0; i < numEvents; i++ )
        {
            f->BeginChunk();
            EventData* p = &(statePropData->m_StateData[currState]->eventData[i]);
            memcpy( p->eventName , f->GetPString(buf) , 64 );
            p->toState = f->GetLong();
            p->eventID = f->GetLong();
            f->EndChunk();
        }
        
        //Callback data
        statePropData->m_StateData[currState]->callbackData = (CallbackData*)
            ( memptr + sizeof(StateData) +
            ( sizeof(VisibilityData) * numDrawables ) +
            ( sizeof(FrameControllerData) * numFrameControllers ) +
            ( sizeof(EventData) * numEvents ) );

        for ( i=0; i < numCallbacks; i++ )
        {
            f->BeginChunk();
            CallbackData* p = &(statePropData->m_StateData[currState]->callbackData[i]);
            memcpy( p->callbackName , f->GetPString(buf) , 64 );
            p->callbackID = f->GetLong();
            p->onFrame = f->GetFloat();
            f->EndChunk();
        }
         
        f->EndChunk();
    }

    return statePropData;
}
