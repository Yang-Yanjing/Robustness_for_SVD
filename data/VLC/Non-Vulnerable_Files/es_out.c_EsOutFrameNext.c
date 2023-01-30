}
static void EsOutFrameNext( es_out_t *out )
{
    es_out_sys_t *p_sys = out->p_sys;
    es_out_id_t *p_es_video = NULL;
    if( p_sys->b_buffering )
    {
        msg_Warn( p_sys->p_input, "buffering, ignoring 'frame next'" );
        return;
    }
    assert( p_sys->b_paused );
    for( int i = 0; i < p_sys->i_es; i++ )
    {
        es_out_id_t *p_es = p_sys->es[i];
        if( p_es->fmt.i_cat == VIDEO_ES && p_es->p_dec )
        {
            p_es_video = p_es;
            break;
        }
    }
    if( !p_es_video )
    {
        msg_Warn( p_sys->p_input, "No video track selected, ignoring 'frame next'" );
        return;
    }
    mtime_t i_duration;
    input_DecoderFrameNext( p_es_video->p_dec, &i_duration );
    msg_Dbg( out->p_sys->p_input, "EsOutFrameNext consummed %d ms", (int)(i_duration/1000) );
    if( i_duration <= 0 )
        i_duration = 40*1000;
    /* FIXME it is not a clean way ? */
    if( p_sys->i_buffering_extra_initial <= 0 )
    {
        mtime_t i_stream_start;
        mtime_t i_system_start;
        mtime_t i_stream_duration;
        mtime_t i_system_duration;
        int i_ret;
        i_ret = input_clock_GetState( p_sys->p_pgrm->p_clock,
                                      &i_stream_start, &i_system_start,
                                      &i_stream_duration, &i_system_duration );
        if( i_ret )
            return;
        p_sys->i_buffering_extra_initial = 1 + i_stream_duration - p_sys->i_pts_delay; /* FIXME < 0 ? */
        p_sys->i_buffering_extra_system =
        p_sys->i_buffering_extra_stream = p_sys->i_buffering_extra_initial;
    }
    const int i_rate = input_clock_GetRate( p_sys->p_pgrm->p_clock );
    p_sys->b_buffering = true;
    p_sys->i_buffering_extra_system += i_duration;
    p_sys->i_buffering_extra_stream = p_sys->i_buffering_extra_initial +
                                      ( p_sys->i_buffering_extra_system - p_sys->i_buffering_extra_initial ) *
                                                INPUT_RATE_DEFAULT / i_rate;
    p_sys->i_preroll_end = -1;
}
