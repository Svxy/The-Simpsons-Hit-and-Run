//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        WBCamTarget.cpp
//
// Description: Implement WBCamTarget
//
// History:     19/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include "main/toolhack.h"
#include <toollib.hpp>

#include "WBCamTarget.h"
#include "utility/transformmatrix.h"
#include "main/constants.h"

#include "utility/mext.h"


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

void GetMatrix( MObject& obj, tlMatrix& hmatrix )
{
    MFnDagNode fnNode( obj );

    MObject transform;
    transform = fnNode.parent( 0 );
    MFnTransform fnTransform( transform );

    MDagPath dagPath;
    if ( MExt::FindDagNodeByName( &dagPath, fnTransform.name() ) )
    {
        TransformMatrix tm( dagPath );

        tm.GetHierarchyMatrixLHS( hmatrix );
    }
    else
    {
        MExt::DisplayError( "Target matrix is screwy!" );
    }
}


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// WBCamTarget::WBCamTarget
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
WBCamTarget::WBCamTarget() :
    mTarget( MObject::kNullObj )
{
}

//==============================================================================
// Description: Constructor.
//
// Parameters:	MObject& target.
//
// Return:      N/A.
//
//==============================================================================
WBCamTarget::WBCamTarget( MObject& target ) :
    mTarget( target )
{
}

//==============================================================================
// WBCamTarget::~WBCamTarget
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
WBCamTarget::~WBCamTarget()
{
}

//=============================================================================
// WBCamTarget::GetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* position )
//
// Return:      void 
//
//=============================================================================
void WBCamTarget::GetPosition( rmt::Vector* position )
{
    tlMatrix hmatrix;
    GetMatrix( mTarget, hmatrix );

    tlPoint point = hmatrix.GetRow( 3 );

    *position = point;

    *position /= WBConstants::Scale;
}

//=============================================================================
// WBCamTarget::GetHeading
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* heading )
//
// Return:      void 
//
//=============================================================================
void WBCamTarget::GetHeading( rmt::Vector* heading )
{
    tlMatrix hmatrix;
    GetMatrix( mTarget, hmatrix );

    tlPoint point = hmatrix.GetRow( 2 );

    *heading = point;

    *heading /= WBConstants::Scale;
}

//=============================================================================
// WBCamTarget::GetVUP
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* vup )
//
// Return:      void 
//
//=============================================================================
void WBCamTarget::GetVUP( rmt::Vector* vup )
{
    tlMatrix hmatrix;
    GetMatrix( mTarget, hmatrix );

    tlPoint point = hmatrix.GetRow( 1 );

    *vup = point;

    *vup /= WBConstants::Scale;
}

//=============================================================================
// WBCamTarget::GetVelocity
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* velocity )
//
// Return:      void 
//
//=============================================================================
void WBCamTarget::GetVelocity( rmt::Vector* velocity )
{
}

//=============================================================================
// WBCamTarget::GetID
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
unsigned int WBCamTarget::GetID()
{
    return 1;
}

//=============================================================================
// WBCamTarget::IsCar
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool WBCamTarget::IsCar()
{
    return false;
}

//=============================================================================
// WBCamTarget::IsAirborn
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool WBCamTarget::IsAirborn()
{
    return false;
}

//=============================================================================
// WBCamTarget::IsUnstable
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool WBCamTarget::IsUnstable()
{
    return false;
}

//=============================================================================
// WBCamTarget::IsQuickTurn
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool WBCamTarget::IsQuickTurn()
{
    return false;
}

//=============================================================================
// WBCamTarget::GetFirstPersonPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* position )
//
// Return:      void 
//
//=============================================================================
void WBCamTarget::GetFirstPersonPosition( rmt::Vector* position )
{
    return;
}

//=============================================================================
// WBCamTarget::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
const char* const WBCamTarget::GetName()
{
    MFnDependencyNode fnDepNode( mTarget );

    return fnDepNode.name().asChar();
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
