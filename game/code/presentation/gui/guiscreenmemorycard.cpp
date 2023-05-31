//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMemoryCard
//
// Description: Implementation of the CGuiScreenMemoryCard class.
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
#include <presentation/gui/guiscreenmemorycard.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guitextbible.h>
#include <presentation/gui/guiscreenprompt.h>
#include <presentation/gui/guimanager.h>

#include <data/gamedatamanager.h>
#include <data/memcard/memorycardmanager.h>
#include <events/eventmanager.h>
#include <gameflow/gameflow.h>
#include <memory/srrmemory.h>

#include <raddebug.hpp>     // Foundation
#include <layer.h>
#include <group.h>
#include <page.h>
#include <screen.h>
#include <strings/unicodestring.h>
#include <text.h>
#ifdef RAD_PS2
#include <libmtap.h>
#endif

const tColour DEFAULT_DISABLED_ITEM_COLOUR_GREY( 128, 128, 128 ); // the same as in guimenu.cpp

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

enum eMemoryCardMenuItem
{
    MENU_ITEM_MEMORY_DEVICE,

    NUM_MEMORY_CARD_MENU_ITEMS
};

bool CGuiScreenLoadSave::s_forceGotoMemoryCardScreen = false;

int CGuiScreenMemoryCard::s_currentMemoryCardSlot = 0;

//===========================================================================
// Public Member Functions
//===========================================================================

CGuiScreenLoadSave::CGuiScreenLoadSave( Scrooby::Screen* pScreen )
:   m_currentSlot( -1 ),
    m_lastError( Success ),
    m_currentMemoryDevice( NULL ),
    m_currentDriveIndex( -1 ),
	m_minimumFormatTime(1000),
	m_formatState(false),
    m_operation( SCREEN_OP_IDLE )
{
    rAssert( pScreen != NULL );
#ifdef RAD_WIN32
    Scrooby::Page* pPage = pScreen->GetPage( "GameSlots" );
    if( pPage != NULL )
    {
        Scrooby::Text* pText = pPage->GetText( "LoadSaveMessage" );
        if( pText != NULL )
        {
            pText->SetTextMode( Scrooby::TEXT_WRAP );
        }
    }
#else
    Scrooby::Page* pPage = pScreen->GetPage( "SelectMemoryDevice" );
	if( pPage != NULL )
    {
        Scrooby::Layer* foreground = pPage->GetLayer( "Foreground" );
        Scrooby::Text* pText = foreground->GetText( "LoadSave" );
        if( pText != NULL )
        {
            pText->SetTextMode( Scrooby::TEXT_WRAP );
        }

        m_currentMemoryDevice = foreground->GetText( "CurrentMemoryDevice" );
        rAssert( m_currentMemoryDevice != NULL );
        m_currentMemoryDevice->SetTextMode( Scrooby::TEXT_WRAP );

        // set platform-specific text
        //
        Scrooby::Group* selectMemoryDevice = foreground->GetGroup( "SelectMemoryDevice" );
        rAssert( selectMemoryDevice != NULL );

#ifdef RAD_XBOX
        selectMemoryDevice->SetVisible( false );
#endif

        pText = selectMemoryDevice->GetText( "SelectMemoryDevice" );
        if( pText != NULL )
        {
            pText->SetIndex( PLATFORM_TEXT_INDEX );

            // and apply wrapping
            //
            pText->SetTextMode( Scrooby::TEXT_WRAP );
        }
    }
#endif
}

CGuiScreenLoadSave::~CGuiScreenLoadSave()
{
}

