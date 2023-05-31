//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        treelinecontext.h
//
// Description: Blahblahblah
//
// History:     27/05/2002 + Created -- Cary Brisebois
//
//=============================================================================
#include "precompiled/PCH.h"

#ifndef TREELINECONTEXT_H
#define TREELINECONTEXT_H

//========================================
// Nested Includes
//========================================


//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class TreeLineContext : public MPxContext
{
public:

    enum Stimulus       // Maskable values.
    {
        BUTTONDOWN      = 0x0001,
        BUTTONUP        = 0x0002,
        MOUSEDRAG       = 0x0004,
        COMPLETED       = 0x0008,
        DELETED         = 0x0010,
        ABORTED         = 0x0020
    };     

    TreeLineContext();
    virtual ~TreeLineContext();

    static const char* stringId;

    virtual void    toolOnSetup( MEvent& event);
    virtual void    toolOffCleanup();
    virtual MStatus doPress( MEvent& event);
    virtual MStatus doDrag( MEvent& event );
    virtual MStatus doRelease( MEvent& event );
    virtual MStatus doHold( MEvent& event );
    virtual MStatus doEnterRegion( MEvent& event );
    virtual void    deleteAction();
    virtual void    completeAction();
    virtual void    abortAction();

private:
    void    ProcessState( Stimulus stimulus );
    void    AddPoint( MPoint& point );
    void    DeleteLast();
    void    SetHelpString();

    MPointArray             mPoints;
    MString                 mHelp;

    static MObject          mCurrentTreeLine;
    static bool             mWorking;

    short                   mXCurrent, mYCurrent;

private:

    //Prevent wasteful constructor creation.
    TreeLineContext( const TreeLineContext& treelinecontext );
    TreeLineContext& operator=( const TreeLineContext& treelinecontext );
};

//******************************************************************************
//
// TreeLineContextCmd
//
//******************************************************************************

class TreeLineContextCmd : public MPxContextCommand
{
    public:
                                TreeLineContextCmd() {};
        virtual                ~TreeLineContextCmd() {};
                                
        static  void*           creator();

        virtual MPxContext*     makeObj();

    private:
};

//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// TreeLineContextCmd::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
inline void* TreeLineContextCmd::creator()
{
    return new TreeLineContextCmd();
}

//=============================================================================
// TreeLineContextCmd::makeObj
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MPxContext
//
//=============================================================================
inline MPxContext* TreeLineContextCmd::makeObj()
{
    return new TreeLineContext();
}


#endif //TREELINECONTEXT_H

