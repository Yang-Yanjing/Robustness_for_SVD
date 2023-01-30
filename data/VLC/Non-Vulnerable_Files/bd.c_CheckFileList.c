 *****************************************************************************/
static int CheckFileList( const char *psz_base, const char *ppsz_name[] )
{
    for( int i = 0; ppsz_name[i] != NULL ; i++ )
    {
        struct stat s;
        char *psz_tmp;
        if( asprintf( &psz_tmp, "%s/%s", psz_base, ppsz_name[i] ) < 0 )
            return VLC_EGENERIC;
        bool b_ok = vlc_stat( psz_tmp, &s ) == 0 && S_ISREG( s.st_mode );
        free( psz_tmp );
        if( !b_ok )
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
