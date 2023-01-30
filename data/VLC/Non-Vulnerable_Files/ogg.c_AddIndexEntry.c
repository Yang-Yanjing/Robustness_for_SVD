}
static bool AddIndexEntry( sout_mux_t *p_mux, uint64_t i_time, sout_input_t *p_input )
{
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    ogg_stream_t *p_stream = (ogg_stream_t *) p_input->p_sys;
    uint64_t i_posdelta;
    uint64_t i_timedelta;
    if ( !p_sys->skeleton.b_create || p_mux->p_sys->skeleton.i_index_intvl == 0
         || !p_stream->skeleton.p_index )
        return false;
    if ( p_stream->skeleton.i_last_keyframe_pos == 0 )
        p_stream->skeleton.i_last_keyframe_pos = p_sys->i_segment_start;
    i_posdelta = p_sys->i_pos - p_stream->skeleton.i_last_keyframe_pos;
    i_timedelta = i_time - p_stream->skeleton.i_last_keyframe_time;
    if ( i_timedelta <= ( (uint64_t) p_mux->p_sys->skeleton.i_index_intvl * 1000 )
         || i_posdelta <= 0xFFFF )
        return false;
    /* do inserts */
    int i_ret;
    if ( !p_stream->skeleton.p_index ) return false;
    uint64_t i_offset = p_stream->skeleton.i_index_payload;
    i_ret = WriteQWVariableLE( i_posdelta, i_offset, p_stream->skeleton.p_index,
                               p_stream->skeleton.i_index_size );
    if ( i_ret == -1 ) return false;
    i_offset += i_ret;
    i_ret = WriteQWVariableLE( i_timedelta, i_offset, p_stream->skeleton.p_index,
                               p_stream->skeleton.i_index_size );
    if ( i_ret == -1 ) return false;
    p_stream->skeleton.i_index_payload = i_offset + i_ret;
    p_stream->skeleton.i_index_count++;
    /* update diff points */
    p_stream->skeleton.i_last_keyframe_pos = p_sys->i_pos;
    p_stream->skeleton.i_last_keyframe_time = i_time;
    msg_Dbg( p_mux, "Added index on stream %d entry %zd %"PRIu64,
             p_stream->i_serial_no, p_sys->i_pos - p_sys->i_segment_start, i_time );
    return true;
}
