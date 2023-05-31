//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenSaveGame
//
// Description: Implementation of the CGuiScreenSaveGame class.
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
#include <presentation/gui/ingame/guiscreensavegame.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guimenu.h> 
#include <presentation/gui/guitextbible.h>
#include <presentation/gui/guiscreenmessage.h>
#include <presentation/gui/guiscreenprompt.h>

#include <data/gamedatamanager.h>
#include <data/savegameinfo.h>
#include <data/memcard/memorycardmanager.h>
#include <memory/srrmemory.h>
#include <gameflow/gameflow.h>

#include <raddebug.hpp>     // Foundation
#include <group.h>
#include <layer.h>
#include <page.h>
#include <screen.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

#ifdef RAD_XBOX
extern char gGameFileName[NUM_GAME_SLOTS][radFileFilenameMax+1];
#endif
//===========================================================================
// CGuiScreenSaveGame::CGuiScreenSaveGame
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
CGuiScreenSaveGame::CGuiScreenSaveGame
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:
	CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_SAVE_GAME ),
    CGuiScreenLoadSave( pScreen ),
    m_pMenu( NULL ),
	m_pFullText( NULL ),
    m_StatusPromptShown(false),
    m_nonEmptySlots( 0 )
{
MEMTRACK_PUSH_GROUP( "CGUIScreenSaveGame" );
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "GameSlots" );
    rAssert( pPage );

    Scrooby::Group* menu = pPage->GetGroup( "Menu" );
    rAssert( menu != NULL );

	m_pFullText = pPage->GetText( "FullMessage" );
	rAssert( m_pFullText != NULL );
	m_pFullText->SetVisible(false);
    m_pFullText->SetTextMode( Scrooby::TEXT_WRAP );
    // Create a menu.
    //
    m_pMenu = new(GMA_LEVEL_HUD) CGuiMenu( this, NUM_GAME_SLOTS );
    rAssert( m_pMenu != NULL );

    // Add menu items
    //
    for( unsigned int i = 0; i < NUM_GAME_SLOTS; i++ )
    {
        char objectName[ 32 ];
        sprintf( objectName, "Slot%d", i );

        m_pMenu->AddMenuItem( menu->GetText( objectName ) );
    }

#ifdef RAD_WIN32
    Scrooby::Text* pText = pPage->GetText( "LoadSaveMessage" );
    if( pText != NULL )
    {
        pText->SetIndex( 1 );
    }
#else
    pPage = m_pScroobyScreen->GetPage( "SelectMemoryDevice" );
    rAssert( pPage != NULL );
    Scrooby::Layer* foreground = pPage->GetLayer( "Foreground" );
    rAssert( foreground != NULL );

    Scrooby::Text* pText = foreground->GetText( "LoadSave" );
    if( pText != NULL )
    {
        pText->SetIndex( 1 );
    }
#endif

    this->AutoScaleFrame( m_pScroobyScreen->GetPage( "BigBoard" ) );
MEMTRACK_POP_GROUP("CGUIScreenSaveGame");
}


