/*-----------------------------------------------------------------------------
** Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
**
** winutil.h
**
** Description: A set of utilities which are specific to the windows development
**              environment.
**
** Modification History:
**  + Created Aug 03, 2001 -- bkusy 
**---------------------------------------------------------------------------*/

/*----------------------------------------
** System Includes
**--------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <assert.h>

/*----------------------------------------
** Project Includes
**--------------------------------------*/
#include "util.h"

/*----------------------------------------
** Constants
**--------------------------------------*/
#define BUFFER_SIZE 512

/*----------------------------------------
** Globals
**--------------------------------------*/
char* util_optarg;
int   util_optind;
int   util_opterr = 1;
int   util_optopt;


/*-----------------------------------------------------------------------------
** u t i l _ b a s e n a m e
**
** Synopsis:    Determine the basename in a file path. ( eg/ would return
**              "newfile.c" from the path "/usr/home/newfile.c".
**
** Parameters:  buffer - buffer to receive basename.
**              size   - size of buffer.
**              path   - path to determine basename from.
**
** Returns:     non-zero if successful, 0 if buffer is too small.
**
** Constraints: path must use '/' and not '\' as directory separators.
**
**---------------------------------------------------------------------------*/
int util_basename( char* buffer, int size, const char* path )
{
    char* d;
    char* n;
    char* e;
    int length;
    int result = 0;

    assert( buffer );

    util_decomposeFilePath( path, &d, &n, &e );

    length = strlen( n ) + strlen( e ) + 1;

    if ( length < size )
    {
        strcpy( buffer, n );
        strcat( buffer, "." );
        strcat( buffer, e );

        result = 1;
    }

    return result;
}
/*-----------------------------------------------------------------------------
** u t i l _ c h a n g e F i l e E x t e n s i o n
**
** Synopsis:    Changes the file extension in the given buffer to the given 
**              given extension.
**
** Parameters:  buffer  - the file path to be modified.
**              ext     - the new file extension.
**
** Returns:     NOTHING
**
** Constraints: If the original file path does not have an extension, as 
**              identified by the presence of a "." character, no change will
**              take place.
**              If the new extension is longer than the old extension, no
**              change will take place.
**
**---------------------------------------------------------------------------*/
void util_changeFileExtension( char* buffer, const char* ext )
{
    char* oldExt = 0;

    assert( buffer );
    assert( ext );

    oldExt = (char*)(util_rindex( buffer, '.' ));

    if ( oldExt )
    {
        oldExt++;

        if ( strlen( oldExt ) <= strlen( ext ) )
        {
            strcpy( oldExt, ext );
        }
    }
}

/*-----------------------------------------------------------------------------
** u t i l _ d e c o m p o s e F i l e P a t h 
**
** Synopsis:    Decompose a file path into its directory, filename and extension.
**
** Parameters:  path    - the path to decompose.
**              dir     - a pointer which will be set to the start of the directory.
**              name    - a pointer which will be set to the start of the name.
**              ext     - a pointer which will be set to the start of the extension.
**
** Returns:     NOTHING
**
** Constraints: path must use '/' and not '\' as directory separators.  The pointers
**              returned in dir, name, and ext are only valid until the next call
**              to util_DecomposePath().
**
**---------------------------------------------------------------------------*/
void util_decomposeFilePath( const char* path, char** dir, char** name, char** ext )
{
    static char buffer[ BUFFER_SIZE + 1 ];
    strncpy( buffer, path, BUFFER_SIZE );

    *ext = strrchr( buffer, '.' );
    if ( *ext )
    {
        /*
         * Remove the extension by replacing '.' with a NULL character.
         */
        **ext = '\0';

        /*
         * The extension will be in the next position.
         */
        (*ext)++;
    }

    *name = strrchr( buffer, '/' );
    if ( *name )
    {
        /*
         * Remove the name by replacing the last '/' with a NULL character.
         */
        **name = '\0';

        /*
         * The name will be in the next position.
         */
        (*name)++;

        /*
         * Set dir to the start of the string and we are done.
         */
        *dir = buffer;
    }
    else
    {
        /*
         * Name must extend to the beginning of the path. Set the name pointer
         * accordingly and set dir to null.
         */
        *name = buffer;
        *dir = 0;
    }
}

