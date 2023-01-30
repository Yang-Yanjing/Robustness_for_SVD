};
static int DecoderOpen( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    const int16_t *table;
    switch( p_dec->fmt_in.i_codec )
    {
        case VLC_CODEC_ALAW:
            table = alawtos16;
            break;
        case VLC_CODEC_MULAW:
            table = ulawtos16;
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
    msg_Dbg( p_dec, "samplerate:%dHz channels:%d",
             p_dec->fmt_in.audio.i_rate, p_dec->fmt_in.audio.i_channels );
    /* Allocate the memory needed to store the decoder's structure */
    decoder_sys_t *p_sys = malloc(sizeof(*p_sys));
    if( unlikely(p_sys == NULL) )
        return VLC_ENOMEM;
    /* Set output properties */
    p_dec->pf_decode_audio = DecodeBlock;
    p_dec->p_sys = p_sys;
    p_dec->fmt_out.i_cat = AUDIO_ES;
    p_dec->fmt_out.i_codec = VLC_CODEC_S16N;
    p_dec->fmt_out.audio.i_format = VLC_CODEC_S16N;
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
    p_sys->table = table;
    date_Init( &p_sys->end_date, p_dec->fmt_out.audio.i_rate, 1 );
    date_Set( &p_sys->end_date, 0 );
    return VLC_SUCCESS;
}