//===========================================================================
// CGuiScreenSaveGame::~CGuiScreenSaveGame
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
CGuiScreenSaveGame::~CGuiScreenSaveGame()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenSaveGame::HandleMessage
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
void CGuiScreenSaveGame::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
	if (message == GUI_MSG_MESSAGE_UPDATE)
	{
		if (m_formatState)
		{
			m_elapsedFormatTime += param1;
			if (m_elapsedFormatTime > m_minimumFormatTime && m_formatDone)
            {   
                m_StatusPromptShown = true;
                m_formatState = false;

				if( m_formatResult == Success )
				{
#ifdef RAD_PS2
                    m_currentSlot = 0; // on ps2 continue to save
                    this->SaveGame();
#else
                    m_guiManager->DisplayPrompt(PROMPT_FORMAT_SUCCESS_GC + PLATFORM_TEXT_INDEX, this, PROMPT_TYPE_CONTINUE); // format success
#endif
                }
				else 
				{
					m_guiManager->DisplayPrompt(PROMPT_FORMAT_FAIL_GC + PLATFORM_TEXT_INDEX, this, PROMPT_TYPE_CONTINUE); // format fail
				}
			}
		}
	} 
	else if (message == GUI_MSG_PROMPT_UPDATE)
	{
 		GetMemoryCardManager()->Update( param1 ); // update so we know the status

        if (m_StatusPromptShown==false) { // check for user unplugging memcard if not showing status
		    int current_drive = GetMemoryCardManager()->GetCurrentDriveIndex();

		    if( !GetMemoryCardManager()->IsCurrentDrivePresent(current_drive) )
			    ReloadScreen();
        }
    }
	else if( message == GUI_MSG_ON_DISPLAY_MESSAGE )
    {
		if( m_operation == SAVE )
		{
			// start the save game process
			//
			rAssert( m_currentSlot != -1 );
#ifdef RAD_XBOX
			// has existing filename
			radFileError err = Success;
			if (gGameFileName[m_currentSlot][0]!=0) // delete existing file if overwriting (on xbox each filename is unique)
			{
				err = GetGameDataManager()->DeleteGame(gGameFileName[m_currentSlot]);
			}
			if (err!=Success)
                OnSaveGameComplete(err);
            else
                GetGameDataManager()->SaveGame( m_currentSlot, this );
#else
            GetGameDataManager()->SaveGame( m_currentSlot, this );
#endif
		}
        else if( m_operation == DELETE_GAME )
        {
            // get the filename
            char filename[ radFileFilenameMax + 1 ];
#ifdef RAD_XBOX
            strcpy(filename, gGameFileName[m_currentSlot]);
#else
            GetGameDataManager()->FormatSavedGameFilename( filename,
                sizeof( filename ),
                m_currentSlot );
#endif
            radFileError err = GetGameDataManager()->DeleteGame( filename, true, this );
        }
		else 
		{
			FormatCurrentDrive();
		}
    }
    else if ( message==GUI_MSG_PROMPT_START_RESPONSE )
    {
        m_pParent->HandleMessage( GUI_MSG_UNPAUSE_INGAME );
    }
	else if (message == GUI_MSG_ERROR_PROMPT_RESPONSE )
	{
		 this->HandleErrorResponse( static_cast<CGuiMenuPrompt::ePromptResponse>( param2 ) );
    }
	else if( message == GUI_MSG_MENU_PROMPT_RESPONSE )
    {
        switch( param1 )
        {

            case PROMPT_FORMAT_CONFIRM_GC: 
		    case PROMPT_FORMAT_CONFIRM_PS2: 
		    case PROMPT_FORMAT_CONFIRM_XBOX: // do you really want to format
            {	
                if (param2==CGuiMenuPrompt::RESPONSE_YES) 
				    m_guiManager->DisplayPrompt(PROMPT_FORMAT_CONFIRM2_GC + PLATFORM_TEXT_INDEX,this);
			    else
			    {
                    this->GotoMemoryCardScreen( true );
			    }
			    break;
            }
		    case PROMPT_FORMAT_CONFIRM2_GC:
		    case PROMPT_FORMAT_CONFIRM2_PS2:
		    case PROMPT_FORMAT_CONFIRM2_XBOX: // really format
            {
			    if (param2==CGuiMenuPrompt::RESPONSE_YES) 
			    {
				    m_operation = FORMAT;
				    m_guiManager->DisplayMessage(CGuiScreenMessage::MSG_ID_FORMATTING_GC + PLATFORM_TEXT_INDEX, this);
			    }
			    else
			    {
                    this->GotoMemoryCardScreen( true );
			    }
			    break;
            }
            case PROMPT_LOAD_DELETE_CORRUPT_GC:
            case PROMPT_LOAD_DELETE_CORRUPT_PS2:
            case PROMPT_LOAD_DELETE_CORRUPT_XBOX:
            case PROMPT_LOAD_DELETE_CORRUPT_XBOX_HD:
            {
                if (param2==CGuiMenuPrompt::RESPONSE_NO)
                {
                    this->ReloadScreen();
                }
                else if (param2==CGuiMenuPrompt::RESPONSE_YES)
                {
#ifdef RAD_PS2
                    m_operation = DELETE_GAME;
                    m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_DELETING_GC + PLATFORM_TEXT_INDEX, this );
#else
                    // get the filename
                    char filename[ radFileFilenameMax + 1 ];
    #ifdef RAD_XBOX
                    strcpy(filename, gGameFileName[m_currentSlot]);
    #else
                    GetGameDataManager()->FormatSavedGameFilename( filename,
                        sizeof( filename ),
                        m_currentSlot );
    #endif
                    radFileError err = GetGameDataManager()->DeleteGame(filename);
                    this->OnDeleteGameComplete( err );
#endif // RAD_PS2
                }
                else
                {
                    rTuneAssert(!"not reached");
                }

                break;
            }    

		    case PROMPT_FORMAT_SUCCESS_GC: 
		    case PROMPT_FORMAT_SUCCESS_PS2: 
		    case PROMPT_FORMAT_SUCCESS_XBOX: // format ok
            {
            // m_currentSlot = 0; on ps2 we don't come here
            // this->SaveGame(); 
			    this->ReloadScreen(); 
			    break;
            }

		    case PROMPT_FORMAT_FAIL_GC:
		    case PROMPT_FORMAT_FAIL_PS2:
		    case PROMPT_FORMAT_FAIL_XBOX: // format fail
            {
			    GetMemoryCardManager()->ClearCurrentDrive();
                this->GotoMemoryCardScreen( true );

			    break;
            }
		
            case PROMPT_DELETE_CORRUPT_SUCCESS_GC:
            case PROMPT_DELETE_CORRUPT_SUCCESS_PS2:
            case PROMPT_DELETE_CORRUPT_SUCCESS_XBOX:

            case PROMPT_DELETE_CORRUPT_FAIL_GC:
            case PROMPT_DELETE_CORRUPT_FAIL_PS2:
            case PROMPT_DELETE_CORRUPT_FAIL_XBOX:
            {
                this->ReloadScreen();
                break;	
            }

            case PROMPT_SAVE_CONFIRM_OVERWRITE_GC:
            {
                if( param2 == CGuiMenuPrompt::RESPONSE_YES )
                {
                    m_guiManager->DisplayPrompt( PROMPT_SAVE_CONFIRM_GC, this, PROMPT_TYPE_SAVE );
                }
                else
                {
                    rAssert( param2 == CGuiMenuPrompt::RESPONSE_NO );

                    this->ReloadScreen();
                }

                break;
            }
            case PROMPT_SAVE_CONFIRM_GC:
            case PROMPT_SAVE_CONFIRM_PS2:
            case PROMPT_SAVE_CONFIRM_XBOX:
            case PROMPT_SAVE_CONFIRM_OVERWRITE_PS2:
            case PROMPT_SAVE_CONFIRM_OVERWRITE_XBOX:
            {
                if( param2 == CGuiMenuPrompt::RESPONSE_YES ||
                    param2 == CGuiMenuPrompt::RESPONSE_SAVE )
                {
                    this->SaveGame();
                }
                else
                {
                    rAssert( param2 == CGuiMenuPrompt::RESPONSE_NO );

                    this->ReloadScreen();
                }

                break;
            }

            case PROMPT_SAVE_SUCCESSFUL:
            {
                m_pParent->HandleMessage( GUI_MSG_ON_SAVE_GAME_COMPLETE );

                break;
            }

            default:
            {
                // handle normal error response, "continue", etc
                this->HandleErrorResponse( static_cast<CGuiMenuPrompt::ePromptResponse>( param2 ) );

                break;
            }
        }
    }

    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        if( message == GUI_MSG_CONTROLLER_SELECT &&
            !GetMemoryCardManager()->IsMemcardInfoLoaded() )
        {
            // ignore user select inputs until memcard info is loaded
            //
            return;
        }

        switch( message )
        {
#ifdef RAD_PS2
            case GUI_MSG_CONTROLLER_START:
            {
                if ( GetGameFlow()->GetCurrentContext() == CONTEXT_PAUSE )
                    m_pParent->HandleMessage( GUI_MSG_UNPAUSE_INGAME );

                break;
            }
#endif
            case GUI_MSG_MENU_SELECTION_MADE:
            {
                m_currentSlot = param1; //  // param1 = slot

                SaveGameInfo saveGameInfo;
                bool corrupt = false;
                IRadDrive* currentDrive = GetMemoryCardManager()->GetCurrentDrive();
                bool saveGameExists = GetGameDataManager()->GetSaveGameInfo( currentDrive, m_currentSlot, &saveGameInfo, &corrupt );
                
                if (corrupt)
                {
                    int plat_index = PLATFORM_TEXT_INDEX;
#ifdef RAD_XBOX
                    if (GetMemoryCardManager()->GetCurrentDriveIndex()==0)
                        plat_index++;
#endif
#ifdef RAD_GAMECUBE
                    int errorMessage = GetErrorMessageIndex( DataCorrupt, ERROR_DURING_SAVING );
                    m_guiManager->DisplayErrorPrompt( errorMessage, this,
                                                      ERROR_RESPONSE_CONTINUE_WITHOUT_SAVE | ERROR_RESPONSE_RETRY | ERROR_RESPONSE_DELETE );
                    m_operation = SAVE;
#else
                    m_guiManager->DisplayPrompt( PROMPT_LOAD_DELETE_CORRUPT_GC + plat_index, this );
#endif
                }
                else if( (m_nonEmptySlots & (1 << m_currentSlot)) > 0 )
                {
                    // saved game exists in current slot; prompt w/ overwrite
                    // confirmation message
                    //
#ifdef RAD_GAMECUBE
                    m_guiManager->DisplayPrompt( PROMPT_SAVE_CONFIRM_OVERWRITE_GC, this );
#endif

#ifdef RAD_PS2
                    m_guiManager->DisplayPrompt( PROMPT_SAVE_CONFIRM_OVERWRITE_PS2, this );
#endif

#ifdef RAD_XBOX
                    m_guiManager->DisplayPrompt( PROMPT_SAVE_CONFIRM_OVERWRITE_XBOX, this );
#endif

#ifdef RAD_WIN32
                    m_guiManager->DisplayPrompt( PROMPT_SAVE_CONFIRM_OVERWRITE_XBOX, this );
#endif
                }
                else
                {
#ifdef RAD_GAMECUBE
                    m_guiManager->DisplayPrompt( PROMPT_SAVE_CONFIRM_GC, this, PROMPT_TYPE_SAVE );
#endif

#ifdef RAD_PS2
                    m_guiManager->DisplayPrompt( PROMPT_SAVE_CONFIRM_PS2, this );
#endif

#ifdef RAD_XBOX
                    m_guiManager->DisplayPrompt( PROMPT_SAVE_CONFIRM_XBOX, this );
#endif

#ifdef RAD_WIN32
                    m_guiManager->DisplayPrompt( PROMPT_SAVE_CONFIRM_XBOX, this );
#endif
                }

                break;
            }
            case GUI_MSG_CONTROLLER_BACK:
            {
#ifdef RAD_XBOX
                s_forceGotoMemoryCardScreen = true;
                this->GotoMemoryCardScreen();
#endif
                break;
            }
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

        CGuiScreenLoadSave::HandleMessage( message, param1, param2 );
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}

