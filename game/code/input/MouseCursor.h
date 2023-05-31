/******************************************************************************
	File:		MouseCursor.h
	Desc:		Interface for the Mouse cursor.
				- Responsible for drawing the 3D/2D cursor.

	Date:		May 16, 2003
	History:
 *****************************************************************************/
#ifndef  MOUSECURSOR_H
#define  MOUSECURSOR_H

class MouseCursor
{
public:
    MouseCursor();
    ~MouseCursor();

    void SetPos( float x, float y, float z = 10.0f ) 
    { 
        m_position.x = x; 
        m_position.y = y; 
        m_position.z = z;
    }

    void SetVisible( bool bVisible ) 
    { 
        m_bVisible = bVisible; 
    }

    void SetSize( int width, int height )
    {
        m_width = width;
        m_height = height;
    }

    float XPos() const { return m_position.x; }
    float YPos() const { return m_position.y; }
    float ZPos() const { return m_position.z; }

    bool IsVisible() const { return m_bVisible; }

    int Width()  const { return m_width; }
    int Height() const { return m_height; }
    
    void Set( tDrawable* pCursorIcon );
    void Render();

private:
    tDrawable*      m_pCursorIcon;
    rmt::Vector     m_position;
    bool            m_bVisible;
    int             m_width;
    int             m_height;
};


#endif
