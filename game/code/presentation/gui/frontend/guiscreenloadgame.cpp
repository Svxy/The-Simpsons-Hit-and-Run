//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLoadGame
//
// Description: Implementation of the CGuiScreenLoadGame class.
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
#include <presentation/gui/frontend/guiscreenloadgame.h>
#include <presentation/gui/frontend/guiscreenmainmenu.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guitextbible.h>
#include <presentation/gui/guiscreenmessage.h>
#include <presentation/gui/guiscreenprompt.h>

#include <data/gamedatamanager.h>
#include <data/savegameinfo.h>
#include <data/memcard/memorycardmanager.h>
#include <memory/srrmemory.h>

#include <raddebug.hpp>     // Foundation
#include <page.h>
#include <screen.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================
#ifdef RAD_XBOX
    char gGameFileName[NUM_GAME_SLOTS][radFileFilenameMax+1];
#endif

#ifdef RAD_PS2
    const unsigned int AUTO_LOAD_MINIMUM_DISPLAY_TIME = 5000; // in msec
#endif

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenLoadGame::CGuiScreenLoadGame
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
CGuiScreenLoadGame::CGuiScreenLoadGame
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent,
    eGuiWindowID windowID
)
:   CGuiScreen( pScreen, pParent, windowID ),
    CGuiScreenLoadSave( pScreen ),
    m_pMenu( NULL ),
    m_pFullText( NULL ),
    m_StatusPromptShown(false)

{
MEMTRACK_PUSH_GROUP( "CGUIScreenLoadGame" );
    if( windowID == GUI_SCREEN_ID_LOAD_GAME )
    {
        // Retrieve the Scrooby drawing elements.
        //
        Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "GameSlots" );
        rAssert( pPage );

		// hide full text field
		m_pFullText = pPage->GetText( "FullMessage" );
		rAssert( m_pFullText != NULL );
		m_pFullText->SetVisible(false);
        m_pFullText->SetTextMode( Scrooby::TEXT_WRAP );
 
        // Create a menu.
        //
        m_pMenu = new(GMA_LEVEL_FE) CGuiMenu( this, NUM_GAME_SLOTS );
        rAssert( m_pMenu != NULL );

        // Add menu items
        //
        for( unsigned int i = 0; i < NUM_GAME_SLOTS; i++ )
        {
            char objectName[ 32 ];
            sprintf( objectName, "Slot%d", i );

            m_pMenu->AddMenuItem( pPage->GetText( objectName ) );
        }
    }
MEMTRACK_POP_GROUP( "CGUIScreenLoadGame" );
}


