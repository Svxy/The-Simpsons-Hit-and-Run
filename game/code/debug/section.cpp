#include <debug/section.h>
#include <p3d/matrixstack.hpp>
#include <p3d/utility.hpp>

Section::Section(tFont* pFont, const char* szName) :
	_pDebugFont(pFont),
	_pLineShader(NULL),
    _autoReset(true)
{
	assert(szName);

	int i;

	Reset(szName);

	_pLineShader = p3d::device->NewShader(/*p3d::pddi, */"simple");
	_pLineShader->AddRef();
	_pLineShader->SetInt(PDDI_SP_BLENDMODE, PDDI_BLEND_ALPHA);

	//Allocate string primitives for the 3d strings
	for(i=0;i<MaxText;i++)
	{
		if(_pDebugFont)
		{
            _pDebugFont->AddRef();

            P3D_UNICODE string[256];
            char emptyString[] = "Empty String";
            p3d::AsciiToUnicode(emptyString, string, strlen(emptyString));

			_pText[i]._pText = new tTextString(_pDebugFont, string);
			_pText[i]._pText->AddRef();
		}
		else
		{
			_pText[i]._pText = NULL;
		}
	}
}
Section::~Section()
{
	for(int i=0;i<MaxText;i++)
	{
		tRefCounted::Release(_pText[i]._pText);
	}

	tRefCounted::Release(_pLineShader);
	tRefCounted::Release(_pDebugFont);
}

void Section::Reset(const char *szName)
{
	assert(szName);
	strncpy(_szName, szName, sizeof(_szName));

	_NumLines = _NumText = _NumScreenLines = _NumScreenText = 0;
	ClearScreenText();
}

const char* Section::GetName() const
{

	return _szName;
}

void Section::AddLine(const rmt::Vector& a, const rmt::Vector& b, tColour colour/*, float time*/)
{
	if(_NumLines < MaxLines)
	{
		_Lines[_NumLines]._a = a;
		_Lines[_NumLines]._b = b;
		_Lines[_NumLines]._Colour = colour;
		_NumLines++;
	}
}

void Section::AddText(const char *szName, const rmt::Vector &pos, tColour colour/*, float time = 0*/)
{
	assert(szName);
	if(_NumText < MaxText && _pText[_NumText]._pText)
	{
		tTextString* p = _pText[_NumText]._pText;

        P3D_UNICODE string[256];
        p3d::AsciiToUnicode(szName, string, strlen(szName));

		p->SetString(string);
		p->SetColour(colour);
		p->SetScale(20.0f); //Fudge factor to make font appear about 1 meter high
		_pText[_NumText]._Pos = pos;
		_NumText++;
	}
}

void Section::AddScreenLine(const rmt::Vector &a, const rmt::Vector &b, tColour colour)
{
	if(_NumScreenLines < MaxScreenLines)
	{
		_ScreenLines[_NumScreenLines]._a = a;
		_ScreenLines[_NumScreenLines]._b = b;
		_ScreenLines[_NumScreenLines]._Colour = colour;
		_NumScreenLines++;
	}
}

void Section::AddScreenText(const char* szName, tColour colour )
{
	assert(szName);
	ScreenText &pST =  _pScreenText[_NumScreenText];

	if(_NumScreenText<MaxScreenText && pST._pText)
	{
		strncpy(pST._pText, szName, sizeof(pST._pText));
		pST._Colour = colour;
		pST._Pos = rmt::Vector(0.05f,_ScreenTextPos, 0);

		//TODO:get correct debug font height!
		_ScreenTextPos += 0.03f;
		_NumScreenText++;
	}
}
void Section::AddScreenText(const char* szName, const rmt::Vector &a, tColour colour)
{
	assert(szName);
	ScreenText &pST =  _pScreenText[_NumScreenText];

	if(_NumScreenText<MaxScreenText && pST._pText)
	{
		if(pST._pText)
			strncpy(pST._pText, szName, sizeof(pST._pText));
		pST._Colour = colour;
		pST._Pos = a;
		_NumScreenText++;
	}
}

void Section::ClearScreenText()
{
	_NumScreenText = 0;
	_ScreenTextPos = 0.15f;
}

void Section::Render()
{
	pddiPrimStream* pStream;
	int i;
	assert(_pLineShader);

	//Special case to get the pddi debugging information
	p3d::pddi->EnableStatsOverlay(0==strcmp(_szName, "pddi"));

	//Stream out world space lines
	if(_NumLines > 0)
    { 
        pStream = p3d::pddi->BeginPrims(_pLineShader, PDDI_PRIM_LINES, PDDI_V_C, _NumLines*2);
        if(pStream)
        {
		    for(i=0; i<_NumLines; i++)
		    {
			    Line &l = _Lines[i];

			    pddiVector A(l._a.x, l._a.y, l._a.z);
		   	    pddiVector B(l._b.x, l._b.y, l._b.z);
			    pStream->Vertex(&A,l._Colour);
			    pStream->Vertex(&B,l._Colour);
		    }
	    }
	    p3d::pddi->EndPrims(pStream);
    }

	//Draw all the world space vector text
	for(i=0;i<_NumText;i++)
	{
		p3d::stack->Push();
		p3d::stack->Translate(_pText[i]._Pos);
		_pText[i]._pText->Display();
		p3d::stack->Pop();
	}

	//Draw screen space lines
	{
		//Setup the matrix stack
		p3d::stack->Push();
		p3d::pddi->EnableZBuffer(false);
		p3d::stack->LoadIdentity();
		p3d::stack->Scale((float)p3d::display->GetWidth(),(float)p3d::display->GetHeight(),1);
		p3d::pddi->SetProjectionMode(PDDI_PROJECTION_DEVICE);

		//Stream out lines
	    if(_NumScreenLines > 0)
		{
            pStream = p3d::pddi->BeginPrims(_pLineShader, PDDI_PRIM_LINES, PDDI_V_C, _NumScreenLines*2);
		    if(pStream)
		    {
			    for(i=0; i<_NumScreenLines; i++)
			    {
				    Line &l = _ScreenLines[i];

				    pddiVector A(l._a.x, l._a.y, 1);
				    pddiVector B(l._b.x, l._b.y, 1);
				    pStream->Vertex(&A,l._Colour);
				    pStream->Vertex(&B,l._Colour);
			    }
		    }
		    p3d::pddi->EndPrims(pStream);
        }

		//Restore the old stack
		p3d::pddi->SetProjectionMode(PDDI_PROJECTION_PERSPECTIVE);
		p3d::pddi->EnableZBuffer(true);
		p3d::stack->Pop();
	}

	//Draw all the screen text
	p3d::stack->Push();
	p3d::stack->LoadIdentity();
	int height = p3d::display->GetHeight();
	int width = p3d::display->GetWidth();

	for(i=0;i<_NumScreenText;i++)
	{
		ScreenText &pST =  _pScreenText[i];
		if(pST._pText)
		{

			p3d::pddi->DrawString(pST._pText, 
				(int)(pST._Pos.x*width),
				(int)(pST._Pos.y*height), 
				pST._Colour);

		}
	}

	p3d::stack->Pop();
}

