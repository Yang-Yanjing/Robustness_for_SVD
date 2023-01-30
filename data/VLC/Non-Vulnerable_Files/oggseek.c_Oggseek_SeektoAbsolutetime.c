}
int Oggseek_SeektoAbsolutetime( demux_t *p_demux, logical_stream_t *p_stream,
                                int64_t i_time )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
    OggDebug( msg_Dbg( p_demux, "=================== Seeking To Absolute Time %"PRId64, i_time ) );
    int64_t i_offset_lower = -1;
    int64_t i_offset_upper = -1;
    if ( Ogg_GetBoundsUsingSkeletonIndex( p_stream, i_time, &i_offset_lower, &i_offset_upper ) )
    {
        /* Exact match */
        OggDebug( msg_Dbg( p_demux, "Found keyframe at %"PRId64" using skeleton index", i_offset_lower ) );
        if ( i_offset_lower == -1 ) i_offset_lower = p_stream->i_data_start;
        p_sys->i_input_position = i_offset_lower;
        seek_byte( p_demux, p_sys->i_input_position );
        ogg_stream_reset( &p_stream->os );
        return i_offset_lower;
    }
    OggDebug( msg_Dbg( p_demux, "Search bounds set to %"PRId64" %"PRId64" using skeleton index", i_offset_lower, i_offset_upper ) );
    OggNoDebug(
        OggSeekIndexFind( p_stream, i_time, &i_offset_lower, &i_offset_upper )
    );
    i_offset_lower = __MAX( i_offset_lower, p_stream->i_data_start );
    i_offset_upper = __MIN( i_offset_upper, p_sys->i_total_length );
    int64_t i_pagepos = OggBisectSearchByTime( p_demux, p_stream, i_time,
                                       i_offset_lower, i_offset_upper);
    if ( i_pagepos >= 0 )
    {
        /* be sure to clear any state or read+pagein() will fail on same # */
        ogg_stream_reset( &p_stream->os );
        seek_byte( p_demux, p_sys->i_input_position );
    }
    /* Insert keyframe position into index */
    OggNoDebug(
    if ( i_pagepos >= p_stream->i_data_start )
        OggSeek_IndexAdd( p_stream, i_time, i_pagepos )
    );
    OggDebug( msg_Dbg( p_demux, "=================== Seeked To %"PRId64" time %"PRId64, i_pagepos, i_time ) );
    return i_pagepos;
}
