 *****************************************************************************/
static int OpenEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys;
    /* We only support DVD LPCM yet. */
    if( p_enc->fmt_out.i_codec != VLC_CODEC_DVD_LPCM )
        return VLC_EGENERIC;
    if( p_enc->fmt_in.audio.i_rate != 48000 &&
        p_enc->fmt_in.audio.i_rate != 96000 &&
        p_enc->fmt_in.audio.i_rate != 44100 &&
        p_enc->fmt_in.audio.i_rate != 32000 )
    {
        msg_Err( p_enc, "DVD LPCM supports only sample rates of 48, 96, 44.1 or 32 kHz" );
        return VLC_EGENERIC;
    }
    if( p_enc->fmt_in.audio.i_channels > 8 )
    {
        msg_Err( p_enc, "DVD LPCM supports a maximum of eight channels" );
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the encoder's structure */
    if( ( p_enc->p_sys = p_sys =
          (encoder_sys_t *)malloc(sizeof(encoder_sys_t)) ) == NULL )
        return VLC_ENOMEM;
    /* In DVD LCPM, a frame is always 150 PTS ticks. */
    p_sys->i_frame_samples = p_enc->fmt_in.audio.i_rate * 150 / 90000;
    p_sys->p_buffer = xmalloc(p_sys->i_frame_samples
                            * p_enc->fmt_in.audio.i_channels * 16);
    p_sys->i_buffer_used = 0;
    p_sys->i_frame_num = 0;
    p_sys->i_channels = p_enc->fmt_in.audio.i_channels;
    p_sys->i_rate = p_enc->fmt_in.audio.i_rate;
    p_enc->pf_encode_audio = EncodeFrames;
    p_enc->fmt_in.i_codec = p_enc->fmt_out.i_codec;
    p_enc->fmt_in.audio.i_bitspersample = 16;
    p_enc->fmt_in.i_codec = VLC_CODEC_S16N;
    p_enc->fmt_out.i_bitrate =
        p_enc->fmt_in.audio.i_channels *
        p_enc->fmt_in.audio.i_rate *
        p_enc->fmt_in.audio.i_bitspersample *
        (p_sys->i_frame_samples + LPCM_VOB_HEADER_LEN) /
        p_sys->i_frame_samples;
    return VLC_SUCCESS;
}
