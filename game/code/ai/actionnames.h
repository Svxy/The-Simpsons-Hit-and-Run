#ifndef ACTION_NAMES_H_
#define ACTION_NAMES_H_

namespace ActionButton
{
    const char* const ActionName[] =
    { 
        "ToggleOnOff", 
        "ToggleReverse", 
        "PlayAnim", 
        "PlayAnimLoop",
        "AutoPlayAnim",
        "AutoPlayAnimLoop",
        "AutoPlayAnimInOut",
        "DestroyObject",
		"PowerCoupling",
        "UseVendingMachine", 
        "PrankPhone",
        "SummonVehiclePhone",
        "Doorbell",
        "OpenDoor",
        "TalkFood",
        "TalkCollectible",
        "TalkDialog",
        "TalkMission",
        "FoodSmall",
        "FoodLarge",
        "CollectorCard",
		"AlienCamera",
        "PlayOnce",
        "AutomaticDoor",
        "Wrench",
        "Teleport",
        "PurchaseCar",
        "PurchaseSkin",
        "Nitro"
    };
    const int ActionNameSize = ( sizeof(ActionButton::ActionName) / sizeof(ActionButton::ActionName[0]) ); 
   
    const char* const ButtonName[] = 
    {
        "NONE",
        "LeftStickX",
        "LeftStickY",
        "Action",
        "Jump",
        "Dash",
        "Attack",
        "DPadUp",
        "DPadDown",
        "DPadLeft",
        "DPadRight"
#ifdef RAD_WIN32
        ,"GetOutCar"
        ,"MouseLookLeft"
        ,"MouseLookRight"
#endif
    };

    const int ButtonNameListSize = ( sizeof(ActionButton::ButtonName) / sizeof(ActionButton::ButtonName[0]) );
};

#endif // ACTION_NAMES_H_
