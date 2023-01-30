 */
static int EncoderOpen( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    void (*encode)(void *, const uint8_t *, unsigned) = NULL;
    switch( p_enc->fmt_out.i_codec )
    {
    case VLC_CODEC_S8:
        encode = S8Decode;
    case VLC_CODEC_U8:
        p_enc->fmt_in.i_codec = VLC_CODEC_U8;
        p_enc->fmt_out.audio.i_bitspersample = 8;
        break;
    case VLC_CODEC_U16I:
        encode = U16IEncode;
        p_enc->fmt_in.i_codec = VLC_CODEC_S16N;
        p_enc->fmt_out.audio.i_bitspersample = 16;
        break;
    case VLC_CODEC_U16N:
        encode = U16NEncode;
        p_enc->fmt_in.i_codec = VLC_CODEC_S16N;
        p_enc->fmt_out.audio.i_bitspersample = 16;
        break;
    case VLC_CODEC_S16I:
        encode = S16IDecode;
    case VLC_CODEC_S16N:
        p_enc->fmt_in.i_codec = VLC_CODEC_S16N;
        p_enc->fmt_out.audio.i_bitspersample = 16;
        break;
    case VLC_CODEC_U24B:
        encode = U24BEncode;
        p_enc->fmt_in.i_codec = VLC_CODEC_S32N;
        p_enc->fmt_out.audio.i_bitspersample = 24;
        break;
    case VLC_CODEC_U24L:
        encode = U24LEncode;
        p_enc->fmt_in.i_codec = VLC_CODEC_S32N;
        p_enc->fmt_out.audio.i_bitspersample = 24;
        break;
    case VLC_CODEC_S24B:
        encode = S24BEncode;
        p_enc->fmt_in.i_codec = VLC_CODEC_S32N;
        p_enc->fmt_out.audio.i_bitspersample = 24;
        break;
    case VLC_CODEC_S24L:
        encode = S24LEncode;
        p_enc->fmt_in.i_codec = VLC_CODEC_S32N;
        p_enc->fmt_out.audio.i_bitspersample = 24;
        break;
    case VLC_CODEC_U32I:
        encode = U32IEncode;
        p_enc->fmt_in.i_codec = VLC_CODEC_S32N;
        p_enc->fmt_out.audio.i_bitspersample = 32;
        break;
    case VLC_CODEC_U32N:
        encode = U32NEncode;
        p_enc->fmt_in.i_codec = VLC_CODEC_S32N;
        p_enc->fmt_out.audio.i_bitspersample = 32;
        break;
    case VLC_CODEC_S32I:
        encode = S32IEncode;
    case VLC_CODEC_S32N:
        p_enc->fmt_in.i_codec = VLC_CODEC_S32N;
        p_enc->fmt_out.audio.i_bitspersample = 32;
        break;
#ifdef WORDS_BIGENDIAN
    case VLC_CODEC_F32L:
#else
    case VLC_CODEC_F32B:
#endif
        encode = F32IEncode;
    case VLC_CODEC_FL32:
        p_enc->fmt_in.i_codec = VLC_CODEC_FL32;
        p_enc->fmt_out.audio.i_bitspersample = 32;
        break;
#ifdef WORDS_BIGENDIAN
    case VLC_CODEC_F64L:
#else
    case VLC_CODEC_F64B:
#endif
        encode = F64IEncode;
    case VLC_CODEC_FL64:
        p_enc->fmt_in.i_codec = VLC_CODEC_FL64;
        p_enc->fmt_out.audio.i_bitspersample = 64;
        break;
    default:
        return VLC_EGENERIC;
    }
    p_enc->p_sys = (void *)encode;
    p_enc->pf_encode_audio = Encode;
    p_enc->fmt_out.audio.i_bytes_per_frame =
        (p_enc->fmt_out.audio.i_bitspersample / 8) *
        p_enc->fmt_in.audio.i_channels;
    p_enc->fmt_out.i_bitrate =
        p_enc->fmt_in.audio.i_channels *
        p_enc->fmt_in.audio.i_rate *
        p_enc->fmt_out.audio.i_bitspersample;
    msg_Dbg( p_enc, "samplerate:%dHz channels:%d bits/sample:%d",
             p_enc->fmt_out.audio.i_rate, p_enc->fmt_out.audio.i_channels,
             p_enc->fmt_out.audio.i_bitspersample );
    return VLC_SUCCESS;
}
