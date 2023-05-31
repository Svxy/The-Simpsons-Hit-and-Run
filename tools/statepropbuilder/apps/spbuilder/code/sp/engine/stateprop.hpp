#ifndef _STATEPROP_HPP_
#define _STATEPROP_HPP_

#include "radmath/radmath.hpp"

//=============================================================================
// Forward Class/Struct Declarations
//=============================================================================

class tFrameController;
class tAnimatedObject;
class CStatePropData;
class CStateProp;
class tCompositeDrawable;


//=============================================================================
// Class Declarations
// PropListener
//=============================================================================

class CStatePropListener : public tRefCounted
{
public:
	virtual void RecieveEvent( int event , CStateProp* stateProp ) = 0;
};

//=============================================================================
// Definitions
//=============================================================================


//=============================================================================
// Class Declarations
// CStateProp
//=============================================================================
class CStateProp : public tEntity
{
public:
    
    static CStateProp* CreateCStateProp( CStatePropData* statePropData , unsigned int state );

    CStateProp( tAnimatedObject* animatedObject , CStatePropData* statePropData , unsigned int state = 0 );
    ~CStateProp();

    //Per frame update
    void Update( float dt );

    //Render
    void Render();

    //call before render
    void UpdateFrameControllersForRender();

    unsigned int GetState();
    void SetState( unsigned int state );
    void NextState();
    void PrevState();

	void OnEvent( unsigned int eventID );

    void SetTransformationMatrix( const rmt::Matrix &tm);
    const rmt::Matrix& GetTransformationMatrix() const;
    
    void SetPosition( const rmt::Vector& pos );
    const rmt::Vector& GetPosition() const;

    //Get State Prop Data
	CStatePropData* GetCStatePropData();

    // Update for new data
	void UpdateOnDataEdit();

    //accessors
	float GetBaseFrameControllerFrame();
    unsigned int GetNumberOfFrameControllers();
    tFrameController* GetFrameControllerByIndex( unsigned int i );
    unsigned int GetNumberOfDrawableElements();
    tCompositeDrawable::DrawableElement* GetDrawableElement( unsigned int i );
    const char* GetDrawableName( unsigned int i );
    const char* GetPropName();

    //Export the SmartPropData
    void ExportChunk( const char* filename );


private:
   
	// Orientation and location
	rmt::Matrix     m_Transform;
	
	//Private members
    CStatePropData* m_StatePropData;
	tAnimatedObject* m_AnimatedObject;
	tFrameController* m_BaseFrameController;    
    unsigned int m_CurrentState;
    
    CStatePropListener* m_StatePropListener;
};


inline void CStateProp::SetPosition( const rmt::Vector& pos )
{
    m_Transform.FillTranslate( pos );
}
inline const rmt::Vector& CStateProp::GetPosition() const 
{
    return const_cast< RadicalMathLibrary::Matrix & >( m_Transform ).Row(3);
}

inline void CStateProp::SetTransformationMatrix(const rmt::Matrix& tmx)
{
    m_Transform = tmx;
}
inline const rmt::Matrix& CStateProp::GetTransformationMatrix() const
{
    return m_Transform;
}

#endif //_STATEPROP_HPP_