}
static int ReadHeader( vlc_object_t *p_this,
                       vlc_dictionary_t *p_resp_headers,
                       int *done )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    char *psz_original = NULL;
    char *psz_line = NULL;
    char *psz_token;
    char *psz_next;
    char *psz_name;
    char *psz_value;
    int i_err = VLC_SUCCESS;
    psz_line = net_Gets( p_this, p_sys->i_control_fd, NULL );
    if ( !psz_line )
    {
        i_err = VLC_EGENERIC;
        goto error;
    }
    /* Empty line for response end */
    if ( psz_line[0] == '\0' )
        *done = 1;
    else
    {
        psz_original = strdup( psz_line );
        psz_next = psz_line;
        psz_token = strsep( &psz_next, psz_delim_colon );
        if ( !psz_token || psz_next[0] != ' ' )
        {
            msg_Err( p_this, "Invalid header format (%s)", psz_original );
            i_err = VLC_EGENERIC;
            goto error;
        }
        psz_name = psz_token;
        psz_value = psz_next + 1;
        vlc_dictionary_insert( p_resp_headers, psz_name, strdup( psz_value ) );
    }
error:
    free( psz_original );
    free( psz_line );
    return i_err;
}
