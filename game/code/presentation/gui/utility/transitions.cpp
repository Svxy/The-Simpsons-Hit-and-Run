//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Transitions.cpp
//
// Description: Some transitions for the hud/frontned
//
// Authors:     Ian Gipson
//
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <boundeddrawable.h>
#include <contexts/gameplay/gameplaycontext.h>
#include <events/eventmanager.h>
#include <input/inputmanager.h>
#include <memory/classsizetracker.h>
#include <p3d/anim/multicontroller.hpp>
#include <p3d/utility.hpp>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guiwindow.h>
#include <presentation/gui/utility/colourutility.h>
#include <presentation/gui/utility/transitions.h>
#include <radmath/trig.hpp>
#include <sprite.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================
#define WIDESCREEN_EXTRA_PIXELS 107
//===========================================================================
// Public Member Functions
//===========================================================================

namespace GuiSFX
{

#ifdef DEBUGWATCH
    void ActivateCallback( void* userData )
    {
        Transition* transition = reinterpret_cast< Transition* >( userData );
        Chainable*  chainable = dynamic_cast< Chainable* >( transition );
        if( chainable != NULL )
        {
            chainable->ResetChain();
            chainable->Activate();
        }
        else
        {
            transition->Reset();
            transition->Activate();
        }
    }

