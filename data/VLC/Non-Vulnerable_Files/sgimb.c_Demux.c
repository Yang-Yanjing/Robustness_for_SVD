 *****************************************************************************/
static int Demux ( demux_t *p_demux )
{
    demux_sys_t     *p_sys = p_demux->p_sys;
    input_item_t    *p_child = NULL;
    char            *psz_line;
    input_item_t *p_current_input = GetCurrentItem(p_demux);
    while( ( psz_line = stream_ReadLine( p_demux->s ) ) )
    {
        ParseLine( p_demux, psz_line );
        free( psz_line );
    }
    if( p_sys->psz_mcast_ip )
    {
        /* Definetly schedules multicast session */
        /* We don't care if it's live or not */
        free( p_sys->psz_uri );
        if( asprintf( &p_sys->psz_uri, "udp://@" "%s:%i", p_sys->psz_mcast_ip, p_sys->i_mcast_port ) == -1 )
        {
            p_sys->psz_uri = NULL;
            return -1;
        }
    }
    if( p_sys->psz_uri == NULL )
    {
        if( p_sys->psz_server && p_sys->psz_location )
        {
            if( asprintf( &p_sys->psz_uri, "rtsp://" "%s:%i%s",
                     p_sys->psz_server, p_sys->i_port > 0 ? p_sys->i_port : 554, p_sys->psz_location ) == -1 )
            {
                p_sys->psz_uri = NULL;
                return -1;
            }
        }
    }
    if( p_sys->b_concert )
    {
        /* It's definetly a simulcasted scheduled stream */
        /* We don't care if it's live or not */
        if( p_sys->psz_uri == NULL )
        {
            msg_Err( p_demux, "no URI was found" );
            return -1;
        }
        char *uri;
        if( asprintf( &uri, "%s%%3FMeDiAbAsEshowingId=%d%%26MeDiAbAsEconcert"
                      "%%3FMeDiAbAsE", p_sys->psz_uri, p_sys->i_sid ) == -1 )
            return -1;
        free( p_sys->psz_uri );
        p_sys->psz_uri = uri;
    }
    p_child = input_item_NewWithType( p_sys->psz_uri,
                      p_sys->psz_name ? p_sys->psz_name : p_sys->psz_uri,
                      0, NULL, 0, p_sys->i_duration, ITEM_TYPE_NET );
    if( !p_child )
    {
        msg_Err( p_demux, "A valid playlistitem could not be created" );
        return -1;
    }
    input_item_CopyOptions( p_current_input, p_child );
    if( p_sys->i_packet_size && p_sys->psz_mcast_ip )
    {
        char *psz_option;
        p_sys->i_packet_size += 1000;
        if( asprintf( &psz_option, "mtu=%i", p_sys->i_packet_size ) != -1 )
        {
            input_item_AddOption( p_child, psz_option, VLC_INPUT_OPTION_TRUSTED );
            free( psz_option );
        }
    }
    if( !p_sys->psz_mcast_ip )
        input_item_AddOption( p_child, "rtsp-caching=5000", VLC_INPUT_OPTION_TRUSTED );
    if( !p_sys->psz_mcast_ip && p_sys->b_rtsp_kasenna )
        input_item_AddOption( p_child, "rtsp-kasenna", VLC_INPUT_OPTION_TRUSTED );
    input_item_PostSubItem( p_current_input, p_child );
    vlc_gc_decref( p_child );
    vlc_gc_decref(p_current_input);
    return 0; /* Needed for correct operation of go back */
}
