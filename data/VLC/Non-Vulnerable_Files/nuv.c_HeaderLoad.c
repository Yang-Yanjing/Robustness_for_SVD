 */
static int HeaderLoad( demux_t *p_demux, header_t *h )
{
    uint8_t buffer[72];
    if( stream_Read( p_demux->s, buffer, 72 ) != 72 )
        return VLC_EGENERIC;
    /* XXX: they are alignment to take care of (another broken format) */
    memcpy( h->id,      &buffer[ 0], 12 );
    memcpy( h->version, &buffer[12], 5 );
    h->i_width = GetDWLE( &buffer[20] );
    h->i_height = GetDWLE( &buffer[24] );
    h->i_width_desired = GetDWLE( &buffer[28] );
    h->i_height_desired = GetDWLE( &buffer[32] );
    h->i_mode = buffer[36];
    GetDoubleLE( &h->d_aspect, &buffer[40] );
    GetDoubleLE( &h->d_fps, &buffer[48] );
    h->i_video_blocks = GetDWLE( &buffer[56] );
    h->i_audio_blocks = GetDWLE( &buffer[60] );
    h->i_text_blocks = GetDWLE( &buffer[64] );
    h->i_keyframe_distance = GetDWLE( &buffer[68] );
#if 0
    msg_Dbg( p_demux, "nuv: h=%s v=%s %dx%d a=%f fps=%f v=%d a=%d t=%d kfd=%d",
             h->id, h->version, h->i_width, h->i_height, h->d_aspect,
             h->d_fps, h->i_video_blocks, h->i_audio_blocks, h->i_text_blocks,
             h->i_keyframe_distance );
#endif
    return VLC_SUCCESS;
}