void
CGuiScreenLoadSave::HandleMessage( eGuiMessage message,
                                   unsigned int param1,
                                   unsigned int param2 )
{
    switch( message )
    {
        case GUI_MSG_UPDATE:
        {
            GetMemoryCardManager()->Update( param1 );

            if( !GetMemoryCardManager()->IsCurrentDriveReady() )
            {
                this->GotoMemoryCardScreen();

                return;
            }

            break;
        }
        case GUI_MSG_CONTROLLER_AUX_X:
        {
#if defined( RAD_GAMECUBE ) || defined( RAD_PS2 )
            this->GotoMemoryCardScreen();

            GetEventManager()->TriggerEvent( EVENT_FE_MENU_SELECT );
#endif

            break;
        }
        default:
        {
            break;
        }
    }
}
void 
CGuiScreenLoadSave::FormatCurrentDrive()
{
	m_formatState = true;
	m_elapsedFormatTime = 0;
	int currentDrive = GetMemoryCardManager()->GetCurrentDriveIndex();
	m_formatDone = false;
	GetMemoryCardManager()->FormatDrive(currentDrive, this);
}

void
CGuiScreenLoadSave::OnFormatOperationComplete( radFileError errorCode )
{
	if (m_formatDone != true) // check if we already received an error or not
		m_formatResult = errorCode;
	m_formatDone = true;
}

void
CGuiScreenLoadSave::UpdateCurrentMemoryDevice()
{
#ifndef RAD_WIN32
    IRadDrive* currentDrive = GetMemoryCardManager()->GetCurrentDrive();
    if( currentDrive != NULL )
    {
        char textBibleEntry[ 32 ];
#ifdef RAD_GAMECUBE
        sprintf( textBibleEntry, "GC_%s", currentDrive->GetDriveName() );
#endif
#ifdef RAD_PS2
        sprintf( textBibleEntry, "PS2_MEMCARD1A:" );
#endif
#ifdef RAD_XBOX
        sprintf( textBibleEntry, "XBOX_%s", currentDrive->GetDriveName() );
#endif

        HeapMgr()->PushHeap( GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND ?
                             GMA_LEVEL_FE : GMA_LEVEL_HUD );

        UnicodeString deviceName;
        deviceName.ReadUnicode( GetTextBibleString( textBibleEntry ) );

#ifdef RAD_PS2
        int port_number = CGuiScreenMemoryCard::s_currentMemoryCardSlot/4;
        if (sceMtapGetConnection(port_number)==1) { // is multitap
            deviceName.Append('1' + (CGuiScreenMemoryCard::s_currentMemoryCardSlot/4));
            deviceName.Append('-' );
            deviceName.Append('A' + (CGuiScreenMemoryCard::s_currentMemoryCardSlot%4));
        }
        else {
            deviceName.Append('1' + (CGuiScreenMemoryCard::s_currentMemoryCardSlot/4));
        }
#endif																	    
#ifdef RAD_XBOX		
		const char *volname = GetMemoryCardManager()->GetCurrentDriveVolumeName();
		UnicodeString muName;
		if (volname[0]!=0) 
		{
			if (p3d::UnicodeStrLen((P3D_UNICODE*)( volname ) ) < MAX_MEM_CARD_NAME)
				muName.ReadUnicode((P3D_UNICODE*) volname );
			else
			{
				muName.ReadUnicode((P3D_UNICODE*) volname, MAX_MEM_CARD_NAME-1);
				UnicodeString ellipsis;
				ellipsis.ReadAscii("...");
				muName += ellipsis;
			} 
			deviceName.Append(' ');
			deviceName.Append('(');
			deviceName += muName;
			deviceName.Append(')');
		}
#endif

        rAssert( m_currentMemoryDevice != NULL );
        m_currentMemoryDevice->SetString( 0, deviceName );

        HeapMgr()->PopHeap(GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND ?
                             GMA_LEVEL_FE : GMA_LEVEL_HUD);
    }
#endif

    // update current drive index
    //
    m_currentDriveIndex = GetMemoryCardManager()->GetCurrentDriveIndex();
}

void
CGuiScreenLoadSave::HandleErrorResponse( CGuiMenuPrompt::ePromptResponse response )
{
}


//===========================================================================
// CGuiScreenMemoryCard::CGuiScreenMemoryCard
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
CGuiScreenMemoryCard::CGuiScreenMemoryCard
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_MEMORY_CARD,
                SCREEN_FX_FADE | SCREEN_FX_SLIDE_Y ),
    m_layerSelectMemoryDevice( NULL ),
    m_layerNoMemoryDevice( NULL ),
