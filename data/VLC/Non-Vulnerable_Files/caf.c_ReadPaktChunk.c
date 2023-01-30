}
static int ReadPaktChunk( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    const uint8_t *p_peek;
    if ( stream_Peek( p_demux->s, &p_peek, 8 + 8 + 4 + 4 ) < ( 8 + 8 + 4 + 4 ))
    {
        msg_Err( p_demux, "Couldn't peek packet descriptions" );
        return VLC_EGENERIC;
    }
    if( ReadBEInt64ToUInt64( p_peek, &p_sys->packet_table.i_num_packets ))
    {
        msg_Err( p_demux, "Invalid packet table: i_num_packets is negative.");
        return VLC_EGENERIC;
    }
    if( ReadBEInt64ToUInt64( p_peek + 8, &p_sys->packet_table.i_num_valid_frames ))
    {
        msg_Err( p_demux, "Invalid packet table: i_num_valid_frames is negative.");
        return VLC_EGENERIC;
    }
    if( ReadBEInt32ToUInt32( p_peek + 16, &p_sys->packet_table.i_num_priming_frames ))
    {
        msg_Err( p_demux, "Invalid packet table: i_num_priming_frames is negative.");
        return VLC_EGENERIC;
    }
    if( ReadBEInt32ToUInt32( p_peek + 20, &p_sys->packet_table.i_num_remainder_frames ))
    {
        msg_Err( p_demux, "Invalid packet table: i_num_remainder_frames is negative.");
        return VLC_EGENERIC;
    }
    p_sys->packet_table.i_descriptions_start = stream_Tell( p_demux->s ) + 24;
    return VLC_SUCCESS;
}
