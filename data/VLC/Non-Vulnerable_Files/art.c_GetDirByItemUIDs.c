}
static char * GetDirByItemUIDs( char *psz_uid )
{
    char *psz_cachedir = config_GetUserDir(VLC_CACHE_DIR);
    char *psz_dir;
    if( asprintf( &psz_dir, "%s" DIR_SEP
                  "by-iiuid" DIR_SEP
                  "%s",
                  psz_cachedir, psz_uid ) == -1 )
    {
        psz_dir = NULL;
    }
    free( psz_cachedir );
    return psz_dir;
}