#ifdef RAD_XBOX
    m_numFreeBlocks( NULL ),
#endif
    m_pMenu( NULL ),
    m_numAttachedDevices( -1 )
{
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage;
	pPage = m_pScroobyScreen->GetPage( "MemoryCard" );
	rAssert( pPage );

    m_layerSelectMemoryDevice = pPage->GetGroup( "SelectMemoryDevice" );
    rAssert( m_layerSelectMemoryDevice );
    m_layerSelectMemoryDevice->SetVisible( false );

    m_layerNoMemoryDevice = pPage->GetGroup( "NoMemoryDevice" );
    rAssert( m_layerNoMemoryDevice );
    m_layerNoMemoryDevice->SetVisible( false );
 
	m_memStatusText = pPage->GetText("Status");
	rAssert(m_memStatusText);

	m_memStatusText->SetTextMode( Scrooby::TEXT_WRAP );

#ifdef RAD_XBOX
    Scrooby::Group* freeSpace = pPage->GetGroup( "FreeSpace" );
    rAssert( freeSpace != NULL );
    m_numFreeBlocks = freeSpace->GetText( "NumFreeBlocks" );
#else
    // hide free space display for non-Xbox platforms
    //
    Scrooby::Group* freeSpace = pPage->GetGroup( "FreeSpace" );
    rAssert( freeSpace != NULL );
    freeSpace->SetVisible( false );
#endif

    // set platform-specific text
    //
    Scrooby::Text* pText = m_layerSelectMemoryDevice->GetText( "SelectMemoryDevice" );
    if( pText != NULL )
    {
        pText->SetIndex( PLATFORM_TEXT_INDEX );

        // and apply wrapping
        //
        pText->SetTextMode( Scrooby::TEXT_WRAP );
    }

    pText = m_layerNoMemoryDevice->GetText( "NoMemoryDevice" );
    if( pText != NULL )
    {
        pText->SetIndex( PLATFORM_TEXT_INDEX );

        // and apply wrapping
        //
        pText->SetTextMode( Scrooby::TEXT_WRAP );
    }

    pText = m_layerSelectMemoryDevice->GetText( "MemoryDevice" );
    if( pText != NULL )
    {
        pText->SetTextMode( Scrooby::TEXT_WRAP );
    }

    // Create a menu.
    //
    m_pMenu = new CGuiMenu( this, NUM_MEMORY_CARD_MENU_ITEMS, GUI_TEXT_MENU, MENU_SFX_NONE );
    rAssert( m_pMenu != NULL );

    // Add menu items
    //
    Scrooby::Group* selectMemoryDevice = pPage->GetGroup( "SelectMemoryDevice" );
    m_pMenu->AddMenuItem( selectMemoryDevice->GetText( "SelectMemoryDevice" ),
                          selectMemoryDevice->GetText( "MemoryDevice" ),
                          NULL,
                          NULL,
                          selectMemoryDevice->GetSprite( "LArrow" ),
                          selectMemoryDevice->GetSprite( "RArrow" ) );
 	
	Scrooby::Text * selectDeviceText = selectMemoryDevice->GetText( "SelectMemoryDevice" );
	selectDeviceText->SetTextMode( Scrooby::TEXT_WRAP );

    this->AutoScaleFrame( m_pScroobyScreen->GetPage( "BigBoard" ) );
#ifdef RAD_XBOX
    for(int j = 0; j < radFileDriveMax; j++ )
    {
        m_driveMountedFlag[j] = NULL;
    }
#endif
}


