 *****************************************************************************/
static int Open( vlc_object_t * p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    const uint8_t *p_peek;
    frame_header_t fh;
    /* Check id */
    if( stream_Peek( p_demux->s, &p_peek, 12 ) != 12 ||
        ( strncmp( (char *)p_peek, "MythTVVideo", 11 ) &&
          strncmp( (char *)p_peek, "NuppelVideo", 11 ) ) )
        return VLC_EGENERIC;
    p_sys = malloc( sizeof( demux_sys_t ) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    memset( p_sys, 0, sizeof( demux_sys_t ) );
    p_sys->p_es_video = NULL;
    p_sys->p_es_audio = NULL;
    p_sys->p_extra_f = NULL;
    p_sys->i_pcr = -1;
    p_sys->b_index = false;
    p_sys->i_total_frames = -1;
    p_sys->i_total_length = -1;
    demux_IndexInit( &p_sys->idx );
    p_demux->p_sys = p_sys;
    /* Info about the stream */
    stream_Control( p_demux->s, STREAM_CAN_SEEK, &p_sys->b_seekable );
#if 0
    if( p_sys->b_seekable )
        msg_Dbg( p_demux, "stream is seekable" );
    else
        msg_Dbg( p_demux, "stream is NOT seekable" );
#endif
    if( HeaderLoad( p_demux, &p_sys->hdr ) )
        goto error;
    /* Load 'D' */
    if( FrameHeaderLoad( p_demux, &fh ) || fh.i_type != 'D' )
        goto error;
    if( fh.i_length > 0 )
    {
        if( fh.i_compression == 'F' || fh.i_compression == 'R' )
        {
            /* libavcodec extra data */
            p_sys->i_extra_f = fh.i_length;
            p_sys->p_extra_f = malloc( fh.i_length );
            if( p_sys->p_extra_f == NULL || stream_Read( p_demux->s,
                             p_sys->p_extra_f, fh.i_length ) != fh.i_length )
                goto error;
        }
        else
        {
            msg_Warn( p_demux, "unsupported 'D' frame (c=%c)", fh.i_compression );
            if( stream_Read( p_demux->s, NULL, fh.i_length ) != fh.i_length )
                goto error;
        }
    }
    /* Check and load extented */
    if( stream_Peek( p_demux->s, &p_peek, 1 ) != 1 )
        goto error;
    if( p_peek[0] == 'X' )
    {
        if( FrameHeaderLoad( p_demux, &fh ) )
            goto error;
        if( fh.i_length != 512 )
            goto error;
        if( ExtendedHeaderLoad( p_demux, &p_sys->exh ) )
            goto error;
        if( !p_sys->b_seekable )
            msg_Warn( p_demux, "stream is not seekable, skipping seektable" );
        else if( SeekTableLoad( p_demux, p_sys ) )
        {
            p_sys->b_index = false;
            msg_Warn( p_demux, "Seektable is broken, seek won't be accurate" );
        }
    }
    else
    {
        /* XXX: for now only file with extended chunk are supported
         * why: because else we need to have support for rtjpeg+stupid nuv shit */
        msg_Err( p_demux, "VLC doesn't support NUV without extended chunks (please upload samples)" );
        goto error;
    }
    /* Create audio/video (will work only with extended header and audio=mp3 */
    if( p_sys->hdr.i_video_blocks != 0 )
    {
        es_format_t fmt;
        es_format_Init( &fmt, VIDEO_ES, p_sys->exh.i_video_fcc );
        fmt.video.i_width = p_sys->hdr.i_width;
        fmt.video.i_height = p_sys->hdr.i_height;
        fmt.i_extra = p_sys->i_extra_f;
        fmt.p_extra = p_sys->p_extra_f;
        fmt.video.i_sar_num = p_sys->hdr.d_aspect * fmt.video.i_height;
        fmt.video.i_sar_den = fmt.video.i_width;
        p_sys->p_es_video = es_out_Add( p_demux->out, &fmt );
    }
    if( p_sys->hdr.i_audio_blocks != 0 )
    {
        es_format_t fmt;
        es_format_Init( &fmt, AUDIO_ES, VLC_CODEC_MPGA );
        fmt.audio.i_rate = p_sys->exh.i_audio_sample_rate;
        fmt.audio.i_bitspersample = p_sys->exh.i_audio_bits_per_sample;
        p_sys->p_es_audio = es_out_Add( p_demux->out, &fmt );
    }
    if( p_sys->hdr.i_text_blocks != 0 )
    {
        msg_Warn( p_demux, "text not yet supported (upload samples)" );
    }
    p_sys->i_first_frame_offset = stream_Tell( p_demux->s );
    /* Fill p_demux fields */
    p_demux->pf_demux = Demux;
    p_demux->pf_control = Control;
    return VLC_SUCCESS;
error:
    msg_Warn( p_demux, "cannot load Nuv file" );
    p_demux->p_sys = NULL;
    free( p_sys );
    return VLC_EGENERIC;
}
