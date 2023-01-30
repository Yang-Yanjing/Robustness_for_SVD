}
static int SendRecord( vlc_object_t *p_this )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    vlc_dictionary_t req_headers;
    vlc_dictionary_t resp_headers;
    int i_err = VLC_SUCCESS;
    char *psz_value;
    vlc_dictionary_init( &req_headers, 0 );
    vlc_dictionary_init( &resp_headers, 0 );
    vlc_dictionary_insert( &req_headers, "Range", (void *)"npt=0-" );
    vlc_dictionary_insert( &req_headers, "RTP-Info",
                           (void *)"seq=0;rtptime=0" );
    vlc_dictionary_insert( &req_headers, "Session",
                           (void *)p_sys->psz_session );
    i_err = ExecRequest( p_this, "RECORD", NULL, NULL,
                         &req_headers, &resp_headers );
    if ( i_err != VLC_SUCCESS )
        goto error;
    psz_value = vlc_dictionary_value_for_key( &resp_headers, "Audio-Latency" );
    if ( psz_value )
        p_sys->i_audio_latency = atoi( psz_value );
    else
        p_sys->i_audio_latency = 0;
error:
    vlc_dictionary_clear( &req_headers, NULL, NULL );
    vlc_dictionary_clear( &resp_headers, FreeHeader, NULL );
    return i_err;
}
