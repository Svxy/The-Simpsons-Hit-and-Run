//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        railcam.h
//
// Description: Blahblahblah
//
// History:     17/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef RAILCAM_H
#define RAILCAM_H

//========================================
// Nested Includes
//========================================
#include <radmath/radmath.hpp>

#ifndef WORLD_BUILDER
#include <camera/supercam.h>
#else
#include "supercam.h"
#endif

//========================================
// Forward References
//========================================
class ISuperCamTarget;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class RailCam : public SuperCam
{
public:
    RailCam();
    virtual ~RailCam();

    //-----------  From SuperCam
    //Update: Called when you want the super cam to update its state.
    virtual void Update( unsigned int milliseconds );

    //Returns the name of the super cam.  
    //This can be used in the FE or debug info
    virtual const char* const GetName() const;

    virtual Type GetType();

    virtual void InitController();

    //This loads the off-line created settings for the camera.  
    //It is passed in as a byte stream of some data of known size.
    virtual void LoadSettings( unsigned char* settings ); 

    //These are for favourable support of this command
    virtual void SetTarget( ISuperCamTarget* target ); 
    virtual void AddTarget( ISuperCamTarget* target );

    virtual unsigned int GetNumTargets() const;

    //-----------  RailCam specific
    typedef enum { DISTANCE=1, PROJECTION, NUM_BEHAVIOUR } Behaviour;
    static const char* const BehaviourNames[];
    
    void SetBehaviour( Behaviour b );
    void SetMaxStep( float du );
    void SetMinRadius( float r );
    void SetMaxRadius( float r );
    void SetClosedRail( bool closed );
    void SetNumCVs( unsigned int num );
    void SetVertex( unsigned int idx, const rmt::Vector& cv );
    void SetTrackRail( bool trackRail );
    void SetTrackDist( float offset );
    void SetStartPosition( float u0 );  
    void SetStartPositionCB();  //Called by the watcher.
    void SetReverseSense( bool reverseSense );
    void SetTargetOffset( const rmt::Vector& offset );
    void SetAxisPlay( const rmt::Vector& play );
    void SetPositionLag( float lag );
    void SetTargetLag( float lag );

    void SetDrawRail( bool draw );
    void SetDrawHull( bool draw );
    void SetDrawCylinder( bool draw );
    void SetDrawIntersections( bool draw );

    void SetSplineCurve( rmt::SplineCurve& curve );

    void SetMaxFOV( float fov );
    void SetFOVLag( float lag );

    void SetReset( bool reset );

    #ifdef DEBUGWATCH
        virtual const char* GetWatcherName() const;
    #endif

protected:
    //Init...  This gets called when the camera is registered
    virtual void OnInit();
    virtual void OnShutdown();

    //These functions are to allow real-time control of the settings of 
    //the supercam.
    virtual void OnRegisterDebugControls();
    virtual void OnUnregisterDebugControls();

    virtual float GetTargetSpeedModifier();

    //-----------  RailCam specific RENDERING
    void DrawRail( bool active );
    void DrawHull( bool active );
    void DrawCylinder( const rmt::Vector& origin );


private:

    //-----------  From SuperCam
    void GetTargetPosition( rmt::Vector* position, bool withOffset = true );
    void DenyUpdate();
    void AllowUpdate();


    ISuperCamTarget* mTarget;


    //-----------  RailCam specific
    typedef enum { EXACT, APPROX, WORSTCASE } SolutionType;

    SolutionType IntervalClamp(float &t) const;
    float& ProjectPointOnLine(const rmt::Vector& A, const rmt::Vector& B, const rmt::Vector& O, float& t) const;
    SolutionType IntersectLineCylinder(const int segment, const rmt::Vector& origin, const float radius, const rmt::Vector& neighbour, float& t);

    rmt::Vector FindCameraPosition_Distance(const rmt::Vector& target, const float radius);
    rmt::Vector FindCameraPosition_Projection(const rmt::Vector& target, const float pOffset);

    rmt::Vector FindCameraLookAt(const rmt::Vector& target, const rmt::Vector& desiredPos );
    rmt::Vector FinalizeRailPosition(SolutionType index);

