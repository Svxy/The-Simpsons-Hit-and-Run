#ifndef _STATEPROPDATA_HPP_
#define _STATEPROPDATA_HPP_


#include <radmath/radmath.hpp>
#include <p3d/loadmanager.hpp>

#include "statepropdatatypes.hpp"

//=============================================================================
// Forward Class/Struct Declarations
//=============================================================================

class CStateProp;
class tAnimatedObjectFactory;
class CStatePropDataLoader;

//=============================================================================
// Definitions
//=============================================================================
#define MAX_STATES 50
#define MAX_FRAMECONTROLLERS 250
#define MAX_VISIBILITIES 250
#define MAX_EVENTS 50

//State data
struct StateData
{
    TransitionData          transitionData;
	VisibilityData          visibilityData[MAX_VISIBILITIES];
    FrameControllerData     frameControllerData[MAX_FRAMECONTROLLERS];
	EventData               eventData[MAX_EVENTS];
	CallbackData            callbackData[MAX_EVENTS];	

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

	friend class CStateProp;
	friend class CStatePropDataLoader;

    CStatePropData( tAnimatedObjectFactory* factory );
	CStatePropData();
    ~CStatePropData();

    // State Data =====================================================================================
        //Get
        unsigned int GetNumberOfStates();
        StateData GetStateData( unsigned int state );
        //Set
        void InsertState( unsigned int state );
	    void DeleteState( unsigned int state );

	//Transition Data ==================================================================================
        //Get
	    TransitionData GetTransitionData( int state );
        //Set
	    void SetAutoTransition( int state, bool b );
	    void SetAutoTransitionOnFrame( int state, float onFrame );
	    void SetAutoTransitionToState( int state, int toState );

	//Visibility Data ==================================================================================
        //Get
        VisibilityData GetVisibilityData( int state , int index);
        //Set
        void SetVisible( int state , int index , bool b );
	    void SetAllVisibilities( int state , bool b );
        void ShowAll(int state);
        void HideAll(int state);

    //Frame Controller Data =============================================================================
	    //Get
        FrameControllerData GetFrameControllerData( int state, int fc );
        //Set
	    void SetCyclic( int state ,int fc, bool isCyclic );
	    void SetRelativeSpeed( int state ,int fc, float speed );
        void SetFrameRange( int state ,int fc, float min, float max );
        void SetHoldFrame( int state , int fc , bool holdFrame );
        void SetNumberOfCycles( int state , int fc , unsigned int numberOfCycles );

	//Event Data ========================================================================================
        //Get
        unsigned int GetNumberOfEvents( int state );
	    EventData GetEventData( int state , int eventindex );
        //Set
        void AddEvent( const char* event , int eventEnum , int toState , int fromState );
	    void EditEvent( int state, int EventIndex, char* eventName, int eventEnum , int toState );
        void DeleteEvent( int fromState , int index );

	//Callback Data ======================================================================================
        //Get
	    unsigned int GetNumberOfCallbacks( int state );
	    CallbackData GetCallbackData( int state , int eventindex );
        //Set
        void AddCallback( int state , const char* event , int eventEnum , float frame );
	    void EditCallback( int state, int CBIndex, char* eventname, int eventEnum , float frame );
	    void DeleteCallback( int state , int index );

private:

    //reset the prop data
	void ResetData();

	//animated object factory
	char m_FactoryName[64];
	tAnimatedObjectFactory* m_ObjectFactory;
	
    //total number of states
	int m_NumStates;
    StateData m_StateData[MAX_STATES];

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
