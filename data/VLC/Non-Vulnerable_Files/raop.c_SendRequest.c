}
static int SendRequest( vlc_object_t *p_this, const char *psz_method,
                        const char *psz_content_type, const char *psz_body,
                        vlc_dictionary_t *p_req_headers )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    const unsigned char psz_headers_end[] = "\r\n";
    size_t i_body_length = 0;
    int i_err = VLC_SUCCESS;
    int i_rc;
    i_rc = net_Printf( p_this, p_sys->i_control_fd, NULL,
                       "%s %s RTSP/1.0\r\n"
                       "User-Agent: " RAOP_USER_AGENT "\r\n"
                       "Client-Instance: %s\r\n"
                       "CSeq: %d\r\n",
                       psz_method, p_sys->psz_url,
                       p_sys->psz_client_instance,
                       ++p_sys->i_cseq );
    if ( i_rc < 0 )
    {
        i_err = VLC_EGENERIC;
        goto error;
    }
    if ( psz_content_type )
    {
        i_rc = net_Printf( p_this, p_sys->i_control_fd, NULL,
                           "Content-Type: %s\r\n", psz_content_type );
        if ( i_rc < 0 )
        {
            i_err = VLC_ENOMEM;
            goto error;
        }
    }
    if ( psz_body )
    {
        i_body_length = strlen( psz_body );
        i_rc = net_Printf( p_this, p_sys->i_control_fd, NULL,
                           "Content-Length: %u\r\n",
                           (unsigned int)i_body_length );
        if ( i_rc < 0 )
        {
            i_err = VLC_ENOMEM;
            goto error;
        }
    }
    i_err = WriteAuxHeaders( p_this, p_req_headers );
    if ( i_err != VLC_SUCCESS )
        goto error;
    i_rc = net_Write( p_this, p_sys->i_control_fd, NULL,
                      psz_headers_end, sizeof( psz_headers_end ) - 1 );
    if ( i_rc < 0 )
    {
        i_err = VLC_ENOMEM;
        goto error;
    }
    if ( psz_body )
        net_Write( p_this, p_sys->i_control_fd, NULL,
                   psz_body, i_body_length );
error:
    return i_err;
}