//===========================================================================
// CGuiScreenMemoryCard::~CGuiScreenMemoryCard
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
CGuiScreenMemoryCard::~CGuiScreenMemoryCard()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenMemoryCard::HandleMessage
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
void CGuiScreenMemoryCard::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if ( message==GUI_MSG_PROMPT_START_RESPONSE )
    {
        m_pParent->HandleMessage( GUI_MSG_UNPAUSE_INGAME );
    }
    else if( m_state == GUI_WINDOW_STATE_RUNNING || message == GUI_MSG_MENU_PROMPT_RESPONSE)
    {
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
            case GUI_MSG_UPDATE:
            {
                GetMemoryCardManager()->Update( param1 );

                this->UpdateDeviceList();

                break;
            }

            case GUI_MSG_MENU_SELECTION_VALUE_CHANGED:
            {
                rAssert( param1 == MENU_ITEM_MEMORY_DEVICE );

                this->UpdateFreeSpace( param2 );

                break;
            }

            case GUI_MSG_MENU_SELECTION_MADE:
            {
                rAssert( param1 == MENU_ITEM_MEMORY_DEVICE );

                int selectedDevice = m_pMenu->GetSelectionValue( MENU_ITEM_MEMORY_DEVICE );

#ifdef RAD_XBOX
                // check if user selected full xbox hd
                if ( selectedDevice==0 
                    && !GetMemoryCardManager()->EnoughFreeSpace( 0 )
                    && !GetGameDataManager()->DoesSaveGameExist(m_availableDrives[ 0 ], false) )
                {
                    m_guiManager->DisplayPrompt(PROMPT_HD_FULL_XBOX, this, PROMPT_TYPE_CONTINUE);

                    break;
                } 
#endif
				GetMemoryCardManager()->SetCurrentDrive( m_availableDrives[ selectedDevice ] );

                s_currentMemoryCardSlot = GetMemoryCardManager()->GetCurrentDriveIndex();

                CGuiScreenLoadSave::s_forceGotoMemoryCardScreen = false;

                m_pParent->HandleMessage( GUI_MSG_BACK_SCREEN );

                break;
            }

            case GUI_MSG_CONTROLLER_BACK:
            {
                if( !GetMemoryCardManager()->IsCurrentDriveReady( true ) ||
                    CGuiScreenLoadSave::s_forceGotoMemoryCardScreen )
                {
                    if( GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND )
                    {
                        this->StartTransitionAnimation( 230, 260 );
                    }

                    m_pParent->HandleMessage( GUI_MSG_BACK_SCREEN, 1 );

                    GetEventManager()->TriggerEvent( EVENT_FE_MENU_BACK );
                }

                break;
            }
            case GUI_MSG_MENU_PROMPT_RESPONSE:
            {
                ReloadScreen();
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
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenMemoryCard::InitIntro
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
void CGuiScreenMemoryCard::InitIntro()
{


	this->UpdateDeviceList( true );

    // set selection to current device, if still attached
    //
    IRadDrive* currentDrive = GetMemoryCardManager()->GetCurrentDrive();
    if( currentDrive != NULL )
    {
        if( GetMemoryCardManager()->IsCurrentDriveReady( true ) )
        {
            for( int i = 0; i < m_numAttachedDevices; i++ )
            {
                if( m_availableDrives[ i ] == currentDrive )
                {
                    rAssert( m_pMenu );
                    m_pMenu->SetSelectionValue( MENU_ITEM_MEMORY_DEVICE, i );

                    break;
                }
            }
        }
    }
	this->UpdateFreeSpace( m_pMenu->GetSelectionValue( MENU_ITEM_MEMORY_DEVICE ) );
}


//===========================================================================
// CGuiScreenMemoryCard::InitRunning
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
void CGuiScreenMemoryCard::InitRunning()
{
}


//===========================================================================
// CGuiScreenMemoryCard::InitOutro
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
void CGuiScreenMemoryCard::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenMemoryCard::UpdateDeviceList( bool forceUpdate )
{
    IRadDrive* currentSelectedDrive = NULL;

#ifdef RAD_XBOX
    IRadDrive *driveMountedFlag[radFileDriveMax];
    int numAvailableDrives = GetMemoryCardManager()->GetAvailableDrives( m_availableDrives,
                                                                         m_mediaInfos,
                                                                         driveMountedFlag);
#else
    if( m_numAttachedDevices > 0 )
    {
        currentSelectedDrive = m_availableDrives[ m_pMenu->GetSelectionValue( MENU_ITEM_MEMORY_DEVICE ) ];
    }

    int numAvailableDrives = GetMemoryCardManager()->GetAvailableDrives( m_availableDrives,
                                                                         m_mediaInfos);
#endif

    bool hasMoreDrive = false;
    bool memoryDevicesAvailable = (numAvailableDrives > 0);

//    this->SetButtonVisible( BUTTON_ICON_BACK, !memoryDevicesAvailable );

    if( numAvailableDrives == m_numAttachedDevices &&
        !forceUpdate )
    {
        // since number of attached memory devices remain the same,
        // assume no other changes occurred since last poll
        //
        return;
    }
    else
    {
        if (numAvailableDrives > m_numAttachedDevices)
            hasMoreDrive = true;
        // update number of attached memory devices
        //
        m_numAttachedDevices = numAvailableDrives;
    }

	this->SetButtonVisible( BUTTON_ICON_ACCEPT, memoryDevicesAvailable );

    for( int i = 0; i < numAvailableDrives; i++ )
    {
        rAssert( m_availableDrives[ i ] );
        rAssert( m_mediaInfos[ i ] );

        char textBibleEntry[ 32 ];
#ifdef RAD_GAMECUBE
        sprintf( textBibleEntry, "GC_%s", m_availableDrives[ i ]->GetDriveName() );
#endif
#ifdef RAD_PS2
        sprintf( textBibleEntry, "PS2_MEMCARD1A:" );
#endif
#ifdef RAD_XBOX
        sprintf( textBibleEntry, "XBOX_%s", m_availableDrives[ i ]->GetDriveName() );
#endif
#ifdef RAD_WIN32
        sprintf( textBibleEntry, "XBOX_U:" );  // Temp.
#endif

        HeapMgr()->PushHeap( GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND ?
                             GMA_LEVEL_FE : GMA_LEVEL_HUD );

        UnicodeString deviceName;
        deviceName.ReadUnicode( GetTextBibleString( textBibleEntry ) );

#ifdef RAD_PS2
        int drive_index = GetMemoryCardManager()->GetDriveIndex(m_availableDrives[ i ]);
        int port_number = drive_index/4;
        if (sceMtapGetConnection(port_number)==1) { // is multitap
            deviceName.Append('1' + (drive_index/4));
            deviceName.Append('-' );
            deviceName.Append('A' + (drive_index%4));
        }
        else {
            deviceName.Append('1' + drive_index/4);
        }
#endif
		
#ifdef RAD_XBOX		
		if (i > 0 && m_mediaInfos[ i ]->m_VolumeName[0]!=0) // i==0 is the hard disk
		{
			UnicodeString muName;
			if (p3d::UnicodeStrLen((P3D_UNICODE*)( m_mediaInfos[ i ]->m_VolumeName) ) < MAX_MEM_CARD_NAME)
				muName.ReadUnicode((P3D_UNICODE*) m_mediaInfos[ i ]->m_VolumeName);
			else
			{
				muName.ReadUnicode((P3D_UNICODE*) m_mediaInfos[ i ]->m_VolumeName, MAX_MEM_CARD_NAME-1);
				UnicodeString ellipsis;
				ellipsis.ReadAscii("...");
				muName += ellipsis;
			} 
			deviceName.Append(' ');
			deviceName.Append('(');
			deviceName += muName;
			deviceName.Append(')');


		}
#endif

 
 

        Scrooby::Text* memoryDeviceText = dynamic_cast<Scrooby::Text*>( m_pMenu->GetMenuItem( MENU_ITEM_MEMORY_DEVICE )->GetItemValue() );
        rAssert( memoryDeviceText != NULL );
        memoryDeviceText->SetString( i, deviceName );

        HeapMgr()->PopHeap(GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND ?
                             GMA_LEVEL_FE : GMA_LEVEL_HUD);

    }

    m_layerSelectMemoryDevice->SetVisible( memoryDevicesAvailable );
    m_layerNoMemoryDevice->SetVisible( !memoryDevicesAvailable );

    rAssert( m_pMenu );
    if( memoryDevicesAvailable )
    {
        m_pMenu->GetMenuItem( MENU_ITEM_MEMORY_DEVICE )->m_attributes |= SELECTABLE;
    }
    else
    {
        m_pMenu->GetMenuItem( MENU_ITEM_MEMORY_DEVICE )->m_attributes &= ~SELECTABLE;
        m_pMenu->SetSelectionValueCount( MENU_ITEM_MEMORY_DEVICE, 1 ); // to disable left/right scrolling
    }
//    m_pMenu->SetMenuItemEnabled( MENU_ITEM_MEMORY_DEVICE, memoryDevicesAvailable );

    if( memoryDevicesAvailable )
    {
        m_pMenu->SetSelectionValueCount( MENU_ITEM_MEMORY_DEVICE, numAvailableDrives );

        m_pMenu->SetSelectionValue( MENU_ITEM_MEMORY_DEVICE, 0 );

#ifndef RAD_XBOX
        for( int i = 0; i < m_numAttachedDevices; i++ )
        {
            if( m_availableDrives[ i ] == currentSelectedDrive )
            {
                m_pMenu->SetSelectionValue( MENU_ITEM_MEMORY_DEVICE, i );

                break;
            }
        }
#endif
    }
#ifdef RAD_XBOX
    int j;
    if (hasMoreDrive)  // find which drive is new and select it
    {
        for(j = 0; j < radFileDriveMax; j++ )
        {
            if (driveMountedFlag[j] && m_driveMountedFlag[j]==NULL)
            {
                for (int x = 0; x < numAvailableDrives; x++)
                {
                    if (m_availableDrives[x] == driveMountedFlag[j])
                    {
                        m_pMenu->SetSelectionValue( MENU_ITEM_MEMORY_DEVICE, x );
                        break;
                    }
                }
                break;
            }
        }
        rAssert(j<radFileDriveMax); // we must find the different drive
    }

    for(j = 0; j < radFileDriveMax; j++ )
    {
        m_driveMountedFlag[j] = driveMountedFlag[j];
    }
#endif
}

void
CGuiScreenMemoryCard::UpdateFreeSpace( unsigned int currentDriveIndex )
{
    Scrooby::BoundedDrawable* menu_drawable 
        = m_pMenu->GetMenuItem(MENU_ITEM_MEMORY_DEVICE)->GetItemValue();
    
    menu_drawable->SetColour(m_pMenu->GetHighlightColour());

    m_memStatusText->SetVisible(false);
 
    // enable selection
    //
    if( m_numAttachedDevices > 0 )
    {
        m_pMenu->GetMenuItem( MENU_ITEM_MEMORY_DEVICE )->m_attributes |= SELECTABLE;
        SetButtonVisible( BUTTON_ICON_ACCEPT, true );
    }

	HeapMgr()->PushHeap( GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND ?
                         GMA_LEVEL_FE : GMA_LEVEL_HUD );

#ifdef RAD_XBOX
	if( static_cast<int>( currentDriveIndex ) < m_numAttachedDevices )
	{
		const unsigned int MAX_NUM_FREE_BLOCKS = 50000;
        unsigned int numFreeBlocks = m_mediaInfos[ currentDriveIndex ]->m_FreeSpace /
            NUM_BYTES_PER_BLOCK;

		char buffer[ 16 ];
		if( numFreeBlocks > MAX_NUM_FREE_BLOCKS )
		{
			sprintf( buffer, "50,000+" );
		}
		else
		{
			if( numFreeBlocks < 1000 )
			{
				sprintf( buffer, "%d", numFreeBlocks );
			}
			else
			{
				sprintf( buffer, "%d,%03d", numFreeBlocks / 1000, numFreeBlocks % 1000 );
			}
		}

		rAssert( m_numFreeBlocks != NULL );
		m_numFreeBlocks->SetString( 0, buffer );

		m_pMenu->GetMenuItem( MENU_ITEM_MEMORY_DEVICE )->m_attributes |= SELECTABLE;
		SetButtonVisible(BUTTON_ICON_ACCEPT , true);
	}
#endif // RAD_XBOX

	if (((int)currentDriveIndex < m_numAttachedDevices) && m_mediaInfos[ currentDriveIndex ]->m_MediaState != IRadDrive::MediaInfo::MediaPresent)
	{
		m_memStatusText->SetVisible(true);
		m_memStatusText->SetIndex(
			(m_mediaInfos[ currentDriveIndex ]->m_MediaState - IRadDrive::MediaInfo::MediaNotFormatted) * 3 
			+ PLATFORM_TEXT_INDEX);
		bool disable = true;

#ifdef RAD_GAMECUBE
        // on GameCube, if memory card is unformatted or formatted for another market, allow it to be
        // selected so that if can be formatted
        //
        if( m_mediaInfos[ currentDriveIndex ]->m_MediaState == IRadDrive::MediaInfo::MediaNotFormatted ||
            m_mediaInfos[ currentDriveIndex ]->m_MediaState == IRadDrive::MediaInfo::MediaEncodingErr )
        {
            disable = false;

            m_memStatusText->SetVisible( false ); // no need to display status text
        }
#endif // RAD_GAMECUBE

#ifdef RAD_PS2
        // on PS2, if memory card is unformatted, allow it to be selected so that it can be formatted
        //
        if( m_mediaInfos[ currentDriveIndex ]->m_MediaState == IRadDrive::MediaInfo::MediaNotFormatted )
        {
            disable = false;

            m_memStatusText->SetVisible( false ); // no need to display status text
        }
#endif // RAD_PS2

		if (disable) 
		{
			// disable selection
			m_pMenu->GetMenuItem( MENU_ITEM_MEMORY_DEVICE )->m_attributes &= ~SELECTABLE;
			SetButtonVisible(BUTTON_ICON_ACCEPT , false);
            // change color
            menu_drawable->SetColour( DEFAULT_DISABLED_ITEM_COLOUR_GREY );
		}
	}
    else // no error
    {
        if ( ((int)currentDriveIndex < m_numAttachedDevices) && GetMemoryCardManager()->IsMemcardInfoLoaded() )
        {
            // this drive index is different than currentDriveIndex
            int memcard_driveindex = GetMemoryCardManager()->GetDriveIndex(m_availableDrives[ currentDriveIndex ]);
            rTuneAssert(memcard_driveindex >= 0);
            if ( GetMemoryCardManager()->EnoughFreeSpace( memcard_driveindex )==false
                && GetGameDataManager()->DoesSaveGameExist(m_availableDrives[ currentDriveIndex ], false)==false ) // check for full without save game
            { 
                int message_index = 5 * 3 + PLATFORM_TEXT_INDEX;
    #ifdef RAD_XBOX
                if (currentDriveIndex==0) // hd
                    message_index ++;
    #endif 
                m_memStatusText->SetVisible(true);
                m_memStatusText->SetIndex( message_index );

#ifdef RAD_GAMECUBE
                // append "Use Memory Card Screen" text to message; this is done in
                // code because the text bible compiler can't handle strings with
                // more than 255 characters
                //
                UnicodeString useMemCardScreen;
                useMemCardScreen.ReadUnicode( GetTextBibleString( "USE_MEMORY_CARD_SCREEN" ) );

                UnicodeString newString;
                newString.ReadUnicode( GetTextBibleString( "MU_FULL_STATUS_(GC)" ) );
                newString.Append( ' ' );
                newString += useMemCardScreen;

                m_memStatusText->SetString( message_index, newString );
#endif // RAD_GAMECUBE

                bool disable = true;
#ifdef RAD_XBOX
                if (currentDriveIndex==0) // hd
                    disable = false;
#endif 

                if (disable)
                {
                    // change color
                    menu_drawable->SetColour( DEFAULT_DISABLED_ITEM_COLOUR_GREY ); 

                    // disable selection
                    m_pMenu->GetMenuItem( MENU_ITEM_MEMORY_DEVICE )->m_attributes &= ~SELECTABLE;
                    SetButtonVisible(BUTTON_ICON_ACCEPT , false);
                }

            }
        }
    }

	HeapMgr()->PopHeap( GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND ?
						GMA_LEVEL_FE : GMA_LEVEL_HUD );

}

