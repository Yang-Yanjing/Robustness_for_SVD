}
static int Open(vlc_object_t *p_this)
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if (p_dec->fmt_in.i_codec != VLC_CODEC_FLAC)
        return VLC_EGENERIC;
    /* */
    p_dec->p_sys = p_sys = malloc(sizeof(*p_sys));
    if (!p_sys)
        return VLC_ENOMEM;
    p_sys->i_state       = STATE_NOSYNC;
    p_sys->b_stream_info = false;
    p_sys->i_pts         = VLC_TS_INVALID;
    block_BytestreamInit(&p_sys->bytestream);
    /* */
    es_format_Copy(&p_dec->fmt_out, &p_dec->fmt_in);
    p_dec->fmt_out.i_cat   = AUDIO_ES;
    p_dec->fmt_out.i_codec = VLC_CODEC_FLAC;
    /* */
    p_dec->pf_decode_audio = NULL;
    p_dec->pf_packetize    = Packetize;
    return VLC_SUCCESS;
}
