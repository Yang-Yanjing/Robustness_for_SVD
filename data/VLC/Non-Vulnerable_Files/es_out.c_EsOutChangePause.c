}
static void EsOutChangePause( es_out_t *out, bool b_paused, mtime_t i_date )
{
    es_out_sys_t *p_sys = out->p_sys;
    /* XXX the order is important */
    if( b_paused )
    {
        EsOutDecodersChangePause( out, true, i_date );
        EsOutProgramChangePause( out, true, i_date );
    }
    else
    {
        if( p_sys->i_buffering_extra_initial > 0 )
        {
            mtime_t i_stream_start;
            mtime_t i_system_start;
            mtime_t i_stream_duration;
            mtime_t i_system_duration;
            int i_ret;
            i_ret = input_clock_GetState( p_sys->p_pgrm->p_clock,
                                          &i_stream_start, &i_system_start,
                                          &i_stream_duration, &i_system_duration );
            if( !i_ret )
            {
                /* FIXME pcr != exactly what wanted */
                const mtime_t i_used = /*(i_stream_duration - p_sys->p_input->p->i_pts_delay)*/ p_sys->i_buffering_extra_system - p_sys->i_buffering_extra_initial;
                i_date -= i_used;
            }
            p_sys->i_buffering_extra_initial = 0;
            p_sys->i_buffering_extra_stream = 0;
            p_sys->i_buffering_extra_system = 0;
        }
        EsOutProgramChangePause( out, false, i_date );
        EsOutDecodersChangePause( out, false, i_date );
        EsOutProgramsChangeRate( out );
    }
    p_sys->b_paused = b_paused;
    p_sys->i_pause_date = i_date;
}
