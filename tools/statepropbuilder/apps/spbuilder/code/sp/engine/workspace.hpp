#ifndef _WORKSPACE_HPP_
#define _WORKSPACE_HPP_

#include <radmath/radmath.hpp>
#include <simcollision/collisionmanager.hpp>

#include "aicollisionsolveragent.hpp"

class CStateProp;
class tDrawable;

//=============================================================================
// Class Declarations
// ViewerIterator
//=============================================================================
class ViewerIterator
{
public:
    virtual ~ViewerIterator() {}
    virtual tEntity* Current(void) = 0;
    virtual tEntity* Next(bool loop = false) = 0;
    virtual tEntity* Prev(void) = 0;
    virtual tEntity* First(void) = 0;
};


//=============================================================================
// Class Declarations
// Workspace
//=============================================================================
class Workspace : public tRefCounted
{
public:
    Workspace();
    ~Workspace();
	
	void Init();
	void ResetAll( bool emptyInventory = true );
	void Advance(float dt_ms);
    void Display(tContext* context);
    
	int Load( const char* name );
	int Pause(bool b);

	void LoadFloor( const char* p3dfilename );

private:
    
	void ResetProp();
	CStateProp* m_CStateProp;
    
    AICollisionSolverAgent *m_CollisionSolver;
    sim::CollisionManager* m_CollisionManager;

    ViewerIterator* m_DrawableIterator;
	ViewerIterator* m_ObjectFactoryIterator;
	ViewerIterator* m_CStatePropDataIterator;

	tDrawable* m_DrawableFloor;
	sim::SimState* m_SimStateFloor;

	char m_Path[256];
};



#endif
