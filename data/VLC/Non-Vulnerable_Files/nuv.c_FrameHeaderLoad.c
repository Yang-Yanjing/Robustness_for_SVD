 */
static int FrameHeaderLoad( demux_t *p_demux, frame_header_t *h )
{
    uint8_t* buffer = p_demux->p_sys->fh_buffer;
    if( stream_Read( p_demux->s, buffer, 12 ) != 12 )
        return VLC_EGENERIC;
    h->i_type = buffer[0];
    h->i_compression = buffer[1];
    h->i_keyframe = buffer[2];
    h->i_filters = buffer[3];
    h->i_timecode = GetDWLE( &buffer[4] );
    h->i_length = GetDWLE( &buffer[8] );
#if 0
    msg_Dbg( p_demux, "frame hdr: t=%c c=%c k=%d f=0x%x timecode=%d l=%d",
             h->i_type,
             h->i_compression ? h->i_compression : ' ',
             h->i_keyframe ? h->i_keyframe : ' ',
             h->i_filters,
             h->i_timecode, h->i_length );
#endif
    return VLC_SUCCESS;
}
