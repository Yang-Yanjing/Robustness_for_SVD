}
static int Send( es_out_t *p_out, es_out_id_t *p_es, block_t *p_block )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    ts_cmd_t cmd;
    int i_ret = VLC_SUCCESS;
    vlc_mutex_lock( &p_sys->lock );
    TsAutoStop( p_out );
    CmdInitSend( &cmd, p_es, p_block );
    if( p_sys->b_delayed )
        TsPushCmd( p_sys->p_ts, &cmd );
    else
        i_ret = CmdExecuteSend( p_sys->p_out, &cmd) ;
    vlc_mutex_unlock( &p_sys->lock );
    return i_ret;
}