void
CGuiScreenSaveGame::OnSaveGameComplete( radFileError errorCode )
{
    m_lastError = errorCode;
    m_StatusPromptShown = true;

    if( errorCode == Success )
    {
        m_guiManager->DisplayPrompt( PROMPT_SAVE_SUCCESSFUL, this, PROMPT_TYPE_CONTINUE );
    }
    else
    {
        int errorMessage = GetErrorMessageIndex( errorCode, ERROR_DURING_SAVING );

#ifdef RAD_GAMECUBE
        switch( errorCode )
        {
            case Success:
            {
                rAssert( false );
                break;
            }
            case MediaCorrupt:
            {
				m_guiManager->DisplayErrorPrompt( errorMessage, this, ERROR_RESPONSE_CONTINUE );

                break;
            }
            case MediaEncodingErr:
            case MediaNotFormatted:
            {
                m_guiManager->DisplayErrorPrompt( errorMessage, this,
                                                  ERROR_RESPONSE_CONTINUE | ERROR_RESPONSE_RETRY | ERROR_RESPONSE_FORMAT );

            }
            default:
            {
				m_guiManager->DisplayErrorPrompt( errorMessage, this,
                                                  ERROR_RESPONSE_CONTINUE | ERROR_RESPONSE_RETRY );

                break;
            }
        }
#endif // RAD_GAMECUBE

#ifdef RAD_PS2
        switch( errorCode )
        {
            case Success:
            {
                rAssert( false );
                break;
            }
            default:
            {
                m_guiManager->DisplayErrorPrompt( errorMessage, this,
                                                  ERROR_RESPONSE_CONTINUE );

                break;
            }
        }
#endif // RAD_PS2

#ifdef RAD_XBOX
        switch( errorCode )
        {
            case Success:
            {
                rAssert( false );
                break;
            }
            default:
            {
                m_guiManager->DisplayErrorPrompt( errorMessage, this,
                                                  ERROR_RESPONSE_CONTINUE );

                break;
            }
        }
#endif // RAD_XBOX

#ifdef RAD_WIN32
        switch( errorCode )
        {
            case Success:
            {
                rAssert( false );
                break;
            }
            default:
            {
                m_guiManager->DisplayErrorPrompt( errorMessage, this,
                                                  ERROR_RESPONSE_CONTINUE );
                break;
            }
        }
#endif // RAD_WIN32
    }
}

