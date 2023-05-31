#include <p3d/utility.hpp>
#include <p3d/matrixstack.hpp>
#include <p3d/anim/multicontroller.hpp>
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/anim/animatedobject.hpp>
#include <p3d/anim/animate.hpp>

#include <toollib/chunks16/inc/tlStatePropChunk.hpp>
#include <toollib/inc/tlFile.hpp>
#include <toollib/inc/tlFileByteStream.hpp>

#include "stateprop.hpp"
#include "statepropdata.hpp"

CStateProp* CStateProp::CreateCStateProp( CStatePropData* statePropData , unsigned int state )
{
    tAnimatedObjectFactory* objectFactory = statePropData->m_ObjectFactory;

    if ( !objectFactory )
    {
        objectFactory = p3d::find<tAnimatedObjectFactory>( statePropData->m_FactoryName );
        if ( objectFactory ) 
        {
            statePropData->m_ObjectFactory = objectFactory;
            statePropData->m_ObjectFactory->AddRef();
        }
    }

    if ( objectFactory )
	{
		tAnimatedObject* animatedObject = objectFactory->CreateObject(NULL);
		CStateProp* stateProp = new CStateProp( animatedObject , statePropData , state );
		return stateProp;
	}

	return NULL;
}

CStateProp::CStateProp( tAnimatedObject* animatedObject , CStatePropData* statePropData , unsigned int state ) :
    m_StatePropData( NULL ),
    m_AnimatedObject( NULL ),
    m_BaseFrameController( NULL ),
    m_StatePropListener( NULL ),
    m_CurrentState( state )
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

    //initilize transform
	rmt::Matrix identity;
	identity.Identity();
	SetTransformationMatrix( identity );

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
}

