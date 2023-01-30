#ifdef ENABLE_SOUT
static int InitSout( input_thread_t * p_input )
{
    if( p_input->b_preparsing )
        return VLC_SUCCESS;
    /* Find a usable sout and attach it to p_input */
    char *psz = var_GetNonEmptyString( p_input, "sout" );
    if( psz && strncasecmp( p_input->p->p_item->psz_uri, "vlc:", 4 ) )
    {
        p_input->p->p_sout  = input_resource_RequestSout( p_input->p->p_resource, NULL, psz );
        if( !p_input->p->p_sout )
        {
            input_ChangeState( p_input, ERROR_S );
            msg_Err( p_input, "cannot start stream output instance, " \
                              "aborting" );
            free( psz );
            return VLC_EGENERIC;
        }
        if( libvlc_stats( p_input ) )
        {
            INIT_COUNTER( sout_sent_packets, COUNTER );
            INIT_COUNTER( sout_sent_bytes, COUNTER );
            INIT_COUNTER( sout_send_bitrate, DERIVATIVE );
        }
    }
    else
    {
        input_resource_RequestSout( p_input->p->p_resource, NULL, NULL );
    }
    free( psz );
    return VLC_SUCCESS;
}