void
CGuiScreenSaveGame::OnDeleteGameComplete( radFileError errorCode )
{
    m_operation = SCREEN_OP_IDLE;
    m_StatusPromptShown = true;

    if( errorCode == Success )
    {
        m_guiManager->DisplayPrompt( PROMPT_DELETE_CORRUPT_SUCCESS_GC + PLATFORM_TEXT_INDEX,
                                     this,
                                     PROMPT_TYPE_CONTINUE );
    }
    else
    {
        m_guiManager->DisplayPrompt( PROMPT_DELETE_CORRUPT_FAIL_GC + PLATFORM_TEXT_INDEX,
                                     this,
                                     PROMPT_TYPE_CONTINUE );
    }
}

void
CGuiScreenSaveGame::HandleErrorResponse( CGuiMenuPrompt::ePromptResponse response )
{
    switch( response )
    {
        case (CGuiMenuPrompt::RESPONSE_CONTINUE):
        case (CGuiMenuPrompt::RESPONSE_CONTINUE_WITHOUT_SAVE):
        {
            if( m_operation == SAVE )
            {
                this->ReloadScreen();
            }
            else if( m_operation == FORMAT )
            {
                this->ReloadScreen();
            }
            else
            {
                this->GotoMemoryCardScreen( true );
            }

            break;
        }
        case (CGuiMenuPrompt::RESPONSE_RETRY):
        {
            if( m_operation == SAVE )
            {
#ifdef RAD_GAMECUBE
                SaveGameInfo saveGameInfo;
                bool corrupt = false;
                IRadDrive* currentDrive = GetMemoryCardManager()->GetCurrentDrive();
                GetGameDataManager()->GetSaveGameInfo( currentDrive, m_currentSlot, &saveGameInfo, &corrupt );
                if( corrupt )
                {
                    int errorMessage = GetErrorMessageIndex( DataCorrupt, ERROR_DURING_SAVING );
                    m_guiManager->DisplayErrorPrompt( errorMessage, this,
                                                      ERROR_RESPONSE_CONTINUE_WITHOUT_SAVE | ERROR_RESPONSE_RETRY | ERROR_RESPONSE_DELETE );
                }
                else
#endif
                {
                    this->SaveGame();
                }
            }
            else if( m_operation == FORMAT )
            {
                m_guiManager->DisplayMessage(CGuiScreenMessage::MSG_ID_FORMATTING_GC + PLATFORM_TEXT_INDEX, this);
            }
            else
            {
                this->ReloadScreen();
            }

            break;
        }

#ifdef RAD_GAMECUBE
        case (CGuiMenuPrompt::RESPONSE_DELETE):
        {
            m_guiManager->DisplayPrompt( PROMPT_LOAD_DELETE_CORRUPT_GC, this );

            break;
        }
#endif // RAD_GAMECUBE

        case (CGuiMenuPrompt::RESPONSE_FORMAT_GC):
        case (CGuiMenuPrompt::RESPONSE_FORMAT_XBOX):
        case (CGuiMenuPrompt::RESPONSE_FORMAT_PS2):
        {
            m_guiManager->DisplayPrompt(PROMPT_FORMAT_CONFIRM2_GC + PLATFORM_TEXT_INDEX,this);
            break;
        }
        default:
        {
            rTunePrintf( "*** WARNING: Unhandled response for error [%d]!\n", m_lastError );
            rAssert( false );

            this->ReloadScreen();

            break;
        }
    }

    CGuiScreenLoadSave::HandleErrorResponse( response );
}


