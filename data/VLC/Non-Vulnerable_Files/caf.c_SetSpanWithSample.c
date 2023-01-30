 potentially slow operation! */
static int SetSpanWithSample( demux_t *p_demux, frame_span_t *p_span, uint64_t i_target_sample )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    uint64_t i_num_frames = TotalNumFrames( p_demux );
    if( !NeedsPacketTable( p_sys ))
    {
        uint64_t i_frame = i_target_sample / p_sys->fmt.audio.i_frame_length;
        uint64_t i_remaining = i_target_sample - i_frame * p_sys->fmt.audio.i_frame_length;
        if( i_remaining > ( p_sys->fmt.audio.i_frame_length / 2 ))
            i_frame++;
        if( i_frame > i_num_frames )
            i_frame = i_num_frames;
        p_span->i_frames = i_frame;
        p_span->i_samples = i_frame * p_sys->fmt.audio.i_frame_length;
        p_span->i_bytes = i_frame * p_sys->fmt.audio.i_bytes_per_frame;
        p_span->i_desc_bytes = 0;
    }
    else
    {
        *p_span = (frame_span_t){0};
        frame_span_t prev_span;
        while( p_span->i_samples < i_target_sample && p_span->i_frames < i_num_frames )
        {
            prev_span = *p_span;
            if( FrameSpanAddDescription( p_demux, p_span->i_desc_bytes, p_span ))
                return VLC_EGENERIC;
            if( p_span->i_samples >= i_target_sample )
            {
                uint64_t i_this_samples = p_span->i_samples - prev_span.i_samples;
                if( i_target_sample - prev_span.i_samples < i_this_samples / 2 )
                    *p_span = prev_span;
                break;
            }
        }
    }
    return VLC_SUCCESS;
}
