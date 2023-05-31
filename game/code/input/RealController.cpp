/******************************************************************************
	File:		RealController.cpp
	Desc:		Defines the RealController class.
	Author:		Neil Haran
	Date:		May 16, 2003
	History:
 *****************************************************************************/
#include <input/RealController.h>
#include <input/usercontrollerWin32.h>
#include <input/inputmanager.h>

/******************************************************************************
	Construction/Destruction
 *****************************************************************************/
RealController::RealController( eControllerType type )
:   m_radController(NULL),
    m_eControllerType(type),
    m_bConnected(false),
    m_InputToDICode(NULL),
    m_numInputPoints(0)
{
}

RealController::~RealController()
{
    if( m_InputToDICode != NULL )
    {
        delete [] m_InputToDICode;
    }
}

//--------------------------------------------------------
// Init
//--------------------------------------------------------
void RealController::Init( IRadController* pRadController )
{
//	rAssert(!m_radController);
	if( !m_radController )
    {
		m_radController = pRadController;

        MapInputToDICode();
    }
}

void RealController::Release()
{
	if( m_radController )
		m_radController = NULL;
}

//==============================================================================
// RealController::IsInputAxis
//==============================================================================
//
// Description: Identifies any keys that are input axes. (can go in two directions)
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//
// Return:      true if input is an axis
//
//==============================================================================

bool RealController::IsInputAxis( int dxKey ) const
{
    return false;
}

//==============================================================================
// RealController::IsMouseAxis
//==============================================================================
//
// Description: Identifies any keys that are mouse axes. (special to mouse)
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//
// Return:      true if input is a mouse axis
//
//==============================================================================

bool RealController::IsMouseAxis( int dxKey ) const
{
    return false;
}

//==============================================================================
// RealController::IsPovHat
//==============================================================================
//
// Description: Identifies any keys that are pov hats (go in 4 directions)
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//
// Return:      true if input is a pov hat
//
//==============================================================================

bool RealController::IsPovHat( int dxKey ) const
{
    return false;
}

//==============================================================================
// RealController::IsBannedInput
//==============================================================================
//
// Description: Returns true if the key is banned for mapping.
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//
// Return:      true if key is banned for mapping
//
//==============================================================================

bool RealController::IsBannedInput( int dxKey ) const
{
    return false;
}

//==============================================================================
// RealController::GetInputName
//==============================================================================
//
// Description: Returns the name of the input
//
// Parameters:	dxkey - the directinput index identifying the physical input.
//
// Return:      name
//
//==============================================================================

const char* RealController::GetInputName( int dxKey ) const
{
    if( IsValidInput( dxKey ) )
    {
        for( int i = 0; i < m_numInputPoints; i++ )
        {
            if( GetDICode( i ) == dxKey )
            {
                IRadControllerInputPoint* pInputPoint = m_radController->GetInputPointByIndex( i );
                rAssert( pInputPoint != NULL );

                return pInputPoint->GetName();
            }
        }
    }

    rAssert( false );
    return NULL;
}

void RealController::AddInputPoints( IRadControllerInputPoint* pInputPoint )
{
	rAssert(pInputPoint);

    // Check if the input point is already in the list.
    for( INPUTPOINTITER iter = m_inputPointList.begin(); iter != m_inputPointList.end(); iter++ )
    {
        if( *iter == pInputPoint )
        {
            return;
        }
    }
    
    // Add the point.
	m_inputPointList.push_back( pInputPoint );	
}

void RealController::ReleaseInputPoints( UserController* parent )
{
	INPUTPOINTITER iter;

	for( iter = m_inputPointList.begin(); iter != m_inputPointList.end(); iter++ )
	{
		IRadControllerInputPoint* pInputPoint = *iter;
        if( pInputPoint )
        {
			pInputPoint->UnRegisterControllerInputPointCallback( static_cast< IRadControllerInputPointCallback* >( parent ) );
        }
	}
	// after unregistering, we don't need this point anymore.
	m_inputPointList.clear();
}
	
//==============================================================================
// RealController::GetDICode
//==============================================================================
//
// Description: Returns the direct input keycode associated with an input point,
//              or Input::INVALID_CONTROLLERID if there is no controller or input code.
//
// Parameters:	n/a
//
// Return:      n/a
//
//==============================================================================

int RealController::GetDICode( int inputpoint ) const
{
    rAssert( inputpoint >= 0 && inputpoint < m_numInputPoints );

    if( m_InputToDICode != NULL && inputpoint >= 0 && inputpoint < m_numInputPoints )
    {
        return m_InputToDICode[ inputpoint ];
    }
    else
    {
        return Input::INVALID_CONTROLLERID;
    }
}