//===========================================================================
// CGuiScreenSaveGame::InitIntro
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
void CGuiScreenSaveGame::InitIntro()
{
	unsigned int num_empty_slots = 0;
	bool unformatted = false;
    IRadDrive::MediaInfo::MediaState mediaState;

    m_StatusPromptShown = false;
    m_operation = SCREEN_OP_IDLE;
    
	if( s_forceGotoMemoryCardScreen || !GetMemoryCardManager()->IsCurrentDriveReady( true, &unformatted, &mediaState ))
    {
		if (unformatted && !s_forceGotoMemoryCardScreen)
		{
#ifdef RAD_GAMECUBE 
            int errorMessage;

            errorMessage = GetErrorMessageIndex( mediaState );
            m_guiManager->DisplayErrorPrompt( errorMessage,
                                              this,
                                              ERROR_RESPONSE_CONTINUE_WITHOUT_SAVE | ERROR_RESPONSE_RETRY | ERROR_RESPONSE_FORMAT );
#else
			m_guiManager->DisplayPrompt(PROMPT_FORMAT_CONFIRM_GC+PLATFORM_TEXT_INDEX,this);
#endif
			m_numTransitionsPending = -1; // disable all transitions
		}
		else
		{
			this->GotoMemoryCardScreen();
			m_numTransitionsPending = -1; // disable all transitions
		}
        return;
    }

    m_nonEmptySlots = 0; // reset non-empty slots bitmask

    this->UpdateCurrentMemoryDevice();

    rAssert( m_pMenu );
    m_pMenu->Reset();

    IRadDrive* currentDrive = GetMemoryCardManager()->GetCurrentDrive();

    int currentDriveIndex = GetMemoryCardManager()->GetCurrentDriveIndex();
    bool enoughFreeSpace = GetMemoryCardManager()->EnoughFreeSpace( currentDriveIndex );

    radDate mostRecentTimestamp;
    mostRecentTimestamp.m_Year = 0;

    // update all save game slots display info
    //
    for( unsigned int i = 0; i < NUM_GAME_SLOTS; i++ )
    {
        SaveGameInfo saveGameInfo;
        bool corrupt;
        bool saveGameExists = GetGameDataManager()->GetSaveGameInfo( currentDrive, i, &saveGameInfo, &corrupt );
//        saveGameExists = saveGameExists && saveGameInfo.CheckData();

        Scrooby::Text* slotText = dynamic_cast<Scrooby::Text*>( m_pMenu->GetMenuItem( i )->GetItem() );
        rAssert( slotText != NULL );

        HeapMgr()->PushHeap( GMA_LEVEL_HUD );
#ifdef RAD_XBOX
		gGameFileName[i][0] = 0;
#endif
       if( saveGameExists )
        {
            if (corrupt)
            {
                UnicodeString corruptSlot;
#ifdef RAD_XBOX
                corruptSlot.ReadUnicode( GetTextBibleString( "CORRUPT_SLOT_(XBOX)" ) );
#endif
#ifdef RAD_WIN32
                corruptSlot.ReadUnicode( GetTextBibleString( "CORRUPT_SLOT_(XBOX)" ) );
#endif
#ifdef RAD_PS2
                corruptSlot.ReadUnicode( GetTextBibleString( "CORRUPT_SLOT_(PS2)" ) );
#endif
#ifdef RAD_GAMECUBE
                corruptSlot.ReadUnicode( GetTextBibleString( "CORRUPT_SLOT_(GC)" ) );
#endif
                slotText->SetString(0,corruptSlot);
#ifdef RAD_XBOX
                strcpy(gGameFileName[i], saveGameInfo.m_displayFilename); // cache filename in the slot
#endif
            }
            else
            {
                slotText->SetString( 0, saveGameInfo.m_displayFilename );
            #ifdef RAD_XBOX
			    strcpy(gGameFileName[i], saveGameInfo.m_displayFilename); // cache filename in the slot
            #endif
            }

            // default to slot with most recent saved game file
            //
            const SaveGameInfoData* pData = saveGameInfo.GetData();
            rAssert( pData != NULL );
            if( SaveGameInfo::CompareTimeStamps( pData->m_timeStamp, mostRecentTimestamp ) > 0 )
            {
                memcpy( &mostRecentTimestamp, &pData->m_timeStamp, sizeof( radDate ) );

                m_pMenu->Reset( i );
            }

            // update non-empty slots bitmask
            //
            m_nonEmptySlots |= (1 << i);
        }
        else
        {
            UnicodeString emptySlot;
            if (enoughFreeSpace)
                emptySlot.ReadUnicode( GetTextBibleString( "EMPTY_SLOT" ) );
            else
                emptySlot.ReadUnicode( GetTextBibleString( "FULL_SLOT" ) );

#ifdef RAD_XBOX
			if (num_empty_slots) // blank out extra empty slot item
				emptySlot.ReadUnicode( GetTextBibleString ("SPACE") );
#endif
            slotText->SetString( 0, emptySlot );
			num_empty_slots++;
        }
        HeapMgr()->PopHeap(GMA_LEVEL_HUD);

        // enable slot selection only if save game exists or there's enough
        // free space to save a new game
        //
        m_pMenu->SetMenuItemEnabled( i, saveGameExists || enoughFreeSpace );
#ifdef RAD_XBOX
	if (num_empty_slots > 1) // disable extra empty slot for xbox
		m_pMenu->SetMenuItemEnabled( i, false );
#endif
    }

	/* display/hide full message */
	if (!enoughFreeSpace)
	{
		int message_index = 0; // no existing slot
		if (num_empty_slots < NUM_GAME_SLOTS )
			message_index = 1; // has existing slot
		// we have 2 group of per platform messages, gc,ps2,xbox_mu, xbox_hd
		message_index = message_index * 4 + PLATFORM_TEXT_INDEX;
#ifdef RAD_XBOX
		if (currentDriveIndex==0)
		{
			message_index++; // xbox hard disk
		}
#endif
#ifdef RAD_WIN32
        message_index = 9;
#endif
		m_pFullText->SetIndex(message_index);
		m_pFullText->SetVisible(true);

#ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_LEVEL_HUD );

        // append "Use Memory Card Screen" text to message; this is done in
        // code because the text bible compiler can't handle strings with
        // more than 255 characters
        //
        UnicodeString useMemCardScreen;
        useMemCardScreen.ReadUnicode( GetTextBibleString( "USE_MEMORY_CARD_SCREEN" ) );

        UnicodeString newString;
        newString.ReadUnicode( GetTextBibleString( "MEMCARD_FULL_HAS_EXISTING_(GC)" ) );
        newString.Append( ' ' );
        newString += useMemCardScreen;

        m_pFullText->SetString( message_index, newString );

        HeapMgr()->PopHeap( GMA_LEVEL_HUD );
#endif // RAD_GAMECUBE
	}
	else
	{
		m_pFullText->SetVisible(false);
	}
	this->SetButtonVisible( BUTTON_ICON_ACCEPT, m_pMenu->GetSelection() != -1 );
}


