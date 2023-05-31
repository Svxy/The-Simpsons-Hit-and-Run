//==============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        debuginfo.h
//
// Description: In-game visual debug output
//
// History:     2002/07/02 + Migrated from Mirth -- Darwin Chau
//
//==============================================================================

#ifndef DEBUGINFO_HPP
#define DEBUGINFO_HPP


//
// DebugInfo is only available in RAD_DEBUG and RAD_TUNE configurations
//
#ifndef RAD_RELEASE
#define DEBUGINFO_ENABLED
#endif // RAD_RELEASE



#include "main/commandlineoptions.h"


//==============================================================================
// *** USE THESE MACROS, DO NOT INVOKE DEBUG INFO DIRECTLY ***
//==============================================================================

#ifndef DEBUGINFO_ENABLED

#define CREATE_DEBUGINFO()
#define DESTROY_DEBUGINFO()

#define DEBUGINFO_PUSH_SECTION(s)
#define DEBUGINFO_POP_SECTION()

#define DEBUGINFO_ADDSCREENTEXT(s)
#define DEBUGINFO_ADDSCREENTEXTVECTOR(s, v)
#define DEBUGINFO_ADDSCREENTEXTVECTORCOLOUR(s, v, c)
#define DEBUGINFO_ADDSCREENLINE(v1, v2, c)
#define DEBUGINFO_ADDLINE(v1, v2, c)
#define DEBUGINFO_ADDCIRCLE(v1, v2, c)
#define DEBUGINFO_ADDSTAR(v1, c, s)

#define DEBUGINFO_TOGGLE(step)

#define DEBUGINFO_RENDER();

#else

#define CREATE_DEBUGINFO()                              if(!CommandLineOptions::Get(CLO_FIREWIRE)) { DebugInfo::CreateInstance(); }
#define DESTROY_DEBUGINFO()                             if(!CommandLineOptions::Get(CLO_FIREWIRE)) { DebugInfo::DestroyInstance(); }

#define DEBUGINFO_PUSH_SECTION(s)                       (!CommandLineOptions::Get(CLO_FIREWIRE)) ? DebugInfo::GetInstance()->Push(s) : false
#define DEBUGINFO_POP_SECTION()                         if(!CommandLineOptions::Get(CLO_FIREWIRE)) { DebugInfo::GetInstance()->Pop(); }

#define DEBUGINFO_ADDSCREENTEXT(s)                      if(!CommandLineOptions::Get(CLO_FIREWIRE)) { DebugInfo::GetInstance()->AddScreenText(s); }
#define DEBUGINFO_ADDSCREENTEXTVECTOR(s, v)             if(!CommandLineOptions::Get(CLO_FIREWIRE)) { DebugInfo::GetInstance()->AddScreenText(s, v); }
#define DEBUGINFO_ADDSCREENTEXTVECTORCOLOUR(s, v, c)    if(!CommandLineOptions::Get(CLO_FIREWIRE)) { DebugInfo::GetInstance()->AddScreenText(s, v, c); }
#define DEBUGINFO_ADDSCREENLINE(v1, v2, c)              if(!CommandLineOptions::Get(CLO_FIREWIRE)) { DebugInfo::GetInstance()->AddScreenLine(v1, v2, c); }
#define DEBUGINFO_ADDLINE(v1, v2, c)                    if(!CommandLineOptions::Get(CLO_FIREWIRE)) { DebugInfo::GetInstance()->AddLine(v1, v2, c); }
#define DEBUGINFO_ADDCIRCLE(v1, v2, c)                  if(!CommandLineOptions::Get(CLO_FIREWIRE)) { DebugInfo::GetInstance()->AddCircle(v1, v2, c); }
#define DEBUGINFO_ADDSTAR(v1, c, s)                     if(!CommandLineOptions::Get(CLO_FIREWIRE)) { DebugInfo::GetInstance()->AddStar(v1, c, s); }

