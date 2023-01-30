 *****************************************************************************/
int Import_SGIMB( vlc_object_t * p_this )
{
    demux_t *p_demux = (demux_t *)p_this;
    const uint8_t *p_peek;
    int i_size;
    /* Lets check the content to see if this is a sgi mediabase file */
    i_size = stream_Peek( p_demux->s, &p_peek, MAX_LINE );
    i_size -= sizeof("sgiNameServerHost=") - 1;
    if ( i_size > 0 )
    {
        unsigned int i_len = sizeof("sgiNameServerHost=") - 1;
        while ( i_size && strncasecmp( (char *)p_peek, "sgiNameServerHost=", i_len ) )
        {
            p_peek++;
            i_size--;
        }
        if ( !strncasecmp( (char *)p_peek, "sgiNameServerHost=", i_len ) )
        {
            STANDARD_DEMUX_INIT_MSG( "using SGIMB playlist reader" );
            p_demux->p_sys->psz_uri = NULL;
            p_demux->p_sys->psz_server = NULL;
            p_demux->p_sys->psz_location = NULL;
            p_demux->p_sys->psz_name = NULL;
            p_demux->p_sys->psz_user = NULL;
            p_demux->p_sys->psz_password = NULL;
            p_demux->p_sys->psz_mcast_ip = NULL;
            p_demux->p_sys->i_mcast_port = 0;
            p_demux->p_sys->i_packet_size = 0;
            p_demux->p_sys->i_duration = 0;
            p_demux->p_sys->i_port = 0;
            p_demux->p_sys->i_sid = 0;
            p_demux->p_sys->b_rtsp_kasenna = false;
            p_demux->p_sys->b_concert = false;
            return VLC_SUCCESS;
        }
    }
    return VLC_EGENERIC;
}
