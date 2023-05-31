/******************************************************************************
	File:		Mouse.h
	Desc:		Interface for the Mouse class.
				- Responsible for managing relative axis info
                  for the mouse and other mouse related stuff.

	Date:		May 16, 2003
	History:
 *****************************************************************************/
#ifndef  MOUSE_H
#define  MOUSE_H

#include <input/RealController.h>

struct MouseCoord
{
	float x, y;
};

// We need the eMouseButtons enum to provide a sequential equivalent to DIMOFS_X... 
// so that we can use them to access our arrays.
enum eMouseButton
{
    MOUSE_X,
    MOUSE_Y,
    MOUSE_Z,
    MOUSE_BUTTON0,
    MOUSE_BUTTON1,
    MOUSE_BUTTON2,
    MOUSE_BUTTON3,
    MOUSE_BUTTON4,
    MOUSE_BUTTON5,
    MOUSE_BUTTON6,
    MOUSE_BUTTON7,
    NUM_MOUSE_BUTTONS
};

class Mouse : public RealController
{
public:
	Mouse();
	virtual ~Mouse();

	float XPos() const { return m_relPosition.x; }
	float YPos() const { return m_relPosition.y; }
    void setSensitivityX( float fSensitivityX ) { m_fSensitivityX = fSensitivityX; }
    void setSensitivityY( float fSensitivityY ) { m_fSensitivityY = fSensitivityY; }
    float getSensitivityX() const { return m_fSensitivityX; }
    float getSensitivityY() const { return m_fSensitivityY; }
	
    virtual bool IsMouseAxis( int dxKey ) const;

    // Returns true if this is a valid input for the controller.
    virtual bool IsValidInput( int dxKey ) const;

	float CalculateMouseAxis( int mouseAxis, float value );

    // Sets up a mapping from a dxkey/direction to a virtual button
    virtual bool SetMap( int dxKey, eDirectionType dir, int virtualButton );
    // Retrieves the virtual button of the given type mapped to a dxKey, direction
    virtual int  GetMap( int dxKey, eDirectionType dir, eMapType map ) const;
    // Clears the specified mapping so it no longer exists.
    virtual void ClearMap( int dxKey, eDirectionType dir, int virtualButton );
    // Clears all the cached mappings.
    virtual void ClearMappedButtons();


private:
    virtual void MapInputToDICode();

    eMouseButton GetButtonEnum( int dxKey ) const;

private:
	MouseCoord	m_relPosition;
    MouseCoord  m_absPosition;
    float       m_fSensitivityX;
    float       m_fSensitivityY;
    int         m_ButtonMap[ NUM_MAPTYPES ][ NUM_MOUSE_BUTTONS ][ NUM_DIRTYPES ];
};
#endif
