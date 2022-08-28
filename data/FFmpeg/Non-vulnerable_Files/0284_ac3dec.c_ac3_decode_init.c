static av_cold int ac3_decode_init(AVCodecContext *avctx)
{
    AC3DecodeContext *s = avctx->priv_data;
    int i;
    s->avctx = avctx;
    ff_ac3_common_init();
    ac3_tables_init();
    ff_mdct_init(&s->imdct_256, 8, 1, 1.0);
    ff_mdct_init(&s->imdct_512, 9, 1, 1.0);
    AC3_RENAME(ff_kbd_window_init)(s->window, 5.0, 256);
    ff_bswapdsp_init(&s->bdsp);
#if (USE_FIXED)
    s->fdsp = avpriv_alloc_fixed_dsp(avctx->flags & AV_CODEC_FLAG_BITEXACT);
#else
    s->fdsp = avpriv_float_dsp_alloc(avctx->flags & AV_CODEC_FLAG_BITEXACT);
    ff_fmt_convert_init(&s->fmt_conv, avctx);
#endif
    ff_ac3dsp_init(&s->ac3dsp, avctx->flags & AV_CODEC_FLAG_BITEXACT);
    av_lfg_init(&s->dith_state, 0);
    if (USE_FIXED)
        avctx->sample_fmt = AV_SAMPLE_FMT_S16P;
    else
        avctx->sample_fmt = AV_SAMPLE_FMT_FLTP;
    
#if FF_API_REQUEST_CHANNELS
FF_DISABLE_DEPRECATION_WARNINGS
    if (avctx->request_channels == 1)
        avctx->request_channel_layout = AV_CH_LAYOUT_MONO;
    else if (avctx->request_channels == 2)
        avctx->request_channel_layout = AV_CH_LAYOUT_STEREO;
FF_ENABLE_DEPRECATION_WARNINGS
#endif
    if (avctx->channels > 1 &&
        avctx->request_channel_layout == AV_CH_LAYOUT_MONO)
        avctx->channels = 1;
    else if (avctx->channels > 2 &&
             avctx->request_channel_layout == AV_CH_LAYOUT_STEREO)
        avctx->channels = 2;
    s->downmixed = 1;
    for (i = 0; i < AC3_MAX_CHANNELS; i++) {
        s->xcfptr[i] = s->transform_coeffs[i];
        s->dlyptr[i] = s->delay[i];
    }
    return 0;
}
