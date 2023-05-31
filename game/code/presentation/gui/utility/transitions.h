//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   SpecialFX
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/30      TChu        Created for SRR2
//
//===========================================================================

#ifndef TRANSITIONS_H
#define TRANSITIONS_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <p3d/entity.hpp>
#include "events/eventenum.h"
#include "events/eventlistener.h"
#include <input/controller.h>
#include <gameflow/gameflow.h>
#include <presentation/gui/guiwindow.h>

//===========================================================================
// Forward References
//===========================================================================

namespace Scrooby
{
    class Drawable;
    class BoundedDrawable;
    class HasBoundingBox;
    class Sprite;
}

class tMultiController;
//===========================================================================
// Interface Definitions
//===========================================================================

namespace GuiSFX
{
#ifdef DEBUGWATCH
    #define WATCH( object, nameSpace ) object.Watch( nameSpace )
#else
    #define WATCH( object, nameSpace )
#endif

#ifdef DEBUGWATCH
    void ActivateCallback  ( void* userData );
        void DeactivateCallback( void* userData );
#endif
//==============================================================================
// Transition
//==============================================================================
class Transition
{
public:
    Transition();
    Transition( const tName& name );
    virtual void Activate();
    virtual void Deactivate();
    Scrooby::Drawable* GetDrawable();
    bool         IsActive()      const;
    bool         IsDone()        const;
    virtual bool MovesDrawable() const;
    Transition& operator=( const Transition& right );
    virtual void Reset();
    virtual void SetDrawable( Scrooby::Drawable* drawable );
    virtual void SetDrawable( Scrooby::Drawable& drawable );
    virtual void Update( const float deltaT );

    #ifdef DEBUGWATCH
        virtual void Watch( const char* nameSpace );
    #endif
    
protected:
#ifdef DEBUGWATCH
    tName                   m_Name;
    int                     m_Id;
    bool                    m_DummyVariableForActivate;
    bool                    m_DummyVariableForDeactivate;
#endif
    bool                    m_Active : 1;
    bool                    m_Done   : 1;
    Scrooby::Drawable*      m_Drawable;
    float                   m_ElapsedTime;
};

//==============================================================================
// Chainable
//==============================================================================
class Chainable : public Transition
{
public:
    Chainable();
    Chainable( const tName& name );
    virtual void ContinueChain()     = 0;
    virtual void DeactivateChain()   = 0;
    virtual bool IsChainDone() const = 0;
    virtual void ResetChain()        = 0;
protected:
};

//==============================================================================
// Chainable1 - these transitions can be chained together temporally
//==============================================================================
class Chainable1 : public Chainable
{
private:
    typedef Chainable Parent;
public:
    Chainable1();
    Chainable1( const tName& name );
    Chainable1& operator=( const Chainable1& right );
    virtual void ContinueChain();
    virtual void DeactivateChain();
    virtual bool IsChainDone() const;
    virtual void ResetChain();
    void SetNextTransition( Chainable* transition );
    void SetNextTransition( Chainable& transition );
protected:
    Chainable* m_NextTransition;
};

//==============================================================================
// Chainable2 - one transition that spawns 2 others at the same time
//==============================================================================
class Chainable2 : public Chainable
{
public:
    Chainable2();
    Chainable2( const tName& name );
    virtual void ContinueChain();
    virtual void DeactivateChain();
    virtual bool IsChainDone() const;
    virtual void ResetChain();
    void SetNextTransition( const unsigned int index, Chainable* transition );
    void SetNextTransition( const unsigned int index, Chainable& transition );

protected:
    Chainable* m_NextTransition[ 2 ];
};

//==============================================================================
// Chainable3 - one transition that spawns 3 others at the same time
//==============================================================================
//Ian - this should be a templatized class...
class Chainable3 : public Chainable
{
public:
    Chainable3();
    Chainable3( const tName& name );
    virtual void ContinueChain();
    virtual void DeactivateChain();
    virtual bool IsChainDone() const;
    virtual void ResetChain();
    void SetNextTransition( const unsigned int index, Chainable* transition );
    void SetNextTransition( const unsigned int index, Chainable& transition );

protected:
    Chainable* m_NextTransition[ 3 ];
};

//==============================================================================
// Junction2
//==============================================================================
class Junction2 : public Chainable2
{
public:
    void Activate();
protected:
};

//==============================================================================
// Junction3
//==============================================================================
//IAN - this should be a templatized class too
class Junction3 : public Chainable3
{
public:
    void Activate();
protected:
};

//==============================================================================
// Dummy - this "transition" does nothing. it's a placeholder
//==============================================================================
class Dummy : public Chainable1
{
private:
    typedef Chainable1 Parent;
public:
    Dummy();
    Dummy( const tName& name );
    void Activate();
};

//==============================================================================
// GotoScreen
//==============================================================================
class GotoScreen : public Chainable1
{
private:
    typedef Chainable1 Parent;
public:
    GotoScreen();
    GotoScreen( const tName& name );
    void Activate();
    void SetEventData( void* eventData );
    void SetParam1( const unsigned int param1 );
    void SetParam2( const unsigned int param2 );
    void SetScreen( CGuiWindow::eGuiWindowID screen );
    void SetWindowOptions( const unsigned int windowOptions );

protected:
    CGuiWindow::eGuiWindowID   mScreen;
    unsigned int               mParam1;
    unsigned int               mParam2;
    void*                      mEventData;
    unsigned int               mWindowOptions;
};


//==============================================================================
// Hide
//==============================================================================
class Hide : public Chainable1
{
public:
    Hide();
    Hide( const tName& name );
    void Activate();

protected:
};

//==============================================================================
// InputStateChange
//==============================================================================
class InputStateChange : public Chainable1
{
private:
    typedef Chainable1 Parent;
public:
    InputStateChange();
    InputStateChange( const tName& name );
    void Activate();
    void SetState( const Input::ActiveState state );
protected:
private:
    Input::ActiveState mState;
};

//==============================================================================
// RecieveEvent
//==============================================================================
class RecieveEvent : 
    public Chainable1,
    public EventListener
{
public:
    RecieveEvent();
    RecieveEvent( const tName& name );
    void         Activate();
    virtual void HandleEvent( EventEnum id, void* pEventData );
    void         SetEvent( const EventEnum event );
protected:
private:
    EventEnum mEvent;
};

//==============================================================================
// SendEvent
//==============================================================================
class SendEvent : public Chainable1
{
public:
    SendEvent();
    SendEvent( const tName& name );
    void Activate();
    void SetEvent( EventEnum event );
    void SetEventData( void* eventData );

protected:
    EventEnum   mEvent;
    void*       mEventData;
};

//==============================================================================
// SwitchContext
//==============================================================================
class SwitchContext : public Chainable1
{
private:
    typedef Chainable1 Parent;
public:
    SwitchContext();
    SwitchContext( const tName& name );
    void Activate();
    void SetContext( ContextEnum event );

protected:
    ContextEnum mContext;
};

//==============================================================================
// HasMulticontroller
//==============================================================================
class HasMulticontroller
{
public:
    void SetMultiController( tMultiController* multicontroller );
protected:
    void ResetMultiControllerFrames();

