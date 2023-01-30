}
static int TsChangeRate( ts_thread_t *p_ts, int i_src_rate, int i_rate )
{
    int i_ret;
    vlc_mutex_lock( &p_ts->lock );
    p_ts->i_cmd_delay += p_ts->i_rate_delay;
    p_ts->i_rate_date = -1;
    p_ts->i_rate_delay = 0;
    p_ts->i_rate = i_rate;
    p_ts->i_rate_source = i_src_rate;
    i_ret = es_out_SetRate( p_ts->p_out, i_rate, i_rate );
    vlc_mutex_unlock( &p_ts->lock );
    return i_ret;
}
