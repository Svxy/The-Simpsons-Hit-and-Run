#ifndef _UTIL_H
#define _UTIL_H
/*-----------------------------------------------------------------------------
** Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
**
** util.h
**
** Description: A set of utilities for various things.
**
** Modification History:
**  + Created Aug 03, 2001 -- bkusy 
**---------------------------------------------------------------------------*/

/*----------------------------------------
** System Includes
**--------------------------------------*/

/*----------------------------------------
** Project Includes
**--------------------------------------*/

/*----------------------------------------
** Forward References
**--------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------
** Globals
**--------------------------------------*/
extern char* util_optarg;
extern int   util_optind;
extern int   util_opterr;
extern int   util_optopt;

/*----------------------------------------
** Macros
**--------------------------------------*/


/*----------------------------------------
** Functions
**--------------------------------------*/

extern int         util_basename( char* buffer, int size, const char* path );

extern void        util_changeFileExtension( char* buffer, const char* ext );

extern void        util_decomposeFilePath( const char* path, 
                                           char** dir, 
                                           char** name, 
                                           char** ext 
                                         );

extern int         util_dirname( char* buffer, int size, const char* path );

extern int         util_fileExists( const char* filename );

extern void        util_getopt_init();

extern int         util_getopt( int argc, char* const* argv, const char *opts );

extern const char* util_index( const char* s, int c );

extern void        util_posixFilePath( char* path );

extern void        util_replaceCharacters( char find, 
                                           char replace, 
                                           char* string 
                                         );

extern const char* util_reverseSpan( const char* string, const char* spanSet );

extern const char* util_rindex( const char* s, int c );

extern int         util_substitute( const char* find, 
                                    const char* replace,
                                    char* string,
                                    int stringSize,
                                    int firstOnly
                                   );
#ifdef __cplusplus
}
#endif

#endif
