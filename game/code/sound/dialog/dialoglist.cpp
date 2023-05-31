//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialoglist.cpp
//
// Description: Loads and maintains the list of dialog lines and conversations
//              (which group multiple dialog lines, and potentially link 
//              conversations to other conversations that occur later).
//
// History:     01/09/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radnamespace.hpp>
#include <raddebugwatch.hpp>

#include <p3d/anim/skeleton.hpp>

//========================================
// Project Includes
//========================================
#include <sound/dialog/dialoglist.h>

#include <sound/dialog/dialogline.h>
#include <sound/dialog/conversationmatcher.h>
#include <sound/soundrenderer/idasoundresource.h>

#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>
#include <render/Enums/RenderEnums.h>
#include <gameflow/gameflow.h>
#include <worldsim/character/charactermanager.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// Arbitrary buffer length for filenames
//
static const int FILENAME_BUFFER_LEN = 100;

//
// Stinky hack support
//
radKey32 DialogList::s_introKey = 0;
radKey32 DialogList::s_aztecKey = 0;
tUID DialogList::s_milhouseKey = 0;
tUID DialogList::s_nelsonKey = 0;
tUID DialogList::s_raceZombie1 = 0;
tUID DialogList::s_raceZombie2 = 0;

enum pedDialogType
{
    PED_MALE1,
    PED_MALE2,
    PED_FEMALE1,
    PED_FEMALE2,
    PED_BOY1,
    PED_BOY2,
    PED_GIRL1,
    PED_GIRL2,
    PED_ZOMBIE1,
    PED_ZOMBIE2,
    PED_ZOMBIE3,
    PED_ZOMBIE4,

    PED_NUM_TYPES
};

enum skinDialogType
{
    SKIN_APU,
    SKIN_BART,
    SKIN_HOMER,
    SKIN_LISA,
    SKIN_MARGE,
    SKIN_BARNEY,
    SKIN_MILHOUSE,
    SKIN_NELSON,
    SKIN_RALPH,
    SKIN_CLETUS,
    SKIN_ZOMBIE1,
    SKIN_ZOMBIE2,
    SKIN_ZOMBIE3,
    SKIN_ZOMBIE4,
    SKIN_OTTO,
    SKIN_WILLIE,
    SKIN_KEARNEY,
    SKIN_SKINNER,
    SKIN_GRANDPA,
    SKIN_CBG,
    SKIN_FRINK,
    SKIN_SNAKE,
    SKIN_SMITHERS,

    SKIN_NUM_TYPES
};

struct pedTypeInfo
{
    radInt64      pedUID;
    const char*   pedName;
    pedDialogType dialogGroup;
};

static pedTypeInfo pedestrianNameTable[] =
{
    { 0, "boy",          PED_BOY1    },
    { 0, "boy2",         PED_BOY2    },
    { 0, "boy3",         PED_BOY1    },
    { 0, "bum",          PED_MALE1   },
    { 0, "busm1",        PED_MALE2   },
    { 0, "busm2",        PED_MALE1   },
    { 0, "busw1",        PED_FEMALE1 },
    { 0, "const1",       PED_MALE2   },
    { 0, "const2",       PED_MALE1   },
    { 0, "farmr1",       PED_MALE2   },
    { 0, "fem1",         PED_FEMALE2 },
    { 0, "fem2",         PED_FEMALE1 },
    { 0, "fem3",         PED_FEMALE2 },
    { 0, "fem4",         PED_FEMALE1 },
    { 0, "girl1",        PED_GIRL1   },
    { 0, "girl2",        PED_GIRL2   },
    { 0, "hooker",       PED_FEMALE2 },
    { 0, "joger1",       PED_FEMALE1 },
    { 0, "joger2",       PED_MALE1   },
    { 0, "male1",        PED_MALE2   },
    { 0, "male2",        PED_MALE1   },
    { 0, "male3",        PED_MALE2   },
    { 0, "male4",        PED_MALE1   },
    { 0, "male5",        PED_MALE2   },
    { 0, "male6",        PED_MALE1   },
    { 0, "mobstr",       PED_MALE2   },
    { 0, "nuclear",      PED_MALE1   },
    { 0, "olady1",       PED_FEMALE2 },
    { 0, "olady2",       PED_FEMALE1 },
    { 0, "olady3",       PED_FEMALE2 },
    { 0, "rednk1",       PED_BOY2    },
    { 0, "rednk2",       PED_BOY1    },
    { 0, "sail1",        PED_MALE2   },
    { 0, "sail2",        PED_MALE1   },
    { 0, "sail3",        PED_MALE2   },
    { 0, "sail4",        PED_MALE1   },
    { 0, "witch",        PED_GIRL1   },
    { 0, "frankenstein", PED_BOY2    },
    { 0, "zfem1",        PED_ZOMBIE3 },
    { 0, "zfem5",        PED_ZOMBIE3 },
    { 0, "zmale1",       PED_ZOMBIE1 },
    { 0, "zmale3",       PED_ZOMBIE2 },
    { 0, "zmale4",       PED_ZOMBIE4 }
};

