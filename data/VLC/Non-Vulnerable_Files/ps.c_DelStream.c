 *****************************************************************************/
static int DelStream( sout_mux_t *p_mux, sout_input_t *p_input )
{
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    ps_stream_t *p_stream =(ps_stream_t*)p_input->p_sys;
    msg_Dbg( p_mux, "removing input" );
    switch( p_input->p_fmt->i_codec )
    {
        case VLC_CODEC_MPGV:
            StreamIdRelease( p_sys->stream_id_mpgv, 0xe0,
                             p_stream->i_stream_id );
            break;
        case VLC_CODEC_DVD_LPCM:
            StreamIdRelease( p_sys->stream_id_lpcm, 0xa0,
                             p_stream->i_stream_id&0xff );
            break;
        case VLC_CODEC_DTS:
            StreamIdRelease( p_sys->stream_id_dts, 0x88,
                             p_stream->i_stream_id&0xff );
            break;
        case VLC_CODEC_A52:
            StreamIdRelease( p_sys->stream_id_a52, 0x80,
                             p_stream->i_stream_id&0xff );
            break;
        case VLC_CODEC_MPGA:
            StreamIdRelease( p_sys->stream_id_mpga, 0xc0,
                             p_stream->i_stream_id  );
            break;
        case VLC_CODEC_SPU:
            StreamIdRelease( p_sys->stream_id_spu, 0x20,
                             p_stream->i_stream_id&0xff );
            break;
        default:
            /* Never reached */
            break;
    }
    if( p_input->p_fmt->i_cat == AUDIO_ES )
    {
        p_sys->i_audio_bound--;
    }
    else if( p_input->p_fmt->i_cat == VIDEO_ES )
    {
        p_sys->i_video_bound--;
    }
    /* Try to set a sensible default value for the instant bitrate */
    p_sys->i_instant_bitrate -= (p_input->p_fmt->i_bitrate + 1000);
    /* rate_bound is in units of 50 bytes/second */
    p_sys->i_rate_bound -= (p_input->p_fmt->i_bitrate * 2)/(8 * 50);
    p_sys->i_psm_version++;
    free( p_stream );
    return VLC_SUCCESS;
}
