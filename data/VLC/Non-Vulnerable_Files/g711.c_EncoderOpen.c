};
static int EncoderOpen( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    if( p_enc->fmt_out.i_codec != VLC_CODEC_ALAW
     && p_enc->fmt_out.i_codec != VLC_CODEC_MULAW )
        return VLC_EGENERIC;
    p_enc->fmt_in.i_codec = VLC_CODEC_S16N;
    p_enc->fmt_in.audio.i_bitspersample = 16;
    p_enc->fmt_out.audio.i_bitspersample = 8;
    p_enc->fmt_out.i_bitrate = 8 * p_enc->fmt_in.audio.i_channels
                                 * p_enc->fmt_in.audio.i_rate;
    p_enc->pf_encode_audio = EncoderEncode;
    msg_Dbg( p_enc, "samplerate:%dHz channels:%d",
             p_enc->fmt_out.audio.i_rate, p_enc->fmt_out.audio.i_channels );
    return VLC_SUCCESS;
}
