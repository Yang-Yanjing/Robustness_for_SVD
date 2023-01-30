}
static int64_t OggBackwardSeekToFrame( demux_t *p_demux, int64_t i_pos1, int64_t i_pos2,
                               logical_stream_t *p_stream, int64_t i_granulepos )
{
    int64_t i_result;
    int64_t i_offset = __MAX( 1 + ( (i_pos2 - i_pos1) >> 1 ), OGGSEEK_BYTES_TO_READ );
restart:
    OggDebug(
        msg_Dbg( p_demux, "Probing Back %"PRId64" %"PRId64" for granule %"PRId64,
        i_pos1, i_pos2, i_granulepos );
    );
    i_result = OggForwardSeekToFrame( p_demux, i_pos1, i_pos2, p_stream,
                                      i_granulepos, true );
    if ( i_result == SEGMENT_NOT_FOUND && i_pos1 > p_stream->i_data_start )
    {
        i_pos1 = __MAX( p_stream->i_data_start, i_pos1 - i_offset );
        goto restart;
    }
    return i_result;
}