    tMultiController*      m_MultiController;
    float                  m_NumFrames;
};

//==============================================================================
// HasMulticontroller
//==============================================================================
class HasTimeInterval
{
public:
    HasTimeInterval();
    void SetTimeInterval( const float interval );
    #ifdef DEBUGWATCH
        virtual void Watch( const char* nameSpace );
    #endif

protected:
    float m_TimeInterval;
};

//==============================================================================
// ColorChange
//==============================================================================
class ColorChange : 
    public Chainable1,
    public HasTimeInterval
{
private:
    typedef Chainable1      Parent1;
    typedef HasTimeInterval Parent2;
public:
    ColorChange();
    ColorChange( const tName& name );
    void SetStartColour( const tColour c );
    void SetEndColour( const tColour c );
    void Update( const float deltaT );
    #ifdef DEBUGWATCH
        virtual void Watch( const char* nameSpace );
    #endif

protected:
    tColour m_StartColor;
    tColour m_EndColor;
};

//==============================================================================
// IrisWipeClose
//==============================================================================
class IrisWipeOpen 
    : public Chainable1,
      public HasMulticontroller

{
    //this class doesn't even need a drawable really
public:
    IrisWipeOpen();
    IrisWipeOpen( const tName& name );
    void Activate();
    void Update( const float deltaT );
protected:
};

//==============================================================================
// IrisWipeOpen
//==============================================================================
class IrisWipeClose
    : public Chainable1,
      public HasMulticontroller
{
    //this class doesn't even need a drawable really
public:
    IrisWipeClose();
    IrisWipeClose( const tName& name );
    void Activate();
    void Deactivate();
    void Update( const float deltaT );
protected:
};

//==============================================================================
// Show
//==============================================================================
class Show : public Chainable1
{
public:
    Show();
    Show( const tName& name );
    void Activate();

protected:
};

//=============================================================================
// Pause
//=============================================================================
class Pause : 
    public Chainable1,
    public HasTimeInterval
{
private:
    typedef Chainable1      Parent1;
    typedef HasTimeInterval Parent2;

public:
    Pause();
    Pause( const tName& name );
    void Update( const float deltaT );
    #ifdef DEBUGWATCH
        virtual void Watch( const char* nameSpace );
    #endif
protected:
};

//=============================================================================
// PauseInFrames
//=============================================================================
class PauseInFrames : 
    public Chainable1
{
private:
    typedef Chainable1 Parent;
public:
    PauseInFrames();
    PauseInFrames( const tName& name );
    void Reset();
    void SetNumberOfFrames( const unsigned int i );
    void Update( const float deltaT );
protected:
    unsigned int m_FramesToPause;
    unsigned int m_FramesElapsed;
};

//==============================================================================
// PauseGame
//==============================================================================
class PauseGame :
    public Chainable1
{
    void Activate();
};

//==============================================================================
// ResumeGame
//==============================================================================
class ResumeGame :
    public Chainable1
{
public:
    ResumeGame();
    ResumeGame( const tName& name );
    void Activate();
};

//==============================================================================
// ImageCycler - causes images to cycle forever
//==============================================================================
class ImageCycler : public Transition
{
public:
    void SetDrawable( Scrooby::Drawable* drawable );
    void SetDrawable( Scrooby::Drawable& drawable );
    void SetPeriod( const float period );
    void Update( const float deltaT );
protected:
    float m_Period;
    Scrooby::Sprite* m_Sprite;      //this is a waste of memory
};

//==============================================================================
// Pulse - pulses the scale of the image about its center
//==============================================================================
class PulseScale : public Transition
{
private:
    typedef Transition Parent;
public:
    PulseScale();
    PulseScale( const tName& name );
    virtual bool MovesDrawable() const;
    void SetAmplitude( const float amplitude );
    void SetDrawable( Scrooby::Drawable* drawable );
    void SetDrawable( Scrooby::Drawable& drawable );
    void SetFrequency( const float frequency );
    void Update( const float deltaT );