void CStateProp::Update( float dt )
{
    unsigned int i;

	float lastFrame = m_BaseFrameController->GetFrame();
	m_BaseFrameController->Advance( dt );
	float newFrame = m_BaseFrameController->GetFrame();

	//Check out transition
	TransitionData tansdata = m_StatePropData->GetTransitionData( m_CurrentState );
	if ( tansdata.autoTransition )
	{
		if (tansdata.onFrame >= lastFrame && tansdata.onFrame < newFrame)
		{
			SetState( tansdata.toState );
		}
	}

	//Check callback events
    if ( m_StatePropListener )
    {
        for ( i = 0; i < m_StatePropData->GetNumberOfCallbacks( m_CurrentState ); i++ )
        {
		    CallbackData callbackData = m_StatePropData->GetCallbackData( m_CurrentState , i );
            if ( callbackData.onFrame >= lastFrame &&  callbackData.onFrame < newFrame)
            {
    			m_StatePropListener->RecieveEvent( callbackData.callbackID , this );
			}
        }
    }

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

void CStateProp::Render()
{
    p3d::stack->Push();
    p3d::stack->Multiply( m_Transform );

	m_AnimatedObject->Display();

    p3d::stack->Pop();
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
    for ( i = 0; i < numFrameControllers; i++ )
    {
		FrameControllerData frameControllerData = m_StatePropData->GetFrameControllerData( m_CurrentState , i );
        tFrameController* fc = GetFrameControllerByIndex(i);

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
	for ( i = 0; i < numElements; i++ )
	{
        tCompositeDrawable::DrawableElement* de = GetDrawableElement(i);
		
        de->SetLockVisibility(false);
        VisibilityData visibilityData = m_StatePropData->GetVisibilityData( m_CurrentState , i );
        bool visible = visibilityData.isVisible == 1;
        de->SetVisibility( visible );
        
        //lock visibility if visiblility if false
        de->SetLockVisibility(!visible);
	}
}

void CStateProp::OnEvent( unsigned int eventID )
{
    unsigned int i;
	unsigned int numEvents = m_StatePropData->GetNumberOfEvents( m_CurrentState );
    for ( i = 0; i < numEvents; i++ )
    {
        EventData eventData = m_StatePropData->GetEventData( m_CurrentState , i );
        if ( eventData.eventID == eventID )
        {
            SetState( eventData.toState );
        }
    }
}

void CStateProp::UpdateOnDataEdit()
{
	//Update without reseting the current frame or current state info
	unsigned int i;

	//update frame controllers
	unsigned int numFrameControllers = GetNumberOfFrameControllers();
    for ( i = 0; i < numFrameControllers; i++ )
    {
		FrameControllerData frameControllerData = m_StatePropData->GetFrameControllerData( m_CurrentState , i );
        tFrameController* fc = GetFrameControllerByIndex(i);
        float frame = fc->GetFrame();
        fc->Reset();
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
        
        fc->SetFrame( frame );
    }

    //set visibilites
    unsigned int numDrawableElements = GetNumberOfDrawableElements();
	for ( i = 0; i < numDrawableElements; i++ )
	{
        tCompositeDrawable::DrawableElement* de = GetDrawableElement(i);
		
        de->SetLockVisibility(false);
        VisibilityData visibilityData = m_StatePropData->GetVisibilityData( m_CurrentState , i );
        bool visible = visibilityData.isVisible == 1;
        de->SetVisibility( visible );
        
        //lock visibility if visiblility if false
        de->SetLockVisibility(!visible);
	}
}

float CStateProp::GetBaseFrameControllerFrame()
{
	return m_BaseFrameController->GetFrame();
}

CStatePropData* CStateProp::GetCStatePropData()
{
    return m_StatePropData;
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

const char* CStateProp::GetDrawableName( unsigned int i )
{
    return m_AnimatedObject->GetBaseObject()->GetDrawableElement( i )->GetDrawable()->GetName();
}

const char* CStateProp::GetPropName( )
{
    return GetName();
}

//Export the CStatePropData
void CStateProp::ExportChunk( const char* filename )
{
	//create the outchunk data chunk
	tlDataChunk* outchunk = new tlDataChunk;

	//create smart prop chunk
	char buf[256];
	tlStatePropChunk* statepropchunk = new tlStatePropChunk;
    
    //version one has extra floats and ints
    statepropchunk->SetVersion( 1 );
	
    statepropchunk->SetName( m_StatePropData->GetName() );
	statepropchunk->SetObjectFactoryName( m_StatePropData->GetName() );
	statepropchunk->SetNumStates( m_StatePropData->GetNumberOfStates() );

	unsigned int state;
	int j;
	//create state subchunks 
	for ( state = 0; state < m_StatePropData->GetNumberOfStates(); state++ )
	{
		tlStatePropStateDataChunk* statechunk = new tlStatePropStateDataChunk;
		sprintf(buf, "state%i", state );
		statechunk->SetName( buf );
        TransitionData tData = m_StatePropData->GetTransitionData( state );
		statechunk->SetAutoTransition( tData.autoTransition );
		statechunk->SetOutFrame( tData.onFrame );
		statechunk->SetOutState( tData.toState );

		int numDrawables = GetNumberOfDrawableElements();
		statechunk->SetNumDrawables( numDrawables );
		for ( j = 0; j < numDrawables; j++ )
		{	
			tlStatePropVisibilitiesDataChunk* visibilitychunk = new tlStatePropVisibilitiesDataChunk;
            VisibilityData vData = m_StatePropData->GetVisibilityData( state , j );
			visibilitychunk->SetName( GetDrawableElement(j)->GetDrawable()->GetName() );
			visibilitychunk->SetVisible( vData.isVisible );
			statechunk->AppendSubChunk( visibilitychunk );
		}

		int numFrameControllers = GetNumberOfFrameControllers();
		statechunk->SetNumFrameControllers( numFrameControllers );
		for ( j = 0; j < numFrameControllers; j++ )
		{	
			tlStatePropFrameControllerDataChunk* framecontrollerchunk = new tlStatePropFrameControllerDataChunk;
            FrameControllerData fData = m_StatePropData->GetFrameControllerData( state , j );
			framecontrollerchunk->SetName( GetFrameControllerByIndex(j)->GetName() );
			framecontrollerchunk->SetCyclic( fData.isCyclic );
			framecontrollerchunk->SetMinFrame( fData.minFrame );
			framecontrollerchunk->SetMaxFrame( fData.maxFrame );
			framecontrollerchunk->SetRelativeSpeed( fData.relativeSpeed );
            framecontrollerchunk->SetHoldFrame( fData.holdFrame );
            framecontrollerchunk->SetNumberOfCycles( fData.numberOfCycles );
			statechunk->AppendSubChunk( framecontrollerchunk );
		}

		int numEvents = m_StatePropData->GetNumberOfEvents( state );
		statechunk->SetNumEvents( numEvents );
		for ( j = 0; j < numEvents; j++ )
		{
			tlStatePropEventDataChunk* eventdatachunk = new tlStatePropEventDataChunk;
            EventData eData = m_StatePropData->GetEventData( state , j );
			eventdatachunk->SetName( eData.eventName );
			eventdatachunk->SetState( eData.toState );
			eventdatachunk->SetEventEnum( eData.eventID );
			statechunk->AppendSubChunk( eventdatachunk );
		}

		int numCallbacks = m_StatePropData->GetNumberOfCallbacks( state );
		statechunk->SetNumCallbacks( numCallbacks );
		for ( j = 0; j < numCallbacks; j++ )
		{
			tlStatePropCallbackDataChunk* callbackChunk = new tlStatePropCallbackDataChunk;
            CallbackData cData = m_StatePropData->GetCallbackData( state , j );
			callbackChunk->SetName( cData.callbackName );
			callbackChunk->SetOnFrame( cData.onFrame );
			callbackChunk->SetEventEnum( cData.callbackID );
			statechunk->AppendSubChunk( callbackChunk );
		}
		
		//append the sate chunk
		statepropchunk->AppendSubChunk( statechunk );
	}

	outchunk->AppendSubChunk( statepropchunk );

    // create the new output file
    tlFile output(new tlFileByteStream(filename,omWRITE), tlFile::CHUNK32);

    if(!output.IsOpen()) 
    {
        printf("Could not open %s for writing\n", filename);
		return ;
    }

	outchunk->Write(&output);
	output.Close();   
}