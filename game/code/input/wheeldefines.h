#ifndef WHEEL_DEFINES
#define WHEEL_DEFINES

#ifdef RAD_PS2
typedef struct lgDevForceEffect LGForceEffect;
#endif

#ifdef RAD_PS2
//Why the hell are these different?
#define LG_TYPE_DAMPER LGTYPE_DAMPER
#define LG_TYPE_SPRING LGTYPE_SPRING
#define LG_TYPE_CONSTANT LGTYPE_CONSTANT
#define LG_TYPE_TRIANGLE LGTYPE_TRIANGLE
#define LG_TYPE_SQUARE LGTYPE_SQUARE
#define LG_DURATION_INFINITE LGDURATION_INFINITE
#define type Type
#define duration Duration
#define startDelay StartDelay
#define magnitude Magnitude
#define direction Direction
#define offset Offset
#define deadband Deadband
#define saturationNeg SaturationNeg
#define saturationPos SaturationPos
#define coefficientNeg CoefficientNeg
#define coefficientPos CoefficientPos
#define period Period
#define phase Phase
#define attackTime AttackTime
#define fadeTime FadeTime
#define attackLevel AttackLevel
#define fadeLevel FadeLevel
#endif

#ifdef RAD_WIN32
enum eForceTypes
{
    CONSTANT_FORCE, 
    RAMP_FORCE,
    SQUARE, 
    SINE,
    TRIANGLE,
    SAWTOOTH_UP, 
    SAWTOOTH_DOWN, 
    SPRING, 
    DAMPER,
    INERTIA, 
    FRICTION, 
    CUSTOM_FORCE 
};
#endif

#endif