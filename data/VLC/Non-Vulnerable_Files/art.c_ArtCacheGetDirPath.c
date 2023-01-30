}
static char* ArtCacheGetDirPath( const char *psz_arturl, const char *psz_artist,
                                 const char *psz_album,  const char *psz_title )
{
    char *psz_dir;
    char *psz_cachedir = config_GetUserDir(VLC_CACHE_DIR);
    if( !EMPTY_STR(psz_artist) && !EMPTY_STR(psz_album) )
    {
        char *psz_album_sanitized = strdup( psz_album );
        filename_sanitize( psz_album_sanitized );
        char *psz_artist_sanitized = strdup( psz_artist );
        filename_sanitize( psz_artist_sanitized );
        if( asprintf( &psz_dir, "%s" DIR_SEP "art" DIR_SEP "artistalbum"
                      DIR_SEP "%s" DIR_SEP "%s", psz_cachedir,
                      psz_artist_sanitized, psz_album_sanitized ) == -1 )
            psz_dir = NULL;
        free( psz_album_sanitized );
        free( psz_artist_sanitized );
    }
    else
    {
        /* If artist or album are missing, cache by art download URL.
         * If the URL is an attachment://, add the title to the cache name.
         * It will be md5 hashed to form a valid cache filename.
         * We assume that psz_arturl is always the download URL and not the
         * already hashed filename.
         * (We should never need to call this function if art has already been
         * downloaded anyway).
         */
        struct md5_s md5;
        InitMD5( &md5 );
        AddMD5( &md5, psz_arturl, strlen( psz_arturl ) );
        if( !strncmp( psz_arturl, "attachment://", 13 ) )
            AddMD5( &md5, psz_title, strlen( psz_title ) );
        EndMD5( &md5 );
        char * psz_arturl_sanitized = psz_md5_hash( &md5 );
        if( asprintf( &psz_dir, "%s" DIR_SEP "art" DIR_SEP "arturl" DIR_SEP
                      "%s", psz_cachedir, psz_arturl_sanitized ) == -1 )
            psz_dir = NULL;
        free( psz_arturl_sanitized );
    }
    free( psz_cachedir );
    return psz_dir;
}
