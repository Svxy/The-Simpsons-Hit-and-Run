/*===========================================================================
Workspace.cpp - Pure3D testbed

  
    Copyright (c) 1995-1999 Radical Entertainment, Inc.
    All rights reserved.
===========================================================================*/
#include <windows.h>

#include <assert.h>
#include <string.h>
#include <typeinfo.h>
#include <ctype.h> //for isdigit
#include <stdlib.h> //for atoi

#include <p3d/pure3d.hpp>
#include <p3d/anim/animatedobject.hpp>
#include <p3d/anim/poseanimation.hpp>
#include <p3d/anim/skeleton.hpp>
#include <p3d/geometry.hpp>
#include <p3d/anim/polyskin.hpp>
#include <p3d/utility.hpp>
#include <p3d/drawable.hpp>

#include <simcommon/simutility.hpp>
#include <simcommon/simenvironment.hpp>   // for SimUnits
#include <simcommon/simulatedobject.hpp>
#include <simcollision/collisionmanager.hpp>
#include <simcollision/collisiondisplay.hpp>
#include <simcollision/proximitydetection.hpp>

#include <sp/interface.hpp>
#include "workspace.hpp"

#include "stateprop.hpp"
#include "statepropdata.hpp"

void RegisterExtraChunks(void)
{ //do nothing
}

bool g_DrawCollision = true;
bool g_IsPaused = false;
bool g_DebugPhysicsDisplay = false;
float g_TotalTime_sec = 0.f;

const char c_StatePropExtension[] = "_spdata.p3d";


Workspace* g_Workspace = NULL;
CStateProp* g_CStateProp = NULL;

template <class T> class TemplateIterator : public ViewerIterator
{
public:
    TemplateIterator(T* t)
    {
        iterator = t;
        current = iterator->First();
    }
    
    ~TemplateIterator()
    {
        delete iterator;
    }
    
    tEntity* First(void)
    {
        current = iterator->First();
        
        return current;
    }
    
    tEntity* Current(void)
    {
        if(!current)
            current = iterator->First();
        
        return current;
    }
    
    tEntity* Next(bool loop = false)
    {
        current = iterator->Next();
        if(loop && !current)
            current = iterator->First();
        return current;
    }
    
    tEntity* Prev(void)
    {
        current = iterator->Prev();
        if(!current)
            current = iterator->Last();
        return current;
        
    }
    
private:
    T* iterator;
    tEntity* current; 
};

class DrawIterator : public tInventory::Iterator<tDrawable> {};
class ObjectFactoryIterator : public tInventory::Iterator<tAnimatedObjectFactory> {};
class CStatePropDataIterator : public tInventory::Iterator<CStatePropData> {};


//Get allocated memory
unsigned int GetAllocatedMemory()
{
    // on windows can't rely on the total physical memory so we must walk over
    // each heap and accumulate all the memory allocated in it. there is stack
    // overhead too but that should be minimal.
    HANDLE heaps[128];

    int numHeaps = GetProcessHeaps(sizeof(heaps)/sizeof(heaps[0]), heaps);
    if(numHeaps > sizeof(heaps)/sizeof(heaps[0]))
    {
        numHeaps = sizeof(heaps)/sizeof(heaps[0]);
    }

    unsigned int total = 0;
    int i;
    for(i=0; i<numHeaps; i++)
    {
        PROCESS_HEAP_ENTRY heapEntry;

        heapEntry.lpData = NULL;
        if(!HeapLock(heaps[i]))
        {
            //HC_DEBUGPRINT(("could not lock heap %d", i));
            continue;
        }
        while(HeapWalk(heaps[i], &heapEntry))
        {
            if(heapEntry.wFlags & PROCESS_HEAP_UNCOMMITTED_RANGE)
            {
                // don't count non-committed pages as some of pure3d seems to
                // mmap or allocate large ranges that never actually get paged
                // in
                //total += heapEntry.cbData;
            }
            else
            {
                total += heapEntry.cbData;
            }
        }
        HeapUnlock(heaps[i]);
    }

    return total;
}

//-------------------------------------------------------------------
// Workspace class
// derive from Testbed. 
//
//-------------------------------------------------------------------

Workspace::Workspace() :
	m_CStateProp(NULL),
	m_CollisionManager(NULL),
	m_CollisionSolver(NULL),
	m_DrawableFloor(NULL),
	m_SimStateFloor(NULL)
{
    sim::InstallSimLoaders();
    
    //draw outlines for debug render
    sim::SetDrawVolumeMethod( sim::DrawVolumeShape );

	Init();
}


