//==============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        debuginfo.cpp
//
// Description: In-game visual debug output
//
// History:     2002/07/02 + Migrated from Mirth -- Darwin Chau
//
//==============================================================================

//========================================
// System Includes
//========================================
#include <string.h>
// FTech
#include <raddebug.hpp>
#include <raddebugwatch.hpp>
// Pure3D
#include <p3d/font.hpp>
#include <p3d/matrixstack.hpp>
#include <p3d/texturefont.hpp>
#include <p3d/unicode.hpp>
#include <p3d/utility.hpp>
#include <pddi/pddi.hpp>

//========================================
// Project Includes
//========================================
#include <debug/debuginfo.h>
#include <debug/section.h>

#include <memory/srrmemory.h>

#ifdef DEBUGINFO_ENABLED

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//The Adlib font.  <sigh>
static unsigned char gFont[] = 
#include <font/defaultfont.h>

// Static pointer to instance of singleton.
DebugInfo* DebugInfo::_Instance = NULL;

static void Next()
{
    DebugInfo::GetInstance()->OnNext();
}

static void Switch()
{
    DebugInfo::GetInstance()->OnSwitch();
}

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************


//==============================================================================
// DebugInfo::DebugInfo
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
DebugInfo::DebugInfo() :
    _pDebugFont(NULL),
    _pTypeFace(NULL),
    _NumSection(0),
    _StackSize(0),
    _CurrentSection(0),
    _DebugMenuTime(0.0f),
    _isRenderEnabled(false),
    _isCreationSectionOpen(false),
    _mode(OFF),
    _shader(NULL)
{
    for(int i=0; i<MaxSections; i++)
    {
    	_ppSections[i] = NULL;
    }

    radDbgWatchAddFunction( "Next Section", (RADDEBUGWATCH_CALLBACK)Next, 0, "DebugInfo" );
    radDbgWatchAddFunction( "Toggle Display", (RADDEBUGWATCH_CALLBACK)Switch, 0, "DebugInfo" );
}


//==============================================================================
// DebugInfo::~DebugInfo
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
DebugInfo::~DebugInfo()
{
    if(_pDebugFont)
    {
        _pDebugFont->Release ();
    }

    if(_shader)
    	_shader->Release();

    int i;
    for(i=0; i<MaxSections; i++)
    {
    	if(_ppSections[i])
    		delete (GMA_DEBUG, _ppSections[i]);
    }
}


//==============================================================================
// DebugInfo::CreateInstance
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "DebugInfo" );
    HeapMgr()->PushHeap( GMA_DEBUG );

    _Instance = new DebugInfo;

    HeapMgr()->PopHeap ( GMA_DEBUG );
MEMTRACK_POP_GROUP( "DebugInfo" );
}


//==============================================================================
// DebugInfo::CreateNewSection
//==============================================================================
//
// Description: Creates a new section
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::CreateNewSection( const char* section )
{
    HeapMgr()->PushHeap( GMA_DEBUG );
    //Lazy section allocation
    if(_ppSections[_NumSection])
    {
    	_ppSections[_NumSection]->Reset( section );
    }
    else
    {
        if( _pDebugFont == NULL )
        {
            //
            // find the font
            //
            _pDebugFont = p3d::find<tTextureFont>("adlibn_20");

            if( _pDebugFont == NULL )
            {
                // Convert memory buffer into a texturefont.
                //
                p3d::load(gFont, DEFAULTFONT_SIZE, GMA_DEBUG);
                _pDebugFont = p3d::find<tTextureFont>("adlibn_20");
                rAssertMsg(_pDebugFont, ("ERROR - debug font not found."));
            }

            _pDebugFont->AddRef();
        }

        _ppSections[_NumSection] = new Section( _pDebugFont, section );
    }
    //_pStack[_StackSize++] = _NumSection;
    _NumSection++;
    HeapMgr()->PopHeap( GMA_DEBUG );
}

//==============================================================================
// DebugInfo::DestroyInstance
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::DestroyInstance()
{
    delete(GMA_DEBUG, _Instance);
}


//==============================================================================
// DebugInfo::GetInstance
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
DebugInfo* DebugInfo::GetInstance()
{
    if(!_Instance)
    {
        CreateInstance();
    }

    return _Instance;
}

//==============================================================================
// DebugInfo::InitializeStaticVariables
//==============================================================================
//
// Description: Initializes all the sections that we're ever going to use
//
// Parameters:  none
//
// Return:      none
//
//==============================================================================
void DebugInfo::InitializeStaticVariables()
{
    GetInstance()->CreateNewSection( "Vehicle Terrain Type" );
    GetInstance()->CreateNewSection( "Vehicle Shit" );
}

//==============================================================================
// DebugInfo::OnSwitch
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::OnSwitch() 
{ 
    _mode = (Mode) ((_mode + 1) % MODE_MAX);

    if(_mode == OFF)
    {
        _isRenderEnabled = false;
        _isBackgroundEnabled = false;
    }
    else if(_mode == BACKGROUND)
    {
        _isRenderEnabled = true;
        _isBackgroundEnabled = true;
    }
    else if(_mode == NOBACKGROUND)
    {
        _isRenderEnabled = true;
        _isBackgroundEnabled = false;
    }
    
}


