//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMessage
//
// Description: Implementation of the CGuiScreenMessage class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/04      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/guiscreenmessage.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/utility/specialfx.h>
#include <presentation/gui/guiscreenmemorycard.h>
#include <presentation/gui/guitextbible.h>
#include <presentation/gui/ingame/guimanageringame.h>

#include <data/memcard/memorycardmanager.h> 
#include <gameflow/gameflow.h>
#include <memory/srrmemory.h>

#include <p3d/unicode.hpp>
#include <raddebug.hpp> // Foundation
#include <layer.h>
#include <page.h>
#include <screen.h>
#include <sprite.h>
#include <text.h>

#ifdef RAD_PS2
#include <libmtap.h>
#endif
//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

int CGuiScreenMessage::s_ControllerDisconnectedPort = 0;

int CGuiScreenMessage::s_messageIndex = -1;
CGuiEntity* CGuiScreenMessage::s_pMessageCallback = NULL;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenMessage::CGuiScreenMessage
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
CGuiScreenMessage::CGuiScreenMessage
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_GENERIC_MESSAGE ),
    m_pMenu( NULL ),
    m_messageText( NULL ),
    m_messageIcon( NULL ),
    m_elapsedTime( 0 )
{
    m_originalStringBuffer[ 0 ] = '\0';

MEMTRACK_PUSH_GROUP( "GUIScreenMessage" );
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "Message" );
    rAssert( pPage != NULL );

    Scrooby::Layer* pLayer = pPage->GetLayer( "Foreground" );
    rAssert( pLayer != NULL );

    m_messageText = pLayer->GetText( "Message" );
    rAssert( m_messageText );

    // wrap text message
    //
    m_messageText->SetTextMode( Scrooby::TEXT_WRAP );
    m_messageText->ResetTransformation();
    m_messageText->ScaleAboutCenter(0.9f);

    // Retrieve the Scrooby drawing elements (from MessageBox page).
    //
    pPage = m_pScroobyScreen->GetPage( "MessageBox" );
    rAssert( pPage != NULL );

    m_messageIcon = pPage->GetSprite( "ErrorIcon" );

    this->AutoScaleFrame( pPage );

	this->SetFadeTime( 0.0f );
MEMTRACK_POP_GROUP( "GUIScreenMessage" );
}


//===========================================================================
// CGuiScreenMessage::~CGuiScreenMessage
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
CGuiScreenMessage::~CGuiScreenMessage()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenMessage::HandleMessage
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
void CGuiScreenMessage::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                m_elapsedTime += param1;

                // pulse message icon
                //
                float scale = GuiSFX::Pulse( (float)m_elapsedTime,
                                             500.0f,
                                             1.0f * MESSAGE_ICON_CORRECTION_SCALE,
                                             0.1f * MESSAGE_ICON_CORRECTION_SCALE );

                rAssert( m_messageIcon );
                m_messageIcon->ResetTransformation();
                m_messageIcon->ScaleAboutCenter( scale );

                if( s_pMessageCallback != NULL )
                {
				    s_pMessageCallback->HandleMessage( GUI_MSG_MESSAGE_UPDATE, param1 );
                }

                break;
            }

            case GUI_MSG_CONTROLLER_BACK:
            {
                // ignore BACK inputs, thereby, not allowing users to back
                // out of prompt
                return;

                break;
            }
