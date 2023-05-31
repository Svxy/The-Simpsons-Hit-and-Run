//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   HudCardCollected
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
#include <presentation/gui/ingame/hudevents/hudcardcollected.h>
#include <presentation/gui/utility/specialfx.h>

#include <cards/cardgallery.h>
#include <mission/gameplaymanager.h>

// Scrooby
#include <app.h>
#include <page.h>
#include <group.h>
#include <sprite.h>
#include <text.h>

#ifdef RAD_WIN32
const float HUDCARD_THUMBNAIL_SCALE = 0.44f;
#endif

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

HudCardCollected::HudCardCollected( Scrooby::Page* pPage )
:   HudEventHandler( pPage->GetGroup( "ItemCollected" ) ),
    m_currentSubState( 0 ),
    m_cardImage( NULL ),
    m_cardText( NULL ),
    m_cardGet( NULL ),
    m_cardTitle( NULL ),
    m_itemsCount( NULL ),
    m_numCards( NULL ),
    m_itemsComplete( NULL ),
    m_cardDeckComplete( NULL ),
    m_isCardDeckComplete( false ),
    m_itemsUnlocked( NULL )
{
    rAssert( pPage != NULL );
    Scrooby::Group* itemCollected = pPage->GetGroup( "ItemCollected" );
    rAssert( itemCollected != NULL );
    m_cardImage = itemCollected->GetSprite( "CardCollected" );
    rAssert( m_cardImage != NULL );

    m_cardText = itemCollected->GetGroup( "CardText" );
    rAssert( m_cardText != NULL );

    m_cardGet = m_cardText->GetText( "CardGet" );
    rAssert( m_cardGet != NULL );

    m_cardTitle = m_cardText->GetText( "CardTitle" );
    rAssert( m_cardTitle != NULL );
    m_cardTitle->SetTextMode( Scrooby::TEXT_WRAP );

    m_itemsCount = itemCollected->GetGroup( "ItemsCount" );
    rAssert( m_itemsCount != NULL );

    m_numCards = m_itemsCount->GetSprite( "ItemsCount" );
    rAssert( m_numCards != NULL );
    m_numCards->SetSpriteMode( Scrooby::SPRITE_BITMAP_TEXT );
    m_numCards->CreateBitmapTextBuffer( BITMAP_TEXT_BUFFER_SIZE );
    m_numCards->SetBitmapTextSpacing( -8 );

    m_itemsComplete = itemCollected->GetGroup( "ItemsComplete" );
    rAssert( m_itemsComplete != NULL );

    m_cardDeckComplete = m_itemsComplete->GetText( "ItemsComplete" );
    rAssert( m_cardDeckComplete != NULL );
    m_cardDeckComplete->SetTextMode( Scrooby::TEXT_WRAP );

    m_itemsUnlocked = m_itemsComplete->GetText( "ItemsUnlocked" );
    rAssert(m_itemsUnlocked != NULL );
    m_itemsUnlocked->SetTextMode( Scrooby::TEXT_WRAP );
}

HudCardCollected::~HudCardCollected()
{
}

void
HudCardCollected::Start()
{
    this->OnStart();

    m_currentSubState = STATE_CARD_TRANSITION_IN;

    // restore drawable properties
    //
    rAssert( m_cardImage != NULL );
    m_cardImage->ResetTransformation();
    m_cardImage->SetAlpha( 1.0f );

    rAssert( m_cardText != NULL );
    m_cardText->ResetTransformation();
    m_cardText->SetAlpha( 1.0f );

    rAssert( m_itemsCount != NULL );
    m_itemsCount->ResetTransformation();
    m_itemsCount->SetAlpha( 1.0f );

    rAssert( m_itemsComplete != NULL );
    m_itemsComplete->ResetTransformation();
    m_itemsComplete->SetAlpha( 1.0f );
    m_itemsComplete->SetVisible( false );

    // show all the cards sub-groups
    //
    m_cardImage->SetVisible( true );
    m_cardText->SetVisible( true );

    // update card count
    //
    int currentLevel = GetGameplayManager()->GetCurrentLevelIndex();
    const CardList* cardList = GetCardGallery()->GetCollectedCards( currentLevel );
    rAssert( cardList != NULL );
    this->SetCardCount( cardList->m_numCards, NUM_CARDS_PER_LEVEL );
}

void
HudCardCollected::Stop()
{
    this->OnStop();
}