//==============================================================================
// DebugInfo::Push
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
bool DebugInfo::Push(char* szSection)
{
MEMTRACK_PUSH_GROUP( "DebugInfo" );
    HeapMgr()->PushHeap (GMA_DEBUG);

    rAssert(szSection);
    rAssert(_StackSize<MaxStackSize);

    int i;
    bool bFound = false;

    //Check for exsisting section
    for(i=0;i<_NumSection;i++)
    {
    	//look for the global section (NULL) or a matching name
    	const char* szName = _ppSections[i]->GetName();
    	if(0==strcmp(szSection,szName))
    	{
    		_pStack[_StackSize++] = i;
    		bFound = true;
    		break;
    	}
    }
    //Create a new section
    if(!bFound)
    {
        CreateNewSection( szSection );
    }
MEMTRACK_POP_GROUP( "DebugInfo" );
    HeapMgr()->PopHeap (GMA_DEBUG);
    
    //Return true if this is the current section
    return (bFound && i==_CurrentSection);
}


//==============================================================================
// DebugInfo::GetCurrentSection
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
const char* DebugInfo::GetCurrentSection()
{
    Section *pSect = _ppSections[_CurrentSection];
    if (pSect && _isRenderEnabled)
        return (pSect->GetName());
    else
    	return (NULL);
}


//==============================================================================
// DebugInfo::Pop
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::Pop()
{
    rAssert( _NumSection > 0 );
    rAssert( _StackSize >= 0 );
    --_StackSize;
}



//==============================================================================
// int SignedMod
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//

//==============================================================================
int SignedMod(int a, int b)
{
    rAssert(b>0);
    if (a>=0) return (a%b);
    else return (b - ((-a)%b));
}


//==============================================================================
// DebugInfo::Toggle
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::Toggle(int step)
{
    _CurrentSection = SignedMod(_CurrentSection+step, _NumSection);
    _DebugMenuTime = 1.0f;
}


//==============================================================================
// DebugInfo::SetAutoReset
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::SetAutoReset(bool autoreset)
{
   rAssert(_NumSection>0);
   rAssert(_StackSize>0);

   _ppSections[_pStack[_StackSize-1]]->SetAutoReset(autoreset);
}



//==============================================================================
// DebugInfo::Reset
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::Reset(char* sectionName)
{
   rAssert(_NumSection>0);
   rAssert(_StackSize>0);

   _ppSections[_pStack[_StackSize-1]]->Reset(sectionName);
}


//==============================================================================
// DebugInfo::AddLine
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::AddLine(const rmt::Vector& a, const rmt::Vector& b, tColour colour)
{
    rAssert(_NumSection>0);
    rAssert(_StackSize>0);

    _ppSections[_pStack[_StackSize-1]]->AddLine(a, b, colour);
}


//==============================================================================
// DebugInfo::AddHVector
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::AddHVector(rmt::Vector o, rmt::Vector v, float h, tColour colour)
{
    // Add the origin to the vector
    v.Add(o);
    // Add the elevation
    v.Add(rmt::Vector(0, h, 0));
    o.Add(rmt::Vector(0, h, 0));
    // Then, its a regular line
    AddLine(o, v, colour);
}


//==============================================================================
// DebugInfo::AddStar
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::AddStar(const rmt::Vector& vx, tColour colour, float scale)
{
    rAssert(_NumSection>0);
    rAssert(_StackSize>0);

    //Draw a kind of star 
    rmt::Vector a, b;
    a = b = vx;
    a.x += scale; b.x -= scale;
    AddLine( a, b, colour );
    a = b = vx;
    a.y += scale; b.y -= scale;
    AddLine( a, b, colour );
    a = b = vx;
    a.z += scale; b.z -= scale;
    AddLine( a, b, colour );
}


//==============================================================================
// DebugInfo::AddBox
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::AddBox(const rmt::Vector& a, const rmt::Vector& b, tColour colour)
{
    //Draw all 12 lines that make up the box
    for(int i=0;i<4;i++)
    {
    	AddLine(rmt::Vector((i&2)?a.x:b.x,(i&1)?a.y:b.y,a.z),
    		rmt::Vector((i&2)?a.x:b.x,(i&1)?a.y:b.y,b.z),colour);
    	AddLine(rmt::Vector((i&2)?a.x:b.x,a.y,(i&1)?a.z:b.z),
    		rmt::Vector((i&2)?a.x:b.x,b.y,(i&1)?a.z:b.z),colour);
    	AddLine(rmt::Vector(a.x,(i&2)?a.y:b.y,(i&1)?a.z:b.z),
    		rmt::Vector(b.x,(i&2)?a.y:b.y,(i&1)?a.z:b.z),colour);
    }
}


