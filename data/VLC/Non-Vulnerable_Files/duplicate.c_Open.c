 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_stream_t     *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys;
    config_chain_t        *p_cfg;
    msg_Dbg( p_stream, "creating 'duplicate'" );
    p_sys = malloc( sizeof( sout_stream_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    TAB_INIT( p_sys->i_nb_streams, p_sys->pp_streams );
    TAB_INIT( p_sys->i_nb_last_streams, p_sys->pp_last_streams );
    TAB_INIT( p_sys->i_nb_select, p_sys->ppsz_select );
    for( p_cfg = p_stream->p_cfg; p_cfg != NULL; p_cfg = p_cfg->p_next )
    {
        if( !strncmp( p_cfg->psz_name, "dst", strlen( "dst" ) ) )
        {
            sout_stream_t *s, *p_last;
            msg_Dbg( p_stream, " * adding `%s'", p_cfg->psz_value );
            s = sout_StreamChainNew( p_stream->p_sout, p_cfg->psz_value,
                p_stream->p_next, &p_last );
            if( s )
            {
                TAB_APPEND( p_sys->i_nb_streams, p_sys->pp_streams, s );
                TAB_APPEND( p_sys->i_nb_last_streams, p_sys->pp_last_streams,
                    p_last );
                TAB_APPEND( p_sys->i_nb_select,  p_sys->ppsz_select, NULL );
            }
        }
        else if( !strncmp( p_cfg->psz_name, "select", strlen( "select" ) ) )
        {
            char *psz = p_cfg->psz_value;
            if( p_sys->i_nb_select > 0 && psz && *psz )
            {
                char **ppsz_select = &p_sys->ppsz_select[p_sys->i_nb_select - 1];
                if( *ppsz_select )
                {
                    msg_Err( p_stream, " * ignore selection `%s' (it already has `%s')",
                             psz, *ppsz_select );
                }
                else
                {
                    msg_Dbg( p_stream, " * apply selection `%s'", psz );
                    *ppsz_select = strdup( psz );
                }
            }
        }
        else
        {
            msg_Err( p_stream, " * ignore unknown option `%s'", p_cfg->psz_name );
        }
    }
    if( p_sys->i_nb_streams == 0 )
    {
        msg_Err( p_stream, "no destination given" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    p_stream->pf_add    = Add;
    p_stream->pf_del    = Del;
    p_stream->pf_send   = Send;
    p_stream->p_sys     = p_sys;
    return VLC_SUCCESS;
}
