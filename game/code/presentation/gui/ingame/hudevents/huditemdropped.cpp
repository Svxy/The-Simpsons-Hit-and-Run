//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   HudItemDropped
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/02/05		TChu		Created
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/ingame/hudevents/huditemdropped.h>
#include <presentation/gui/utility/specialfx.h>

// Scrooby
#include <page.h>
#include <group.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

HudItemDropped::HudItemDropped( Scrooby::Page* pPage )
:   HudEventHandler( pPage->GetGroup( "ItemDropped" ) ),
    m_currentSubState( 0 ),
    m_itemDropped( NULL )
{
    m_itemDropped = m_drawableGroup->GetText( "ItemDropped" );
    rAssert( m_itemDropped != NULL );
    m_itemDropped->SetTextMode( Scrooby::TEXT_WRAP );
}

HudItemDropped::~HudItemDropped()
{
}

void
HudItemDropped::Start()
{
    this->OnStart();

    m_currentSubState = STATE_TRANSITION_IN;

    rAssert( m_itemDropped != NULL );
    m_itemDropped->ResetTransformation();
    m_itemDropped->SetAlpha( 0.0f );
    m_itemDropped->SetVisible( true );
}

void
HudItemDropped::Stop()
{
    this->OnStop();
}

void
HudItemDropped::Update( float elapsedTime )
{
    if( m_currentState == STATE_RUNNING )
    {
        m_elapsedTime += elapsedTime;

        switch( m_currentSubState )
        {
            case STATE_TRANSITION_IN:
            {
                const float TRANSITION_IN_TIME = 200.0f;

                rAssert( m_itemDropped != NULL );
                m_itemDropped->ResetTransformation();

                if( m_elapsedTime < TRANSITION_IN_TIME )
                {
                    float percentageDone = m_elapsedTime / TRANSITION_IN_TIME;

                    // slide in from HUD icon
                    //
                    m_itemDropped->Translate( 0, static_cast<int>( (1.0f - percentageDone) * 100 ) );

                    // fade in coin coint
                    //
                    m_itemDropped->SetAlpha( percentageDone );
                }
                else
                {
                    m_itemDropped->ResetTransformation();
                    m_itemDropped->SetAlpha( 1.0f );

                    // advance to next sub state
                    //
                    m_elapsedTime = 0.0f;
                    m_currentSubState++;
                }

                break;
            }
            case STATE_DISPLAY_HOLD:
            {
                const float BLINKING_PERIOD = 250.0f;

                bool isBlinked = GuiSFX::Blink( m_itemDropped,
                                                m_elapsedTime,
                                                BLINKING_PERIOD );

                if( isBlinked )
                {
                    m_elapsedTime = static_cast<float>( static_cast<int>( m_elapsedTime ) %
                                                        static_cast<int>( BLINKING_PERIOD ) );
                }
/*
                const float DISPLAY_HOLD_TIME = 2000.0f;

                if( m_elapsedTime < DISPLAY_HOLD_TIME )
                {
                    static int coinPosX = CGuiScreen::IsWideScreenDisplay() ? 500 : 555;
                    static int coinPosY = 422;
                    GetCoinManager()->SetHUDCoin( coinPosX, coinPosY, true );

                    int currentBankValue = GetCoinManager()->GetBankValue();

                    rAssert( m_numCoins != NULL );
                    if( m_currentItemCount > currentBankValue )
                    {
                        // coin loss
                        //
                        m_elapsedCoinDecrementTime += elapsedTime;

                        tColour coinColour;
                        GuiSFX::ModulateColour( &coinColour,
                                                m_elapsedCoinDecrementTime,
                                                UPDATE_PERIOD * 2,
                                                tColour( 255, 255, 255 ),
                                                tColour( 255, 0, 0 ) );

                        m_numCoins->SetColour( coinColour );
                    }
                    else
                    {
                        m_numCoins->SetColour( tColour( 255, 255, 255 ) );
                    }
                }
                else
                {
                    GetCoinManager()->SetHUDCoin( 0, 0, false );

                    rAssert( m_numCoins != NULL );
                    m_numCoins->SetColour( tColour( 255, 255, 255 ) );

                    // advance to next sub state
                    //
                    m_elapsedTime = 0.0f;
                    m_currentSubState++;
                }
*/
                break;
            }
            case STATE_TRANSITION_OUT:
            {
                const float TRANSITION_OUT_TIME = 200.0f;
/*
                if( m_elapsedTime < TRANSITION_OUT_TIME )
                {
                    float percentageDone = m_elapsedTime / TRANSITION_OUT_TIME;

                    // slide out coin count
                    //
                    GuiSFX::SlideY( m_itemsCount,
                                    m_elapsedTime,
                                    TRANSITION_OUT_TIME,
                                    false,
                                    GuiSFX::SLIDE_BORDER_TOP,
                                    100 );
                }
                else
                {
                    // advance to next sub state
                    //
                    m_elapsedTime = 0.0f;
                    m_currentSubState++;

                    if( !m_isFullCount )
                    {
                        // skip to final sub state
                        //
                        m_currentSubState = NUM_SUB_STATES;

                    }
                }
*/
                break;
            }
            case NUM_SUB_STATES:
            {
                // ok, we're done
                //
                this->Stop();

                break;
            }
            default:
            {
                rAssertMsg( false, "Unhandled sub-state!" );
                break;
            }
        }
    }
}

