 *****************************************************************************/
static int DecoderOpen( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    vlc_fourcc_t format = p_dec->fmt_in.i_codec;
    switch( p_dec->fmt_in.i_codec )
    {
    case VLC_FOURCC('a','r','a','w'):
    case VLC_FOURCC('a','f','l','t'):
    /* _signed_ big endian samples (mov) */
    case VLC_FOURCC('t','w','o','s'):
    /* _signed_ little endian samples (mov) */
    case VLC_FOURCC('s','o','w','t'):
        format =
            vlc_fourcc_GetCodecAudio( p_dec->fmt_in.i_codec,
                                      p_dec->fmt_in.audio.i_bitspersample );
        if( !format )
        {
            msg_Err( p_dec, "bad parameters(bits/sample)" );
            return VLC_EGENERIC;
        }
        break;
    }
    void (*decode) (void *, const uint8_t *, unsigned) = NULL;
    uint_fast8_t bits;
    switch( format )
    {
#ifdef WORDS_BIGENDIAN
    case VLC_CODEC_F64L:
#else
    case VLC_CODEC_F64B:
#endif
        format = VLC_CODEC_FL64;
        decode = F64IDecode;
        bits = 64;
        break;
    case VLC_CODEC_FL64:
        decode = F64NDecode;
        bits = 64;
        break;
#ifdef WORDS_BIGENDIAN
    case VLC_CODEC_F32L:
#else
    case VLC_CODEC_F32B:
#endif
        format = VLC_CODEC_FL32;
        decode = F32IDecode;
        bits = 32;
        break;
    case VLC_CODEC_FL32:
        decode = F32NDecode;
        bits = 32;
        break;
    case VLC_CODEC_U32B:
        format = VLC_CODEC_S32N;
        decode = U32BDecode;
        bits = 32;
        break;
    case VLC_CODEC_U32L:
        format = VLC_CODEC_S32N;
        decode = U32LDecode;
        bits = 32;
        break;
    case VLC_CODEC_S32I:
        format = VLC_CODEC_S32N;
        decode = S32IDecode;
    case VLC_CODEC_S32N:
        bits = 32;
        break;
    case VLC_CODEC_S24B32:
        format = VLC_CODEC_S32N;
        decode = S24B32Decode;
        bits = 32;
        break;
    case VLC_CODEC_S24L32:
        format = VLC_CODEC_S32N;
        decode = S24L32Decode;
        bits = 32;
        break;
    case VLC_CODEC_U24B:
        format = VLC_CODEC_S32N;
        decode = U24BDecode;
        bits = 24;
        break;
    case VLC_CODEC_U24L:
        format = VLC_CODEC_S32N;
        decode = U24LDecode;
        bits = 24;
        break;
    case VLC_CODEC_S24B:
        format = VLC_CODEC_S32N;
        decode = S24BDecode;
        bits = 24;
        break;
    case VLC_CODEC_S24L:
        format = VLC_CODEC_S32N;
        decode = S24LDecode;
        bits = 24;
        break;
    case VLC_CODEC_S20B:
        format = VLC_CODEC_S32N;
        decode = S20BDecode;
        bits = 20;
        break;
    case VLC_CODEC_U16B:
        format = VLC_CODEC_S16N;
        decode = U16BDecode;
        bits = 16;
        break;
    case VLC_CODEC_U16L:
        format = VLC_CODEC_S16N;
        decode = U16LDecode;
        bits = 16;
        break;
    case VLC_CODEC_S16I:
        format = VLC_CODEC_S16N;
        decode = S16IDecode;
    case VLC_CODEC_S16N:
        bits = 16;
        break;
    case VLC_CODEC_DAT12:
        format = VLC_CODEC_S16N;
        decode = DAT12Decode;
        bits = 12;
        break;
    case VLC_CODEC_S8:
        decode = S8Decode;
        format = VLC_CODEC_U8;
    case VLC_CODEC_U8:
        bits = 8;
        break;
    default:
        return VLC_EGENERIC;
    }
    if( p_dec->fmt_in.audio.i_channels <= 0 ||
        p_dec->fmt_in.audio.i_channels > AOUT_CHAN_MAX )
    {
        msg_Err( p_dec, "bad channels count (1-9): %i",
                 p_dec->fmt_in.audio.i_channels );
        return VLC_EGENERIC;
    }
    if( p_dec->fmt_in.audio.i_rate <= 0 )
    {
        msg_Err( p_dec, "bad samplerate: %d Hz", p_dec->fmt_in.audio.i_rate );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_dec, "samplerate:%dHz channels:%d bits/sample:%d",
             p_dec->fmt_in.audio.i_rate, p_dec->fmt_in.audio.i_channels,
             p_dec->fmt_in.audio.i_bitspersample );
    /* Allocate the memory needed to store the decoder's structure */
    decoder_sys_t *p_sys = malloc(sizeof(*p_sys));
    if( unlikely(p_sys == NULL) )
        return VLC_ENOMEM;
    /* Set output properties */
    p_dec->fmt_out.i_cat = AUDIO_ES;
    p_dec->fmt_out.i_codec = format;
    p_dec->fmt_out.audio.i_format = format;
    p_dec->fmt_out.audio.i_rate = p_dec->fmt_in.audio.i_rate;
    if( p_dec->fmt_in.audio.i_physical_channels )
        p_dec->fmt_out.audio.i_physical_channels =
                                       p_dec->fmt_in.audio.i_physical_channels;
    else
        p_dec->fmt_out.audio.i_physical_channels =
                              pi_channels_maps[p_dec->fmt_in.audio.i_channels];
    if( p_dec->fmt_in.audio.i_original_channels )
        p_dec->fmt_out.audio.i_original_channels =
                                       p_dec->fmt_in.audio.i_original_channels;
    else
        p_dec->fmt_out.audio.i_original_channels =
                                      p_dec->fmt_out.audio.i_physical_channels;
    aout_FormatPrepare( &p_dec->fmt_out.audio );
    p_sys->decode = decode;
    p_sys->framebits = bits * p_dec->fmt_out.audio.i_channels;
    assert( p_sys->framebits );
    date_Init( &p_sys->end_date, p_dec->fmt_out.audio.i_rate, 1 );
    date_Set( &p_sys->end_date, 0 );
    p_dec->pf_decode_audio = DecodeBlock;
    p_dec->p_sys = p_sys;
    return VLC_SUCCESS;
}