Workspace::~Workspace()
{
    ResetAll();
    
	if ( m_CStateProp )
		m_CStateProp->Release();

    p3d::loadManager->RemoveAllHandlers(); // should be in testbed
}

void Workspace::Init()
{
	// get path to exe
    GetModuleFileName(NULL, m_Path, sizeof(m_Path) - 1);
    m_Path[ strlen(m_Path) - 13 ] = '\0';

	m_DrawableIterator = new TemplateIterator<DrawIterator>(new DrawIterator);
	m_ObjectFactoryIterator = new TemplateIterator<ObjectFactoryIterator>(new ObjectFactoryIterator);
	m_CStatePropDataIterator = new TemplateIterator<CStatePropDataIterator>(new CStatePropDataIterator);

	// install loader for Prop
	tP3DFileHandler* p3d = p3d::loadManager->GetP3DHandler();
	if(p3d)
	{   
		p3d->AddHandler( new CStatePropDataLoader );
	}

  	if (!sim::SimUnits::UnitSet())
	{
        sim::InitializeSimulation( sim::MetersUnits );
	}

    if (!m_CollisionManager)
    {
        float MAX_UPDATEAI_TIME_ms = 1000.0f*1.0f/30.0f; // 30 fps
        //sim::SimulatedObject::SetMaxTimeStep(MAX_UPDATEAI_TIME_ms/1000.0f);
        float collisionDistanceCGS = 2.0f; // cm   
        m_CollisionManager = sim::CollisionManager::GetInstance();
        m_CollisionManager->AddRef();
        sim::SimEnvironment* simEnvironment = sim::SimEnvironment::GetDefaultSimEnvironment();
        simEnvironment->SetCollisionDistanceCGS(collisionDistanceCGS);
        m_CollisionManager->SetCollisionManagerAttributes( sim::CM_DetectAll | sim::CM_SolveAll );
        
        P3DASSERT(m_CollisionSolver == NULL);
        m_CollisionSolver = new AICollisionSolverAgent();
        m_CollisionSolver->AddRef();
        m_CollisionManager->GetImpulseBasedCollisionSolver()->SetCollisionSolverAgent(m_CollisionSolver);
    }
}

void Workspace::LoadFloor( const char* p3dfilename )
{
	if ( m_DrawableFloor )
		m_DrawableFloor->Release();
	
	if ( m_SimStateFloor )
		m_SimStateFloor->Release();

	//check that a data file exist
	FILE* p = fopen( p3dfilename , "r" );
	if ( p )
	{
        p3d::inventory->RemoveAllElements();
		
        (void) p3d::load(p3dfilename);

        m_DrawableFloor = p3d::find<tDrawable>("floorShape");
        if ( m_DrawableFloor )
			m_DrawableFloor->AddRef();

		fclose(p);
	}
}

void Workspace::ResetAll(bool emptyInventory)
{
    if (emptyInventory)
    {
        delete m_DrawableIterator;
		delete m_ObjectFactoryIterator;
		delete m_CStatePropDataIterator;
        
        p3d::inventory->RemoveAllElements();
    }

	ResetProp();


	if ( m_CollisionSolver )
	{
		m_CollisionSolver->Release();
		m_CollisionSolver = NULL;
	}

	if ( m_CollisionManager )
	{
		m_CollisionManager->ResetAll();	
		m_CollisionManager->Release();
		m_CollisionManager = NULL;
	}

	this->Init();
}

void Workspace::Advance(float dt_ms)
{
	g_TotalTime_sec += dt_ms / 1000.f;
	float inDt_Sec = dt_ms / 1000.f;

	if ( !g_IsPaused )
	{
        if ( m_CStateProp )
        {
            m_CStateProp->Update(dt_ms);
        }
        if ( m_CollisionManager )
        {
            m_CollisionManager->Update( inDt_Sec , g_TotalTime_sec );
        }
	}
}

void Workspace::Display(tContext* context)
{
	if ( m_DrawableFloor )
    {
		m_DrawableFloor->Display();
    }
	
    if ( m_CStateProp )
    {
        m_CStateProp->UpdateFrameControllersForRender();
        m_CStateProp->Render();
    }

    if ( g_DrawCollision )
    {
	    sim::DisplayCollisionObjects(sim::CollisionManager::GetInstance());
    }
}

void Workspace::ResetProp()
{
	if ( m_CStateProp )
	{
		m_CStateProp->Release();
	}

    m_CStateProp = NULL;
    g_CStateProp = NULL;
}

