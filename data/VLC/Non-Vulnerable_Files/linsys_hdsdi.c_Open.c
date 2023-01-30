 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t *)p_this;
    demux_sys_t *p_sys;
    char        *psz_parser;
    /* Fill p_demux field */
    p_demux->pf_demux = Demux;
    p_demux->pf_control = Control;
    p_demux->p_sys = p_sys = calloc( 1, sizeof( demux_sys_t ) );
    if( unlikely(!p_sys) )
        return VLC_ENOMEM;
    /* HDSDI AR */
    char *psz_ar = var_InheritString( p_demux, "linsys-hdsdi-aspect-ratio" );
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
    p_sys->i_id_video = var_InheritInteger( p_demux, "linsys-hdsdi-id-video" );
    /* Audio ES */
    char *psz_string = psz_parser = var_InheritString( p_demux,
                                                       "linsys-hdsdi-audio" );
    int i = 0;
    p_sys->i_max_channel = -1;
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
            p_sys->p_audios[i].i_channel = (i_group - 1) * 2 + (i_pair - 1);
            if ( p_sys->p_audios[i].i_channel > p_sys->i_max_channel )
                p_sys->i_max_channel = p_sys->p_audios[i].i_channel;
            p_sys->p_audios[i].i_id = i_id;
            i++;
        }
        else
            msg_Warn( p_demux, "malformed audio configuration (%s)",
                      psz_parser );
        psz_parser = psz_next;
    }
    free( psz_string );
    for ( ; i < MAX_AUDIOS; i++ )
        p_sys->p_audios[i].i_channel = -1;
    p_sys->i_link = var_InheritInteger( p_demux, "linsys-hdsdi-link" );
    if( InitCapture( p_demux ) != VLC_SUCCESS )
    {
        free( p_sys );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
