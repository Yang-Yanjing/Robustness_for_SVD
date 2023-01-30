}
static uint64_t TotalNumFrames( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    if( !NeedsPacketTable( p_sys ))
    {
        uint64_t i_data_size;
        if( p_sys->i_data_size != kCHUNK_SIZE_EOF)
        {
            i_data_size = p_sys->i_data_size;
        }
        else
        {
            int64_t i_stream_size = stream_Size( p_demux->s );
            if(i_stream_size >= 0 && (uint64_t)i_stream_size >= p_sys->i_data_offset)
                i_data_size = i_stream_size - p_sys->i_data_offset;
            else
                i_data_size = 0;
        }
        return i_data_size / p_sys->fmt.audio.i_bytes_per_frame;
    }
    else
    {
        return p_sys->packet_table.i_num_packets;
    }
}