static unsigned int pedestrianTableLength = sizeof( pedestrianNameTable ) / sizeof( pedTypeInfo );

struct skinTypeInfo
{
    radInt64       skinUID;
    const char*    skinName;
    skinDialogType dialogGroup;
};

static skinTypeInfo skinNameTable[] =
{
    { 0, "a_american", SKIN_APU    },
    { 0, "a_army",     SKIN_APU    },
    { 0, "a_besharp",  SKIN_APU    },
    { 0, "b_football", SKIN_BART   },
    { 0, "b_hugo",     SKIN_BART   },
    { 0, "b_man",      SKIN_BART   },
    { 0, "b_military", SKIN_BART   },
    { 0, "b_ninja",    SKIN_BART   },
    { 0, "b_tall",     SKIN_BART   },
    { 0, "h_donut",    SKIN_HOMER  },
    { 0, "h_evil",     SKIN_HOMER  },
    { 0, "h_fat",      SKIN_HOMER  },
    { 0, "h_scuzzy",   SKIN_HOMER  },
    { 0, "h_stcrobe",  SKIN_HOMER  },
    { 0, "h_undrwr",   SKIN_HOMER  },
    { 0, "reward_homer", SKIN_HOMER },
    { 0, "l_cool",     SKIN_LISA   },
    { 0, "l_florida",  SKIN_LISA   },
    { 0, "l_jersey",   SKIN_LISA   },
    { 0, "m_pink",     SKIN_MARGE  },
    { 0, "m_police",   SKIN_MARGE  },
    { 0, "m_prison",   SKIN_MARGE  },
    { 0, "brn_unf",    SKIN_BARNEY },
    { 0, "reward_barney", SKIN_BARNEY },
    { 0, "b_milhouse", SKIN_MILHOUSE },
    { 0, "b_nelson",   SKIN_NELSON },
    { 0, "b_ralph",    SKIN_RALPH },
    { 0, "b_cletus",   SKIN_CLETUS },
    { 0, "b_zmale1",   SKIN_ZOMBIE1 },
    { 0, "b_zmale3",   SKIN_ZOMBIE2 },
    { 0, "b_zfem5",    SKIN_ZOMBIE3 },
    { 0, "b_zmale4",   SKIN_ZOMBIE4 },
    { 0, "b_zfem1",    SKIN_ZOMBIE3 },
    { 0, "b_skinner", SKIN_SKINNER },
    { 0, "b_grandpa", SKIN_GRANDPA },
    { 0, "b_cbg", SKIN_CBG },
    { 0, "b_barney", SKIN_BARNEY },
    { 0, "b_frink", SKIN_FRINK },
    { 0, "b_snake", SKIN_SNAKE },
    { 0, "b_smithers", SKIN_SMITHERS },
    { 0, "reward_otto", SKIN_OTTO },
    { 0, "reward_willie", SKIN_WILLIE },
    { 0, "reward_kearney", SKIN_KEARNEY }
};

static unsigned int skinTableLength = sizeof( skinNameTable ) / sizeof( skinTypeInfo );

struct pedDialogGroupInfo
{
    radInt64 pedUID;
    const char* pedName;
};

//
// Size must be PED_NUM_TYPES
//
static pedDialogGroupInfo dialogGroupTable[] =
{
    { 0, "male1"   },
    { 0, "male2"   },
    { 0, "fem1"    },
    { 0, "fem2"    },
    { 0, "boy1"    },
    { 0, "boy2"    },
    { 0, "girl1"   },
    { 0, "girl2"   },
    { 0, "zombie1" },
    { 0, "zombie2" },
    { 0, "zombie3" },
    { 0, "zombie4" }
};

struct skinDialogGroupInfo
{
    radInt64    charUID;
    const char* charName;
};

//
// Size must be SKIN_NUM_TYPES
//
static skinDialogGroupInfo skinDialogGroupTable[] =
{
    { 0, "apu"    },
    { 0, "bart"   },
    { 0, "homer"  },
    { 0, "lisa"   },
    { 0, "marge"  },
    { 0, "barney" },
    { 0, "milhouse" },
    { 0, "nelson" },
    { 0, "ralph" },
    { 0, "cletus" },
    { 0, "zombie1" },
    { 0, "zombie2" },
    { 0, "zombie3" },
    { 0, "zombie4" },
    { 0, "otto" },
    { 0, "willie" },
    { 0, "kearney" },
    { 0, "skinner" },
    { 0, "grandpa" },
    { 0, "cbg" },
    { 0, "frink" },
    { 0, "snake" },
    { 0, "smithers" }
};

