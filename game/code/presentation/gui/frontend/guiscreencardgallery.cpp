//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenCardGallery
//
// Description: Implementation of the CGuiScreenCardGallery class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/21      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/frontend/guiscreencardgallery.h>
#include <presentation/gui/frontend/guiscreenscrapbookcontents.h>
#include <presentation/gui/utility/specialfx.h>
#include <presentation/gui/utility/scrollingtext.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guimenu.h>

#include <cards/card.h>
#include <cards/cardsdb.h>
#include <events/eventmanager.h>
#include <memory/srrmemory.h>

#include <raddebug.hpp> // Foundation

#include <screen.h>
#include <page.h>
#include <layer.h>
#include <group.h>
#include <sprite.h>
#include <text.h>
#include <pure3dobject.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const unsigned int NUM_CARD_ROWS = 2;
const unsigned int NUM_CARD_COLUMNS = 4;
const float CARD_TRANSITION_TIME = 250.0f; // in msec
const float CARD_PROJECTILE_GRAVITY = 0.005f; // in m/ms/ms

#ifdef RAD_WIN32
const float CARD_THUMBNAIL_SCALE = 0.44f;
const float CARD_DESCRIPTION_TEXT_SCALE = 0.9f;
const float CARD_QUESTION_CORRECTION_SCALE = 2.0f / 3.0f;
const float CARD_GREY_CORRECTION_SCALE = 2.0f / 3.0f;
#else
const float CARD_THUMBNAIL_SCALE = 0.44f;
const float CARD_DESCRIPTION_TEXT_SCALE = 0.9f;
const float CARD_QUESTION_CORRECTION_SCALE = 2.0f;
const float CARD_GREY_CORRECTION_SCALE = 2.0f;
#endif

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenCardGallery::CGuiScreenCardGallery
//===========================================================================
// Description: Constructor.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
CGuiScreenCardGallery::CGuiScreenCardGallery
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent,
    eGuiWindowID windowID
)
:   CGuiScreen( pScreen, pParent, windowID ),
    m_cardGalleryState( STATE_BROWSING_CARDS ),
    m_cardScaleSmall( CARD_THUMBNAIL_SCALE ),
    m_cardScaleLarge( 1.0f ),
    m_pMenu( NULL ),
    m_viewCard( NULL ),
    m_viewCardDistX( 0 ),
    m_viewCardDistY( 0 ),
    m_cardBrowseLayer( NULL ),
    m_cardViewLayer( NULL ),
    m_cardSFXLayer( NULL ),
    m_elapsedTime( 0 ),
    m_currentCard( NULL ),
    m_cardTitle( NULL ),
    m_cardEpisode( NULL ),
    m_cardDescription( NULL ),
    m_currentQuote( -1 )
{
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "CardGallery" );
    if( windowID == GUI_SCREEN_ID_VIEW_CARDS )
    {
        pPage = m_pScroobyScreen->GetPage( "PauseViewCards" );
    }
    rAssert( pPage != NULL );

    char cardName[ 8 ];

    // create a 2D sprite menu
    //
    m_pMenu = new CGuiMenu2D( this, NUM_CARDS_PER_LEVEL, 4, GUI_SPRITE_MENU, MENU_SFX_NONE );
    rAssert( m_pMenu != NULL );
    m_pMenu->SetGreyOutEnabled( false );

    Scrooby::Group* pGroup = pPage->GetGroup( "CollectedCards" );
    rAssert( pGroup != NULL );

    // add sprites to menu
    //
    for( unsigned int i = 0; i < NUM_CARDS_PER_LEVEL; i++ )
    {
        sprintf( cardName, "Card%d", i );
        m_pMenu->AddMenuItem( pGroup->GetSprite( cardName ) );
    }

    // add menu cursor
    //
    m_pMenu->SetCursor( pGroup->GetSprite( "CardFrame" ) );

    m_cardBrowseLayer = pPage->GetLayer( "Foreground" );
    rAssert( m_cardBrowseLayer );

    // from ViewCard page
    //
	pPage = m_pScroobyScreen->GetPage( "ViewCard" );
	rAssert( pPage != NULL );

    this->AutoScaleFrame( pPage );

    for( unsigned int j = 0; j < NUM_CARDS_PER_LEVEL; j++ )
    {
        sprintf( cardName, "Card%d", j );
        Scrooby::Sprite* greyCard = pPage->GetSprite( cardName );
        if( greyCard != NULL )
        {
            greyCard->ResetTransformation();
            greyCard->ScaleAboutCenter( CARD_GREY_CORRECTION_SCALE );
        }
    }

    m_cardViewLayer = pPage->GetLayer( "ViewCard" );
    rAssert( m_cardViewLayer );
    m_cardViewLayer->SetVisible( false );

    m_cardTitle = pPage->GetText( "CardTitle" );
    rAssert( m_cardTitle );
    m_cardTitle->SetTextMode( Scrooby::TEXT_WRAP );

    m_cardEpisode = pPage->GetText( "CardEpisode" );
    rAssert( m_cardEpisode );
    m_cardEpisode->SetTextMode( Scrooby::TEXT_WRAP );

    m_cardDescription = pPage->GetText( "CardDescription" );
    rAssert( m_cardDescription );
    m_cardDescription->SetTextMode( Scrooby::TEXT_WRAP );
    m_cardDescription->ScaleAboutPoint( CARD_DESCRIPTION_TEXT_SCALE, 0, 0 );

    for( unsigned int i = 0; i < MAX_NUM_QUOTES; i++ )
    {
        char name[ 32 ];
        sprintf( name, "Quote%d", i );

        m_quoteIcon[ i ] = pPage->GetSprite( name );
        rAssert( m_quoteIcon[ i ] );

        m_quote[ i ] = new( GMA_LEVEL_FE ) ScrollingText( pPage->GetText( name ) );
        rAssert( m_quote[ i ] );
    }

    if( windowID == GUI_SCREEN_ID_CARD_GALLERY )
    {
        // from HighResCard page
        //
	    pPage = m_pScroobyScreen->GetPage( "HighResCard" );
    }
    else
    {
        // from LowResCard page
        //
	    pPage = m_pScroobyScreen->GetPage( "LowResCard" );
    }

    rAssert( pPage != NULL );
    Scrooby::Layer* cardLayer = pPage->GetLayer( "Card" );
    rAssert( cardLayer != NULL );

    m_viewCard = cardLayer->GetSprite( "Card" );
    rAssert( m_viewCard );
    m_viewCard->SetVisible( false );

    m_cardSFXLayer = pPage->GetLayer( "SFX" );
    if( m_cardSFXLayer != NULL )
    {
        m_cardSFXLayer->SetVisible( false );
    }

    if( this->IsWideScreenDisplay() )
    {
        if( m_cardSFXLayer != NULL )
        {
            Scrooby::Pure3dObject* cardSparkle = m_cardSFXLayer->GetPure3dObject( "CardSparkle" );
            if( cardSparkle != NULL )
            {
                cardSparkle->SetWideScreenCorrectionEnabled( true );
            }
        }

        cardLayer->ResetTransformation();
        this->ApplyWideScreenCorrectionScale( cardLayer );

        m_cardViewLayer->ResetTransformation();
        this->ApplyWideScreenCorrectionScale( m_cardViewLayer );
    }
}


