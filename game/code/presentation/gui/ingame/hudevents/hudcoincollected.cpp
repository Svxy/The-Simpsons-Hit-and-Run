//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   HudCoinCollected
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
#include <presentation/gui/ingame/hudevents/hudcoincollected.h>
#include <presentation/gui/utility/specialfx.h>
#include <presentation/gui/guiscreen.h>

#include <worldsim/coins/coinmanager.h>

// Scrooby
#include <app.h>
#include <page.h>
#include <group.h>
#include <sprite.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const short NUMERIC_TEXT_SPACING = -12;

//===========================================================================
// Public Member Functions
//===========================================================================

HudCoinCollected::HudCoinCollected( Scrooby::Page* pPage )
:   HudEventHandler( pPage->GetGroup( "ItemCollected" ) ),
    m_currentSubState( 0 ),
    m_cardImage( NULL ),
    m_cardText( NULL ),
    m_itemsCount( NULL ),
    m_numCoins( NULL ),
    m_currentItemCount( 0 ),
    m_itemsComplete( NULL ),
    m_isFullCount( false ),
    m_elapsedCoinDecrementTime( 0.0f )
{
    rAssert( pPage != NULL );

    m_itemsCount = pPage->GetGroup( "ItemsCount" );
    rAssert( m_itemsCount != NULL );

    m_numCoins = m_itemsCount->GetSprite( "ItemsCount" );
    rAssert( m_numCoins != NULL );
    m_numCoins->SetSpriteMode( Scrooby::SPRITE_BITMAP_TEXT );
    m_numCoins->CreateBitmapTextBuffer( BITMAP_TEXT_BUFFER_SIZE );
    m_numCoins->SetBitmapTextSpacing( NUMERIC_TEXT_SPACING );
#ifdef RAD_WIN32
    m_numCoins->Translate( 70, 0 );
    m_numCoins->ScaleAboutCenter( 0.5f );
#endif

    m_itemsComplete = pPage->GetGroup( "ItemsComplete" );

    Scrooby::Group* itemCollected = pPage->GetGroup( "ItemCollected" );
    rAssert( itemCollected != NULL );

    m_cardImage = itemCollected->GetSprite( "CardCollected" );
    rAssert( m_cardImage != NULL );

    m_cardText = itemCollected->GetGroup( "CardText" );
    rAssert( m_cardText != NULL );

    m_currentItemCount = GetCoinManager()->GetBankValue();
}

HudCoinCollected::~HudCoinCollected()
{
}

void
HudCoinCollected::Start()
{
    if( m_currentState == STATE_RUNNING )
    {
        if( m_currentSubState > STATE_COIN_TRANSITION_IN )
        {
            // return to beginning of display hold state
            //
            m_currentSubState = STATE_COIN_DISPLAY_HOLD;
            m_elapsedTime = 0.0f;
            m_elapsedCoinDecrementTime = 0.0f;

            // restore drawable properties
            //
            rAssert( m_itemsCount != NULL );
            m_itemsCount->ResetTransformation();
            m_itemsCount->SetAlpha( 1.0f );
        }
    }
    else
    {
        m_currentSubState = STATE_COIN_TRANSITION_IN;
        m_elapsedCoinDecrementTime = 0.0f;

        rAssert( m_numCoins != NULL );
        m_numCoins->SetColour( tColour( 255, 255, 255 ) );

        // restore drawable properties
        //
        rAssert( m_itemsCount != NULL );
        m_itemsCount->ResetTransformation();
        m_itemsCount->SetAlpha( 0.0f );

        rAssert( m_itemsComplete != NULL );
        m_itemsComplete->SetVisible( false );

        // hide all the cards sub-groups
        //
        m_cardImage->SetVisible( false );
        m_cardText->SetVisible( false );
    }

    this->OnStart();

    // update coin count
    //
    this->SetItemCount( static_cast<unsigned int>( m_currentItemCount ), 0 );
}

void
HudCoinCollected::Stop()
{
    GetCoinManager()->SetHUDCoin( 0, 0, false );

    this->OnStop();
}

void
HudCoinCollected::Update( float elapsedTime )
{
    if( m_currentState == STATE_RUNNING )
    {
        m_elapsedTime += elapsedTime;

        switch( m_currentSubState )
        {
            case STATE_COIN_TRANSITION_IN:
            {
                const float COIN_TRANSITION_IN_TIME = 200.0f;

                rAssert( m_itemsCount != NULL );

                if( m_elapsedTime < COIN_TRANSITION_IN_TIME )
                {
                    float percentageDone = m_elapsedTime / COIN_TRANSITION_IN_TIME;

                    // slide in coin count
                    //
                    GuiSFX::SlideY( m_itemsCount,
                                    m_elapsedTime,
                                    COIN_TRANSITION_IN_TIME,
                                    true,
                                    GuiSFX::SLIDE_BORDER_TOP,
                                    100 );

                    // fade in coin coint
                    //
                    m_itemsCount->SetAlpha( percentageDone );
                }
                else
                {
                    m_itemsCount->ResetTransformation();
                    m_itemsCount->SetAlpha( 1.0f );

                    // advance to next sub state
                    //
                    m_elapsedTime = 0.0f;
                    m_currentSubState++;
                }

                break;
            }
            case STATE_COIN_DISPLAY_HOLD:
            {
                const float COIN_DISPLAY_HOLD_TIME = 2000.0f;
                const float COIN_UPDATE_PERIOD = 40.0f;

                if( m_elapsedTime < COIN_DISPLAY_HOLD_TIME )
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
                                                COIN_UPDATE_PERIOD * 2,
                                                tColour( 255, 255, 255 ),
                                                tColour( 255, 0, 0 ) );

                        m_numCoins->SetColour( coinColour );
                    }
                    else
                    {
                        m_numCoins->SetColour( tColour( 255, 255, 255 ) );
                    }

                    // update coin change progressively
                    //
                    if( m_elapsedTime > COIN_UPDATE_PERIOD )
                    {
                        if( m_currentItemCount < currentBankValue )
                        {
                            m_currentItemCount++;
                            this->SetItemCount( static_cast<unsigned int>( m_currentItemCount ), 0 );

                            m_elapsedTime -= COIN_UPDATE_PERIOD;
                        }
                        else if( m_currentItemCount > currentBankValue )
                        {
                            m_currentItemCount--;
                            this->SetItemCount( static_cast<unsigned int>( m_currentItemCount ), 0 );

                            GetCoinManager()->AddFlyDownCoin();

							m_elapsedTime -= COIN_UPDATE_PERIOD;
                        }
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

                break;
            }
            case STATE_COIN_TRANSITION_OUT:
            {
                const float COIN_TRANSITION_OUT_TIME = 200.0f;

                if( m_elapsedTime < COIN_TRANSITION_OUT_TIME )
                {
                    float percentageDone = m_elapsedTime / COIN_TRANSITION_OUT_TIME;

                    // slide out coin count
                    //
                    GuiSFX::SlideY( m_itemsCount,
                                    m_elapsedTime,
                                    COIN_TRANSITION_OUT_TIME,
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

void
HudCoinCollected::SetItemCount( unsigned int numCollected,
                                unsigned int numCollectibles )
{
    char buffer[ BITMAP_TEXT_BUFFER_SIZE ];
    sprintf( buffer, "%d", numCollected );

    rAssert( m_numCoins != NULL );
    m_numCoins->SetBitmapText( buffer );

    // set card deck complete flag
    //
    m_isFullCount = (numCollected == numCollectibles);
}