int Workspace::Load( const char* name )
{
	ResetAll();

	(void) p3d::load(name);

	//Search for object factories
	m_ObjectFactoryIterator->First();
	m_CStatePropDataIterator->First();
	
	if ( m_ObjectFactoryIterator->Current() && m_CStatePropDataIterator->Current() )
	{
		//if we have an objectfactory and a prop we are good (1 file with all chunks)
		CStatePropData* statePropData = ((CStatePropData*)m_CStatePropDataIterator->Current());
        m_CStateProp = CStateProp::CreateCStateProp( statePropData , 0 );
		m_CStateProp->AddRef();
	}
	else if( m_ObjectFactoryIterator->Current() )
	{
		//if we just have the factory we need the file with the prop data so load it
		char buf[256];
		memcpy( buf , name , strlen(name) - strlen(".p3d") );
		buf[strlen(name) - strlen(".p3d")] = '\0';
		strcat(buf , c_StatePropExtension );

		//check that a data file exist
		FILE* p = fopen( buf , "r" );
		if ( p )
		{
			(void) p3d::load(buf);

			m_CStatePropDataIterator->First();
			if ( m_CStatePropDataIterator->Current() )
			{
		        //if we have an objectfactory and a prop we are good (1 file with all chunks)
		        CStatePropData* statePropData = ((CStatePropData*)m_CStatePropDataIterator->Current());
                m_CStateProp = CStateProp::CreateCStateProp( statePropData , 0 );
		        m_CStateProp->AddRef();
			}
			fclose(p);
		}
		else
		{
			//Create a new prop
			CStatePropData* statePropData = new CStatePropData( (tAnimatedObjectFactory*)m_ObjectFactoryIterator->Current() );
			if ( statePropData )
			{
                m_CStateProp = CStateProp::CreateCStateProp( statePropData , 0 );
				m_CStateProp->AddRef();
			}
		}
	}
	else if( m_CStatePropDataIterator->Current() )
	{
		//if we just have the prop we need the file with the factory
		char buf[256];
		memcpy( buf , name , strlen(name) - strlen(c_StatePropExtension) );
		buf[strlen(name) - strlen(c_StatePropExtension)] = '\0';
		strcat(buf , ".p3d");
		
		//check that a data file exist
		FILE* p = fopen( buf , "r" );
		if ( p )
		{
			(void) p3d::load(buf);

			m_ObjectFactoryIterator->First();
			if ( m_ObjectFactoryIterator->Current() )
			{
				CStatePropData* statePropData = ((CStatePropData*)m_CStatePropDataIterator->Current());
				m_CStateProp = CStateProp::CreateCStateProp( statePropData , 0 );
				m_CStateProp->AddRef();
			}
			fclose(p);
		}
		else
		{
			//no file associated with this prop data...
			ResetAll();
		}
	}

    if ( m_CStateProp )
    {
        g_CStateProp = m_CStateProp;
    }
    else
    {
        g_CStateProp = NULL;
    }

    
	return 1;
}


//=============================================================================
// CStateProp - DLL interface
//=============================================================================
//Load a pure3D file with art for the background===============================
int SP_CALLCONV SPLoadBackground( const char* filename  )
{
    if ( g_Workspace )
    {
        g_Workspace->LoadFloor( filename );
        return 1;
    }
    return 0;
}

//Load pure3D file ============================================================
int SP_CALLCONV SPLoad( const char* filename  )
{
    if ( g_Workspace )
    {
        g_Workspace->Load( filename );
        return 1;
    }
    return 0;
}

//Export the SmartPropData ====================================================
int SP_CALLCONV SPExportStatePropData( const char* filename )
{
    if ( g_CStateProp )
    {
		char buf[256];
		memcpy( buf , filename , strlen(filename) - strlen(".p3d") );
		buf[strlen(filename) - strlen(".p3d")] = '\0';
		strcat( buf , c_StatePropExtension );
        g_CStateProp->ExportChunk( buf );
        return 1;
    }
    return 0;
}

//Display collision  ==========================================================
int SP_CALLCONV SPShowCollision( int show )
{
    g_DrawCollision = ( show == 1 );
    return 1;
}

//Prop name ===================================================================
const char* SP_CALLCONV SPGetPropName()
{
    if ( g_CStateProp )
    {
        return g_CStateProp->GetCStatePropData()->GetName();
    }
    return NULL;
}