//===========================================================================
// CGuiScreenCardGallery::~CGuiScreenCardGallery
//===========================================================================
// Description: Destructor.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
CGuiScreenCardGallery::~CGuiScreenCardGallery()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }

    for( unsigned int i = 0; i < MAX_NUM_QUOTES; i++ )
    {
        if( m_quote[ i ] != NULL )
        {
            delete m_quote[ i ];
            m_quote[ i ] = NULL;
        }
    }
}


//===========================================================================
// CGuiScreenCardGallery::HandleMessage
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenCardGallery::HandleMessage
(
	eGuiMessage message,
	unsigned int param1,
	unsigned int param2
)
{
    if( m_cardGalleryState != STATE_BROWSING_CARDS )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                if( m_cardGalleryState == STATE_GOTO_VIEW_CARD )
                {
                    this->UpdateCardTransition( param1, true );
                }
                else if( m_cardGalleryState == STATE_BACK_VIEW_CARD )
                {
                    this->UpdateCardTransition( param1, false );
                }
                else
                {
                    rAssert( m_cardGalleryState == STATE_VIEWING_CARD );
                    this->UpdateViewCard( param1 );
                }

                break;
            }

            case GUI_MSG_CONTROLLER_BACK:
            {
                if( m_cardGalleryState == STATE_VIEWING_CARD )
                {
                    if( m_cardSFXLayer != NULL )
                    {
                        m_cardSFXLayer->SetVisible( false );
                    }

                    m_cardGalleryState = STATE_BACK_VIEW_CARD;
                    m_elapsedTime = 0;

                    GetEventManager()->TriggerEvent( EVENT_FE_MENU_BACK ); // sound effect
                }

                break;
            }
            
            default:
            {
                break;
            }
        }

        return;
    }

    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                // pulse cursor alpha
                //
                Scrooby::Drawable* cursor = m_pMenu->GetCursor();
                if( cursor != NULL )
                {
                    const unsigned int PULSE_PERIOD = 1000;

                    float alpha = GuiSFX::Pulse( (float)m_elapsedTime,
                                                 (float)PULSE_PERIOD,
                                                 0.75f,
                                                 0.25f,
                                                 -rmt::PI_BY2 );

                    cursor->SetAlpha( alpha );

                    m_elapsedTime += param1;
                    m_elapsedTime %= PULSE_PERIOD;
                }

                break;
            }
