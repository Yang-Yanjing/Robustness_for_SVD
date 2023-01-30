}
static int TsStart( es_out_t *p_out )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    ts_thread_t *p_ts;
    assert( !p_sys->b_delayed );
    p_sys->p_ts = p_ts = calloc(1, sizeof(*p_ts));
    if( !p_ts )
        return VLC_EGENERIC;
    p_ts->i_tmp_size_max = p_sys->i_tmp_size_max;
    p_ts->psz_tmp_path = p_sys->psz_tmp_path;
    p_ts->p_input = p_sys->p_input;
    p_ts->p_out = p_sys->p_out;
    vlc_mutex_init( &p_ts->lock );
    vlc_cond_init( &p_ts->wait );
    p_ts->b_paused = p_sys->b_input_paused && !p_sys->b_input_paused_source;
    p_ts->i_pause_date = p_ts->b_paused ? mdate() : -1;
    p_ts->i_rate_source = p_sys->i_input_rate_source;
    p_ts->i_rate        = p_sys->i_input_rate;
    p_ts->i_rate_date = -1;
    p_ts->i_rate_delay = 0;
    p_ts->i_buffering_delay = 0;
    p_ts->i_cmd_delay = 0;
    p_ts->p_storage_r = NULL;
    p_ts->p_storage_w = NULL;
    p_sys->b_delayed = true;
    if( vlc_clone( &p_ts->thread, TsRun, p_ts, VLC_THREAD_PRIORITY_INPUT ) )
    {
        msg_Err( p_sys->p_input, "cannot create timeshift thread" );
        TsDestroy( p_ts );
        p_sys->b_delayed = false;
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
