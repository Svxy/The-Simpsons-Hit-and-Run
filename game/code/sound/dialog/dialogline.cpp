//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialogline.cpp
//
// Description: Atomic unit of dialog.  A DialogLine object represents a 
//              complete line of dialog spoken by a single character.
//
// History:     02/09/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radkey.hpp>
#include <radnamespace.hpp>
#include <p3d/anim/skeleton.hpp>

//========================================
// Project Includes
//========================================
#include <sound/dialog/dialogline.h>

#include <sound/dialog/dialogselectiongroup.h>

#include <sound/simpsonssoundplayer.h>
#include <sound/soundrenderer/idasoundresource.h>
#include <sound/soundrenderer/soundrenderingmanager.h>

#include <mission/gameplaymanager.h>
#include <worldsim/character/character.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

EventTableEntry eventTable[] =
{
    { "GIC", 0, EVENT_GETINTOVEHICLE_START, 2500 },
    { "GOC", 0, EVENT_GETOUTOFVEHICLE_START, 2500 },
    { "convinit", 0, EVENT_CONVERSATION_INIT_DIALOG, 2500 },
    { "noboxconv", 0, EVENT_IN_GAMEPLAY_CONVERSATION, 2500 },
    { "tutorial", 0, EVENT_TUTORIAL_DIALOG_PLAY, 2500 },
    { "Mcrash", 0, EVENT_MINOR_VEHICLE_CRASH, 2500 },
    { "Bcrash", 0, EVENT_BIG_VEHICLE_CRASH, 2500 },
    { "Arrive", 0, EVENT_DESTINATION_REACHED, 2500 },
    { "Air", 0, EVENT_BIG_AIR, 2500 },
    { "Burn", 0, EVENT_BURNOUT, 2500 },
    { "ObjectW", 0, EVENT_COLLECT_OBJECT, 2500 },
    { "Break", 0, EVENT_HIT_BREAKABLE, 2500 },
    { "Mfail", 0, EVENT_MISSION_FAILURE, 4000 },
    { "Mvic", 0, EVENT_MISSION_SUCCESS_DIALOG, 4000 },
    { "Tail", 0, EVENT_TAIL_LOST_DIALOG, 2500 },
    { "NewAI", 0, EVENT_CHASE_VEHICLE_PROXIMITY, 2500 },
    { "Time", 0, EVENT_TIME_RUNNING_OUT, 2500 },
    { "Pass", 0, EVENT_RACE_PASSED_AI, 2500 },
    { "Passed", 0, EVENT_RACE_GOT_PASSED_BY_AI, 2500 },
    { "Activate", 0, EVENT_BIG_RED_SWITCH_PRESSED, 250 },  // Make this quite short
    { "Fall", 0, EVENT_DEATH_VOLUME_SOUND, 2500 },
    { "Char", 0, EVENT_PEDESTRIAN_SMACKDOWN, 2500 },
    { "HitByW", 0, EVENT_KICK_NPC_SOUND, 2500 },
    { "BreakCa", 0, EVENT_BREAK_CAMERA_OR_BOX, 2500 },
    { "Turbo", 0, EVENT_CHARACTER_TIRED_NOW, 2500 },
    { "Springboard", 0, static_cast<EventEnum>(EVENT_LOCATOR + LocatorEvent::BOUNCEPAD), 2500 },
    { "NHitByC", 0, EVENT_PEDESTRIAN_DODGE, 2500 },
    { "HitByC", 0, EVENT_PLAYER_CAR_HIT_NPC, 2500 },
    { "HitP", 0, EVENT_PLAYER_MAKES_LIGHT_OF_CAR_HITTING_NPC, 2500 },
    { "Damage", 0, EVENT_BIG_CRASH, 2500 },
    { "Door", 0, EVENT_WRONG_SIDE_DUMBASS, 2500 },
    { "CarWay", 0, EVENT_TRAFFIC_IMPEDED, 2500 },
    { "Doorbell", 0, EVENT_DING_DONG, 2500 },
    { "Askride", 0, EVENT_PHONE_BOOTH_RIDE_REQUEST, 0 },
    { "Ridereply", 0, EVENT_PHONE_BOOTH_NEW_VEHICLE_SELECTED, 0 },
    { "Answer", 0, EVENT_PHONE_BOOTH_OLD_VEHICLE_RESELECTED, 0 },
    { "HitCar", 0, EVENT_VILLAIN_CAR_HIT_PLAYER, 2500 },
    { "Greeting", 0, EVENT_AMBIENT_GREETING, 2500 },
    { "Idlereply", 0, EVENT_AMBIENT_RESPONSE, 0 },
    { "Askfood", 0, EVENT_AMBIENT_ASKFOOD, 2500 },
    { "Foodreply", 0, EVENT_AMBIENT_FOODREPLY, 0 },
    { "CarBuy", 0, EVENT_HAGGLING_WITH_GIL, 2500 },
    { "Dcar", 0, EVENT_BIG_BOOM_SOUND, 5000 },
    { "Card", 0, EVENT_CARD_COLLECTED, 2500 },
    { "Mstart", 0, EVENT_MISSION_BRIEFING_ACCEPTED, 0 }
};