//
// Debug flag
//
bool DialogList::s_showDialogSpew = false;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// DialogList::DialogList
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
DialogList::DialogList()
{
    unsigned int i;

    //
    // Lazy initialization
    //
    if( s_introKey == 0 )
    {
        s_introKey = ::radMakeKey32( "intro" );
        s_aztecKey = ::radMakeKey32( "aztec" );
        s_milhouseKey = tEntity::MakeUID( "milhouse" );
        s_nelsonKey = tEntity::MakeUID( "nelson" );
        s_raceZombie1 = tEntity::MakeUID( "zmale3" );
        s_raceZombie2 = tEntity::MakeUID( "zfem1" );

        //
        // Also do the tables of UIDs we use to identify peds and skins
        //
        for( i = 0; i < pedestrianTableLength; i++ )
        {
            pedestrianNameTable[i].pedUID = tEntity::MakeUID( pedestrianNameTable[i].pedName );
        }

        for( i = 0; i < PED_NUM_TYPES; i++ )
        {
            dialogGroupTable[i].pedUID = tEntity::MakeUID( dialogGroupTable[i].pedName );
        }

        for( i = 0; i < skinTableLength; i++ )
        {
            skinNameTable[i].skinUID = tEntity::MakeUID( skinNameTable[i].skinName );
        }

        for( i = 0; i < SKIN_NUM_TYPES; i++ )
        {
            skinDialogGroupTable[i].charUID = tEntity::MakeUID( skinDialogGroupTable[i].charName );
        }

        //
        // Debug spew
        //
        radDbgWatchAddBoolean( &s_showDialogSpew, "Show Dialog Spew", "Sound Info" );
        radDbgWatchAddFunction( "Print Dialog Coverage", &dumpDialogCoverage, this, "Sound Info" );
    }
}

//==============================================================================
// DialogList::~DialogList
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
DialogList::~DialogList()
{
}

//=============================================================================
// DialogList::OrganizeDialog
//=============================================================================
// Description: Go through the list of dialog resources, creating SelectableDialog
//              objects organized as directed by the naming conventions.
//
// Parameters:  namespaceObj -- object containing the list of dialog resources
//                              as read from the script file
//
// Return:      void 
//
//=============================================================================
void DialogList::OrganizeDialog( IRadNameSpace* namespaceObj )
{
    int mission;
    int level;
    IDaSoundResource* resource;
    DialogLine* newLine;
    ConversationMatcher matcher;
    SelectableDialogList* dialogList;
    SelectableDialog* foundDialog;

    //
    // Go through the list of sound resources looking for dialog
    //
    resource = reinterpret_cast< IDaSoundResource* >( namespaceObj->GetFirst( NULL) );
    while( resource != NULL )
    {
        if( isIndividualLine( resource ) )
        {
            //
            // Resource is dialog but not conversation.  Create a DialogLine object
            // to hold the information about it and store it in the appropriate list.
            //
#ifdef RAD_GAMECUBE
            newLine = new( GMA_GC_VMM ) DialogLine( resource );
#else
            newLine = new( GMA_PERSISTENT ) DialogLine( resource );
#endif     
            if( newLine->IsLevelSpecific() )
            {
                dialogList = &(m_missionLists[newLine->GetLevel() - 1][newLine->GetMission()]);
            }
            else
            {
                dialogList = &m_genericDialogList;
            }

            //
            // Search the list.  If we've already got a dialog for the same situation, lump
            // this one in with it, otherwise stash it straight into the list
            //
            foundDialog = searchDialogList( newLine->GetEvent(), newLine->GetCharacterUID(), 0,
                                            *dialogList, 0, newLine->IsVillainLine(), false );
            HeapMgr()->PushHeap( GMA_AUDIO_PERSISTENT );
            if( foundDialog != NULL )
            {
                foundDialog->AddMatchingDialog( *newLine, *dialogList );
            }
            else
            {
                dialogList->push_back( newLine );
            }
            HeapMgr()->PopHeap( GMA_AUDIO_PERSISTENT );
        }
        else if( isConversationLine( resource ) )
        {
            //
            // Resource is part of a conversation.  Give it to the object
            // responsible for matching the pieces together
            //
            matcher.AddNewLine( resource );
        }
        //
        // Otherwise, this isn't dialog, so we don't have to do anything with it
        //

        //
        // Next resource in the list
        //
        resource = reinterpret_cast< IDaSoundResource* >( namespaceObj->GetNext( NULL) );
    }

    //
    // Do a sanity check on the conversations
    //
    rAssert( matcher.AreAllConversationsComplete() );

    //
    // Add the completed conversations to the appropriate lists
    //
    for( level = 0; level < GameplayManager::MAX_LEVELS; level++ )
    {
        for( mission = 0; mission < GameplayManager::MAX_MISSIONS; mission++ )
        {
            matcher.AddConversationsToList( level + 1, mission, m_missionLists[level][mission] );
        }
    }

    matcher.AddConversationsToList( SelectableDialog::NO_LEVEL, SelectableDialog::NO_MISSION, m_genericDialogList );
}

