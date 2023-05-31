

#if 1

#ifndef _SECTION_HPP
#define _SECTION_HPP

#include <p3d/textstring.hpp>
#include <p3d/font.hpp>
#include <pddi/pddi.hpp>
#include <radmath/radmath.hpp>

// TODO: move this to internal namespace

/**
 * Section.
 */
class Section
{
public:
    /** Constructor 
     * @param sectionName name of the section
     */
	Section(tFont* pFont, const char* sectionName);

    /** Destructor */
	~Section();

    void SetAutoReset(bool autoReset) { _autoReset = autoReset; }
    bool GetAutoReset()               { return _autoReset; }

	void Reset(const char *name);
	void ClearScreenText();
	void Render();

	const char* GetName() const;

	//Debugging output functions
	void AddLine(const rmt::Vector &a, const rmt::Vector &b, tColour colour);
	void AddText(const char *szName, const rmt::Vector &pos, tColour colour);

	//Output functions for screen space primitives
	//All screen space is in x=[0..1], y=[0..1], with (0,0) at the top left
	void AddScreenLine(const rmt::Vector &a, const rmt::Vector &b, tColour colour);
	void AddScreenText(const char* szName, tColour colour);
	void AddScreenText(const char* szName, const rmt::Vector &a, tColour colour);

private:
	struct Line
	{
		rmt::Vector	_a;
		rmt::Vector	_b;
		tColour _Colour;
	};

	struct ScreenText
	{
		enum { MaxTextSize = 256 };
		
		tColour _Colour;
		char    _pText[MaxTextSize];
		rmt::Vector	_Pos;
	};

	struct VectorText
	{
		tTextString* _pText;
		rmt::Vector  _Pos;
	};


private:
	char _szName[256];

	tFont* _pDebugFont;

	float _ScreenTextPos;

	pddiShader* _pLineShader;

	int _NumLines;
	enum { MaxLines = 50 };	
	Line _Lines[MaxLines];

	int _NumScreenLines;
	enum { MaxScreenLines = 50 };
	Line _ScreenLines[MaxScreenLines];

	int _NumText;
	enum { MaxText = 50 };
	VectorText  _pText[MaxText];

	int _NumScreenText;
	enum { MaxScreenText = 50 };
	ScreenText  _pScreenText[MaxScreenText];

   bool _autoReset;

//denied
private:
	Section();//don't use default constructor
};


#endif


#endif
