}
static void EsOutDecodersStopBuffering( es_out_t *out, bool b_forced )
{
    es_out_sys_t *p_sys = out->p_sys;
    int i_ret;
    mtime_t i_stream_start;
    mtime_t i_system_start;
    mtime_t i_stream_duration;
    mtime_t i_system_duration;
    if (input_clock_GetState( p_sys->p_pgrm->p_clock,
                                  &i_stream_start, &i_system_start,
                                  &i_stream_duration, &i_system_duration ))
        return;
    mtime_t i_preroll_duration = 0;
    if( p_sys->i_preroll_end >= 0 )
        i_preroll_duration = __MAX( p_sys->i_preroll_end - i_stream_start, 0 );
    const mtime_t i_buffering_duration = p_sys->i_pts_delay +
                                         i_preroll_duration +
                                         p_sys->i_buffering_extra_stream - p_sys->i_buffering_extra_initial;
    if( i_stream_duration <= i_buffering_duration && !b_forced )
    {
        double f_level;
        if (i_buffering_duration == 0)
            f_level = 0;
        else
            f_level = __MAX( (double)i_stream_duration / i_buffering_duration, 0 );
        input_SendEventCache( p_sys->p_input, f_level );
        msg_Dbg( p_sys->p_input, "Buffering %d%%", (int)(100 * f_level) );
        return;
    }
    input_SendEventCache( p_sys->p_input, 1.0 );
    msg_Dbg( p_sys->p_input, "Stream buffering done (%d ms in %d ms)",
              (int)(i_stream_duration/1000), (int)(i_system_duration/1000) );
    p_sys->b_buffering = false;
    p_sys->i_preroll_end = -1;
    if( p_sys->i_buffering_extra_initial > 0 )
    {
        /* FIXME wrong ? */
        return;
    }
    const mtime_t i_decoder_buffering_start = mdate();
    for( int i = 0; i < p_sys->i_es; i++ )
    {
        es_out_id_t *p_es = p_sys->es[i];
        if( !p_es->p_dec || p_es->fmt.i_cat == SPU_ES )
            continue;
        input_DecoderWait( p_es->p_dec );
        if( p_es->p_dec_record )
            input_DecoderWait( p_es->p_dec_record );
    }
    msg_Dbg( p_sys->p_input, "Decoder wait done in %d ms",
              (int)(mdate() - i_decoder_buffering_start)/1000 );
    /* Here is a good place to destroy unused vout with every demuxer */
    input_resource_TerminateVout( p_sys->p_input->p->p_resource );
    /* */
    const mtime_t i_wakeup_delay = 10*1000; /* FIXME CLEANUP thread wake up time*/
    const mtime_t i_current_date = p_sys->b_paused ? p_sys->i_pause_date : mdate();
    input_clock_ChangeSystemOrigin( p_sys->p_pgrm->p_clock, true,
                                    i_current_date + i_wakeup_delay - i_buffering_duration );
    for( int i = 0; i < p_sys->i_es; i++ )
    {
        es_out_id_t *p_es = p_sys->es[i];
        if( !p_es->p_dec )
            continue;
        input_DecoderStopWait( p_es->p_dec );
        if( p_es->p_dec_record )
            input_DecoderStopWait( p_es->p_dec_record );
    }
}
