#ifndef _STATEPROPDATATYPES_HPP_
#define _STATEPROPDATATYPES_HPP_

namespace StatePropDataTypes
{

//Transition data
struct TransitionData
{
    unsigned int    autoTransition;
	unsigned int    toState;
	float           onFrame;
};

//Visibility data
struct VisibilityData
{
	unsigned int    isVisible;
};

//Frame controller data
struct FrameControllerData
{
    unsigned int    isCyclic;
    unsigned int    numberOfCycles;
    unsigned int    holdFrame;
	float           minFrame;
	float           maxFrame;
	float           relativeSpeed;
};

//Event data 
struct EventData
{
    char            eventName[64];
	unsigned int    eventID;
    unsigned int    toState;
};

//Callback data
struct CallbackData
{
    char            callbackName[64];
	unsigned int    callbackID;
    float           onFrame;
};

};

#endif //_STATEPROPDATATYPES_HPP_
