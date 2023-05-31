#include <input/MouseCursor.h>

// THIS CURSOR STUFF IS JUST FOR TESTING!.
static void StreamLine(pddiPrimStream* pstream, rmt::Vector a, rmt::Vector b, tColour colora, tColour colorb)
{
    pddiVector pa;
    pddiVector pb;

    pa.x = a.x;
    pa.y = a.y;
    pa.z = a.z;

    pb.x = b.x;
    pb.y = b.y;
    pb.z = b.z;

    pstream->Vertex(&pa, colora);
    pstream->Vertex(&pb, colorb);
}
static void StreamLine(pddiPrimStream* pstream, rmt::Vector a, rmt::Vector b, tColour color)
{
    StreamLine(pstream, a, b, color, color);
}

static pddiShader* simpleShader = NULL;

static void DrawSquare(rmt::Vector v1, rmt::Vector v2, rmt::Vector v3, rmt::Vector v4, 
                       tColour color = tColour(255, 255, 255),
                       pddiPrimType primType = PDDI_PRIM_LINES)
{
    if(simpleShader == NULL)
    {
        simpleShader = p3d::device->NewShader("simple");
        simpleShader->AddRef();
        simpleShader->SetInt(PDDI_SP_BLENDMODE, PDDI_BLEND_ALPHA);
    }

    pddiPrimStream* pStream;
    pStream = p3d::pddi->BeginPrims(simpleShader, primType, PDDI_V_C, 8);
    if(pStream)
    {
        StreamLine(pStream, v1, v2, color);
        StreamLine(pStream, v2, v4, color);
        StreamLine(pStream, v4, v3, color);
        StreamLine(pStream, v3, v1, color);
    }
    p3d::pddi->EndPrims(pStream);
}

/*
* DrawSquare:
*   the square is defined as:
*
*  v1 ---------+
*   |          |
*   |          |
*   |          |
*   |          |
*   +--------- v2
*
*/
static void DrawSquare(rmt::Vector v1, rmt::Vector v2,
                       tColour color = tColour(255, 255, 255),
                       pddiPrimType primType = PDDI_PRIM_LINES)
{
    DrawSquare(
        rmt::Vector(v1.x, v1.y, 0),
        rmt::Vector(v2.x, v1.y, 0),
        rmt::Vector(v1.x, v2.y, 0),
        rmt::Vector(v2.x, v2.y, 0),
        color, primType);
}

/******************************************************************************
    Some global constants/defines for this file.
 *****************************************************************************/


/******************************************************************************
    Initialization of static member variables.
 *****************************************************************************/

/******************************************************************************
	Construction/Destruction
 *****************************************************************************/

MouseCursor::MouseCursor()
:  m_bVisible(false),
   m_position(),
   m_pCursorIcon(NULL),
   m_width(10),
   m_height(10)
{
}

MouseCursor::~MouseCursor()
{
    tRefCounted::Release( m_pCursorIcon );
}

void MouseCursor::Set( tDrawable* pCursorIcon )
{
    if( !m_pCursorIcon )
        tRefCounted::Assign(m_pCursorIcon, pCursorIcon);
}

void MouseCursor::Render()
{
    if( !m_bVisible ) return;

    rmt::Vector v1(-0.1f, -0.1f, 0.0f);
    rmt::Vector v2(-0.1f, 0.1f, 0.0f);
    rmt::Vector v3(0.1f, 0.1f, 0.0f);
    rmt::Vector v4(0.1f, -0.1f, 0.0f);

    pddiProjectionMode pMode = p3d::pddi->GetProjectionMode();
    p3d::pddi->SetProjectionMode(PDDI_PROJECTION_ORTHOGRAPHIC);

    p3d::stack->Push();
    p3d::stack->LoadIdentity();

    // Offsets are hardcoded right now for the type of cursor that it is.
    static float XOFFSET = 0.020f;
    static float YOFFSET = 0.035f;
    p3d::stack->Translate( m_position.x-XOFFSET, m_position.y-YOFFSET, m_position.z );

    if (m_pCursorIcon)
    {
        static float scaleFactor = 0.5f;
        p3d::stack->Scale(scaleFactor, scaleFactor, 1.0f);
        //p3d::stack->Scale(0.0007f, 0.0007f, 0.0007f);
        m_pCursorIcon->Display();
    }
    else
    {
        float scale = 0.2f;
        p3d::stack->Scale(scale, scale, scale);
        pddiColour c = pddiColour(255, 0, 0); // 0-255
        //This should be replaced with a DrawCursor function.
        DrawSquare(v2, v3, v1, v4, c);
    }

    p3d::pddi->Clear(PDDI_BUFFER_DEPTH | PDDI_BUFFER_STENCIL);

    p3d::stack->Pop();

    //Restore things we've messed with
    p3d::pddi->SetProjectionMode(pMode);
}