static unsigned int eventTableLength = sizeof( eventTable ) / sizeof( EventTableEntry );

CharacterTableEntry characterTable[] =
{
    { 0, "Hom", 0, "homer" },
    { 0, "Mrg", 0, "marge" },
    { 0, "Brt", 0, "bart" },
    { 0, "Lis", 0, "lisa" },
    { 0, "Apu", 0, "apu" },
    { 0, "Agn", 0, "askinner" },
    { 0, "Brn", 0, "barney" },
    { 0, "Bee", 0, "beeman" },
    { 0, "Crl", 0, "carl" },
    { 0, "Cbg", 0, "cbg" },
    { 0, "Clt", 0, "cletus" },
    { 0, "Dol", 0, "dolph" },
    { 0, "Nic", 0, "nriviera" },
    { 0, "Frk", 0, "frink" },
    { 0, "Fla", 0, "ned" },
    { 0, "By1", 0, "boy1" },
    { 0, "By2", 0, "boy2" },
    { 0, "Fm1", 0, "fem1" },
    { 0, "Fm2", 0, "fem2" },
    { 0, "Gil", 0, "gil" },
    { 0, "Gr1", 0, "girl1" },
    { 0, "Gr2", 0, "girl2" },
    { 0, "Kan", 0, "kang" },
    { 0, "Kod", 0, "kodos" },
    { 0, "Mn1", 0, "male1" },
    { 0, "Mn2", 0, "male2" },
    { 0, "Grp", 0, "grandpa" },
    { 0, "Mol", 0, "moleman" },
    { 0, "Jas", 0, "jasper" },
    { 0, "Jim", 0, "jimbo" },
    { 0, "Kea", 0, "kearney" },
    { 0, "Kru", 0, "krusty" },
    { 0, "Len", 0, "lenny" },
    { 0, "Loe", 0, "lou" },
    { 0, "Lou", 0, "louie" },
    { 0, "Mil", 0, "milhouse" },
    { 0, "Moe", 0, "moe" },
    { 0, "Nel", 0, "nelson" },
    { 0, "Oto", 0, "otto" },
    { 0, "Pat", 0, "patty" },
    { 0, "Qim", 0, "quimby" },
    { 0, "Ral", 0, "ralph" },
    { 0, "Sea", 0, "captain" },
    { 0, "Sel", 0, "selma" },
    { 0, "Skn", 0, "skinner" },
    { 0, "Smi", 0, "smithers" },
    { 0, "Snk", 0, "snake" },
    { 0, "Svt", 0, "teen" },
    { 0, "Wig", 0, "wiggum" },
    { 0, "Wil", 0, "willie" },
    { 0, "Zom", 0, "zombie" },
    { 0, "Zm1", 0, "zombie1" },
    { 0, "Zm2", 0, "zombie2" },
    { 0, "Zm3", 0, "zombie3" },
    { 0, "Zm4", 0, "zombie4" },
    { 0, "Hib", 0, "hibbert" },
    { 0, "Bur", 0, "burns" },
    { 0, "Rod", 0, "rod" },
    { 0, "Todd", 0, "todd" },
    { 0, "Bkm", 0, "brockman" },
    { 0, "Hbn", 0, "homerbrain" },
    { 0, "Vm1", 0, "traffic1" },
    { 0, "Vm2", 0, "traffic2" },
    { 0, "Vm3", 0, "traffic3" },
    { 0, "Vm4", 0, "traffic4" }
};