/*
            case GUI_MSG_CONTROLLER_LEFT:
            {
                int newSelection = m_currentSelection - 1;
                if( newSelection < 0 )
                {
                    newSelection += NUM_CARDS_PER_LEVEL;
                }

                this->MoveCursor( m_currentSelection, newSelection );
                m_currentSelection = newSelection;

                break;
            }

            case GUI_MSG_CONTROLLER_RIGHT:
            {
                int newSelection = (m_currentSelection + 1) % NUM_CARDS_PER_LEVEL;
                this->MoveCursor( m_currentSelection, newSelection );
                m_currentSelection = newSelection;

                break;
            }

            case GUI_MSG_CONTROLLER_UP:
            {
                int newSelection = m_currentSelection - NUM_CARD_COLUMNS;
                if( newSelection < 0 )
                {
                    newSelection += NUM_CARDS_PER_LEVEL;
                }

                this->MoveCursor( m_currentSelection, newSelection );
                m_currentSelection = newSelection;

                break;
            }

            case GUI_MSG_CONTROLLER_DOWN:
            {
                int newSelection = (m_currentSelection + NUM_CARD_COLUMNS) % NUM_CARDS_PER_LEVEL;

                this->MoveCursor( m_currentSelection, newSelection );
                m_currentSelection = newSelection;

                break;
            }
*/
            case GUI_MSG_MENU_SELECTION_MADE:
            {
                Scrooby::Sprite* currentCardSelection = dynamic_cast<Scrooby::Sprite*>( m_pMenu->GetMenuItem( param1 )->GetItem() );
                rAssert( currentCardSelection != NULL );

                int cardID = currentCardSelection->GetIndex() - 1;
                if( cardID != -1 )
                {
                    this->SetCurrentViewCard( cardID );

                    int smallPosX, smallPosY;
                    currentCardSelection->GetOriginPosition( smallPosX, smallPosY );

                    int bigPosX, bigPosY;
                    m_viewCard->GetOriginPosition( bigPosX, bigPosY );

                    m_viewCardDistX = smallPosX - bigPosX;
                    m_viewCardDistY = smallPosY - bigPosY;

                    m_cardVelocity.x = m_viewCardDistX / CARD_TRANSITION_TIME;
                    m_cardVelocity.y = (m_viewCardDistY - 0.5f * CARD_PROJECTILE_GRAVITY * CARD_TRANSITION_TIME * CARD_TRANSITION_TIME) / CARD_TRANSITION_TIME;
                    m_cardVelocity.z = 0.0f;

                    this->SetButtonVisible( BUTTON_ICON_ACCEPT, false );

                    // hide level bar
                    //
                    CGuiScreenScrapBookContents* scrapBookContents = static_cast<CGuiScreenScrapBookContents*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS ) );
                    if( scrapBookContents != NULL )
                    {
                        scrapBookContents->SetLevelBarVisible( false );
                    }

                    m_viewCard->SetVisible( true );
                    m_cardViewLayer->SetVisible( true );
                    m_cardGalleryState = STATE_GOTO_VIEW_CARD;
                    m_elapsedTime = 0;

                    this->UpdateCardTransition( 0, true );
                }

                break;
            }

            default:
            {
                break;
            }
        }

        // relay message to menu
        //
        if( m_pMenu != NULL )
        {
            m_pMenu->HandleMessage( message, param1, param2 );
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenCardGallery::InitIntro
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenCardGallery::InitIntro()
{
    CGuiScreenScrapBookContents* scrapBookContents = static_cast<CGuiScreenScrapBookContents*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS ) );
    rAssert( scrapBookContents != NULL );
    this->UpdateCards( scrapBookContents->GetCurrentLevel() );
}


