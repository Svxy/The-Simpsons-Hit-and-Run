//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   HudHitNRun
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
#include <presentation/gui/ingame/hudevents/hudhitnrun.h>
#include <presentation/gui/utility/specialfx.h>

// Scrooby
#include <app.h>
#include <group.h>
#include <page.h>
#include <text.h>
#include <sprite.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

#ifdef RAD_WIN32
const float HNR_MESSAGE_SCALE = 1.75f;
const float HNR_MESSAGE_BUSTED_SCALE = 2.0f;
const float HNR_TICKET_SCALE = 1.0f;
const float HNR_MESSAGE_BUSTED_TILT_ANGLE = -10.0f; // in degrees
const tColour HNR_MESSAGE_BUSTED_COLOUR( 43, 89, 249 );
#else
const float HNR_MESSAGE_SCALE = 1.75f;
const float HNR_MESSAGE_BUSTED_SCALE = 2.0f;
const float HNR_TICKET_SCALE = 2.0f;
const float HNR_MESSAGE_BUSTED_TILT_ANGLE = -10.0f; // in degrees
const tColour HNR_MESSAGE_BUSTED_COLOUR( 43, 89, 249 );
#endif

//===========================================================================
// Public Member Functions
//===========================================================================

HudHitNRun::HudHitNRun( Scrooby::Page* pPage )
:   HudEventHandler( pPage->GetGroup( "HitNRun" ) ),
    m_currentSubState( 0 ),
    m_currentMessage( MSG_HIT_N_RUN ),
    m_hnrMessage( NULL ),
    m_hnrTicket( NULL ),
    m_messageTranslateX( 0 ),
    m_messageTranslateY( 0 ),
    m_defaultMessageColour( 100, 0, 0 )
{
    rAssert( pPage != NULL );

    Scrooby::Group* pGroup = pPage->GetGroup( "HitNRun" );
    rAssert( pGroup != NULL );

    m_hnrMessage = pGroup->GetText( "HitNRun" );
    rAssert( m_hnrMessage != NULL );
    m_hnrMessage->SetTextMode( Scrooby::TEXT_WRAP );
    m_hnrMessage->SetDisplayOutline( true );

//    m_defaultMessageColour = m_hnrMessage->GetColour();

    m_hnrTicket = pGroup->GetSprite( "HitNRunTicket" );

    int messageCenterX = 0;
    int messageCenterY = 0;
    m_hnrMessage->GetBoundingBoxCenter( messageCenterX, messageCenterY );

    Scrooby::Sprite* radar = pPage->GetSprite( "Radar0" );
    rAssert( radar != NULL );
    radar->GetBoundingBoxCenter( m_messageTranslateX, m_messageTranslateY );

    m_messageTranslateX -= messageCenterX;
    m_messageTranslateY -= messageCenterY;
}

HudHitNRun::~HudHitNRun()
{
}

void
HudHitNRun::Start()
{
    if( m_currentState != STATE_RUNNING )
    {
        m_currentSubState = STATE_MESSAGE_TRANSITION_IN;

        rAssert( m_hnrMessage != NULL );
        m_hnrMessage->SetIndex( m_currentMessage );
        m_hnrMessage->SetAlpha( 0.0f );
        m_hnrMessage->SetColour( m_currentMessage == MSG_BUSTED ?
                                 HNR_MESSAGE_BUSTED_COLOUR :
                                 m_defaultMessageColour ); // restore default colour

        rAssert( m_hnrTicket != NULL );
        m_hnrTicket->SetAlpha( 0.0f );
        m_hnrTicket->SetVisible( m_currentMessage == MSG_BUSTED );
    }

    this->OnStart();
}

void
HudHitNRun::Stop()
{
    this->OnStop();
}

