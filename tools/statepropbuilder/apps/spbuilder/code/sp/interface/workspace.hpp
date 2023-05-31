/*
 * fv/interface/platform.hpp
 */

#ifndef SP_INTERFACE_WORKSPACE_HPP
#define SP_INTERFACE_WORKSPACE_HPP

#include <sp/interface/base.hpp>
#include <sp/engine/statepropdatatypes.hpp>

extern "C"
{
    //Load Background =============================================================
    SP_IMPORT int SP_CALLCONV SPLoadBackground( const char* filename  );

	//Load ========================================================================
	SP_IMPORT int SP_CALLCONV SPLoad( const char* filename  );

	//Export ======================================================================
	SP_IMPORT int SP_CALLCONV SPExportStatePropData( const char* filename  );

    //Draw collision ==============================================================
    SP_IMPORT int SP_CALLCONV SPShowCollision( int show );

    //Prop name ===================================================================
    SP_IMPORT const char* SP_CALLCONV SPGetPropName();

    //Advance =====================================================================
	SP_IMPORT int SP_CALLCONV SPPause( bool b  );
	SP_IMPORT int SP_CALLCONV SPAdvanceOneFrame();
	SP_IMPORT int SP_CALLCONV SPBackOneFrame();

    // State Data =================================================================
    SP_IMPORT unsigned int SP_CALLCONV SPGetNumberOfStates();
    SP_IMPORT int SP_CALLCONV SPInsertState( unsigned int state );
    SP_IMPORT int SP_CALLCONV SPDeleteState( unsigned int state );

    SP_IMPORT int SP_CALLCONV SPGetCurrentState();
    SP_IMPORT int SP_CALLCONV SPNextState();
    SP_IMPORT int SP_CALLCONV SPPrevState();
    SP_IMPORT int SP_CALLCONV SPSetState( unsigned int state );

    //Transition Data =============================================================
    SP_IMPORT bool SP_CALLCONV SPGetTransitionData( int state , TransitionData* transitionData );
    SP_IMPORT int SP_CALLCONV SPSetAutoTransition( int state, bool b );
    SP_IMPORT int SP_CALLCONV SPSetAutoTransitionOnFrame( int state, float onFrame );
    SP_IMPORT int SP_CALLCONV SPSetAutoTransitionToState( int state, int toState );

    //Visibility Data =============================================================
    SP_IMPORT bool SP_CALLCONV SPGetVisibilityData( int state , int index , VisibilityData* visibilityData );
    SP_IMPORT int SP_CALLCONV SPSetVisible( int state , int index , bool b );
    SP_IMPORT int SP_CALLCONV SPSetAllVisibilities( int state , bool b );
    SP_IMPORT int SP_CALLCONV SPShowAll(int state);
    SP_IMPORT int SP_CALLCONV SPHideAll(int state);

    SP_IMPORT int SP_CALLCONV SPGetNumDrawables();
    SP_IMPORT const char* SP_CALLCONV SPGetDrawableName( int index );

    //Frame Controller Data =======================================================
    SP_IMPORT bool SPGetFrameControllerData( int state, int fc , FrameControllerData* fcData );
    SP_IMPORT int SP_CALLCONV SPSetCyclic( int state ,int fc, bool isCyclic );
    SP_IMPORT int SP_CALLCONV SPSetRelativeSpeed( int state ,int fc, float speed );
    SP_IMPORT int SP_CALLCONV SPSetFrameRange( int state ,int fc, float min, float max );
    SP_IMPORT int SP_CALLCONV SPSetHoldFrame( int state , int fc , bool holdFrame );
    SP_IMPORT int SP_CALLCONV SPSetNumberOfCycles( int state , int fc , unsigned int numberOfCycles );
    
    SP_IMPORT int SP_CALLCONV SPGetNumFrameControllers();
    SP_IMPORT float SP_CALLCONV SPGetBaseFrameControllerFrame();
    SP_IMPORT float SP_CALLCONV SPGetFrameControllerFrame( int index );
    SP_IMPORT const char* SP_CALLCONV SPGetFrameControllerName( int index );

    //Event Data ==================================================================
    SP_IMPORT unsigned int SP_CALLCONV SPGetNumberOfEvents( int state );
    SP_IMPORT bool SP_CALLCONV SPGetEventData( int state , int eventindex , EventData *eventData);
    SP_IMPORT int SP_CALLCONV SPAddEvent( const char* event , int eventEnum , int toState , int fromState );
    SP_IMPORT int SP_CALLCONV SPEditEvent( int state, int EventIndex, char* eventName, int eventEnum , int toState );
    SP_IMPORT int SP_CALLCONV SPDeleteEvent( int fromState , int index );

    //Callback Data ===============================================================
    SP_IMPORT unsigned int SP_CALLCONV SPGetNumberOfCallbacks( int state );
    SP_IMPORT bool SP_CALLCONV SPGetCallbackData( int state , int index , CallbackData* callbackData );
    SP_IMPORT int SP_CALLCONV SPAddCallback( int state , const char* event , int eventEnum , float frame );
    SP_IMPORT int SP_CALLCONV SPEditCallback( int state, int CBIndex, char* eventname, int eventEnum , float frame );
    SP_IMPORT int SP_CALLCONV SPDeleteCallback( int state , int index );
}


#endif // SP_INTERFACE_WORKSPACE_HPP