//===========================================================================
// CGuiScreenLoadGame::~CGuiScreenLoadGame
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
CGuiScreenLoadGame::~CGuiScreenLoadGame()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenLoadGame::HandleMessage
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
void CGuiScreenLoadGame::HandleMessage
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
				if( m_formatResult == Success )
				{
					m_guiManager->DisplayPrompt(PROMPT_FORMAT_SUCCESS_GC + PLATFORM_TEXT_INDEX, this, PROMPT_TYPE_CONTINUE); // format success
					m_formatState = false;
				}
				else 
				{
					m_guiManager->DisplayPrompt(PROMPT_FORMAT_FAIL_GC + PLATFORM_TEXT_INDEX, this, PROMPT_TYPE_CONTINUE); // format fail
					m_formatState = false;
				}
			}
		}
	}
	else if (message == GUI_MSG_PROMPT_UPDATE)
	{
        // update so status up to date
        GetMemoryCardManager()->Update( param1 );
        if (m_StatusPromptShown==false) { // check for user unplugging memcard if not showing status
		    int currentDrive = GetMemoryCardManager()->GetCurrentDriveIndex();
		    if( !GetMemoryCardManager()->IsCurrentDrivePresent(currentDrive) )
			    ReloadScreen();
        }
	}
	else if( message == GUI_MSG_ON_DISPLAY_MESSAGE )
    {
		if (m_operation==LOAD)
		{
			// start the load game process
			//
			rAssert( m_currentSlot != -1 );
#ifdef RAD_XBOX
			GetGameDataManager()->LoadGame( m_currentSlot, this, gGameFileName[m_currentSlot] );
#else
			GetGameDataManager()->LoadGame( m_currentSlot, this );
#endif
		}
		else
		{
			FormatCurrentDrive();
		}
    }
	else if ( message == GUI_MSG_ERROR_PROMPT_RESPONSE )
	{
		this->HandleErrorResponse( static_cast<CGuiMenuPrompt::ePromptResponse>( param2 ) );
	}
    else if( message == GUI_MSG_MENU_PROMPT_RESPONSE )
    {
        switch( param1 )
        {
			case PROMPT_LOAD_CARD_EMPTY_GC:
			case PROMPT_LOAD_CARD_EMPTY_PS2:
			case PROMPT_LOAD_CARD_EMPTY_XBOX:
			case PROMPT_LOAD_CARD_EMPTY_XBOX_HD:
                this->GotoMemoryCardScreen( true );
			break;
			
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
                        if (err==Success)
                            m_guiManager->DisplayPrompt(PROMPT_DELETE_CORRUPT_SUCCESS_GC + PLATFORM_TEXT_INDEX, this, 
                            PROMPT_TYPE_CONTINUE); 
                        else
                            m_guiManager->DisplayPrompt(PROMPT_DELETE_CORRUPT_FAIL_GC + PLATFORM_TEXT_INDEX, this, 
                            PROMPT_TYPE_CONTINUE); 

                }

                break;
            }    
            case PROMPT_DELETE_CORRUPT_SUCCESS_GC:
            case PROMPT_DELETE_CORRUPT_SUCCESS_PS2:
            case PROMPT_DELETE_CORRUPT_SUCCESS_XBOX:

            case PROMPT_DELETE_CORRUPT_FAIL_GC:
            case PROMPT_DELETE_CORRUPT_FAIL_PS2:
            case PROMPT_DELETE_CORRUPT_FAIL_XBOX:
				this->ReloadScreen();
			break;

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
		    case PROMPT_FORMAT_SUCCESS_GC: 
		    case PROMPT_FORMAT_SUCCESS_PS2: 
		    case PROMPT_FORMAT_SUCCESS_XBOX: // format ok
            {
                this->GotoMemoryCardScreen( true );

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
            case PROMPT_LOAD_CONFIRM_GC:
            case PROMPT_LOAD_CONFIRM_PS2:
            case PROMPT_LOAD_CONFIRM_XBOX:
            {
                if( param2 == CGuiMenuPrompt::RESPONSE_YES )
                {
                    this->LoadGame();
                }
                else
                {
                    rAssert( param2 == CGuiMenuPrompt::RESPONSE_NO );

                    this->ReloadScreen();
                }

                break;
            }

            case PROMPT_LOAD_SUCCESSFUL:
            {
                CGuiScreen* promptScreen = (CGuiScreen*)m_guiManager->FindWindowByID( GUI_SCREEN_ID_GENERIC_PROMPT );
                rAssert( promptScreen );
                promptScreen->StartTransitionAnimation( 230, 260 );

                // re-init main menu and default to "resume game" selection
                //
				CGuiScreenMainMenu* pScreen =
                    static_cast<CGuiScreenMainMenu*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_MAIN_MENU ) );
                rAssert( pScreen != NULL );
                pScreen->InitMenu();

				m_pParent->HandleMessage( GUI_MSG_BACK_SCREEN );

                break;
            }

            default:
            {
                // handle normal menu condition, "continue", "retry"
                this->HandleErrorResponse( static_cast<CGuiMenuPrompt::ePromptResponse>( param2 ) );

                break;
            }
        }
    }

    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_MENU_SELECTION_MADE:
            {
                m_currentSlot = param1; //  // param1 = slot

                SaveGameInfo saveGameInfo;
                bool corrupt;
                IRadDrive* currentDrive = GetMemoryCardManager()->GetCurrentDrive();
                bool saveGameExists = GetGameDataManager()->GetSaveGameInfo( currentDrive, m_currentSlot, &saveGameInfo, &corrupt );

                if (corrupt)
                {
#ifdef RAD_GAMECUBE
                    int errorMessage = GetErrorMessageIndex( DataCorrupt, ERROR_DURING_LOADING );
                    m_guiManager->DisplayErrorPrompt( errorMessage, this,
                                                      ERROR_RESPONSE_CONTINUE | ERROR_RESPONSE_RETRY | ERROR_RESPONSE_DELETE );
                    m_operation = LOAD;
#endif
#ifdef RAD_PS2
                    rAssertMsg( false, "Corrupted save games should not have been selectable!" );
#endif
#ifdef RAD_XBOX
                    // for xbox don't ask to delete just put up a message
                    int errorMessage = GetErrorMessageIndex( DataCorrupt, ERROR_DURING_LOADING );
                    m_guiManager->DisplayErrorPrompt( errorMessage, this, ERROR_RESPONSE_CONTINUE );
#endif
                }
                else
                {
    #ifdef RAD_GAMECUBE
                    m_guiManager->DisplayPrompt( PROMPT_LOAD_CONFIRM_GC, this );
    #endif

    #ifdef RAD_PS2
                    m_guiManager->DisplayPrompt( PROMPT_LOAD_CONFIRM_PS2, this );
    #endif

    #ifdef RAD_XBOX
                    m_guiManager->DisplayPrompt( PROMPT_LOAD_CONFIRM_XBOX, this );
    #endif

    #ifdef RAD_WIN32
                    m_guiManager->DisplayPrompt( PROMPT_LOAD_CONFIRM_XBOX, this ); // parallel xbox for now.
    #endif
                }

                break;
            }

            case GUI_MSG_CONTROLLER_BACK:
            {
#ifdef RAD_XBOX
                s_forceGotoMemoryCardScreen = true;
                this->GotoMemoryCardScreen();
#else
                this->StartTransitionAnimation( 230, 260 );
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

        if( m_ID == GUI_SCREEN_ID_LOAD_GAME )
        {
            CGuiScreenLoadSave::HandleMessage( message, param1, param2 );
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}


void
CGuiScreenLoadGame::OnLoadGameComplete( radFileError errorCode )
{
    m_lastError = errorCode;
    m_StatusPromptShown = true;
    
	if( errorCode == Success )
	{
		m_guiManager->DisplayPrompt( PROMPT_LOAD_SUCCESSFUL, this, PROMPT_TYPE_CONTINUE );
	}
	else
    {
        int errorMessage = GetErrorMessageIndex( errorCode, ERROR_DURING_LOADING );

#ifdef RAD_GAMECUBE
        switch( errorCode )
        {
            case Success:
            {
                rAssert( false );
                break;
            }
            case MediaEncodingErr:
            case MediaNotFormatted:
            {
                m_guiManager->DisplayErrorPrompt( errorMessage, this,
                                                  ERROR_RESPONSE_CONTINUE | ERROR_RESPONSE_RETRY | ERROR_RESPONSE_FORMAT );

            }
            case DataCorrupt:
            {
                m_guiManager->DisplayErrorPrompt( errorMessage, this,
                                                  ERROR_RESPONSE_CONTINUE | ERROR_RESPONSE_RETRY | ERROR_RESPONSE_DELETE );

                break;
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
/*
            case DataCorrupt:
            {
                m_guiManager->DisplayErrorPrompt( errorMessage, this,
                                                  ERROR_RESPONSE_YES | ERROR_RESPONSE_NO );

                break;
            }
*/
            default:
            {
                m_guiManager->DisplayErrorPrompt( errorMessage, this, ERROR_RESPONSE_CONTINUE );

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
                if (errorCode==DataCorrupt) // no delete corrupt for xbox loading
                    m_guiManager->DisplayErrorPrompt( errorMessage, this,
                    ERROR_RESPONSE_CONTINUE );
                else
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
CGuiScreenLoadGame::HandleErrorResponse( CGuiMenuPrompt::ePromptResponse response )
{
    switch( response )
    {
        case (CGuiMenuPrompt::RESPONSE_CONTINUE):
        {
            if( m_operation == LOAD )
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
            if( m_operation == LOAD )
            {
                this->LoadGame();
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

#if defined( RAD_GAMECUBE ) || defined( RAD_PS2 )
        case (CGuiMenuPrompt::RESPONSE_YES):
        {
            // YES to delete corrupted file
            //
            char filename[ radFileFilenameMax + 1 ];
#ifdef RAD_XBOX
            strcpy( filename, gGameFileName[ m_currentSlot ] );
#else
            GetGameDataManager()->FormatSavedGameFilename( filename,
                                                           sizeof( filename ),
                                                           m_currentSlot );
#endif
            radFileError err = GetGameDataManager()->DeleteGame( filename );
            if( err == Success )
            {
                m_guiManager->DisplayPrompt( PROMPT_DELETE_CORRUPT_SUCCESS_GC + PLATFORM_TEXT_INDEX,
                                             this, PROMPT_TYPE_CONTINUE );
            }
            else
            {
                m_guiManager->DisplayPrompt( PROMPT_DELETE_CORRUPT_FAIL_GC + PLATFORM_TEXT_INDEX,
                                             this, PROMPT_TYPE_CONTINUE );
            }

            break;
        }
        case (CGuiMenuPrompt::RESPONSE_NO):
        {
            // NO to delete corrupted file
            //
            this->ReloadScreen();

            break;
        }
#endif // RAD_GAMECUBE || RAD_PS2

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
// CGuiScreenLoadGame::InitIntro
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
void CGuiScreenLoadGame::InitIntro()
{
	bool unformatted = false;
    bool file_corrupt = false;
    SaveGameInfo saveGameInfo;
    m_StatusPromptShown = false;
    m_operation = SCREEN_OP_IDLE;

    m_pFullText->SetVisible(false);

    IRadDrive::MediaInfo::MediaState mediaState;
    if( s_forceGotoMemoryCardScreen || !GetMemoryCardManager()->IsCurrentDriveReady( true, &unformatted, &mediaState ) )
    {
		if (unformatted && !s_forceGotoMemoryCardScreen)
		{
#ifdef RAD_GAMECUBE 
            int errorMessage = GetErrorMessageIndex( mediaState );
            m_guiManager->DisplayErrorPrompt( errorMessage,
                                              this,
                                              ERROR_RESPONSE_CONTINUE | ERROR_RESPONSE_RETRY | ERROR_RESPONSE_FORMAT );

			m_numTransitionsPending = -1; // disable all transitions

            return;
#endif
		}
		else
		{
			this->GotoMemoryCardScreen();
			m_numTransitionsPending = -1; // disable all transitions

            return;
		}
    }

    this->UpdateCurrentMemoryDevice();

    rAssert( m_pMenu );
    m_pMenu->Reset();

    IRadDrive* currentDrive = GetMemoryCardManager()->GetCurrentDrive();

    radDate mostRecentTimestamp;
    mostRecentTimestamp.m_Year = 0;
    bool has_savegame = false;
    bool has_4_saves = false;

    // update all save game slots display info
    //
	for( unsigned int i = 0; i < NUM_GAME_SLOTS; i++ )
    {
        bool saveGameExists = GetGameDataManager()->GetSaveGameInfo( currentDrive, i, &saveGameInfo, &file_corrupt );
//        saveGameExists = saveGameExists && saveGameInfo.CheckData();

        Scrooby::Text* slotText = dynamic_cast<Scrooby::Text*>( m_pMenu->GetMenuItem( i )->GetItem() );
        rAssert( slotText != NULL );

        HeapMgr()->PushHeap( GMA_LEVEL_FE );

		if( saveGameExists )
        {
            if (i == NUM_GAME_SLOTS-1)
            {
                has_4_saves = true;
            }
            has_savegame = true;
            if (file_corrupt)
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
            }
            else
            {
            #ifdef RAD_XBOX
			     strcpy(gGameFileName[i], saveGameInfo.m_displayFilename); // cache the slot filename
            #endif
                 slotText->SetString( 0, saveGameInfo.m_displayFilename );
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
        }
        else
        {
            UnicodeString emptySlot;
            emptySlot.ReadUnicode( GetTextBibleString( "EMPTY_SLOT" ) );
            slotText->SetString( 0, emptySlot );
        }
        HeapMgr()->PopHeap(GMA_LEVEL_FE);

        // enable slot selection only if save game exists
        //
#ifdef RAD_PS2
        m_pMenu->SetMenuItemEnabled( i, saveGameExists && !file_corrupt );
#else
        m_pMenu->SetMenuItemEnabled( i, saveGameExists );
#endif // RAD_PS2
    }

	if (has_savegame==false) // no file in card
	{
		int prompt_id = PROMPT_LOAD_CARD_EMPTY_GC+PLATFORM_TEXT_INDEX;
#ifdef RAD_XBOX
		if (GetMemoryCardManager()->GetCurrentDriveIndex()==0)
			prompt_id = PROMPT_LOAD_CARD_EMPTY_XBOX_HD;
#endif
		m_guiManager->DisplayPrompt(prompt_id,this,PROMPT_TYPE_CONTINUE);
		m_numTransitionsPending = -1; // disable all transitions
	}
	else
    {
#ifdef RAD_XBOX
        // check if there are more than 4 files on hd
        if ( has_4_saves 
            && GetGameDataManager()->GetSaveGameInfo( currentDrive, NUM_GAME_SLOTS, &saveGameInfo, &file_corrupt )
            )
        {
            m_pFullText->SetIndex( 8 );
            m_pFullText->SetVisible(true);

        }
#endif
	    this->SetButtonVisible( BUTTON_ICON_ACCEPT, true );
    }
}

//===========================================================================
// CGuiScreenLoadGame::InitRunning
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
void CGuiScreenLoadGame::InitRunning()
{
}


//===========================================================================
// CGuiScreenLoadGame::InitOutro
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
void CGuiScreenLoadGame::InitOutro()
{
}

void
CGuiScreenLoadGame::GotoMemoryCardScreen( bool isFromPrompt )
{
#ifdef RAD_WIN32
    if( isFromPrompt )
    {
        CGuiScreen* pScreen = static_cast<CGuiScreen*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_GENERIC_PROMPT ) );
        rAssert( pScreen != NULL );
        pScreen->StartTransitionAnimation( 230, 260 );
    }
    else
    {
        this->StartTransitionAnimation( 230, 260 );
    }

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

void CGuiScreenLoadGame::LoadGame()
{
	m_operation = LOAD;

#ifdef RAD_GAMECUBE
    m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_LOADING_GAME_GC, this );
#endif

#ifdef RAD_PS2
    m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_LOADING_GAME_PS2, this );
#endif

#ifdef RAD_XBOX
    if( m_currentDriveIndex == 0 ) // xbox hard disk
    {
        m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_LOADING_GAME_XBOX_HD, this );
    }
    else // xbox memory unit
    {
        m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_LOADING_GAME_XBOX, this );
    }
#endif

#ifdef RAD_WIN32
    m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_LOADING_GAME_PC, this );
#endif
}

//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------


//===========================================================================
// Public Member Functions (for CGuiScreenAutoLoad)
//===========================================================================

int CGuiScreenAutoLoad::s_autoLoadGameSlot = -1;

CGuiScreenAutoLoad::CGuiScreenAutoLoad( Scrooby::Screen* pScreen, CGuiEntity* pParent )
:   CGuiScreenLoadGame( pScreen, pParent, GUI_SCREEN_ID_AUTO_LOAD )
{
}

CGuiScreenAutoLoad::~CGuiScreenAutoLoad()
{
}

void
CGuiScreenAutoLoad::OnLoadGameComplete( radFileError errorCode )
{
    GetGameDataManager()->RestoreDefaultMinimumLoadSaveTime();

    if( errorCode == Success )
    {
        m_pParent->HandleMessage( GUI_MSG_MEMCARD_CHECK_COMPLETED );
    }
    else
    {
        CGuiScreenLoadGame::OnLoadGameComplete( errorCode );
    }
}

void
CGuiScreenAutoLoad::InitIntro()
{
}

void
CGuiScreenAutoLoad::InitRunning()
{
    m_currentDriveIndex = GetMemoryCardManager()->GetCurrentDriveIndex();
    rAssert( m_currentDriveIndex != -1 );

    IRadDrive* currentDrive = GetMemoryCardManager()->GetCurrentDrive();
    if( currentDrive != NULL )
    {
        rReleasePrintf( "Auto-loading most recent saved game from drive %s\n",
                        currentDrive->GetDriveName() );
    }

    rAssert( s_autoLoadGameSlot != -1 );
    m_currentSlot = s_autoLoadGameSlot;
#ifdef RAD_XBOX
	// for xbox we need to get the filename from slot information first before loading
	SaveGameInfo saveGameInfo;
	gGameFileName[m_currentSlot][0] = 0; // initialize to empty string
	rAssert(m_currentSlot < NUM_GAME_SLOTS);
	bool saveGameExists = GetGameDataManager()->GetSaveGameInfo( currentDrive, m_currentSlot, &saveGameInfo );
	if( saveGameExists )	// game filename is cached in gGameFileName global
	{
		strcpy( gGameFileName[m_currentSlot], saveGameInfo.m_displayFilename);
	}

#endif 

    this->LoadGame();
}

void
CGuiScreenAutoLoad::InitOutro()
{
}

void
CGuiScreenAutoLoad::HandleErrorResponse( CGuiMenuPrompt::ePromptResponse response )
{
    switch( response )
    {
        case (CGuiMenuPrompt::RESPONSE_CONTINUE):
        {
            m_pParent->HandleMessage( GUI_MSG_MEMCARD_CHECK_COMPLETED );

            break;
        }
        case (CGuiMenuPrompt::RESPONSE_RETRY):
        {
            this->LoadGame();

            break;
        }
        default:
        {
            rTunePrintf( "*** WARNING: Unhandled response for error [%d]!\n", m_lastError );
            rAssert( false );

            m_pParent->HandleMessage( GUI_MSG_MEMCARD_CHECK_COMPLETED );

            break;
        }
    }

    CGuiScreenLoadSave::HandleErrorResponse( response );
}

void
CGuiScreenAutoLoad::LoadGame()
{
	m_operation = LOAD;

#ifdef RAD_GAMECUBE
    // TC: GC does not require an auto-loading screen
    //
//    m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_AUTO_LOADING_GAME_GC, this );
    GetGameDataManager()->LoadGame( m_currentSlot, this );
    GetGameDataManager()->SetMinimumLoadSaveTime( 0 );
#endif

#ifdef RAD_PS2
    m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_AUTO_LOADING_GAME_PS2, this );
    GetGameDataManager()->SetMinimumLoadSaveTime( AUTO_LOAD_MINIMUM_DISPLAY_TIME );
#endif

#ifdef RAD_XBOX
    if( m_currentDriveIndex == 0 ) // xbox hard disk
    {
        m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_AUTO_LOADING_GAME_XBOX_HD, this );
    }
    else // xbox memory unit
    {
        m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_AUTO_LOADING_GAME_XBOX, this );
    }
#endif

#ifdef RAD_WIN32
    rAssert( m_currentDriveIndex == 0 );
    m_guiManager->DisplayMessage( CGuiScreenMessage::MSG_ID_AUTO_LOADING_GAME_PC, this );
#endif
}