void
HudHitNRun::Update( float elapsedTime )
{
    if( m_currentState == STATE_RUNNING )
    {
        m_elapsedTime += elapsedTime;

        switch( m_currentSubState )
        {
            case STATE_MESSAGE_TRANSITION_IN:
            {
                const float MESSAGE_TRANSITION_TIME = 200.0f;

                rAssert( m_hnrMessage != NULL );
                m_hnrMessage->ResetTransformation();

                rAssert( m_hnrTicket != NULL );
                m_hnrTicket->ResetTransformation();

                if( m_elapsedTime < MESSAGE_TRANSITION_TIME )
                {
                    float percentageDone = m_elapsedTime / MESSAGE_TRANSITION_TIME;

                    m_hnrMessage->SetAlpha( percentageDone );

                    if( m_hnrMessage->GetIndex() == MSG_HIT_N_RUN )
                    {
                        m_hnrMessage->ScaleAboutCenter( percentageDone * HNR_MESSAGE_SCALE );
                    }
                    else
                    {
                        m_hnrMessage->ScaleAboutCenter( percentageDone * HNR_MESSAGE_BUSTED_SCALE );
                        m_hnrMessage->RotateAboutCenter( percentageDone * HNR_MESSAGE_BUSTED_TILT_ANGLE );

                        m_hnrTicket->SetAlpha( percentageDone );
                        m_hnrTicket->ScaleAboutCenter( percentageDone * HNR_TICKET_SCALE );

                        GuiSFX::Projectile( m_hnrTicket,
                                            m_elapsedTime,
                                            MESSAGE_TRANSITION_TIME,
                                            rmt::Vector( 0, 0, 0 ),
                                            rmt::Vector( (float)m_messageTranslateX, (float)m_messageTranslateY, 0 ),
                                            true,
                                            0.0f );
                    }

                    GuiSFX::Projectile( m_hnrMessage,
                                        m_elapsedTime,
                                        MESSAGE_TRANSITION_TIME,
                                        rmt::Vector( 0, 0, 0 ),
                                        rmt::Vector( (float)m_messageTranslateX, (float)m_messageTranslateY, 0 ),
                                        true,
                                        0.0f );
                }
                else
                {
                    m_hnrMessage->SetAlpha( 1.0f );

                    if( m_hnrMessage->GetIndex() == MSG_HIT_N_RUN )
                    {
                        m_hnrMessage->ScaleAboutCenter( HNR_MESSAGE_SCALE );
                    }
                    else
                    {
                        m_hnrMessage->ScaleAboutCenter( HNR_MESSAGE_BUSTED_SCALE );
                        m_hnrMessage->RotateAboutCenter( HNR_MESSAGE_BUSTED_TILT_ANGLE );

                        m_hnrTicket->SetAlpha( 1.0f );
                        m_hnrTicket->ScaleAboutCenter( HNR_TICKET_SCALE );
                    }

                    // advance to next sub state
                    //
                    m_elapsedTime = 0.0f;
                    m_currentSubState++;
                }

                break;
            }
            case STATE_MESSAGE_DISPLAY_HOLD:
            {
                const float MESSAGE_DISPLAY_HOLD_TIME = 1200.0f;

                rAssert( m_hnrMessage != NULL );
                if( m_elapsedTime < MESSAGE_DISPLAY_HOLD_TIME )
                {
                    if( m_hnrMessage->GetIndex() == MSG_HIT_N_RUN )
                    {
                        static tColour MESSAGE_OTHER_COLOUR( 240, 8, 8 );

                        tColour currentColour;
                        GuiSFX::ModulateColour( &currentColour,
                                                m_elapsedTime,
                                                333.3f,
                                                m_defaultMessageColour,
                                                MESSAGE_OTHER_COLOUR,
                                                rmt::PI_BY2 );

                        m_hnrMessage->SetColour( currentColour );
                    }
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
            case STATE_MESSAGE_TRANSITION_OUT:
            {
                const float MESSAGE_TRANSITION_TIME = 200.0f;

                rAssert( m_hnrMessage != NULL );
                m_hnrMessage->ResetTransformation();

                rAssert( m_hnrTicket != NULL );
                m_hnrTicket->ResetTransformation();

                if( m_elapsedTime < MESSAGE_TRANSITION_TIME )
                {
                    float percentageDone = m_elapsedTime / MESSAGE_TRANSITION_TIME;

                    if( m_hnrMessage->GetIndex() == MSG_HIT_N_RUN )
                    {
                        m_hnrMessage->SetAlpha( 1.0f - percentageDone );
                        m_hnrMessage->ScaleAboutCenter( (1.0f - percentageDone) * HNR_MESSAGE_SCALE );

                        GuiSFX::Projectile( m_hnrMessage,
                                            m_elapsedTime,
                                            MESSAGE_TRANSITION_TIME,
                                            rmt::Vector( 0, 0, 0 ),
                                            rmt::Vector( (float)m_messageTranslateX, (float)m_messageTranslateY, 0 ),
                                            false,
                                            0.0f );
                    }
                    else
                    {
                        m_hnrMessage->SetAlpha( 0.0f );

                        m_hnrTicket->ScaleAboutCenter( (percentageDone * 3 + 1.0f) * HNR_TICKET_SCALE );
                        m_hnrTicket->SetAlpha( 1.0f - percentageDone );
                    }
                }
                else
                {
                    m_hnrMessage->SetAlpha( 0.0f );
                    m_hnrTicket->SetAlpha( 0.0f );

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
HudHitNRun::SetMessage( eMessage message )
{
    m_currentMessage = message;
}

