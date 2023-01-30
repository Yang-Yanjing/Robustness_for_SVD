}
static es_out_id_t *Add( es_out_t *p_out, const es_format_t *p_fmt )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    ts_cmd_t cmd;
    es_out_id_t *p_es = malloc( sizeof( *p_es ) );
    if( !p_es )
        return NULL;
    vlc_mutex_lock( &p_sys->lock );
    TsAutoStop( p_out );
    if( CmdInitAdd( &cmd, p_es, p_fmt, p_sys->b_delayed ) )
    {
        vlc_mutex_unlock( &p_sys->lock );
        free( p_es );
        return NULL;
    }
    TAB_APPEND( p_sys->i_es, p_sys->pp_es, p_es );
    if( p_sys->b_delayed )
        TsPushCmd( p_sys->p_ts, &cmd );
    else
        CmdExecuteAdd( p_sys->p_out, &cmd );
    vlc_mutex_unlock( &p_sys->lock );
    return p_es;
}
