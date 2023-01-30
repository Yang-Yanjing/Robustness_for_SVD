}
static int ExecRequest( vlc_object_t *p_this, const char *psz_method,
                        const char *psz_content_type, const char *psz_body,
                        vlc_dictionary_t *p_req_headers,
                        vlc_dictionary_t *p_resp_headers )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    char *psz_authorization = NULL;
    int headers_done;
    int i_err = VLC_SUCCESS;
    int i_status;
    int i_auth_state;
    if ( p_sys->i_control_fd < 0 )
    {
        msg_Err( p_this, "Control connection not open" );
        i_err = VLC_EGENERIC;
        goto error;
    }
    i_auth_state = 0;
    while ( 1 )
    {
        /* Send header only when Digest authentication is used */
        if ( p_sys->psz_password != NULL && p_sys->auth.psz_nonce != NULL )
        {
            FREENULL( psz_authorization );
            psz_authorization =
                http_auth_FormatAuthorizationHeader( p_this, &p_sys->auth,
                                                     psz_method,
                                                     p_sys->psz_url, "",
                                                     p_sys->psz_password );
            if ( psz_authorization == NULL )
            {
                i_err = VLC_EGENERIC;
                goto error;
            }
            vlc_dictionary_insert( p_req_headers, "Authorization",
                                   psz_authorization );
        }
        /* Send request */
        i_err = SendRequest( p_this, psz_method, psz_content_type, psz_body,
                             p_req_headers);
        if ( i_err != VLC_SUCCESS )
            goto error;
        /* Read status line */
        i_status = ReadStatusLine( p_this );
        if ( i_status < 0 )
        {
            i_err = i_status;
            goto error;
        }
        vlc_dictionary_clear( p_resp_headers, FreeHeader, NULL );
        /* Read headers */
        headers_done = 0;
        while ( !headers_done )
        {
            i_err = ReadHeader( p_this, p_resp_headers, &headers_done );
            if ( i_err != VLC_SUCCESS )
                goto error;
        }
        if ( i_status == 200 )
            /* Request successful */
            break;
        else if ( i_status == 401 )
        {
            /* Authorization required */
            if ( i_auth_state == 1 || p_sys->psz_password == NULL )
            {
                msg_Err( p_this, "Access denied, password invalid" );
                i_err = VLC_EGENERIC;
                goto error;
            }
            i_err = ParseAuthenticateHeader( p_this, p_resp_headers );
            if ( i_err != VLC_SUCCESS )
                goto error;
            i_auth_state = 1;
        }
        else
        {
            msg_Err( p_this, "Request failed (%s), status is %d",
                     p_sys->psz_last_status_line, i_status );
            i_err = VLC_EGENERIC;
            goto error;
        }
    }
error:
    FREENULL( p_sys->psz_last_status_line );
    free( psz_authorization );
    return i_err;
}
