}
static int Load( demux_t *p_demux,
                 const char *psz_dir,
                 int (*pf_filter)( const char * ),
                 void (*pf_load)( demux_t *p_demux, const char *psz_name, int i_id ) )
{
    char *psz_playlist;
    if( asprintf( &psz_playlist, "%s/%s", p_demux->p_sys->psz_base, psz_dir ) < 0 )
        return VLC_EGENERIC;
    char **ppsz_list;
    int i_list = vlc_scandir( psz_playlist, &ppsz_list, pf_filter, ScanSort );
    for( int i = 0; i < i_list; i++ )
    {
        char *psz_file = ppsz_list[i];
        if( !psz_file )
            break;
        char *psz_name;
        if( asprintf( &psz_name, "%s/%s/%s", p_demux->p_sys->psz_base, psz_dir, psz_file ) >= 0)
        {
            const int i_id = strtol( psz_file, NULL, 10 );
            pf_load( p_demux, psz_name, i_id );
            free( psz_name );
        }
        free( psz_file );
    }
    free( ppsz_list );
    free( psz_playlist );
    return VLC_SUCCESS;
}
