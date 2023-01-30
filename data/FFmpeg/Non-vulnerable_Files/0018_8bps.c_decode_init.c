static av_cold int decode_init(AVCodecContext *avctx)
{
    EightBpsContext * const c = avctx->priv_data;
    c->avctx       = avctx;
    switch (avctx->bits_per_coded_sample) {
    case 8:
        avctx->pix_fmt = AV_PIX_FMT_PAL8;
        c->planes      = 1;
        c->planemap[0] = 0; 
        break;
    case 24:
        avctx->pix_fmt = ff_get_format(avctx, pixfmt_rgb24);
        c->planes      = 3;
        c->planemap[0] = 2; 
        c->planemap[1] = 1; 
        c->planemap[2] = 0; 
        break;
    case 32:
        avctx->pix_fmt = AV_PIX_FMT_RGB32;
        c->planes      = 4;
        
        break;
    default:
        av_log(avctx, AV_LOG_ERROR, "Error: Unsupported color depth: %u.\n",
               avctx->bits_per_coded_sample);
        return AVERROR_INVALIDDATA;
    }
    if (avctx->pix_fmt == AV_PIX_FMT_RGB32) {
        c->planemap[0] = HAVE_BIGENDIAN ? 1 : 2; 
        c->planemap[1] = HAVE_BIGENDIAN ? 2 : 1; 
        c->planemap[2] = HAVE_BIGENDIAN ? 3 : 0; 
        c->planemap[3] = HAVE_BIGENDIAN ? 0 : 3; 
    }
    return 0;
}
