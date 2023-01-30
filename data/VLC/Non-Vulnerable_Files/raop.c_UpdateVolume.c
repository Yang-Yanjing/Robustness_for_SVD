}
static int UpdateVolume( vlc_object_t *p_this )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    vlc_dictionary_t req_headers;
    vlc_dictionary_t resp_headers;
    char *psz_parameters = NULL;
    double d_volume;
    int i_err = VLC_SUCCESS;
    int i_rc;
    vlc_dictionary_init( &req_headers, 0 );
    vlc_dictionary_init( &resp_headers, 0 );
    /* Our volume is 0..255, RAOP is -144..0 (-144 off, -30..0 on) */
    /* Limit range */
    p_sys->i_volume = VLC_CLIP( p_sys->i_volume, 0, 255 );
    if ( p_sys->i_volume == 0 )
        d_volume = -144.0;
    else
        d_volume = -30 + ( ( (double)p_sys->i_volume ) * 30.0 / 255.0 );
    /* Format without using locales */
    i_rc = us_asprintf( &psz_parameters, "volume: %0.6f\r\n", d_volume );
    if ( i_rc < 0 )
    {
        i_err = VLC_ENOMEM;
        goto error;
    }
    vlc_dictionary_insert( &req_headers, "Session",
                           (void *)p_sys->psz_session );
    i_err = ExecRequest( p_this, "SET_PARAMETER",
                         "text/parameters", psz_parameters,
                         &req_headers, &resp_headers );
    if ( i_err != VLC_SUCCESS )
        goto error;
error:
    vlc_dictionary_clear( &req_headers, NULL, NULL );
    vlc_dictionary_clear( &resp_headers, FreeHeader, NULL );
    free( psz_parameters );
    return i_err;
}