//=============================================================================
// DialogList::FindDialogForEvent
//=============================================================================
// Description: Search through the dialog lists to find something appropriate
//              for the given dialog event
//
// Parameters:  id - Event ID that we need to find dialog for
//              character1 - character who will say the dialog
//              character2 - if conversation, second character in conversation.
//                           NULL otherwise.
//              charUID1 - if we're searching by UID, this is non-zero and
//                         character1 and character2 are NULL.
//              charUID2 - second character, used for searching by UID
//              convKey - name of conversation, 0 if not applicable
//
// Return:      Const pointer to SelectableDialog object best matching the
//              event, or NULL if nothing found
//
//=============================================================================
SelectableDialog* DialogList::FindDialogForEvent( EventEnum id,
                                                  Character* character1, 
                                                  Character* character2,
                                                  tUID charUID1,
                                                  tUID charUID2,
                                                  radKey32 convKey,
                                                  bool isVillain )
{
    int mission;
    tUID char1UID;
    tUID char2UID;
    char nameBuffer[20];
    unsigned int aztecNumber;
    GameplayManager* gameplayMgr = NULL;
    // For indexing purposes, levels count from zero.
    int level;
    SelectableDialog* dialogMatch = NULL;
    Mission* missionObj;

    if( GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND )
    {
        //
        // Playing dialog in front end, so any level will do
        //
        level = 0;
        missionObj = NULL;
    }
    else
    {
        gameplayMgr = GetGameplayManager();
        rAssert( gameplayMgr != NULL );

        level = gameplayMgr->GetCurrentLevelIndex() - RenderEnums::L1;
        missionObj = gameplayMgr->GetCurrentMission();
    }

    //
    // First, search the list for the current mission.  If it's a conversation
    // event, those only happen during missions even if we're in Sunday Drive
    // (they're the conversations that start the missions) so just start looking there.
    //
    if( missionObj != NULL )
    {
        if( ( !(gameplayMgr->IsSuperSprint()) )
            && ( (!(missionObj->IsSundayDrive()) )
            || ( id == EVENT_CONVERSATION_INIT_DIALOG )
            || ( id == EVENT_TUTORIAL_DIALOG_PLAY ) ) )
        {
            if( id == EVENT_TUTORIAL_DIALOG_PLAY )
            {
                mission = DialogLine::TUTORIAL_MISSION_NUMBER;
            }
            else if( missionObj->IsBonusMission() )
            {
                mission = DialogLine::BONUS_MISSION_NUMBER;
            }
            else if( missionObj->IsRaceMission() )
            {
                mission = DialogLine::FIRST_RACE_MISSION_NUMBER + 
                    ( gameplayMgr->GetCurrentMissionNum() - GameplayManager::MAX_MISSIONS );
            }
            else
            {
                if( convKey == s_introKey )
                {
                    //
                    // Stinky race missions.  The "intro" conversation happens before we've
                    // started the race.  I can't rename them to simple L1, since characters like Homer
                    // have multiple C_intro_*_L1.rsd lines, so there's a naming clash.  Ugh.
                    // To make matters worse, since some of these conversations involve only Homer (or
                    // whoever the driver is), we need to check if either character is Milhouse, Nelson,
                    // Ralph, or their zombie counterparts
                    //
                    rAssert( character1 != NULL );
                    rAssert( character2 != NULL );

                    char1UID = getPuppetUID( character1 );
                    char2UID = getPuppetUID( character2 );
                    if( ( char1UID == s_milhouseKey )
                        || ( char2UID == s_milhouseKey ) 
                        || ( char1UID == s_raceZombie1 )
                        || ( char2UID == s_raceZombie1 ) )
                    {
                        mission = DialogLine::FIRST_RACE_MISSION_NUMBER;
                    }
                    else if( ( char1UID == s_nelsonKey )
                        || ( char2UID == s_nelsonKey ) 
                        || ( char1UID == s_raceZombie2 )
                        || ( char2UID == s_raceZombie2 ) )
                    {
                        mission = DialogLine::FIRST_RACE_MISSION_NUMBER + 1;
                    }
                    else
                    {
                        //
                        // This had better be Ralph or zombie Ralph
                        //
                        mission = DialogLine::FIRST_RACE_MISSION_NUMBER + 2;
                    }
                }
                else if( level == 0 )
                {
                    //
                    // Stinky level 1, tutorial mission screws everything up
                    //
                    mission = gameplayMgr->GetCurrentMissionIndex();
                }
                else
                {
                    mission = gameplayMgr->GetCurrentMissionIndex() + 1;

#ifdef RAD_E3
                    //
                    // E3 hack.  L2M5 is our only mission, and it's going to
                    // come back as mission 1.  Hack it to 5.
                    //
                    mission = 5;
#endif
                }

                if ( convKey == s_aztecKey )
                {
                    //
                    // Another stinky hack.  The teen at the Aztec needs randomized conversations.
                    // Conversations don't really randomize because the conversation builder assumes
                    // that identically-named conversations result from misnamed files.  And the
                    // key isn't a straightforward randomization on the caller's end, since it's
                    // a scripted value.  I'll handle it here.
                    //
                    aztecNumber = ( rand() % 4 ) + 1;
                    sprintf( nameBuffer, "aztec%d", aztecNumber );
                    convKey = ::radMakeKey32( nameBuffer );
                }
            }

            if( s_showDialogSpew )
            {
                rTuneString( "Searching mission-specific dialog\n" );
            }

            if( character1 == NULL )
            {
                // Already have UIDs
                dialogMatch = searchDialogList( id, charUID1, charUID2, m_missionLists[level][mission], convKey, isVillain );
            }
            else
            {
                // Take UID from character objects
                dialogMatch = searchDialogList( id, character1, character2, m_missionLists[level][mission], convKey, isVillain );
            }
        }
    }

    if( dialogMatch == NULL )
    {
        //
        // No mission-specific dialog, search the level-specific stuff
        //
        if( s_showDialogSpew )
        {
            rTuneString( "Searching level-specific dialog\n" );
        }

        if( character1 == NULL )
        {
            dialogMatch = searchDialogList( id, charUID1, charUID2, m_missionLists[level][0], convKey, isVillain );
        }
        else
        {
            dialogMatch = searchDialogList( id, character1, character2, m_missionLists[level][0], convKey, isVillain );
        }

        if( dialogMatch == NULL )
        {
            //
            // No mission- or level-specific dialog, search the generic list
            //
            if( s_showDialogSpew )
            {
                rTuneString( "Searching generic dialog\n" );
            }

            if( character1 == NULL )
            {
                dialogMatch = searchDialogList( id, charUID1, charUID2, m_genericDialogList, convKey, isVillain );
            }
            else
            {
                dialogMatch = searchDialogList( id, character1, character2, m_genericDialogList, convKey, isVillain );
            }
        }
    }

    return( dialogMatch );
}

