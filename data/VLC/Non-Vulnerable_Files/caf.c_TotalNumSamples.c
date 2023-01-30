}
static uint64_t TotalNumSamples( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    if( !NeedsPacketTable( p_sys ))
    {
        return TotalNumFrames( p_demux ) * p_sys->fmt.audio.i_frame_length;
    }
    else
    {
        return p_sys->packet_table.i_num_valid_frames + p_sys->packet_table.i_num_priming_frames +
                p_sys->packet_table.i_num_remainder_frames;
    }
}
