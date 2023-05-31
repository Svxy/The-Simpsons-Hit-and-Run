#ifndef _STATEPROPDATA_HPP_
#define _STATEPROPDATA_HPP_


#include <radmath/radmath.hpp>
#include <p3d/loadmanager.hpp>

#include "stateprop/statepropdatatypes.hpp"

//=============================================================================
// Forward Class/Struct Declarations
//=============================================================================

class CStateProp;
class tAnimatedObjectFactory;
class CStatePropDataLoader;

//=============================================================================
// Definitions
//=============================================================================

//State data
struct StateData
{
    TransitionData          transitionData;
	VisibilityData*         visibilityData;
    FrameControllerData*    frameControllerData;
	EventData*              eventData;
	CallbackData*           callbackData;	

	unsigned int            numEvents;
	unsigned int            numCallbacks;
};

//=============================================================================
// Class Declarations
// Prop
//=============================================================================

class CStatePropData : public tEntity
{

public:

	friend class CStatePropDataLoader;

	CStatePropData();
    ~CStatePropData();

    // State Data =====================================================================================
    unsigned int GetNumberOfStates();
    StateData GetStateData( unsigned int state );

	//Transition Data ==================================================================================
    TransitionData GetTransitionData( int state );

    //Visibility Data ==================================================================================
    VisibilityData GetVisibilityData( int state , int index);

    //Frame Controller Data =============================================================================
    FrameControllerData GetFrameControllerData( int state, int fc );

	//Event Data ========================================================================================
    unsigned int GetNumberOfEvents( int state );
    EventData GetEventData( int state , int eventindex );

	//Callback Data ======================================================================================
    unsigned int GetNumberOfCallbacks( int state );
    CallbackData GetCallbackData( int state , int eventindex );

    tAnimatedObjectFactory* GetAnimatedObjectFactory() { return m_ObjectFactory; }

private:

	//animated object factory
	char m_FactoryName[64];
	tAnimatedObjectFactory* m_ObjectFactory;
	
    //total number of states
	unsigned int m_NumStates;
    StateData** m_StateData;
};


//=============================================================================
// Class Declarations
// PropLoader
//=============================================================================
class CStatePropDataLoader : public tSimpleChunkHandler
{
public:
    CStatePropDataLoader();
    tEntity* LoadObject(tChunkFile* file, tEntityStore* store);

protected:
    ~CStatePropDataLoader() {};
};

#endif //_STATEPROPDATA_HPP_
