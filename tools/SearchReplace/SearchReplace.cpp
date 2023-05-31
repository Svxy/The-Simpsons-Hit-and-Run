// SearchReplace.cpp : Defines the entry point for the console application.
//

#include <assert.h>
#include <vector>

class ReplacementTableEntry;

std::vector<ReplacementTableEntry*> gReplacementTable;

bool BuildReplacementTable( FILE* rulesFile );
void RemoveNewLine( char* string );
bool IsWhiteSpace( char* string );
void ProcessSourceFile( FILE* sourceFile, FILE* targetFile );



class ReplacementTableEntry
{
    public:

        ReplacementTableEntry( const char* findString, const char* replaceString );
        ~ReplacementTableEntry();
    
        char* mFindString;
        char* mReplaceString;
};


ReplacementTableEntry::ReplacementTableEntry
( 
    const char* findString, 
    const char* replaceString
)
:
mFindString( 0 ),
mReplaceString( 0 )
{
    assert( findString );
    assert( replaceString );

    mFindString = new char[strlen(findString)+1];
    strcpy( mFindString, findString );

    mReplaceString = new char[strlen(replaceString)+1];
    strcpy( mReplaceString, replaceString );
}

ReplacementTableEntry::~ReplacementTableEntry()
{
    delete [] mFindString;
    delete [] mReplaceString;
}



//==============================================================================
// int main
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
int main(int argc, char* argv[])
{
	// Validate command line arguments
    assert( argc == 4 );

    const char* rulesName = argv[1];
    const char* sourceName = argv[2];
    const char* targetName = argv[3];

    // Open rules file
    FILE* rulesFile = fopen( rulesName, "rt" );

    if( rulesFile == NULL )
    {
        printf( "Failed to open rules file: %s\n", rulesName );
        return( 0 );
    }

    // Build search and replace table
    bool result = BuildReplacementTable( rulesFile );

    // Close rules file
    fclose( rulesFile );


    // Open source file
    FILE* sourceFile = fopen( sourceName, "rt" );

    if( sourceFile == NULL )
    {
        fclose( rulesFile );
        printf( "Failed to open source file: %s\n", sourceName );
        return( 0 );
    }

    // Open target file
    FILE* targetFile = fopen( targetName, "wt" );

    if( targetFile == NULL )
    {
        fclose( rulesFile );
        fclose( sourceFile );
        printf( "Failed to open target file: %s\n", targetName );
        return( 0 );
    }
    
    // Process source file

    ProcessSourceFile( sourceFile, targetFile );

    fclose( sourceFile );
    fclose( targetFile );
      
  
    // Clean up
    std::vector<ReplacementTableEntry*>::const_iterator iter = gReplacementTable.begin();

    for( ; iter != gReplacementTable.end(); ++iter )
    {
        ReplacementTableEntry* entry = (*iter);
        
        delete entry;
    }

    gReplacementTable.clear();
  
	return 0;
}


//==============================================================================
// bool BuildReplacementTable
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
bool BuildReplacementTable( FILE* rulesFile )
{
    // Get the next line from the file

    int lineCount = 0;

    while( 1 )
    {
        const int BUFFER_SIZE = 512;
        char* fgetsResult;
        char lineBuffer[BUFFER_SIZE];
        
        do
        {   if( feof( rulesFile ) )
            {
                return( true );
            }
            
            fgetsResult = fgets( lineBuffer, BUFFER_SIZE, rulesFile );
            ++lineCount;
        }
        while( IsWhiteSpace( lineBuffer ) == true );

        char findString[BUFFER_SIZE];
        strcpy( findString, lineBuffer );
        RemoveNewLine( findString );
        

        do
        {
            if( feof( rulesFile ) )
            {
                printf( "Error in rules file, missing replace string (line: %d)\n", lineCount );
                return( false );
            }

            fgetsResult = fgets( lineBuffer, BUFFER_SIZE, rulesFile );
            ++lineCount;
        }
        while( IsWhiteSpace( lineBuffer ) == true );

        char replaceString[BUFFER_SIZE];
        strcpy( replaceString, lineBuffer );
        RemoveNewLine( replaceString );
        
        // Create Entry
        //
        ReplacementTableEntry* entry = new ReplacementTableEntry( findString, replaceString );
        gReplacementTable.push_back( entry );
    }
}


//==============================================================================
// void RemoveNewLine
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void RemoveNewLine( char* string )
{
    assert( string );
    
    int length = strlen( string );
    
    int i;
    for( i = 0; i < length; ++i )
    {
        if( '\n' == string[i] )
        {
            string[i] = '\0';
        }
    }
}


//==============================================================================
// bool IsWhiteSpace
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
bool IsWhiteSpace( char* string )
{
    assert( string );
    
    int length = strlen( string );
    
    int i;
    for( i = 0; i < length; ++i )
    {
        if( (string[i] != '\n') && (string[i] != ' ') )
        {
            return( false );
        }
    }

    return( true );
}


//==============================================================================
// void ProcessSourceFile
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void ProcessSourceFile( FILE* sourceFile, FILE* targetFile )
{
    int lineNumber = 0;
    while( !feof( sourceFile ) )
    {
        // Read in a line from the source
        const int BUFFER_SIZE = 1024;
        char* fgetsResult;
        char sourceBuffer[BUFFER_SIZE];
        char targetBuffer[BUFFER_SIZE];
        
        fgetsResult = fgets( sourceBuffer, BUFFER_SIZE, sourceFile );
        strcpy( targetBuffer, sourceBuffer );
        ++lineNumber;

        // Iterate through the find strings for a match
        std::vector<ReplacementTableEntry*>::const_iterator iter = gReplacementTable.begin();

        for( ; iter != gReplacementTable.end(); ++iter )
        {
            ReplacementTableEntry* entry = (*iter);
        
            char* start = sourceBuffer;
            char* targetPos = targetBuffer;
            char* found = NULL;
            
            do
            {
                found = strstr( start, entry->mFindString );
        
                // Replace if found
                if( found != NULL )
                {
                    // Copy over the prefix
                    memcpy(targetPos, start, found - start );
                    targetPos += found - start;
                    
                    // Copy over the replacement string
                    memcpy(targetPos, entry->mReplaceString, strlen( entry->mReplaceString ) );
                    targetPos += strlen( entry->mReplaceString );
                    start = found + strlen( entry->mFindString );

                    // Copy over the postfix
                    memcpy( targetPos, start, strlen(start) );

                    // NULL terminate
                    char* term = targetPos + strlen(start);
                    *term = '\0';
                }
            }
            while( found != NULL );

            // The target now becomes the source to preserve any replacements.
            strcpy( sourceBuffer, targetBuffer );
        }

        // Write out line to target
        fputs( targetBuffer, targetFile );
    }
    

    
    

    
}