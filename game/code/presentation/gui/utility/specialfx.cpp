//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   SpecialFX
//
// Description: Implementation of the SpecialFX class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/30      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/utility/specialfx.h>
#include <boundeddrawable.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

bool GuiSFX::Flash
(
    Scrooby::BoundedDrawable* drawable,
    float elapsedTime,
    float durationTime,
    int zoomRate,
    float maxScale,
    float thresholdScale
)
{
    if( elapsedTime > durationTime )
    {
        return true;
    }

    rAssert( drawable != NULL );
    rAssert( elapsedTime >= 0 && durationTime > 0 );

    float scale = (float)elapsedTime / (float)durationTime;
    for( int i = 0; i < zoomRate; i++ )
    {
        scale = rmt::Sqrt( scale );
    }

    scale *= maxScale;

    drawable->ResetTransformation();
    drawable->ScaleAboutCenter( scale, scale, 1.0f );

    if( scale > thresholdScale )
    {
        drawable->SetAlpha( 1.0f - ((scale - thresholdScale) /
                                    (maxScale - thresholdScale)) );
    }
    else
    {
        drawable->SetAlpha( 1.0f );
    }

    return false;
}

bool GuiSFX::Blink
(
    Scrooby::Drawable* drawable,
    float elapsedTime,
    float blinkingPeriod,
    bool usingAlpha
)
{
    rAssert( drawable );

    if( elapsedTime > blinkingPeriod )
    {
        if( usingAlpha )
        {
            // toggle drawable visiblity (using alpha)
            //
            float newAlpha = (drawable->GetAlpha() > 0.5f) ? 0.0f : 1.0f;
            drawable->SetAlpha( newAlpha );
        }
        else
        {
            // toggle drawable visibility
            //
            drawable->SetVisible( !drawable->IsVisible() );
        }

        return true;
    }

    return false;
}

float GuiSFX::Pulse
(
    float elapsedTime,
    float period,
    float center,
    float amplitude,
    float thetaOffset
)
{
    float theta = rmt::PI_2 * elapsedTime / period - thetaOffset;

    return( amplitude * rmt::Sin( theta ) + center );
}

void GuiSFX::ModulateColour
(
    tColour* currentColour,
    float elapsedTime,
    float period,
    tColour startColour,
    tColour endColour,
    float thetaOffset
)
{
    float modValue = GuiSFX::Pulse( elapsedTime, period, 0.5f, 0.5f, thetaOffset );

    rAssert( currentColour != NULL );

    currentColour->SetRed( startColour.Red() +
                          (int)( modValue * (endColour.Red() - startColour.Red()) ) );

    currentColour->SetGreen( startColour.Green() +
                            (int)( modValue * (endColour.Green() - startColour.Green()) ) );

    currentColour->SetBlue( startColour.Blue() +
                           (int)( modValue * (endColour.Blue() - startColour.Blue()) ) );

    currentColour->SetAlpha( startColour.Alpha() +
                            (int)( modValue * (endColour.Alpha() - startColour.Alpha()) ) );
}

float GuiSFX::Pendulum
(
    Scrooby::BoundedDrawable* drawable,
    float deltaTime,
    float length,
    float currentAngle,
    float initialAngle,
    float gravity
)
{
    rAssertMsg( 0, "WARNING: *** Not yet implemented!" );

    return 0.0f;
/*
    rAssert( drawable );

    float root = (2 * gravity / length) *
                 (rmt::Cos( initialAngle ) - rmt::Cos( currentAngle ));

    int rootSign = root > 0 ? 1 : -1;

    // update current angle
    if( currentAngle > 0.0f )
    {
        currentAngle -= rmt::Sqrt( root * rootSign )
                        * (deltaTime / 1000.0f);
    }
    else
    {
        currentAngle += rmt::Sqrt( root * rootSign )
                        * (deltaTime / 1000.0f);
    }

    // rotate pendulum arm
    drawable->ResetTransformation();
    drawable->RotateAboutCenter( rmt::RadianToDeg( currentAngle ) );

    return currentAngle;
*/
}

