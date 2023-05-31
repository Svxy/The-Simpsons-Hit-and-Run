//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   StunnedBehaviour
//
// Description: Handles being stunned
//
// Authors:     Michael Riegger
//
//===========================================================================

#include <ai\actor\stunnedbehaviour.h>

StunnedBehaviour::StunnedBehaviour( float stunTime )
{
    m_StunTime = stunTime / 1000.0f;
}

StunnedBehaviour::~StunnedBehaviour()
{
    
}

void
StunnedBehaviour::Apply( Actor* actor, unsigned int timeInMS )
{

}