//advance  ====================================================================
int SP_CALLCONV SPPause( bool b  )
{
    g_IsPaused = b;
    return 1;
}
int SP_CALLCONV SPAdvanceOneFrame()
{
    if ( g_CStateProp )
    {
        g_CStateProp->Update( 100.f );
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPBackOneFrame()
{
    if ( g_CStateProp )
    {
        g_CStateProp->Update( -100.f );
        return 1;
    }
    return 0;
}

// State Data =================================================================
unsigned int SP_CALLCONV SPGetNumberOfStates()
{
    if ( g_CStateProp )
    {
        return g_CStateProp->GetCStatePropData()->GetNumberOfStates();
    }
    return 0;
}

int SP_CALLCONV SPInsertState( unsigned int state )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->InsertState( state );
        g_CStateProp->UpdateOnDataEdit();
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPDeleteState( unsigned int state )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->DeleteState( state );
        g_CStateProp->UpdateOnDataEdit();
        return 1;
    }
    return 0;
}

int SP_CALLCONV SPGetCurrentState()
{
    if ( g_CStateProp )
    {
        return g_CStateProp->GetState();
    }
    return 0;
}
int SP_CALLCONV SPNextState()
{
    if ( g_CStateProp )
    {
        g_CStateProp->NextState();
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPPrevState()
{
    if ( g_CStateProp )
    {
        g_CStateProp->PrevState();
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPSetState( unsigned int state )
{
    if ( g_CStateProp )
    {
        g_CStateProp->SetState( state );
        return 1;
    }
    return 0;
}

//Transition Data =============================================================
bool SP_CALLCONV SPGetTransitionData( int state , TransitionData* transitionData )
{
    if ( g_CStateProp )
    {
        transitionData->autoTransition = g_CStateProp->GetCStatePropData()->GetTransitionData( state ).autoTransition;
        transitionData->onFrame = g_CStateProp->GetCStatePropData()->GetTransitionData( state ).onFrame;
        transitionData->toState = g_CStateProp->GetCStatePropData()->GetTransitionData( state ).toState;
        return true;
    }
    return false;
}
int SP_CALLCONV SPSetAutoTransition( int state, bool b )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->SetAutoTransition( state , b );
        g_CStateProp->UpdateOnDataEdit();
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPSetAutoTransitionOnFrame( int state, float onFrame )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->SetAutoTransitionOnFrame( state , onFrame );
        g_CStateProp->UpdateOnDataEdit();
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPSetAutoTransitionToState( int state, int toState )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->SetAutoTransitionToState( state , toState );
        g_CStateProp->UpdateOnDataEdit();
        return 1;
    }
    return 0;
}

//Visibility Data =============================================================
bool SP_CALLCONV SPGetVisibilityData( int state , int index , VisibilityData* visibilityData )
{
    if ( g_CStateProp )
    {
        visibilityData->isVisible = g_CStateProp->GetCStatePropData()->GetVisibilityData( state , index ).isVisible;
        return true;
    }
    return false;
}
int SP_CALLCONV SPSetVisible( int state , int index , bool b )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->SetVisible( state , index , b );
        g_CStateProp->UpdateOnDataEdit();
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPSetAllVisibilities( int state , bool b )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->SetAllVisibilities( state , b );
        g_CStateProp->UpdateOnDataEdit();
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPShowAll(int state)
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->ShowAll( state );
        g_CStateProp->UpdateOnDataEdit();
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPHideAll(int state)
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->HideAll( state );
        g_CStateProp->UpdateOnDataEdit();
        return 1;
    }
    return 0;
}

SP_IMPORT int SP_CALLCONV SPGetNumDrawables()
{
    if ( g_CStateProp )
    {
        return g_CStateProp->GetNumberOfDrawableElements();
    }
    return 0;
}

const char* SP_CALLCONV SPGetDrawableName( int index )
{
    if ( g_CStateProp )
    {
        return g_CStateProp->GetDrawableName( index );
    }
    return NULL;
}

