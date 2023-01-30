}
static int WriteAuxHeaders( vlc_object_t *p_this,
                            vlc_dictionary_t *p_req_headers )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    char **ppsz_keys = NULL;
    char *psz_key;
    char *psz_value;
    int i_err = VLC_SUCCESS;
    int i_rc;
    size_t i;
    ppsz_keys = vlc_dictionary_all_keys( p_req_headers );
    for ( i = 0; ppsz_keys[i]; ++i )
    {
        psz_key = ppsz_keys[i];
        psz_value = vlc_dictionary_value_for_key( p_req_headers, psz_key );
        i_rc = net_Printf( p_this, p_sys->i_control_fd, NULL,
                           "%s: %s\r\n", psz_key, psz_value );
        if ( i_rc < 0 )
        {
            i_err = VLC_EGENERIC;
            goto error;
        }
    }
error:
    for ( i = 0; ppsz_keys[i]; ++i )
        free( ppsz_keys[i] );
    free( ppsz_keys );
    return i_err;
}
