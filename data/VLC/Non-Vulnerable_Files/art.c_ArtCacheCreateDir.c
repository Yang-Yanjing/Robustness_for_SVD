#include "art.h"
static void ArtCacheCreateDir( const char *psz_dir )
{
    char newdir[strlen( psz_dir ) + 1];
    strcpy( newdir, psz_dir );
    char * psz_newdir = newdir;
    char * psz = psz_newdir;
    while( *psz )
    {
        while( *psz && *psz != DIR_SEP_CHAR) psz++;
        if( !*psz ) break;
        *psz = 0;
        if( !EMPTY_STR( psz_newdir ) )
            vlc_mkdir( psz_newdir, 0700 );
        *psz = DIR_SEP_CHAR;
        psz++;
    }
    vlc_mkdir( psz_dir, 0700 );
}