//===========================================================================
// CGuiScreenCardGallery::InitRunning
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenCardGallery::InitRunning()
{
}


//===========================================================================
// CGuiScreenCardGallery::InitOutro
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenCardGallery::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

/*
void
CGuiScreenCardGallery::MoveCursor( unsigned int from,
                                        unsigned int to )
{
    if( m_cursor != NULL )
    {
        rAssert( from < NUM_CARDS_PER_LEVEL && to < NUM_CARDS_PER_LEVEL );

        int posX = 0;
        int posY = 0;
        m_collectedCards[ to ]->GetOriginPosition( posX, posY );

        m_cursor->SetPosition( posX, posY );
    }

    // turn on/off 'accept' button
    //
    bool isSelectable = (m_collectedCards[ to ]->GetIndex() != 0);
    this->SetButtonVisible( BUTTON_ICON_ACCEPT, isSelectable );
}
*/

void
CGuiScreenCardGallery::UpdateCards( unsigned int currentLevel )
{
    // update the cards
    //
    const CardList* collectedCards = GetCardGallery()->GetCollectedCards( currentLevel );
    rAssert( collectedCards != NULL );
    rAssert( m_pMenu != NULL );

    // hide accept button by default (unless there is at least one collected card to view)
    //
    this->SetButtonVisible( BUTTON_ICON_ACCEPT, false );

    for( unsigned int i = 0; i < NUM_CARDS_PER_LEVEL; i++ )
    {
        Scrooby::Sprite* cardSelection = dynamic_cast<Scrooby::Sprite*>( m_pMenu->GetMenuItem( i )->GetItem() );
        rAssert( cardSelection != NULL );
        cardSelection->ResetTransformation();

        bool isCardCollected = ( collectedCards->m_cards[ i ] != NULL );
        m_pMenu->SetMenuItemEnabled( i, isCardCollected );

        if( isCardCollected )
        {
            unsigned int cardID = collectedCards->m_cards[ i ]->GetID();
            rAssert( cardID < static_cast<unsigned int>( cardSelection->GetNumOfImages() ) );
            cardSelection->SetIndex( cardID + 1 );
            cardSelection->ScaleAboutCenter( m_cardScaleSmall );

//            m_pMenu->Reset();
            this->SetButtonVisible( BUTTON_ICON_ACCEPT, true );
        }
        else
        {
            cardSelection->SetIndex( 0 );
            cardSelection->ScaleAboutCenter( CARD_QUESTION_CORRECTION_SCALE );
        }
    }
}

void
CGuiScreenCardGallery::UpdateCardTransition( unsigned int elapsedTime, bool toViewCard )
{
    rAssert( m_viewCard );
    m_viewCard->ResetTransformation();

    if( m_elapsedTime < CARD_TRANSITION_TIME )
    {
        float t = toViewCard ? (float)m_elapsedTime / CARD_TRANSITION_TIME :
                               1.0f - (float)m_elapsedTime / CARD_TRANSITION_TIME;

        // scale card
        //
        float scale = t * (m_cardScaleLarge - m_cardScaleSmall) + m_cardScaleSmall;
        m_viewCard->ScaleAboutCenter( scale );

/*
        // translate card
        //
        int translateX = static_cast<int>( (1.0f - t) * m_viewCardDistX );
        int translateY = static_cast<int>( (1.0f - t) * m_viewCardDistY );
        m_viewCard->Translate( translateX, translateY );
*/

        GuiSFX::Projectile( m_viewCard,
                            (float)m_elapsedTime,
                            CARD_TRANSITION_TIME,
                            m_cardVelocity,
                            toViewCard,
                            CARD_PROJECTILE_GRAVITY );

        // fade in/out foreground layers (non-linearly)
        //
        m_cardBrowseLayer->SetAlpha( (1.0f - t) * (1.0f - t) );
        m_cardViewLayer->SetAlpha( t * t );

        m_elapsedTime += elapsedTime;
    }
    else
    {
        // transition completed
        //
        if( toViewCard )
        {
            m_viewCard->ScaleAboutCenter( m_cardScaleLarge );

            m_cardGalleryState = STATE_VIEWING_CARD;

            m_cardBrowseLayer->SetAlpha( 0.0f );
            m_cardViewLayer->SetAlpha( 1.0f );

            if( m_cardSFXLayer != NULL )
            {
                m_cardSFXLayer->SetVisible( true );
            }
        }
        else
        {
            m_viewCard->ScaleAboutCenter( m_cardScaleSmall );

            m_cardViewLayer->SetVisible( false );
            m_viewCard->SetVisible( false );
            m_cardGalleryState = STATE_BROWSING_CARDS;

            m_cardBrowseLayer->SetAlpha( 1.0f );
            m_cardViewLayer->SetAlpha( 0.0f );

            this->SetButtonVisible( BUTTON_ICON_ACCEPT, true );

            // show level bar
            //
            CGuiScreenScrapBookContents* scrapBookContents = static_cast<CGuiScreenScrapBookContents*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS ) );
            if( scrapBookContents != NULL )
            {
                scrapBookContents->SetLevelBarVisible( true );
            }
        }
    }
}

