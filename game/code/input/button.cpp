#include <input/button.h>

unsigned int Button::mTickCount = 0;

Button::Button( void ) :
    mfValue( 0.0f ),
    mTickCountAtChange( 0 )
{
}

Button::~Button( void )
{
}