//=============================================================================
// DialogList::GetStinkySkinPointer
//=============================================================================
// Description: Given a UID, see if we can dig up a character for it looking
//              through all the possible skins.
//
// Parameters:  charUID - tUID of character
//
// Return:      Character* if match found, NULL otherwise
//
//=============================================================================
Character* DialogList::GetStinkySkinPointer( tUID charUID )
{
    int skinType;
    unsigned int i;
    Character* charPtr;

    for( skinType = 0; skinType < SKIN_NUM_TYPES; skinType++ )
    {
        if( skinDialogGroupTable[skinType].charUID == charUID )
        {
            break;
        }
    }

    if( skinType == SKIN_NUM_TYPES )
    {
        //
        // No skin exists for given character
        //
        return( NULL );
    }

    //
    // At this point, the character has skins.  Look for a match.
    //
    for( i = 0; i < skinTableLength; i++ )
    {
        if( skinNameTable[i].dialogGroup == skinType )
        {
            charPtr = GetCharacterManager()->GetCharacterByName( skinNameTable[i].skinUID );
            if( charPtr != NULL )
            {
                return( charPtr );
            }
        }
    }

    //
    // No skins found
    //
    return( NULL );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// DialogList::hasOneLinerPrefix
//=============================================================================
// Description: Determines if resource name belongs to one-liner dialog.  This
//              is deemed to be true if it starts with a valid role abbreviation.
//
// Parameters:  name - name of resource
//
// Return:      true if role field found, false otherwise
//
//=============================================================================
bool DialogList::hasOneLinerPrefix( const char* name )
{
    return( ( name[1] == '_' )
            && ( ( name[0] == 'W' )
                 || ( name[0] == 'D' )
                 || ( name[0] == 'P' )
                 || ( name[0] == 'V' ) ) );
}

//=============================================================================
// DialogList::isIndividualLine
//=============================================================================
// Description: Test for whether given resource is a dialog one-liner
//
// Parameters:  resource - sound resource to test
//
// Return:      true if one-liner, false otherwise
//
//=============================================================================
bool DialogList::isIndividualLine( IDaSoundResource* resource )
{
    char tempBuffer[FILENAME_BUFFER_LEN];
    char buffer[FILENAME_BUFFER_LEN];

    //
    // Get the first filename belonging to the resource.  Don't bother checking
    // for >1 file---if they exist, then the names had better be interchangable.
    //
    tempBuffer[0] = '\0';
    resource->GetFileNameAt( 0, tempBuffer, FILENAME_BUFFER_LEN );
    rAssert( strlen( tempBuffer ) > 0 );

    DialogLine::StripDirectoryCrud( tempBuffer, buffer, FILENAME_BUFFER_LEN );

    //
    // Simple test: we'll call it a line if it has at least two underscores
    // and no "C_" prefix
    //
    return( ( !hasConversationPrefix( buffer ) )
            && ( hasOneLinerPrefix( buffer ) )
            && ( underscoreCount( buffer ) > 1 ) );
}

//=============================================================================
// DialogList::isConversationLine
//=============================================================================
// Description: Test for whether given resource is part of a dialog
//              conversation
//
// Parameters:  resource - sound resource to test
//
// Return:      true if conversation line, false otherwise
//
//=============================================================================
bool DialogList::isConversationLine( IDaSoundResource* resource )
{
    char tempBuffer[FILENAME_BUFFER_LEN];
    char buffer[FILENAME_BUFFER_LEN];

    //
    // Get the first filename belonging to the resource.  Don't bother checking
    // for >1 file---if they exist, then the names had better be interchangable.
    //
    tempBuffer[0] = '\0';
    resource->GetFileNameAt( 0, tempBuffer, FILENAME_BUFFER_LEN );
    rAssert( strlen( tempBuffer ) > 0 );

    DialogLine::StripDirectoryCrud( tempBuffer, buffer, FILENAME_BUFFER_LEN );

    //
    // Test: line belongs to conversation if it has at least three underscores
    // and a "C_" prefix
    //
    return( hasConversationPrefix( buffer ) &&
            ( underscoreCount( buffer ) > 3 ) );
}

//=============================================================================
// DialogList::underscoreCount
//=============================================================================
// Description: Return number of underscores in the given string
//
// Parameters:  name - string to count in
//
// Return:      number of underscores found
//
//=============================================================================
unsigned int DialogList::underscoreCount( const char* name )
{
    unsigned int i = 0;
    unsigned int count = 0;


    while( name[i] != '\0' )
    {
        if( name[i] == '_' )
        {
            ++count;
        }

        i++;
    }

    return( count );
}

//=============================================================================
// DialogList::searchDialogList
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, tUID characterUID1, tUID characterUID2, SelectableDialog* list )
//
// Return:      SelectableDialog
//
//=============================================================================
SelectableDialog* DialogList::searchDialogList( EventEnum id, Character* character1, 
                                                Character* character2, SelectableDialogList& list,
                                                radKey32 convName, bool isVillain )
{
    tUID UID1 = 0;
    tUID UID2 = 0;

    if( character1 == NULL )
    {
        UID1 = 0;
    }
    else
    {
        //
        // Can't just get the character UID, since they're not guaranteed to be consistent
        // with the model you see on the screen.  Need the skeleton UID, it appears
        //
        UID1 = getPuppetUID( character1 );
    }

    if( character2 == NULL )
    {
        UID2 = 0;
    }
    else
    {
        UID2 = getPuppetUID( character2 );
    }

    return( searchDialogList( id, UID1, UID2, list, convName, isVillain, true ) );
}