static unsigned int characterTableLength = sizeof( characterTable ) / sizeof( CharacterTableEntry );

//
// The food guys
//
static unsigned int APU_INDEX = 4;
static unsigned int TEEN_INDEX = 47;

//
// Arbitrary number, used in field name buffers
//
static const int FIELD_BUFFER_LEN = 64;
static const int MY_FILENAME_BUFFER_LEN = 150;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// DialogLine::DialogLine
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
DialogLine::DialogLine( IDaSoundResource* resource )
{
    m_resource = resource;
    m_characterIndex = -1;
    m_role = ROLE_NONE;
    m_conversationPosition = NOT_CONVERSATION_LINE;
    m_ConversationName = 0;
    
    static bool tablesInitialized = false;

    if( !tablesInitialized )
    {
        initializeTables();
        tablesInitialized = true;
    }

    parseResourceFilename();
}

//==============================================================================
// DialogLine::~DialogLine
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
DialogLine::~DialogLine()
{
}

//==============================================================================
// DialogLine::PlayLine
//==============================================================================
// Description: Play the dialog line
//
// Parameters:	lineIndex - ignored, only used for other SelectableDialog subclasses
//              player - what we use for the playing
//              callback - who we call when we're done
//
// Return:      N/A.
//
//==============================================================================
void DialogLine::PlayLine( unsigned int lineIndex,
                           SimpsonsSoundPlayer& player,
                           SimpsonsSoundPlayerCallback* callback )
{
    player.PlayResource( m_resource, callback );
}

//=============================================================================
// DialogLine::QueueLine
//=============================================================================
// Description: Buffer a line of dialog for playback
//
// Parameters:  lineIndex - ignored, only used for other SelectableDialog subclasses
//              player - what we use for the queueing
//
// Return:      void 
//
//=============================================================================
void DialogLine::QueueLine( unsigned int lineIndex, SimpsonsSoundPlayer& player )
{
    player.QueueSound( m_resource );
}

//=============================================================================
// DialogLine::PlayQueuedLine
//=============================================================================
// Description: Play the sound we queued with QueueLine
//
// Parameters:  player - what we use for the playing
//              callback - if non-NULL, object to notify when playback done
//
// Return:      void 
//
//=============================================================================
void DialogLine::PlayQueuedLine( SimpsonsSoundPlayer& player,
                                 SimpsonsSoundPlayerCallback* callback )
{
    player.PlayQueuedSound( callback );
}

//=============================================================================
// DialogLine::StripDirectoryCrud
//=============================================================================
// Description: Utility function for stripping directory prefix from filenames
//
// Parameters:  filename - filename to strip
//              buffer - holds stripped filename
//              bufferLen - length of buffer
//
// Return:      void 
//
//=============================================================================
void DialogLine::StripDirectoryCrud( const char* filename, char* buffer, int bufferLen )
{
    const char* charPtr;
    int i, j;
    int strippedLength = 0;

    i = strlen( filename );
    charPtr = &(filename[i]);
    while( i > 0 )
    {
        if( ( *charPtr == '/' ) || ( *charPtr == '\\' ) )
        {
            break;
        }
        else
        {
            ++strippedLength;
            --i;
            --charPtr;
        }
    }

    //
    // Copy stripped filename to buffer
    //
    if( i > 0 )
    {
        ++charPtr;
    }

    if( strippedLength > bufferLen )
    {
        strippedLength = bufferLen;
    }

    for( j = 0; j < strippedLength; j++ )
    {
        buffer[j] = *charPtr++;
    }
}

//=============================================================================
// DialogLine::GetEventTableEntry
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int index )
//
// Return:      EventTableEntry
//
//=============================================================================
const EventTableEntry* DialogLine::GetEventTableEntry( unsigned int index )
{
    if( index >= eventTableLength )
    {
        return( NULL );
    }
    else
    {
        return( &(eventTable[index]) );
    }
}

//=============================================================================
// DialogLine::GetEventTableSize
//=============================================================================
// Description: Return the number of entries in the event table
//
// Parameters:  None
//
// Return:      table size
//
//=============================================================================
unsigned int DialogLine::GetEventTableSize()
{
    return( eventTableLength );
}