void
CGuiScreenCardGallery::UpdateViewCard( unsigned int elapsedTime )
{
    rAssert( m_currentCard );

    if( m_currentQuote != -1 )
    {
        // update current scrolling quote
        //
        rAssert( m_quote[ m_currentQuote ] );
        m_quote[ m_currentQuote ]->Update( elapsedTime );

        if( m_quote[ m_currentQuote ]->GetCurrentState() == ScrollingText::STATE_IDLE )
        {
            // turn off current quote icon
            //
            m_quoteIcon[ m_currentQuote ]->SetVisible( false );

            // start next quote (and wrap to first one)
            //
            m_currentQuote = (m_currentQuote + 1) % m_currentCard->GetNumQuotes();
            m_quote[ m_currentQuote ]->Start();

            // turn on new quote icon
            //
            m_quoteIcon[ m_currentQuote ]->SetVisible( true );
        }
    }
}

void
CGuiScreenCardGallery::SetCurrentViewCard( unsigned int cardID )
{
    // set current card reference
    //
    m_currentCard = GetCardGallery()->GetCardsDB()->GetCardByID( cardID );
    rAssert( m_currentCard );

    // set card image
    //
    rAssert( m_viewCard );
    rAssert( cardID < static_cast<unsigned int>( m_viewCard->GetNumOfImages() ) );
    m_viewCard->SetIndex( cardID );

    // set card title
    //
    rAssert( m_cardTitle );
    rAssert( cardID < static_cast<unsigned int>( m_cardTitle->GetNumOfStrings() ) );
    m_cardTitle->SetIndex( cardID );

    // set card episode
    //
    rAssert( m_cardEpisode );
    rAssert( cardID < static_cast<unsigned int>( m_cardEpisode->GetNumOfStrings() ) );
    m_cardEpisode->SetIndex( cardID );

    // set card description
    //
    rAssert( m_cardDescription );
    rAssert( cardID < static_cast<unsigned int>( m_cardDescription->GetNumOfStrings() ) );
    m_cardDescription->SetIndex( cardID );

    // stop current quote
    //
    if( m_currentQuote != -1 )
    {
        m_quote[ m_currentQuote ]->Stop();
    }

    // set card quotes
    //
    for( unsigned int i = 0; i < MAX_NUM_QUOTES; i++ )
    {
        m_quoteIcon[ i ]->SetVisible( false );

        eQuoteID quoteID = m_currentCard->GetQuoteID( i );
        if( quoteID != EMPTY_QUOTE )
        {
            m_quoteIcon[ i ]->SetIndex( static_cast<int>( quoteID ) );
        }

        m_quote[ i ]->SetTextIndex( cardID );
    }

    if( m_currentCard->GetNumQuotes() > 0 )
    {
        // start scrolling first quote
        //
        m_currentQuote = 0;
        m_quote[ m_currentQuote ]->Start();
        m_quoteIcon[ m_currentQuote ]->SetVisible( true );
    }
    else
    {
        m_currentQuote = -1;
    }
}