#define DEBUGINFO_TOGGLE(step)                          if(!CommandLineOptions::Get(CLO_FIREWIRE)) { DebugInfo::GetInstance()->Toggle(step); }

#define DEBUGINFO_RENDER()                              if(!CommandLineOptions::Get(CLO_FIREWIRE)) { DebugInfo::GetInstance()->Render(); }


//========================================
// Nested Includes
//========================================
// Radmath
#include <radmath/radmath.hpp>
// Pure3D
#include <p3d/p3dtypes.hpp>
#include <pddi/pddi.hpp>

//========================================
// Forward References
//========================================

class tFont;
class tTypeFace;
class Section;

//==============================================================================
//
// Synopsis: DebugInfo class provides visual debugging support.
//
//==============================================================================
class DebugInfo 
{
public:
    /** get the singleton */
    static DebugInfo* GetInstance();

    /** creates the singleton */
    static void CreateInstance();

    /** destroys the instance */
    static void DestroyInstance();
    static void InitializeStaticVariables();
    

    /** Render */
    void Render();

    /** Push a section */
    bool Push(char* szSection);

    /** Pop a section */
    void Pop();

    /** Get the name of the last section pushed */
    const char* GetCurrentSection();


    //Toggle through the current sections
    void Toggle(int step);

    //Debugging output functions, for world space primitives
    void BeginSectionCreation() { _isCreationSectionOpen = true; };
    void EndSectionCreation()  { _isCreationSectionOpen = false; };

    /** make the section to resest automatically after the render */
    void SetAutoReset(bool autoreset);

    /** reset the current section 
     *  @param sectionName the new name of the section
     */
    void Reset(char* sectionName);

    void AddLine(const rmt::Vector &a, const rmt::Vector &b, tColour colour = tColour(255,255,255));
    void AddHVector(rmt::Vector o, rmt::Vector v, float h, tColour colour = tColour(255,255,255));
    void AddBox(const rmt::Vector &a, const rmt::Vector &b, tColour colour = tColour(255,255,255));
    void AddBox(const rmt::Vector &center, const float r, tColour colour = tColour(255,255,255));
    void AddCircle(const rmt::Vector &center, const float r, tColour colour = tColour(255,255,255));
    void AddStar(const rmt::Vector& vx, tColour colour = tColour(255,255,255), float scale = 0.1f);
    void AddText(const char *szName, const rmt::Vector &pos, tColour colour = tColour(255,255,255));

    //Output functions for screen space primitives
    //All screen space is in x=[0..1], y=[0..1], with (0,0) at the top left
    void AddScreenLine(const rmt::Vector &a, const rmt::Vector &b, tColour colour = tColour(255,255,255));
    void AddScreenText(const char* szName, tColour colour = tColour(255,255,255));
    void AddScreenText(const char* szName, const rmt::Vector &a, tColour colour = tColour(255,255,255));

    //
    // GUI events
    //
    void OnNext()   { Toggle(1); };
    void OnSwitch();

protected:
    void CreateNewSection( const char* section );
private:
    /** Constructor */
    DebugInfo();
    
    /** Destructor */
    ~DebugInfo();

    /** Render the backgound */
    void RenderBackground();


// TODO: (chakib) do we need this ?
#if 1
    tFont* _pDebugFont;
    tTypeFace* _pTypeFace;
#endif

    enum { MaxSections = 20 };
    int _NumSection;
    Section* _ppSections[MaxSections];

    enum { MaxStackSize = 20 };
    int _StackSize;
    int _pStack[MaxStackSize];

    int _CurrentSection;

    float _DebugMenuTime;

    static DebugInfo* _Instance;

    bool _isRenderEnabled;
    bool _isBackgroundEnabled;
    bool _isCreationSectionOpen;
    
    enum Mode {OFF, BACKGROUND, NOBACKGROUND, MODE_MAX};
    Mode _mode;

    pddiShader* _shader; //@- used the render the background
};


#endif // ! DEBUGINFO_ENABLED

#endif // DEBUGINFO_HPP