/*-----------------------------------------------------------------------------
** u t i l _ d i r n a m e
**
** Synopsis:    Determine the directory in a file path.
**
** Parameters:  buffer - buffer to recieve directory.
**              size   - size of buffer.
**              path   - the path to retrieve the directory from.
**
** Returns:     non-zero if successful, zero if buffer is too small to receive
**              the directory.
**
** Constraints: path must use '/' and not '\' as directory separators.
**
**---------------------------------------------------------------------------*/
int util_dirname( char* buffer, int size, const char* path )
{
    char* d;
    char* n;
    char* e;
    int length;
    int result = 0;

    util_decomposeFilePath( path, &d, &n, &e );

    length = strlen( d );
    if ( length < size )
    {
        strcpy( buffer, d );
        result = 1;
    }

    return result;
}

/*-----------------------------------------------------------------------------
** u t i l _ f i l e E x i s t s
**
** Synopsis:    Determines if the give file exists.  
**
** Parameters:  filename - the name of the file to check for existence.
**
** Returns:     0 if file does not exist, non-zero otherwise.
**
** Constraints: 
**
**---------------------------------------------------------------------------*/
int util_fileExists( const char* filename )
{
    FILE *fp;

    assert( filename );

    fp = fopen( filename, "r" );

    if ( fp ) 
    {
        fclose( fp );
        return 1;
    }

    return 0;
}

/*-----------------------------------------------------------------------------
** u t i l _ g e t o p t _ i n i t
**
** Synopsis:    Initializes the getopt parser.
**
** Parameters:  NONE
**
** Returns:     NOTHING
**
** Constraints: NONE
**
** Author:  Angus Mackay
**
**---------------------------------------------------------------------------*/
void util_getopt_init()
{
    util_optind = 1;
}

/*-----------------------------------------------------------------------------
** u t i l _ g e t o p t 
**
** Synopsis:    Used to parse command line arguments based on a specified
**              syntax.  The argument for the current option is stored in 
**              the global variable "util_optarg".
**
** Parameters:  argc - the number of arguments in the array.
**              argv - the argumnet array.
**              opts - option specifier string. Options can be of two forms:
**                     boolean flags ( its either there or it isn't ) and
**                     flag value pairs in which a flag is always followed
**                     by some value.  Options which require an argument
**                     must be followed by a ":" in the option specifier
**                     string.  So for a command which can have two boolean
**                     flags, "h" and "i", and two argument flags, "t" and
**                     "u", the following option specifier string would be
**                     used: "hit:u:".
**
** Returns:     the option character found, or -1 if no more option
**              characters.
**
** Constraints: NONE
**
** Author:  Angus Mackay
**
**---------------------------------------------------------------------------*/
int util_getopt( int argc, char* const* argv, const char* opts )
{
  static int init_done = 0;
  static int suboptionpos = 1;

  if(!init_done) { util_getopt_init(); init_done = 1; }

  util_optarg = NULL;

  if(util_optind == argc)
  {
    /* we are done */
    return(-1);
  }

  if(argv[util_optind][0] == '-')
  {
    char *argp;

    /* test for end of arg marker */
    if(argv[util_optind][1] == '-' && argv[util_optind][2] == '\0')
    {
      suboptionpos = 1;
      util_optind++;
      return(-1);
    }

    for(argp=&(argv[util_optind][suboptionpos]); *argp != '\0'; argp++)
    {
      char *optp;
      int numcolon = 0;
      char *p;

      if((optp=strchr(opts, *argp)) == NULL)
      {
        if(util_opterr != 0)
        {
          fprintf(stderr, "%s: illegal option -- %c\n", argv[0], *argp);
        }
        util_optopt = *argp;
        suboptionpos++;
        if(argv[util_optind][suboptionpos] == '\0')
        {
          suboptionpos = 1;
          util_optind++;
        }
        return('?');
      }

      /* zero, one or two colons? */
      for(p=(optp+1); *p == ':'; p++) { numcolon++; }
      switch(numcolon)
      {
        /* no argument */
        case 0:
          suboptionpos++;
          if(argv[util_optind][suboptionpos] == '\0')
          {
            suboptionpos = 1;
            util_optind++;
          }
          return(*optp);
          break;

        /* manditory argument */
        case 1:
          /* the argument is seperated by a space */
          if(argp[1] == '\0')
          {
            /* ther are more args */
            if(util_optind+1 == argc)
            {
              suboptionpos++;
              if(argv[util_optind][suboptionpos] == '\0')
              {
                suboptionpos = 1;
                util_optind++;
              }
              if(util_opterr != 0)
              {
                fprintf(stderr, "%s: option requires an argument -- %c\n", 
                        argv[0], *argp);
              }
              util_optopt = *argp;
              return('?');
            }

            util_optind++;
            suboptionpos = 1;
            util_optarg = argv[util_optind];
            util_optind++;
            return(*optp);
          }

          /* the argument is attached */
          util_optarg = argp+1;
          suboptionpos = 1;
          util_optind++;
          return(*optp);
          break;

        /* optional argument */
        case 2:
          /* the argument is seperated by a space */
          if(argp[1] == '\0')
          {
            util_optind++;
            suboptionpos = 1;
            util_optarg = NULL;
            return(*optp);
          }

          /* the argument is attached */
          suboptionpos = 1;
          util_optarg = argp+1;
          util_optind++;
          return(*optp);
          break;

        /* a case of too many colons */
        default:
          suboptionpos++;
          if(argv[util_optind][suboptionpos] == '\0')
          {
            suboptionpos = 1;
            util_optind++;
          }
          util_optopt = '?';
          return('?');
          break;
      }
    }
    suboptionpos = 1;
  }
  else
  {
    /* we are done */
    return(-1);
  }

  /* we shouldn't get here */
  return(-1);
} 

