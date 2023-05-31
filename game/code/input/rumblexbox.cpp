#include <raddebug.hpp>
#include <raddebugwatch.hpp>

#include <input/rumbleeffect.h>

#ifdef DEBUGWATCH
extern int gRECount;
#endif

EffectValue VALUES[] = 
{
    // timeout gain  motor name - Note: Do not change motor here!  It is platform specific.
    {   1,     0.6f,   0,  "Light" },
    {   1,     0.8f,   0,  "Med" },
    {   1,     1.0f,   0,  "Hard1" },
    {   1,     1.0f,   1,  "Hard2" },
    { 100,     0.9f,   1,  "Ground 1" },
    { 120,     1.0f,   1,  "Ground 2" },
    { 150,    0.95f,   1,  "Ground 3" },
    { 120,    0.95f,   1,  "Ground 4" },
    {   1,     1.0f,   1,  "Pulse" },
}; 

EffectValue DYNA_VALUES[] =
{
    // timeout gain  motor name - Note: Do not change motor here!  It is platform specific.
    {   1,     1.0f,   1,  "Collision1" },
    {   1,     1.0f,   1,  "Collision2" }
};


//=============================================================================
// RumbleEffect::SetWheelEffect
//=============================================================================
// Description: Comment
//
// Parameters:  ( IRadControllerOutputPoint* wheelEffect )
//
// Return:      void 
//
//=============================================================================
void RumbleEffect::SetWheelEffect( IRadControllerOutputPoint* wheelEffect )
{
}

//*****************************************************************************
//
//Private Member Functions
//
//*****************************************************************************