//=============================================================================
// DialogLine::GetCharacterTableEntry
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int index )
//
// Return:      CharacterTableEntry
//
//=============================================================================
const CharacterTableEntry* DialogLine::GetCharacterTableEntry( unsigned int index )
{
    if( index >= characterTableLength )
    {
        return( NULL );
    }
    else
    {
        return( &(characterTable[index]) );
    }
}

//=============================================================================
// DialogLine::GetCharacterTableSize
//=============================================================================
// Description: Return the number of entries in the character table
//
// Parameters:  None
//
// Return:      table size
//
//=============================================================================
unsigned int DialogLine::GetCharacterTableSize()
{
    return( characterTableLength );
}

//=============================================================================
// DialogLine::UsesCharacter
//=============================================================================
// Description: Indicate whether this dialog line comes from
//              the character given
//
// Parameters:  characterObj - character to match
//
// Return:      true if character matches this line, false otherwise
//
//=============================================================================
bool DialogLine::UsesCharacter( tUID characterUID )
{
    return( characterUID == GetCharacterUID( ) );
}

//=============================================================================
// DialogLine::AddMatchingDialog
//=============================================================================
// Description: Request to a SelectableDialog object (this one) to add a new
//              DialogLine or such with the same characteristics.  Since this
//              object represents a single line, we need to create a
//              DialogSelectionGroup and add self and the new dialog line to it,
//              and put it in our spot in the list.
//
// Parameters:  newDialog - dialogLine that matches this one
//
// Return:      void 
//
//=============================================================================
void DialogLine::AddMatchingDialog( SelectableDialog& newDialog, SelectableDialogList& list )
{
    HeapMgr()->PushHeap( GMA_AUDIO_PERSISTENT );

    DialogSelectionGroup* group = new DialogSelectionGroup( *this, newDialog );

    rAssert( group != NULL );

    list.remove( this );
    list.push_back( group );

    HeapMgr()->PopHeap( GMA_AUDIO_PERSISTENT );
}

//=============================================================================
// DialogLine::GetConversationName
//=============================================================================
// Description: Returns the name of the conversation this line belongs to,
//              if this is a conversation line.  We'll calculate it on the
//              fly, no sense in wasting more space than we already are.
//
// Parameters:  None
//
// Return:      radKey32 representation of conversation name field, or 0
//              if this isn't a conversation line
//
//=============================================================================

void DialogLine::parseConversationName( )
{    
    bool fieldFound;
    char fieldBuffer[FIELD_BUFFER_LEN];
    char filenameBuffer[MY_FILENAME_BUFFER_LEN];
    char tempBuffer[MY_FILENAME_BUFFER_LEN];

    //
    // Strip the directory crud
    //
    m_resource->GetFileNameAt( 0, tempBuffer, MY_FILENAME_BUFFER_LEN );
    StripDirectoryCrud( tempBuffer, filenameBuffer, MY_FILENAME_BUFFER_LEN );

    fieldFound = getNameField( filenameBuffer, 0, fieldBuffer, FIELD_BUFFER_LEN );
    rAssert( fieldFound );

    if( ( fieldBuffer[0] == 'C') && ( fieldBuffer[1] == '\0' ) )
    {
        //
        // Conversation line
        //
        fieldFound = getNameField( filenameBuffer, 1, fieldBuffer, FIELD_BUFFER_LEN );
        m_ConversationName = radMakeKey32( fieldBuffer );
    }
    else
    {
        //
        // Not conversation line
        //
        rTuneAssert( false );
    }
}

//=============================================================================
// DialogLine::IsFoodCharacter
//=============================================================================
// Description: Determine whether this character gets Askfood lines or
//              Idlereply
//
// Parameters:  theGuy - ambient character being talked to
//
// Return:      true for Askfood, false for Idlereply 
//
//=============================================================================
bool DialogLine::IsFoodCharacter( Character* theGuy )
{
    tUID UID1;
    bool retVal = false;

    choreo::Puppet* puppet = theGuy->GetPuppet();
    if( puppet != NULL )
    {
        UID1 = puppet->GetPose()->GetSkeleton()->GetUID();

        //
        // Hard-coded hack
        //
        rAssert( strcmp( "Apu", characterTable[APU_INDEX].characterString ) == 0 );
        rAssert( strcmp( "Svt", characterTable[TEEN_INDEX].characterString ) == 0 );

        if( ( UID1 == static_cast< tUID >( characterTable[APU_INDEX].realCharacterUID ) )
            || ( UID1 == static_cast< tUID >( characterTable[TEEN_INDEX].realCharacterUID ) ) )
        {
            retVal = true;
        }
    }

    return( retVal );
}

