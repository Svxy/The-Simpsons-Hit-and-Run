#include <sound/soundfx/soundeffectplayer.cpp>
#include <sound/soundfx/soundfxfrontendlogic.cpp>
#include <sound/soundfx/soundfxgameplaylogic.cpp>
#include <sound/soundfx/soundfxlogic.cpp>
#include <sound/soundfx/soundfxpauselogic.cpp>
#include <sound/soundfx/reverbcontroller.cpp>
#include <sound/soundfx/reverbsettings.cpp>
#include <sound/soundfx/positionalsoundsettings.cpp>

#ifdef RAD_PS2
#include <sound/soundfx/ps2reverbcontroller.cpp>
#else
#include <sound/soundfx/gcreverbcontroller.cpp>
#endif