//=============================================================================
// DialogList::searchDialogList
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, tUID driverUID, SelectableDialogList& list )
//
// Return:      SelectableDialog
//
//=============================================================================
SelectableDialog* DialogList::searchDialogList( EventEnum id, tUID characterUID1, 
                                                tUID characterUID2,
                                                SelectableDialogList& list,
                                                radKey32 convName,
                                                bool isVillain )
{
    return( searchDialogList( id, characterUID1, characterUID2, list, convName, isVillain, true ) );
}

//=============================================================================
// DialogList::searchDialogList
//=============================================================================
// Description: Search the given list for a SelectableDialog object with the
//              given event ID.
//
// Parameters:  id - event ID to find a match for
//              list - list to search
//
// Return:      pointer to SelectableDialog object matching the event ID if
//              it exists, NULL otherwise
//
//=============================================================================
SelectableDialog* DialogList::searchDialogList( EventEnum id, tUID characterUID1,
                                                tUID characterUID2, SelectableDialogList& list,
                                                radKey32 convName, bool isVillain, bool fuzzyPedMatch )
{
    char eventName[30];
    char char1Name[30];
    char char2Name[30];
    char convBuffer[30];
    char villain[3];
    SelectableDialog* currentDialog;
    SelectableDialogList::const_iterator iter = list.begin();
    SelectableDialog* returnValue = NULL;

    if( s_showDialogSpew )
    {
        //
        // Print a message for the stuff we're trying to match with
        //
        DialogLine::FillEventName( eventName, 30, id );
        DialogLine::FillCharacterName( char1Name, 30, characterUID1 );
        DialogLine::FillCharacterName( char2Name, 30, characterUID2 );
        if( convName != 0 )
        {
            sprintf( convBuffer, ", conv %d", convName );
        }
        else
        {
            convBuffer[0] = '\0';
        }
        if( isVillain )
        {
            villain[0] = 'V';
        }
        else
        {
            villain[0] = 'W';
        }
        villain[1] = '\0';
        rTunePrintf( "Dialog: Looking for event %s, char1 %s, char2 %s %s %s\n",
                     eventName, char1Name, char2Name, villain, convBuffer );
    }

    for( ; iter != list.end(); ++iter )
    {
        currentDialog = *iter;

        if( s_showDialogSpew )
        {
            //
            // Print a message for the stuff we're currently looking at
            //
            DialogLine::FillEventName( eventName, 30, currentDialog->GetEvent() );
            DialogLine::FillCharacterName( char1Name, 30, currentDialog->GetDialogLineCharacterUID( 1 ) );

            if( currentDialog->GetNumDialogLines() > 1 )
            {
                DialogLine::FillCharacterName( char2Name, 30, currentDialog->GetDialogLineCharacterUID( 2 ) );
            }
            else
            {
                char2Name[0] = '-';
                char2Name[1] = '\0';
            }

            if( convName != 0 )
            {
                sprintf( convBuffer, ", conv %d", currentDialog->GetConversationName() );
            }
            else
            {
                convBuffer[0] = '\0';
            }

            if( currentDialog->IsVillainLine() )
            {
                villain[0] = 'V';
            }
            else
            {
                villain[0] = 'W';
            }
            villain[1] = '\0';

            rTunePrintf( "Dialog: Matching against event %s, char1 %s, char2 %s %s %s\n",
                eventName, char1Name, char2Name, villain, convBuffer );
        }

        if( ( currentDialog->GetEvent() == id ) 
            && ( currentDialog->IsVillainLine() == isVillain )
            // If a conversation name is supplied, that has to match
            && ( ( convName == 0 )
                 || ( currentDialog->GetConversationName() == convName ) ) )
        {
            if( currentDialog->GetNumDialogLines() == 1 )
            {
                //
                // Match either character
                //
                if( characterMatches( characterUID1, currentDialog, fuzzyPedMatch ) 
                    || characterMatches( characterUID2, currentDialog, fuzzyPedMatch ) )
                {
                    returnValue = currentDialog;
                }
            }
            else
            {
                //
                // Multi-line dialog.  Match both.
                //
                if( characterMatches( characterUID1, currentDialog, fuzzyPedMatch ) 
                    && characterMatches( characterUID2, currentDialog, fuzzyPedMatch ) )
                {
                    returnValue = currentDialog;
                }
            }

            if( returnValue != NULL )
            {
                if( s_showDialogSpew )
                {
                    rTunePrintf( "Dialog: Match found\n" );
                }

                //
                // We're done
                //
                break;
            }
        }
    }

    return( returnValue );
}

