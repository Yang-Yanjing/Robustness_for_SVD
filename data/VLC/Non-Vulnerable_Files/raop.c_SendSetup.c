}
static int SendSetup( vlc_object_t *p_this )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    vlc_dictionary_t req_headers;
    vlc_dictionary_t resp_headers;
    int i_err = VLC_SUCCESS;
    char *psz_tmp;
    char *psz_next;
    char *psz_name;
    char *psz_value;
    vlc_dictionary_init( &req_headers, 0 );
    vlc_dictionary_init( &resp_headers, 0 );
    vlc_dictionary_insert( &req_headers, "Transport",
                           ((void*)"RTP/AVP/TCP;unicast;interleaved=0-1;"
                            "mode=record") );
    i_err = ExecRequest( p_this, "SETUP", NULL, NULL,
                         &req_headers, &resp_headers );
    if ( i_err != VLC_SUCCESS )
        goto error;
    psz_tmp = vlc_dictionary_value_for_key( &resp_headers, "Session" );
    if ( !psz_tmp )
    {
        msg_Err( p_this, "Missing 'Session' header during setup" );
        i_err = VLC_EGENERIC;
        goto error;
    }
    free( p_sys->psz_session );
    p_sys->psz_session = strdup( psz_tmp );
    /* Get server_port */
    psz_next = vlc_dictionary_value_for_key( &resp_headers, "Transport" );
    while ( SplitHeader( &psz_next, &psz_name, &psz_value ) )
    {
        if ( psz_value && strcmp( psz_name, "server_port" ) == 0 )
        {
            p_sys->i_server_port = atoi( psz_value );
            break;
        }
    }
    if ( !p_sys->i_server_port )
    {
        msg_Err( p_this, "Missing 'server_port' during setup" );
        i_err = VLC_EGENERIC;
        goto error;
    }
    /* Get jack type */
    psz_next = vlc_dictionary_value_for_key( &resp_headers,
                                             "Audio-Jack-Status" );
    while ( SplitHeader( &psz_next, &psz_name, &psz_value ) )
    {
        if ( strcmp( psz_name, "type" ) != 0 )
            continue;
        if ( strcmp( psz_value, "analog" ) == 0 )
            p_sys->i_jack_type = JACK_TYPE_ANALOG;
        else if ( strcmp( psz_value, "digital" ) == 0 )
            p_sys->i_jack_type = JACK_TYPE_DIGITAL;
        break;
    }
error:
    vlc_dictionary_clear( &req_headers, NULL, NULL );
    vlc_dictionary_clear( &resp_headers, FreeHeader, NULL );
    return i_err;
}