//=============================================================================
// DialogLine::GetLifeInMsecsForEvent
//=============================================================================
// Description: Given an event, return the number of milliseconds that the
//              dialog for that event should be allowed to live in the queue
//
// Parameters:  eventID - ID for event
//
// Return:      lifetime in msecs, 0 for infinite lifetime
//
//=============================================================================
unsigned int DialogLine::GetLifeInMsecsForEvent( EventEnum eventID )
{
    unsigned int i;
    unsigned int lifetime = 2500;  // 2.5 sec. default

    for( i = 0; i < eventTableLength; i++ )
    {
        if( eventTable[i].event == eventID )
        {
            lifetime = eventTable[i].lifeInMsecs;
            break;
        }
    }

    return( lifetime );
}

//=============================================================================
// DialogLine::FillCharacterName
//=============================================================================
// Description: Fill the given buffer with the text name of the character
//              matching the given UID
//
// Parameters:  buffer - buffer to write name to
//              bufferSize - length of buffer
//              characterUID - UID of character to find name for
//
// Return:      void 
//
//=============================================================================
void DialogLine::FillCharacterName( char* buffer, unsigned int bufferSize, tUID characterUID )
{
    unsigned int i;
    unsigned int tableSize;
    const CharacterTableEntry* charTableLine;

    tableSize = GetCharacterTableSize();

    for( i = 0; i < tableSize; i++ )
    {
        charTableLine = GetCharacterTableEntry( i );
        if( charTableLine->realCharacterUID == characterUID )
        {
            break;
        }
    }

    if( i < tableSize )
    {
        rAssert( strlen( charTableLine->characterString ) < bufferSize );
        strcpy( buffer, charTableLine->characterString );
    }
    else
    {
        rAssert( bufferSize >= 4 );
        strcpy( buffer, "???" );
    }
}

//=============================================================================
// DialogLine::FillEventName
//=============================================================================
// Description: Fill the given buffer with the text name of the event
//              matching the given ID
//
// Parameters:  buffer - buffer to write name to
//              bufferSize - length of buffer
//              eventID - ID of event to find name for
//
// Return:      void 
//
//=============================================================================
void DialogLine::FillEventName( char* buffer, unsigned int bufferSize, EventEnum eventID )
{
    unsigned int i;
    unsigned int tableSize;
    const EventTableEntry* eventTableLine;

    tableSize = GetEventTableSize();

    for( i = 0; i < tableSize; i++ )
    {
        eventTableLine = GetEventTableEntry( i );
        if( eventTableLine->event == eventID )
        {
            break;
        }
    }

    if( i < tableSize )
    {
        rAssert( strlen( eventTableLine->eventString ) < bufferSize );
        strcpy( buffer, eventTableLine->eventString );
    }
    else
    {
        rAssert( bufferSize >= 4 );
        strcpy( buffer, "???" );
    }
}

