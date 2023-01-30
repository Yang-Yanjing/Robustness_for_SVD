}
static mtime_t EsOutGetBuffering( es_out_t *out )
{
    es_out_sys_t *p_sys = out->p_sys;
    if( !p_sys->p_pgrm )
        return 0;
    int i_ret;
    mtime_t i_stream_start;
    mtime_t i_system_start;
    mtime_t i_stream_duration;
    mtime_t i_system_duration;
    i_ret = input_clock_GetState( p_sys->p_pgrm->p_clock,
                                  &i_stream_start, &i_system_start,
                                  &i_stream_duration, &i_system_duration );
    if( i_ret )
        return 0;
    mtime_t i_delay;
    if( p_sys->b_buffering && p_sys->i_buffering_extra_initial <= 0 )
    {
        i_delay = i_stream_duration;
    }
    else
    {
        mtime_t i_system_duration;
        if( p_sys->b_paused )
        {
            i_system_duration = p_sys->i_pause_date  - i_system_start;
            if( p_sys->i_buffering_extra_initial > 0 )
                i_system_duration += p_sys->i_buffering_extra_system - p_sys->i_buffering_extra_initial;
        }
        else
        {
            i_system_duration = mdate() - i_system_start;
        }
        const mtime_t i_consumed = i_system_duration * INPUT_RATE_DEFAULT / p_sys->i_rate - i_stream_duration;
        i_delay = p_sys->i_pts_delay - i_consumed;
    }
    if( i_delay < 0 )
        return 0;
    return i_delay;
}