    rmt::Vector TestEval( float u );

    Behaviour   mBehaviour;
    float       mMinRadius;
    float       mMaxRadius;
    float       mTrackDist;
    float       mStartU;

    rmt::SplineCurve mQ;
    rmt::SplineCurve mQd;  //Derivative of mQ

    float mU;       //Current parametric position along curve
    float mStep;    //max parametric step size along rail

    struct CamPosition {
        CamPosition() { /**/ };
        void Reset() { segment=-1; u=0.0f; dist=9.9E9f; pDist=9.9E9f; }
 
        int         segment;    // segment of the curve for this solution
        float       u;          // u value along this segment [0, 1]
        float       dist;       // world-space distance to current camera position
        float       pDist;      // parameter space distance to current camera position
        rmt::Vector pu;         // world-space position of camera at u
    } mCandidates[3];

    rmt::Vector mTargetOffset;
    rmt::Vector mAxisPlay;
    float mPositionLag;
    float mTargetLag;

    rmt::Vector mPosition;
    rmt::Vector mPositionDelta;

    rmt::Vector mTargetPosition;
    rmt::Vector mTargetPositionDelta;

    float mFOVDelta;
    float mMaxFOV;
    float mFOVLag;

#ifdef DEBUGWATCH
    bool mTrackRail;
    bool mReverseSensing;
    bool mDrawRail;
    bool mDrawHull;
    bool mDrawCylinder;
    bool mDrawIntersections;
#else
    bool mTrackRail         : 1;
    bool mReverseSensing    : 1;
    bool mDrawRail          : 1;
    bool mDrawHull          : 1;
    bool mDrawCylinder      : 1;
    bool mDrawIntersections : 1;
#endif
    bool mAllowUpdate       : 1;
    bool mReset             : 1;
    bool mResetting         : 1;

    //Prevent wasteful constructor creation.
    RailCam( const RailCam& railcam );
    RailCam& operator=( const RailCam& railcam );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// RailCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================
inline const char* const RailCam::GetName() const
{
    return "RAIL_CAM";
}

//=============================================================================
// RailCam::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
inline SuperCam::Type RailCam::GetType()
{
    return RAIL_CAM;
}

//=============================================================================
// RailCam::SetTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetTarget( ISuperCamTarget* target )
{
    mTarget = target;
}

//=============================================================================
// RailCam::AddTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ( ISuperCamTarget* target )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::AddTarget( ISuperCamTarget* target )
{
    rAssertMsg( false, "Only call SetTarget on the RailCam" );
}

//=============================================================================
// ChaseCam::GetNumTargets
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int RailCam::GetNumTargets() const
{
    if ( mTarget )
    {
        return 1;
    }

    return 0;
}

//=============================================================================
// RailCam::SetBehaviour
//=============================================================================
// Description: Comment
//
// Parameters:  ( Behaviour b )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetBehaviour( Behaviour b )
{
    mBehaviour = b;
}

//=============================================================================
// RailCam::SetMaxStep
//=============================================================================
// Description: Comment
//
// Parameters:  ( float du )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetMaxStep( float du )
{
    mStep = du;
}

//=============================================================================
// RailCam::SetMinRadius
//=============================================================================
// Description: Comment
//
// Parameters:  ( float r )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetMinRadius( float r )
{
    mMinRadius = r;
}

//=============================================================================
// RailCam::SetMaxRadius
//=============================================================================
// Description: Comment
//
// Parameters:  ( float r )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetMaxRadius( float r )
{
    mMaxRadius = r;
}

//=============================================================================
// RailCam::SetClosedRail
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool closed )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetClosedRail( bool closed )
{
    mQ.SetClosed( closed );
}

//=============================================================================
// RailCam::SetNumCVs
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int num )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetNumCVs( unsigned int num )
{
    mQ.SetNumVertices( num );
    mQd.SetNumVertices( num );

    if ( (mStartU >= 0.0f) && (mStartU <= 1.0f) )
    {
        mStartU = mStartU * mQ.GetEndParam();
    }
    else
    {
        mStartU = -1.0f;
    }
}

