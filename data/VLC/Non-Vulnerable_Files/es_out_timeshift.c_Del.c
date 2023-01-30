}
static void Del( es_out_t *p_out, es_out_id_t *p_es )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    ts_cmd_t cmd;
    vlc_mutex_lock( &p_sys->lock );
    TsAutoStop( p_out );
    CmdInitDel( &cmd, p_es );
    if( p_sys->b_delayed )
        TsPushCmd( p_sys->p_ts, &cmd );
    else
        CmdExecuteDel( p_sys->p_out, &cmd );
    TAB_REMOVE( p_sys->i_es, p_sys->pp_es, p_es );
    vlc_mutex_unlock( &p_sys->lock );
}
