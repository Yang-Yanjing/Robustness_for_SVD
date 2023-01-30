}
static int TsChangePause( ts_thread_t *p_ts, bool b_source_paused, bool b_paused, mtime_t i_date )
{
    vlc_mutex_lock( &p_ts->lock );
    int i_ret;
    if( b_paused )
    {
        assert( !b_source_paused );
        i_ret = es_out_SetPauseState( p_ts->p_out, true, true, i_date );
    }
    else
    {
        i_ret = es_out_SetPauseState( p_ts->p_out, false, false, i_date );
    }
    if( !i_ret )
    {
        if( !b_paused )
        {
            assert( p_ts->i_pause_date > 0 );
            p_ts->i_cmd_delay += i_date - p_ts->i_pause_date;
        }
        p_ts->b_paused = b_paused;
        p_ts->i_pause_date = i_date;
        vlc_cond_signal( &p_ts->wait );
    }
    vlc_mutex_unlock( &p_ts->lock );
    return i_ret;
}