//===========================================================================
// CGuiScreenSaveGame::InitRunning
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
void CGuiScreenSaveGame::InitRunning()
{
//    rAssertMsg( GetMemoryCardManager()->IsMemcardInfoLoaded(),
//                "WARNING: *** Memory card info not loaded yet!" );
}


//===========================================================================
// CGuiScreenSaveGame::InitOutro
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
void CGuiScreenSaveGame::InitOutro()
{
}


void
CGuiScreenSaveGame::GotoMemoryCardScreen( bool isFromPrompt )
{
#ifdef RAD_WIN32
    m_pParent->HandleMessage( GUI_MSG_BACK_SCREEN );
#else
    if( isFromPrompt )
    {
        s_forceGotoMemoryCardScreen = true;
        this->ReloadScreen();
    }
    else
    {
        m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN, GUI_SCREEN_ID_MEMORY_CARD );
    }
#endif // RAD_WIN32
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenSaveGame::SaveGame()
{
	m_operation = SAVE;

#ifdef RAD_GAMECUBE
    m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_SAVING_GAME_GC, this );
#endif

#ifdef RAD_PS2
    m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_SAVING_GAME_PS2, this );
#endif

#ifdef RAD_XBOX
    if( m_currentDriveIndex == 0 ) // xbox hard disk
    {
        m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_SAVING_GAME_XBOX_HD, this );
    }
    else // xbox memory unit
    {
        m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_SAVING_GAME_XBOX, this );
    }
#endif

#ifdef RAD_WIN32
    m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_SAVING_GAME_PC, this );
#endif
}

