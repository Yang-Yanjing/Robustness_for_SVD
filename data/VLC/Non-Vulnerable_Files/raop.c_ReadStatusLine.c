}
static int ReadStatusLine( vlc_object_t *p_this )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    char *psz_line = NULL;
    char *psz_token;
    char *psz_next;
    int i_result = VLC_EGENERIC;
    p_sys->psz_last_status_line = net_Gets( p_this, p_sys->i_control_fd,
                                            NULL );
    if ( !p_sys->psz_last_status_line )
        goto error;
    /* Create working copy */
    psz_line = strdup( p_sys->psz_last_status_line );
    psz_next = psz_line;
    /* Protocol field */
    psz_token = strsep( &psz_next, psz_delim_space );
    if ( !psz_token || strncmp( psz_token, "RTSP/1.", 7 ) != 0 )
    {
        msg_Err( p_this, "Unknown protocol (%s)",
                 p_sys->psz_last_status_line );
        goto error;
    }
    /* Status field */
    psz_token = strsep( &psz_next, psz_delim_space );
    if ( !psz_token )
    {
        msg_Err( p_this, "Request failed (%s)",
                 p_sys->psz_last_status_line );
        goto error;
    }
    i_result = atoi( psz_token );
error:
    free( psz_line );
    return i_result;
}
