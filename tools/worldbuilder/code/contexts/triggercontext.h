#include "precompiled/PCH.h"

#ifndef TRIGGER_CONTEXT
#define TRIGGER_CONTEXT

//----------------------------------------
// System Includes
//----------------------------------------


//----------------------------------------
// Forward References
//----------------------------------------

//-----------------------------------------------------------------------------
//
// T r i g g e r C o n t e x t
//
//-----------------------------------------------------------------------------
class TriggerContext : public MPxContext
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
            

                        TriggerContext();
        virtual        ~TriggerContext();

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
};

//-----------------------------------------------------------------------------
//
// T r i g g e r C o n t e x t C m d
//
//-----------------------------------------------------------------------------
class TriggerContextCmd : public MPxContextCommand
{
    public:
                                TriggerContextCmd();
        virtual                ~TriggerContextCmd();
                                
        static  void*           creator();

        virtual MPxContext*     makeObj();

    private:
};

#endif //TRIGGER_CONTEXT
