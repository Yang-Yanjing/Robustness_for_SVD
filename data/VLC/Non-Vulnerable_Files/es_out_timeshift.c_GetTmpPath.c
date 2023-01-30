 *****************************************************************************/
static char *GetTmpPath( char *psz_path )
{
    if( psz_path && *psz_path )
    {
        /* Make sure that the path exists and is a directory */
        struct stat s;
        const int i_ret = vlc_stat( psz_path, &s );
        if( i_ret < 0 && !vlc_mkdir( psz_path, 0600 ) )
            return psz_path;
        else if( i_ret == 0 && ( s.st_mode & S_IFDIR ) )
            return psz_path;
    }
    free( psz_path );
    /* Create a suitable path */
#if defined (_WIN32) && !VLC_WINSTORE_APP
    const DWORD dwCount = GetTempPathW( 0, NULL );
    wchar_t *psw_path = calloc( dwCount + 1, sizeof(wchar_t) );
    if( psw_path )
    {
        if( GetTempPathW( dwCount + 1, psw_path ) <= 0 )
        {
            free( psw_path );
            psw_path = _wgetcwd( NULL, 0 );
        }
    }
    psz_path = NULL;
    if( psw_path )
    {
        psz_path = FromWide( psw_path );
        while( psz_path && *psz_path && psz_path[strlen( psz_path ) - 1] == '\\' )
            psz_path[strlen( psz_path ) - 1] = '\0';
        free( psw_path );
    }
    if( !psz_path || *psz_path == '\0' )
    {
        free( psz_path );
        return strdup( "C:" );
    }
#else
    psz_path = strdup( DIR_SEP"tmp" );
#endif
    return psz_path;
}