/*-----------------------------------------------------------------------------
** u t i l _ i n d e x 
**
** Synopsis:    returns a pointer to the first occurrence of the character
**              "c" in the string "s".
**
** Parameters:  s - the string to search.
**              c - the character to search for.
**
** Returns:     a pointer to the matched character, or NULL if the character
**              is not found.
**
** Constraints: NONE
**
**---------------------------------------------------------------------------*/
const char* util_index( const char* s, int c )
{
    const char* result = 0;
    const char* sp = 0;

    assert( s );

    sp = s;

    while ( *sp != '\0' )
    {
        if ( *sp == c )
        {
            result = sp;
            break;
        }

        sp++;
    }

    return result;
}  

/*-----------------------------------------------------------------------------
** u t i l _ p o s i x F i l e P a t h 
**
** Synopsis:    Ensures the given path uses POSIX style slashes.
**
** Parameters:  path - the path to "posixfy".
**
** Returns:     NOTHING
**
** Constraints: NONE
**
**---------------------------------------------------------------------------*/
void util_posixFilePath( char* path )
{
    char* pp = path;
    while ( *pp )
    {
        if ( '\\' == *pp ) *pp = '/';
        pp++;
    }
}   

/*-----------------------------------------------------------------------------
** u t i l _ r e p l a c e C h a r a c t e r s
**
** Synopsis:    Replace characters in a string.
**
** Parameters:  find    - the character to find.
**              replace - the charcter to replace the found character with.
**              string  - the string within which to do the find/replace.
**
** Returns:     NOTHING
**
** Constraints: NONE
**
**---------------------------------------------------------------------------*/
void util_replaceCharacters( char find, char replace, char* string )
{
    char* p = 0;

    if ( string )
    {
        while ( p = strchr( string, find ) )
        {
            *p = replace;
        }
    }
}