//=============================================================================
// DialogList::characterMatches
//=============================================================================
// Description: Determine whether the given dialog matches the character
//              given.  UID of zero always matches.
//
// Parameters:  characterObj - character to match
//              dialog - dialog to match to
//              fuzzyPedMatch - true if we want to fudge UIDs to group
//                              pedestrians, false otherwise
//
// Return:      true if match, false otherwise
//
//=============================================================================
bool DialogList::characterMatches( tUID characterUID, SelectableDialog* dialog,
                                   bool fuzzyPedMatch )
{
    unsigned int i;
    tUID effectiveUID;  // Unix humour.  Nyuck!
    pedDialogType dialogType;
    bool switchMade = false;

    if( characterUID == static_cast< tUID >( 0 ) )
    {
        return( false );
    }

    //
    // Argh!!  We have a whole bunch of pedestrian UIDs which need to be mapped
    // to eight dialog characters.  If this actually shows in a profiler, we'll
    // need to mark the peds in the Character objects when they're spawned somehow
    // to avoid this search
    //

    //
    // Double argh!!  Now we've got a bunch of character skins that are breaking
    // the dialog system.  We have to search for those as well.
    //
    effectiveUID = characterUID;

    if( fuzzyPedMatch )
    {
        for( i = 0; i < pedestrianTableLength; i++ )
        {
            if( effectiveUID == static_cast< tUID >( pedestrianNameTable[i].pedUID ) )
            {
                //
                // Is ped, map new UID
                //

                //
                // Another hack: zombie1/2 and zombie3/4 should be randomly chosen.
                // TODO: leave zombie3 as zombie3 for E3.
                //
                dialogType = pedestrianNameTable[i].dialogGroup;
                if( ( dialogType == PED_ZOMBIE1 )
                    && ( dialog->GetEvent() != EVENT_CONVERSATION_INIT_DIALOG ) 
                    && ( dialog->GetEvent() != EVENT_IN_GAMEPLAY_CONVERSATION ) )
                {
                    if( ( rand() % 2 ) == 0 )
                    {
                        dialogType = PED_ZOMBIE2;
                    }
                }

                effectiveUID = dialogGroupTable[dialogType].pedUID;
                switchMade = true;
                break;
            }
        }

        if( !switchMade )
        {
            //
            // Not a ped, check for skins
            //
            for( i = 0; i < skinTableLength; i++ )
            {
                if( effectiveUID == static_cast< tUID >( skinNameTable[i].skinUID ) )
                {
                    //
                    // Is skin, map new UID
                    //
                    effectiveUID = skinDialogGroupTable[skinNameTable[i].dialogGroup].charUID;
                    break;
                }
            }
        }
    }

    return( dialog->UsesCharacter( effectiveUID ) );
}