//=============================================================================
// DialogLine::PrintResourceName
//=============================================================================
// Description: Dump out the name of the sound resource.  For debugging.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogLine::PrintResourceName()
{
#ifndef RAD_RELEASE
    char buffer[256];

    rAssert( m_resource != NULL );

    m_resource->GetFileNameAt( 0, buffer, 256 );
    rDebugPrintf( "DialogLine - %s\n", buffer );
#endif
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// DialogLine::parseResourceFilename
//=============================================================================
// Description: Look at the resource filename and fill out the dialog line
//              attributes based on what we find
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogLine::parseResourceFilename()
{
    bool fieldFound;
    char fieldBuffer[FIELD_BUFFER_LEN];
    char filenameBuffer[MY_FILENAME_BUFFER_LEN];
    char tempBuffer[MY_FILENAME_BUFFER_LEN];

    //
    // Strip the directory crud
    //
    m_resource->GetFileNameAt( 0, tempBuffer, MY_FILENAME_BUFFER_LEN );
    StripDirectoryCrud( tempBuffer, filenameBuffer, MY_FILENAME_BUFFER_LEN );

    fieldFound = getNameField( filenameBuffer, 0, fieldBuffer, FIELD_BUFFER_LEN );
    rAssert( fieldFound );

    if( ( fieldBuffer[0] == 'C') && ( fieldBuffer[1] == '\0' ) )
    {
        //
        // Conversation line
        //
        parseConversationName( );
        matchOrderField( filenameBuffer, 2 );
        matchEventField( filenameBuffer, 3 );
        matchCharacterField( filenameBuffer, 4 );
        matchLevelField( filenameBuffer, 5 );
    }
    else
    {
        //
        // One-liner
        //
        matchRoleField( filenameBuffer, 0 );
        matchEventField( filenameBuffer, 1 );
        matchCharacterField( filenameBuffer, 2 );
        matchLevelField( filenameBuffer, 3 );
    }
}

void DialogLine::matchRoleField( const char* filename, int field )
{
    char buffer[FIELD_BUFFER_LEN];
    bool fieldFound;

    fieldFound = getNameField( filename, field, buffer, FIELD_BUFFER_LEN );
    rAssert( fieldFound );

    switch( buffer[0] )
    {
    case 'W':
        m_role = ROLE_WALKER;
        break;
    case 'D':
        m_role = ROLE_DRIVER;
        break;
    case 'P':
        m_role = ROLE_PEDESTRIAN;
        break;
    case 'V':
        m_role = ROLE_VILLAIN;
        break;
    default:
        rAssertMsg( false, "Unknown role field in dialog file\n" );
        break;
    }
}

//=============================================================================
// DialogLine::matchEventField
//=============================================================================
// Description: Parse the field at the given position and store the eventEnum
//              that matches it
//
// Parameters:  field - number of field to parse
//
// Return:      void 
//
//=============================================================================
void DialogLine::matchEventField( const char* filename, int field )
{
    unsigned int i;
    char buffer[FIELD_BUFFER_LEN];
    radKey32 fieldKey;
    bool fieldFound;

    //
    // Event name.  Use a lookup table to translate to the event
    // enumeration.
    //
    fieldFound = getNameField( filename, field, buffer, FIELD_BUFFER_LEN );
    rAssert( fieldFound );

    fieldKey = ::radMakeCaseInsensitiveKey32( buffer );
    for( i = 0; i < eventTableLength; i++ )
    {
        if( fieldKey == eventTable[i].eventKey )
        {
            m_event = eventTable[i].event;
            break;
        }
    }
}

//=============================================================================
// DialogLine::matchOrderField
//=============================================================================
// Description: Parse the field at the given position and store it as the value
//              for the conversation position
//
// Parameters:  field - number of field to parse
//
// Return:      void 
//
//=============================================================================
void DialogLine::matchOrderField( const char* filename, int field )
{
    char buffer[FIELD_BUFFER_LEN];
    bool fieldFound;

    //
    // Event name.  Use a lookup table to translate to the event
    // enumeration.
    //
    fieldFound = getNameField( filename, field, buffer, FIELD_BUFFER_LEN );
    rAssert( fieldFound );

    rAssert( buffer[0] >= '0' );
    rAssert( buffer[0] <= '9' );

    m_conversationPosition = buffer[0] - '0';
}

//=============================================================================
// DialogLine::matchCharacterField
//=============================================================================
// Description: Parse the field at the given position and store the characterEnum
//              that matches it
//
// Parameters:  field - number of field to parse
//
// Return:      void 
//
//=============================================================================
void DialogLine::matchCharacterField( const char* filename, int field )
{
    unsigned int i;
    char buffer[FIELD_BUFFER_LEN];
    radKey32 fieldKey;
    bool fieldFound;

    //
    // Character.  Use another lookup table.
    //
    fieldFound = getNameField( filename, field, buffer, FIELD_BUFFER_LEN );
    rAssert( fieldFound );

    fieldKey = ::radMakeCaseInsensitiveKey32( buffer );
    for( i = 0; i < characterTableLength; i++ )
    {
        if( fieldKey == characterTable[i].characterKey )
        {
            m_characterIndex = i;
            break;
        }
    }

    rAssert( ( i < characterTableLength )
             || ( filename[0] != 'C' ) );
}

//=============================================================================
// DialogLine::matchLevelField
//=============================================================================
// Description: Parse the field at the given position and store the level
//              and mission numbers that match it
//
// Parameters:  field - number of field to parse
//
// Return:      void 
//
//=============================================================================
void DialogLine::matchLevelField( const char* filename, int field )
{
    char buffer[FIELD_BUFFER_LEN];
    bool fieldFound;

    //
    // Level/Mission.
    //
    fieldFound = getNameField( filename, field, buffer, FIELD_BUFFER_LEN );

    if( fieldFound )
    {
        if( ( buffer[0] == 'L' ) 
            && ( buffer[1] >= '0' )
            && ( buffer[1] <= '9' ) )
        {
            //
            // Level number, start to string is "Lx"
            //
            m_levelNum = buffer[1] - '0';
            rAssert( m_levelNum > 0 );
            rAssert( m_levelNum < GameplayManager::MAX_LEVELS );

            if( strlen( buffer ) >= 4 )
            {
                //
                // Level and mission, expected string is "LxMy"
                //
                rAssert( ( buffer[2] == 'M' ) || ( buffer[2] == 'B' ) || ( buffer[2] == 'R' ) || ( buffer[2] == 'T' ) );
                m_missionNum = buffer[3] - '0';
                rAssert( m_missionNum >= 1 );
                rAssert( m_missionNum <= 7 );

                //
                // Bonus == 8
                // Races == 9-11
                // Tutorial == 12
                //
                if( buffer[2] == 'B' )
                {
                    rAssert( m_missionNum == 1 );
                    m_missionNum = BONUS_MISSION_NUMBER;
                }
                else if( buffer[2] == 'R' )
                {
                    rAssert( m_missionNum >= 1 );
                    rAssert( m_missionNum <= 3 );
                    m_missionNum += FIRST_RACE_MISSION_NUMBER - 1;
                }
                else if( buffer[2] == 'T' )
                {
                    rAssert( m_missionNum == 1 );
                    m_missionNum = TUTORIAL_MISSION_NUMBER;
                }
            }
            else
            {
                m_missionNum = NO_MISSION;
            }
        }
    }
}

//=============================================================================
// DialogLine::getNameField
//=============================================================================
// Description: Parse the filename for a particular field, and return it in
//              the given buffer
//
// Parameters:  filename -- name of file to parse
//              field -- number of field to find (counting from 0)
//              buffer -- buffer to copy field into
//              bufferLen -- length of buffer
//
// Return:      true if requested field exists, false otherwise 
//
//=============================================================================
bool DialogLine::getNameField( const char* filename, int field, char* buffer, int bufferLen )
{
    const char* currentChar;
    int i;
    int fieldCount = field;
    bool fieldFound = false;

    currentChar = filename;
    while( ( fieldCount > 0 ) && ( *currentChar != '\0' ) && ( *currentChar != '.' ) )
    {
        if( *currentChar == '_' )
        {
            --fieldCount;
        }

        ++currentChar;
    }

    if( ( *currentChar != '\0' ) && ( *currentChar != '.' ) )
    {
        //
        // Field found, copy to buffer.  Since GC has no string functions (argh),
        // use hand-rolled strncpy
        //
        for( i = 0; 
             ( (i < bufferLen-1) && (currentChar[i] != '\0') && (currentChar[i] != '_') && (currentChar[i] != '.') ); 
             i++ )
        {
            buffer[i] = currentChar[i];
        }
        buffer[i] = '\0';

        fieldFound = true;
    }

    return( fieldFound );
}

//=============================================================================
// DialogLine::initializeTables
//=============================================================================
// Description: Fill out the radKey32 entries in the event table.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogLine::initializeTables()
{
    unsigned int i;

    for( i = 0; i < eventTableLength; i++ )
    {
        eventTable[i].eventKey = ::radMakeCaseInsensitiveKey32( eventTable[i].eventString );
    }

    for( i = 0; i < characterTableLength; i++ )
    {
        characterTable[i].characterKey = ::radMakeCaseInsensitiveKey32( characterTable[i].characterString );
        characterTable[i].realCharacterUID = tEntity::MakeUID( characterTable[i].realCharacterName );
    }
}

tUID DialogLine::GetCharacterUID( void )
{
    if ( -1 == m_characterIndex )
    {
        return 0;
    }

    tUID uid = characterTable[ m_characterIndex ].realCharacterUID;
    return uid;
}

tUID DialogLine::GetDialogLineCharacterUID( unsigned int lineNum )
{
    return GetCharacterUID( );
}
        