//=============================================================================
// RailCam::SetVertex
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int idx, const rmt::Vector& cv )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetVertex( unsigned int idx, const rmt::Vector& cv )
{
    mQ.SetCntrlVertex(idx, cv);
    mQd.SetCntrlVertex(idx, cv);

}

//=============================================================================
// RailCam::SetTrackRail
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool trackRail )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetTrackRail( bool trackRail )
{
    mTrackRail = trackRail;
}

//=============================================================================
// RailCam::SetTrackDist
//=============================================================================
// Description: Comment
//
// Parameters:  ( float offset )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetTrackDist( float offset )
{
    mTrackDist = offset;
}

//=============================================================================
// RailCam::SetStartPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( float u0 )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetStartPosition( float u0 )
{
    if ( mQ.GetNumVertices() > 0 )
    {
        if ( (u0 >= 0.0f && u0 <= 1.0f) )
        {
            mStartU = u0 * mQ.GetEndParam();
        }
        else 
        {
            mStartU = -1.0f;
        }
    } 
    else
    {
        // The first time this is called CVs are not yet set on the spline, 
        // so we don't know how long it'll be.
        // SetCameraNumCVs() fixes this...
        mStartU = u0;
    }

}

//=============================================================================
// RailCam::SetStartPositionCB
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetStartPositionCB()
{
    SetStartPosition(mStartU);
}

//=============================================================================
// RailCam::SetReverseSense
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool reverseSense )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetReverseSense( bool reverseSense )
{
    mReverseSensing = reverseSense;
}

//=============================================================================
// RailCam::SetTargetOffset
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& offset )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetTargetOffset( const rmt::Vector& offset )
{
    mTargetOffset = offset;
}

//=============================================================================
// RailCam::SetAxisPlay
//=============================================================================
// Description: Comment
//
// Parameters:  ( const rmt::Vector& play )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetAxisPlay( const rmt::Vector& play )
{
    mAxisPlay = play;
}

//=============================================================================
// RailCam::SetPositionLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetPositionLag( float lag )
{
    mPositionLag = lag;
}

//=============================================================================
// RailCam::SetTargetLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetTargetLag( float lag )
{
    mTargetLag = lag;
}


//=============================================================================
// RailCam::SetDrawRail
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool draw )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetDrawRail( bool draw )
{
    mDrawRail = draw;
}

//=============================================================================
// RailCam::SetDrawHull
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool draw )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetDrawHull( bool draw )
{
    mDrawHull = draw;
}

//=============================================================================
// RailCam::SetDrawCylinder
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool draw )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetDrawCylinder( bool draw )
{
    mDrawCylinder = draw;
}

//=============================================================================
// RailCam::SetDrawIntersections
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool draw )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetDrawIntersections( bool draw )
{
    mDrawIntersections = draw;
}

//=============================================================================
// RailCam::SetSplineCurve
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::SplineCurve& curve )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetSplineCurve( rmt::SplineCurve& curve )
{
    mQ.SetNumVertices( curve.GetNumVertices() );
    mQd.SetNumVertices( curve.GetNumVertices() );

    mQ = curve;
    mQd = curve;

    mQd.SetBasis( rmt::Spline::DBSpline );

    if ( (mStartU >= 0.0f) && (mStartU <= 1.0f) )
    {
        mStartU = mStartU * mQ.GetEndParam();
    }
    else
    {
        mStartU = -1.0f;
    }
}

//=============================================================================
// RailCam::SetMaxFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( float fov )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetMaxFOV( float fov )
{
    mMaxFOV = fov;
}

//=============================================================================
// RailCam::SetFOVLag
//=============================================================================
// Description: Comment
//
// Parameters:  ( float lag )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetFOVLag( float lag )
{
    mFOVLag = lag;
}

//=============================================================================
// RailCam::SetReset
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool reset )
//
// Return:      void 
//
//=============================================================================
inline void RailCam::SetReset( bool reset )
{
    mReset = true;
}

//=============================================================================
// RailCam::AllowUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void RailCam::AllowUpdate()
{
    mAllowUpdate = true;
}

//=============================================================================
// RailCam::DenyUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
inline void RailCam::DenyUpdate()
{
    mAllowUpdate = false;
}

#endif //RAILCAM_H