//Frame Controller Data =======================================================
bool SPGetFrameControllerData( int state, int fc , FrameControllerData* fcData )
{
    if ( g_CStateProp )
    {
        fcData->holdFrame = g_CStateProp->GetCStatePropData()->GetFrameControllerData( state , fc ).holdFrame;
        fcData->isCyclic = g_CStateProp->GetCStatePropData()->GetFrameControllerData( state , fc ).isCyclic;
        fcData->maxFrame = g_CStateProp->GetCStatePropData()->GetFrameControllerData( state , fc ).maxFrame;
        fcData->minFrame = g_CStateProp->GetCStatePropData()->GetFrameControllerData( state , fc ).minFrame;
        fcData->numberOfCycles = g_CStateProp->GetCStatePropData()->GetFrameControllerData( state , fc ).numberOfCycles;
        fcData->relativeSpeed = g_CStateProp->GetCStatePropData()->GetFrameControllerData( state , fc ).relativeSpeed;
        return true;
    }
    return false;
}
int SP_CALLCONV SPSetCyclic( int state ,int fc, bool isCyclic )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->SetCyclic( state , fc , isCyclic );
        g_CStateProp->UpdateOnDataEdit();
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPSetRelativeSpeed( int state ,int fc, float speed )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->SetRelativeSpeed( state , fc , speed );
        g_CStateProp->UpdateOnDataEdit();
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPSetFrameRange( int state ,int fc, float min, float max )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->SetFrameRange( state , fc , min , max );
        g_CStateProp->UpdateOnDataEdit();
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPSetHoldFrame( int state , int fc , bool holdFrame )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->SetHoldFrame( state , fc , holdFrame );
        g_CStateProp->UpdateOnDataEdit();
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPSetNumberOfCycles( int state , int fc , unsigned int numberOfCycles )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->SetNumberOfCycles( state , fc , numberOfCycles );
        return 1;
    }
    return 0;
}

int SP_CALLCONV SPGetNumFrameControllers()
{
    if ( g_CStateProp )
    {
        return g_CStateProp->GetNumberOfFrameControllers();
    }
    return 0;
}
float SP_CALLCONV SPGetBaseFrameControllerFrame()
{
    if ( g_CStateProp )
    {
        return g_CStateProp->GetBaseFrameControllerFrame();
    }
    return 0.f;
}
float SP_CALLCONV SPGetFrameControllerFrame( int index )
{
    if ( g_CStateProp )
    {
        return g_CStateProp->GetFrameControllerByIndex( index )->GetFrame();
    }
    return 0.f;
}
const char* SP_CALLCONV SPGetFrameControllerName( int index )
{
    if ( g_CStateProp )
    {
        return g_CStateProp->GetFrameControllerByIndex( index )->GetName();
    }
    return NULL;
}

//Event Data ==================================================================
unsigned int SP_CALLCONV SPGetNumberOfEvents( int state )
{
    if ( g_CStateProp )
    {
        return g_CStateProp->GetCStatePropData()->GetNumberOfEvents( state );
    }
    return 0;
}
bool SP_CALLCONV SPGetEventData( int state , int eventindex , EventData *eventData)
{
    if ( g_CStateProp )
    {
        EventData ed = g_CStateProp->GetCStatePropData()->GetEventData( state , eventindex );
        eventData->eventID = ed.eventID;
        strcpy( eventData->eventName , ed.eventName );
        eventData->toState = ed.toState;
        return true;
    }
    return false;
}
int SP_CALLCONV SPAddEvent( const char* event , int eventEnum , int toState , int fromState )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->AddEvent( event , eventEnum , toState , fromState );
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPEditEvent( int state, int EventIndex, char* eventName, int eventEnum , int toState )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->EditEvent( state , EventIndex , eventName , eventEnum , toState );
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPDeleteEvent( int fromState , int index )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->DeleteEvent( fromState , index );
        return 1;
    }
    return 0;
}

//Callback Data ===============================================================
unsigned int SP_CALLCONV SPGetNumberOfCallbacks( int state )
{
    if ( g_CStateProp )
    {
        return g_CStateProp->GetCStatePropData()->GetNumberOfCallbacks( state );
    }
    return 0;
}
bool SP_CALLCONV SPGetCallbackData( int state , int index , CallbackData* callbackData )
{
    if ( g_CStateProp )
    {
        callbackData->callbackID = g_CStateProp->GetCStatePropData()->GetCallbackData( state , index ).callbackID;
        strcpy( callbackData->callbackName , g_CStateProp->GetCStatePropData()->GetCallbackData( state , index ).callbackName );
        callbackData->onFrame = g_CStateProp->GetCStatePropData()->GetCallbackData( state , index ).onFrame;
        return true;
    }
    return false;
}
int SP_CALLCONV SPAddCallback( int state , const char* event , int eventEnum , float frame )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->AddCallback( state , event , eventEnum , frame );
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPEditCallback( int state, int CBIndex, char* eventname, int eventEnum , float frame )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->EditCallback( state , CBIndex , eventname , eventEnum , frame );
        return 1;
    }
    return 0;
}
int SP_CALLCONV SPDeleteCallback( int state , int index )
{
    if ( g_CStateProp )
    {
        g_CStateProp->GetCStatePropData()->DeleteCallback( state , index );
        return 1;
    }
    return 0;
}