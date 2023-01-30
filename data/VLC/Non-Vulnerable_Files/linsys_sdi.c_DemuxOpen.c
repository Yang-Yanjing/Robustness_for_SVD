 *****************************************************************************/
static int DemuxOpen( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t *)p_this;
    demux_sys_t *p_sys;
    char        *psz_parser;
    /* Fill p_demux field */
    p_demux->pf_demux = DemuxDemux;
    p_demux->pf_control = DemuxControl;
    p_demux->p_sys = p_sys = calloc( 1, sizeof( demux_sys_t ) );
    if( unlikely(!p_sys ) )
        return VLC_ENOMEM;
    p_sys->i_state = STATE_NOSYNC;
    p_sys->i_last_state_change = mdate();
    /* SDI AR */
    char *psz_ar = var_InheritString( p_demux, "linsys-sdi-aspect-ratio" );
    if ( psz_ar != NULL )
    {
        psz_parser = strchr( psz_ar, ':' );
        if ( psz_parser )
        {
            *psz_parser++ = '\0';
            p_sys->i_forced_aspect = p_sys->i_aspect =
                 strtol( psz_ar, NULL, 0 ) * VOUT_ASPECT_FACTOR
                 / strtol( psz_parser, NULL, 0 );
        }
        else
            p_sys->i_forced_aspect = 0;
        free( psz_ar );
    }
    /* */
    p_sys->i_id_video = var_InheritInteger( p_demux, "linsys-sdi-id-video" );
    /* Audio ES */
    char *psz_string = psz_parser = var_InheritString( p_demux,
                                                       "linsys-sdi-audio" );
    int i = 0;
    while ( psz_parser != NULL && *psz_parser )
    {
        int i_id, i_group, i_pair;
        char *psz_next = strchr( psz_parser, '=' );
        if ( psz_next != NULL )
        {
            *psz_next = '\0';
            i_id = strtol( psz_parser, NULL, 0 );
            psz_parser = psz_next + 1;
        }
        else
            i_id = 0;
        psz_next = strchr( psz_parser, ':' );
        if ( psz_next != NULL )
        {
            *psz_next = '\0';
            psz_next++;
        }
        if ( sscanf( psz_parser, "%d,%d", &i_group, &i_pair ) == 2 )
        {
            p_sys->p_audios[i].i_group = i_group;
            p_sys->p_audios[i].i_pair = i_pair;
            p_sys->p_audios[i].i_id = i_id;
            i++;
        }
        else
            msg_Warn( p_demux, "malformed audio configuration (%s)",
                      psz_parser );
        psz_parser = psz_next;
    }
    free( psz_string );
    /* Teletext ES */
    p_sys->psz_telx = var_InheritString( p_demux, "linsys-sdi-telx" );
    p_sys->psz_telx_lang = var_InheritString( p_demux, "linsys-sdi-telx-lang" );
    return VLC_SUCCESS;
}
