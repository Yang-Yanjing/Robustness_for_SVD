av_cold int ff_dct_encode_init(MpegEncContext *s) {
    if (ARCH_X86)
        ff_dct_encode_init_x86(s);
    if (CONFIG_H263_ENCODER)
        ff_h263dsp_init(&s->h263dsp);
    if (!s->dct_quantize)
        s->dct_quantize = ff_dct_quantize_c;
    if (!s->denoise_dct)
        s->denoise_dct  = denoise_dct_c;
    s->fast_dct_quantize = s->dct_quantize;
    if (s->avctx->trellis)
        s->dct_quantize  = dct_quantize_trellis_c;
    return 0;
}
