}
static int ControlSetPosition( demux_t *p_demux, int64_t i_pos, bool b_guess )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
    if( i_pos < 0 )
        return VLC_EGENERIC;
    /* if we can seek in the stream */
    if( p_sys->b_seekable && !b_guess )
    {
        if( stream_Seek( p_demux->s, i_pos ) )
            return VLC_EGENERIC;
    }
    else
    {
        /* forward seek */
        if( i_pos > stream_Tell( p_demux->s ) )
        {
            msg_Dbg( p_demux, "unable to seek, skipping frames (slow)" );
        }
        else
        {
            msg_Warn( p_demux, "unable to seek, only forward seeking is possible" );
            return VLC_EGENERIC;
        }
    }
    while( vlc_object_alive (p_demux) )
    {
        frame_header_t fh;
        int64_t i_tell;
        if( ( i_tell = stream_Tell( p_demux->s ) ) >= i_pos )
            break;
        if( FrameHeaderLoad( p_demux, &fh ) )
            return VLC_EGENERIC;
        if( fh.i_type == 'A' || fh.i_type == 'V' )
        {
            if( !fh.i_keyframe && !p_sys->b_index )
                demux_IndexAppend( &p_sys->idx,(int64_t)fh.i_timecode*1000, i_tell );
        }
        if( fh.i_type != 'R' && fh.i_length > 0 )
        {
            if( stream_Read( p_demux->s, NULL, fh.i_length ) != fh.i_length )
                return VLC_EGENERIC;
        }
    }
    return VLC_SUCCESS;
}
