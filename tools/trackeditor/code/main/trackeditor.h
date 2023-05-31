#include "precompiled/PCH.h"

#ifndef TRACK_EDITOR
#define TRACK_EDITOR

//This node exists as the top node of all the other TrackEditor types.
//See te_setup.mel for more details of how this is a node.
//There should only ever be one of these in the Hypergraph.

//This is the place where options will be stored also.

class TrackEditor
{
public:
    TrackEditor();
    ~TrackEditor();

    static const char*  Name;

    static bool Exists();
    static MStatus AddChild( MObject& obj );

    //These are the Track Editing functions and state.
    enum EditMode
    {
        OFF,
        EDIT,
        DISPLAY
    };

    static EditMode GetEditMode();

    static void SetDeleteTreelines( bool del );
    static bool GetDeleteTreelines();

protected:

    friend class TEStateChangeCommand;
    static EditMode sEditMode;
    static void SetEditMode( EditMode mode );

    static bool sDeleteTreelines;

private:
    static unsigned int sNodeAddedbackID;
    static unsigned int sWindowClosedCallbackID;

    static void NodeAddedCB( MObject& node, void* data );
    static void WindowClosedCB( void* data ); 
    static void CreateTrackEditorNode();
    static void CreateTileDisplayNode();
    static void RemoveTileDisplayNode();
};

//=============================================================================
// TrackEditor::SetDeleteTreelines
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool del )
//
// Return:      void 
//
//=============================================================================
inline void TrackEditor::SetDeleteTreelines( bool del )
{
    sDeleteTreelines = del;
}

//=============================================================================
// TrackEditor::GetDeleteTreelines
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool TrackEditor::GetDeleteTreelines()
{
    return sDeleteTreelines;
}

#endif