//==============================================================================
// DebugInfo::AddBox
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::AddBox(const rmt::Vector &center, const float r, tColour colour)
{
    rmt::Vector a,b;
    a.Sub(center, rmt::Vector(r,r,r));
    b.Add(center, rmt::Vector(r,r,r));

    AddBox(a,b,colour);
}


//==============================================================================
// DebugInfo::AddCircle
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::AddCircle( const rmt::Vector& center, const float r, tColour colour )
{
    rmt::Vector a,b;
    int i;
    int n = 8;
    for( i = 0; i < n; i++ )
    {
        a.x = center.x + r * rmt::Cos( i * rmt::PI_2 / n );
    	a.y = center.y;
        a.z = center.z + r * rmt::Sin( i * rmt::PI_2 / n );
    	b.x = center.x + r * rmt::Cos( ( i + 1 ) * rmt::PI_2 / n );
    	b.y = center.y;
    	b.z = center.z + r * rmt::Sin( ( i + 1 ) * rmt::PI_2 / n );
    	AddLine( a, b, colour );
    }
}


//==============================================================================
// DebugInfo::AddText
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::AddText(const char *szName, const rmt::Vector &pos, tColour colour)
{
    rAssert(_NumSection>0);
    rAssert(_StackSize>0);

    _ppSections[_pStack[_StackSize-1]]->AddText(szName, pos, colour);
}


//==============================================================================
// DebugInfo::AddScreenLine
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::AddScreenLine(const rmt::Vector& a, const rmt::Vector& b, tColour colour)
{
    rAssert(_NumSection>0);
    rAssert(_StackSize>0);

    _ppSections[_pStack[_StackSize-1]]->AddScreenLine(a, b, colour);
}


//==============================================================================
// DebugInfo::AddScreenText
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::AddScreenText(const char* szName, tColour colour )
{
    rAssert(_NumSection>0);
    rAssert(_StackSize>0);

    _ppSections[_pStack[_StackSize-1]]->AddScreenText(szName, colour);
}


//==============================================================================
// DebugInfo::AddScreenText
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::AddScreenText(const char* szName, const rmt::Vector &a, tColour colour )
{
    rAssert(_NumSection>0);
    rAssert(_StackSize>0);

    _ppSections[_pStack[_StackSize-1]]->AddScreenText(szName, a,colour);
}


//==============================================================================
// DebugInfo::RenderBackground
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::RenderBackground()
{
    p3d::stack->Push();
    p3d::stack->LoadIdentity();
    pddiProjectionMode mode = p3d::pddi->GetProjectionMode();
    p3d::pddi->SetProjectionMode(PDDI_PROJECTION_DEVICE);
    p3d::pddi->SetCullMode(PDDI_CULL_NONE);
    
    if( _shader == NULL )
    {
        //
        // init the shader
        //
        _shader = p3d::device->NewShader("simple");
        _shader->SetInt(PDDI_SP_SHADEMODE, PDDI_SHADE_FLAT); 
    }
    _shader->SetInt(PDDI_SP_BLENDMODE, PDDI_BLEND_ALPHA);

    pddiPrimStream* stream = p3d::pddi->BeginPrims(_shader, PDDI_PRIM_TRIANGLES, PDDI_V_C, 6);

    float nearplane, farplane, fov, aspect;
    p3d::pddi->GetCamera(&nearplane, &farplane, &fov, &aspect);
    nearplane += 0.01f;

    float width, height;
    width = (float)p3d::display->GetWidth();
    height = (float)p3d::display->GetHeight();
    pddiColour colour(0, 0, 0, 200); 

    stream->Colour(colour); stream->Coord(0,      height, nearplane);
    stream->Colour(colour); stream->Coord(width,  height, nearplane);
    stream->Colour(colour); stream->Coord(width,  0,      nearplane);
    stream->Colour(colour); stream->Coord(0,      height, nearplane);
    stream->Colour(colour); stream->Coord(width,  0,      nearplane);
    stream->Colour(colour); stream->Coord(0,      0,      nearplane);


    p3d::pddi->EndPrims(stream);

    p3d::pddi->SetProjectionMode(mode);
    p3d::stack->Pop();
}


//==============================================================================
// DebugInfo::Render
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugInfo::Render()
{
    //rAssert(_StackSize==0);

    // skip if disabled or no sections
    if(! _isRenderEnabled) return;
    if(_NumSection == 0) return;

    // this makes the text easier to read
    if(_isBackgroundEnabled) RenderBackground();

    // render the title with the name of the current section
    char sectionName[255] = "DebugInfo : ";
    Section *pSect = _ppSections[_CurrentSection];
    sprintf(sectionName, "DebugInfo : %s", pSect->GetName());
    p3d::pddi->DrawString(sectionName, 40, 50, tColour(0,255,255));

    // render the current section
   	pSect->Render();

    //Clear all the sections after we display
    int i;
    for(i=0;i<_NumSection;i++)
    { 
        if(_ppSections[i]->GetAutoReset())
        {
            _ppSections[i]->Reset(_ppSections[i]->GetName());
        }
    }    
}

#endif // DEBUGINFO_ENABLED