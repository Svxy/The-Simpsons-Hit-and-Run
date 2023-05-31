//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   SpecialFX
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/30      TChu        Created for SRR2
//
//===========================================================================

#ifndef SPECIALFX_H
#define SPECIALFX_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <p3d/p3dtypes.hpp>
#include <radmath/radmath.hpp>

//===========================================================================
// Forward References
//===========================================================================

namespace Scrooby
{
    class Drawable;
    class BoundedDrawable;
}

//===========================================================================
// Interface Definitions
//===========================================================================

namespace GuiSFX
{

bool Flash
(
    Scrooby::BoundedDrawable* drawable,
    float elapsedTime,
    float durationTime,
    int zoomRate = 0, // the higher the number, the faster the zoom
    float maxScale = 1.5f,
    float thresholdScale = 1.0f
);

bool Blink
(
    Scrooby::Drawable* drawable,
    float elapsedTime,
    float blinkingPeriod = 1000.0f,
    bool usingAlpha = false
);

float Pulse
(
    float elapsedTime,
    float period,
    float center = 0.0f,
    float amplitude = 1.0f,
    float thetaOffset = 0.0f
);

void ModulateColour
(
    tColour* currentColour,
    float elapsedTime,
    float period,
    tColour startColour,
    tColour endColour,
    float thetaOffset = 0.0f
);

float Pendulum
(
    Scrooby::BoundedDrawable* drawable,
    float deltaTime,
    float length,
    float currentAngle,
    float initialAngle,
    float gravity = 9.81f
);

bool Spiral
(
    Scrooby::BoundedDrawable* drawable,
    float elapsedTime,
    float durationTime,
    float rotationPeriod,
    float startScale = 0.0f,
    float endScale = 1.0f,
    bool withFading = true
);

enum eSlideBorderX
{
    SLIDE_BORDER_LEFT = -1,
    SLIDE_BORDER_RIGHT = 1
};

bool SlideX
(
    Scrooby::Drawable* drawable,
    float elapsedTime,
    float durationTime,
    bool slideInwards,
    int fromBorder,
    int screenWidth = 640
);

enum eSlideBorderY
{
    SLIDE_BORDER_BOTTOM = -1,
    SLIDE_BORDER_TOP = 1
};

bool SlideY
(
    Scrooby::Drawable* drawable,
    float elapsedTime,
    float durationTime,
    bool slideInwards,
    int fromBorder,
    int screenHeight = 480
);

bool Flip
(
    Scrooby::BoundedDrawable* drawable,
    float elapsedTime,
    float durationTime,
    float startAngle = 0.0f,
    float endAngle = rmt::PI_2,
    rmt::Vector axis = rmt::Vector( 1, 0, 0 )
);

void Projectile
(
    Scrooby::Drawable* drawable,
    float elapsedTime,
    float durationTime,
    rmt::Vector initVelocity,
    bool reverse = false,
    float gravity = 0.005f
);

void Projectile
(
    Scrooby::Drawable* drawable,
    float elapsedTime,
    float durationTime,
    rmt::Vector start,
    rmt::Vector end,
    bool reverse = false,
    float gravity = 0.005f
);

} // GuiSFX namespace

#endif // SPECIALFX_H
