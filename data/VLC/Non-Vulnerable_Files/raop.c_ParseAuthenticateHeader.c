}
static int ParseAuthenticateHeader( vlc_object_t *p_this,
                                    vlc_dictionary_t *p_resp_headers )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    char *psz_auth;
    int i_err = VLC_SUCCESS;
    psz_auth = vlc_dictionary_value_for_key( p_resp_headers,
                                             "WWW-Authenticate" );
    if ( psz_auth == NULL )
    {
        msg_Err( p_this, "HTTP 401 response missing "
                         "WWW-Authenticate header" );
        i_err = VLC_EGENERIC;
        goto error;
    }
    http_auth_ParseWwwAuthenticateHeader( p_this, &p_sys->auth, psz_auth );
error:
    return i_err;
}
