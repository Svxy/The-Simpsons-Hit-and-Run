#include "precompiled/PCH.h"

#ifndef LOCATOR_CONTEXT
#define LOCATOR_CONTEXT

//----------------------------------------
// System Includes
//----------------------------------------
#include <..\..\..\game\code\meta\locatortypes.h>
#include <..\..\..\game\code\meta\locatorevents.h>

//----------------------------------------
// Forward References
//----------------------------------------

//-----------------------------------------------------------------------------
//
// L o c a t o r C o n t e x t
//
//-----------------------------------------------------------------------------
class LocatorContext : public MPxContext
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
            

                        LocatorContext();
        virtual        ~LocatorContext();

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
        void    InitLocator();

        MString     mHelp;
        
        short       mXCurrent, mYCurrent;
        short       mXDrag, mYDrag;
        MObject     mLocator;
        MObject     mLocatorTransform;
        
        static LocatorType::Type mLastLocatorType;
        static LocatorEvent::Event mLastEvent;
};

//-----------------------------------------------------------------------------
//
// I n t e r s e c t i o n C o n t e x t C m d
//
//-----------------------------------------------------------------------------
class LocatorContextCmd : public MPxContextCommand
{
    public:
                                LocatorContextCmd();
        virtual                ~LocatorContextCmd();
                                
        static  void*           creator();

        virtual MPxContext*     makeObj();

    private:
};

#endif