    void DeativateCallback( void* userData )
    {
        Transition* transition = reinterpret_cast< Transition* >( userData );
        transition->Deactivate();
    }

#endif

//==============================================================================
// Chainable::Chainable
//==============================================================================
// Description: constructor
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Chainable::Chainable()
{
}

//==============================================================================
// Chainable::Chainable
//==============================================================================
// Description: constructor
//
// Parameters:	name - the name of the transition.
//
// Return:      N/A.
//
//==============================================================================
Chainable::Chainable( const tName& name ):
    Transition( name )
{
    //CLASSTRACKER_CREATE( Chainable );
}


//==============================================================================
// Chainable1::Chainable1
//==============================================================================
// Description: constructor
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Chainable1::Chainable1():
    m_NextTransition( NULL )
{
    //nothing
}

//==============================================================================
// Chainable1::Chainable1
//==============================================================================
// Description: constructor
//
// Parameters:	name - the name of the object
//
// Return:      N/A.
//
//==============================================================================
Chainable1::Chainable1( const tName& name )
    : Chainable( name )
{
    //nothing
}

//==============================================================================
// Chainable1::ContinueChain
//==============================================================================
// Description: moves on to the next transition in the chain
//
// Parameters:	none
//
// Return:      N/A.
//
//==============================================================================
void Chainable1::ContinueChain()
{
    Deactivate();
    if( m_NextTransition != NULL )
    {
        m_NextTransition->Activate();
    }
}

//==============================================================================
// Chainable1::DeactivateChain
//==============================================================================
// Description: shuts down the entire chain
//
// Parameters:	none
//
// Return:      N/A.
//
//==============================================================================
void Chainable1::DeactivateChain()
{
    Deactivate();
    if( m_NextTransition != NULL )
    {
        m_NextTransition->DeactivateChain();
    }
}

//==============================================================================
// Chainable1::IsChainDone
//==============================================================================
// Description: determines if the entire chain of transitions is done
//
// Parameters:	none
//
// Return:      N/A.
//
//==============================================================================
bool Chainable1::IsChainDone() const
{
    if( !IsDone() )
    {
        return false;
    }

    //
    // Yay recursion!
    //
    if( m_NextTransition != NULL )
    {
        return m_NextTransition->IsChainDone();
    }
    return true;
}

//==============================================================================
// Chainable1::operator=
//==============================================================================
// Description: assignment operator
//
// Parameters:	right - the one we're assigning from
//
// Return:      reference to self
//
//==============================================================================
Chainable1& Chainable1::operator=( const Chainable1& right )
{
    if( this == &right )
    {
        return *this;
    }
    Transition::operator=( right );
    m_NextTransition = right.m_NextTransition;
    return *this;
}

//==============================================================================
// Chainable1::ResetChain
//==============================================================================
// Description: resets the transition, and all subsequent transitions
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
void Chainable1::ResetChain()
{
    Reset();
    if( m_NextTransition != NULL )
    {
        return m_NextTransition->ResetChain();
    }

}

//==============================================================================
// Chainable1::SetNextTransition
//==============================================================================
// Description: sets the next transition in the chain
//
// Parameters:	transition - the next transition to use
//
// Return:      N/A.
//
//==============================================================================
void Chainable1::SetNextTransition( Chainable* transition )
{
    m_NextTransition = transition;
}

//==============================================================================
// Chainable1::SetNextTransition
//==============================================================================
// Description: sets the next transition in the chain
//
// Parameters:	transition - the next transition to use
//
// Return:      N/A.
//
//==============================================================================
void Chainable1::SetNextTransition( Chainable& transition )
{
    m_NextTransition = &transition;
}

//==============================================================================
// Chainable2::Chainable2()
//==============================================================================
// Description: consturctor
//
// Parameters:	none
//
// Return:      N/A.
//
//==============================================================================
Chainable2::Chainable2()
{
    m_NextTransition[ 0 ] = NULL;
    m_NextTransition[ 1 ] = NULL;
}

//==============================================================================
// Chainable2::Chainable2()
//==============================================================================
// Description: consturctor
//
// Parameters:	name - the name of the transition
//
// Return:      N/A.
//
//==============================================================================
Chainable2::Chainable2( const tName& name ):
    Chainable( name )
{
    m_NextTransition[ 0 ] = NULL;
    m_NextTransition[ 1 ] = NULL;
}

//==============================================================================
// Chainable2::ContinueChain()
//==============================================================================
// Description: continues the chain of transitions
//
// Parameters:	name - the name of the transition
//
// Return:      N/A.
//
//==============================================================================
void Chainable2::ContinueChain()
{
    Deactivate();
    int i;
    for( i = 0; i < 2; ++i )
    {
        if( m_NextTransition[ i ] != NULL )
        {
            m_NextTransition[ i ]->Activate();
        }
    }
}

//==============================================================================
// Chainable2::DeactivateChain()
//==============================================================================
// Description: shuts down the entire chain
//
// Parameters:	none
//
// Return:      N/A.
//
//==============================================================================
void Chainable2::DeactivateChain()
{
    Deactivate();
    int i;
    for( i = 0; i < 2; ++i )
    {
        if( m_NextTransition[ i ] != NULL )
        {
            m_NextTransition[ i ]->DeactivateChain();
        }
    }
}

//==============================================================================
// Chainable2::IsChainDone
//==============================================================================
// Description: determines if the entire chain of transitions is done
//
// Parameters:	none
//
// Return:      N/A.
//
//==============================================================================
bool Chainable2::IsChainDone() const
{
    if( !IsDone() )
    {
        return false;
    }

    //
    // Yay recursion!
    //
    int i;
    for( i = 0; i < 2; ++i )
    {
        if( m_NextTransition[ i ] != NULL )
        {
            bool nextDone = m_NextTransition[ i ]->IsChainDone();
            if( !nextDone )
            {
                return false;
            }
        }
    }
    return true;
}

//==============================================================================
// Chainable2::ResetChain
//==============================================================================
// Description: resets the transition, and all subsequent transitions
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
void Chainable2::ResetChain()
{
    Reset();
    if( m_NextTransition[ 0 ] != NULL )
    {
        m_NextTransition[ 0 ]->ResetChain();
    }

    if( m_NextTransition[ 1 ] != NULL )
    {
        m_NextTransition[ 1 ]->ResetChain();
    }
}

//==============================================================================
// Chainable2::SetNextTransition
//==============================================================================
// Description: sets one of the next transitions triggered by this one
//
// Parameters:	index - which one of the next transitions to set
//              transition - pointer to the transition
//
// Return:      N/A.
//
//==============================================================================
void Chainable2::SetNextTransition( const unsigned int index, Chainable* transition )
{
    rAssert( index < 2 );
    m_NextTransition[ index ] = transition;
}

//==============================================================================
// Chainable2::SetNextTransition
//==============================================================================
// Description: sets one of the next transitions triggered by this one
//
// Parameters:	index - which one of the next transitions to set
//              transition - pointer to the transition
//
// Return:      N/A.
//
//==============================================================================
void Chainable2::SetNextTransition( const unsigned int index, Chainable& transition )
{
    SetNextTransition( index, &transition );
}

//==============================================================================
// Chainable3::Chainable3()
//==============================================================================
// Description: consturctor
//
// Parameters:	none
//
// Return:      N/A.
//
//==============================================================================
Chainable3::Chainable3()
{
    m_NextTransition[ 0 ] = NULL;
    m_NextTransition[ 1 ] = NULL;
    m_NextTransition[ 2 ] = NULL;
}

//==============================================================================
// Chainable3::Chainable3()
//==============================================================================
// Description: consturctor
//
// Parameters:	name - the name of the transition
//
// Return:      N/A.
//
//==============================================================================
Chainable3::Chainable3( const tName& name ):
    Chainable( name )
{
    m_NextTransition[ 0 ] = NULL;
    m_NextTransition[ 1 ] = NULL;
    m_NextTransition[ 2 ] = NULL;
}

//==============================================================================
// Chainable3::ContinueChain()
//==============================================================================
// Description: continues the chain of transitions
//
// Parameters:	name - the name of the transition
//
// Return:      N/A.
//
//==============================================================================
void Chainable3::ContinueChain()
{
    Deactivate();
    int i;
    for( i = 0; i < 3; ++i )
    {
        if( m_NextTransition[ i ] != NULL )
        {
            m_NextTransition[ i ]->Activate();
        }
    }
}

//==============================================================================
// Chainable3::DeactivateChain()
//==============================================================================
// Description: shuts down the entire chain
//
// Parameters:	none
//
// Return:      N/A.
//
//==============================================================================
void Chainable3::DeactivateChain()
{
    Deactivate();
    int i;
    for( i = 0; i < 3; ++i )
    {
        if( m_NextTransition[ i ] != NULL )
        {
            m_NextTransition[ i ]->DeactivateChain();
        }
    }
}


//==============================================================================
// Chainable3::IsChainDone
//==============================================================================
// Description: determines if the entire chain of transitions is done
//
// Parameters:	none
//
// Return:      N/A.
//
//==============================================================================
bool Chainable3::IsChainDone() const
{
    if( !IsDone() )
    {
        return false;
    }

    //
    // Yay recursion!
    //
    int i;
    for( i = 0; i < 3; ++i )
    {
        if( m_NextTransition[ i ] != NULL )
        {
            bool nextDone = m_NextTransition[ i ]->IsChainDone();
            if( !nextDone )
            {
                return false;
            }
        }
    }
    return true;
}

//==============================================================================
// Chainable3::ResetChain
//==============================================================================
// Description: resets the transition, and all subsequent transitions
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
void Chainable3::ResetChain()
{
    Reset();
    if( m_NextTransition[ 0 ] != NULL )
    {
        m_NextTransition[ 0 ]->ResetChain();
    }

    if( m_NextTransition[ 1 ] != NULL )
    {
        m_NextTransition[ 1 ]->ResetChain();
    }

    if( m_NextTransition[ 2 ] != NULL )
    {
        m_NextTransition[ 2 ]->ResetChain();
    }
}

//==============================================================================
// Chainable3::SetNextTransition
//==============================================================================
// Description: sets one of the next transitions triggered by this one
//
// Parameters:	index - which one of the next transitions to set
//              transition - pointer to the transition
//
// Return:      N/A.
//
//==============================================================================
void Chainable3::SetNextTransition( const unsigned int index, Chainable* transition )
{
    rAssert( index < 3 );
    m_NextTransition[ index ] = transition;
}

//==============================================================================
// Chainable3::SetNextTransition
//==============================================================================
// Description: sets one of the next transitions triggered by this one
//
// Parameters:	index - which one of the next transitions to set
//              transition - pointer to the transition
//
// Return:      N/A.
//
//==============================================================================
void Chainable3::SetNextTransition( const unsigned int index, Chainable& transition )
{
    SetNextTransition( index, &transition );
}

//==============================================================================
// ColorChange::ColorChange
//==============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
ColorChange::ColorChange()
{
}

//==============================================================================
// ColorChange::ColorChange
//==============================================================================
// Description: constructor
//
// Parameters:	name - the name of the object
//
// Return:      N/A.
//
//==============================================================================
ColorChange::ColorChange( const tName& name ):
    Chainable1( name )
{
}

//==============================================================================
// ColorChange::SetStartColour
//==============================================================================
// Description: sets the starting color for the color transition
//
// Parameters:	transition - the next transition to use
//
// Return:      N/A.
//
//==============================================================================
void ColorChange::SetStartColour( const tColour c )
{
    m_StartColor = c;
}

//==============================================================================
// ColorChange::SetEndColour
//==============================================================================
// Description: sets the end color for the transition
//
// Parameters:	transition - the next transition to use
//
// Return:      N/A.
//
//==============================================================================
void ColorChange::SetEndColour( const tColour c )
{
    m_EndColor = c;
}

//==============================================================================
// ColorChange::Update
//==============================================================================
// Description: updates the color change transition
//
// Parameters:	transition - the next transition to use
//
// Return:      N/A.
//
//==============================================================================
void ColorChange::Update( const float deltaT )
{
    if( this->IsActive() )
    {
        Transition::Update( deltaT );
        float spillover = rmt::Max( m_ElapsedTime - m_TimeInterval, 0.0f );
        m_ElapsedTime = rmt::Min( m_ElapsedTime, m_TimeInterval );

        float percent = m_ElapsedTime / m_TimeInterval;
        tColour color = m_StartColor * ( 1.0f - percent ) + m_EndColor * percent;
        if( m_Drawable != NULL )
        {
            m_Drawable->SetColour( color );
        }

        if( spillover > 0 )
        {
            ContinueChain();
        }
    }
}

//==============================================================================
// ColorChange::Watch
//==============================================================================
// Description: Adds this transition to the watcher
//
// Parameters:	nameSpace - the name of the group in the watcher
//
// Return:      N/A.
//
//==============================================================================
#ifdef DEBUGWATCH
void ColorChange::Watch( const char* nameSpace )
{
    char output[ 1024 ];
    sprintf( output, "%s\\%s", nameSpace, m_Name.GetText() );
    Parent1::Watch( output );
    Parent2::Watch( output );
}
#endif

//==============================================================================
// Dummy::Dummy
//==============================================================================
// Description: Constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
Dummy::Dummy()
{
}

//==============================================================================
// Dummy::Dummy
//==============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
Dummy::Dummy( const tName& name ):
    Parent( name )
{
}

//==============================================================================
// Dummy::Activate
//==============================================================================
// Description: does nothing, and carrys on
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
void Dummy::Activate()
{
    Parent::Activate();
    ContinueChain();
}

//=============================================================================
// GotoScreen::GotoScreen()
//=============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
GotoScreen::GotoScreen():
    mScreen( CGuiWindow::GUI_WINDOW_ID_UNDEFINED ),
    mParam1( 0 ),
    mParam2( 0 ),
    mWindowOptions( 0 )
{
    //nothing
}

//=============================================================================
// GotoScreen::GotoScreen()
//=============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
GotoScreen::GotoScreen( const tName& name ):
    Parent( name ),
    mScreen( CGuiWindow::GUI_WINDOW_ID_UNDEFINED ),
    mParam1( 0 ),
    mParam2( 0 ),
    mWindowOptions( 0 )
{
    //nothing
}

//=============================================================================
// SendEvent::Activate()
//=============================================================================
// Description: fires off the event that we're supposed to send at this point
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
void GotoScreen::Activate()
{
    Transition::Activate();
    rAssert( mScreen != CGuiWindow::GUI_WINDOW_ID_UNDEFINED );
    GetGuiSystem()->GotoScreen( mScreen, mParam1, mParam2, mWindowOptions );
    ContinueChain();
}

//=============================================================================
// SendEvent::SetEventData
//=============================================================================
// Description: sets the event data that we're going to fire off later on 
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
void GotoScreen::SetEventData( void* eventData )
{
    mEventData = eventData;
}

//=============================================================================
// GotoScreen::SetParam1
//=============================================================================
// Description: sets the event data that we're going to fire off later on 
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
void GotoScreen::SetParam1( const unsigned int param1 )
{
    mParam1 = param1;
}

//=============================================================================
// GotoScreen::SetParam2
//=============================================================================
// Description: sets the event data that we're going to fire off later on 
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
void GotoScreen::SetParam2( const unsigned int param2 )
{
    mParam2 = param2;
}

//=============================================================================
// SendEvent::SetEvent
//=============================================================================
// Description: sets the event that we're going to fire off later on 
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
void GotoScreen::SetScreen( CGuiWindow::eGuiWindowID screen )
{
    mScreen = screen;
}

//=============================================================================
// GotoScreen::SetWindowOptions
//=============================================================================
// Description: sets the event data that we're going to fire off later on 
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
void GotoScreen::SetWindowOptions( const unsigned int windowOptions )
{
    mWindowOptions = windowOptions;
}

//==============================================================================
// HasMulticontroller::ResetMultiControllerFrames
//==============================================================================
// Description: sometimes you screw up the nubmer of frames in a multicontoller
//              calling this function will fix things
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
void HasMulticontroller::ResetMultiControllerFrames()
{
    m_MultiController->SetFrameRange( 0.0f, m_NumFrames );
}

//==============================================================================
// HasMulticontroller::SetMultiController
//==============================================================================
// Description: sets up the multicontroller that this transition requires
//
// Parameters:	multicontroller - the multicontroller to use
//
// Return:      N/A.
//
//==============================================================================
void HasMulticontroller::SetMultiController( tMultiController* multicontroller )
{
    m_MultiController = multicontroller;
    m_NumFrames = m_MultiController->GetNumFrames();
}

//==============================================================================
// HasTimeInterval::HasTimeInterval
//==============================================================================
// Description: constructor
//
// Parameters:	none
//
// Return:      N/A.
//
//==============================================================================
HasTimeInterval::HasTimeInterval():
    m_TimeInterval( 1000.0f )
{
    //nothing
}

//==============================================================================
// HasTimeInterval::SetTimeInterval
//==============================================================================
// Description: sets the time interval that this transition should wait
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
void HasTimeInterval::SetTimeInterval( const float interval )
{
    m_TimeInterval = interval;
}

//==============================================================================
// HasTimeInterval::Watch
//==============================================================================
// Description: Adds this transition to the watcher
//
// Parameters:	nameSpace - the name of the group in the watcher
//
// Return:      N/A.
//
//==============================================================================
#ifdef DEBUGWATCH
void HasTimeInterval::Watch( const char* nameSpace )
{
    radDbgWatchDelete( &m_TimeInterval   );
    radDbgWatchAddFloat( &m_TimeInterval, "timeInterval", nameSpace, NULL, NULL, 0.0f, 1000.0f );
}
#endif

//==============================================================================
// Hide::Hide
//==============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
Hide::Hide()
{
    //nothing
}

//==============================================================================
// Hide::Hide
//==============================================================================
// Description: constructor
//
// Parameters:	name - the name of the object
//
// Return:      N/A.
//
//==============================================================================
Hide::Hide( const tName& name ):
    Chainable1( name )
{
}

//==============================================================================
// Hide::Activate
//==============================================================================
// Description: this gets called when a hide transition is triggered
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
void Hide::Activate()
{
    Transition::Activate();
    rAssert( m_Drawable != NULL );
    if( m_Drawable != NULL )
    {
        m_Drawable->SetVisible( false );
    }
    ContinueChain();
}

//==============================================================================
// ImageCycler::SetDrawable
//==============================================================================
// Description: set the period for cycling images
//
// Parameters:	period - float period in ms
//
// Return:      N/A.
//
//==============================================================================
void ImageCycler::SetDrawable( Scrooby::Drawable* drawable )
{
    Transition::SetDrawable( drawable );
    m_Sprite = dynamic_cast< Scrooby::Sprite* >( m_Drawable );
    rAssert( m_Sprite != NULL );
}

//==============================================================================
// ImageCycler::SetDrawable
//==============================================================================
// Description: set the period for cycling images
//
// Parameters:	period - float period in ms
//
// Return:      N/A.
//
//==============================================================================
void ImageCycler::SetDrawable( Scrooby::Drawable& drawable )
{
    Transition::SetDrawable( drawable );
    m_Sprite = dynamic_cast< Scrooby::Sprite* >( m_Drawable );
    rAssert( m_Sprite != NULL );
}

//==============================================================================
// InputStateChange::InputStateChange
//==============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
InputStateChange::InputStateChange():
    Parent(),
    mState( Input::ACTIVE_NONE )
{
    //nothing
}

//==============================================================================
// InputStateChange::InputStateChange
//==============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
InputStateChange::InputStateChange( const tName& name ):
    Parent( name ),
    mState( Input::ACTIVE_NONE )
{
    //nothing
}

//==============================================================================
// InputStateChange::Activate
//==============================================================================
// Description: called when this transition is activated
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
void InputStateChange::Activate()
{
    Parent::Activate();
    InputManager::GetInstance()->SetGameState( mState );
    ContinueChain();
}

//==============================================================================
// InputStateChange::SetState
//==============================================================================
// Description: set the state that this transition will take the input to
//
// Parameters:	state - what input state should we go to?
//
// Return:      N/A.
//
//==============================================================================
void InputStateChange::SetState( const Input::ActiveState state )
{
    mState = state;
}

//==============================================================================
// IrisWipeClose::IrisWipeClose
//==============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
IrisWipeClose::IrisWipeClose()
{
    //none
}

//==============================================================================
// IrisWipeClose::IrisWipeClose
//==============================================================================
// Description: constructor
//
// Parameters:	name - the name of the object for debugging
//
// Return:      N/A.
//
//==============================================================================
IrisWipeClose::IrisWipeClose( const tName& name ):
    Chainable1( name )
{
    //none
}

//==============================================================================
// IrisWipeClose::Activate
//==============================================================================
// Description: starts the iris wipe transition 
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
void IrisWipeClose::Activate()
{
    m_MultiController->Reset();
    m_MultiController->SetFrameRange( 0, m_NumFrames * 0.5f );
    m_MultiController->SetFrame( 0 );
    Chainable1::Activate();
}

void IrisWipeClose::Deactivate()
{
    Chainable1::Deactivate();
}

//==============================================================================
// IrisWipeOpen::Update
//==============================================================================
// Description: needed to check if the irisWipeTransition can move on
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
void IrisWipeClose::Update( const float deltaT )
{
    if( IsActive() )
    {
        Chainable1::Update( deltaT );
        if( m_MultiController != NULL )
        {
            if( m_MultiController->LastFrameReached() )
            {
                m_MultiController->SetFrameRange( 0.0f, m_NumFrames );
                ResetMultiControllerFrames();
                ContinueChain();
            }
        }
    }
}

//==============================================================================
// IrisWipeOpen::IrisWipeOpen
//==============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
IrisWipeOpen::IrisWipeOpen()
{
    //none
}

//==============================================================================
// IrisWipeOpen::IrisWipeOpen
//==============================================================================
// Description: constructor
//
// Parameters:	name - the name of the object for debugging
//
// Return:      N/A.
//
//==============================================================================
IrisWipeOpen::IrisWipeOpen( const tName& name ):
    Chainable1( name )
{
    //none
}

//==============================================================================
// IrisWipeOpen::Activate
//==============================================================================
// Description: starts the iris wipe transition 
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
void IrisWipeOpen::Activate()
{
    tMultiController* multiController = p3d::find< tMultiController >( "IrisController" );
    rAssert( m_MultiController != NULL );
    SetMultiController( multiController );

    m_MultiController->Reset();
    m_MultiController->SetFrameRange( m_NumFrames * 0.5f, m_NumFrames );
    m_MultiController->SetFrame( m_NumFrames * 0.5f );
    Chainable1::Activate();
}

//==============================================================================
// IrisWipeOpen::Update
//==============================================================================
// Description: needed to check if the irisWipeTransition can move on
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
void IrisWipeOpen::Update( const float deltaT )
{
    if( IsActive() )
    {
        if( m_MultiController->LastFrameReached() )
        {
            m_MultiController->SetFrameRange( 0.0f, m_NumFrames );
            ResetMultiControllerFrames();
            ContinueChain();
        }
    }
}

//==============================================================================
// ImageCycler::SetPeriod
//==============================================================================
// Description: set the period for cycling images
//
// Parameters:	period - float period in ms
//
// Return:      N/A.
//
//==============================================================================
void ImageCycler::SetPeriod( const float period )
{
    m_Period = period;
}

//==============================================================================
// ImageCycler::Update
//==============================================================================
// Description: updates the transition animation
//
// Parameters:	deltaT - the time elapsed
//
// Return:      N/A.
//
//==============================================================================
void ImageCycler::Update( const float deltaT )
{
    if( IsActive() )
    {
        Transition::Update( deltaT );
        if( m_Sprite != NULL )
        {
            int nubmerOfStates = m_Sprite->GetNumOfImages();
            int periodsElapsed = static_cast< int >( m_ElapsedTime / m_Period );
            int imageNumber = periodsElapsed % nubmerOfStates;
            m_Sprite->SetIndex( imageNumber );
        }
    }
}

//==============================================================================
// Junction2::Activate()
//==============================================================================
// Description: activates all the transitions leaving this junction
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
void Junction2::Activate()
{
    ContinueChain();
}

//==============================================================================
// Junction3::Activate()
//==============================================================================
// Description: activates all the transitions leaving this junction
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
void Junction3::Activate()
{
    ContinueChain();
}

//==============================================================================
// Pause::Pause
//==============================================================================
// Description: constructor
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Pause::Pause()
{
    //nothing
}

//==============================================================================
// Pause::Pause
//==============================================================================
// Description: constructor
//
// Parameters:	name - the name of the object
//
// Return:      N/A.
//
//==============================================================================
Pause::Pause( const tName& name ):
    Chainable1( name )
{
}

//==============================================================================
// Pause::Update
//==============================================================================
// Description: Updates the transition
//
// Parameters:	elapsed time - how much time has passed
//
// Return:      N/A.
//
//==============================================================================
void Pause::Update( const float deltaT )
{
    if( IsActive() )
    {
        Transition::Update( deltaT );
        float spillover = rmt::Max( m_ElapsedTime - m_TimeInterval, 0.0f );
        m_ElapsedTime = rmt::Min( m_ElapsedTime, m_TimeInterval );
        if( spillover > 0 )
        {
            ContinueChain();
        }
    }
}

//=============================================================================
// Pause::Watch
//=============================================================================
// Description: adds the pause transition object to the watcher
//
// Parameters:	deleaT - how much time has passed
//
// Return:      N/A.
//
//=============================================================================
#ifdef DEBUGWATCH
void Pause::Watch( const char* nameSpace )
{
    char output[ 1024 ];
    sprintf( output, "%s\\%s", nameSpace, m_Name.GetText() );
    Parent1::Watch( output );
    Parent2::Watch( output );
}
#endif

//==============================================================================
// PauseInFrames::PauseInFrames
//==============================================================================
// Description: constructor
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
PauseInFrames::PauseInFrames():
    m_FramesToPause( 1 ),
    m_FramesElapsed( 0 )
{
    //nothing
}

//==============================================================================
// Pause::Pause
//==============================================================================
// Description: constructor
//
// Parameters:	name - the name of the object
//
// Return:      N/A.
//
//==============================================================================
PauseInFrames::PauseInFrames( const tName& name ):
    Parent( name ),
    m_FramesToPause( 1 ),
    m_FramesElapsed( 0 )
{
    //nothing
}

//==============================================================================
// PauseInFrames::Reset
//==============================================================================
// Description: Resets the transition
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
void PauseInFrames::Reset()
{
    Parent::Reset();
    m_FramesElapsed = 0;
}

//==============================================================================
// PauseInFrames::SetNumberOfFrames
//==============================================================================
// Description: How many frames should this transition pause for?
//
// Parameters:	i - thenubmer of fraems to pause
//
// Return:      N/A.
//
//==============================================================================
void PauseInFrames::SetNumberOfFrames( const unsigned int i )
{
    m_FramesToPause = i;
}

//==============================================================================
// PauseInFrames::Update
//==============================================================================
// Description: Updates the transition
//
// Parameters:	elapsed time - how much time has passed
//
// Return:      N/A.
//
//==============================================================================
void PauseInFrames::Update( const float deltaT )
{
    if( IsActive() )
    {
        Parent::Update( deltaT );

        if( deltaT > 0.0f )
        {
            ++m_FramesElapsed;
        }

        if( m_FramesElapsed == m_FramesToPause )
        {
            ContinueChain();
        }
    }
}

//==============================================================================
// PauseGame::Activate
//==============================================================================
// Description: transition that pauses the game
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
void PauseGame::Activate()
{
    GameplayContext::GetInstance()->PauseAllButPresentation( true );
    ContinueChain();
}

//==============================================================================
// PulseScale::PulseScale
//==============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
PulseScale::PulseScale():
    Transition(),
    m_BoundedDrawable( NULL ),
    m_Amplitude( 0.25f ),
    m_Frequency( 5.0f )
{
}

//==============================================================================
// PulseScale::PulseScale
//==============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
PulseScale::PulseScale( const tName& name ):
    Transition( name ),
    m_BoundedDrawable( NULL ),
    m_Amplitude( 0.25f ),
    m_Frequency( 5.0f )
{
}

//==============================================================================
// PulseScale::MovesDrawable
//==============================================================================
// Description: does this transition move a drawable object?
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
bool PulseScale::MovesDrawable() const
{
    return true;
}

//==============================================================================
// PulseScale::SetAmplitude
//==============================================================================
// Description: sets the amplitude of the pulsing
//
// Parameters:	amplitude - the size of the pulsing
//
// Return:      N/A.
//
//==============================================================================
void PulseScale::SetAmplitude( const float amplitude )
{
    m_Amplitude = amplitude;
}

//==============================================================================
// PulseScale::SetDrawable
//==============================================================================
// Description: sets the drawable that we'll be pulsing
//
// Parameters:	drawable
//
// Return:      N/A.
//
//==============================================================================
void PulseScale::SetDrawable( Scrooby::Drawable& drawable )
{
    Transition::SetDrawable( drawable );
    m_BoundedDrawable = dynamic_cast< Scrooby::BoundedDrawable* >( m_Drawable );
    rAssert( m_BoundedDrawable != NULL );
}

//==============================================================================
// PulseScale::SetDrawable
//==============================================================================
// Description: sets the drawable that we'll be pulsing
//
// Parameters:	drawable
//
// Return:      N/A.
//
//==============================================================================
void PulseScale::SetDrawable( Scrooby::Drawable* drawable )
{
    Transition::SetDrawable( drawable );
    m_BoundedDrawable = dynamic_cast< Scrooby::HasBoundingBox* >( m_Drawable );
    rAssert( m_BoundedDrawable != NULL );
}

//==============================================================================
// Pulse::SetFrequency
//==============================================================================
// Description: sets the frequency of the pulse effect (in Hz)
//
// Parameters:	frequency - the frequency
//
// Return:      N/A.
//
//==============================================================================
void PulseScale::SetFrequency( const float frequency )
{
    m_Frequency = frequency;
}

//=============================================================================
// Pulse::Update
//=============================================================================
// Description: causes the bitmap to pulse
//
// Parameters:	deleaT - how much time has passed
//
// Return:      N/A.
//
//=============================================================================
void PulseScale::Update( const float deltaT )
{
    if( IsActive() )
    {
        Transition::Update( deltaT );
        float period = 1000.0f / m_Frequency;
        m_ElapsedTime = fmodf( m_ElapsedTime, period );
        float sin = rmt::Sin( m_ElapsedTime * m_Frequency / 1000.0f * rmt::PI * 2 );
        float scale = sin * m_Amplitude + 1.0f;
        m_BoundedDrawable->ScaleAboutCenter( scale );
    }
}

//=============================================================================
// PulseScale::Watch
//=============================================================================
// Description: adds the pulse transition object to the watcher
//
// Parameters:	deleaT - how much time has passed
//
// Return:      N/A.
//
//=============================================================================
#ifdef DEBUGWATCH
void PulseScale::Watch( const char* nameSpace )
{
    Parent::Watch( nameSpace );
    char output[ 1024 ];
    sprintf( output, "%s\\%s", nameSpace, m_Name.GetText() );
    radDbgWatchDelete( &m_Amplitude );
    radDbgWatchDelete( &m_Frequency );
    radDbgWatchAddFloat( &m_Amplitude, "Amplitude", output, NULL, NULL, 0.0f, 1.0f );
    radDbgWatchAddFloat( &m_Frequency, "Frequency", output, NULL, NULL, 0.0f, rmt::PI * 4 );
}
#endif


//==============================================================================
// RecieveEvent::RecieveEvent
//==============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
RecieveEvent::RecieveEvent():
    Chainable1(),
    mEvent( NUM_EVENTS )
{
    //nothing
}

//=============================================================================
// RecieveEvent::RecieveEvent
//=============================================================================
// Description: constructor
//
// Parameters:	name - the name of the object
//
// Return:      N/A.
//
//=============================================================================
RecieveEvent::RecieveEvent( const tName& name ):
    Chainable1( name ),
    mEvent( NUM_EVENTS )
{
    //nothing
}

//=============================================================================
// RecieveEvent::Activate
//=============================================================================
// Description: activate makes the object start listening for it's event
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
void RecieveEvent::Activate()
{
    rAssertMsg( mEvent != NUM_EVENTS, "you need to set the event to something meaningful before this can be activated" );
    GetEventManager()->AddListener( this, mEvent );
}

//=============================================================================
// RecieveEvent::HandleEvent
//=============================================================================
// Description: we've got the message we were listenting for if we get this
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
void RecieveEvent::HandleEvent( EventEnum id, void* pEventData )
{
    GetEventManager()->RemoveListener( this, mEvent );
    ContinueChain();
}

//=============================================================================
// RecieveEvent::SetEvent
//=============================================================================
// Description: specify which event we should be listenting for
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
void RecieveEvent::SetEvent( const EventEnum event )
{
    mEvent = event;
    //IAN: better ensure that this listener isn't active at the moment
}

//==============================================================================
// ResumeGame::Activate
//==============================================================================
// Description: transition that resumes a paused game the game
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
ResumeGame::ResumeGame():
    Chainable1()
{
    //nothing
}

//==============================================================================
// ResumeGame::Activate
//==============================================================================
// Description: transition that resumes a paused game the game
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
ResumeGame::ResumeGame( const tName& name ):
    Chainable1( name )
{
    //nothing
}

//==============================================================================
// ResumeGame::Activate
//==============================================================================
// Description: transition that resumes a paused game the game
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
void ResumeGame::Activate()
{
    GameplayContext::GetInstance()->PauseAllButPresentation( false );
    ContinueChain();
}

//=============================================================================
// SendEvent::SendEvent()
//=============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
SendEvent::SendEvent():
    mEvent( NUM_EVENTS )
{
    //nothing
}

//=============================================================================
// SendEvent::SendEvent()
//=============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
SendEvent::SendEvent( const tName& name ):
    Chainable1( name ),
    mEvent( NUM_EVENTS )
{
    //nothing
}

//=============================================================================
// SendEvent::Activate()
//=============================================================================
// Description: fires off the event that we're supposed to send at this point
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
void SendEvent::Activate()
{
    Transition::Activate();
    rAssert( mEvent != NUM_EVENTS );
    GetEventManager()->TriggerEvent( mEvent, mEventData );
    ContinueChain();
}

//=============================================================================
// SendEvent::SetEvent
//=============================================================================
// Description: sets the event that we're going to fire off later on 
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
void SendEvent::SetEvent( EventEnum event )
{
    mEvent = event;
}

//=============================================================================
// SendEvent::SetEventData
//=============================================================================
// Description: sets the event data that we're going to fire off later on 
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
void SendEvent::SetEventData( void* eventData )
{
    mEventData = eventData;
}

//=============================================================================
// SwitchContext::SwitchContext()
//=============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
SwitchContext::SwitchContext():
    mContext( NUM_CONTEXTS )
{
    //nothing
}

//=============================================================================
// SwitchContext::SwitchContext()
//=============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
SwitchContext::SwitchContext( const tName& name ):
    Parent( name ),
    mContext( NUM_CONTEXTS )
{
    //nothing
}

//=============================================================================
// SwitchContext::Activate()
//=============================================================================
// Description: changes to the context that we want
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
void SwitchContext::Activate()
{
    Transition::Activate();
    rAssert( mContext != NUM_CONTEXTS );
    GetGameFlow()->SetContext( mContext );
    ContinueChain();
}

//=============================================================================
// SwitchContext::SetContext
//=============================================================================
// Description: sets the context to which we will be going
//
// Parameters:	NONE
//
// Return:      N/A.
//
//=============================================================================
void SwitchContext::SetContext( ContextEnum context )
{
    mContext = context;
}

//==============================================================================
// Show::Show
//==============================================================================
// Description: constructor
//
// Parameters:	NONE
//
// Return:      N/A.
//
//==============================================================================
Show::Show()
{
}
//==============================================================================
// Show::Show
//==============================================================================
// Description: constructor
//
// Parameters:	name - the name of the object
//
// Return:      N/A.
//
//==============================================================================
Show::Show( const tName& name ):
    Chainable1( name )
{
    //nothing
}

//==============================================================================
// Show::Activate
//==============================================================================
// Description: this gets called when a show transition is triggered
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
void Show::Activate()
{
    Transition::Activate();
    rAssert( m_Drawable != NULL );
    if( m_Drawable != NULL )
    {
        m_Drawable->SetVisible( true );
    }
    ContinueChain();
}


//==============================================================================
// Spin::Spin
//==============================================================================
// Description: Constructor
//
// Parameters:	none
//
// Return:      N/A.
//
//==============================================================================
Spin::Spin():
    Transition( "Spin" ),
    m_Period( 1.0f )
{
    //nothing
}

//==============================================================================
// Spin::MovesDrawable
//==============================================================================
// Description: does this transition move a drawable object?
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
bool Spin::MovesDrawable() const
{
    return true;
}

//==============================================================================
// Spin::SetPeriod
//==============================================================================
// Description: Sets the period of the spinning object
//
// Parameters:	period - float time in seconds for the period
//
// Return:      N/A.
//
//==============================================================================
void Spin::SetDrawable( Scrooby::Drawable* drawable )
{
    Transition::SetDrawable( drawable );
    m_BoundedDrawable = dynamic_cast< Scrooby::BoundedDrawable* >( drawable );
    rAssert( m_BoundedDrawable != NULL );
}

//==============================================================================
// Spin::SetPeriod
//==============================================================================
// Description: Sets the period of the spinning object
//
// Parameters:	period - float time in seconds for the period
//
// Return:      N/A.
//
//==============================================================================
void Spin::SetPeriod( const float period )
{
    m_Period = period;
}

//==============================================================================
// Spin::Update
//==============================================================================
// Description: Does the animation
//
// Parameters:	deltaT - elapsed time
//
// Return:      N/A.
//
//==============================================================================
void Spin::Update( const float deltaT )
{
    if( IsActive() )
    {
        Transition::Update( deltaT );
        m_ElapsedTime = fmodf( m_ElapsedTime, m_Period * 1000.0f );
        float angle = m_ElapsedTime / ( m_Period * 1000 ) * 360.0f;
        if( m_BoundedDrawable != NULL )
        {
            m_BoundedDrawable->RotateAboutCenter( angle );
        }
    }
}

//==============================================================================
// Transition::Transition
//==============================================================================
// Description: constructor
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Transition::Transition():
#ifdef RAD_DEBUG
    m_Name( "NONE" ),
#endif
    m_Active( false ),
    m_Done( false ),
    m_Drawable( NULL ),
    m_ElapsedTime( 0.0f )
{
#ifdef RAD_DEBUG
    static int count = 0;
    m_Id = count;
    ++count;
#endif
}

//==============================================================================
// Transition::Transition
//==============================================================================
// Description: constructor
//
// Parameters:	name - the name to assign - only in debug builds.
//
// Return:      N/A.
//
//==============================================================================
Transition::Transition( const tName& name ):
    m_Active( false ),
    m_Done( false ),
    m_Drawable( NULL ),
    m_ElapsedTime( 0.0f )    
{
#ifdef RAD_DEBUG
    m_Name = name;
#endif
}

//==============================================================================
// Transition::Activate
//==============================================================================
// Description: activates the transition
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
void Transition::Activate()
{
    #ifdef RAD_DEBUG
        //rAssert( m_Name != "NONE" );
        if( m_Name.GetUID() != static_cast< tUID >( 0 ) )
        {
            const char* name = m_Name.GetText();
            rDebugPrintf( "Transition Starting - %s\n", name );
        }
    #endif

    m_Active = true;
    m_Done = false;
    Update( 0.0f );
}

//==============================================================================
// Transition::Deactivate
//==============================================================================
// Description: deactivates the transition
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
void Transition::Deactivate()
{
    m_Active = false;
    m_Done = true;
}

//==============================================================================
// Transition::GetDrawable
//==============================================================================
// Description: returns the drawable associated with this transition
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Scrooby::Drawable* Transition::GetDrawable()
{
    return m_Drawable;
}

//==============================================================================
// Transition::IsActive
//==============================================================================
// Description: queries whether or not the transition is active
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
bool Transition::IsActive() const
{
    return m_Active;
}

//==============================================================================
// Transition::IsDone
//==============================================================================
// Description: queries whether or not the transition is done
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
bool Transition::IsDone() const
{
    return m_Done;
}

//==============================================================================
// Transition::MovesDrawable
//==============================================================================
// Description: does this transition move a drawable object?
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
bool Transition::MovesDrawable() const
{
    return false;
}

//==============================================================================
// Transition::operator=
//==============================================================================
// Description: assignment operator
//
// Parameters:	right - what we're assigning
//
// Return:      N/A.
//
//==============================================================================
Transition& Transition::operator=( const Transition& right )
{
    if( this == &right)
    {
        return *this;
    }
    m_Active        = right.m_Active;
    m_Drawable      = right.m_Drawable;
    m_ElapsedTime   = right.m_ElapsedTime;
    return *this;
}

//==============================================================================
// Transition::Reset
//==============================================================================
// Description: resets the transition to it's 0 time state
//
// Parameters:	None
//
// Return:      N/A.
//
//==============================================================================
void Transition::Reset()
{
    m_ElapsedTime = 0;
    m_Done = false;
}

//==============================================================================
// Transition::SetDrawable
//==============================================================================
// Description: sets the drawable that this transition will apply to
//
// Parameters:	drawable - the drawable
//
// Return:      N/A.
//
//==============================================================================
void Transition::SetDrawable( Scrooby::Drawable* drawable )
{
    m_Drawable = drawable;
}

//==============================================================================
// Transition::SetDrawable
//==============================================================================
// Description: sets the drawable that this transition will apply to
//
// Parameters:	drawable - the drawable
//
// Return:      N/A.
//
//==============================================================================
void Transition::SetDrawable( Scrooby::Drawable& drawable )
{
    m_Drawable = &drawable;
}

//=============================================================================
// Transition::Update
//=============================================================================
// Description: applies the transition to the front end element being modified
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
void Transition::Update( const float elapsedTime )
{
    if( !IsActive() )
    {
        return;
    }

    //rDebugPrintf(   "%s\n", m_Name.GetText() );

    m_ElapsedTime += elapsedTime;
}

//=============================================================================
// Transition::Watch
//=============================================================================
// Description: adds a function to thw watcher to enable/disable this
//              transition
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
#ifdef DEBUGWATCH
void Transition::Watch( const char* nameSpace )
{
    char output[ 1024 ];
    sprintf( output, "%s\\%s", nameSpace, m_Name.GetText() );
    radDbgWatchDelete( &m_DummyVariableForActivate );
    radDbgWatchDelete( &m_DummyVariableForDeactivate );
    radDbgWatchAddBoolean( &m_DummyVariableForActivate,   "Activate",  output, ActivateCallback,  this );
    radDbgWatchAddBoolean( &m_DummyVariableForDeactivate, "Deativate", output, DeativateCallback, this );
}
#endif


//==============================================================================
// Translator::Translator
//==============================================================================
// Description: constructor
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Translator::Translator():
    startX( 0 ),
    startY( 0 ),
    endX( 0 ),
    endY( 0 )
{
    //nothing
}

//==============================================================================
// Translator::Translator
//==============================================================================
// Description: constructor
//
// Parameters:	name - name of the object
//
// Return:      N/A.
//
//==============================================================================
Translator::Translator( const tName& name ):
    Chainable1( name ),
    startX( 0 ),
    startY( 0 ),
    endX( 0 ),
    endY( 0 )
{
}

//==============================================================================
// Translator::~Translator
//==============================================================================
// Description: destructor
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Translator::~Translator()
{
    //nothing
}

//=============================================================================
// Translator::MateCoordsEnd
//=============================================================================
// Description: match up the end coordinates of the transition with where the
//              scrooby object is supposed to be
//
// Parameters:	drawable - the object to match up with
//
// Return:      N/A.
//
//=============================================================================
void Translator::MateCoordsEnd( const Scrooby::HasBoundingBox* drawable )
{
    SetCoordsEnd( 0, 0 );
}

//=============================================================================
// Translator::MateCoordsStart
//=============================================================================
// Description: match up the start coordinates of the transition with where the
//              scrooby object is supposed to be
//
// Parameters:	drawable - the object to match up with
//
// Return:      N/A.
//
//=============================================================================
void Translator::MateCoordsStart( const Scrooby::HasBoundingBox* drawable )
{
    SetCoordsStart( 0, 0 );
}

//==============================================================================
// Translator::MovesDrawable
//==============================================================================
// Description: does this transition move a drawable object?
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
bool Translator::MovesDrawable() const
{
    return true;
}

//==============================================================================
// Translator::operator=
//==============================================================================
// Description: assignment operator
//
// Parameters:	right - what we're being assigned
//
// Return:      reference to self
//
//==============================================================================
Translator& Translator::operator=( const Translator& right )
{
    if( this == &right )
    {
        return *this;
    }
    
    Chainable1::operator=( right );

    startX         = right.startX;
    startY         = right.startY;
    endX           = right.endX;
    endY           = right.endY;
    m_TimeInterval = right.m_TimeInterval;
    return *this;
}

//==============================================================================
// Translator::SetEndCoords
//==============================================================================
// Description: sets the ending position of the object
//
// Parameters:	x, y, the coords
//
// Return:      N/A.
//
//==============================================================================
void Translator::SetCoordsEnd( const int x, const int y )
{
    endX = x;
    endY = y;
}

//==============================================================================
// Translator::SetStartCoords
//==============================================================================
// Description: sets the starting position of the object
//
// Parameters:	x, y, the coords
//
// Return:      N/A.
//
//==============================================================================
void Translator::SetCoordsStart( const int x, const int y )
{
    startX = x;
    startY = y;
}

//==============================================================================
// Translator::SetEndOffscreenBottom
//==============================================================================
// Description: positions the transition to end the object offscreen to the bottom
//
// Parameters:	drawable - the drawable to line up offscreen with
//
// Return:      N/A.
//
//==============================================================================
void Translator::SetEndOffscreenBottom( const Scrooby::Drawable* drawable )
{
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    drawable->GetBoundingBox( xMin, yMin, xMax, yMax );
    #ifdef RAD_PS2
        SetCoordsEnd( 0, -20 -yMax );    
    #else
        SetCoordsEnd( 0, -yMax );    
    #endif
}

//==============================================================================
// Translator::SetEndOffscreenLeft
//==============================================================================
// Description: positions the transition to end the object offscreen to the left
//
// Parameters:	drawable - the drawable to line up offscreen with
//
// Return:      N/A.
//
//==============================================================================
void Translator::SetEndOffscreenLeft( const Scrooby::Drawable* drawable )
{
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    drawable->GetBoundingBox( xMin, yMin, xMax, yMax );
    int width = xMax - xMin;

    if( p3d::display->IsWidescreen() )
    {
        SetCoordsEnd( -xMin - width - WIDESCREEN_EXTRA_PIXELS, 0 );
    }
    else
    {
        SetCoordsEnd( -xMin - width, 0 );
    }
}
//==============================================================================
// Translator::SetEndOffscreenRight
//==============================================================================
// Description: positions the transition to end the object offscreen to the right
//
// Parameters:	drawable - the drawable to line up offscreen with
//
// Return:      N/A.
//
//==============================================================================
void Translator::SetEndOffscreenRight( const Scrooby::Drawable* drawable )
{
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    drawable->GetBoundingBox( xMin, yMin, xMax, yMax );
    int width = xMax - xMin;
    if( p3d::display->IsWidescreen() )
    {
        SetCoordsEnd( 640 - xMin + WIDESCREEN_EXTRA_PIXELS, 0 );    
    }
    else
    {
        SetCoordsEnd( 640 - xMin, 0 );    
    }
}

//==============================================================================
// Translator::SetEndOffscreenTop
//==============================================================================
// Description: positions the transition to end the object offscreen to the left
//
// Parameters:	drawable - the drawable to line up offscreen with
//
// Return:      N/A.
//
//==============================================================================
void Translator::SetEndOffscreenTop( const Scrooby::Drawable* drawable )
{
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    drawable->GetBoundingBox( xMin, yMin, xMax, yMax );
    #ifdef RAD_PS2
        SetCoordsEnd( 0, 480 + 20 - yMin );    
    #else
        SetCoordsEnd( 0, 480 - yMin );    
    #endif
}

//==============================================================================
// Translator::SetStartOffscreenBottom
//==============================================================================
// Description: positions the transition to start the object offscreen to the bottom
//
// Parameters:	drawable - the drawable to line up offscreen with
//
// Return:      N/A.
//
//==============================================================================
void Translator::SetStartOffscreenBottom ( const Scrooby::Drawable* drawable )
{
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    drawable->GetBoundingBox( xMin, yMin, xMax, yMax );
    
    #ifdef RAD_PS2
        SetCoordsStart( 0, -20 -yMax );    
    #else
        SetCoordsStart( 0, -yMax );    
    #endif

}

//==============================================================================
// Translator::SetTimeInterval
//==============================================================================
// Description: positions the transition to start the object offscreen to the left
//
// Parameters:	drawable - the drawable to line up offscreen with
//
// Return:      N/A.
//
//==============================================================================
void Translator::SetStartOffscreenLeft( const Scrooby::Drawable* drawable )
{
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    drawable->GetBoundingBox( xMin, yMin, xMax, yMax );
    int width = xMax - xMin;

    if( p3d::display->IsWidescreen() )
    {
        SetCoordsStart( -xMin - width - WIDESCREEN_EXTRA_PIXELS, 0 );    
    }
    else
    {
        SetCoordsStart( -xMin - width, 0 );    
    }
}
//==============================================================================
// Translator::SetStartOffscreenRight
//==============================================================================
// Description: positions the transition to start the object offscreen to the right
//
// Parameters:	drawable - the drawable to line up offscreen with
//
// Return:      N/A.
//
//==============================================================================
void Translator::SetStartOffscreenRight  ( const Scrooby::Drawable* drawable )
{
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    drawable->GetBoundingBox( xMin, yMin, xMax, yMax );
    int width = xMax - xMin;
    if( p3d::display->IsWidescreen() )
    {
        SetCoordsStart( 640 - xMin + WIDESCREEN_EXTRA_PIXELS, 0 );    
    }
    else
    {
        SetCoordsStart( 640 - xMin, 0 );    
    }
}

//==============================================================================
// Translator::SetStartOffscreenTop
//==============================================================================
// Description: positions the transition to start the object offscreen to the top
//
// Parameters:	drawable - the drawable to line up offscreen with
//
// Return:      N/A.
//
//==============================================================================
void Translator::SetStartOffscreenTop( const Scrooby::Drawable* drawable )
{
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    drawable->GetBoundingBox( xMin, yMin, xMax, yMax );
    SetCoordsStart( 0, 480 - yMin );    
    #ifdef RAD_PS2
        SetCoordsStart( 0, 480 + 20 - yMin );    
    #else
        SetCoordsStart( 0, 480 - yMin );    
    #endif

}

//==============================================================================
// Translator::Apply
//==============================================================================
// Description: applies the transition to the front end element being modified
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
void Translator::Update( const float deltaT )
{
    if( IsActive() )
    {
        Transition::Update( deltaT );
        float spillover = rmt::Max( m_ElapsedTime - m_TimeInterval, 0.0f );
        m_ElapsedTime = rmt::Min( m_ElapsedTime, m_TimeInterval );

        float percentDone = rmt::Min( m_ElapsedTime / m_TimeInterval, 1.0f );
        int x = static_cast< int >( ( endX - startX ) * percentDone + startX );
        int y = static_cast< int >( ( endY - startY ) * percentDone + startY );
        if( m_Drawable != NULL )
        {
            m_Drawable->Translate( x, y );
        }

        if( spillover > 0 )
        {
            ContinueChain();
        }
    }
}

//==============================================================================
// Translator::Watch
//==============================================================================
// Description: Adds this transition to the watcher
//
// Parameters:	nameSpace - the name of the group in the watcher
//
// Return:      N/A.
//
//==============================================================================
#ifdef DEBUGWATCH
void Translator::Watch( const char* nameSpace )
{
    char output[ 1024 ];
    sprintf( output, "%s\\%s", nameSpace, m_Name.GetText() );
    Parent1::Watch( nameSpace );
    Parent2::Watch( output    ); //HasTimeInterval doesn't know the name.
    radDbgWatchDelete( &startX );
    radDbgWatchDelete( &startY );
    radDbgWatchDelete( &endX   );
    radDbgWatchDelete( &endY   );
    radDbgWatchAddShort( &startX, "startX", output, NULL, NULL, -640, 640 * 2 );
    radDbgWatchAddShort( &startY, "startY", output, NULL, NULL, -640, 640 * 2 );
    radDbgWatchAddShort( &endX,   "endX",   output, NULL, NULL, -640, 640 * 2 );
    radDbgWatchAddShort( &endY,   "endY",   output, NULL, NULL, -640, 640 * 2 );
}
#endif

//==============================================================================
// UnderdampedTranslator::UnderdampedTranslator
//==============================================================================
// Description: constructor
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
UnderdampedTranslator::UnderdampedTranslator():
    Translator()
{
}

//==============================================================================
// UnderdampedTranslator::UnderdampedTranslator
//==============================================================================
// Description: constructor
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
UnderdampedTranslator::UnderdampedTranslator( const tName& name ):
    Translator( name )
{
}

//==============================================================================
// UnderdampedTranslator::MovesDrawable
//==============================================================================
// Description: does this transition move a drawable object?
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
bool UnderdampedTranslator::MovesDrawable() const
{
    return true;
}

//==============================================================================
// UnderdampedTranslator::SetFrequency
//==============================================================================
// Description: sets the frequency of oscillation of the underdamped translator
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
void UnderdampedTranslator::SetFrequency( const float frequency )
{
    m_Frequency = frequency;
}

//==============================================================================
// UnderdampedTranslator::Update
//==============================================================================
// Description: updates the object that we're translating
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
void UnderdampedTranslator::Update( const float deltaT )
{
    if( IsActive() )
    {
        Transition::Update( deltaT );
        float spillover = rmt::Max( m_ElapsedTime - m_TimeInterval, 0.0f );
        m_ElapsedTime = rmt::Min( m_ElapsedTime, m_TimeInterval );
        float decayRate = logf( 0.01f ) / m_TimeInterval;
        float exponentialPart = expf( decayRate * m_ElapsedTime );
        float cosinePart = cosf( 2.0f * rmt::PI * m_Frequency * m_ElapsedTime / 1000.0f );
        float percentage = 1.0f -  exponentialPart * cosinePart;
        int x = static_cast< int >( ( endX - startX ) * percentage + startX );
        int y = static_cast< int >( ( endY - startY ) * percentage + startY );
        m_Drawable->Translate( x, y );

        if( spillover > 0 )
        {
            ContinueChain();
        }
    }
}

}