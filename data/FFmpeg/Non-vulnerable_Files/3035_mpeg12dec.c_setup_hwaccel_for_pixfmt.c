static void setup_hwaccel_for_pixfmt(AVCodecContext *avctx)
{
    
    if (avctx->hwaccel
#if FF_API_VDPAU
        || uses_vdpau(avctx)
#endif
        )
        if (avctx->idct_algo == FF_IDCT_AUTO)
            avctx->idct_algo = FF_IDCT_SIMPLE;
    if (avctx->hwaccel && avctx->pix_fmt == AV_PIX_FMT_XVMC) {
        Mpeg1Context *s1 = avctx->priv_data;
        MpegEncContext *s = &s1->mpeg_enc_ctx;
        s->pack_pblocks = 1;
#if FF_API_XVMC
FF_DISABLE_DEPRECATION_WARNINGS
        avctx->xvmc_acceleration = 2;
FF_ENABLE_DEPRECATION_WARNINGS
#endif 
    }
}
