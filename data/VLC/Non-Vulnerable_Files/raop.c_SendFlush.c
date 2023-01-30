}
static int SendFlush( vlc_object_t *p_this )
{
    VLC_UNUSED( p_this );
    vlc_dictionary_t resp_headers;
    vlc_dictionary_t req_headers;
    int i_err = VLC_SUCCESS;
    vlc_dictionary_init( &req_headers, 0 );
    vlc_dictionary_init( &resp_headers, 0 );
    vlc_dictionary_insert( &req_headers, "RTP-Info",
                           (void *)"seq=0;rtptime=0" );
    i_err = ExecRequest( p_this, "FLUSH", NULL, NULL,
                         &req_headers, &resp_headers );
    if ( i_err != VLC_SUCCESS )
        goto error;
error:
    vlc_dictionary_clear( &req_headers, NULL, NULL );
    vlc_dictionary_clear( &resp_headers, FreeHeader, NULL );
    return i_err;
}
