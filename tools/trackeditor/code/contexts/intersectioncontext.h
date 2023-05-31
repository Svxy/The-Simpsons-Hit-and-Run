#include "precompiled/PCH.h"

#ifndef INTERSECTION_CONTEXT
#define INTERSECTION_CONTEXT

//----------------------------------------
// System Includes
//----------------------------------------


//----------------------------------------
// Forward References
//----------------------------------------

//-----------------------------------------------------------------------------
//
// I n t e r s e c t i o n C o n t e x t
//
//-----------------------------------------------------------------------------
class IntersectionContext : public MPxContext
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
            

                        IntersectionContext();
        virtual        ~IntersectionContext();

        static const char* stringId;

        virtual void    toolOnSetup( MEvent& );
        virtual void    toolOffCleanup();
        virtual MStatus doPress( MEvent& );
        virtual MStatus doDrag( MEvent& );
        virtual MStatus doRelease( MEvent& event );
        virtual MStatus doHold( MEvent& event );
        virtual MStatus doEnterRegion( MEvent& event );
        virtual void    deleteAction();
        virtual void    completeAction();
        virtual void    abortAction();

    private:
        void    ProcessState( Stimulus stimulus );
        void    SetHelpString();
        void    InitIntersection();

        MString     mHelp;
        
        short       mXCurrent, mYCurrent;
        short       mXDrag, mYDrag;
        MObject     mIntersection;
        MObject     mIntersectionTransform;
};

//-----------------------------------------------------------------------------
//
// I n t e r s e c t i o n C o n t e x t C m d
//
//-----------------------------------------------------------------------------
class IntersectionContextCmd : public MPxContextCommand
{
    public:
                                IntersectionContextCmd();
        virtual                ~IntersectionContextCmd();
                                
        static  void*           creator();

        virtual MPxContext*     makeObj();

    private:
};

#endif