/*
            case GUI_MSG_MENU_SELECTION_MADE:
            {
                rAssert( s_pMessageCallback );
                s_pMessageCallback->HandleMessage( GUI_MSG_MENU_MESSAGE_RESPONSE,
                                                   param1, param2 );

                break;
            }
*/
            default:
            {
                break;
            }
        }

        // relay message to menu
        if( m_pMenu != NULL )
        {
            m_pMenu->HandleMessage( message, param1, param2 );
        }
    }
    else if( m_state == GUI_WINDOW_STATE_OUTRO )
    {
        if( m_numTransitionsPending < 0 )
        {
            // restore original string buffer
            //
            if( m_originalStringBuffer[ 0 ] != '\0' )
            {
                rAssert( m_messageText != NULL );
                UnicodeChar* stringBuffer = m_messageText->GetStringBuffer();
                rAssert( stringBuffer != NULL );

                p3d::UnicodeStrCpy( static_cast<P3D_UNICODE*>( m_originalStringBuffer ),
                                    static_cast<P3D_UNICODE*>( stringBuffer ),
                                    sizeof( m_originalStringBuffer ) );

                m_originalStringBuffer[ 0 ] = '\0';
            }
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}


void
CGuiScreenMessage::Display( int messageIndex, CGuiEntity* pCallback )
{
    s_messageIndex = messageIndex;
    s_pMessageCallback = pCallback;
}
void
CGuiScreenMessage::GetControllerDisconnectedMessage(int controller_id, char *str_buffer, int max_char)
{
    P3D_UNICODE* multitapString = NULL;

    s_ControllerDisconnectedPort = controller_id;

#ifdef RAD_GAMECUBE
    P3D_UNICODE* uni_string = GetTextBibleString( "MSG_CONTROLLER_DISCONNECTED_(GC)" );
#endif

#ifdef RAD_XBOX
    P3D_UNICODE* uni_string = GetTextBibleString( "MSG_CONTROLLER_DISCONNECTED_(XBOX)" );
#endif

#ifdef RAD_PS2
    P3D_UNICODE* uni_string = GetTextBibleString( "MSG_CONTROLLER_DISCONNECTED_(PS2)" );
    if (s_ControllerDisconnectedPort > 7)
        uni_string = GetTextBibleString( "MSG_CONTROLLER_DISCONNECTED_WHEEL" ); // usb
    int port = controller_id/4;
    if ( controller_id < 8 &&
         GetInputManager()->GetLastMultitapStatus( port ) != 0 &&
         sceMtapGetConnection( port ) == 0 )
    {
        uni_string = GetTextBibleString( "MSG_CONTROLLER_DISCONNECTED_MULTITAP" ); // multitap

        // convert the first controller port wildcard character to either 1 or 2
        //
    	const UnicodeChar CONTROLLER_PORT_WILDCARD_CHARACTER = 0xa5;

        HeapMgr()->PushHeap( GMA_TEMP );

        multitapString = new P3D_UNICODE[ max_char + 1 ];
        rAssert( multitapString != NULL );
        p3d::UnicodeStrCpy( uni_string, multitapString, max_char );
        for( int i = 0; multitapString[ i ] != '\0'; i++ )
        {
            if( multitapString[ i ] == CONTROLLER_PORT_WILDCARD_CHARACTER )
            {
                // found it!
                //
                multitapString[ i ] = '1' + port;
                break;
            }
        }

        uni_string = multitapString;

        HeapMgr()->PopHeap( GMA_TEMP );
    }
#endif

#ifdef RAD_WIN32
    P3D_UNICODE* uni_string = GetTextBibleString( "MSG_CONTROLLER_DISCONNECTED_(XBOX)" );
#endif

    CGuiScreenMessage::ConvertUnicodeToChar(str_buffer, uni_string, max_char);

    if( multitapString != NULL )
    {
        delete multitapString;
        multitapString = NULL;
    }
}
void
CGuiScreenMessage::ConvertUnicodeToChar(char *str, P3D_UNICODE* uni_str, int max_char)
{
    const UnicodeChar CONTROLLER_PORT_WILDCARD_CHARACTER = 0xa5;// since > 0x7f, '¥' wont work;
    const UnicodeChar NEWLINE_CHARACTER = 0x5c; //'\';
    int i = 0;
    while (*uni_str && i < max_char)
    {
        if (*uni_str==NEWLINE_CHARACTER)
            *str = '\n';
        else if (*uni_str==CONTROLLER_PORT_WILDCARD_CHARACTER)
        {
#ifdef RAD_PS2
            int port_number = s_ControllerDisconnectedPort/4;
            if (sceMtapGetConnection(port_number)==1 
                || GetInputManager()->GetLastMultitapStatus(port_number) == 1) { // is multitap

                UnicodeString unicodeString; // splice the mu name inside
                UnicodeString restofString;
                UnicodeString deviceName;

                *str++ = '1' + (s_ControllerDisconnectedPort/4);
                i++;
                if (i >= max_char) break;
                *str++ = '-';
                i++;
                if (i >= max_char) break;
                *str = 'A' + (s_ControllerDisconnectedPort%4);
            }
            else // no multitap
            {
                *str  = '1' + (s_ControllerDisconnectedPort/4);
                if (s_ControllerDisconnectedPort==8)
                    *str = '1';
                else if (s_ControllerDisconnectedPort==9)
                    *str = '2';
            }

#else
            *str = '1' + s_ControllerDisconnectedPort;
#endif
        }
        else
            *str = (char)*uni_str;
        
        
        i++;
        str++;
        uni_str++;
    }
    *str++ = 0;
}
void
CGuiScreenMessage::FormatMessage( Scrooby::Text* pText,
                                  UnicodeChar* originalStringBuffer,
                                  int stringBufferLength )
{
    const UnicodeChar SLOT_WILDCARD_CHARACTER = '$';
	const UnicodeChar BLOCKS_WILDCARD_CHARACTER = '#';
	const UnicodeChar CONTROLLER_PORT_WILDCARD_CHARACTER = 0xa5;// since > 0x7f, '¥' wont work;

    rAssert( pText != NULL );
    UnicodeChar* stringBuffer = pText->GetStringBuffer();
    rAssert( stringBuffer != NULL );

    // save copy of original string buffer
    //
    if( originalStringBuffer != NULL )
    {
        p3d::UnicodeStrCpy( static_cast<P3D_UNICODE*>( stringBuffer ),
                            static_cast<P3D_UNICODE*>( originalStringBuffer ),
                            stringBufferLength );
    }
 
	for( int i = 0; stringBuffer[ i ] != '\0'; i++ )
	{
		// search for slot wildcard character
		//
		if( stringBuffer[ i ] == CONTROLLER_PORT_WILDCARD_CHARACTER )
		{
#ifdef RAD_PS2
            int port_number = s_ControllerDisconnectedPort/4;
            if (sceMtapGetConnection(port_number)==1) { // is multitap
                HeapMgr()->PushHeap( GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND ?
                                        GMA_LEVEL_FE : GMA_LEVEL_HUD );

                UnicodeString unicodeString; // splice the mu name inside
                UnicodeString restofString;
                UnicodeString deviceName;

                deviceName.Append('1' + (s_ControllerDisconnectedPort/4));
                deviceName.Append('-' );
                deviceName.Append('A' + (s_ControllerDisconnectedPort%4));

                unicodeString.ReadUnicode(stringBuffer, i);
                restofString.ReadUnicode(&stringBuffer[i+1]);		  
                unicodeString += deviceName;
                unicodeString += restofString;
                pText->SetString(pText->GetIndex(),unicodeString);

                HeapMgr()->PopHeap(GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND ?
                                    GMA_LEVEL_FE : GMA_LEVEL_HUD);
            }
            else // no multitap
                stringBuffer[ i ] = '1' + (s_ControllerDisconnectedPort/4);

#else
			stringBuffer[ i ] = '1' + s_ControllerDisconnectedPort;
#endif
			break;
		}
	}
// update slot wild card character---------------------

#ifdef RAD_GAMECUBE
    for( int i = 0; stringBuffer[ i ] != '\0'; i++ )
    {
        // search for slot wildcard character
        //
        if( stringBuffer[ i ] == SLOT_WILDCARD_CHARACTER )
        {
            stringBuffer[ i ] = 'A' + CGuiScreenMemoryCard::s_currentMemoryCardSlot;

            break;
        }
    }
#endif // RAD_GAMECUBE

#ifdef RAD_PS2
    for( int i = 0; stringBuffer[ i ] != '\0'; i++ )
    {
        // search for slot wildcard character
        //
        if( stringBuffer[ i ] == SLOT_WILDCARD_CHARACTER )
        {
            int port_number = CGuiScreenMemoryCard::s_currentMemoryCardSlot/4;
            if (sceMtapGetConnection(port_number)==1) { // is multitap

                HeapMgr()->PushHeap( GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND ?
                                        GMA_LEVEL_FE : GMA_LEVEL_HUD );

                UnicodeString unicodeString; // splice the mu name inside
                UnicodeString restofString;
                UnicodeString deviceName;

                deviceName.Append('1' + (CGuiScreenMemoryCard::s_currentMemoryCardSlot/4));
                deviceName.Append('-' );
                deviceName.Append('A' + (CGuiScreenMemoryCard::s_currentMemoryCardSlot%4));

                unicodeString.ReadUnicode(stringBuffer, i);
                restofString.ReadUnicode(&stringBuffer[i+1]);
                unicodeString += deviceName;
                unicodeString += restofString;
                pText->SetString(pText->GetIndex(),unicodeString);

                HeapMgr()->PopHeap(GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND ?
                                    GMA_LEVEL_FE : GMA_LEVEL_HUD);
            }
            else 
            {
                stringBuffer[ i ] = '1' + CGuiScreenMemoryCard::s_currentMemoryCardSlot/4;
            }
            break;
        }

    }
#endif // RAD_PS2

#ifdef RAD_XBOX
    for( int i = 0; stringBuffer[ i ] != '\0'; i++ )
    {
        // search for num blocks wildcard character
        //
        if( stringBuffer[ i ] == BLOCKS_WILDCARD_CHARACTER )
        {
            const IRadDrive::MediaInfo* mediaInfo = GetMemoryCardManager()->GetMediaInfo( static_cast<unsigned int>( CGuiScreenMemoryCard::s_currentMemoryCardSlot ) );
            rAssert( mediaInfo != NULL );

            unsigned int savedGameSize = GetMemoryCardManager()->GetSavedGameCreationSize( static_cast<unsigned int>( CGuiScreenMemoryCard::s_currentMemoryCardSlot ) );

            unsigned int numBlocksRequired = (savedGameSize - mediaInfo->m_FreeSpace) / NUM_BYTES_PER_BLOCK;
            rAssert( numBlocksRequired > 0 );
            rWarningMsg( numBlocksRequired < 10, "WARNING: *** Number of free blocks required is more than one digit long!" );

            stringBuffer[ i ] = '0' + numBlocksRequired;

            break;
        }
    }
	for( int i = 0; stringBuffer[ i ] != '\0'; i++ )
	{
		// search for slot wildcard character
		//
		if( stringBuffer[ i ] == SLOT_WILDCARD_CHARACTER )
		{
			HeapMgr()->PushHeap( GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND ?
									GMA_LEVEL_FE : GMA_LEVEL_HUD );
			UnicodeString deviceName;

			const IRadDrive::MediaInfo* mediaInfo = 
				GetMemoryCardManager()->GetMediaInfo( static_cast<unsigned int>(
											CGuiScreenMemoryCard::s_currentMemoryCardSlot ) );
            char textBibleEntry[ 32 ];
            sprintf( textBibleEntry, "XBOX_%s", 
                GetMemoryCardManager()->GetDriveName( static_cast<unsigned int>(
                CGuiScreenMemoryCard::s_currentMemoryCardSlot ) ) );
            deviceName.ReadUnicode( GetTextBibleString( textBibleEntry ) );

            if (mediaInfo->m_VolumeName[0]!=0)  // append personalized  name
			{
				UnicodeString muName;
				if (p3d::UnicodeStrLen((P3D_UNICODE*)( mediaInfo->m_VolumeName) ) < MAX_MEM_CARD_NAME)
					muName.ReadUnicode((P3D_UNICODE*) mediaInfo->m_VolumeName);
				else
				{
					muName.ReadUnicode((P3D_UNICODE*) mediaInfo->m_VolumeName, MAX_MEM_CARD_NAME-1);
					UnicodeString ellipsis;
					ellipsis.ReadAscii("...");
					muName += ellipsis;
				} 
				deviceName.Append(' ');
				deviceName.Append('(');
				deviceName += muName;
				deviceName.Append(')');
			}
			UnicodeString unicodeString; // splice the mu name inside
			UnicodeString restofString;
			unicodeString.ReadUnicode(stringBuffer, i);
			restofString.ReadUnicode(&stringBuffer[i+1]);
			unicodeString += deviceName;
			unicodeString += restofString;

			pText->SetString(pText->GetIndex(),unicodeString);

			HeapMgr()->PopHeap(GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND ?
									GMA_LEVEL_FE : GMA_LEVEL_HUD);
		}
	}


#endif // RAD_XBOX
}

//===========================================================================
// CGuiScreenMessage::InitIntro
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
void CGuiScreenMessage::InitIntro()
{
    rAssert( m_messageText );
    rAssert( s_messageIndex >= 0 &&
             s_messageIndex < m_messageText->GetNumOfStrings() );

    // set message text
    //
    m_messageText->SetIndex( s_messageIndex );

    CGuiScreenMessage::FormatMessage( m_messageText,
                                      m_originalStringBuffer,
                                      sizeof( m_originalStringBuffer ) );

    rAssert( m_messageIcon );
    m_messageIcon->ResetTransformation();

    m_elapsedTime = 0;

/*
    // show/hide accept button depending on whether menu is visible or not
    //
    rAssert( s_menuGroup );
    this->SetButtonVisible( BUTTON_ICON_ACCEPT, s_menuGroup->IsVisible() );
*/
}


//===========================================================================
// CGuiScreenMessage::InitRunning
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
void CGuiScreenMessage::InitRunning()
{
    if( s_pMessageCallback != NULL )
    {
        s_pMessageCallback->HandleMessage( GUI_MSG_ON_DISPLAY_MESSAGE );
    }
}


//===========================================================================
// CGuiScreenMessage::InitOutro
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
void CGuiScreenMessage::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

