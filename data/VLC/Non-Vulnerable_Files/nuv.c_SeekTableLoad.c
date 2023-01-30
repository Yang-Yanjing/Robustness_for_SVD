*/
static int SeekTableLoad( demux_t *p_demux, demux_sys_t *p_sys )
{
    frame_header_t fh;
    int64_t i_original_pos;
    int64_t i_time, i_offset;
    int keyframe, last_keyframe = 0, frame = 0, kfa_entry_id = 0;
    if( p_sys->exh.i_seektable_offset <= 0 )
        return VLC_SUCCESS;
    /* Save current position */
    i_original_pos = stream_Tell( p_demux->s );
#if 0
    msg_Dbg( p_demux, "current offset %"PRIi64, i_original_pos );
    msg_Dbg( p_demux, "seeking in stream to %"PRIi64, p_sys->exh.i_seektable_offset );
#endif
    if( stream_Seek( p_demux->s, p_sys->exh.i_seektable_offset ) )
        return VLC_EGENERIC;
    if( FrameHeaderLoad( p_demux, &fh ) )
        return VLC_EGENERIC;
    if( fh.i_type != 'Q' )
    {
        msg_Warn( p_demux, "invalid seektable, frame type=%c", fh.i_type );
        stream_Seek( p_demux->s, i_original_pos );
        return VLC_EGENERIC;
    }
    /* */
    uint8_t *p_seek_table = malloc( fh.i_length );
    if( p_seek_table == NULL )
        return VLC_ENOMEM;
    if( stream_Read( p_demux->s, p_seek_table, fh.i_length ) != fh.i_length )
    {
        free( p_seek_table );
        return VLC_EGENERIC;
    }
    const int32_t i_seek_elements = fh.i_length / 12;
    /* Get keyframe adjust offsets */
    int32_t i_kfa_elements = 0;
    uint8_t *p_kfa_table = NULL;
    if( p_sys->exh.i_keyframe_adjust_offset > 0 )
    {
        msg_Dbg( p_demux, "seeking in stream to %"PRIi64, p_sys->exh.i_keyframe_adjust_offset );
        if( stream_Seek( p_demux->s, p_sys->exh.i_keyframe_adjust_offset ) )
        {
            free( p_seek_table );
            return VLC_EGENERIC;
        }
        if( FrameHeaderLoad( p_demux, &fh ) )
        {
            free( p_seek_table );
            return VLC_EGENERIC;
        }
        if( fh.i_type == 'K' && fh.i_length >= 8 )
        {
            p_kfa_table = malloc( fh.i_length );
            if( p_kfa_table == NULL )
            {
                free( p_seek_table );
                return VLC_ENOMEM;
            }
            if( stream_Read( p_demux->s, p_kfa_table, fh.i_length ) != fh.i_length )
            {
                free( p_seek_table );
                free( p_kfa_table );
                return VLC_EGENERIC;
            }
            i_kfa_elements = fh.i_length / 8;
        }
    }
    if( i_kfa_elements > 0 )
        msg_Warn( p_demux, "untested keyframe adjust support, upload samples" );
    for( int32_t j = 0; j < i_seek_elements; j++)
    {
#if 0
        uint8_t* p = p_seek_table + j * 12;
        msg_Dbg( p_demux, "%x %x %x %x %x %x %x %x %x %x %x %x",
        p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11]);
#endif
        keyframe = GetDWLE( p_seek_table + j * 12 + 8 );
        frame += (keyframe - last_keyframe) * p_sys->hdr.i_keyframe_distance;
        if( kfa_entry_id < i_kfa_elements && *(int32_t*)(p_kfa_table + kfa_entry_id * 12 + 4) == j )
        {
            frame -= *(int32_t*)(p_kfa_table + kfa_entry_id * 12);
            msg_Dbg( p_demux, "corrected keyframe %d with current frame number %d (corrected with %d)",
                        keyframe, frame, *(int32_t*)(p_kfa_table + kfa_entry_id * 12) );
            kfa_entry_id++;
        }
        i_time = (double)( (int64_t)frame * 1000000 ) / p_sys->hdr.d_fps;
        i_offset = GetQWLE( p_seek_table + j * 12 );
        if( i_offset == 0 && i_time != 0 )
            msg_Dbg( p_demux, "invalid file offset %d %"PRIi64, keyframe, i_offset );
        else
        {
            demux_IndexAppend( &p_sys->idx, i_time , i_offset );
#if 0
            msg_Dbg( p_demux, "adding entry position %d %"PRIi64 " file offset %"PRIi64, keyframe, i_time, i_offset );
#endif
        }
        last_keyframe = keyframe;
    }
    p_sys->i_total_frames = (int64_t)frame;
    p_sys->b_index = true;
    p_sys->i_total_length = p_sys->i_total_frames * 1000000 / p_sys->hdr.d_fps;
    msg_Dbg( p_demux, "index table loaded (%d elements)", i_seek_elements );
    if( i_kfa_elements )
        free ( p_kfa_table );
    free ( p_seek_table );
    /* Restore stream position */
    if( stream_Seek( p_demux->s, i_original_pos ) )
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