bool GuiSFX::Spiral
(
    Scrooby::BoundedDrawable* drawable,
    float elapsedTime,
    float durationTime,
    float rotationPeriod,
    float startScale,
    float endScale,
    bool withFading
)
{
    if( elapsedTime > durationTime )
    {
        drawable->ResetTransformation();
        drawable->ScaleAboutCenter( endScale );

        if( withFading )
        {
            drawable->SetAlpha( 1.0f );
        }

        return true;
    }

    float spiralValue = elapsedTime / durationTime;

    // calculate current scale factor
    //
    float scale = startScale +
                  spiralValue * (endScale - startScale);

    // slow down rotation when nearing end of spiral effect
    //
    rotationPeriod += spiralValue * rotationPeriod;

    // calculate current rotation angle
    //
    float rotation = elapsedTime / rotationPeriod * rmt::PI_2;

    // scale and rotate drawable
    //
    rAssert( drawable );
    drawable->ResetTransformation();
    drawable->ScaleAboutCenter( scale );
    drawable->RotateAboutCenter( rmt::RadianToDeg( rotation ) );

    // if enabled, fade in the drawable as well
    if( withFading )
    {
        drawable->SetAlpha( spiralValue );
    }

    return false;
}

bool GuiSFX::SlideX
(
    Scrooby::Drawable* drawable,
    float elapsedTime,
    float durationTime,
    bool slideInwards,
    int fromBorder,
    int screenWidth
)
{
    rAssert( drawable );

    if( elapsedTime > durationTime )
    {
        drawable->ResetTransformation();

        return true;
    }

    int posX = 0;
    int posY = 0;
    int width = 0;
    int height = 0;
    drawable->GetOriginPosition( posX, posY );
    drawable->GetBoundingBoxSize( width, height );

    int distanceX = (fromBorder > 0) ? (screenWidth - posX) : (posX + width);

    float translationX = slideInwards ?
                         (1.0f - elapsedTime / durationTime) * distanceX :
                         (elapsedTime / durationTime) * distanceX;

    drawable->ResetTransformation();
    drawable->Translate( static_cast<int>( translationX ), 0 );

    return false;
}

bool GuiSFX::SlideY
(
    Scrooby::Drawable* drawable,
    float elapsedTime,
    float durationTime,
    bool slideInwards,
    int fromBorder,
    int screenHeight
)
{
    rAssert( drawable );

    if( elapsedTime > durationTime )
    {
        drawable->ResetTransformation();

        return true;
    }

    int posX = 0;
    int posY = 0;
    int width = 0;
    int height = 0;
    drawable->GetOriginPosition( posX, posY );
    drawable->GetBoundingBoxSize( width, height );

    int distanceY = (fromBorder > 0) ? (screenHeight - posY) : (posY + height);

    float translationY = slideInwards ?
                         (1.0f - elapsedTime / durationTime) * distanceY :
                         (elapsedTime / durationTime) * distanceY;

    drawable->ResetTransformation();
    drawable->Translate( 0, static_cast<int>( translationY ) );

    return false;
}

bool GuiSFX::Flip
(
    Scrooby::BoundedDrawable* drawable,
    float elapsedTime,
    float durationTime,
    float startAngle,
    float endAngle,
    rmt::Vector axis
)
{
    rAssert( drawable );
    drawable->ResetTransformation();

    if( elapsedTime > durationTime )
    {
        return true;
    }

    // calculate current rotation angle
    float rotation = startAngle +
                     elapsedTime / durationTime * (endAngle - startAngle);

    // rotate the drawable about specified axis
    drawable->RotateAboutCenter( rmt::RadianToDeg( rotation ), axis );

    return false;
}

void GuiSFX::Projectile
(
    Scrooby::Drawable* drawable,
    float elapsedTime,
    float durationTime,
    rmt::Vector initVelocity,
    bool reverse,
    float gravity
)
{
    if( reverse )
    {
        // reverse the elapsed time
        //
        elapsedTime = durationTime - elapsedTime;
    }

    // calculate the current trajectory position
    //
    rmt::Vector currentPos;
    currentPos.x = initVelocity.x * elapsedTime;
    currentPos.y = initVelocity.y * elapsedTime +
                   0.5f * gravity * elapsedTime * elapsedTime;
    currentPos.z = 0.0f;

    // translate the drawable
    //
    rAssert( drawable );
    drawable->Translate( static_cast<int>( currentPos.x ),
                         static_cast<int>( currentPos.y ) );
}

void GuiSFX::Projectile
(
    Scrooby::Drawable* drawable,
    float elapsedTime,
    float durationTime,
    rmt::Vector start,
    rmt::Vector end,
    bool reverse,
    float gravity
)
{
    // calculate the initial velocity
    //
    rmt::Vector initVelocity;
    initVelocity.x = (end.x - start.x) / durationTime;
    initVelocity.y = (end.y - start.y - 0.5f * gravity * durationTime * durationTime) / durationTime;
    initVelocity.z = 0.0f;

    GuiSFX::Projectile( drawable,
                        elapsedTime,
                        durationTime,
                        initVelocity,
                        reverse,
                        gravity );
}