void
HudCardCollected::Update( float elapsedTime )
{
    if( m_currentState == STATE_RUNNING )
    {
        m_elapsedTime += elapsedTime;

        switch( m_currentSubState )
        {
            case STATE_CARD_TRANSITION_IN:
            {
                const float CARD_TRANSITION_IN_TIME = 500.0f;

                const float centerX = Scrooby::App::GetInstance()->GetScreenWidth() / 2.0f;
                const float centerY = Scrooby::App::GetInstance()->GetScreenHeight() / 2.0f;

                rAssert( m_cardImage != NULL );
                m_cardImage->ResetTransformation();

                int cardCenterX = 0;
                int cardCenterY = 0;
                m_cardImage->GetBoundingBoxCenter( cardCenterX, cardCenterY );

                rAssert( m_cardText != NULL );
                m_cardText->ResetTransformation();

                rAssert( m_itemsCount != NULL );

                if( m_elapsedTime < CARD_TRANSITION_IN_TIME )
                {
                    float percentageDone = m_elapsedTime / CARD_TRANSITION_IN_TIME;

                    // fade in card image (and scale up)
                    //
                    m_cardImage->SetAlpha( percentageDone );
#ifdef RAD_WIN32
                    m_cardImage->ScaleAboutCenter( percentageDone * HUDCARD_THUMBNAIL_SCALE );
#else
                    m_cardImage->ScaleAboutCenter( percentageDone );
#endif

                    // apply projectile motion effect to card image
                    //
                    const float CARD_GRAVITY = 0.005f;
                    GuiSFX::Projectile( m_cardImage,
                                        m_elapsedTime,
                                        CARD_TRANSITION_IN_TIME,
                                        rmt::Vector( (float)cardCenterX, (float)cardCenterY, 0.0f ),
                                        rmt::Vector( centerX, centerY, 0.0f ),
                                        true,
                                        CARD_GRAVITY );

                    // scale up card text
                    //
                    m_cardText->ScaleAboutCenter( percentageDone );

                    // apply projectile motion effect to card text
                    //
                    const float TEXT_GRAVITY = 0.0005f;
                    GuiSFX::Projectile( m_cardText,
                                        m_elapsedTime,
                                        CARD_TRANSITION_IN_TIME,
                                        rmt::Vector( 0.0f, 0.0f, 0.0f ),
                                        rmt::Vector( 0.0f, 0.0f, 0.0f ),
                                        false,
                                        TEXT_GRAVITY );

                    // fade in card count
                    //
                    m_itemsCount->SetAlpha( percentageDone );
/*
                    // flash card text
                    //
                    GuiSFX::Flash( m_cardGet, m_elapsedTime, CARD_TRANSITION_IN_TIME, 1, 1.1f, 1.0f );
                    GuiSFX::Flash( m_cardTitle, m_elapsedTime, CARD_TRANSITION_IN_TIME, 1, 1.1f, 1.0f );
*/
                }
                else
                {
                    m_cardImage->SetAlpha( 1.0f );
                    m_itemsCount->SetAlpha( 1.0f );
#ifdef RAD_WIN32
                    m_cardImage->ScaleAboutCenter( HUDCARD_THUMBNAIL_SCALE );
#endif

                    // advance to next sub state
                    //
                    m_elapsedTime = 0.0f;
                    m_currentSubState++;
                }

                break;
            }
            case STATE_CARD_DISPLAY_HOLD:
            {
                const float CARD_DISPLAY_HOLD_TIME = 2000.0f;

                if( m_elapsedTime < CARD_DISPLAY_HOLD_TIME )
                {
                    // do nothing
                }
                else
                {
                    // advance to next sub state
                    //
                    m_elapsedTime = 0.0f;
                    m_currentSubState++;
                }

                break;
            }
            case STATE_CARD_TRANSITION_OUT:
            {
                const float CARD_TRANSITION_OUT_TIME = 300.0f;

                m_itemsComplete->ResetTransformation();

                if( m_elapsedTime < CARD_TRANSITION_OUT_TIME )
                {
                    float percentageDone = m_elapsedTime / CARD_TRANSITION_OUT_TIME;

                    // slide out card image
                    //
                    GuiSFX::SlideY( m_cardImage,
                                    m_elapsedTime,
                                    CARD_TRANSITION_OUT_TIME,
                                    false,
                                    GuiSFX::SLIDE_BORDER_TOP );

                    // slide out card count
                    //
                    GuiSFX::SlideY( m_itemsCount,
                                    m_elapsedTime,
                                    CARD_TRANSITION_OUT_TIME,
                                    false,
                                    GuiSFX::SLIDE_BORDER_TOP,
                                    100 );

                    // fade out card text (and scale up)
                    //
                    rAssert( m_cardText != NULL );
                    m_cardText->SetAlpha( 1.0f - percentageDone );

                    m_cardText->ResetTransformation();
                    m_cardText->ScaleAboutCenter( 1.0f + percentageDone * 0.5f );

#ifdef RAD_WIN32
                    m_cardImage->ScaleAboutCenter( HUDCARD_THUMBNAIL_SCALE );
#endif

                    if( m_isCardDeckComplete )
                    {
                        m_itemsComplete->SetVisible( true );

                        // scale up card deck complete text
                        //
                        m_itemsComplete->ScaleAboutCenter( percentageDone );

                        // apply projectile motion effect to card deck complete text
                        //
                        const float TEXT_GRAVITY = 0.005f;
                        GuiSFX::Projectile( m_itemsComplete,
                                            m_elapsedTime,
                                            CARD_TRANSITION_OUT_TIME,
                                            rmt::Vector( 0.0f, 0.0f, 0.0f ),
                                            rmt::Vector( 0.0f, 0.0f, 0.0f ),
                                            false,
                                            TEXT_GRAVITY );
                    }
                }
                else
                {
                    m_cardText->SetAlpha( 0.0f );

                    // advance to next sub state
                    //
                    m_elapsedTime = 0.0f;
                    m_currentSubState++;

                    if( !m_isCardDeckComplete )
                    {
                        // skip to final sub state
                        //
                        m_currentSubState = NUM_SUB_STATES;
                    }
                }

                break;
            }
            case STATE_CARD_DECK_COMPLETE_DISPLAY_HOLD:
            {
                const float CARD_DISPLAY_HOLD_TIME = 2000.0f;

                if( m_elapsedTime < CARD_DISPLAY_HOLD_TIME )
                {
                    // do nothing
                }
                else
                {
                    // advance to next sub state
                    //
                    m_elapsedTime = 0.0f;
                    m_currentSubState++;
                }

                break;
            }
            case STATE_CARD_DECK_COMPLETE_TRANSITION_OUT:
            {
                const float CARD_TRANSITION_OUT_TIME = 300.0f;

                if( m_elapsedTime < CARD_TRANSITION_OUT_TIME )
                {
                    float percentageDone = m_elapsedTime / CARD_TRANSITION_OUT_TIME;

                    // fade out card text (and scale up)
                    //
                    rAssert( m_itemsComplete != NULL );
                    m_itemsComplete->SetAlpha( 1.0f - percentageDone );

                    m_itemsComplete->ResetTransformation();
                    m_itemsComplete->ScaleAboutCenter( 1.0f + percentageDone * 0.5f );
                }
                else
                {
                    // advance to next sub state
                    //
                    m_elapsedTime = 0.0f;
                    m_currentSubState++;
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
HudCardCollected::SetCurrentCard( unsigned int cardID )
{
    rAssert( m_cardImage != NULL );
    m_cardImage->SetIndex( cardID );

    rAssert( m_cardTitle != NULL );
    m_cardTitle->SetIndex( cardID );
}

void
HudCardCollected::SetCardCount( unsigned int numCollected,
                                unsigned int numCollectibles )
{
    char buffer[ BITMAP_TEXT_BUFFER_SIZE ];
    sprintf( buffer, "%d/%d", numCollected, numCollectibles );

    rAssert( m_numCards != NULL );
    m_numCards->SetBitmapText( buffer );

#ifndef RAD_DEMO
    // set card deck complete flag
    //
    m_isCardDeckComplete = (numCollected == numCollectibles);

    if( m_isCardDeckComplete &&
        GetCardGallery()->GetNumCardDecksCompleted() == RenderEnums::numLevels )
    {
        // all card decks completed
        //
        rAssert( m_cardDeckComplete != NULL );
        m_cardDeckComplete->SetIndex( CARD_DECK_COMPLETE );

        rAssert( m_itemsUnlocked != NULL );
        m_itemsUnlocked->SetIndex( UNLOCKED_IS_MOVIE );
    }
#endif // !RAD_DEMO
}