//=============================================================================
// DialogList::getPuppetUID
//=============================================================================
// Description: Get UID for Choreo puppet for character
//
// Parameters:  ( Character* characterPtr )
//
// Return:      tUID if skeleton found, 0 otherwise
//
//=============================================================================
tUID DialogList::getPuppetUID( Character* characterPtr )
{
    const char* modelName;
    rAssert( characterPtr != NULL );

    modelName = GetCharacterManager()->GetModelName( characterPtr );
    if( modelName != NULL )
    {
        return( tEntity::MakeUID( modelName ) );
    }
    else
    {
        return( 0 );
    }
}

void DialogList::dumpDialogCoverage( void* userData )
{
#ifndef RAD_RELEASE
    SelectableDialogList::const_iterator iter;
    int i, j;
    SelectableDialog* currentDialog;
    char eventName[30];
    char char1Name[30];
    char char2Name[30];
    char convBuffer[30];
    DialogList* listObj = static_cast<DialogList*>(userData);

    for( i = 0; i < GameplayManager::MAX_LEVELS; i++ )
    {
        for( j = 0; j < GameplayManager::MAX_MISSIONS+1; j++ )
        {
            rTunePrintf( "\nDialogue for level %d mission %d list\n", i, j );

            iter = listObj->m_missionLists[i][j].begin();
            for( ; iter != listObj->m_missionLists[i][j].end(); ++iter )
            {
                currentDialog = *iter;
                if( currentDialog != NULL )
                {
                    DialogLine::FillEventName( eventName, 30, currentDialog->GetEvent() );
                    DialogLine::FillCharacterName( char1Name, 30, currentDialog->GetDialogLineCharacterUID( 1 ) );

                    if( currentDialog->GetNumDialogLines() > 1 )
                    {
                        DialogLine::FillCharacterName( char2Name, 30, currentDialog->GetDialogLineCharacterUID( 2 ) );
                    }
                    else
                    {
                        char2Name[0] = '-';
                        char2Name[1] = '\0';
                    }

                    sprintf( convBuffer, ", conv %d", currentDialog->GetConversationName() );

                    rTunePrintf( "Dialog: Event %s, char1 %s, char2 %s%s : ",
                        eventName, char1Name, char2Name, convBuffer );
                    currentDialog->PrintPlayedStatus();
                }
            }
        }
    }

    rTuneString( "\nGeneric dialogue list:\n" );

    iter = listObj->m_genericDialogList.begin();
    for( ; iter != listObj->m_genericDialogList.end(); ++iter )
    {
        currentDialog = *iter;
        if( currentDialog != NULL )
        {
            DialogLine::FillEventName( eventName, 30, currentDialog->GetEvent() );
            DialogLine::FillCharacterName( char1Name, 30, currentDialog->GetDialogLineCharacterUID( 1 ) );

            if( currentDialog->GetNumDialogLines() > 1 )
            {
                DialogLine::FillCharacterName( char2Name, 30, currentDialog->GetDialogLineCharacterUID( 2 ) );
            }
            else
            {
                char2Name[0] = '-';
                char2Name[1] = '\0';
            }

            sprintf( convBuffer, ", conv %d", currentDialog->GetConversationName() );

            rTunePrintf( "Dialog: Event %s, char1 %s, char2 %s%s : ",
                eventName, char1Name, char2Name, convBuffer );
            currentDialog->PrintPlayedStatus();
        }
    }

#endif
}