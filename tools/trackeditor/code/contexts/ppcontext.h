#include "precompiled/PCH.h"

#ifndef PPCONTEXT
#define PPCONTEXT

//----------------------------------------
// System Includes
//----------------------------------------


//----------------------------------------
// Forward References
//----------------------------------------

//-----------------------------------------------------------------------------
//
// B o u n d i n g v o l u m e C o n t e x t
//
//-----------------------------------------------------------------------------
class PPContext : public MPxContext
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
            

                        PPContext();
        virtual        ~PPContext();

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

        static const MString DEFAULT_GROUP_NAME;
        static MObject sCurrentGroup;

    private:
        void    ProcessState( Stimulus stimulus );
        void    AddPoint( MObject obj );
        void    DeleteLast();
        void    CloseLoop();
        void    SetHelpString();

        MObjectArray            mPoints;
        MString                 mHelp;
        
        short                   mXCurrent, mYCurrent;
};

//-----------------------------------------------------------------------------
//
// B o u n d i n g v o l u m e C o n t e x t C m d
//
//-----------------------------------------------------------------------------
class PPContextCmd : public MPxContextCommand
{
    public:
                                PPContextCmd();
        virtual                ~PPContextCmd();
                                
        static  void*           creator();

        virtual MPxContext*     makeObj();

    private:
};

class PPSplitCmd : public MPxCommand
{
public:
    MStatus doIt( const MArgList& args );
    static void* creator();

    static const char* stringId;

private:
    void Split( MObject& node1, MObject& node2 ); 
};

#endif