    #ifdef DEBUGWATCH
        virtual void Watch( const char* nameSpace );
    #endif
    
protected:
    Scrooby::HasBoundingBox* m_BoundedDrawable;
    float m_Amplitude;
    float m_Frequency;
};

//==============================================================================
// Spin - causes the object to spin about its center forever
//==============================================================================
class Spin : public Transition
{
public:
    Spin();
    virtual bool MovesDrawable() const;
    void SetDrawable( Scrooby::Drawable* drawable );
    void SetPeriod( const float period );
    void Update( const float deltaT );
protected:
    float m_Period;
    Scrooby::BoundedDrawable* m_BoundedDrawable;
};

//==============================================================================
// Translator
//==============================================================================
class Translator : 
    public Chainable1,
    public HasTimeInterval
{
private:
    typedef Chainable       Parent1;
    typedef HasTimeInterval Parent2;

public:
    Translator();
    Translator( const tName& name );
    Translator( const Translator& right );
    ~Translator();
    void MateCoordsEnd( const Scrooby::HasBoundingBox* drawable );
    void MateCoordsStart( const Scrooby::HasBoundingBox* drawable );
    virtual bool MovesDrawable() const;
    Translator& operator=( const Translator& right );
    void SetCoordsEnd  ( const int x, const int y );
    void SetCoordsStart( const int x, const int y );
    void SetEndOffscreenBottom   ( const Scrooby::Drawable* drawable );
    void SetEndOffscreenLeft     ( const Scrooby::Drawable* drawable );
    void SetEndOffscreenRight    ( const Scrooby::Drawable* drawable );
    void SetEndOffscreenTop      ( const Scrooby::Drawable* drawable );
    void SetStartOffscreenBottom ( const Scrooby::Drawable* drawable );
    void SetStartOffscreenLeft   ( const Scrooby::Drawable* drawable );
    void SetStartOffscreenRight  ( const Scrooby::Drawable* drawable );
    void SetStartOffscreenTop    ( const Scrooby::Drawable* drawable );
    void Update( const float deltaT );
    #ifdef DEBUGWATCH
        virtual void Watch( const char* nameSpace );
    #endif

protected:
    short int startX;
    short int startY;
    short int endX;
    short int endY;
};

//==============================================================================
// Translator - with a springy type of effect. 
//==============================================================================
class UnderdampedTranslator : public Translator
{
public:
    UnderdampedTranslator();
    UnderdampedTranslator( const tName& name );
    virtual bool MovesDrawable() const;
    void SetFrequency( const float frequency );
    void Update( const float deltaT );
protected:
    float m_Frequency;
};

} // GuiSFX namespace

#endif // TRANSITIONS_H