/*-----------------------------------------------------------------------------
** u t i l _ r e v e r s e S p a n 
**
** Synopsis:    Finds the suffix which is composed completely of characters in
**              in the spanSet in the specified string.
**
** Example:     end = util_ReverseSpan( "name123", "312" );
**               * 
**               * end now points to "123".
**               *
**              end = util_ReverseSpan( "name123", "21" );
**               *
**               * end would point to NULL because the character "3" is not
**               * in the spanSet.
**
** Parameters:  string  - the string to be searched.
**              spanSet - a string of characters that can be in the suffix. 
**
** Returns:     a pointer to the suffix string, or NULL if there is no
**              suffix.
**
** Constraints: NONE
**
**---------------------------------------------------------------------------*/
const char* util_reverseSpan( const char* string, const char* spanSet )
{
    const char* sp = 0;
    const char* next_sp = 0;
    int isEndSpan = 0;
    
    /* 
     * Set a pointer to the end of the string to be searched. 
     */
    sp = string + strlen( string );

    /*
     *  If the string passed in is empty, we are done.
     */
    if ( sp == string ) return sp;

    while( sp > string && !isEndSpan )
    {
        next_sp = sp - 1;

        if ( strchr( spanSet, *next_sp ) )
        {
            sp--;
        }
        else
        {
            isEndSpan = 1;
        }
    }

    return sp;
}

/*-----------------------------------------------------------------------------
** u t i l _ r i n d e x 
**
** Synopsis:    returns a pointer to the last occurrence of the character
**              "c" in the string "s".
**
** Parameters:  s - the string to search.
**              c - the character to search for.
**
** Returns:     a pointer to the matched character, or NULL if the character
**              is not found.
**
** Constraints: NONE
**
**---------------------------------------------------------------------------*/
const char* util_rindex( const char* s, int c )
{
    const char* result = 0;
    const char* sp = 0;
    int length;

    assert( s );

    length = strlen(s);
    sp = s + length;

    if ( length > 0 )
    {
        while ( sp != s )
        {
            sp--;

            if ( *sp == c )
            {
                result = sp;
                break;
            }
        }
    }

    return result;
}

/*-----------------------------------------------------------------------------
** u t i l _ s u b s t i t u t e 
**
** Synopsis:    Substitue strings matchin "find" with "replace" in "string".
**              If this cannot be done without exceeding the size of "string"
**              An error will be returned and "string" will be as it was 
**              when util_substitute was originally called.
**
** Parameters:  find       - the substring to find.
**              replace    - the substring to insert in place of found strings.
**              string     - the string to operate on.
**              stringSize - the maximum size the string can grow to.
**              firstOnly  - flag.  If non-zero only the first occurence of
**                           "find" will be replaced.  If zero, all occurences
**                           will be replaced.
**
** Returns:     0 on error, the new size of the string on success.
**
** Constraints: NONE
**
**---------------------------------------------------------------------------*/
int util_substitute( const char* find,
                     const char* replace,
                     char* string,
                     int stringSize,
                     int firstOnly
                   )
{
    char buffer[ BUFFER_SIZE + 1 ];
    char* sp;
    char* bp;
    char* p;
    int findLength = strlen( find );
    int replaceLength = strlen( replace );
    int count = 0;
    int total = 0;
    int tooBig = 0;
    int status = 0;

    assert( stringSize <= BUFFER_SIZE );
    
    bp = buffer;
    sp = string;
    while( !tooBig && (p = strstr( sp, find )) )
    {
        /*
         * Determine the number of characters since last find.
         * Add to total characters.
         * If total characters exceeds "stringSize" then break and return 
         * error. Otherwise copy characters since last find to buffer.
         */
        count = p - sp;
        total += count;
        if ( total >= stringSize )
        {
            tooBig = 1;
            continue;
        }
        strncpy( bp, sp, count );
        bp += count;
        *bp = '\0';

        /*
         * If adding the replace string causes buffer to exceed "stringSize"
         * break and return error.  Otherwise copy the replace string into
         * buffer.
         */
        total += replaceLength;
        if ( total >= stringSize ) 
        {
            tooBig = 1;
            continue;
        }
        strcpy( bp, replace );
        bp += replaceLength;
        
        /*
         * Update the position of sp so that it points to next character
         * after the last found "find" string.
         */
        sp += count + findLength;
    }

    if ( !tooBig )
    {
        /*
         * Attempt to copy the remaining portion of string into buffer.
         * If successful, copy buffer into string and return success.
         * Otherwise return error.
         */
        total += strlen( sp );
        if ( total < stringSize )
        {
            strcpy( bp, sp );
            strcpy( string, buffer );
            status = strlen( string );
        }
    }

    return status;
}

