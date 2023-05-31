#ifndef _STATEPROP_HPP_
#define _STATEPROP_HPP_

#include <radmath/radmath.hpp>

#include <p3d/anim/compositedrawable.hpp>

//=============================================================================
// Forward Class/Struct Declarations
//=============================================================================

class tFrameController;
class tAnimatedObject;
class CStatePropData;
class CStateProp;
class StatePropDSGProcAnimator;
class tPose;

//=============================================================================
// Class Declarations
// PropListener
//=============================================================================

class CStatePropListener
{
public:
	virtual void RecieveEvent( int callback , CStateProp* stateProp ) = 0;
};

//=============================================================================
// Definitions
//=============================================================================
#define MAX_LISTENERS 10

const int STATEPROP_CHANGE_STATE_EVENT = -1;

//=============================================================================
// Class Declarations
// CStateProp
//=============================================================================
class CStateProp : public tEntity
{
public:
    
    //Static function to create a new CStateProp instance
    static CStateProp* CreateStateProp( CStatePropData* statePropData , unsigned int state = 0, tPose* pose = NULL );

    CStateProp( tAnimatedObject* animatedObject , CStatePropData* statePropData , unsigned int state = 0 );
    ~CStateProp();

    //Per frame update
    void Update( float dt );

    //call before render
    void UpdateFrameControllersForRender();

    unsigned int GetState();
    void SetState( unsigned int state );
    void NextState();
    void PrevState();

    // MikeR -  modified OnEvent to return boolean indicating whether or not the
    // event was successfully handled by the stateprop
	bool OnEvent( unsigned int eventID );

    void AddStatePropListener( CStatePropListener* statePropListener );
	void RemoveStatePropListener( CStatePropListener* statePropListener );

    tDrawable* GetDrawable();

    void Display( StatePropDSGProcAnimator* ProcAnimator = 0 );
    bool UsingFastDisplay()const { return m_FastDisplayDrawable != NULL; }
    CStatePropData* GetStatePropData()const { return m_StatePropData; }

private:

    //accessor helpers
    unsigned int GetNumberOfFrameControllers();
    tFrameController* GetFrameControllerByIndex( unsigned int i );
    unsigned int GetNumberOfDrawableElements();
    tCompositeDrawable::DrawableElement* GetDrawableElement( unsigned int i );
    
	//Private members
    CStatePropData* m_StatePropData;
	tAnimatedObject* m_AnimatedObject;
	tFrameController* m_BaseFrameController; 
    tDrawable* m_FastDisplayDrawable;
    unsigned int m_CurrentState;
    
    unsigned int m_NumStatePropListeners;
    CStatePropListener* m_StatePropListener[MAX_LISTENERS];
};

#endif //_STATEPROP_HPP_