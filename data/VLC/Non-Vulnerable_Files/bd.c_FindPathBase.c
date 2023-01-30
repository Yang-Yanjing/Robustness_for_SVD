/* */
static char *FindPathBase( const char *psz_path, bool *pb_shortname )
{
    struct stat s;
    char *psz_tmp;
    /* */
    char *psz_base = strdup( psz_path );
    if( !psz_base )
        return NULL;
    /* */
    while( *psz_base && psz_base[strlen(psz_base)-1] == '/' )
        psz_base[strlen(psz_base)-1] = '\0';
    /* */
    if( vlc_stat( psz_base, &s ) || !S_ISDIR( s.st_mode ) )
        goto error;
    /* Check BDMV */
    if( asprintf( &psz_tmp, "%s/BDMV", psz_base ) < 0 )
        goto error;
    if( !vlc_stat( psz_tmp, &s ) && S_ISDIR( s.st_mode ) )
    {
        free( psz_base );
        psz_base = psz_tmp;
    }
    else
    {
        free( psz_tmp );
    }
    /* Check presence of mandatory files */
    static const char *ppsz_name_long[] = {
        "index.bdmv",
        "MovieObject.bdmv",
        NULL
    };
    static const char *ppsz_name_short[] = {
        "INDEX.BDM",
        "MOVIEOBJ.BDM",
        NULL
    };
    *pb_shortname = false;
    if( CheckFileList( psz_base, ppsz_name_long ) )
    {
        if( CheckFileList( psz_base, ppsz_name_short ) )
            goto error;
        *pb_shortname = true;
    }
    return psz_base;
error:
    free( psz_base );
    return NULL;
}
