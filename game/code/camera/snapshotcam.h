//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        snapshotcam.h
//
// Description: Blahblahblah
//
// History:     11/28/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef SNAPSHOTCAM_H
#define SNAPSHOTCAM_H

//========================================
// Nested Includes
//========================================
#include <camera/supercam.h>

//========================================
// Forward References
//========================================
class tVectorCamera;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class SnapshotCam : public SuperCam
{
public:
    enum { MAX_TCAMERAS = 50 };

    SnapshotCam();
	virtual ~SnapshotCam();

    //Update: Called when you want the super cam to update its state.
    virtual void Update( unsigned int milliseconds );

    //Returns the name of the super cam.  
    //This can be used in the FE or debug info
    virtual const char* const GetName() const;

    //This loads the off-line created settings for the camera.  
    //It is passed in as a byte stream of some data of known size.
    virtual void LoadSettings( unsigned char* settings ) {}; 
 
    virtual Type GetType();

    //These are for favourable support of this command
    virtual void SetTarget( ISuperCamTarget* target ); 
    virtual void AddTarget( ISuperCamTarget* target );
    
    unsigned int GetNumTargets() const;

private:
    //These functions are to allow real-time control of the settings of 
    //the supercam.
    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();

    virtual void OnInit() { InitMyController(); };

    tVectorCamera* mCameras[ MAX_TCAMERAS ];
    unsigned int mNumCameras;
    unsigned int mCurrentCamera;

    bool mToggling;

    //Prevent wasteful constructor creation.
	SnapshotCam( const SnapshotCam& snapshotcam );
	SnapshotCam& operator=( const SnapshotCam& snapshotcam );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// SnapshotCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================
inline const char* const SnapshotCam::GetName() const
{
    return "SNAPSHOT_CAM";
}

//=============================================================================
// SnapshotCam::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
inline SuperCam::Type SnapshotCam::GetType()
{
    return SNAPSHOT_CAM;
}

//=============================================================================
// SnapshotCam::SetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void SnapshotCam::SetTarget( ISuperCamTarget* target )
{
}

//=============================================================================
// SnapshotCam::AddTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void SnapshotCam::AddTarget( ISuperCamTarget* target )
{
}

//=============================================================================
// SnapshotCam::GetNumTargets
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int SnapshotCam::GetNumTargets() const
{
    return 0;
}

#endif //SNAPSHOTCAM_H
